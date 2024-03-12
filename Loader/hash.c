/*
    Hash Table Implementation

    Author      => Abdallah Mohamed Elsharif
    Date        => 18-1-2024
*/

#include "hash.h"

Key* key_new(void* data, int nSize, void (*print)(void*), int (*compare)(void*, void*))
{
    Key* k;

    if (k = malloc(sizeof(Key)))
    {
        k->print = print;
        k->compare = compare;
        k->nSize = nSize;

        if (k->data = malloc(k->nSize))
        {
            memcpy(k->data, data, k->nSize);
            return k;
        }

        free(k);
    }

    return NULL;
}

int key_compare(Key* k, void* pKeyValue, int nKeySize)
{
    // Check if the key data type wasn't same
    if (k->nSize != nKeySize)
        return k->nSize - nKeySize;

    return k->compare(k->data, pKeyValue);
}

unsigned long key_hash(Key* k, size_t lMaxSize)
{
    return genhash(k->data, k->nSize, lMaxSize);
}

void key_destroy(Key** k)
{
    free((*k)->data);
    free(*k);
    (*k) = NULL;
}

Item* item_new(void* data, int nSize, void* (*allocate)(size_t), void (*deallocate)(void*), void (*print)(void*), int (*compare)(void*, void*))
{
    Item* i;

    if (i = malloc(sizeof(Item)))
    {
        i->allocate = allocate;
        i->deallocate = deallocate;
        i->print = print;
        i->compare = compare;
        i->nSize = nSize;

        if (i->data = i->allocate(i->nSize))
        {
            memcpy(i->data, data, i->nSize);
            return i;
        }

        free(i);
    }

    return NULL;
}

void item_destroy(Item** i)
{
    (*i)->deallocate((*i)->data);
    free(*i);
    (*i) = NULL;
}

HashNode* node_new(Key* k, Item* i)
{
    HashNode* node;

    if (node = malloc(sizeof(HashNode)))
    {
        node->key = k;
        node->item = i;
        return node;
    }

    return NULL;
}

// Callback for the list
int node_compare(void* node, void* key)
{
    HashNode* n;

    // We store the addresses of the nodes, so we dereferencing in this way
    n = *(HashNode**)node;

    // Compare the sizes of the keys
    if (n->key->nSize != ((Key*)key)->nSize) // if the types are not same
        return n->key->nSize - ((Key*)key)->nSize;

    return n->key->compare(n->key->data, ((Key*)key)->data);
}

void node_destroy(HashNode** node)
{
    free(*node);
    (*node) = NULL;
}

Hash* hash_new(size_t lMaxSize, HType type)
{
    Hash* h;

    if (h = malloc(sizeof(Hash)))
    {
        h->lMaxSize = lMaxSize;
        h->lSize = h->lCollisions = h->lNextItemIdx = 0;
        h->type = type;

        if (h->data = calloc(h->lMaxSize, sizeof(void*)))
            return h;

        free(h);
    }

    return NULL;
}

int hash_empty(Hash* h)
{
    return (hash_size(h) == 0);
}

