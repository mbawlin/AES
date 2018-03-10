#include <stdio.h>

#include "aes.h"

int main(int argc, char *argv[]) {

    // Running variables in for loops
	uint8_t i = 0, j = 1, r, input;
    uint8_t *in;
	// Default 128 bit key
	uint8_t key[] = {
		//0x0f, 0x15, 0x71, 0xc9, 0x47, 0xd9, 0xe8, 0x59,
		//0x0c, 0xb7, 0xad, 0xd6, 0xaf, 0x7f, 0x67, 0x98,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

	};
    
    //plaintext input
    printf("\nWrite any plaintext:\n");
    //uint8_t in[BLOCK_SIZE];
    in = (uint8_t*)malloc(sizeof(uint8_t));
    while (input != '\n'){
        input = getc(stdin);
        in = (uint8_t*)realloc(in, sizeof(in) * sizeof(uint8_t));
        in[i] = input;
        i++;
    }
    i--;
    for (i = i; i < BLOCK_SIZE; i++)
        in[i] = 0;
    
    uint8_t out[BLOCK_SIZE];
	uint8_t roundkeys[ROUND_KEY_SIZE];
    
	printf("\n--------------------------------------------------------\n");
	printf("Plain text:\n");
	for (i = 0; i < BLOCK_SIZE; i++) {
		printf("%2x ", in[i]);
	}
	printf("\n\n");	

	// key schedule
	KeyExpansion(key, roundkeys);
    
	printf("Round Keys:\n");
	for ( r = 0; r <= ROUNDS; r++ ) {
		for (i = 0; i < BLOCK_SIZE; i++) {
			printf("%2x ", roundkeys[r*BLOCK_SIZE+i]);
		}
		printf("\n");
	}
	printf("\n");

	// encryption
	Cipher(roundkeys, in, out);
	printf("Cipher text:\n");
	for (i = 0; i < BLOCK_SIZE; i++) {
		printf("%2x ", out[i]);
    }
    printf("\n");
	return 0;
    
    free(in);

}
