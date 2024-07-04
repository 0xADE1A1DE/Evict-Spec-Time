#pragma once

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

#  define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
//#define GETU32(p) (*((u32*)(p)))
#  define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); (ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }

//typedef unsigned long long u64;
# ifdef AES_LONG
typedef unsigned long u32;
# else
typedef unsigned int u32;
# endif
typedef unsigned short u16;
typedef unsigned char u8;


# define AES_MAXNR 14
# define AES_BLOCK_SIZE 16

struct aes_key_st {
# ifdef AES_LONG
    unsigned long rd_key[4 * (AES_MAXNR + 1)];
# else
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
# endif
    int rounds;
};
typedef struct aes_key_st AES_KEY;

# define MAXKC   (256/32)
# define MAXKB   (256/8)
# define MAXNR   14

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);
int AES_encrypt2(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key, int bytes);
void AES_decrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);      
int AES_decrypt2(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key, int bytes);
int AES_decrypt3(const unsigned char *in, unsigned char *out,
const AES_KEY *key, int bytes);
int AES_decrypt4(const unsigned char *in, unsigned char *out,
const AES_KEY *key, int bytes);
int AES_missed(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);




