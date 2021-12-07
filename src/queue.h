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
#include "scanner.h"
#include "symtable.h"
typedef struct QueueElement{
    htab_item_t *id;
    token_t *token;
    unsigned int cnt_if;
    unsigned int cnt_while;
    struct QueueElement *previous_element;
    struct QueueElement *next_element;
    bool used;
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
 * @brief Empty queue
 * @param queue
 */
void queue_dispose(Queue *queue);

/**
 * @brief Free queue
 * @param queue
 */
void queue_free(Queue *queue);

/**
 * @brief Function to find out if queue is empty
 * @param queue
 * @return When is empty True, otherwise False
 */
bool queue_isEmpty(Queue *queue);

/**
 * @brief Remove front element
 * @param queue
 */
void queue_remove_front(Queue *queue);

/**
 * @brief Remove rear element
 * @param queue
 */
void queue_remove_rear(Queue *queue);

/**
 * @brief Add element on the end of queue
 * @param queue
 * @return On success True, otherwise False
 */
bool queue_add_rear(Queue *queue);

/**
 * @brief Add element htab_item_t
 * @param queue
 * @param if
 * @return On success True, otherwise False
 */
bool queue_add_id_rear(Queue *queue, htab_item_t *id);

/**
* @brief Add element token_t
* @param queue
* @param token
* @return On success True, otherwise False
*/
bool queue_add_token_rear(Queue *queue, token_t *token);

/**
 * @brief Add element on start of queue
 * @param queue
 * @return On success True, otherwise False
 */
bool queue_add_front(Queue *queue);

/**
 * @brief Add element htab_item_t
 * @param queue
 * @param if
 * @return On success True, otherwise False
 */
bool queue_add_id_front(Queue *queue, htab_item_t *id);

/**
* @brief Add element token_t on front
* @param queue
* @param token
* @return On success True, otherwise False
*/
bool queue_add_token_front(Queue *queue, token_t *token);

#endif // _QUEUE_H
