/**
 * @file queue.c
 * @brief Implementation of a queue that supports FIFO and LIFO operations.
 *
 * This queue implementation uses a singly-linked list to represent the
 * queue elements. Each queue element stores a string value.
 *
 * Assignment for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 * Extended to store strings, 2018
 *
 * TODO: fill in your name and Andrew ID
 * @author XXX <XXX@andrew.cmu.edu>
 */

#include "queue.h"
#include "harness.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocates a new queue
 * @return The new queue, or NULL if memory allocation failed
 */
queue_t *queue_new(void) {
    queue_t *q = malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    if (q == NULL) return NULL; // return NULL if alloction failed
    // initialize the 3 elements of a queue_t
    q->size = 0;
    q->tail = NULL;
    q->head = NULL;
    return q;
}

/**
 * @brief Frees all memory used by a queue
 * @param[in] q The queue to free
 */
void queue_free(queue_t *q) {
    /* How about freeing the list elements and the strings? */
    
    // no need to free if NULL
    if (q == NULL) {
        return;
    }
    // free elements of queue and temporary variable created
    while (q->head != NULL) {
        list_ele_t *temp = q->head;
        q->head = q->head->next;
        free(temp->value);
        free(temp);
    }
    /* Free queue structure */
    free(q);
}

/**
 * @brief Attempts to insert an element at head of a queue
 *
 * This function explicitly allocates space to create a copy of `s`.
 * The inserted element points to a copy of `s`, instead of `s` itself.
 *
 * @param[in] q The queue to insert into
 * @param[in] s String to be copied and inserted into the queue
 *
 * @return true if insertion was successful
 * @return false if q is NULL, or memory allocation failed
 */
bool queue_insert_head(queue_t *q, const char *s) {
    list_ele_t *newh;
    /* What should you do if the q is NULL? */
    // return false if q is NULL
    if (q == NULL || s == NULL) {
        return false;
    }
    /* Don't forget to allocate space for the string and copy it */
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL) {
        return false; 
    }
    size_t len = strlen(s);
    /* What if either call to malloc returns NULL? */
    // every allocation should be followed by NULL examination 
    newh->value = (char *)malloc(len + 1);
    if (newh->value == NULL) {
        free(newh);
        return false; 
    }
    strcpy(newh->value, s);
    // the 3 elements of queue must be updated
    // pay attention to the 0 element queue
    newh->next = q->head;
    q->head = newh;
    // tail pointer
    if (q->tail == NULL) {
        q->tail = newh; 
    }
    // size of queue
    q->size++;
    return true;
}

/**
 * @brief Attempts to insert an element at tail of a queue
 *
 * This function explicitly allocates space to create a copy of `s`.
 * The inserted element points to a copy of `s`, instead of `s` itself.
 *
 * @param[in] q The queue to insert into
 * @param[in] s String to be copied and inserted into the queue
 *
 * @return true if insertion was successful
 * @return false if q is NULL, or memory allocation failed
 */
bool queue_insert_tail(queue_t *q, const char *s) {
    /* You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */
    // a tail pointer is needed if operating in O(1)
    // NULL examination
    if (q == NULL || s == NULL) {
        return false;
    }
    // copy of s and NULL examination
    list_ele_t *newt = malloc(sizeof(list_ele_t));
    if (newt == NULL) {
        return false; 
    }
    size_t len = strlen(s);
    newt->value = (char *)malloc(len + 1);
    if (newt->value == NULL) {
        free(newt);
        return false; 
    }
    strcpy(newt->value, s);
    // update 3 elements
    // pay attention to the 0 element queue
    newt->next = NULL;

    if (q->tail == NULL) {
        q->head = newt;
        q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }

    q->size++;
    return true;
}

/**
 * @brief Attempts to remove an element from head of a queue
 *
 * If removal succeeds, this function frees all memory used by the
 * removed list element and its string value before returning.
 *
 * If removal succeeds and `buf` is non-NULL, this function copies up to
 * `bufsize - 1` characters from the removed string into `buf`, and writes
 * a null terminator '\0' after the copied string.
 *
 * @param[in]  q       The queue to remove from
 * @param[out] buf     Output buffer to write a string value into
 * @param[in]  bufsize Size of the buffer `buf` points to
 *
 * @return true if removal succeeded
 * @return false if q is NULL or empty
 */
bool queue_remove_head(queue_t *q, char *buf, size_t bufsize) {
    /* You need to fix up this code. */
    // NULL examination
    if (q == NULL || q->head == NULL) {
        return false;
    }

    list_ele_t *temp = q->head;
    q->head = q->head->next;

    /**
     * char *strncpy(char *dest, const char *src, size_t n)
     * @brief copy n characters including '\0' from src to dest
     */
    if (buf != NULL && bufsize > 0) {
        strncpy(buf, temp->value, bufsize - 1);
        buf[bufsize - 1] = '\0';
    }
    // free must follow allocation
    free(temp->value);
    free(temp);

    q->size--;
    // pay attention to the 0 element queue
    if (q->head == NULL) {
        q->tail = NULL; 
    }

    return true;
}

/**
 * @brief Returns the number of elements in a queue
 *
 * This function runs in O(1) time.
 *
 * @param[in] q The queue to examine
 *
 * @return the number of elements in the queue, or
 *         0 if q is NULL or empty
 */
size_t queue_size(queue_t *q) {
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    // size is needed if operating in O(1)
    if (q == NULL) {
        return 0;
    }

    return q->size;
}

/**
 * @brief Reverse the elements in a queue
 *
 * This function does not allocate or free any list elements, i.e. it does
 * not call malloc or free, including inside helper functions. Instead, it
 * rearranges the existing elements of the queue.
 *
 * @param[in] q The queue to reverse
 */
void queue_reverse(queue_t *q) {
    /* You need to write the code for this function */
    // no reverse situation
    if (q == NULL || q->head == NULL || q->size == 1) {
        return; 
    }
    // define previous pointer -> current pointer -> next pointer
    list_ele_t *prev = NULL;
    list_ele_t *current = q->head;
    list_ele_t *next = NULL;
    /**
     * 1. update next pointer to keep connection
     * 2. reverse current pointer to connect previous pointer
     * 3. update previous pointer to current pointer
     * 4. update current pointer to next pointer
     */
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    // update tail pointer and head pointer
    q->tail = q->head;
    q->head = prev;
}
