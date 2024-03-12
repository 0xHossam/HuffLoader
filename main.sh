#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo -e "\e[32mUsage: $0 <original.txt> <encoded.bin>\e[0m"
    exit 1
fi

green="\x1B[38;2;0;255;0m"
reset="\x1B[92m"

echo -e "${green}\t    __  __      ________   __                    __         "
echo -e "\t   / / / /_  __/ __/ __/  / /   ____  ____ _____/ /__  _____"
echo -e "\t  / /_/ / / / / /_/ /_   / /   / __ \\/ __ \`/ __  / _ \\/ ___/"
echo -e "\t / __  / /_/ / __/ __/  / /___/ /_/ / /_/ / /_/ /  __/ /    "
echo -e "\t/_/ /_/\\__,_/_/ /_/    /_____/\____/\\__,_/\\__,_/\\___/_/     ${reset}\n"

echo -e "${green}\tTitle   : Huffman Coding / Shellcode Obfuscation & Indirect Syscalls Ldr"
echo -e "\tAuthor  : Hossam Ehab"
echo -e "\tInfo    : Github: Github.com/0xHossam\n${reset}"

cd Encoder-Decoder
gcc -I../DS/ -I../Algo/ ../Algo/helpers.c ../DS/heap.c ../DS/llist.c ../DS/hash.c ../Algo/huffman.c huffman_encode.c -o huffman_encode

if [ $? -ne 0 ]; then
    echo -e "${green}Compilation failed.${reset}"
    exit 2
fi

./huffman_encode "$1" "$2"