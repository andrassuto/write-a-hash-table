#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"

#define HT_INITIAL_SIZE 53
#define HT_PRIME_A 2161
#define HT_PRIME_B 2179


static ht_item* new_item(const char* key, const char* value) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(key);
    i->value = strdup(value);
    return i;
}

hash_table* new_ht() {
    hash_table* ht = malloc(sizeof(hash_table));
    ht->size = HT_INITIAL_SIZE;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

static void delete_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

void delete_ht(hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            delete_item(item);
        }  
    }
    free(ht->items);
    free(ht);
}

static int hash(const char* string, const int a, const int num_buckets) {
    long hash = 0;
    int stringLength = strlen(string);

    for (int i = 0; i < stringLength; i++) {
        hash += (long)pow(a, stringLength - (i+1)) * string[i];
        hash = hash % num_buckets;
    }
    return (int)hash;
}

static int get_hash(const char* string, const int num_buckets, const int attempts) {
    const int hash_a = hash(string, HT_PRIME_A, num_buckets);
    const int hash_b = hash(string, HT_PRIME_B, num_buckets);
    return (hash_a + (attempts * (hash_b + 1))) % num_buckets;
}

void ht_insert(hash_table* ht, const char* key, const char* value) {
    ht_item* item = new_item(key, value);
    int index = get_hash(item->key, ht->size, 0);
    ht_item* current_item = ht->items[index];

    int i = 1;
    while (current_item != NULL) {
        if (current_item != &DELETED_ITEM) {
            if (strcmp(current_item->key, key) == 0) {
                ht_del_item(current_item);
                ht->items[index] = item;
                return;
            }
        }
        index = get_hash(item->key, ht->size, i);
        current_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}


char* ht_search(hash_table* ht, const char* key) {
    int index = get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;

    // iterate through all non-empty buckets
    while (item != NULL) {
        if (item != &DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->key;
            }
            int index = get_hash(key, ht->size, i);
            item = ht->items[index];
            i++;
        } //TODO: check closing bracket location -- delete logic modification
    }
    return NULL; //item not found
}

static ht_item DELETED_ITEM = {NULL, NULL};

void ht_delete(hash_table* ht, const char* key) {
    int index = get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;

    //find item to be deleted and replace it with a pointer to DELETED_ITEM placeholder 
    while (item != NULL) {
        if (item != &DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                delete_item(item);
                ht->items[index] = &DELETED_ITEM;
            }
        }
        int index = get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    ht->count--;
}