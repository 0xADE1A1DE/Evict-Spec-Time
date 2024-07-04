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
import re
import math

df1 = pd.read_csv('./sample_results/round2_benchmark.csv', sep=' <--> ', engine='python')
score_list = df1['score'].tolist()
cts_number = df1['cts'].tolist()
correct_cases = score_list.count(16)
print("correct cases: ", correct_cases)

num_cases = len(cts_number)


actual_score_list = [0] * correct_cases
cts_list = [0] * correct_cases

counter = 0
for val, ct in zip(score_list, cts_number):
    if val == 16:
        actual_score_list[counter] = val
        cts_list[counter] = ct
        tmp = [val, ct]
        counter += 1

print(actual_score_list.count(16))


result_pair = []
for val, ct in zip(score_list, cts_number):
    result_pair.append([val,ct])



success_rate_array = []
flag = 1
for vv in range( int(np.min(cts_number) / 100) * 100, math.ceil(np.max(cts_number)/100) * 100, 100):
    correct_counter = 0
    for i in range(0, len(result_pair)):
        if result_pair[i][0] == 16 and result_pair[i][1] <= vv:
            correct_counter += 1
    tmp_success_rate = correct_counter / num_cases
    if tmp_success_rate > 0.5 and flag == 1:
        flag = 0
    success_rate_array.append(tmp_success_rate)



plt.figure(figsize=(6, 2), dpi=80)
plt.rcParams['pdf.fonttype'] = 42
plt.rcParams['ps.fonttype'] = 42

x_xaxis = np.arange(len(success_rate_array))
plt.plot(x_xaxis, success_rate_array)
plt.gca().set_yticklabels([f'{x:.0%}' for x in plt.gca().get_yticks()]) 
plt.xticks(np.arange(0,len(success_rate_array),10), np.arange(int(np.min(cts_number) / 100) * 100, math.ceil(np.max(cts_number)/100) * 100,1000), rotation=70)

plt.xlabel("Number of Ciphertexts")
plt.ylabel("Success Rate")

plt.savefig("round2_benchmark.pdf", bbox_inches='tight')
