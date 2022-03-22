#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hash_table.h"


#define HT_PRIME_1 227
#define HT_PRIME_2 229


/**
 * The initialization function for ht_item 's.
 * The function is marked as static since it will only
 * ever be called by code internal to the hash table.
 * @param: Key and value for initialization
 * @return: Pointer to the initialized ht_item
 */
static ht_item* ht_new_item(const char* key, const char* value) {
    ht_item* ht_new = malloc(sizeof(ht_item));  /* ht_new is cast automatically */
    ht_new->key = strdup(key);
    ht_new->value = strdup(value);
    return ht_new;
}


/**
 * The initialization function for ht_hash_table 's.
 * @return: Pointer to the initialized ht_hash_table
 */
ht_hash_table* ht_new() {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht->size = 53;  /* Temporary, until I learn how to resize */
    ht->count = 0;  /* An initialized ht_hash_table has no items */

    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}


/**
 * Destructor function for ht_item 's.
 */
void ht_del_item(ht_item* item) {
    free(item->key);
    free(item->value);
    free(item);
}


/**
 * Destructor function for ht_hash_table 's.
 */
void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; ++i) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}


/**
 * Standard hash function.
 * @param: The value (in string form) that needs to be hashed,
 * a prime number 'a' that is larger than ASCII-printable characters,
 * and the size of the hash table.
 * @return: An index in the range (0, size)
 */
static int ht_hash(const char* s, const int a, const int size) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; ++i) {
        hash += (long)pow(a, len_s - i + 1) * s[i];
        hash %= size;
    }
    return (int)hash;
}


/**
 * Double hashing so collisions can be handled.
 * @param: The value (in string form) that needs to be hashed,
 * a prime number 'a' that is larger than ASCII-printable characters,
 * and the size of the hash table.
 * @return: A key in the range (0, size)
 */
static int ht_get_hash(const char* s, const int num_buckets, const int num_attempts) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + num_attempts * (hash_b + 1)) % num_buckets;
}


/* A global sentinel item used to mark deleted
 * entries in a hash table.
 * Purpose: Usage in the ht_delete method in order to mark items
 * as deleted, since setting an item to NULL will break the
 * collision chain, making items further down the chain impossible
 * to find using ht_search or ht_delete.
 */
static ht_item HT_DELETED_ITEM = {NULL, NULL};


/**
 * An insert method for the hash table.
 * @param: The table itself, a key, a value.
 */
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        if (strcmp(cur_item->key, key) == 0) {
            ht_del_item(cur_item);
            ht->items[index] = item;
            return;
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        ++i;
    }
    ht->items[index] = item;
    ht->count++;
}


/**
 * A search method for the hash table.
 * @param: The table itself and a key
 */
char* ht_search(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;

    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        ++i;
    }
    return NULL;
}


/**
 * A deletion method for the hash table.
 * @param: The table itself and a key
 */
void ht_delete(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        ++i;
    }
    ht->count--;
}