//
//  main.c
//  BinTree
//
//  Created by tom hoefer on 25.01.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.


/*
http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
 
 
S ::= Prog$
Prog ::= id = E;

E --> P A
A -> BPA | Epsilon
P --> V | "(" E ")" | U P
B --> "+" | "-" | "*" | "/" | "^"
U --> "-"
V = id | number

FIRST(E) = id number ( - 
FIRST(A) = Epsilon + - * / ^
FOLLOW(A) = TOK_SEMICOLON, TOK_PAREN_CLOSE  
FIRST(P) = id number ( - 
FIRST(B) = + - * / ^ 
FIRST(U) = - 
FIRST(V) = id number 

*/


#include "scanner/scanner.h"
#include <stdlib.h>
#include <stdio.h>

void syntax_error(int tok);
Token *consume_token(void); 
void match(int token); 
void test_tokens(void); 

void parse_start(void);
void parse_program(void);
void parse_expr(void);
void parse_a(void); 
void parse_p(void); 
void parse_b(void); 
void parse_u(void);
void parse_v(void); 



static Token *lookahead; 


int main(int argc, char **args) { 
    
    char *source = "/Users/tom/tom/NetBeansProjects/xcode/C-Progr/BinTree/BinTree/code2.txt";
    
    init_scanner(source);
    
    // test_tokens();
    // exit(0);
    
    consume_token();
    parse_start();
    printf("syntax ok\n");

    return EXIT_SUCCESS; 

}


void parse_start() {
    
    switch (lookahead->tok_type) {
        case TOK_ID:
            parse_program(); 
            match(TOK_TERMINATE); 
        break;
        default:
            syntax_error(TOK_ID); 
    }

}


void parse_program() {

    switch (lookahead->tok_type) {
        case TOK_ID:
            match(TOK_ID); 
            match(TOK_EQ); 
            parse_expr(); 
            match(TOK_SEMICOLON); 
        break;
        default:
            syntax_error(TOK_ID); 
    }

}

// E --> P A
// FIRST(E) = id number ( - 
void parse_expr() {

    switch (lookahead->tok_type) {
        case TOK_ID:
        case TOK_NUMBER:
        case TOK_PAREN_OPEN:
        case TOK_MINUS:
            parse_p();
            parse_a();
        break;
            
        default:
            printf("fehler parse_expr");
    }

}


// Exp-Operator ^ noch nicht implementiert!!
// A -> BPA | Epsilon
// FIRST(A) = + - * / ^ Epsilon
// FOLLOW(A) = TOK_SEMICOLON, TOK_PAREN_CLOSE
void parse_a() {
        
    switch (lookahead->tok_type) {
        case TOK_BINOP:
            parse_b();
            parse_p();
            parse_a(); // rechtsrekursiver aufruf
        break;
        case TOK_SEMICOLON:
        case TOK_PAREN_CLOSE:
            // Epsilon-Produktion d.h. lookahead nicht in FIRST(a), daher muss es in FOLLOW(a) in gültigem Satz sein
            // Epsilon ist letztes Symbol der rechtsrekursiven Aufrufe
        break;
            
        default:
            printf("fehler in parse_a, is: %s \n", tok_type_tostring(lookahead->tok_type));
    }

}


// P --> V | "(" E ")" | U P
// FIRST(P) = id number ( - 
void parse_p() {

    switch (lookahead->tok_type) {
        case TOK_ID:
        case TOK_NUMBER:
            parse_v();
        break;
        case TOK_PAREN_OPEN:
            match(TOK_PAREN_OPEN);
            parse_expr();
            match(TOK_PAREN_CLOSE); 
        break;
        case TOK_MINUS:
            parse_u();
            parse_p(); 
        break;
            
        default:
            printf("fehler in parse_p"); 
    }
}


// B --> "+" | "-" | "*" | "/" | "^"
void parse_b() {

    switch (lookahead->tok_type) {
        case TOK_BINOP:
            match(TOK_BINOP);
        break;
        default:
            syntax_error(TOK_BINOP); 
    }
    
}


