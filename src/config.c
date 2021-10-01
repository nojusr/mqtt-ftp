#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <sys/syslog.h>

#include "config.h"


char *get_config_path(int argc, char *argv[]) {

    char *output = NULL;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0) {
            if (i+1 >= argc) { // check if next argv exists
                printf("missing argument for --config\n");
                exit(0);
            }

            if (argv[i+1][0] != '-') { // check if next argv is another param
                output = strdup(argv[i+1]);
            } else {
                printf("missing argument for --config\n");
                exit(0);
            }
        }
    }

    if (output == NULL) { // unspecified
        return DEFAULT_CONFIG_PATH;
    }
    return output;
}

void debug_print_mqtt_config(struct mqtt_ftp_config *conf) {
    printf("mqtt-ftp-config: \n\n");
    printf(" mqtt settings:\n");
    printf("  host: %s\n", conf->mosq_host);
    printf("  port: %d\n", conf->mosq_port);
    printf("  input topic: %s\n", conf->mosq_input_topic);
    printf("  output topic: %s\n", conf->mosq_output_topic);
    printf("  use_credentials: %d\n", conf->use_credentials);
    
    if (conf->use_credentials == 1) {
        printf("  username: %s\n", conf->mosq_username);
        printf("  password: %s\n\n", conf->mosq_password);
    }

    printf("  use_credentials: %d\n", conf->use_tls);
    if (conf->use_tls == 1) {
        printf("cert_path: %s\n", conf->mosq_cert_path);
    }

    printf(" socket settings:\n");
    printf("  socket path: %s\n\n\n", conf->sun_path);
}

void check_config_rc(int rc, char *value) {
    if (rc != CONFIG_TRUE) {
        syslog(LOG_ERR, "Failed to get value in config; %s", value);
        exit(1);
    }
}

void read_mqtt_ftp_config_from_file(char *path, struct mqtt_ftp_config *conf) {
    config_t cfg;
    const char *cfg_str_buf = NULL;
    int rc;

    config_init(&cfg);

    syslog(LOG_INFO, "Reading config from %s", path);

    if (!config_read_file(&cfg, path)) {
        syslog(LOG_ERR, "Failed to parse config file: %s:%d - %s", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
        exit(1);
    }

    rc = config_lookup_string(&cfg, "mqtt_host", &cfg_str_buf);
    check_config_rc(rc, "mqtt_host");
    conf->mosq_host = strdup(cfg_str_buf);

    rc = config_lookup_int(&cfg, "mqtt_port", &(conf->mosq_port));
    check_config_rc(rc, "mqtt_port");

    rc = config_lookup_string(&cfg, "mqtt_input_topic", &cfg_str_buf);
    check_config_rc(rc, "mqtt_input_topic");
    conf->mosq_input_topic = strdup(cfg_str_buf);

    rc = config_lookup_string(&cfg, "mqtt_output_topic", &cfg_str_buf);
    check_config_rc(rc, "mqtt_output_topic");
    conf->mosq_output_topic = strdup(cfg_str_buf);

    rc = config_lookup_bool(&cfg, "use_mqtt_credentials", &(conf->use_credentials));
    check_config_rc(rc, "use_mqtt_credentials");

    if (conf->use_credentials == 1) {
        rc = config_lookup_string(&cfg, "mqtt_username", &cfg_str_buf);
        check_config_rc(rc, "mqtt_username");
        conf->mosq_username = strdup(cfg_str_buf);

        rc = config_lookup_string(&cfg, "mqtt_password", &cfg_str_buf);
        check_config_rc(rc, "mqtt_password");
        conf->mosq_password = strdup(cfg_str_buf);
    }
    rc = config_lookup_bool(&cfg, "use_tls", &(conf->use_tls));
    check_config_rc(rc, "use_tls");

    if (conf->use_tls == 1) {
        rc = config_lookup_string(&cfg, "tls_cert_path", &cfg_str_buf);
        check_config_rc(rc, "tls_cert_path");
        conf->mosq_cert_path = strdup(cfg_str_buf);
    }

    conf->sun_path = NULL;
    rc = config_lookup_string(&cfg, "sun_path", &cfg_str_buf);
    if (rc == CONFIG_TRUE) {
        conf->sun_path = strdup(cfg_str_buf);
    }

    debug_print_mqtt_config(conf);
}

void print_help_msg(int argc, char *argv[]) {

    int should_print = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            should_print = 1;
        } 
    }

    if (should_print == 0) {
        return;
    }

    printf("Arguments:\n");
    printf("    --config PATH, -c PATH   specify the config path to use\n");
    printf("                             defaults to \"/etc/mqtt-ftp/mqtt-ftp.conf\"\n");
    printf("\n");
    printf("    --help, -h               print this message\n\n");
    exit(0);
}