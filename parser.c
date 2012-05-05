//
//  main.c
//  BinTree
//
//  Created by tom hoefer on 25.01.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
/*


http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm



Informal grammar:
+++++++++++++ 
S ::= STMT_LIST$
STMT_LIST ::= STMT STMT_LIST | EPSILON 
STMT ::= id = E;
STMT ::= p EX; | p "";
STMT ::= is EX ( stmt_list )

EX ::= E cmp E 
 

Expressions:
+++++++++++++
E --> P A
A -> BPA | Epsilon
P --> V | "(" E ")" | U P
B --> "+" | "-" | "*" | "/" | "^"
U --> "-"
V = id | number

FIRST(E) = id number ( - 
FIRST(A) = Epsilon + - * / ^
FOLLOW(A) = TOK_SEMICOLON, TOK_PAREN_OPEN, TOK_CMP  
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
#include "hash_table.h"

// constants associativity 
#define LEFT 0
#define RIGHT 1

enum types {
    AST_STMT_LIST, AST_ASSIGNMENT, AST_PRINT, AST_IS, AST_CMP
    };

struct Binary {
    short sentinel;
    Token *token; 
}; 


// the current lookahead-terminal
static Token *lookahead;


// nodes needed for evaluation of expressions
struct NExpr {
    struct Binary *operator; 
    struct NExpr *left;
    struct NExpr *right;
    Token *value;
    char *symkey;
};
struct NAssignment {
    Token *lvalue;
    int rvalue;
};


// AST nodes
typedef struct {
    int type;
    void *ref;
} Node;

typedef struct {
    Token *lvalue;
    struct NExpr *expr; 
} NodeAssign;

typedef struct {
    struct NExpr *expr;
} NodePrint;

typedef struct {
    Token *cmp;
    struct NExpr *expr1;
    struct NExpr *expr2;
    Node *then;
} NodeCmp;

typedef struct {
    QHandle *stmts;
} NodeStmtList;




void syntax_error(int tok);
Token *consume_token(void); 
void match(int token); 
void test_tokens(void); 

void parse_start(void);
void parse_stmt_list(NodeStmtList *stmt_list); 
void parse_assignment(NodeStmtList *stmt_list);
void parse_print(NodeStmtList *stmt_list); 
void parse_is(NodeStmtList *stmt_list);
void parse_expr(void);
void parse_a(void); 
void parse_p(void); 
void parse_b(void); 
void parse_u(void);
void parse_v(void); 
struct NExpr *shunting_yard(void); 
short prec_for_binop(struct Binary *binary);
short assoc_for_binop(struct Binary *binary); 
void stack_test_callback(void *it); 
struct NExpr *make_leaf(Token *tok, char *symkey); 
struct NExpr *make_node(struct Binary *binary, struct NExpr *e0, struct NExpr *e1); 
void push_operator(struct Binary *binary, SHandle *operators, SHandle *operands); 
void pop_operator(SHandle *operators, SHandle *operands);
struct Binary *make_binary(short is_sentinel, Token *token); 
int interpret_expr(struct NExpr *expr); 
void debug_expr_ast(struct NExpr *expr);
void interpret_node(Node *node); 



// toplevel statements 
Node *toplevel;

// state of the queue which is used as input for shunting yard
QHandle *expr_queue_state;

// symbol table 
HState *symtab;


int main(int argc, char **args) { 
    
    char *source = "/Users/tom/tom/NetBeansProjects/xcode/C-Progr/BinTree/BinTree/code2.txt";
    
    init_scanner(source);
    
    //test_tokens();
    // exit(0); 
    
    NodeStmtList *stmt_list = (NodeStmtList *) malloc(sizeof(NodeStmtList));
    stmt_list->stmts = queue_new();    
    toplevel = malloc(sizeof(Node));
    toplevel->type = AST_STMT_LIST;
    toplevel->ref = stmt_list;

    symtab = hash_new(); 
    consume_token();
    parse_start();
    printf("\nsyntax ok\n");

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




struct NExpr *shunting_yard() { 

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

    stack_push(operators, make_binary(1, NULL)); // == sentinel  
    input = queue_dequeue(expr_queue_state); 
    if(input->tok_type == TOK_NUMBER)
        stack_push(operands, make_leaf(input, NULL));
    else if(input->tok_type == TOK_ID)
        stack_push(operands, make_leaf(NULL, input->lexem_one));
    
    while((input = queue_dequeue(expr_queue_state)) != NULL) {         
        if(input->tok_type == TOK_BINOP) {
            push_operator(make_binary(0, input), operators, operands); 
            input = queue_dequeue(expr_queue_state); 
            if(input->tok_type == TOK_NUMBER)
                stack_push(operands, make_leaf(input, NULL)); 
            else if(input->tok_type == TOK_ID)
                stack_push(operands, make_leaf(NULL, input->lexem_one));
        }
    }
    
    struct Binary *top;
    while((top = stack_top(operators)) != NULL && top->sentinel != 1) { 
        pop_operator(operators, operands); 
    }
    
    struct NExpr *tos = stack_top(operands); 
    return (tos); 
    // printf("result: %i", interpret(tos));
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



int interpret_expr(struct NExpr *expr) {
    if(expr->value != NULL) {
        return atoi(expr->value->lexem_one); 
    } else if (expr->symkey != NULL) {
        int value = hash_lookup(symtab, expr->symkey);
        return value;
    } else {
        switch (expr->operator->token->lexem_one[0]) {
            case '+':
                return interpret_expr(expr->left) + interpret_expr(expr->right); 
                break;
            case '-':
                return interpret_expr(expr->left) - interpret_expr(expr->right); 
                break;
            case '*':
                return interpret_expr(expr->left) * interpret_expr(expr->right); 
                break;
            case '/':
                return interpret_expr(expr->left) / interpret_expr(expr->right); 
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


struct NExpr *make_leaf(Token *tok, char *symkey) {
    struct NExpr *leaf = (struct NExpr *) malloc(sizeof(struct NExpr));  
    if(NULL != tok) {
        leaf->value = tok; 
    } else if(NULL != symkey) {
        leaf->symkey = symkey; 
    }
    
    return leaf; 
}


struct NExpr *make_node(struct Binary *binary, struct NExpr *e0, struct NExpr *e1) {
    struct NExpr *node = (struct NExpr *) malloc(sizeof(struct NExpr));
    if(NULL == node) {
        printf("mem error make_node"); 
    }
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


void interpret_node(Node *node) {

    NodeStmtList *stmt_list;
    NodePrint *print;
    NodeAssign *assign;
    NodeCmp *cmp;
    Node *current;
    
    switch (node->type) {
        case AST_STMT_LIST:
            stmt_list = node->ref; 
            while ((current = queue_dequeue(stmt_list->stmts)) != NULL) {
                interpret_node(current);
            }
        break;
        case AST_PRINT:
            print = (NodePrint *) node->ref;
            printf("ast_print: %i \n", interpret_expr(print->expr)); 
        break;
        case AST_ASSIGNMENT:
            assign = node->ref;
            // hier rumgecaste besser verstehen!!!!!
            hash_add(symtab, assign->lvalue->lexem_one, interpret_expr(assign->expr));
            int result = hash_lookup(symtab, assign->lvalue->lexem_one);
            printf("ast_assignment: %i \n", result);
        break; 
        case AST_CMP:
            cmp = node->ref; 
            if(strcmp("==", cmp->cmp->lexem_one) == 0) {
                if (interpret_expr(cmp->expr1) == interpret_expr(cmp->expr2)) {
                    interpret_node(cmp->then); 
                }
            }
        break;
        default:
            break;
    } 
}


Node *generate_node(int type, void *ref) {
    Node *node = (Node *) malloc(sizeof(Node));
    if(NULL == node)
        printf("mem error at generate_node");
    node->type = type;
    node->ref = ref;
    return node; 
}


void parse_start() {
    parse_stmt_list(toplevel->ref);
    match(TOK_TERMINATE); 
    // AST creation finished! 
    interpret_node(toplevel); 
}



void parse_stmt_list(NodeStmtList *stmt_list) {
    switch (lookahead->tok_type) {
        case TOK_ID:
            parse_assignment(stmt_list);
            parse_stmt_list(stmt_list); 
            break;
        case TOK_P:
            parse_print(stmt_list);
            parse_stmt_list(stmt_list);
        break;
        case TOK_IS:
            parse_is(stmt_list);
            parse_stmt_list(stmt_list);
        break;
        case TOK_PAREN_CLOSE:
        case TOK_TERMINATE:
            
        break;
        default:
            printf("error, tok_type is %s \n", tok_type_tostring(lookahead->tok_type)); 
    }
}


void parse_is(NodeStmtList *stmt_list) {

    switch (lookahead->tok_type) {
        case TOK_IS:
            match(TOK_IS);
            expr_queue_state = queue_new();
            parse_expr();
            struct NExpr *expr1 = shunting_yard();
            Token *token_cmp = lookahead;
            match(TOK_CMP);
            expr_queue_state = queue_new();
            parse_expr();
            struct NExpr *expr2 = shunting_yard();
            match(TOK_PAREN_OPEN);
            
            NodeCmp *node_cmp = malloc(sizeof(NodeCmp));
            node_cmp->cmp = token_cmp;
            node_cmp->expr1 = expr1;
            node_cmp->expr2 = expr2;
            NodeStmtList *then_stmt_list = malloc(sizeof(NodeStmtList));
            then_stmt_list->stmts = queue_new();
            Node *then = generate_node(AST_STMT_LIST, then_stmt_list);
            node_cmp->then = then;
            Node *node = generate_node(AST_CMP, node_cmp); 
            parse_stmt_list(then_stmt_list); 
            queue_enqueue(stmt_list->stmts, node);
            match(TOK_PAREN_CLOSE);
        break;
        default:
            printf("error parse_is...");
    }
    
}


void parse_print(NodeStmtList *stmt_list) {
    switch (lookahead->tok_type) {
        case TOK_P:
            match(TOK_P);
            expr_queue_state = queue_new(); 
            parse_expr(); 
            struct NExpr *expr = shunting_yard(); 
            match(TOK_SEMICOLON); 
            NodePrint *node_print = (NodePrint *) malloc(sizeof(NodePrint)); 
            node_print->expr = expr; 
            Node *node = generate_node(AST_PRINT, node_print);
            queue_enqueue(stmt_list->stmts, node); 
            break; 
        default:
            break;
    }
}


void parse_assignment(NodeStmtList *stmt_list) {

    Token *lvalue;
    switch (lookahead->tok_type) {
        case TOK_ID:
            lvalue = lookahead; 
            match(TOK_ID); 
            match(TOK_EQ); 
            expr_queue_state = queue_new(); 
            parse_expr(); 
            struct NExpr *expr = shunting_yard();
            match(TOK_SEMICOLON);
            NodeAssign *node_assign = malloc(sizeof(NodeAssign));
            node_assign->expr = expr;
            node_assign->lvalue = lvalue;
            Node *node = generate_node(AST_ASSIGNMENT, node_assign);
            queue_enqueue(stmt_list->stmts, node);  
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
// FOLLOW(A) = TOK_SEMICOLON, TOK_PAREN_OPEN, TOK_CMP
void parse_a() {
        
    switch (lookahead->tok_type) {
        case TOK_BINOP:
            parse_b();
            parse_p();
            parse_a(); // rechtsrekursiver aufruf
        break;
        case TOK_SEMICOLON:
        case TOK_PAREN_OPEN:
        case TOK_CMP:
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


 