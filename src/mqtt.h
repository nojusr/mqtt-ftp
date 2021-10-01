#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>

#include "config.h"

void mosq_full_init(struct mosquitto *mosq, struct mqtt_ftp_config *config);

void mosq_on_connect(struct mosquitto *mosq, void *obj, int reason_code);

void mosq_on_sub(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);

void mosq_on_msg(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

void mosq_on_pub(struct mosquitto *mosq, void *obj, int mid);

#endif /* MQTT_H */