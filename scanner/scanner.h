enum Tokens {
    TOK_ID, TOK_EQ, TOK_NUMBER, TOK_BINOP, TOK_SEMICOLON, TOK_TERMINATE, TOK_PAREN_OPEN, TOK_PAREN_CLOSE, TOK_MINUS, TOK_P, TOK_IS, TOK_CMP
};

typedef struct {
    int tok_type;
    int line;
    int row; 
    char *lexem_one;
    char *lexem_two;
} Token; 


Token *scanner(void); 
void init_scanner(char *source); 
char *tok_type_tostring(int tok_type);