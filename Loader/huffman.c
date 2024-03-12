/*
    Huffman Implementation

    Author      => Abdallah Mohamed Elsharif
    Date        => 29-1-2024
*/

#include "huffman.h"

HuffmanTreeNode* huffman_calcfreq(unsigned char* data, size_t ulSize, __uint16_t* unpNumberOfNodes)
{
    HuffmanTreeNode* pNodes;
    Hash* htable;
    Key* pKey;
    Item* pItem;
    size_t ulCtr, ulIdx;

    if (ulSize == 0)
        return NULL;

    if (!(htable = hash_new(256, REPLACEMENT)))
        return NULL;

    // Iterate over all bytes to be inserted into a hash table with their frequency
    while (ulSize--)
    {
        if (pKey = hash_getkey(htable, data, sizeof(unsigned char)))
        {
            // Check if the frequency reached the maximum ( freq will be stored in only 4 bytes within the table )
            if ((ulCtr = *(__uint32_t*)hash_get(htable, pKey)) == 0xffffffff)
            {
                hash_destroy(&htable);
                return NULL;
            }

            ulCtr++;
            pItem = item_new(&ulCtr, sizeof(__uint32_t), malloc, free, NULL, NULL);
            hash_update(htable, pKey, pItem);
        }

        else {
            ulCtr = 1;
            pKey = key_new(data, sizeof(unsigned char), NULL, ByteCmp);
            pItem = item_new(&ulCtr, sizeof(__uint32_t), malloc, free, NULL, NULL);
            hash_insert(htable, pKey, pItem);
        }

        data++;
    }

    // Set the number of nodes
    (*unpNumberOfNodes) = hash_size(htable);

    // Allocate memory for the nodes
    if (!(pNodes = (HuffmanTreeNode*)malloc(*unpNumberOfNodes * sizeof(HuffmanTreeNode))))
    {
        hash_destroy(&htable);
        return NULL;
    }

    ulIdx = ulCtr = 0;

    while (hash_next(htable, &ulCtr, &pKey, &pItem))
    {
        (pNodes + ulIdx)->data = *(unsigned char*)pKey->data;
        (pNodes + ulIdx)->ulFrequency = *(__uint32_t*)hash_get(htable, pKey);
        (pNodes + ulIdx)->left = NULL;
        (pNodes + ulIdx)->right = NULL;
        ulIdx++;
    }

    hash_destroy(&htable);

    return pNodes;
}

HuffmanTreeNode* huffman_parsefreq(unsigned char* data, __uint16_t* unpNumberOfNodes)
{
    HuffmanTreeNode* pNodes;

    // Freq table size ( We start counting from 0, so we have to add one )
    *unpNumberOfNodes = (__uint16_t)(*data++) + 1;

    if (!(pNodes = malloc(*unpNumberOfNodes * sizeof(HuffmanTreeNode))))
        return NULL;

    for (__uint16_t unIdx = 0; unIdx < *unpNumberOfNodes; unIdx++)
    {
        pNodes[unIdx].data = ((HuffmanItem*)data)->data;
        pNodes[unIdx].ulFrequency = ((HuffmanItem*)data)->unFrequency;
        pNodes[unIdx].left = NULL;
        pNodes[unIdx].right = NULL;

        // Next freq entry
        data += sizeof(HuffmanItem);
    }

    return pNodes;
}

int huffman_cmpnode(void* x, void* y)
{
    return ULongCmp(
        &((HuffmanTreeNode*)x)->ulFrequency,
        &((HuffmanTreeNode*)y)->ulFrequency
    );
}

HuffmanTreeNode* huffman_newnode(HuffmanTreeNode* node)
{
    HuffmanTreeNode* hpNode;

    if (hpNode = malloc(sizeof(HuffmanTreeNode)))
    {
        hpNode->data = node->data;
        hpNode->ulFrequency = node->ulFrequency;
        hpNode->left = node->left;
        hpNode->right = node->right;
        return hpNode;
    }

    return NULL;
}

HuffmanTreeNode* huffman_build(HuffmanTreeNode* hNodes, __uint16_t unSize)
{
    Heap* htree;
    HuffmanTreeNode* hpLeftTreeNode, * hpRightTreeNode, * hpTreeNode;
    HuffmanTreeNode hLeftNode, hRightNode, hNode;

    if (!(htree = heap_init(unSize, sizeof(HuffmanTreeNode), MINHEAP, malloc, free, NULL, huffman_cmpnode)))
        return NULL;

    if (!heap_build(htree, hNodes, unSize))
    {
        heap_cleanup(&htree);
        return NULL;
    }

    while (heap_size(htree) > 1)
    {
        hLeftNode = *(HuffmanTreeNode*)heap_root(htree)->data;
        hpLeftTreeNode = huffman_newnode(&hLeftNode);
        heap_delete(htree, &hLeftNode);
        hRightNode = *(HuffmanTreeNode*)heap_root(htree)->data;
        hpRightTreeNode = huffman_newnode(&hRightNode);
        heap_delete(htree, &hRightNode);
        hNode.ulFrequency = hLeftNode.ulFrequency + hRightNode.ulFrequency;
        hNode.left = hpLeftTreeNode;
        hNode.right = hpRightTreeNode;
        hNode.data = 0x00;
        heap_insert(htree, &hNode);
    }

    hNode = *(HuffmanTreeNode*)heap_root(htree)->data;
    hpTreeNode = huffman_newnode(&hNode);
    heap_delete(htree, &hNode);
    heap_cleanup(&htree);

    return hpTreeNode;
}

