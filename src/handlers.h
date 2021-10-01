#ifndef HANDLERS_H
#define HANDLERS_H

#include <mosquitto.h>

#include "config.h"

// 900 byte limit for response payload.
// the actual limit is 1024 bytes, the leftovers are to be used for all other data
#define RESPONSE_PAYLOAD_LIMIT 900



void handle_request(struct mosquitto *mosq, struct mqtt_ftp_config *config);


#endif /* HANDLERS_H */