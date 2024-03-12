#pragma once
#include <stdio.h>
#include <windows.h>
#include "structs.h"
#include <stdarg.h>

#define INVALID_SSN 0xffff
#define HALOS_DOWN 32
#define HALOS_UP -32
#define HELLGATE_OFFSET 8
#define TARTARUSGATE_OFFSET 12

typedef enum { SUCCESS, STATUS, FAIL, LINE } MsgType;
/* Colors */
#define BOLD "\033[1m"
#define GREEN "\x1B[38;2;0;255;0m"
#define BLUE "\x1B[38;2;0;255;255m"
#define RED "\033[0;31m"
#define NC "\033[0m"
#define NL "\n"

void print(MsgType type, const char* fmt, ...) {
    char cFullMsg[BUFSIZ];
    WORD wMsgSize;
    va_list args;

    switch (type) {
    case SUCCESS:
        printf(GREEN "");
        break;

    case STATUS:
        printf(BLUE "");
        break;

    case LINE:
        printf(NC "");
        break;

    case FAIL:
        printf(RED "");
        break;
    }



    va_start(args, fmt);
    wMsgSize = vsnprintf(cFullMsg, BUFSIZ, fmt, args);
    va_end(args);

    for (WORD wIdx = 0; wIdx < wMsgSize; wIdx++) {
        fputc(cFullMsg[wIdx], stdout);
        fflush(stdout);
        Sleep(50); // Adjust the sleep time as needed
    }

    printf(NC);
}

EXTERN_C VOID AmmunitionPrepare(WORD systemCall);
EXTERN_C ULONG GunPrepare(INT_PTR syscallAdr);
EXTERN_C PVOID GetPEB();
EXTERN_C NTSTATUS GunFire();

WORD FindAmmunitionsAndPrepareGun(LPVOID addr) {
    print(STATUS, "[*] Checking syscall number at address: 0x%p\n", (LPVOID)((INT_PTR)addr + 0x12));


    WORD syscall;
    INT_PTR syscallAddr;

    // Hell's Gate method: check for direct syscall instruction with a specific pattern
    if (*((PBYTE)addr) == 0x4c &&
        *((PBYTE)addr + 1) == 0x8b &&
        *((PBYTE)addr + 2) == 0xd1 &&
        *((PBYTE)addr + 3) == 0xb8 &&
        *((PBYTE)addr + HELLGATE_OFFSET) == 0x00 &&
        *((PBYTE)addr + HELLGATE_OFFSET + 1) == 0x00) {

        BYTE high = *((PBYTE)addr + HELLGATE_OFFSET - 1);
        BYTE low = *((PBYTE)addr + HELLGATE_OFFSET - 2);
        syscall = (high << 8) | low;

        print(SUCCESS, "[+] Syscall number found using Hell's Gate: 0x%hx\n", syscall);

        syscallAddr = (INT_PTR)addr + 0x12;
        if (GunPrepare(syscallAddr) == 0L)
            print(SUCCESS, "[+] Syscall instruction address found at: 0x%p\n", (LPVOID)syscallAddr);

        return syscall;
    }

    // HalosGate method: check for direct syscall instruction with a specific pattern
    if (*((PBYTE)addr) == 0x4c &&
        *((PBYTE)addr + 1) == 0x8b &&
        *((PBYTE)addr + 2) == 0xd1 &&
        *((PBYTE)addr + 3) == 0xb8 &&
        *((PBYTE)addr + 6) == 0x00 &&
        *((PBYTE)addr + 7) == 0x00) {

        BYTE high = *((PBYTE)addr + 5);
        BYTE low = *((PBYTE)addr + 4);
        syscall = (high << 8) | low;

        print(SUCCESS, "[+] Syscall number found using HalosGate: 0x%hx\n", syscall);

        syscallAddr = (INT_PTR)addr + 0x12;
        if (GunPrepare(syscallAddr) == 0L)
            print(SUCCESS, "[+] Syscall instruction address found at: 0x%p\n", (LPVOID)syscallAddr);

        return syscall;
    }

    // TartarusGate method: look for JMP instructions at specific positions within the code
    if (*((PBYTE)addr) == 0xe9 ||
        *((PBYTE)addr + TARTARUSGATE_OFFSET) == 0xe9 ||
        *((PBYTE)addr + 7) == 0xe9 ||
        *((PBYTE)addr + 9) == 0xe9 ||
        *((PBYTE)addr + 11) == 0xe9) {

        // Search around the address using HalosGate up and down techniques
        for (WORD idx = 1; idx <= 500; idx++) {
            if (*((PBYTE)addr + idx * HALOS_DOWN) == 0x4c &&
                *((PBYTE)addr + 1 + idx * HALOS_DOWN) == 0x8b &&
                *((PBYTE)addr + 2 + idx * HALOS_DOWN) == 0xd1 &&
                *((PBYTE)addr + 3 + idx * HALOS_DOWN) == 0xb8 &&
                *((PBYTE)addr + 6 + idx * HALOS_DOWN) == 0x00 &&
                *((PBYTE)addr + 7 + idx * HALOS_DOWN) == 0x00) {

                BYTE high = *((PBYTE)addr + 5 + idx * HALOS_DOWN);
                BYTE low = *((PBYTE)addr + 4 + idx * HALOS_DOWN);
                syscall = (high << 8) | low - idx;

                print(SUCCESS, "[+] Syscall number found using TartarusGate: 0x%hx, with offset: %hd\n", syscall, idx);

                syscallAddr = (INT_PTR)addr + 0x12 + idx * HALOS_DOWN;
                if (GunPrepare(syscallAddr) == 0L)
                    print(SUCCESS, "[+] Syscall instruction address found at: 0x%p\n", (LPVOID)syscallAddr);

                return syscall;
            }
            if (*((PBYTE)addr + idx * HALOS_UP) == 0x4c &&
                *((PBYTE)addr + 1 + idx * HALOS_UP) == 0x8b &&
                *((PBYTE)addr + 2 + idx * HALOS_UP) == 0xd1 &&
                *((PBYTE)addr + 3 + idx * HALOS_UP) == 0xb8 &&
                *((PBYTE)addr + 6 + idx * HALOS_UP) == 0x00 &&
                *((PBYTE)addr + 7 + idx * HALOS_UP) == 0x00) {

                BYTE high = *((PBYTE)addr + 5 + idx * HALOS_UP);
                BYTE low = *((PBYTE)addr + 4 + idx * HALOS_UP);
                syscall = (high << 8) | low + idx;

                print(SUCCESS, "[+] Syscall number found using TartarusGate: 0x%hx, with offset: %hd\n", syscall, idx);

                syscallAddr = (INT_PTR)addr + 0x12 + idx * HALOS_UP;
                if (GunPrepare(syscallAddr) == 0L)
                    print(SUCCESS, "[+] Syscall instruction address found at: 0x%p\n", (LPVOID)syscallAddr);

                return syscall;
            }
        }
    }

    printf("[-] Syscall number not found or address is heavily hooked beyond recovery.\n");
    return INVALID_SSN;
}
