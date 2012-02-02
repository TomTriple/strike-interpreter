//
//  expr_linked_list.c
//  BinTree
//
//  Created by tom hoefer on 01.02.12.
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
    
    if(state->head == NULL) {
        state->last = QE_alloc(); 
        state->last->item = item;
        state->head = state->last;
    } else {
        struct QElement *new_last = QE_alloc(); 
        new_last->item = item;
        state->last->next = new_last; 
        state->last = new_last; 
    }
    
}


void *queue_dequeue(struct QState *state) { 
    
    state->iterator = state->iterator == NULL ? state->head : state->iterator->next;
    return state->iterator == NULL ? NULL : state->iterator->item; 
    
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






