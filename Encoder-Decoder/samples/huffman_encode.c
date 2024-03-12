#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned char *readfile(char *cpFileName, size_t *ulpFileSize)
{
    FILE *fp;
    unsigned char *pBuffer;

    if ( !(fp = fopen(cpFileName, "rb")) )
        return NULL;

    fseek(fp, 0L, SEEK_END);
    *ulpFileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if ( (pBuffer = malloc(*ulpFileSize)) )
        fread(pBuffer, *ulpFileSize, 1, fp);

    fclose(fp);
    return pBuffer;
}

bool writefile(char *cpFileName, unsigned char *pBuffer, size_t ulSize)
{
    FILE *fp;

    if ( !(fp = fopen(cpFileName, "wb")) )
        return false;

    fwrite(pBuffer, ulSize, 1, fp);
    fclose(fp);
    return true;
}

void writeHexFormatToFile(unsigned char *data, size_t size, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for writing hex format");
        return;
    }

    // Write the payload array
    fprintf(fp, "#pragma once\n\n");
    fprintf(fp, "unsigned char payload[] =\n\"");
    for (size_t i = 0; i < size; ++i)
    {
        fprintf(fp, "\\x%02X", data[i]);
        if ((i + 1) % 16 == 0 && i != size - 1)
            fprintf(fp, "\"\n\"");
    }
    fprintf(fp, "\";\n\n"); // Close the payload string

    // Write the payload size
    fprintf(fp, "#define payload_size %zu\n", size);

    fclose(fp);
}

int main(int argc, char *argv[])
{
    unsigned char *pBuffer, *pEncodedData;
    size_t ulSize;
    bool bWritten;

    if ( argc <= 2 )
        return puts("Usage :\n\t./huffman_encode <original.txt> <encoded.bin>");

    if ( !(pBuffer = readfile(argv[1], &ulSize)) )
        return puts("[!] Failed to read the input file");

    printf("[*] Original data size   => %luB ( %luKB )\n", ulSize, ulSize/1024);
    pEncodedData = huffman_encode(pBuffer, ulSize, false, &ulSize);
    free( pBuffer );

    if ( ! pEncodedData )
        return puts("[!] Failed to compress the given file data");

    printf("[*] Compressed data size => %luB ( %luKB )\n", ulSize, ulSize/1024);
    bWritten = writefile(argv[2], pEncodedData, ulSize);
    
    // Write the encoded data in hex format to payload.h
    writeHexFormatToFile(pEncodedData, ulSize, "payload.h");

    free( pEncodedData );

    if ( ! bWritten )
        return puts("[!] Failed to write the output into the file");

    printf("[+] The data successfully compressed and has been written to '%s' and 'payload.h'\n", argv[2]);
    return 0;
}
