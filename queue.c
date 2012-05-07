//
//  expr_linked_list.c
//
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h> 
#include "scanner/scanner.h" 
#include "queue.h"


static struct QElement *QE_alloc(void);


struct QState *queue_new() {
    struct QState *initial_state = (struct QState *) malloc(sizeof(struct QState));
    if(initial_state == NULL)
        printf("fehler new_queue...");
    return initial_state; 
}


void queue_enqueue(struct QState *state, void *item) {

    struct QElement *it = QE_alloc(); 
    if(state->head == NULL) { 
        it->item = item; 
        state->traversal = state->head = state->last = it;
    } else {
        it->item = item; 
        state->last->next = it; 
        state->last = it;
    }

}


void *queue_dequeue(struct QState *state) { 
    struct QElement *it = state->traversal; 
    if(it == NULL)
        return NULL;
    state->traversal = state->traversal->next; 
    return it->item; 
}


void queue_reset(struct QState *state) {
    state->traversal = state->head; 
}


void queue_test(struct QState *state, void (*callback)(void *it)) {
    void *it;
    struct QState *initial_state = &(*state);
    while ((it = queue_dequeue(initial_state)) != NULL) {
        callback(it);
    }
}


static struct QElement *QE_alloc() {

    struct QElement *elem = (struct QElement *) malloc(sizeof(struct QElement));
    if(elem == NULL)
        printf("kein speicher für QElem");
    return elem; 

}






