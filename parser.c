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
#include "queue.h"
#include "stack.h"
#include <string.h> 
#include <assert.h> 

#define LEFT 0
#define RIGHT 1

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
void shunting_yard(void); 
short prec_for_binop(Token *token); 
short assoc_for_binop(Token *token); 
void stack_test_callback(void *it); 



// the current lookahead-terminal
static Token *lookahead; 


// nodes for the AST

struct NExpr {
    Token *operator; 
    struct NExpr *expr_left;
    struct NExpr *expr_right;
    Token *value_left;
    Token *value_right; 
};

struct NAssignment {
    Token *lvalue;
    struct NExpr *rvalue;
};


// root-node of the program
struct NAssignment root;

// state of the queue which is used as input for shunting yard
QHandle *expr_queue_state;




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


short prec_for_binop(Token *token) {
    
    short result;
    
    switch (token->lexem_one[0]) {
        case '-':
        case '+':
            result = 10;
        break;
        case '*':
        case '/':
            result = 20;
        break; 
        default:
            printf("fehler, prec_for_binop: %c\n", token->lexem_one[0]);
    }
    
    return result; 
}


short assoc_for_binop(Token *token) {

    short result;
    
    switch (token->lexem_one[0]) {
        case '-':
        case '+':
        case '/':
        case '*':            
            result = LEFT;
        break;
        case '^':
            result = RIGHT;
        break; 
        default:
            printf("fehler, assoc_for_binop\n"); 
    }
    
    return result; 

}


void shunting_yard() {
    
    Token *input;
    Token *top; 
    QHandle *output = queue_new(); 
    SHandle *ops = stack_new();

    /*
    // TEST-SUITE, Queue 
    
    queue_enqueue(output, "tom"); 
    queue_enqueue(output, "franz");     
    queue_enqueue(output, "hoefer");
    
    assert(strcmp(queue_dequeue(output), "tom") == 0);
    assert(strcmp(queue_dequeue(output), "franz") == 0); 
    assert(strcmp(queue_dequeue(output), "hoefer") == 0);
    assert(queue_dequeue(output) == NULL); 
    assert(queue_dequeue(output) == NULL); 
    assert(queue_dequeue(output) == NULL);     
    
    queue_enqueue(output, "munich");
    queue_enqueue(output, "new york");
    assert(queue_dequeue(output) == NULL);
    assert(queue_dequeue(output) == NULL);
    
    printf("queue ok \n"); 
    return; 
    */ 
    

    /*
    // TEST-SUITE, STACK
     
    stack_push(ops, "test"); 
    stack_push(ops, "tom");
    stack_push(ops, "hoefer"); 
    
    
    assert(stack_is_empty(ops) == 0);
    
    assert(strcmp(stack_top(ops), "hoefer") == 0);
    assert(strcmp(stack_pop(ops), "hoefer") == 0);
    assert(strcmp(stack_pop(ops), "tom") == 0);
    assert(strcmp(stack_top(ops), "test") == 0);
    stack_push(ops, "a");

    stack_push(ops, "b");    
    assert(strcmp(stack_top(ops), "b") == 0);
    assert(strcmp(stack_pop(ops), "b") == 0);
    stack_pop(ops);
    assert(stack_is_empty(ops) == 0);
    assert(strcmp(stack_pop(ops), "test") == 0);
    stack_pop(ops);

    stack_pop(ops);
    assert(stack_is_empty(ops) == 1);

    
    printf("stack ok\n"); 
    return; 
    */
     

    
    while ((input = queue_dequeue(expr_queue_state)) != NULL) { 
        switch (input->tok_type) {
            case TOK_BINOP:

                top = stack_top(ops);
                if(top == NULL) {
                    stack_push(ops, input);
                    continue;
                }
                while((assoc_for_binop(input) == LEFT && prec_for_binop(input) <= prec_for_binop(top)) || 
                      (assoc_for_binop(input) == RIGHT && prec_for_binop(input) < prec_for_binop(top))) {
                    queue_enqueue(output, stack_pop(ops)); 
                    top = stack_top(ops);
                    if(top == NULL) 
                        break; 
                } 
                stack_push(ops, input); 
                
            break;
            case TOK_NUMBER:
            case TOK_ID: 
                queue_enqueue(output, input);
            break;
            default:
                printf("fehler shunting_yard...");
        }
    }
    
    Token *remaining_binop;
    while ((remaining_binop = stack_pop(ops)) != NULL) {
        queue_enqueue(output, remaining_binop); 
    } 
    // rpn representation is finished here:    
    queue_test(output, stack_test_callback);
    
    return;
    
    // convert rpn to ast:    
    SHandle *result = stack_new();
    struct NExpr *expr;
    Token *token, *t1, *t2;
    
    while ((token = queue_dequeue(output)) != NULL) {
        if(token->tok_type == TOK_BINOP) { 
            expr = malloc(sizeof(struct NExpr));
            expr->value_right = stack_pop(result);
            expr->value_left = stack_pop(result);
            expr->operator = token;
            
        } else {
            stack_push(result, token);
        }
    }

}


void stack_test_callback(void *it) {

    Token *token = it;
    printf("token: %s, lexem: %s\n", tok_type_tostring(token->tok_type), token->lexem_one); 
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
            root.lvalue = lookahead;
            match(TOK_ID); 
            match(TOK_EQ); 
            expr_queue_state = queue_new(); 
            parse_expr(); 
            shunting_yard();
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
            queue_enqueue(expr_queue_state, lookahead); 
            match(TOK_PAREN_OPEN);
            parse_expr();
            queue_enqueue(expr_queue_state, lookahead); 
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
            queue_enqueue(expr_queue_state, lookahead); 
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
            queue_enqueue(expr_queue_state, lookahead); 
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
            queue_enqueue(expr_queue_state, lookahead); 
            match(TOK_NUMBER);
        break;
        case TOK_ID:
            queue_enqueue(expr_queue_state, lookahead); 
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


 