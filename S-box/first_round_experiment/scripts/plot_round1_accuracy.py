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
import matplotlib.ticker as mtick

file1 = open('round1_benchmark_result.csv', 'r')
count = 0

overall_length = 100
samples = 10000
timing_result = []
while True:
    count += 1
 
    # Get next line from file
    line = file1.readline()
    tmp = re.findall(r'\d+', line)
    counter = 0
    for val in tmp:
        tmp[counter] = int(int(val)/16)
        counter += 1
    timing_result.append(tmp)
    if not line:
        break

samples = count - 1
timing_result.pop(samples)
file1.close()

accuracy_rate = [0] * overall_length
for i in range(0, overall_length):
    for tmp in timing_result:
        accuracy_rate[i] += tmp[i]

counter = 1
for val in accuracy_rate:
    accuracy_rate[counter-1] = val / samples
    counter += 1

plt.figure(figsize=(6, 2), dpi=80)
plt.rcParams['pdf.fonttype'] = 42
plt.rcParams['ps.fonttype'] = 42

x_xaxis = np.arange(100)
plt.plot(x_xaxis, accuracy_rate)
plt.gca().set_yticklabels([f'{x:.0%}' for x in plt.gca().get_yticks()]) 
plt.xticks(np.arange(0,101,5), np.arange(0,10001,500), rotation=70)
plt.xlabel("Number of Samples")
plt.ylabel("Accuracy")
plt.savefig('round1_accuracy.pdf', bbox_inches='tight')
