#define SIZE 101 

struct bucket {
    struct bucket *next;
    char *key;
    void *value;
};

struct hstate {
    struct bucket *table[101];
};

typedef struct hstate HState;

void *hash_lookup(HState *state, char *key);  
void hash_add(HState *state, char *key, void *value);
HState *hash_new(void);


