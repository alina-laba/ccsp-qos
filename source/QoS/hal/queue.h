#ifndef __QOS_QUEUE_HAL_H__
#define __QOS_QUEUE_HAL_H__

#include <net/if.h>

#define QUEUE_MAX_TC 16

enum queue_alg { QUEUE_ALG_SP, QUEUE_ALG_WRR };

struct qos_queue
{
    // Interface name
    char device_name[IFNAMSIZ];
    // tc algorithm: QUEUE_ALG_SP (strict priority) or QUEUE_ALG_WRR (weighted
    // round robin)
    int alg;
    // Queue priority: lower number -> higher priopity
    unsigned priority;
    // Queue weight for QUEUE_ALG_WRR algorithms
    int weight;
    // Queue rate in kbps
    unsigned shaping_rate;
    // Number of elements in class_list
    unsigned class_size;
    // List of traffic classes related to the queue
    int class_list[QUEUE_MAX_TC];
};

// Adds queue & shaping
int qos_addQueue(int index, struct qos_queue *queue);
// Removes ALL queues & shaping
int qos_removeQueue(struct qos_queue *queue);

#endif
