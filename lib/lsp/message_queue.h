#ifndef LIB_LSP_MESSAGE_QUEUE_H_
#define LIB_LSP_MESSAGE_QUEUE_H_

#include <pthread.h>

typedef struct Message {
    void* data;
    struct Message* next;
} Message;

typedef struct MessageQueue {
    Message* front;
    Message* rear;
    pthread_mutex_t mutex;
} MessageQueue;

MessageQueue* msgq_create(void);

void msgq_enqueue(MessageQueue* queue, void* data);

void* msgq_dequeue(MessageQueue* queue);

#endif  // LIB_LSP_MESSAGE_QUEUE_H_
