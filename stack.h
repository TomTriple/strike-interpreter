struct SElement {
    void *item;
    struct SElement *prev;
};

struct SState {
    unsigned end_seen;
    struct SElement *head, *iterator, *first; 
}; 

typedef struct SState SHandle;

struct SState *stack_new(void);
void *stack_pop(struct SState *state);
void stack_push(struct SState *state, void *item);
void *stack_top(struct SState *state); 
int stack_is_empty(struct SState *state);
void stack_test(struct SState *state, void (*callback)(void *item)); 