#ifndef _HASH_H_
#define _HASH_H_

typedef struct hashtable hashtable_t;
typedef unsigned int (*hash_fun_t)(void* key,int hashtable_size);

hashtable_t* hashtable_init(int buckets,hash_fun_t hash_fun);
void hashtable_destroy(hashtable_t** hash);
int hashtable_add(hashtable_t* hash,void* key,int key_size,void* val,int val_size);
int hashtable_del(hashtable_t* hash,void* key,int key_size);
void* hashtable_search(hashtable_t* hash,void* key,int key_size);


//测试使用
void hash_test();

#endif
