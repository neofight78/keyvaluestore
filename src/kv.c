#include <kv.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE 1

kv_t *kv_init(size_t capacity) {
    if (capacity == 0) return NULL;

    kv_t *table = malloc(sizeof(kv_t));
    if (table == NULL) {
        return NULL;
    }

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(capacity, sizeof(kv_entry_t));
    if (table->entries == NULL) {
        return NULL;
    }

    return table;
}

size_t hash(char *value, int capacity) {
    size_t hash = 0x13371337deadbeef;

    while(*value) {
        hash ^= *value;
        hash = hash << 8;
        hash += *value;

        value++;
    }

    return hash % capacity;
}

// fn kv_ut
// params:
//  - db:       a pointer to the db
//  - key:      a pointer to the key value
//  - value:    a pointer to the value itself
// returns: 0 if successful otherwise on
// error, returns -1, on table full returns -2
int kv_put(kv_t *db, char *key, char *value) {
    if(!db || !key || !value) return -1;

    size_t idx = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity - 1; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key && entry->key != (void*)TOMBSTONE && !strcmp(entry->key, key)) {
            char *new_value = strdup(value);
            if (!new_value) return -1;
            entry->value = new_value;
            return 0;
        }

        if (!entry->key || entry->key == (void*)TOMBSTONE) {
            char *new_key = strdup(key);
            char *new_value = strdup(value);
            if (!new_key || !new_value) {
                free(new_key);
                free(new_value);
                return -1;
            }
            entry->key = new_key;
            entry->value = new_value;
            db->count++;
            return 0;
        }
    }

    return -2;
}
