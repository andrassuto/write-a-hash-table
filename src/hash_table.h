
typedef struct ht_item {
    char* key;
    char* value;
} ht_item;

typedef struct hash_table {
    int size;
    int count;
    int base_size;
    ht_item** items;
} hash_table;


hash_table* new_ht();

void delete_ht(hash_table* ht); 

void ht_insert(hash_table* ht, const char* key, const char* value);

char* ht_search(hash_table* ht, const char* key);

void ht_delete(hash_table* ht, const char* key);
