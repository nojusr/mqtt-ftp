#include <mosquitto.h>
#include <sys/syslog.h>
#include <string.h>

#include "request.h"
#include "queue.h"
#include "config.h"
#include "mqtt.h"

void mosq_full_init(struct mosquitto **mosq, struct mqtt_ftp_config *config) {
    int rc;

    // If there's a need for a mqtt toggle functionality, check and return here
    
    syslog(LOG_INFO, "Initializing mosquitto");

    mosquitto_lib_init();

    *mosq = mosquitto_new(NULL, true, config);

    if (*mosq == NULL) {
        syslog(LOG_CRIT, "out of memory!");
        exit(1);
    }


    syslog(LOG_INFO, "SCOPE: mosq_full_init.");
    debug_print_mqtt_config(config);

    mosquitto_connect_callback_set(*mosq, mosq_on_connect);
    mosquitto_subscribe_callback_set(*mosq, mosq_on_sub);
    mosquitto_message_callback_set(*mosq, mosq_on_msg);

    syslog(LOG_INFO, "Connecting to %s:%d", config->mosq_host, config->mosq_port);



    rc = mosquitto_connect(*mosq, config->mosq_host, config->mosq_port, 60);
    if(rc != MOSQ_ERR_SUCCESS){
        mosquitto_destroy(*mosq);
        syslog(LOG_ERR, "Mosquitto error: %s\n", mosquitto_strerror(rc));
        exit(1);
    }
        
}

void mosq_on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
    int rc;



    syslog(LOG_INFO, "Connecting: %s", mosquitto_connack_string(reason_code));
    if(reason_code != 0){
        syslog(LOG_ERR, "Mosquitto connection failed.");
        mosquitto_disconnect(mosq);
    }

    struct mqtt_ftp_config *conf = (struct mqtt_ftp_config *)obj;

    syslog(LOG_INFO, "SCOPE: mosq_on_connect.");
    debug_print_mqtt_config(conf);

    syslog(LOG_INFO, "Subscribing to %s", conf->mosq_input_topic);
    rc = mosquitto_subscribe(mosq, NULL, conf->mosq_input_topic, 1);
    if (rc != MOSQ_ERR_SUCCESS) {
        syslog(LOG_ERR, "Mosquitto error when subscribing: %s", mosquitto_strerror(rc));
        mosquitto_disconnect(mosq);
    }

}

void mosq_on_sub(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos) {
    int i;
    bool have_subscription = false;

    for(i=0; i<qos_count; i++){
        if(granted_qos[i] <= 2){
            have_subscription = true;
        }
    }
    if(have_subscription == false){
        syslog(LOG_ERR, "Mosquitto error: rejected.");
    }

}

void mosq_on_msg(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    // get message, try to parse as JSON, add to queue

    syslog(LOG_INFO, "got msg: %s", (char*)msg->payload);

    struct mqtt_ftp_config *conf = (struct mqtt_ftp_config *)obj;

    struct mqtt_ftp_req *req = calloc(1, sizeof(struct mqtt_ftp_req));

    parse_req_from_str((char *)msg->payload, req);
    req->origin = MQTT_FTP_REQ_ORIGIN_MQTT;

    if (req->id != -1) {
        syslog(LOG_DEBUG, "REQUEST IS VALID.");
        //debug_print_request(req);
        q_req_enqueue(&(conf->messages), req);
        debug_print_q_req(conf->messages);
    }
}