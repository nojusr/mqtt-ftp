#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_CONFIG_PATH "/etc/mqtt-ftp/mqtt-ftp.conf"

#include "queue.h"

struct mqtt_ftp_config {
    // mosq related
    char *mosq_host;
    int mosq_port;
    char *mosq_input_topic;
    char *mosq_output_topic;

    int use_credentials; // 0 if no, 1 if yes
    char *mosq_username;
    char *mosq_password;

    int use_tls; // 0 if no, 1 if yes
    char *mosq_cert_path;

    // unix socket related
    int socket; // locally stored
    char *sun_path;

    // internal variables, used for
    struct q_req messages;
};

// tries to get config path from commandline args,
// if no path, returns default as defined above
char *get_config_path(int argc, char *argv[]);

// will forcefully exit if error in config.
void read_mqtt_ftp_config_from_file(char *path, struct mqtt_ftp_config *conf);

// if the program finds "--help" in argv, print help msg and exit
void print_help_msg(int argc, char *argv[]);

// debug print config
void debug_print_mqtt_config(struct mqtt_ftp_config *conf);

#endif /* CONFIG_H */