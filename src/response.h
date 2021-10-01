#ifndef RESPONSE_H
#define RESPONSE_H

#include <json-c/json_object.h>

// enum for returncode of mqtt response
typedef enum {
    RESP_OK,
    // RESP_OK_TRUNCATED // should i implement this to inform the end-user that the response is truncated?
    RESP_ERR, // general err
    RESP_NOT_FOUND,
    RESP_MISSING_PERMS, // can't read file due to permissions
    __RESP_MAX,
} mqtt_ftp_resp_rc;

struct mqtt_ftp_resp {
    int id;
    char *output;
    mqtt_ftp_resp_rc rc;
};

// converts to json char*
const char *resp_to_str(struct mqtt_ftp_resp input);

json_object *resp_to_json_obj(struct mqtt_ftp_resp input);

void debug_print_response(struct mqtt_ftp_resp input); 

#endif /* RESPONSE_H */