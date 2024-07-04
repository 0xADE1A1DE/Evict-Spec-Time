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

#define SAMPLES 10000 //0 // Repeat each measurement X times and take average
#define ITERATION 800 // It defines how many instructions to be inserted
#define memory_barrier asm volatile ("sfence;\nmfence;\nlfence;\n");

int array[256 * 1024] __attribute__((aligned(4096))) = {0}; // victim array
int array_att[256 * 1024] __attribute__((aligned(4096))) = {0}; // attacker array

uint64_t result_flush[SAMPLES] = {0};
uint64_t result_cache[SAMPLES] = {0};
void init_array(int *buffer);

void *create_buffer(int size);
void * generate_func(int num, int size);

int main()
{
  srand(time(NULL));

  // Construct functions to exhaust backend resources: ROB, RS
  void (*exhaust_bs[ITERATION])(uint64_t val);
  for (int i = 0; i < ITERATION; i++)
    exhaust_bs[i] = generate_func(i,4096);

  // Warmup
  for (int i = 0; i < 20; i++)
    delayloop(0x8000000);
  memory_barrier

  uint64_t overall_time = 0;

  // Initialize the array
  init_array(array);
  init_array(array_att);

  // Measure overall execution time when evicting victim variable
  for (int i = 0; i < ITERATION; i++) {
    overall_time = 0;
    for (int s = 0; s < SAMPLES; s++) {
      int index_v = random() % 256;
      int index_att = random() % 256;
      
      memory_barrier
      clflush(&array_att[array_att[index_att << 10] << 10]);
      clflush(&array_att[index_att << 10]);
      memory_barrier

      // Evict victim address
      clflush(&array[index_v << 10]);
      memory_barrier

      // Measuring ...
      uint32_t start = rdtscp();
      volatile int cmp_value = array_att[array_att[index_att << 10] << 10];
      exhaust_bs[i](cmp_value);
      *(volatile uint8_t *)&array[index_v << 10];
      uint32_t end = rdtscp();

      overall_time += (end - start);
    }
    result_flush[i] = overall_time / SAMPLES;
  }

  memory_barrier
  memory_barrier
  memory_barrier
  memory_barrier
  memory_barrier

  // Measure overall execution time when caching victim variable
  for (int i = 0; i < ITERATION; i++) {
    overall_time = 0;
    for (int s = 0; s < SAMPLES; s++) {
      int index_v = random() % 256;
      int index_att = random() % 256;
      
      memory_barrier
      clflush(&array_att[array_att[index_att << 10] << 10]);
      clflush(&array_att[index_att << 10]);
      memory_barrier

      // Cache victim address
      *(volatile uint8_t *)&array[index_v << 10];
      memory_barrier

      // Measuring ...
      uint32_t start = rdtscp();
      volatile int cmp_value = array_att[array_att[index_att << 10] << 10];
      exhaust_bs[i](cmp_value);
      *(volatile uint8_t *)&array[index_v << 10];
      uint32_t end = rdtscp();

      overall_time += (end - start);
    }
    result_cache[i] = overall_time / SAMPLES;
  }

  printf("flush cache\n");
  for (int i = 0; i < ITERATION; i++)
    printf("%ld %ld\n", result_flush[i], result_cache[i]);

  for (int i = 0; i < ITERATION; i++)
    munmap(exhaust_bs[i], 4096);
}

// Gnerate function to exhaust backend resources
void * generate_func(int num, int size)
{
  void *buffer = create_buffer(size);
  assemblyline_t al = asm_create_instance(buffer, size);
  for (int i = 0; i < num; i++) {
#if NOPDEF
  asm_assemble_str(al, "NOP");
#else
    asm_assemble_str(al, "CMP RDI, RCX");
#endif
  }
  asm_assemble_str(al, "ret");
  void (*func)() = asm_get_code(al);
  asm_destroy_instance(al);
  return func;
}

void *create_buffer(int size)
{
  void * buffer = (void *)mmap ( NULL, size,
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE,
			     -1, 0 );
  if (buffer == MAP_FAILED) {
    printf ("Fail to allocate memory\n");
  } 
  return buffer;
}

void init_array(int *buffer)
{
    for (size_t k = 0; k < 256; ++k)
    {
        size_t x = ((k * 167) + 13) & (0xff);
        buffer[k * 1024] = x;
    }
}


