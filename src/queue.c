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
    while (!queue_isEmpty(queue)) {
        if(queue->front->token){
            if (queue->front->token->type == T_ID || queue->front->token->type == T_STRING) {
                str_free(&queue->front->token->attr.id);
            }
            free(queue->front->token);
        }
        tmp = queue->front;
        queue->front = queue->front->previous_element;
        free(tmp);
    }
}

void queue_free(Queue *queue){
    if(queue){
        queue_dispose(queue);
        free(queue);
    }
}

bool queue_isEmpty(Queue *queue){
    return (queue->front == NULL);
}

void queue_remove_front(Queue *queue){
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

void queue_remove_rear(Queue *queue){
    if(queue_isEmpty(queue)){
        return;
    }
    if(queue->front == queue->rear){
        free(queue->front);
        queue->front = NULL;
        queue->rear = NULL;
        return;
    }
    queue->rear = queue->rear->next_element;
    free(queue->rear->previous_element);
    queue->rear->previous_element = NULL;
}

bool queue_add_rear(Queue *queue){
    QueueElementPtr *new_element =(QueueElementPtr *) calloc(1, sizeof(QueueElementPtr));
    if(!new_element){
        return false;
    }
    if (queue_isEmpty(queue)) {
        new_element->next_element = NULL;
        queue->front = new_element;
    } else {
        new_element->next_element = queue->rear;
        queue->rear->previous_element = new_element;
    }
    new_element->previous_element = NULL;
    queue->rear = new_element;
    return true;
}

bool queue_add_id_rear(Queue *queue, htab_item_t *id) {
    if(queue_add_rear(queue)){
        queue->rear->id = id;
        return true;
    }
    return false;

}
bool queue_add_token_rear(Queue *queue, token_t *token) {
    if(queue_add_rear(queue)){
        queue->rear->token = token;
        return true;
    }
    return false;
}

bool queue_add_front(Queue *queue){
    QueueElementPtr *new_element =(QueueElementPtr *) calloc(1, sizeof(QueueElementPtr));
    if(!new_element){
        return false;
    }
    if (queue_isEmpty(queue)) {
        new_element->previous_element = NULL;
        queue->rear = new_element;
    } else {
        new_element->previous_element = queue->front;
        queue->front->next_element = new_element;
    }
    new_element->next_element = NULL;
    queue->front = new_element;
    return true;
}

bool queue_add_id_front(Queue *queue, htab_item_t *id) {
    if(queue_add_front(queue)){
        queue->front->id = id;
        return true;
    }
    return false;

}
bool queue_add_token_front(Queue *queue, token_t *token) {
    if(queue_add_front(queue)){
        queue->front->token = token;
        return true;
    }
    return false;
}
