/**
 * Project: Compiler IFJ21
 *
 * @file queue.h
 *
 * @brief Implementation of Queue
 *
 * @author Andrej Binovsky <xbinov00>
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>

typedef struct QueueElement{
    htab_item_t *id;
    struct QueueElement *previous_element;
    struct QueueElement *next_element;
} QueueElementPtr;

typedef struct queue{
    QueueElementPtr *front;
    QueueElementPtr *rear;
} Queue;


/**
 * @brief Initialization of empty queue
 * @return On success pointer on queue, otherwise null
 */
Queue* queue_init();

/**
 * @brief Free queue
 * @param queue
 */
void queue_dispose(Queue *queue);


/**
 * @brief Function to find out if queue is empty
 * @param queue
 * @return When is empty True, otherwise False
 */
bool queue_isEmpty(Queue *queue);

/**
 * @brief Remove element from the beginning of queue
 * @param queue
 */
void queue_remove(Queue *queue);

/**
 * @brief Add element from the end of queue
 * @param queue
 * @param id
 * @return On success True, otherwise False
 */
bool queue_add(Queue *queue, htab_item_t *id);

/**
 * @brief Return name of identifier
 * @param queue
 * @return When is empty NULL, otherwise name of identifier
 */
htab_item_t* queue_front(Queue *queue);


#endif // _QUEUE_H
