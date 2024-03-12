#ifndef _dalgo_heap
#define _dalgo_heap

#include <stdlib.h>
#include <string.h>

#define HROOT 0

typedef enum { MINHEAP, MAXHEAP } HeapType;
typedef struct _HNode HNode;

struct _HNode
{
    HNode *left;
    HNode *right;
    void *data;
};

typedef struct 
{
    HNode **nodes;
    size_t ulMaxSize;
    size_t ulSize;
    int nItemSize;
    HeapType type;
    void *(* allocate)(size_t);
    void (* deallocate)(void *);
    void (* print)(void *);
    int (* compare)(void *, void *);
} Heap;

Heap *heap_init(size_t ulMaxSize, int nItemSize, HeapType type, void *(* allocate)(size_t), void (* deallocate)(void *), void (* print)(void *), int (* compare)(void *, void *));
size_t heap_size(Heap *h);
int heap_empty(Heap *h);
int heap_full(Heap *h);
int heap_insert(Heap *h, void *item);
int heap_delete(Heap *h, void *item);
size_t heap_find(Heap *h, void *item);
size_t heap_find2(Heap *h, void *item, size_t ulIdx);
HNode *heap_root(Heap *h);
int heap_build(Heap *h, void *data, size_t ulSize);
void heap_heapify(Heap *h, size_t ulIdx);
void heap_swap(Heap *h, size_t ulNode1Idx, size_t ulNode2Idx);
void heap_cleanup(Heap **h);

HNode *heap_build_node(Heap *h, void *item);
void heap_destroy_nodes(Heap *h);
void heap_destroy_node(Heap *h, HNode *node);

#endif