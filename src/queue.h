#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
 
#include "request.h"


// request queue node
struct q_req_node {
    struct mqtt_ftp_req *request;
    struct q_req_node *next;
};

// request queue
struct q_req {
    struct q_req_node *front;
    struct q_req_node *rear;
};

void q_req_enqueue(struct q_req *queue, struct mqtt_ftp_req *request);

void q_req_dequeue(struct q_req *queue);

void debug_print_q_req(struct q_req queue);

struct q_req create_q_req();

#endif /* QUEUE_H */