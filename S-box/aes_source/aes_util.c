#include "attack.h"
#include "aes.h"
#include <assert.h>

aes_t key;
aes_t actualKey; // copy of the key
AES_KEY aeskey;

extern u8 Te4[256];
void print_aes_access(u32 s0, u32 s1, u32 s2, u32 s3, int round)
{
  printf("Round: %d\n", round);
  printf("0x%x, 0x%x, 0x%x, 0x%x\n", ((s0      ) & 0xff) / 64, ((s1 >>  8) & 0xff) / 64, ((s2 >> 16) & 0xff) / 64, (s3 >> 24) / 64);
  printf("0x%x, 0x%x, 0x%x, 0x%x\n", ((s1      ) & 0xff) / 64, ((s2 >>  8) & 0xff) / 64, ((s3 >> 16) & 0xff) / 64, (s0 >> 24) / 64);
  printf("0x%x, 0x%x, 0x%x, 0x%x\n", ((s2      ) & 0xff) / 64, ((s3 >>  8) & 0xff) / 64, ((s0 >> 16) & 0xff) / 64, (s1 >> 24) / 64);
  printf("0x%x, 0x%x, 0x%x, 0x%x\n", ((s3      ) & 0xff) / 64, ((s0 >>  8) & 0xff) / 64, ((s1 >> 16) & 0xff) / 64, (s2 >> 24) / 64);
}

// Return 1 if it does not access the line
int check_aes_access(u32 s0, u32 s1, u32 s2, u32 s3, int line)
{
  for (int i = 0; i < 4; i++) {
    if ( (((s0 >> (i * 8)) & 0xFF) >> 6) == line )
      return 0;
    if ( (((s1 >> (i * 8)) & 0xFF) >> 6) == line )
      return 0;
    if ( (((s2 >> (i * 8)) & 0xFF) >> 6) == line )
      return 0;
    if ( (((s3 >> (i * 8)) & 0xFF) >> 6) == line )
      return 0;
  }
  return 1;
}

// num represents Bytes
int only_one_access(u32 s0, u32 s1, u32 s2, u32 s3, int num, int line)
{
  aes_t state;

  for (int i = 0; i < 4; i++) {
   state[i] = (s0 >> (i*8)) & 0xFF; 
   state[4 + i] = (s1 >> (i*8)) & 0xFF;
   state[8 + i] = (s2 >> (i*8)) & 0xFF;
   state[12 + i] = (s3 >> (i*8)) & 0xFF;
  }

  
  if ((state[num] >>6) != line)
    return 0;

  for (int i = 0; i < 16; i++) {
    if (i == num)
      continue;
    if ((state[i]>>6) == line)
      return 0;
  }
  return 1;
}



// Convert a string into an AES block
void toBinary(char *data, aes_t aes) {
    assert(strlen(data)>=AESSIZE*2);
    unsigned int x;
    for (int i = 0; i < AESSIZE; i++) {
        sscanf(data+i*2, "%2x", &x);
        aes[i] = x;
    }
}

// Convert an AES block into a string
char *toString(aes_t aes) {
    char buf[AESSIZE * 2 + 1];
    for (int i = 0; i < AESSIZE; i++)
        sprintf(buf + i*2, "%02x", aes[i]);
    return strdup(buf);
}

// Crate a random AES block
void randAes(aes_t aes) {
    for (int i = 0; i < AESSIZE; i++)
        aes[i] = rand() & 0xff;
}

// We assume we know 2 MSBs of the key and we generate pt that
// always hit one cache line in the first round.
void randPt(aes_t pt, aes_t key, int line) {
  for (int i = 0; i < AESSIZE; i++)
    pt[i] = ((key[i] ^ (line << 6)) & 0xC0) | (rand() & 0x3F);
}

// Print the AES block
void printAes(aes_t text) {
    for (int pt=0; pt<AESSIZE; pt++) {
        printf("%02x", text[pt]);
    }
    printf("\n");
}

void warmupAES() {
    aes_t text, output;
    for (int s=0; s<500; s++) {
        randAes(text);
        AES_decrypt(text, output, &aeskey);
    }
}