void huffman_traverse2(HuffmanTreeNode* pRoot, Hash* pHuffmanTable, HuffmanCode c)
{
    if (!pRoot->left && !pRoot->right)
    {
        c.unFrequency = pRoot->ulFrequency;

        hash_insert(
            pHuffmanTable,
            key_new(&pRoot->data, 1, NULL, ByteCmp),
            item_new(&c, sizeof(HuffmanCode), malloc, free, NULL, NULL)
        );
    }

    else {
        c.ulLength++;

        if (pRoot->left)
        {
            c.unCode <<= 1;
            huffman_traverse2(pRoot->left, pHuffmanTable, c);
        }

        if (pRoot->right)
        {
            c.unCode += 1;
            huffman_traverse2(pRoot->right, pHuffmanTable, c);
        }
    }

}

Hash* huffman_traverse(HuffmanTreeNode* pRoot, __uint16_t unNumberOfNodes)
{
    Hash* pHuffmanTable;
    HuffmanCode c = { 0 };

    if (pHuffmanTable = hash_new(256, REPLACEMENT))
    {
        huffman_traverse2(pRoot, pHuffmanTable, c);
        return pHuffmanTable;
    }

    return NULL;
}

// This function calculates the encoded data length from the table
size_t huffman_calcenclen(Hash* pHuffmanTable)
{
    Item* pItem;
    HuffmanCode* pCode;
    size_t ulBitsSize, ulSize, ulPos;

    ulBitsSize = ulSize = ulPos = 0;

    while (hash_next(pHuffmanTable, &ulPos, NULL, &pItem))
    {
        pCode = (HuffmanCode*)pItem->data;
        ulBitsSize += pCode->ulLength * pCode->unFrequency;
    }

    // Padding the bits to be convertible to bytes
    if (ulBitsSize % 8 != 0)
        ulBitsSize += 8 - (ulBitsSize % 8);

    // Bits length in bytes
    ulBitsSize /= 8;

    // frequency table length
    ulSize += 1;

    // frequency table entries size
    ulSize += hash_size(pHuffmanTable) * sizeof(HuffmanItem);

    // Size of the codes 
    ulSize += ulBitsSize;

    return ulSize;
}

// This function calculates the original data length from the table
size_t huffman_calcdeclen(Hash* pHuffmanTable)
{
    HuffmanCode* pCode;
    Item* pItem;
    size_t ulSize, ulPos;

    ulSize = ulPos = 0;

    while (hash_next(pHuffmanTable, &ulPos, NULL, &pItem))
    {
        pCode = (HuffmanCode*)pItem->data;
        ulSize += pCode->unFrequency;
    }

    return ulSize;
}

unsigned char* huffman_encode(unsigned char* data, size_t ulSize, bool bForceEncoding, size_t* ulpResultSize)
{
    HuffmanTreeNode* pHuffmanNodes = NULL;
    HuffmanTreeNode* pHuffmanRoot = NULL;
    Hash* pHuffmanTable = NULL;
    unsigned char* pResult = NULL;
    unsigned char* pTempResult;
    Key* pKey;
    Item* pItem;
    HuffmanCode* pCode;
    __uint16_t unActive;
    __uint8_t unBitPos = 8;
    size_t ulBitsLen, ulPos = 0;

    if (!(pHuffmanNodes = huffman_calcfreq(data, ulSize, &unActive)))
        goto LEAVE;

    if (!(pHuffmanRoot = huffman_build(pHuffmanNodes, unActive)))
        goto LEAVE;

    if (!(pHuffmanTable = huffman_traverse(pHuffmanRoot, unActive)))
        goto LEAVE;

    if ((*ulpResultSize = huffman_calcenclen(pHuffmanTable)) >= ulSize && !bForceEncoding)
        goto LEAVE; // We f*cked up, the result size is greater than the orignal size itself.

    if (!(pTempResult = pResult = (unsigned char*)calloc(*ulpResultSize, sizeof(unsigned char))))
        goto LEAVE;

    // Build table headers (-1 because we store number of actives in one byte, if all bytes were active then one byte not enough and we will need extra space)
    *pTempResult++ = (__uint8_t)unActive - 1;

    while (hash_next(pHuffmanTable, &ulPos, &pKey, &pItem))
    {
        *pTempResult++ = *(unsigned char*)pKey->data;
        *((__uint32_t*)pTempResult) = ((HuffmanCode*)pItem->data)->unFrequency;
        pTempResult += sizeof(__uint32_t);
    }

    // Build bits table
    while (ulSize--)
    {
        // Retrieve huffman codes of current byte from the codes table 
        pKey = hash_getkey(pHuffmanTable, data, 1);
        pCode = (HuffmanCode*)hash_get(pHuffmanTable, pKey);
        ulBitsLen = pCode->ulLength;

        // Iterate over huffman codes of current byte
        while (ulBitsLen--)
        {
            unBitPos--;

            if ((pCode->unCode >> ulBitsLen) & 1)
                *pTempResult |= (1 << unBitPos);

            // Jump on the next byte within encoding buffer if the last bit in current byte had been set
            if (unBitPos == 0)
            {
                unBitPos = 8;
                pTempResult++;
            }

        }

        data++;
    }


LEAVE:
    if (pHuffmanNodes)
        free(pHuffmanNodes);

    if (pHuffmanRoot)
        huffman_cleanup(&pHuffmanRoot);

    if (pHuffmanTable)
        hash_destroy(&pHuffmanTable);

    return pResult;
}

