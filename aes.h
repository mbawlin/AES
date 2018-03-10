#ifndef AES_128_H
#define AES_128_H

#include <stdio.h>
#include <stdint.h>

#define BLOCK_SIZE      16
#define ROUNDS          10 
#define ROUND_KEY_SIZE  176 // AES-128 has 10 rounds, and there is a AddRoundKey before first round. (10+1)x16=176.

void KeyExpansion(const uint8_t *key, uint8_t *roundkeys);

void Cipher(const uint8_t *roundkeys, const uint8_t *in, uint8_t *out);

#endif
