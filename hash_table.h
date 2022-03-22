/* Structure of a key-value pair */
typedef struct {
    char* key;
    char* value;
} ht_item;


/* Structure of the hash table itself.
    Stores an array of pointers to items, 
    and data such as size and number of items
    it contains (count)  */
typedef struct {
    int size;
    int count;
    ht_item** items;
} ht_hash_table;


static ht_item* ht_new_item(const char* key, const char* value);
ht_hash_table* ht_new();
void ht_del_item(ht_item* item);
void ht_del_hash_table(ht_hash_table* ht);
static int ht_hash(const char* s, const int a, const int size);
static int ht_get_hash(const char* s, const int num_buckets, const int num_attempts);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);