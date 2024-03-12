/*
    List Implementation
    
    Author      => Abdallah Mohamed Elsharif
    Date        => 12-2-2022
*/

#include "list.h"
#include "move.h"

List *list_init(size_t lMaxSize, int nItemSize)
{
    List *l = (List *) malloc( sizeof(List) );

    if ( l ) {
        l->lMaxSize = lMaxSize;
        l->lSize = 0;
        l->nItemSize = nItemSize;
        l->data = malloc( l->nItemSize * l->lMaxSize );

        if ( l->data )
            return l;

        /* Free handler */
        free(l);
        
    } 

    return NULL;
}

void list_cleanup(List **l)
{
    /* Free values first */
    free( (*l)->data );

    /* Then free handler */
    free( *l );

    /* Set ptr to null to avoid dangling */
    *l = NULL;
}

size_t list_size(List *l)
{
    return l->lSize;
}

int list_empty(List *l)
{
    return list_size(l) == 0;
}

int list_full(List *l)
{
    return list_size(l) == l->lMaxSize;
}

int list_insert(List *l, void *item)
{
    if ( list_full(l) )
        return 0;

    memcpy( (void *)((__SIZE_TYPE__) l->data + l->lSize++ * l->nItemSize), item, l->nItemSize );

    return 1;
}

int list_insertAt(List *l, size_t lIdx, void *item)
{
    if ( list_full(l) || lIdx < 0 || lIdx >= l->lMaxSize )
        return 0;

    /* Shift elements */
    for (size_t i = list_size(l); i > lIdx; i--)
        memmove(
            (void *)((__SIZE_TYPE__) l->data + i * l->nItemSize),
            (void *)((__SIZE_TYPE__) l->data + (i - 1) * l->nItemSize),
            l->nItemSize
        );
    
    memcpy( (void *)((__SIZE_TYPE__) l->data + lIdx * l->nItemSize), item, l->nItemSize );
    l->lSize++;

    return 1;
}

int list_updateAt(List *l, size_t lIdx, void *item)
{
    if ( lIdx < 0 || lIdx >= l->lMaxSize )
        return 0;

    memcpy( (void *)((__SIZE_TYPE__) l->data + lIdx * l->nItemSize), item, l->nItemSize );

    return 1;
}

int list_delete(List *l)
{
    if ( list_empty(l) )
        return 0;

    l->lSize--;

    return 1;
}

int list_deleteAt(List *l, size_t lIdx)
{
    if ( list_empty(l) || lIdx < 0 || lIdx >= list_size(l) )
        return 0;

    /* Shift elements */
    for (size_t i = lIdx; i < list_size(l); i++)
        memmove(
            (void *)((__SIZE_TYPE__) l->data + i * l->nItemSize),
            (void *)((__SIZE_TYPE__) l->data + (i + 1) * l->nItemSize),
            l->nItemSize
        );
    
    l->lSize--;

    return 1;
}

void *list_getitem(List *l)
{
    if ( list_empty(l) )
        return NULL;
        
    return (void *)((__SIZE_TYPE__) l->data + (l->lSize - 1) * l->nItemSize);
}

void *list_getitemAt(List *l, size_t lIdx)
{
    if ( list_empty(l) || lIdx < 0 || lIdx >= list_size(l))
        return NULL;

    return (void *)((__SIZE_TYPE__) l->data + lIdx * l->nItemSize);
}

size_t list_search(List *l, void *item, int (* compare)(void *, void *))
{
    for (size_t i = 0; i < list_size(l); i++)
        if ( compare(list_getitemAt(l, i), item) == 0 )
            return i;

    return -1;
    
}

void list_reverse(List *l)
{
    size_t lSize = list_size(l) - 1;

    for (size_t i = 0; i <= lSize / 2; i++)
        GSwap(
            (void *)((__SIZE_TYPE__) l->data + (lSize - i) * l->nItemSize),
            (void *)((__SIZE_TYPE__) l->data + i * l->nItemSize),
            l->nItemSize
        );
    
}

void list_clear(List *l)
{
    l->lSize = 0;
}

void list_print(List *l, void (*print)(void *))
{
    printf("[ ");

    for (size_t i = 0; i < list_size(l); i++)
    {
        print( list_getitemAt(l, i) );

        if ( i != list_size(l) - 1 )
            printf(", ");
    }

    puts(" ]");
    
}
