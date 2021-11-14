/**
 * Project: Compiler IFJ21
 *
 * @file queue.h
 *
 * @brief Implementation of Queue
 *
 * @author Andrej Binovsky <xbinov00>
 */

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "symtable.h"


Queue* queue_init(){
    Queue *queue = calloc(1, sizeof(Queue));
    if(!queue){
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}
void queue_dispose(Queue *queue) {
    QueueElementPtr *tmp;
    while (queue->front != NULL) {
        tmp = queue->front;
        queue->front = queue->front->previous_element;
        free(tmp);
    }
    free(queue);
}

bool queue_isEmpty(Queue *queue){
    return (queue->front == NULL);
}


void queue_remove(Queue *queue){
    if(queue_isEmpty(queue)){
        return;
    }
    if(queue->front == queue->rear){
        free(queue->front);
        queue->front = NULL;
        queue->rear = NULL;
        return;
    }
    queue->front = queue->front->previous_element;
    free(queue->front->next_element);
    queue->front->next_element = NULL;
}

bool queue_add(Queue *queue, htab_item_t *id){
    QueueElementPtr *new_element =(QueueElementPtr *) calloc(1, sizeof(QueueElementPtr));
    if(!new_element){
        return false;
    }
    new_element->id = id;
    if (queue_isEmpty(queue)) {
        new_element->next_element = NULL;
        queue->front = new_element;
    } else {
        new_element->next_element = queue->rear;;
        queue->rear->previous_element = new_element;
    }
    new_element->previous_element = NULL;
    queue->rear = new_element;
    return true;
}

htab_item_t* queue_front(Queue *queue){
    if(queue_isEmpty(queue)){
        return NULL;
    }
    return queue->front->id;
}
