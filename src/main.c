#include "hash_table.h"

int main() {
    hash_table* ht = new_ht();
    delete_ht(ht);

    return 0;
}