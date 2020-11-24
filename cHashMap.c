#include "cHashMap.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HASHMAP_SIZE 8

Entry createEntryList(int listSize)
{
    Entry entryList = (Entry)calloc(listSize, sizeof (struct entry));

    return entryList;
}

// 创建哈希结构
HashMap createHashMap()
{
    HashMap hashMap = (HashMap)calloc(1, sizeof(struct hashMap));

    hashMap->size = 0;
    hashMap->listSize   = DEFAULT_HASHMAP_SIZE;
    hashMap->hashCode   = defaultHashCode;
    hashMap->equal      = defaultEqual;
    hashMap->list       = createEntryList(DEFAULT_HASHMAP_SIZE);
    hashMap->put        = defaultPut;
    hashMap->get        = defaultGet;
    hashMap->remove     = defaultRemove;
    hashMap->clear      = defaultClear;
    hashMap->exists     = defaultExists;
    hashMap->autoAssign = true;

    return hashMap;
}

static int defaultHashCode(HashMap hashMap, void * key)
{
    char* k = (char*)key;
    unsigned long h = 0;
    while (*k) {
        h = (h << 4) + *k++;
        unsigned long g = h & 0xF0000000L;
        if (g) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h % hashMap->listSize;
}

static bool defaultEqual(void *key1, void* key2)
{
    return strcmp((char*)key1, (char*)key2) ? true : false;
}

static void defaultPut(HashMap hashMap, void* key, void* value)
{
    int hashCode = hashMap->hashCode(hashMap, key);

    Entry entry = &hashMap->list[hashCode];

    if (entry->key == NULL) {
        entry->key = key;
        entry->value = value;
    } else {
        do {
            if (hashMap->equal(entry->key, key)) {
                entry->value = value;
                return;
            }

            entry = entry->next;
        } while (entry->next != NULL);

        Entry newEntry = createEntryList(1);
        newEntry->key = key;
        newEntry->value = value;
        newEntry->next = NULL;

        entry->next = newEntry;

    }

    hashMap->size++;

    if (hashMap->autoAssign && hashMap->size > hashMap->listSize) {
        resizeHashMap(hashMap, hashMap->listSize * 2);
    }
}

static void * defaultGet(HashMap hashMap, void * key)
{
    int hashCode = hashMap->hashCode(hashMap, key);

    Entry entry = &hashMap->list[hashCode];

    do {
        if (hashMap->equal(entry->key, key)) {
            return entry->value;
        }

        entry = entry->next;
    } while (entry->next != NULL);

    return NULL;
}
