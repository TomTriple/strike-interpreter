//
//  hash_table.c
//  BinTree
//
//  Created by tom hoefer on 27.01.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "hash_table.h"

static unsigned int hash(char *key); 


HState *hash_new() {

    HState *state = (HState *) malloc(sizeof(HState));
    if(NULL == state)
        printf("no mem at hash_new...");
    return state; 
}


void hash_add(HState *state, char *key, void *value) {
    
    struct bucket *bucket = hash_lookup(state, key); 
    if(bucket == NULL) {
        bucket = (struct bucket *) malloc(sizeof(struct bucket)); 
        unsigned hash_value = hash(key); 
        bucket->key = strdup(key); 
        bucket->value = value; 
        
        // nötig, falls zwei keys den gleichen hash-wert haben aber 
        // lookup dennoch NULL liefert da ja zusätzlich auch der suchkey
        // mit einem bucket-key übereinstimmen muss. 
        bucket->next = state->table[hash_value];
        state->table[hash_value] = bucket; 
    } else {
        free(bucket->value);
        bucket->value = value;
    }
    
}

void *hash_lookup(HState *state, char *key) {
    
    struct bucket *bucket = state->table[hash(key)];
    
    while (bucket != NULL) {
        if(strcmp(key, bucket->key) == 0)
            return bucket->value;          
        bucket = bucket->next; 
    }
    
    return NULL; 
}


static unsigned int hash(char *key) {

    unsigned val;
    
    for (val = 0; *key != '\0'; key++) {
        val = *key + 31 * val; 
    }
    
    return val % SIZE; 
    
}

