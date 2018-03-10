#include <stdint.h>
#include <stdio.h>

#include "aes.h"


  //round constants
 
static uint8_t roundConstant[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};


  //Sbox
static uint8_t sbox[256] = {

//   0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //a
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //b
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //c
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //d
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //e
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};//f


/*
  https://en.wikipedia.org/wiki/Finite_field_arithmetic
  Multiply two numbers in the GF(2^8) finite field defined
  by the polynomial x^8 + x^4 + x^3 + x + 1 = 0
 */
static inline uint8_t mul2(uint8_t a) {
    return (a&0x80) ? ((a<<1)^0x1b) : (a<<1);
}


static void ShiftRows(uint8_t *state) {
    uint8_t temp;
    
    // row1
    temp = *(state+1);
    *(state+1) = *(state+5);
    *(state+5) = *(state+9);
    *(state+9) = *(state+13);
    *(state+13)= temp;
    
    // row2
    temp = *(state+2);
    *(state+2) = *(state+10);
    *(state+10) = temp;
    temp = *(state+6);
    *(state+6) = *(state+14);
    *(state+14) = temp;
    
    // row3
    temp = *(state+15);
    *(state+15) = *(state+11);
    *(state+11) = *(state+7);
    *(state+7) = *(state+3);
    *(state+3) = temp;
}


void KeyExpansion(const uint8_t *key, uint8_t *roundkeys) {

    uint8_t temp[4];
    uint8_t *last4bytes; // point to the last 4 bytes of one round
    uint8_t *lastRound;
    uint8_t i;

    for (i = 0; i < 16; ++i) {
        *roundkeys++ = *key++;
    }

    last4bytes = roundkeys-4;
    for (i = 0; i < ROUNDS; ++i) {
        
        // k0-k3 for next round
        temp[3] = sbox[*last4bytes++];
        temp[0] = sbox[*last4bytes++];
        temp[1] = sbox[*last4bytes++];
        temp[2] = sbox[*last4bytes++];
        temp[0] ^= roundConstant[i];
        lastRound = roundkeys-16;
        *roundkeys++ = temp[0] ^ *lastRound++;
        *roundkeys++ = temp[1] ^ *lastRound++;
        *roundkeys++ = temp[2] ^ *lastRound++;
        *roundkeys++ = temp[3] ^ *lastRound++;
        
        // k4-k7 for next round        
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        
        // k8-k11 for next round
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        
        // k12-k15 for next round
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
        *roundkeys++ = *last4bytes++ ^ *lastRound++;
    }
}

void Cipher(const uint8_t *roundkeys, const uint8_t *in, uint8_t *out) {

    uint8_t tmp[16], t;
    uint8_t i, j;

    // first AddRoundKey
    for ( i = 0; i < BLOCK_SIZE; ++i ) {
        *(out+i) = *(in+i) ^ *roundkeys++;
    }

    // 9 rounds
    for (j = 1; j < ROUNDS; ++j) {

        // SubBytes
        for (i = 0; i < BLOCK_SIZE; ++i) {
            *(tmp+i) = sbox[*(out+i)];
        }
        ShiftRows(tmp);
        
        /*
          MixColumns
          [02 03 01 01]   [s0  s4  s8  s12]
          [01 02 03 01] . [s1  s5  s9  s13]
          [01 01 02 03]   [s2  s6  s10 s14]
          [03 01 01 02]   [s3  s7  s11 s15]
         */
        for (i = 0; i < BLOCK_SIZE; i+=4)  {
            t = tmp[i] ^ tmp[i+1] ^ tmp[i+2] ^ tmp[i+3];
            out[i]   = mul2(tmp[i]   ^ tmp[i+1]) ^ tmp[i]   ^ t;
            out[i+1] = mul2(tmp[i+1] ^ tmp[i+2]) ^ tmp[i+1] ^ t;
            out[i+2] = mul2(tmp[i+2] ^ tmp[i+3]) ^ tmp[i+2] ^ t;
            out[i+3] = mul2(tmp[i+3] ^ tmp[i]  ) ^ tmp[i+3] ^ t;
        }

        // AddRoundKey
        for ( i = 0; i < BLOCK_SIZE; ++i ) {
            *(out+i) ^= *roundkeys++;
        }
    }
    
    // last round
    for (i = 0; i < BLOCK_SIZE; ++i) {
        *(out+i) = sbox[*(out+i)];
    }
    ShiftRows(out);
    for ( i = 0; i < BLOCK_SIZE; ++i ) {
        *(out+i) ^= *roundkeys++;
    }
    
    
}
