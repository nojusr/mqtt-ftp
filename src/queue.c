#include <stdlib.h>
#include <sys/syslog.h>
#include <syslog.h>

#include "queue.h"
#include "request.h"

void q_req_enqueue(struct q_req *queue, struct mqtt_ftp_req *request) {
    if (request == NULL) {
        syslog(LOG_ERR, "null pointer passed to q_req_enqueue");
        return;
    }

    //struct q_req_node *nptr = malloc(sizeof(struct q_req_node));

    struct q_req_node *nptr = calloc(1, sizeof(struct q_req_node));

    if (!nptr) {
        syslog(LOG_CRIT, "out of memory!");
        return;
    }

    nptr->request = request;
    nptr->next = NULL;

    if (queue->rear == NULL) {
        queue->front = nptr;
        queue->rear = nptr;
    } else {
        queue->rear->next = nptr;
        queue->rear = queue->rear->next;
    }
}

void q_req_dequeue(struct q_req *queue) {
    if (queue->front == NULL) {
        return; // queue is empty
    } else {
        struct q_req_node *temp;
        temp = queue->front;
        queue->front = queue->front->next;
        if (queue->front == NULL) { // clear queue rear if queue empty
            queue->rear = NULL;
        }
        free(temp);
    }
}

void debug_print_q_req_node(struct q_req_node *node) {

    if (node == NULL) {
        printf("(node is null)\n");
        return;
    }

    printf("node: \n  request:\n");
    if (node->request != NULL) {
        debug_print_request(node->request);
    } else {
        printf("(null)\n");
    }

    printf("next node: %p\n", node->next);
}

void debug_print_q_req(struct q_req queue) {
    struct q_req_node *tmp = NULL;
    tmp = queue.rear;

    if (queue.front == NULL) {
        printf("queue is empty.\n");
        return;
    }

    while (1) {
        debug_print_q_req_node(tmp);
        if (tmp->next == NULL) {
            break;
        }
        tmp = tmp->next;
    }
}



struct q_req create_q_req() {
    struct q_req output;
    output.front = NULL;
    output.rear = NULL;
    return output;
}