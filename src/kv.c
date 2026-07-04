#define _POSIX_C_SOURCE 200809L
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

    while (*value) {
        hash ^= *value;
        hash = hash << 8;
        hash += *value;

        value++;
    }

    return hash % capacity;
}

// fn kv_delete
// params:
//  - db:       a pointer to the db
//  - key:      a pointer to the key value
// returns: the index of the deletion
// -1 if not found
int kv_delete(kv_t *db, char *key) {
    if (!db || !key) return -1;

    size_t idx = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity - 1; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key == NULL) {
            return -1;
        }

        if (entry->key && entry->key != (void *)TOMBSTONE &&
            !strcmp(entry->key, key)) {
            free(entry->key);
            free(entry->value);
            db->count--;
            entry->key = (void *)TOMBSTONE;
            entry->value = NULL;
            return real_idx;
        }
    }

    return -1;
}

// fn kv_get
// params:
//  - db:       a pointer to the db
//  - key:      a pointer to the key value
// returns: the pointer to the key
// NULL if not found
char *kv_get(kv_t *db, char *key) {
    if (!db || !key) return NULL;

    size_t idx = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity - 1; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key == NULL) { // TODO: This isn't right surely?
            return NULL;
        }

        if (entry->key && entry->key != (void *)TOMBSTONE &&
            !strcmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

// fn kv_ut
// params:
//  - db:       a pointer to the db
//  - key:      a pointer to the key value
//  - value:    a pointer to the value itself
// returns: 0 if successful otherwise on
// error, returns -1, on table full returns -2
int kv_put(kv_t *db, char *key, char *value) {
    if (!db || !key || !value) return -1;

    size_t idx = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity - 1; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key && entry->key != (void *)TOMBSTONE &&
            !strcmp(entry->key, key)) {
            char *new_value = strdup(value);
            if (!new_value) return -1;
            free(entry->value);
            entry->value = new_value;
            return 0;
        }

        if (!entry->key || entry->key == (void *)TOMBSTONE) {
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

// fn kv_free
// params:
//  - db:       a pointer to the db
void kv_free(kv_t *db) {
    if (!db) return;

    for (size_t i = 0; i < db->capacity - 1; i++) {
        kv_entry_t *e = &db->entries[i];

        if (e->key && e->key != (void *)TOMBSTONE) {
            free(e->key);
            free(e->value);
            e->key = NULL;
            e->value = NULL;
            db->count--;
        }
    }

    free(db->entries);
    free(db);
}
