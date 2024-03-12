
#include "helpers.h"

void* GenericStrNew(void* pStr, int nSize)
{
    void* pNewStr = NULL;

    if (pNewStr = malloc(nSize))
        memcpy(pNewStr, pStr, nSize);

    return pNewStr;
}

void* CharNewA(char* c)
{
    return GenericStrNew(c, sizeof(char));
}

void* CharNewW(char* c)
{
    size_t lSize = sizeof(wchar_t);
    return GenericStrNew(WideStrFromAnsi(c, lSize), lSize);
}

void* StrNewA(char* cpStr)
{
    return GenericStrNew(cpStr, strlen(cpStr));
}

void* StrNewW(char* cpStr)
{
    size_t lSize = strlen(cpStr);
    return GenericStrNew(WideStrFromAnsi(cpStr, lSize), lSize);
}

int ByteCmp(void* x, void* y)
{
    return *((unsigned char*)x) - *((unsigned char*)y);
}

int CharCmpA(void* x, void* y)
{
    return *((char*)x) - *((char*)y);
}

int CharCmpW(void* x, void* y)
{
    return *((wchar_t*)x) - *((wchar_t*)y);
}

int ShortCmp(void* x, void* y)
{
    return *((short*)x) - *((short*)y);
}

int UShortCmp(void* x, void* y)
{
    return *((unsigned short*)x) - *((unsigned short*)y);
}

int IntCmp(void* x, void* y)
{
    return *((int*)x) - *((int*)y);
}

int UIntCmp(void* x, void* y)
{
    return *((unsigned int*)x) - *((unsigned int*)y);
}

int LongCmp(void* x, void* y)
{
    return *((long*)x) - *((long*)y);
}

int ULongCmp(void* x, void* y)
{
    return *((unsigned long*)x) - *((unsigned long*)y);
}

int GenericStrCmp(void* x, void* y, size_t lSizeX, size_t lSizeY, size_t nItemSize, int (*CharCmp)(void*, void*))
{
    void* pCharX, * pCharY;
    size_t lSize, lCtr;
    int nDelta;

    lSize = (lSizeX < lSizeY) ? lSizeX : lSizeY;
    lCtr = 0;

    while (lSize--)
    {
        // Calculate characters addresses
        pCharX = (void*)((__SIZE_TYPE__)x + lCtr * nItemSize);
        pCharY = (void*)((__SIZE_TYPE__)y + lCtr * nItemSize);
        lCtr++;

        if ((nDelta = CharCmp(pCharX, pCharY)) != 0)
            return nDelta;
    }

    return 0;
}

int StrCmpA(void* x, void* y)
{
    /* ( This implementation for comparing ansi strings, we can use a general function compares strings   )
    char c1, c2;
    int nSizeX, nSizeY, nSize, nDelta;

    x = (void *)(*(char **) x);

    nSizeX = strlen( (char *)x );
    nSizeY = strlen( (char *)y );
    nSize  = ( nSizeX < nSizeY ) ? nSizeX : nSizeY;

    while ( nSize-- )
    {
        c1 = *( (*((char **) &x))++ );
        c2 = *( (*((char **) &y))++ );

        if ( (nDelta = c1 - c2) != 0 )
            return nDelta;
    }
    */

    return GenericStrCmp(
        x,
        y,
        strlen((char*)x),
        strlen((char*)y),
        sizeof(char),
        CharCmpA
    );
}

int StrCmpW(void* x, void* y)
{
    return GenericStrCmp(
        x,
        y,
        wcslen((wchar_t*)x),
        wcslen((wchar_t*)y),
        sizeof(wchar_t),
        CharCmpW
    );
}

int StrPtrCmpA(void* x, void* y)
{
    // x is a pointer to the string pointer, so we dereference it
    return StrCmpA(*(void**)x, y);
}

int StrPtrCmpW(void* x, void* y)
{
    // x is a pointer to the string pointer, so we dereference it
    return StrCmpW(*(void**)x, y);
}

void BytePrint(void* x) { printf("0x%x", *(unsigned char*)x); }
void CharPrintA(void* x) { printf("%c", *(char*)x); }
void CharPrintW(void* x) { wprintf(L"%c", *(wchar_t*)x); }
void ShortPrint(void* x) { printf("%hi", *(short*)x); }
void UShortPrint(void* x) { printf("%hu", *(unsigned short*)x); }
void IntPrint(void* x) { printf("%d", *(int*)x); }
void UIntPrint(void* x) { printf("%u", *(unsigned int*)x); }
void LongPrint(void* x) { printf("%ld", *(long*)x); }
void ULongPrint(void* x) { printf("%lu", *(unsigned long*)x); }
void StrPrintA(void* x) { printf("\"%s\"", (char*)x); }
void StrPrintW(void* x) { wprintf(L"\"%s\"", (wchar_t*)x); }

wchar_t* WideStrFromAnsi(char* cpStr, size_t lSize)
{
    return NULL;
}

unsigned int calc_GCD(unsigned int a, unsigned int b)
{
    unsigned int shift, tmp;

    if (a == 0) return b;
    if (b == 0) return a;

    // Find power of two divisor
    for (shift = 0; ((a | b) & 1) == 0; shift++) { a >>= 1; b >>= 1; }

    // Remove remaining factors of two from a - they are not common
    while ((a & 1) == 0) a >>= 1;

    do
    {
        // Remove remaining factors of two from b - they are not common
        while ((b & 1) == 0) b >>= 1;

        if (a > b) { tmp = a; a = b; b = tmp; } // swap a,b
        b = b - a;
    } while (b != 0);

    return a << shift;
}

void PrintArray(void* data, size_t lSize, int nItemSize, void (*print)(void*))
{
    size_t lCtr = 0;

    printf("[ ");
    while (lSize--)
    {
        print((void*)((__SIZE_TYPE__)data + lCtr++ * nItemSize));

        if (lSize)
            printf(", ");
    }
    puts(" ]");
}

void PrintByteBits(unsigned char b)
{
    for (__uint8_t unBits = 8; unBits--; )
        printf("%c", (b & (1 << unBits)) ? '1' : '0');
}

void PrintBits(unsigned char* pBytes, size_t ulSize, bool bLittleEndian)
{
    if (bLittleEndian)
        pBytes += ulSize - 1;

    while (ulSize--)
    {
        PrintByteBits(*pBytes);

        if (ulSize % 8 == 0)
            printf("\n");
        else
            printf(" ");

        if (bLittleEndian)
            pBytes--;
        else
            pBytes++;
    }

}

void PrintArrayBits(unsigned char* pBytes, size_t ulArrSize, int nItemSize, bool bLittleEndian)
{
    while (ulArrSize--)
    {
        PrintBits(pBytes, nItemSize, bLittleEndian);
        pBytes += nItemSize;
    }
}