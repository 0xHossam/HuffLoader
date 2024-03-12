#ifndef _dalgo_list
#define _dalgo_list

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
    void *data;
    size_t lSize;
    int nItemSize;
    size_t lMaxSize;
} List;


List *list_init(size_t lMaxSize, int nItemSize);
void list_cleanup(List **l);
size_t list_size(List *l);
int list_empty(List *l);
int list_full(List *l);
int list_insert(List *l, void *item);
int list_insertAt(List *l, size_t lIdx, void *item);
int list_updateAt(List *l, size_t lIdx, void *item);
int list_delete(List *l);
int list_deleteAt(List *l, size_t lIdx);
void *list_getitem(List *l);
void *list_getitemAt(List *l, size_t lIdx);
size_t list_search(List *l, void *item, int (* compare)(void *, void *));
void list_reverse(List *l);
void list_clear(List *l);
void list_print(List *l, void (*print)(void *));


#endif