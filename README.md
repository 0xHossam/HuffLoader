# HuffLoader

![Screenshot 2024-03-12 161015](https://github.com/0xHossam/HuffLoader/assets/82971998/158a526c-72f5-4757-b363-bbe4c98ebd16)

## Loader Features

- Huffman Coding-based Shellcode Obfuscation
- Dynamic API Resolution
- Unhooking with HellsGate/HalosGate/TartarusGate Techniques
- Indirect Syscall Execution
- API calls and syscalls hash obfuscation
- Anti-Debugging 
- PEB-based DLL Discovery from TEB
- Loader evaded Kaspersky EDR and many AVs like (Bit-Defender/Avast/Kaspersky)

## Huffman Algorithm Overview

### What Is It?

The Huffman Algorithm is a technique used for compressing data without losing any information commonly applied to texts and multimedia files. 

Created by David A. Huffman in 1952 this method utilizes a tree to assign binary codes to frequently occurring data resulting in efficient compression.

### How It Works

1. Determine the frequency of each data piece
2. Construct a tree where paths are coded with 0s (left) and 1s (right) based on frequency
3. Assign codes, to more frequent data pieces recieve shorter binary codes
4. Compress the data by replacing it with these codes there by reducing its size


### Example

For example when given the input (BCCADBBDABBCCADDCCEE) each unique character is represented by a code based on its frequency. This compression process reduces the size of the data while ensuring that the original content can be perfectly reconstructed using a header containing each characters code.

![image-1](https://github.com/0xHossam/HuffLoader/assets/82971998/f455b235-2972-4aa4-9515-dba44ecd3f47)


**NOTE: THE LOADER HUFFMAN ALGORITHM IS BASED FROM @0xNinjaCyclone IMPLMENEATION** -> https://github.com/0xNinjaCyclone/dalgo/blob/master/Algo/huffman.c

## Importance of Huffman for Red Teamers & Malware Researchers

**Size and Entropy Reduction** 

Huffman encoding, as previously discussed is a method of compressing data, by assigning codes to used symbols in malicious software. This helps to shrink the size of shellcode making it simpler to install and run in environments, with memory.

Moreover the use of Huffman coding reduces the randomness of the shellcode making it more challenging to detect using security analysis techniques based on randomness. Randomness, known as entropy can signal behavior. By decreasing entropy levels Huffman coding helps disguise the shellcode and make it less noticeable.

**Frequency-Size Correlation** 

The effectiveness of Huffman coding in reducing shellcode size depends on the frequency of symbols within the code. When frequently used symbols have shorter codes, wich is making it perfect for Stageless Payloads ^_^

## Usage

- Prepare your shellcode
- Run the payload builder to encode your shellcode:

~~~bash
./main.sh shellcode.bin encoded.bin
~~~

- This generates encoded.bin, ready for the loader.
- Set execution permissions if needed: `chmod +x main.sh`
- Add the payload.h file into the Loader Folder

Done! Now your malware is ready! Go ahead and hack the world!

**Example Image**

![image](https://github.com/0xHossam/HuffLoader/assets/82971998/7b652fe9-a54b-446c-bac5-c0ed3106b2c1)


## About Author

- Medium: [https://medium.com/@0xHossam](https://medium.com/@0xHossam)
- X: [https://www.facebook.com/0xHossam](https://x.com/0xHossam)
- Github: [https://github.com/0xHossam](https://github.com/0xHossam)

## Warning

I am not responsible for any of your actions, this tool for educational purposes only.


