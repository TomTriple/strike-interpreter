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

struct Binary {
    short sentinel;
    Token *token; 
}; 

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
short prec_for_binop(struct Binary *binary);
short assoc_for_binop(struct Binary *binary); 
void stack_test_callback(void *it); 
struct NExpr *make_leaf(Token *tok); 
struct NExpr *make_node(struct Binary *binary, struct NExpr *e0, struct NExpr *e1); 
void push_operator(struct Binary *binary, SHandle *operators, SHandle *operands); 
void pop_operator(SHandle *operators, SHandle *operands);
struct Binary *make_binary(short is_sentinel, Token *token); 
int interpret(struct NExpr *expr); 
void debug_expr_ast(struct NExpr *expr);





// the current lookahead-terminal
static Token *lookahead; 


// nodes for the AST

struct NExpr {
    struct Binary *operator; 
    struct NExpr *left;
    struct NExpr *right;
    Token *value;
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


short prec_for_binop(struct Binary *binary) {
    
    short result;
    
    if(binary->sentinel == 1)
        return 0;
    
    switch (binary->token->lexem_one[0]) {
        case '-':
        case '+':
            result = 10;
        break;
        case '*':
        case '/':
            result = 20;
        break; 
        default:
            printf("fehler, prec_for_binop: %c\n", binary->token->lexem_one[0]);
    }
    
    return result; 
}


short assoc_for_binop(struct Binary *binary) {

    short result; 
    
    if(binary->sentinel == 1)
        return LEFT;
    
    switch (binary->token->lexem_one[0]) {
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


    Token *input;
    SHandle *operators = stack_new();
    SHandle *operands = stack_new();

    stack_push(operators, make_binary(1, NULL));
    input = queue_dequeue(expr_queue_state); 
    stack_push(operands, make_leaf(input));
    
    while((input = queue_dequeue(expr_queue_state)) != NULL) {         
        if(input->tok_type == TOK_BINOP) {
            push_operator(make_binary(0, input), operators, operands); 
            input = queue_dequeue(expr_queue_state); 
            stack_push(operands, make_leaf(input)); 
        }
    }
    
    struct Binary *top;
    while((top = stack_top(operators)) != NULL && top->sentinel != 1) { 
        pop_operator(operators, operands); 
    }
    
    struct NExpr *tos = stack_top(operands); 
    printf("result: %i", interpret(tos));
    // debug_expr_ast(tos); 
    
}


struct Binary *make_binary(short is_sentinel, Token *token) {
    struct Binary *sentinel = (struct Binary *) malloc(sizeof(struct Binary)); 
    if(sentinel == NULL) {
        printf("no memory available"); 
    }
    sentinel->sentinel = is_sentinel;
    sentinel->token = token;
    return sentinel; 
}



int interpret(struct NExpr *expr) {
    if(expr->value != NULL) {
        return atoi(expr->value->lexem_one); 
    } else {
        switch (expr->operator->token->lexem_one[0]) {
            case '+':
                return interpret(expr->left) + interpret(expr->right); 
                break;
            case '-':
                return interpret(expr->left) - interpret(expr->right); 
                break;
            case '*':
                return interpret(expr->left) * interpret(expr->right); 
                break;
            case '/':
                return interpret(expr->left) / interpret(expr->right); 
                break;
        }
    }
}


void debug_expr_ast(struct NExpr *expr) {
    if(expr->value != NULL) {
        printf(expr->value->lexem_one); 
    } else {
        printf("(");
        debug_expr_ast(expr->left);
        printf(" %s ", expr->operator->token->lexem_one);
        debug_expr_ast(expr->right); 
        printf(")");
    }
}


struct NExpr *make_leaf(Token *tok) {
    struct NExpr *leaf = (struct NExpr *) malloc(sizeof(struct NExpr));  
    leaf->value = tok; 
    return leaf; 
}


struct NExpr *make_node(struct Binary *binary, struct NExpr *e0, struct NExpr *e1) {
    struct NExpr *node = (struct NExpr *) malloc(sizeof(struct NExpr));
    node->left = e0;
    node->right = e1; 
    node->operator = binary;
    return node;
}

    
void push_operator(struct Binary *binary, SHandle *operators, SHandle *operands) {
    struct Binary *top; 
    while (prec_for_binop((top = stack_top(operators))) > prec_for_binop(binary) || (assoc_for_binop(top) == LEFT && prec_for_binop(top) == prec_for_binop(binary))) {
        pop_operator(operators, operands); 
    }
    stack_push(operators, binary);
}


void pop_operator(SHandle *operators, SHandle *operands) {
    struct NExpr *expr1 = stack_pop(operands); 
    struct NExpr *expr2 = stack_pop(operands); 
    stack_push(operands, make_node(stack_pop(operators), expr2, expr1)); 
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


 