int hash_insert(Hash* h, Key* k, Item* i)
{
    void** temp;
    HashNode* node;
    size_t lCtr;
    unsigned long ulHash;

    // Generate a hash from the key
    ulHash = key_hash(k, h->lMaxSize);

    if (h->type == REPLACEMENT)
    {
        if (!h->data[ulHash])
        {
            if (!(node = node_new(k, i)))
                return 0;

            h->lSize++;
        }
        else { // Collision case
            node = (HashNode*)h->data[ulHash];

            // Check if the key exist
            if (node_compare(&node, k) == 0)
                return 0; // To avoid manipulating an exist key

            // Remove the old key and item 
            key_destroy(&node->key);
            item_destroy(&node->item);

            // Replace with the new 
            node->key = k;
            node->item = i;

            h->lCollisions++;
        }

        // Insert the node into the table
        h->data[ulHash] = (void*)node;
    }

    else if (h->type == OPEN_ADDRESSING)
    {
        // Check if there are no available entries
        if (h->lSize == h->lMaxSize)
            return 0;

        // The index of the next entry to be read
        lCtr = 0;

        // Iterate over all entries to ensuring that the key doesn't exist
        while (hash_next(h, &lCtr, NULL, NULL))
            if (node_compare(h->data + lCtr - 1, k) == 0)
                return 0;

        // Pointing now to the appropriate entry
        temp = h->data + ulHash;

        // Check whether the appropriate entry is available or not
        if (*temp)
        {
            // This is a f*cking collision case
            h->lCollisions++;

            // Number of entries to the right of the appropriate entry
            lCtr = h->lMaxSize - 1 - ulHash;

            // Find an unused entry next to the appropriate entry
            while (lCtr-- && *(++temp));

            // WHAT THE FUCK IF ALL ENTRIES ON THE RIGHT WERE UNAVAILABLE ???
            if (lCtr == -1)
            {
                // Pointing now to the suitable entry
                temp = h->data + ulHash;

                // The index of the appropriate entry ( Number of entries to the left of it )
                lCtr = ulHash;

                // Search backward until find an available entry
                while (lCtr-- && *(--temp));
            }
        }

        // Create the node
        if (!(node = node_new(k, i)))
            return 0;

        // insert into the table
        (*temp) = (void*)node;

        h->lSize++;
    }

    else if (h->type == CHAINING)
    {
        // Initialize a list for this entry if was not initialized yet
        if (!h->data[ulHash])
            if (!(h->data[ulHash] = (void*)llist_init()))
                return 0;

        // Create a node
        if (!(node = node_new(k, i)))
            return 0;

        // Collision check
        if (!llist_empty((List*)h->data[ulHash]))
        {
            // Check if the given key was used before
            if (llist_search((List*)h->data[ulHash], (void*)k, sizeof(HashNode*)) != -1)
                return 0;

            h->lCollisions++;
        }

        if (!llist_insert(
            (List*)h->data[ulHash],
            (void*)&node, // Insert the address of the node, not the node itself
            sizeof(void*),
            malloc, // malloc and free are suitable for dealing with the node address (we don't need to develop custom allocators)
            free,
            NULL,  // We don't need to print the node.
            node_compare
        ))
            // Something goes wrong
            return 0;

        h->lSize++;
    }

    else // Unsupported type
        return 0;

    return 1;
}

int hash_update(Hash* h, Key* k, Item* i)
{
    void* pEntry;
    size_t lPos;
    unsigned long ulHash;

    // Calculate the hash from the key value
    ulHash = key_hash(k, h->lMaxSize);

    // Find the target entry
    if (pEntry = h->data[ulHash])
    {
        if (h->type == OPEN_ADDRESSING)
        {
            lPos = ulHash;

            // Find the node
            do {
                pEntry = h->data[lPos];

                if (pEntry && node_compare(&pEntry, (void*)k) == 0)
                    break;

            } while (++lPos != h->lMaxSize);

            // If the node was not on the right
            if (lPos == h->lMaxSize)
            {
                // Pointing now to the suitable entry
                lPos = ulHash;

                // Search backward
                while (lPos--)
                    if (pEntry = h->data[lPos])
                        if (node_compare(&pEntry, (void*)k) == 0)
                            break;

                if (lPos == -1) // Not found
                    return 0;

            }

        }

        else if (h->type == CHAINING)
        {
            // pEntry will be set at the end of the function, so it doesn't affect the behavior of the func
            if ((lPos = llist_search2((List*)pEntry, (void*)k, sizeof(HashNode*), &pEntry)) == -1)
                return 0; // Not found

            // We store the address of the node in the list, so we have to deref as follows
            pEntry = *(void**)pEntry;

        }

        // Destroy the old item
        item_destroy(&((HashNode*)pEntry)->item);

        // Set the new item
        ((HashNode*)pEntry)->item = i;

        return 1;
    }

    return 0;
}

size_t hash_size(Hash* h)
{
    return h->lSize;
}

void* hash_get(Hash* h, Key* k)
{
    Item* i;

    // Get the item by the key
    if (!(i = hash_get2(h, k)))
        return NULL;

    return i->data;
}

