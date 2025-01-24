#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"
#include "prime.h"

#define HT_INITIAL_BASE_SIZE 50
#define HT_PRIME_A 2161
#define HT_PRIME_B 2179

static ht_item DELETED_ITEM = {NULL, NULL};

static hash_table* ht_new_size(const int base_size) {
    hash_table* ht = malloc(sizeof(hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

static void ht_resize(hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    hash_table* new_hash_table = ht_new_size(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &DELETED_ITEM) {
            ht_insert(new_hash_table, item->key, item->value);
        }
    }
    ht->base_size = new_hash_table->base_size;
    ht->count = new_hash_table->count;

    //swap attributes
    const int temp_size = ht->size;
    ht->size = new_hash_table->size;
    new_hash_table->size = temp_size;

    ht_item** temp_items = ht->items;
    ht->items = new_hash_table->items;
    new_hash_table->items = temp_items;

    delete_ht(new_hash_table);
}

static void ht_resize_up(hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}

static void ht_resize_down(hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

static ht_item* new_item(const char* key, const char* value) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(key);
    i->value = strdup(value);
    return i;
}

hash_table* new_ht() {
/* Changed in 06-resizing
*
*    hash_table* ht = malloc(sizeof(hash_table));
*    ht->size = HT_INITIAL_BASE_SIZE;
*    ht->count = 0;
*    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
*    return ht;
*/
    return ht_new_size(HT_INITIAL_BASE_SIZE);
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
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    ht_item* item = new_item(key, value);
    int index = get_hash(item->key, ht->size, 0);
    ht_item* current_item = ht->items[index];

    int i = 1;
    while (current_item != NULL) {
        if (current_item != &DELETED_ITEM) {
            if (strcmp(current_item->key, key) == 0) {
                delete_item(current_item);
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
        }
    }
    return NULL;
}

void ht_delete(hash_table* ht, const char* key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize_down(ht);
    }

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
