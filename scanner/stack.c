//
//  expr_linked_list.c
//  BinTree
//
//  Created by tom hoefer on 01.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h> 
#include "../scanner/scanner.h" 
#include "../stack.h"

static struct SElement *SE_alloc(void);

struct SState *stack_new() {

    struct SState *initial_state = (struct SState *)malloc(sizeof(struct SState)); 
    if(initial_state == NULL)
        printf("keiner speicher bei stack_new"); 
    return initial_state; 
    
}


void stack_push(struct SState *state, void *item) {
    
    if(state->head == NULL) {
        state->head = SE_alloc(); 
        state->head->item = item; 
        state->first = state->head; 
    } else {
        struct SElement *new_head = SE_alloc(); 
        new_head->item = item; 
        new_head->prev = state->head;
        state->head = new_head;
    }
    
    state->iterator = NULL; 
    
}


void *stack_pop(struct SState *state) { 
    
    if(state->end_seen)
        return NULL;
    
    state->iterator = state->iterator == NULL ? state->head : state->iterator->prev; 
        
    if(state->iterator == state->first)
        state->end_seen = 1;
    
    state->head = state->head->prev; 
    return state->iterator->item;
}


void *stack_top(struct SState *state) {
    return state->head == NULL ? NULL : state->head->item;
}


int stack_is_empty(struct SState *state) {
    return stack_top(state) == NULL; 
}


void stack_test(struct SState *state, void (*callback)(void *item)) {
    
    struct SState *state_copy = &(*state); 
    void *it;
    while ((it = stack_pop(state_copy)) != NULL) {
        callback(it); 
    }

}


static struct SElement *SE_alloc() {
    
    struct SElement *elem = (struct SElement *) malloc(sizeof(struct SElement));
    if(elem == NULL)
        printf("kein speicher für SElem");
    return elem; 
    
}