Item* hash_get2(Hash* h, Key* k)
{
    void* ent;
    size_t lPos;
    unsigned long ulHash;

    // Calculate the hash
    ulHash = key_hash(k, h->lMaxSize);

    // Find the target entry
    if (ent = h->data[ulHash])
    {
        if (h->type == REPLACEMENT)
            return ((HashNode*)ent)->item;

        else if (h->type == OPEN_ADDRESSING)
        {
            lPos = ulHash;

            // Find the node
            do {
                ent = h->data[lPos];

                if (ent && node_compare(&ent, (void*)k) == 0)
                    break;

            } while (++lPos != h->lMaxSize);

            // If the node was not on the right
            if (lPos == h->lMaxSize)
            {
                // Pointing now to the suitable entry
                lPos = ulHash;

                // Search backward
                while (lPos--)
                    if (ent = h->data[lPos])
                        if (node_compare(&ent, (void*)k) == 0)
                            break;

                if (lPos == -1) // Not found
                    return NULL;

            }

            return ((HashNode*)ent)->item;
        }

        else if (h->type == CHAINING)
        {
            // Find the node 
            // We must pass the node address size, not the key size because the search func expect a node type, not a key
            // However the comparison function can compare a node with a key 
            if ((lPos = llist_search2((List*)ent, (void*)k, sizeof(HashNode*), &ent)) == -1)
                return NULL; // Not found

            // We store the address of the node in the list, so we have to deref as follows
            ent = *(void**)ent;

            return ((HashNode*)ent)->item;
        }
    }

    return NULL;
}

Key* hash_getkey(Hash* h, void* pKeyValue, int nKeySize)
{
    void* pEntry;
    Key* pTempKey;
    size_t lPos;
    unsigned long ulHash;

    // Calculate the hash from the key value
    ulHash = genhash(pKeyValue, nKeySize, h->lMaxSize);

    // Find the target entry
    if (pEntry = h->data[ulHash])
    {
        if (h->type == REPLACEMENT)
        {
            if (key_compare(((HashNode*)pEntry)->key, pKeyValue, nKeySize) == 0)
                return ((HashNode*)pEntry)->key;
        }

        else if (h->type == OPEN_ADDRESSING)
        {
            lPos = ulHash;

            // Find the node
            do {
                pEntry = h->data[lPos];

                if (pEntry && key_compare(((HashNode*)pEntry)->key, pKeyValue, nKeySize) == 0)
                    break;

            } while (++lPos != h->lMaxSize);

            // If the node was not on the right
            if (lPos == h->lMaxSize)
            {
                // Pointing now to the suitable entry
                lPos = ulHash;

                // Search backward
                while (lPos--)
                    if (pEntry = h->data[lPos])
                        if (key_compare(((HashNode*)pEntry)->key, pKeyValue, nKeySize) == 0)
                            break;

                if (lPos == -1) // Not found
                    return NULL;

            }

            return ((HashNode*)pEntry)->key;
        }

        else if (h->type == CHAINING)
        {
            // Create a temp key from the value for search
            if (!(pTempKey = key_new(pKeyValue, nKeySize, NULL, NULL)))
                return NULL;

            if (llist_search2((List*)pEntry, (void*)pTempKey, sizeof(HashNode*), &pEntry) == -1)
            {
                // Remove the temp key
                key_destroy(&pTempKey);

                return NULL; // Not found
            }

            // Remove the temp key
            key_destroy(&pTempKey);

            // We store the address of the node in the list, so we have to deref as follows
            pEntry = *(void**)pEntry;

            return ((HashNode*)pEntry)->key;
        }
    }

    return NULL;
}

int hash_delete(Hash* h, Key* k)
{
    void** temp;
    void* ent;
    size_t lPos;
    unsigned long ulHash;

    // Calculate the hash
    ulHash = key_hash(k, h->lMaxSize);

    // Find the target entry
    if (ent = h->data[ulHash])
    {
        if (h->type == REPLACEMENT)
        {
            if (node_compare(&ent, (void*)k) != 0)
                return 0; // A different key

            key_destroy(&((HashNode*)ent)->key);
            item_destroy(&((HashNode*)ent)->item);
            node_destroy((HashNode**)h->data + ulHash);
            h->lSize--;

            return 1;
        }

        else if (h->type == OPEN_ADDRESSING)
        {
            // Pointing now to the suitable entry
            temp = h->data + ulHash;

            // Search from the hash index 
            lPos = ulHash;

            // Find the target node
            do {

                if (ent && node_compare(&ent, (void*)k) == 0)
                    break;

                // Next entry
                ent = *(++temp);

            } while (++lPos != h->lMaxSize);


            // If the node was not on the right
            if (lPos == h->lMaxSize)
            {
                // The position of the target entry
                lPos = ulHash;

                // Pointing now to the suitable entry
                temp = h->data + lPos;

                // Search backward
                while (lPos--)
                    if (ent = *(--temp))
                        if (node_compare(&ent, (void*)k) == 0)
                            break;

                if (lPos == -1) // Not found
                    return 0;

            }

            key_destroy(&((HashNode*)ent)->key);
            item_destroy(&((HashNode*)ent)->item);
            node_destroy((HashNode**)temp);
            h->lSize--;

            return 1;
        }

        else if (h->type == CHAINING)
        {
            size_t lIdx;

            // Find the node
            if ((lIdx = llist_search2((List*)ent, (void*)k, sizeof(HashNode*), &ent)) == -1)
                return 0; // We couldn't find the node

            // Deref the entry ( we have pushed the address of the entry into the list, and the list returns a ptr to our data )
            // That's why we do dereferencing as follows
            ent = *(void**)ent;

            // Cleanup the node
            key_destroy(&((HashNode*)ent)->key);
            item_destroy(&((HashNode*)ent)->item);
            node_destroy((HashNode**)&ent);

            // Delete the node from the list
            if (!llist_deleteAt((List*)h->data[ulHash], lIdx))
                return 0; // Something goes wrong when deleting the node from the list

            // If the list becomes empty, we have to cleanup the list and mark this entry as unused
            if (llist_empty((List*)h->data[ulHash]))
            {
                llist_cleanup((List**)h->data + ulHash);
                h->data[ulHash] = NULL;
            }

            h->lSize--;

            return 1;
        }

    }

    return 0;
}