// U --> "-"
void parse_u() {
    switch (lookahead->tok_type) {
        case TOK_MINUS:
            match(TOK_MINUS);
        break;
        default:
            syntax_error(TOK_MINUS);
    }
}


// V = id | number
void parse_v() {

    switch (lookahead->tok_type) {
        case TOK_NUMBER:
            match(TOK_NUMBER);
        break;
        case TOK_ID:
            match(TOK_ID);
        break;
        default:
            printf("fehler bei parse_v");
    }

}


void test_tokens() {
    for (Token *token = scanner(); token != NULL; token = scanner()) {
        printf("typ: %s, lexem: %s\n", tok_type_tostring(token->tok_type), token->lexem_one);
    }
}


void match(int token) {
    
    if(lookahead->tok_type != token) {
        syntax_error(token);
    }
    consume_token(); 
    
}


Token *consume_token() {
    
    lookahead = scanner(); 
    return lookahead; 
    
}


void syntax_error(int token_expected) {
    
    printf("-- Syntaxerror at %i : %i --\n", lookahead->line, lookahead->row);
    printf("-- Message --\n");
    printf("Is: %s, Expected: %s\n", tok_type_tostring(lookahead->tok_type), tok_type_tostring(token_expected));
    exit(EXIT_FAILURE); 
    
}




/*
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "hash_table.h"
#include "scanner.h" 

int main(int argc, char *args[]) {

    file_init(); 
    char *word;
    printf("starte...\n"); 
    while ((word = next_word()) != NULL) {
        struct bucket *result = lookup(word);
        if(result == NULL) {
            result = (struct bucket *) malloc(sizeof(struct bucket));
        }
    }
    file_close(); 
    
    
    return 0; 
    
    char key[10]; 

    add("ro", "hier gehts um die stadt rosenheim");
    add("grhh", "inhalt für grossholzhausen"); 
    
    while (1) {
        printf("DB-Abfrage: \n"); 
        int result = scanf("%s", key); 
        if(result == 0) {
            printf("Falsches Format!\n");
        } else {
            struct bucket *result = lookup(key);  
            if(result == NULL) 
                printf("no result\n"); 
            else
                printf("Value for %s: %s\n", key, result->value);
        }
    }

}
 */
 



/*
 
 
#include <stdlib.h>
#include <stdio.h>
#include "array_list.h"
#include <string.h>


int main(int argc, const char *argv[]) {
    
    while (1) {
        
        char word[100]; 
        int count; 
        
        int result = scanf("%s %d", word, &count);
        if(result == 0) {
            fflush(stdin); 
            continue; 
        }
        if (strcmp(word, "info") == 0) {
            ;
        } else {
            char *word2 = strdup(word); 
            list_insert(word2, count);
        }
    }
}
 
 */




/*
#include <stdio.h>
#include "bintree.h"


void success(char *name) {
    printf("found: %s \n", name);
}

void error(char *bla) {
    printf("found nothing :(");
}

typedef void (*Callback)(char *param);

int main (int argc, const char * argv[]) {
    
    printf("starte...\n"); 
    Callback funcs[2] = { success, error }; 
    
    Ort ltk = { 8, "leutkirch" }; 
    Ort rbl = { 10, "raubling" };    
    Ort ro = { 6, "rosenheim" };
    Ort low = { 5, "low" };
    Ort lower = { 1, "lower" };
    
    
    printf("inserting...\n");
    insert(&ltk); 
    insert(&rbl);
    insert(&ro);
    insert(&low);
    insert(&lower);
    insert_by_properties(12, "new york"); 
    printf("searching...\n"); 
    
    Ort *result = find(12);
    if (result == NULL) {
        (*funcs[0])(NULL);
    } else {
        (*funcs[0])(result->name);
    }
    return 0;
}
*/
 