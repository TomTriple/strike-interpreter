


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