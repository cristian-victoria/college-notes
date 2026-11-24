#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char ** items;
    size_t size;
} tokenlist;

char * get_input(void);                             // Reads user input line
tokenlist * get_tokens(char *input);                // Tokenizes input string 
tokenlist * new_tokenlist(void);                    // Creates a new tokenlist 
void add_token(tokenlist *tokens, char *item);      // Adds a token to the tokenlist
void free_tokens(tokenlist *tokens);                // Frees the tokenlist   