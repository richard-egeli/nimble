#include "message_queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* msgq_dequeue(MessageQueue* queue) {
    void* result = NULL;
    pthread_mutex_lock(&queue->mutex);
    if (queue->front != NULL) {
        Message* temp = queue->front;
        queue->front  = queue->front->next;
        if (queue->front == NULL) {
            queue->rear = NULL;
        }

        result = temp->data;
        free(temp);
    }

    pthread_mutex_unlock(&queue->mutex);
    return result;
}

void msgq_enqueue(MessageQueue* queue, void* data) {
    Message* msg = malloc(sizeof(*msg));
    if (msg == NULL) {
        perror("failed to allocate memory for message");
        return;
    }

    msg->data = data;
    msg->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if (queue->rear != NULL) {
        queue->rear->next = msg;
    } else {
        queue->front = msg;
    }

    queue->rear = msg;
    pthread_mutex_unlock(&queue->mutex);
}

MessageQueue* msgq_create(void) {
    MessageQueue* queue = malloc(sizeof(*queue));
    queue->front        = NULL;
    queue->rear         = NULL;

    pthread_mutex_init(&queue->mutex, NULL);
}
