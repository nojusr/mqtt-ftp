#include <json-c/json_object.h>
#include <sys/syslog.h>
#include <syslog.h>
#include <json-c/json.h>
#include <string.h>
#include <stdio.h>

#include "request.h"

char *mqtt_ftp_req_command_itoa(mqtt_ftp_req_command input) {
    switch (input) {
        case MQTT_FTP_REQ_READ:
            return "read";
        case MQTT_FTP_REQ_WRITE:
            return "write";
        case MQTT_FTP_REQ_CREATE:
            return "create";
        case MQTT_FTP_REQ_DELETE:
            return "delete";
        default:
            return NULL;
    }
}

mqtt_ftp_req_command mqtt_ftp_req_command_atoi(const char *input) {
    if (strcmp(input, "read") == 0) {
        return MQTT_FTP_REQ_READ;
    } else if (strcmp(input, "write") == 0) {
        return MQTT_FTP_REQ_WRITE;
    } else if (strcmp(input, "create") == 0) {
        return MQTT_FTP_REQ_CREATE;
    } else if (strcmp(input, "delete") == 0) {
        return MQTT_FTP_REQ_DELETE;
    } else {
        return -1;
    }
}
void parse_req_from_str(char *input, struct mqtt_ftp_req *request) {
    struct json_object *jobj;
    jobj = json_tokener_parse(input);
    parse_req_from_json(jobj, request);
}

void parse_req_from_json(json_object *json, struct mqtt_ftp_req *request) {
    
    struct json_object *buf;

    request->id = -1;
    request->command = -1;
    request->filepath = NULL;
    request->write_input = NULL;

    if (json == NULL) {
        return;
    }

    if (json_object_object_get_ex(json, "command", &buf)) {
        request->command = mqtt_ftp_req_command_atoi(json_object_get_string(buf));
    }
    
    if (json_object_object_get_ex(json, "file_path", &buf)) {
        request->filepath = json_object_get_string(buf);
    }

    if (json_object_object_get_ex(json, "write_input", &buf)) {
        request->write_input = json_object_get_string(buf);
    }

    if (json_object_object_get_ex(json, "request_id", &buf)) {
        request->id = json_object_get_int(buf);
    }

    return;
}

void debug_print_request(struct mqtt_ftp_req *request) {
    printf("\nmqtt-ftp request: \n");
    printf("  id: %d\n", request->id);
    printf("  command: %s\n", mqtt_ftp_req_command_itoa(request->command));
    printf("  filepath: %s\n", request->filepath);
    printf("  write_input: %s\n", request->write_input);
    printf("  origin: %d\n\n", request->origin);
}