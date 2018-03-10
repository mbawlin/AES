The Advanced Encryption Standard (AES) is a symmetric block cipher to encrypt and decrypt electronic data established by the U.S. National Institute of Standards and Technology. 

In my research, I studied the aspects of the encryption process in 128-bit AES: KeyExpansion, AddRoundKey, SubBytes, ShiftRows, and MixColumns. 

My implementation tested my knowledge of the C language and debugging and allowed me to explore my research capabilities. On paper, my code sets a set key, 0123456789abcdef, and then prompts the user to input a plaintext. It prints out the plaintext then, creates the key schedule, adds the first round key, and goes through the rounds of AES -- 9 cycles of SubBytes, ShiftRows, MixColumns, then adds the round key again. The 10th cycle is only a SubByte, ShiftRow, then adds the round key. After encryption, the program then outputs the encrypted text as 16-bit hex and determines if the program correctly encrypted the plaintext.
