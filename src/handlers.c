

#include <asm-generic/errno-base.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <sys/syslog.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "config.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "handlers.h"

// TODO
void handle_command_read(struct mqtt_ftp_req *request, struct mqtt_ftp_resp *response) {
    int count = 0;
    FILE *fp;
    char buf;

    char *output_buf = calloc(RESPONSE_PAYLOAD_LIMIT, sizeof(char));

    if (output_buf == NULL) {
        syslog(LOG_CRIT, "out of memory!");
        // raise(SIGINT) // should i use this instead of kill()?
        exit(1);
    }

    if( access( request->filepath, F_OK | R_OK ) != 0 ) {
        if (errno == EACCES) {
            response->rc = RESP_MISSING_PERMS;
        } else if (errno == ENOENT) {
            response->rc = RESP_NOT_FOUND;
        } else {
            response->rc = RESP_ERR;
        }
        return;
    }

    fp = fopen(request->filepath,"r");

    if( fp == NULL ) {
        syslog(LOG_ERR, "Failed to read file at: %s", request->filepath);
        response->rc = RESP_ERR;
        return;
    }

    while( ( buf = fgetc(fp) ) != EOF || count < RESPONSE_PAYLOAD_LIMIT) {
        output_buf[count++] = buf;
    } 

    response->output = strdup(output_buf);
    response->rc = RESP_OK;

    free(output_buf);
    fclose(fp);
}

void handle_command_write(struct mqtt_ftp_req *request, struct mqtt_ftp_resp *response) {
    FILE *fp;

    if( access( request->filepath, F_OK | W_OK ) != 0 ) {
        if (errno == EACCES) {
            response->rc = RESP_MISSING_PERMS;
            return;
        } else if (errno != ENOENT) {
            response->rc = RESP_ERR;
            return;
        }
    }

    fp = fopen(request->filepath,"w+");

    if (request->write_input != NULL && strlen(request->write_input) != 0) {
        fputs(request->write_input, fp);
    }

    response->rc = RESP_OK;
    response->output = NULL;
    fclose(fp);

}

// the create command is pointless due to fopen behaviour. shouldn't be here, but hindsight is 20/20
void handle_command_create(struct mqtt_ftp_req *request, struct mqtt_ftp_resp *response) {
    request->write_input = NULL;
    handle_command_write(request, response);
}

void handle_command_delete(struct mqtt_ftp_req *request, struct mqtt_ftp_resp *response) {
    if( remove(request->filepath) != 0 ) {
        if (errno == EACCES) {
            response->rc = RESP_MISSING_PERMS;
        } else if (errno == ENOENT) {
            response->rc = RESP_NOT_FOUND;
        } else {
            response->rc = RESP_ERR;
        }

    } else {
        response->rc = RESP_OK;
    }

    return;
}

void send_response_mqtt(const char *output, struct mosquitto *mosq, const struct mqtt_ftp_config *config) {
    syslog(LOG_INFO, "Publishing response `%s` to topic: \"%s\"", output, config->mosq_output_topic);
    mosquitto_publish(mosq, NULL, config->mosq_output_topic, strlen(output), output, 1, false); 
}

void send_response_socket(const char *output, const struct mqtt_ftp_config *config) {

}

void handle_request(struct mosquitto *mosq, struct mqtt_ftp_config *config) {
    // steps:
    // 1. dequeue, check if null
    // 2. handle command
    // 3. create response
    // 4. send response out

    if (config->messages.front == NULL) {
        return; // no messages, nothing to do.
    }

    struct mqtt_ftp_req *request = config->messages.front->request;

    struct mqtt_ftp_resp response; 

    // sanity nullcheck for request
    if (request->id == -1) {
        // if request itself is not null, but request data is null, dequeue
        q_req_dequeue(&config->messages);
        return;
    }

    // at this point we have a good request coming in.
    // prepare response
    response.id = request->id;
    response.output = NULL;
    response.rc = -1;


    switch (request->command) { // switch and scan for command
        case MQTT_FTP_REQ_READ:
            handle_command_read(request, &response);
            break;
        case MQTT_FTP_REQ_WRITE:
            handle_command_write(request, &response);
            break;
        case MQTT_FTP_REQ_CREATE:
            handle_command_write(request, &response);
            break;
        case MQTT_FTP_REQ_DELETE:
            handle_command_delete(request, &response);
            break;
        default:
            syslog(LOG_ERR, "Received unknown command. Passing.");
            return;
    }

    debug_print_response(response);
    

    q_req_dequeue(&config->messages);
    
    const char *output = resp_to_str(response);
    // TODO: check origin in response and direct to proper response location
    send_response_mqtt(output, mosq, config);

}




