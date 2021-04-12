/**
 * Author: Jake Betenson | U0624782
 * Write your solutions here and turn in just this file.
 * Please keep in mind that your solution must compile and work with the basic
 * tester and tests. Feel free to extend them, but we will have our
 * own edits to them and your assignment1.c needs to work correctly with the
 * original tests/tester.
 */
#define _POSIX_C_SOURCE 200809L // for strdup; it won't be part of C until 2023.
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "assignment1.h"

#define MAX_SIZE 16

typedef struct kvp {
    char *key, *value;
}kvp;

static struct kvp lru_cache[MAX_SIZE];


char* cache_get(const char* key) {
    //index thorugh cache look for key
    if (key == NULL)
        return NULL;
    for (int i = 0; i < MAX_SIZE; i = i + 1) {
        if ((lru_cache[i].key != NULL) && (strcmp(key, lru_cache[i].key) == 0)){
            //swap to head, shift remainder            
            kvp temp; 
            temp = lru_cache[i]; //copy to place at head later
            // shift everything up one position till front
            for (int j = i; j > 0; j = j-1) {                
                lru_cache[j] = lru_cache[j-1];                
            }
            //put temp at head
            lru_cache[0] = temp;       
            //return copy of lru_cache[i].value
            char *temp_return = malloc(strlen(lru_cache[0].value)+1);
            return strcpy(temp_return,  lru_cache[0].value); 
        }
    }
    //key not found, return null
    return NULL;
}

void cache_set(const char* key, const char* value) {
    //see if key exists, if it does not then insert at end
    //kvp temp; //for swapping
    //if its found
    for (int i = 0; i < MAX_SIZE; i = i + 1) {
        if ((lru_cache[i].key != NULL) && (strcmp(key, lru_cache[i].key) == 0)){
            //make copy and update
            kvp temp = lru_cache[i];
            char *temp_val = malloc(strlen(value)+1);
            temp.value = strcpy(temp_val, value);
            //move things backwards one space
            for (int j = i; j > 0; j = j-1) {
                lru_cache[j] = lru_cache[j-1];
            }
            //insert updated kvp at head
            lru_cache[0] = temp;
            return;
        }
    }
    //free last item if an eviction is required
    if (lru_cache[15].key != NULL) {
        free(lru_cache[15].key);
        free(lru_cache[15].value);
    }
    //move things up
    for (int i = MAX_SIZE-1; i > 0; i = i -1) {
        lru_cache[i] = lru_cache[i-1];
    }
    //insert new kvp at head
    char *temp_value = malloc(strlen(value) + 1);
    lru_cache[0].value = strcpy(temp_value, value);
    char *temp_key = malloc(strlen(key) + 1);
    lru_cache[0].key = strcpy(temp_key, key);
    return;
}

bool cache_del(const char* key) {
    for (int i = 0; i < MAX_SIZE; i = i + 1) {
        if (strcmp(key, lru_cache[i].key) == 0){
            //free allocated memory
            free(lru_cache[i].key);
            free(lru_cache[i].value);
            //TODO: move array around
            for (int j = i; j < MAX_SIZE-1; j = j +1) {
                lru_cache[j] = lru_cache[j+1];
            }
            if (lru_cache[MAX_SIZE-1].key != NULL) {
                lru_cache[MAX_SIZE-1].key = NULL;
                lru_cache[MAX_SIZE-1].value = NULL;
            }        
            return true;
        }
    }
    return false;
}

void cache_clear(void) {
    //index through entire array and free
    for (int i = 0; i < MAX_SIZE; i = i +1) {
        if (lru_cache[i].key != NULL) {
            free(lru_cache[i].key);
            free(lru_cache[i].value);
        }
    }
}
