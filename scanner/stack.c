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
        struct SElement *new_item = SE_alloc(); 
        new_item->item = item;
        state->head = new_item; 
    } else {
        struct SElement *new_item = SE_alloc(); 
        new_item->next = state->head; 
        new_item->item = item; 
        state->head = new_item; 
    }

}


void *stack_pop(struct SState *state) {
    if(state->head == NULL)
        return NULL;
    
    struct SElement *it; 
    it = state->head; 
    state->head = ((struct SElement *) state->head)->next;
    
    return it->item; 
}


void *stack_top(struct SState *state) {
    if (state->head == NULL)
        return NULL;
    return ((struct SElement *) state->head)->item;
}


int stack_is_empty(struct SState *state) {
    return stack_top(state) == NULL;
}


void stack_test(struct SState *state, void (*callback)(void *item)) {
    


}


static struct SElement *SE_alloc() {
    
    struct SElement *elem = (struct SElement *) malloc(sizeof(struct SElement));
    if(elem == NULL)
        printf("kein speicher für SElem");
    return elem; 
    
}






