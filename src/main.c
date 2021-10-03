#include <stdio.h>
#include <mosquitto.h>
#include <sys/syslog.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"
#include "mqtt.h"
#include "queue.h"
#include "handlers.h"

volatile sig_atomic_t deamonize = 1;

void term_proc(int sigterm) {
	deamonize = 0;
}



int main(int argc, char *argv[]) {

    char *config_path;
    struct mqtt_ftp_config *config = calloc(1 ,sizeof(struct mqtt_ftp_config));
    struct mosquitto *mosq;

    // init log
    openlog("mqtt-ftp", LOG_PERROR, 0);

    print_help_msg(argc, argv); // print help if needed

    // read conf
    config_path = get_config_path(argc, argv);
    read_mqtt_ftp_config_from_file(config_path, config);
    config->messages = create_q_req(); // initialize queue

    syslog(LOG_INFO, "SCOPE: main");
    debug_print_mqtt_config(config);

    // init mosquitto
    mosq_full_init(&mosq, config);
    mosquitto_loop_start(mosq);

    //mosquitto_publish(mosq, NULL, "test/test", strlen("TEST"), "TEST", 2, false);
    //syslog(LOG_INFO, "fuck");

    // daemon init
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);


    syslog(LOG_INFO, "Ready");

    // main event loop
    while (deamonize) {
        handle_request(mosq, config);
        usleep(10000);
    }

    // clean up mosq
    
    mosquitto_loop_stop(mosq, true);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    // close log
    closelog();
    return 0;
}