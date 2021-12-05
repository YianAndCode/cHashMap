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

// 调整哈希结构大小
static void resizeHashMap(HashMap hashMap, int listSize)
{
    if (listSize < DEFAULT_HASHMAP_SIZE) {
        return;
    }

    int oldListSize = hashMap->listSize;
    Entry oldList   = hashMap->list;

    hashMap->listSize = listSize;
    hashMap->list = createEntryList(listSize);

    for (int i = 0; i < oldListSize; i++) {
        Entry entry = &oldList[i];

        while (entry != NULL) {
            // 插入到新的哈希结构
            hashMap->put(hashMap, entry->key, entry->value);

            Entry next = entry->next;

            // 释放内存
            free(entry);

            entry = next;
        }
    }

    free(oldList);
}

static int defaultHashCode(HashMap hashMap, void* key)
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
    if (key1 == NULL && key2 == NULL) {
        return true;
    }
    if (key1 == NULL || key2 == NULL) {
        return false;
    }
    return strcmp((char*)key1, (char*)key2) ? false : true;
}

static void defaultPut(HashMap hashMap, void* key, void* value)
{
    int hashCode = hashMap->hashCode(hashMap, key);

    Entry entry, preEntry;
    entry = preEntry = &hashMap->list[hashCode];

    if (entry->key == NULL) {
        entry->key = key;
        entry->value = value;
    } else {
        do {
            preEntry = entry;
            if (hashMap->equal(entry->key, key)) {
                entry->value = value;
                return;
            }

            entry = entry->next;
        } while (entry != NULL);

        Entry newEntry = createEntryList(1);
        newEntry->key = key;
        newEntry->value = value;
        newEntry->next = NULL;

        preEntry->next = newEntry;

    }

    hashMap->size++;

    if (hashMap->autoAssign && hashMap->size > hashMap->listSize) {
        resizeHashMap(hashMap, hashMap->listSize * 2);
    }
}

// 默认获取键对应值
static void * defaultGet(HashMap hashMap, void* key)
{
    int hashCode = hashMap->hashCode(hashMap, key);

    Entry entry = &hashMap->list[hashCode];

    do {
        if (hashMap->equal(entry->key, key)) {
            return entry->value;
        }

        entry = entry->next;
    } while (entry != NULL);

    return NULL;
}

// 默认删除键
static bool defaultRemove(HashMap hashMap, void* key)
{
    bool isDeleted = false;

    int hashCode = hashMap->hashCode(hashMap, key);

    Entry entry, preEntry;
    entry = preEntry = &hashMap->list[hashCode];

    int count = 0;

    do {
        count++;

        if (entry->key == NULL) {
            isDeleted = false;
            break;
        }

        if (hashMap->equal(entry->key, key)) {
            Entry nextEntry = entry->next;
            if (nextEntry == NULL) {
                /**
                 *  需要判断当前数组有几个元素，
                 *     如果只有 1 个，则直接将 key 和 value 置为 NULL
                 *     如果超过 1 个，则将前一个元素的 next 置为 NULL，同时 free 当前 entry
                 */
                 if (count == 1) {
                     entry->key   = NULL;
                     entry->value = NULL;
                 } else {
                     preEntry->next = NULL;

                     free(entry);
                 }
            } else {
                entry->key   = entry->next->key;
                entry->value = entry->next->value;
                entry->next  = entry->next->next;

                free(nextEntry);
            }

            hashMap->size--;

            isDeleted = true;
            break;
        }

        preEntry = entry;
        entry = entry->next;
    } while (entry != NULL);

    if (hashMap->autoAssign && hashMap->size < hashMap->listSize / 2) {
        // 缩容
        resizeHashMap(hashMap, hashMap->listSize / 2);
    }

    return isDeleted;
}

// 默认判断键是否存在
static bool defaultExists(HashMap hashMap, void* key)
{
    if (key == NULL) {
        return false;
    }

    int hashCode = hashMap->hashCode(hashMap, key);

    Entry entry = &hashMap->list[hashCode];

    while (entry != NULL) {
        if (hashMap->equal(entry->key, key)) {
            return true;
        }

        entry = entry->next;
    }

    return false;
}

// 默认清空Map
static void defaultClear(HashMap hashMap)
{
    for (int i = 0; i < hashMap->listSize; i++) {
        Entry entry = &hashMap->list[i];

        while (entry != NULL) {
            Entry next = entry->next;
            free(entry);

            entry = next;
        }
    }

    free(hashMap->list);

    hashMap->listSize = DEFAULT_HASHMAP_SIZE;
    hashMap->size     = 0;
    hashMap->list     = createEntryList(DEFAULT_HASHMAP_SIZE);
}