int hash_next(Hash* h, size_t* ulpPos, Key** k, Item** i)
{
    void* entry;
    HashNode* node;

    // Check whether the given position is in the expected range or not
    if (hash_empty(h) || *ulpPos >= h->lMaxSize)
        goto FAIL;

    // Find the next available entry
    if (!(entry = h->data[*ulpPos]))
    {
        do {
            // Check if we reach the end of the table
            if (++(*ulpPos) == h->lMaxSize)
                goto FAIL;

            entry = h->data[*ulpPos];
        } while (!entry);
    }

    if (h->type == REPLACEMENT || h->type == OPEN_ADDRESSING)
    {
        if (k)
            *k = ((HashNode*)entry)->key;

        if (i)
            *i = ((HashNode*)entry)->item;

        // Next position 
        (*ulpPos)++;

        return 1;
    }

    else if (h->type == CHAINING)
    {

        // Get current node
        if (node = *(HashNode**)llist_getitemAt((List*)entry, h->lNextItemIdx))
        {
            if (k)
                *k = node->key;

            if (i)
                *i = node->item;

            // Set next item index    
            h->lNextItemIdx++;

            // If the next item reaches the end of the list, we have to reset it to zero, then jump on the next entry
            if (h->lNextItemIdx == llist_size((List*)entry))
            {
                h->lNextItemIdx = 0;
                (*ulpPos)++;
            }

            return 1;
        }

    }

FAIL:
    // Failed to get the node data
    if (k)
        *k = NULL;

    if (i)
        *i = NULL;

    return 0;
}

void hash_print(Hash* h)
{
    Item* pItem = NULL;
    Key* pKey = NULL;
    size_t lPos = 0;

    puts("{");

    while (hash_next(h, &lPos, &pKey, &pItem))
    {
        printf("\t");
        pKey->print(pKey->data);
        printf(": ");
        pItem->print(pItem->data);
        printf(",  // ( 0x%X )\n", key_hash(pKey, h->lMaxSize));
    }

    puts("}");
}

void hash_destroy(Hash** h)
{
    void** temp;
    Item* pItem = NULL;
    Key* pKey = NULL;
    size_t lPos = 0;

    if (!hash_empty(*h))
    {
        // Destroy all keys and items
        while (hash_next(*h, &lPos, &pKey, &pItem))
        {
            key_destroy(&pKey);
            item_destroy(&pItem);
        }

        temp = (*h)->data;

        // Deallocate all entries
        while ((*h)->lMaxSize--)
        {
            if (*temp)
            {
                if ((*h)->type == CHAINING)
                    llist_cleanup((List**)temp);

                else
                    node_destroy((HashNode**)temp);
            }

            // Next entry
            temp++;
        }
    }

    free((*h)->data);
    free(*h);
    (*h) = NULL;
}

unsigned long genhash(void* data, int nSize, size_t lMaxSize)
{
    unsigned long ulHash = 0;

    for (int nIdx = 0; nIdx < nSize; nIdx++)
        ulHash += *((unsigned char*)data + nIdx);

    return ulHash % lMaxSize;
}