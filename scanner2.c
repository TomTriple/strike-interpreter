//
//  scanner.c
//  BinTree
//
//  Created by tom hoefer on 27.01.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include "scanner.h"  

static FILE *fp;


void file_init() {
    fp = fopen("/Users/tom/tom/NetBeansProjects/xcode/C-Progr/BinTree/BinTree/code.txt", "r"); 
    if(fp == NULL) 
        printf("fehler, datei konnte nicht geöffnet werden\n"); 
}

void file_close() {
    fclose(fp); 
}

char *next_word() {

    char c;
    char *word = (char *) malloc(sizeof(char *)); 
    
    while ((c = fgetc(fp)) != EOF) {
        if(c >= 'a' && c <= 'z') {
            strcat(word, &c);            
        } else if(c == ' ') {
            return word; 
        } else {
            printf("blöd gelaufen...");
        }
    }
    
    return NULL; 
    
}