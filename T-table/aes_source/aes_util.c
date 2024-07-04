#include "attack.h"
#include "aes.h"
#include <assert.h>

aes_t key;
aes_t actualKey; // copy of the key
AES_KEY aeskey;

extern u32 Te4[256];

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
