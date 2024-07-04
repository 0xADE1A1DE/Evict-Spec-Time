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


from scipy import stats
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import sys

f = open("round1_benchmark_result.csv", "a")

df1 = pd.read_csv('key_round1.csv', sep=' ')
real_key = df1['key'].tolist()[0]

df = pd.read_csv('data_round1.csv', sep=' ')
cts_data = df['ciphertext'].tolist()
times_data = df['time'].tolist()
cts_original = np.array(cts_data)
timing_result_original = np.array(times_data)
data_len = len(timing_result_original)

threshold = 150

### Each step is 500
score_lists = [0] * 100
for i in range(1, 101) :
    samples = 100 * i

    median_value = np.median(timing_result_original[:samples])
    upper_limit = median_value + 200 #threshold + 200 
    lower_limit = median_value + 150 #threshold - 150 

    correct_score = 0
    final_result = [0] * 16
    #byte = int(sys.argv[1])
    for byte in range(0,16):
        score = [0] * 4
        for x, y in zip(timing_result_original[:samples], cts_original[:samples]):
            if x < lower_limit:
                continue
            if x > upper_limit:
                continue
            score[int(y[byte*2:byte*2+2],16) >> 6] += 1
        final_result[byte] = [int(real_key[byte*2:byte*2+2], 16) >> 6, score.index(np.min(score))]
        correct_byte = (int(real_key[byte*2:byte*2+2], 16) >> 6)
        if final_result[byte][1] == correct_byte:
            correct_score += 1
    score_lists[i-1] = correct_score
    f.write(str(correct_score) + " ")
f.write("\n")
f.close()
exit()
