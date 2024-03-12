#ifndef _dalgo_huffman
#define _dalgo_huffman

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heap.h"
#include "hash.h"
#include "helpers.h"

#ifdef _MSC_VER
#define PACKED(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))

#elif defined(__GNUC__)
#define PACKED(__Declaration__) struct __attribute__((packed)) __Declaration__

#endif


typedef struct _HuffmanTreeNode HuffmanTreeNode;

typedef struct _HuffmanCode
{
    size_t ulLength;
    __uint32_t unCode;
    __uint32_t unFrequency;
} HuffmanCode;

PACKED(
typedef struct _HuffmanItem
{
    unsigned char data;
    __uint32_t unFrequency;
} HuffmanItem;
)

struct _HuffmanTreeNode
{
    HuffmanTreeNode* left;
    HuffmanTreeNode* right;
    unsigned char data;
    unsigned long ulFrequency; // freq within the tree must be larger than "HuffmanItem" for unused items.
};

HuffmanTreeNode* huffman_calcfreq(unsigned char* data, size_t ulSize, __uint16_t* unpNumberOfNodes);
HuffmanTreeNode* huffman_parsefreq(unsigned char* data, __uint16_t* unpNumberOfNodes);
HuffmanTreeNode* huffman_build(HuffmanTreeNode* hNodes, __uint16_t unSize);
Hash* huffman_traverse(HuffmanTreeNode* pRoot, __uint16_t unNumberOfNodes);
void huffman_traverse2(HuffmanTreeNode* pRoot, Hash* pHuffmanTable, HuffmanCode c);
size_t huffman_calcenclen(Hash* pHuffmanTable);
size_t huffman_calcdeclen(Hash* pHuffmanTable);
unsigned char* huffman_encode(unsigned char* data, size_t ulSize, bool bForceEncoding, size_t* ulpResultSize);
unsigned char* huffman_decode(unsigned char* data, size_t ulSize, bool bForceDecoding, size_t* ulpResultSize);
bool huffman_lookup(HuffmanTreeNode* pRoot, unsigned char* pResult, __uint8_t** pBitsTable);
void huffman_cleanup(HuffmanTreeNode** root);

#endif