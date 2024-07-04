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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/mman.h>
#include <assemblyline.h>
#include <assert.h>

#include <mastik/low.h>
#include <mastik/util.h>

#define SAMPLES 100000
#define THRESHOLD 1000 // It is used to filter out the outliers which could be platform specific. 
#define memory_barrier asm volatile ("sfence;\nmfence;\nlfence;\n");


int array[256 * 1024] __attribute__((aligned(4096))) = {0};
int array_att[256 * 1024] __attribute__((aligned(4096))) = {0};

uint64_t result_flush[SAMPLES] = {0};
uint64_t result_cache[SAMPLES] = {0};
void init_array(int *buffer);

int main()
{
  srand(time(NULL));

  // Warmup 
  for (int i = 0; i < 10; i++)
    delayloop(0x8000000);
  memory_barrier

  // Initialize array
  init_array(array); // For victim
  init_array(array_att); // For attacker
  
  // Do not cache victim variable
  for (int i = 0; i < SAMPLES; i++) {
    int index_v = random() % 256;
    int index_att = random() % 256;

    *(volatile uint8_t *)&array_att[array_att[index_att << 10] << 10];
    memory_barrier

    // Flush attacker indexes, to create enough execution time overlap
#if HIDE 
    clflush(&array_att[array_att[index_att << 10] << 10]);
    clflush(&array_att[index_att << 10]);
#endif
    memory_barrier

    // Evict victim address
    clflush(&array[index_v << 10]);
    memory_barrier

    // Measuring ...
    uint32_t start = rdtscp();
    *(volatile uint8_t *)&array_att[array_att[index_att << 10] << 10];
    *(volatile uint8_t *)&array[index_v << 10];
    uint32_t end = rdtscp();

    // Filter outliers
    result_flush[i] = (end - start) > THRESHOLD ? THRESHOLD : (end - start);
  }

  memory_barrier

  // Cache victim variable
  for (int i = 0; i < SAMPLES; i++) {
    int index_v = random() % 256;
    int index_att = random() % 256;

    
    *(volatile uint8_t *)&array_att[array_att[index_att << 10] << 10];
    memory_barrier

    // Flush three attacker index
#if HIDE
    clflush(&array_att[array_att[index_att << 10] << 10]);
    clflush(&array_att[index_att << 10]);
#endif
    memory_barrier
    memory_barrier

    // Cache victim variable
    *(volatile uint8_t *)&array[index_v << 10];
    memory_barrier
    memory_barrier
    memory_barrier
    
    // Measuring ...
    uint32_t start = rdtscp();
    *(volatile uint8_t *)&array_att[array_att[index_att << 10] << 10];
    *(volatile uint8_t *)&array[index_v << 10];
    uint32_t end = rdtscp();

    // Filter outliers
    result_cache[i] = (end - start) > THRESHOLD ? THRESHOLD : (end - start);
  }

  printf("flush cache\n");
  for (int i = 0; i < SAMPLES; i++) {
    printf("%ld %ld\n", result_flush[i], result_cache[i]);
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
