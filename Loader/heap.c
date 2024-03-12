/*
    Heap Tree Implementation

    Author      => Abdallah Mohamed Elsharif
    Date        => 24-1-2024
*/

#include "heap.h"

Heap* heap_init(size_t ulMaxSize, int nItemSize, HeapType type, void* (*allocate)(size_t), void (*deallocate)(void*), void (*print)(void*), int (*compare)(void*, void*))
{
    Heap* h;

    if (h = malloc(sizeof(Heap)))
    {
        h->ulMaxSize = ulMaxSize;
        h->ulSize = 0;
        h->nItemSize = nItemSize;
        h->type = type;
        h->allocate = allocate;
        h->deallocate = deallocate;
        h->print = print;
        h->compare = compare;

        if (h->nodes = (HNode**)malloc(sizeof(HNode*) * h->ulMaxSize))
            return h;

        free(h);
    }

    return NULL;
}

size_t heap_size(Heap* h)
{
    return h->ulSize;
}

int heap_empty(Heap* h)
{
    return heap_size(h) == 0;
}

int heap_full(Heap* h)
{
    return (heap_size(h) == h->ulMaxSize);
}

size_t heap_parentidx(Heap* h, size_t ulIdx)
{
    if (ulIdx < 1)
        return -1;

    return ((ulIdx - 1) / 2);
}

size_t heap_leftidx(Heap* h, size_t ulIdx)
{
    if ((ulIdx * 2) + 1 >= h->ulSize)
        return -1;

    return (2 * ulIdx + 1);
}

size_t heap_rightidx(Heap* h, size_t ulIdx)
{
    if ((ulIdx * 2) + 2 >= h->ulSize)
        return -1;

    return (2 * ulIdx + 2);
}

int heap_insert(Heap* h, void* item)
{
    HNode* node;
    size_t ulParentIdx;

    if (heap_full(h))
        return 0;

    if (node = heap_build_node(h, item))
    {

        h->nodes[h->ulSize++] = node;

        if ((ulParentIdx = heap_parentidx(h, h->ulSize - 1)) != -1)
            do {

                heap_heapify(h, ulParentIdx);

            } while (ulParentIdx--);


            return 1;
    }

    return 0;
}

int heap_delete(Heap* h, void* item)
{
    size_t ulIdx;

    if (heap_empty(h))
        return 0;

    if ((ulIdx = heap_find(h, item)) == -1)
        return 0;

    heap_destroy_node(h, h->nodes[ulIdx]);
    h->ulSize--;

    // Replace the target node with the last node
    heap_swap(h, ulIdx, h->ulSize);

    // Reheapify the tree
    if (h->ulSize && (ulIdx = heap_parentidx(h, h->ulSize - 1)) != -1)
        do {

            heap_heapify(h, ulIdx);

        } while (ulIdx--);

        return 1;
}

size_t heap_find(Heap* h, void* item)
{
    if (!heap_empty(h))
        return heap_find2(h, item, HROOT);

    return -1;
}

size_t heap_find2(Heap* h, void* item, size_t ulIdx)
{
    size_t ulChildIdx, ulRes;

    if (ulIdx >= h->ulSize)
        return -1;

    if (h->type == MINHEAP && h->compare(h->nodes[ulIdx]->data, item) > 0)
        return -1;

    if (h->type == MAXHEAP && h->compare(h->nodes[ulIdx]->data, item) < 0)
        return -1;

    if (h->compare(h->nodes[ulIdx]->data, item) == 0)
        return ulIdx;

    if ((ulChildIdx = heap_leftidx(h, ulIdx)) != -1)
    {
        if ((ulRes = heap_find2(h, item, ulChildIdx)) != -1)
            return ulRes;

        if ((ulChildIdx = heap_rightidx(h, ulIdx)) != -1)
            if ((ulRes = heap_find2(h, item, ulChildIdx)) != -1)
                return ulRes;
    }

    return -1;
}

HNode* heap_root(Heap* h)
{
    if (heap_empty(h))
        return NULL;

    return h->nodes[HROOT];
}

void heap_heapify(Heap* h, size_t ulIdx)
{
    size_t ulLeft;
    size_t ulRight;
    size_t ulItemIdx;

    ulItemIdx = ulIdx;
    ulLeft = heap_leftidx(h, ulIdx);
    ulRight = heap_rightidx(h, ulIdx);

    if (h->type == MINHEAP)
    {
        if (ulLeft != -1 && h->compare(h->nodes[ulLeft]->data, h->nodes[ulItemIdx]->data) < 0)
            ulItemIdx = ulLeft;

        if (ulRight != -1 && h->compare(h->nodes[ulRight]->data, h->nodes[ulItemIdx]->data) < 0)
            ulItemIdx = ulRight;

    }

    else if (h->type == MAXHEAP)
    {
        if (ulLeft != -1 && h->compare(h->nodes[ulLeft]->data, h->nodes[ulItemIdx]->data) > 0)
            ulItemIdx = ulLeft;

        if (ulRight != -1 && h->compare(h->nodes[ulRight]->data, h->nodes[ulItemIdx]->data) > 0)
            ulItemIdx = ulRight;

    }

    else
        return;

    // If the node wasn't in the correct position
    if (ulIdx != ulItemIdx)
    {
        heap_swap(h, ulIdx, ulItemIdx);
        heap_heapify(h, ulItemIdx);
    }

    // Set left and right nodes addresses
    if (ulLeft == -1)
    {
        h->nodes[ulIdx]->left = NULL;
        h->nodes[ulIdx]->right = NULL;
    }

    else {
        h->nodes[ulIdx]->left = h->nodes[ulLeft];

        if (ulRight == -1)
            h->nodes[ulIdx]->right = NULL;

        else
            h->nodes[ulIdx]->right = h->nodes[ulRight];

    }
}

void heap_swap(Heap* h, size_t ulNode1Idx, size_t ulNode2Idx)
{

    HNode* temp;

    temp = h->nodes[ulNode1Idx];
    h->nodes[ulNode1Idx] = h->nodes[ulNode2Idx];
    h->nodes[ulNode2Idx] = temp;
}

int heap_build(Heap* h, void* data, size_t ulSize)
{
    HNode* node;
    size_t ulIdx;

    if (ulSize > h->ulMaxSize)
        return 0;

    // Insert all items into the heap
    for (ulIdx = 0; ulSize--; ulIdx++)
    {
        if (!(node = heap_build_node(h, (void*)((__SIZE_TYPE__)data + ulIdx * h->nItemSize))))
            return 0;

        h->nodes[h->ulSize++] = node;
    }


    if ((ulIdx = heap_parentidx(h, h->ulSize - 1)) == -1)
        return 0;

    do {

        heap_heapify(h, ulIdx);

    } while (ulIdx--);

    return 1;
}

void heap_cleanup(Heap** h)
{
    heap_destroy_nodes(*h);
    free((*h)->nodes);
    free(*h);
    (*h) = NULL;
}

HNode* heap_build_node(Heap* h, void* item)
{
    HNode* node;

    if (node = malloc(sizeof(HNode)))
    {
        node->left = NULL;
        node->right = NULL;

        if (node->data = h->allocate(h->nItemSize))
        {
            memcpy(node->data, item, h->nItemSize);
            return node;
        }

        free(node);
    }

    return NULL;
}

void heap_destroy_nodes(Heap* h)
{
    HNode** temp = h->nodes;

    while (h->ulSize--)
    {
        heap_destroy_node(h, *temp);
        *temp = NULL;
        temp++;
    }
}

void heap_destroy_node(Heap* h, HNode* node)
{
    h->deallocate(node->data);
    free(node);
}