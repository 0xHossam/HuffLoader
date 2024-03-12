/*
    Linked List Implementation

    Author      => Abdallah Mohamed Elsharif
    Date        => 12-2-2022
*/

#include "llist.h"

List* llist_init()
{
    List* l;

    if (l = malloc(sizeof(List)))
    {
        l->first = l->last = NULL;
        l->lSize = 0;
        return l;
    }

    return NULL;
}

size_t llist_size(List* l)
{
    return l->lSize;
}

int llist_empty(List* l)
{
    return !l->first || llist_size(l) == 0;
}

int llist_insert(List* l, void* item, int nItemSize, void* (*allocate)(size_t), void (*deallocate)(void*), void (*print)(void*), int (*compare)(void*, void*))
{
    LNode* node;

    if (node = build_node(item, nItemSize, allocate, deallocate, print, compare))
    {
        if (llist_empty(l))
            l->first = l->last = node;
        else {
            l->last->next = node;
            l->last = node;
        }

        l->lSize++;

        return 1;
    }

    return 0;
}

int llist_insertAtFirst(List* l, void* item, int nItemSize, void* (*allocate)(size_t), void (*deallocate)(void*), void (*print)(void*), int (*compare)(void*, void*))
{
    LNode* node;

    if (node = build_node(item, nItemSize, allocate, deallocate, print, compare))
    {
        if (llist_empty(l))
            l->first = l->last = node;
        else {
            node->next = l->first;
            l->first = node;
        }

        l->lSize++;

        return 1;
    }

    return 0;
}

int llist_insertAt(List* l, size_t lIdx, void* item, int nItemSize, void* (*allocate)(size_t), void (*deallocate)(void*), void (*print)(void*), int (*compare)(void*, void*))
{
    LNode* node, * curr;

    if (lIdx < 0 || lIdx > llist_size(l))
        return 0;

    if (lIdx == 0)
        return llist_insertAtFirst(l, item, nItemSize, allocate, deallocate, print, compare);

    else if (lIdx == llist_size(l))
        return llist_insert(l, item, nItemSize, allocate, deallocate, print, compare);

    else {
        /* Build the node and return if built failed */
        if (!(node = build_node(item, nItemSize, allocate, deallocate, print, compare)))
            return 0;

        /* Move to the item before the target */
        for (
            curr = l->first;
            --lIdx;
            curr = curr->next
            );

        node->next = curr->next;
        curr->next = node;
        l->lSize++;

        return 1;
    }
}

int llist_updateAt(List* l, size_t lIdx, void* item)
{
    LNode* node;

    if (lIdx < 0 || lIdx > llist_size(l))
        return 0;

    if (lIdx == 0)
        memcpy(l->first->data, item, l->first->nSize);

    else if (lIdx == llist_size(l) - 1)
        memcpy(l->last->data, item, l->last->nSize);

    else {
        /* move to the target node */
        for (
            node = l->first;
            lIdx--;
            node = node->next
            );

        memcpy(node->data, item, node->nSize);
    }

    return 1;
}

void* llist_getitemAt(List* l, size_t lIdx)
{
    LNode* node;

    if (lIdx < 0 || lIdx > llist_size(l))
        return NULL;

    if (lIdx == 0)
        return l->first->data;

    else if (lIdx == llist_size(l) - 1)
        return l->last->data;

    else {
        /* move to the target node */
        for (node = l->first; lIdx--; node = node->next);

        return node->data;
    }

}

int llist_delete(List* l)
{
    /* Delete the last item, return 1 if succeed else 0 */

    LNode* prevLast, * temp;
    size_t lSize;

    if (llist_empty(l))
        return 0;

    if (llist_size(l) == 1) {
        destroy_node(l->first);
        l->first = l->last = NULL;
        l->lSize--;

        return 1;
    }

    lSize = llist_size(l);

    /* move to the node before last one */
    for (prevLast = l->first; --lSize - 1; prevLast = prevLast->next);

    temp = l->last;
    l->last = prevLast;
    prevLast->next = NULL;
    destroy_node(temp);
    l->lSize--;

    return 1;
}