unsigned char* huffman_decode(unsigned char* data, size_t ulSize, bool bForceDecoding, size_t* ulpResultSize)
{
    HuffmanTreeNode* pHuffmanNodes = NULL;
    HuffmanTreeNode* pHuffmanRoot = NULL;
    Hash* pHuffmanTable = NULL;
    unsigned char* pResult = NULL;
    __uint8_t* pBitsTable = NULL;
    unsigned char* pTempResult;
    __uint8_t* pTempBitsTable;
    size_t ulHeaderSize, ulBitsLen;
    __uint16_t unActive;

    if (!(pHuffmanNodes = huffman_parsefreq(data, &unActive)))
        goto LEAVE;

    // The table is invalid if the freq table size is greater than the full data
    if (unActive * sizeof(HuffmanItem) > ulSize)
        goto LEAVE;

    if (!(pHuffmanRoot = huffman_build(pHuffmanNodes, unActive)))
        goto LEAVE;

    if (!(pHuffmanTable = huffman_traverse(pHuffmanRoot, unActive)))
        goto LEAVE;

    if ((*ulpResultSize = huffman_calcdeclen(pHuffmanTable)) < ulSize && !bForceDecoding)
        goto LEAVE; // Why is the encoded data size greater than the original, motherf*cker !?!?

    // Calculate table headers size
    ulHeaderSize = 1 + (unActive * sizeof(HuffmanItem));

    // Calculate the length of bits
    ulBitsLen = (ulSize - ulHeaderSize) * 8;

    if (!(pTempBitsTable = pBitsTable = calloc(ulBitsLen, sizeof(__uint8_t))))
        goto LEAVE;

    if (!(pTempResult = pResult = malloc(*ulpResultSize)))
        goto LEAVE;

    // Pointing now to the huffman codes
    data += ulHeaderSize;

    // Read bits from the table
    for (size_t ulBitPos = ulBitsLen; ulBitPos--; )
    {
        // Check if the current bit is set
        if (*data & (1 << ulBitPos % 8))
            (*pTempBitsTable)++;

        // The end of current byte ( jump on the next one ) 
        if (ulBitPos % 8 == 0)
            data++;

        pTempBitsTable++;
    }

    // Back again to pointing to the beginning of the table
    pTempBitsTable = pBitsTable;

    // Decompress all bytes one by one
    for (size_t ulCtr = 0; ulCtr < *ulpResultSize; ulCtr++)
    {
        // Check if we deal with invalid table
        if (!(
            pTempBitsTable < pBitsTable + ulBitsLen &&
            huffman_lookup(pHuffmanRoot, pTempResult++, &pTempBitsTable)
            ))
        {
            free(pResult);
            pResult = NULL;
            break;
        }
    }

LEAVE:
    if (pHuffmanNodes)
        free(pHuffmanNodes);

    if (pHuffmanRoot)
        huffman_cleanup(&pHuffmanRoot);

    if (pHuffmanTable)
        hash_destroy(&pHuffmanTable);

    if (pBitsTable)
        free(pBitsTable);

    return pResult;
}

bool huffman_lookup(HuffmanTreeNode* pRoot, unsigned char* pResult, __uint8_t** pBitsTable)
{
    if (!pRoot->right && !pRoot->left)
    {
        *pResult = pRoot->data;
        return true;
    }

    if (*(*pBitsTable)++)
    {
        if (!pRoot->right)
            return false;

        huffman_lookup(pRoot->right, pResult, pBitsTable);
    }

    else {
        if (!pRoot->left)
            return false;

        huffman_lookup(pRoot->left, pResult, pBitsTable);
    }

}

void huffman_cleanup(HuffmanTreeNode** root)
{
    if ((*root)->left)
        huffman_cleanup(&(*root)->left);

    if ((*root)->right)
        huffman_cleanup(&(*root)->right);

    if (!(*root)->left && !(*root)->right)
    {
        free(*root);
        (*root) = NULL;
    }
}