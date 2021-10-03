#include <json-c/json_object.h>
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

#include "response.h"

const char *resp_to_str(struct mqtt_ftp_resp input) {
    json_object *jobj = resp_to_json_obj(input);
    const char *output = json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN);
    return output;
}

json_object *resp_to_json_obj(struct mqtt_ftp_resp input) {
    json_object *jobj = json_object_new_object();

    json_object_object_add(jobj, "request_id", json_object_new_int(input.id));
    if (input.output != NULL && strlen(input.output) != 0) {
        json_object_object_add(jobj, "output", json_object_new_string(input.output));
    }
    json_object_object_add(jobj, "response", json_object_new_int(input.rc));
    return jobj;
}

void debug_print_response(struct mqtt_ftp_resp input) {
    printf("\n\nmqtt-ftp-response: \n");
    printf("  id: %d\n", input.id);
    printf("  output: %s\n", input.output);
    printf("  response code: %d\n\n", input.rc);
}