int llist_deleteAtFirst(List* l)
{
    /* Delete the first item, return 1 if succeed else 0 */

    LNode* temp;

    if (llist_empty(l))
        return 0;

    /* if there is only one item */
    if (llist_size(l) == 1) {
        destroy_node(l->first);
        l->first = l->last = NULL;
        l->lSize--;

        return 1;
    }

    temp = l->first;
    l->first = l->first->next;
    destroy_node(temp);
    l->lSize--;

    return 1;
}

int llist_deleteAt(List* l, size_t lIdx)
{
    LNode* prev, * temp;

    if (llist_empty(l) || lIdx >= llist_size(l))
        return 0;

    if (lIdx == 0)
        return llist_deleteAtFirst(l);

    else if (lIdx == llist_size(l) - 1)
        return llist_delete(l);

    else {
        /* Move to the node before target */
        for (
            prev = l->first;
            --lIdx;
            prev = prev->next
            );

        temp = prev->next; /* This is the target item */
        prev->next = temp->next;
        destroy_node(temp);
        l->lSize--;

        return 1;
    }
}

long llist_search(List* l, void* item, int nItemSize)
{
    /* return -1 if didn't find it or positon of the item */

    LNode* temp = l->first;

    if (llist_empty(l))
        return -1;

    for (size_t lPos = 0; temp; temp = temp->next, lPos++)
        if (nItemSize == temp->nSize && temp->compare(temp->data, item) == 0)
            return lPos;

    return -1;
}

long llist_search2(List* l, void* item, int nItemSize, void** pReadItem)
{
    /* return -1 if didn't find it or positon of the item */

    LNode* temp = l->first;

    if (llist_empty(l))
        return -1;

    for (size_t lPos = 0; temp; temp = temp->next, lPos++)
        if (nItemSize == temp->nSize && temp->compare(temp->data, item) == 0)
        {
            *pReadItem = temp->data;
            return lPos;
        }

    // Not found
    *pReadItem = NULL;

    return -1;
}

void llist_reverse(List* l)
{
    LNode* prev, * curr, * next;

    if (llist_empty(l) || llist_size(l) == 1)
        return;

    prev = NULL;
    curr = l->first;
    next = curr->next;

    while (next)
    {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    l->first = prev;

}

void llist_clear(List* l)
{
    while (!llist_empty(l))
    {
        /*
            (deleteAtFirst) is the best function to do that
            because its complexity is (Big O(1)) but (delete || deleteAt) is (Big O(n))
        */

        llist_deleteAtFirst(l);
    }
}

void llist_cleanup(List** l)
{
    /* Free all nodes */
    llist_clear(*l);

    /* Free list handler */
    free(*l);

    /* Set ptr to null to avoid dangling */
    (*l) = NULL;
}

void llist_print(List* l)
{
    LNode* node;

    printf("[ ");

    if (node = l->first)
    {
        do {
            node->print(node->data);
            if (node->next) printf(", ");
        } while (node = node->next);
    }

    printf(" ]");
}

LNode* build_node(void* item, int nItemSize, void* (*allocate)(size_t), void (*deallocate)(void*), void (*print)(void*), int (*compare)(void*, void*))
{
    LNode* node;

    if (node = malloc(sizeof(LNode)))
    {
        node->allocate = allocate;
        node->deallocate = deallocate;
        node->print = print;
        node->compare = compare;
        node->nSize = nItemSize;
        node->next = NULL;
        node->data = node->allocate(node->nSize);

        if (!node->data)
        {
            free(node);
            return NULL;
        }

        memcpy(node->data, item, node->nSize);

        return node;
    }

    return NULL;
}

void destroy_node(LNode* node)
{
    node->deallocate(node->data);
    free(node);
}
