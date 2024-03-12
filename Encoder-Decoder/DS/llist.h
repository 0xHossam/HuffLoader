#ifndef _dalgo_llist
#define _dalgo_llist

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Node LNode;

struct Node
{
    LNode *next;
    void *data;
    int nSize;
    void *(* allocate)(size_t);
    void (* deallocate)(void *);
    void (* print)(void *);
    int (* compare)(void *, void *);
};

typedef struct 
{
    LNode *first;
    LNode *last;
    size_t lSize;
} List;


List *llist_init();
size_t llist_size(List *l);
int llist_empty(List *l);
int llist_insert(List *l, void *item, int nItemSize, void *(* allocate)(size_t), void (* deallocate)(void *), void (* print)(void *), int (* compare)(void *, void *));
int llist_insertAtFirst(List *l, void *item, int nItemSize, void *(* allocate)(size_t), void (* deallocate)(void *), void (* print)(void *), int (* compare)(void *, void *));
int llist_insertAt(List *l, size_t lIdx, void *item, int nItemSize, void *(* allocate)(size_t), void (* deallocate)(void *), void (* print)(void *), int (* compare)(void *, void *));
int llist_updateAt(List *l, size_t lIdx, void *item);
void *llist_getitemAt(List *l, size_t lIdx);
int llist_delete(List *l);
int llist_deleteAtFirst(List *l);
int llist_deleteAt(List *l, size_t lIdx);
long llist_search(List *l, void *item, int nItemSize);
long llist_search2(List *l, void *item, int nItemSize, void **pReadItem);
void llist_reverse(List *l);
void llist_clear(List *l);
void llist_cleanup(List **l);
void llist_print(List *l);

LNode *build_node(void *item, int nItemSize, void *(* allocate)(size_t), void (* deallocate)(void *), void (* print)(void *), int (* compare)(void *, void *));
void destroy_node(LNode *node);


#endif