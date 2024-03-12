#ifndef _dalgo_helpers
#define _dalgo_helpers

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>

void *GenericStrNew(void *pStr, int nSize);
void *CharNewA(char *c);
void *CharNewW(char *c);
void *StrNewA(char *pStr);
void *StrNewW(char *pStr);

int ByteCmp(void *x, void *y);
int CharCmpA(void *x, void *y);
int CharCmpW(void *x, void *y);
int ShortCmp(void *x, void *y);
int UShortCmp(void *x, void *y);
int IntCmp(void *x, void *y);
int UIntCmp(void *x, void *y);
int LongCmp(void *x, void *y);
int ULongCmp(void *x, void *y);
int GenericStrCmp(void *x, void *y, size_t lSizeX, size_t lSizeY, size_t nItemSize, int (* CharCmp)(void *, void *));
int StrCmpA(void *x, void *y);
int StrCmpW(void *x, void *y);
int StrPtrCmpA(void *x, void *y);
int StrPtrCmpW(void *x, void *y);

void BytePrint(void *x);
void CharPrintA(void *x);
void CharPrintA(void *x);
void ShortPrint(void *x);
void UShortPrint(void *x);
void IntPrint(void *x);
void UIntPrint(void *x);
void LongPrint(void *x);
void ULongPrint(void *x);
void StrPrintA(void *x);
void StrPrintW(void *x);

wchar_t *WideStrFromAnsi(char *cpStr, size_t lSize);
unsigned int calc_GCD(unsigned int a, unsigned int b);

void PrintArray(void *data, size_t lSize, int nItemSize, void (* print)(void *));
void PrintByteBits(unsigned char b);
void PrintBits(unsigned char *pBytes, size_t ulSize, bool bLittleEndian);
void PrintArrayBits(unsigned char *pBytes, size_t ulArrSize, int nItemSize, bool bLittleEndian);

#endif