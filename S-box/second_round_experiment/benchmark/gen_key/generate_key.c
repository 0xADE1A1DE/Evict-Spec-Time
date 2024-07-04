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
#include "../../../aes_source/attack.h"

int main(int argc, char* argv[])
{
    int random_times = atoi(argv[1]);
    aes_t key, pt, ct, pt_candidate, key_backup, real_key;
    for (int i = 0; i <= random_times; i++)
        randAes(key);
    AES_set_decrypt_key(key, 128, &aeskey);
    const u32 *rk;
    rk = aeskey.rd_key;


    for (int copy_key = 0; copy_key < 16; copy_key++) {
        real_key[copy_key] = (rk[copy_key/4] >> ((copy_key % 4) * 8)) & 0xff;
    }
    
    FILE *f = fopen("key.csv", "w");
    fprintf(f, "r0_key origin_key\n");
    fprintf(f, "%s %s\n", toString(real_key), toString(key));
    fclose(f);
}
