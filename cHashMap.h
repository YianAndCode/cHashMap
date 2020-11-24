#include <stdbool.h>

#ifndef CHASHMAP_CHASHMAP_H
#define CHASHMAP_CHASHMAP_H

typedef struct entry {
    void * key;             // 键
    void * value;           // 值
    struct entry * next;    // 冲突链表
}*Entry;

// 创建哈希列表
Entry createEntryList(int listSize);

// 哈希结构
typedef struct hashMap *HashMap;

// 哈希函数类型
typedef int(*HashCode)(HashMap, void * key);

// 创建哈希结构
HashMap createHashMap();

// 重新构建
static void resizeHashMap(HashMap hashMap, int listSize);

// 判等函数类型
typedef bool(*Equal)(void * key1, void * key2);

// 添加键函数类型
typedef void(*Put)(HashMap hashMap, void * key, void * value);

// 获取键对应值的函数类型
typedef void * (*Get)(HashMap hashMap, void * key);

// 删除键的函数类型
typedef bool(*Remove)(HashMap hashMap, void * key);

// 清空Map的函数类型
typedef void(*Clear)(HashMap hashMap);

// 判断键值是否存在的函数类型
typedef bool(*Exists)(HashMap hashMap, void * key);

typedef struct hashMap {
    int size;           // 当前大小
    int listSize;       // 有效空间大小
    HashCode hashCode;  // 哈希函数
    Equal equal;        // 判等函数
    Entry list;         // 存储区域
    Put put;            // 添加键的函数
    Get get;            // 获取键对应值的函数
    Remove remove;      // 删除键
    Clear clear;        // 清空Map
    Exists exists;      // 判断键是否存在
    bool autoAssign;	// 设定是否根据当前数据量动态调整内存大小，默认开启
}*HashMap;

// 默认哈希函数
static int defaultHashCode(HashMap hashMap, void * key);

// 默认判断键值是否相等
static bool defaultEqual(void * key1, void * key2);

// 默认添加键值对
static void defaultPut(HashMap hashMap, void * key, void * value);

// 默认获取键对应值
static void * defaultGet(HashMap hashMap, void * key);

// 默认删除键
static bool defaultRemove(HashMap hashMap, void * key);

// 默认判断键是否存在
static bool defaultExists(HashMap hashMap, void * key);

// 默认清空Map
static void defaultClear(HashMap hashMap);

#endif//CHASHMAP_CHASHMAP_H
