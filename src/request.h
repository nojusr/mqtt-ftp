#ifndef REQUEST_H
#define REQUEST_H

#include <json-c/json.h>

typedef enum {
    MQTT_FTP_REQ_READ,
    MQTT_FTP_REQ_WRITE,
    MQTT_FTP_REQ_CREATE,
    MQTT_FTP_REQ_DELETE,
    __MQTT_FTP_REQ_MAX,
} mqtt_ftp_req_command;

typedef enum {
    MQTT_FTP_REQ_ORIGIN_MQTT,
    MQTT_FTP_REQ_ORIGIN_SOCK,
} mqtt_ftp_req_origin;

struct mqtt_ftp_req { // asume null if id is -1
    int id;
    mqtt_ftp_req_command command;
    const char *filepath;
    const char *write_input;
    mqtt_ftp_req_origin origin;
};

char *mqtt_ftp_req_command_itoa(mqtt_ftp_req_command input);

mqtt_ftp_req_command mqtt_ftp_req_command_atoi(const char *input);

void parse_req_from_str(char *input, struct mqtt_ftp_req *request);

void parse_req_from_json(json_object *json, struct mqtt_ftp_req *request);

void debug_print_request(struct mqtt_ftp_req *request);

#endif /* REQUEST_H */