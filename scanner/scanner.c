
#include <stdlib.h> 
#include <stdio.h> 
#include <ctype.h> 
#include <string.h>
#include "scanner.h" 


enum ScannerStates {
    SC_START, SC_IN_ID, SC_ID_END, SC_EQ_END, SC_BINOP_END, SC_IN_NUMBER, SC_NUMBER_END, 
    SC_SEMICOLON_END, SC_TERMINATE, SC_PAREN_OPEN, SC_PAREN_CLOSE
};

typedef unsigned State;


static char peek(); 
static char next_char(); 
static void concat_to_lexem(); 

static FILE *fp; 

static char c; 
static char *lexem;
static short end_seen = 0; 

static int line = 1; 
static int row; 


Token *scanner() {
    
    c = next_char(); 
    lexem = (char *)malloc(sizeof(char)); 
    State state = SC_START; 
    Token *token = (Token *) malloc(sizeof(Token)); 
    
    if(end_seen)
        return NULL; 
    
    while (1) {
        // printf("scannerdebug - lexem: %s, char: %c, state: %i\n", lexem, c, state); 
        switch (state) {
            case SC_START:
                if(isblank(c)) {
                    next_char();
                } else if(c == ';') {
                    concat_to_lexem(); 
                    state = SC_SEMICOLON_END;
                } else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
                    concat_to_lexem(); 
                    state = SC_IN_ID; 
                } else if(c == '=') {
                    concat_to_lexem(); 
                    state = SC_EQ_END; 
                } else if(c == '+' || c == '-' || c == '*' || c == '/') {
                    concat_to_lexem(); 
                    state = SC_BINOP_END; 
                } else if(isdigit(c)) {
                    state = SC_IN_NUMBER;
                } else if(c == EOF) {
                    state = SC_TERMINATE; 
                    end_seen = 1;
                } else if(c == '(') {
                    state = SC_PAREN_OPEN; 
                } else if(c == ')') {
                    state = SC_PAREN_CLOSE;
                }
                break;
            case SC_IN_ID: 
                if(c == ' ' || c == '=') {
                    ungetc(c, fp); 
                    state = SC_ID_END;
                } else if(isalpha(c)) { 
                    concat_to_lexem(); 
                    next_char();
                }
                break; 
            case SC_ID_END: 
                token->tok_type = TOK_ID; 
                token->lexem_one = lexem; 
                token->line = line; 
                token->row = row; 
                return token;
            case SC_EQ_END:
                token->tok_type = TOK_EQ; 
                token->line = line; 
                token->row = row; 
                return token; 
                break; 
            case SC_BINOP_END:
                token->tok_type = TOK_BINOP; 
                token->lexem_one = lexem; 
                token->line = line; 
                token->row = row; 
                return token; 
                break;
            case SC_IN_NUMBER:
                if(isdigit(c)) {
                    concat_to_lexem(); 
                    next_char(); 
                } else {
                    ungetc(c, fp); 
                    state = SC_NUMBER_END; 
                }
                break;
            case SC_NUMBER_END:
                token->tok_type = TOK_NUMBER;
                token->lexem_one = lexem; 
                token->line = line; 
                token->row = row; 
                return token; 
                break;
            case SC_SEMICOLON_END:
                token->tok_type = TOK_SEMICOLON; 
                token->line = line; 
                token->row = row; 
                return token; 
                break;
            case SC_TERMINATE:
                token->tok_type = TOK_TERMINATE; 
                token->line = line; 
                token->row = row; 
                return token; 
                break;
            case SC_PAREN_OPEN:
                token->tok_type = TOK_PAREN_OPEN;
                token->line = line;
                token->row = row; 
                return token;
            break;
            case SC_PAREN_CLOSE:
                token->tok_type = TOK_PAREN_CLOSE;
                token->line = line;
                token->row = row; 
                return token;
                break;
            default:
                printf("fall through...\n"); 
                break;
        }
    }
}


void init_scanner(char *sourcefile) {
    fp = fopen(sourcefile, "r");
    if (fp == NULL) {
        printf("file couldnt be opened...\n"); 
        exit(0); 
    }
}


char *tok_type_tostring(int tok_type) {
    
    char *result = malloc(100 * sizeof(char));
    
    switch (tok_type) { 
        case TOK_BINOP:
            result = "TOK_BINOP";
            break;
        case TOK_EQ:
            result =  "TOK_EQ";
            break;
        case TOK_ID:
            result =  "TOK_ID";
            break;
        case TOK_NUMBER:
            result =  "TOK_NUMBER";
            break;
        case TOK_SEMICOLON:
            result =  "TOK_SEMICOLON";
            break;
        case TOK_TERMINATE:
            result =  "TOK_TERMINATE";
            break; 
        case TOK_PAREN_OPEN:
            result =  "TOK_PAREN_OPEN";
            break; 
        case TOK_PAREN_CLOSE:
            result =  "TOK_PAREN_CLOSE";
            break;             
    }
    
    return result; 
}


static char peek() {
    char c = next_char(); 
    ungetc(c, fp); 
    return c;
}


static void concat_to_lexem() {
    strcat(lexem, &c);
}


static char next_char() {
    c = fgetc(fp);
    row++;
    if(c == EOF) {
        fclose(fp); 
    } else if(c == '\n') {
        line++; 
    }
    return c; 
}
