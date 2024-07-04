#include "aes.h"
#include <stdint.h>


// Constants
#define AESSIZE 16
#define SBOX_LINES_IN_TABLE 4
#define SBOX_ENTRIES_IN_LINE 64
#define TTABLE_LINES_IN_TABLE 16
#define TTABLE_ENTRIES_IN_LINE 16


typedef uint8_t aes_t[16];

// aes_util.c
void toBinary(char *data, aes_t aes);
char *toString(aes_t aes);
void randAes(aes_t aes);
void randPt(aes_t pt, aes_t key, int line);
void printAes(aes_t text);
void showKeyComparison(aes_t key, aes_t guess, int bits);
void warmupAES();

void print_aes_access(u32 s0, u32 s1, u32 s2, u32 s3, int round);
int check_aes_access(u32 s0, u32 s1, u32 s2, u32 s3, int line);
int only_one_access(u32 s0, u32 s1, u32 s2, u32 s3, int num, int line);



// Globals
extern aes_t key;
extern aes_t actualKey; // copy of the key
extern AES_KEY aeskey;
extern AES_KEY aeskeyde;
extern size_t aesRuns;
