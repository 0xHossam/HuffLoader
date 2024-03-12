/*
    Huffman Sample
    
    Author      => Abdallah Mohamed Elsharif
    Date        => 30-1-2024
    Compile     => gcc -I../DS/ -I../Algo/ ../Algo/helpers.c ../DS/heap.c ../DS/llist.c ../DS/hash.c ../Algo/huffman.c huffman.c -o huffman
*/

#include "huffman.h"

void print_codes2(HuffmanTreeNode *pRoot, __uint8_t unBits[], size_t ulIdx)
{
    if ( pRoot->left )
    {
        unBits[ ulIdx ] = 0;
        print_codes2( pRoot->left, unBits, ulIdx + 1 );
    }

    if ( pRoot->right )
    {
        unBits[ ulIdx ] = 1;
        print_codes2( pRoot->right, unBits, ulIdx + 1 );
    }

    if ( !pRoot->left && !pRoot->right )
    {
        printf( "   %c\t   %d\t\t", pRoot->data, pRoot->ulFrequency );

        for ( size_t i = 0; i < ulIdx; i++ )
            printf("%d", unBits[i]);

        printf("\n");
    }
}

void print_codes(HuffmanTreeNode *pRoot)
{
    __uint8_t unBits[1024];
    puts("---------------------------------");
    puts("Letter\tFrequency\tCodes");
    puts("---------------------------------");
    print_codes2( pRoot, unBits, 0 );
}

int main()
{
    HuffmanTreeNode *pHuffmanNodes, *pHuffmanRoot;
    unsigned char data[] = "BCCADBBDABBCCADDCCEE";
    __uint16_t unSize = 0;

    pHuffmanNodes = huffman_calcfreq(data, sizeof(data) - 1, &unSize);

    if ( !(pHuffmanRoot = huffman_build(pHuffmanNodes, unSize)) )
        return 1;

    print_codes( pHuffmanRoot );
    huffman_cleanup( &pHuffmanRoot );

    return 0;
}