struct QElement {
    void *item;
    struct QElement *next;
}; 

struct QState {
    struct QElement *head, *last, *iterator;
}; 

typedef struct QState QHandle;

struct QState *queue_new(void); 
void queue_enqueue(struct QState *state, void *item); 
void *queue_dequeue(struct QState *state);
void queue_test(struct QState *state, void (*callback)(void *it));