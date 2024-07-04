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

df = pd.read_csv(sys.argv[1]+".csv", sep=' ')
EST = int(sys.argv[2]) # Always make the correct guess to have positive correlation
measurement = df['Measurement'].tolist()
bytes = df['Byte'].tolist()
access = df['ACCESS'].tolist()

num_samples = 10000
plot_limit = 500
stride = 20

tmp_count = 0

result = [[0 for x in range(num_samples+1)] for y in range(16)]
for byte in bytes:
    for guess in range(16):
        if guess == byte:
            result[guess][tmp_count] = (1 - EST)
        else:
            result[guess][tmp_count] = EST
    tmp_count += 1

## We now need to calculate the pearson value
# g: We now guess key
experiment_num = 16
plot_me = [[0 for x in range(plot_limit+1)] for y in range(experiment_num)]

tmp_count = 0
for g in range(experiment_num):
    tmp_count = 0
    for i in range(plot_limit+1):
        if i == 0:
            r = 0
        else:
            r = stats.pearsonr(measurement[0: i * stride], result[g][0: i * stride])[0]
        if r != r:
            r = 0
        plot_me[g][i] = r

x_axis = np.arange(plot_limit+1)
plt.figure(figsize=(5,3))
for guess in range(experiment_num):
    if guess != 3:
        plt.plot(x_axis, plot_me[guess], alpha=0.5, linewidth=1)
plt.plot(x_axis, plot_me[3], color = "blue", linewidth=1, label="Correct Guess: 0x3")

plt.xlabel("Number of Ciphertexts", fontsize=10)
plt.ylabel("Pearson Correlation", fontsize=10)

plt.xticks(np.arange(0,plot_limit+1,25), np.arange(0,plot_limit*stride+1,500), rotation=70)
# plt.ylim((-0.1, 0.35))
plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.25), ncol=3, fancybox=True, shadow=True, prop={'size': 10})

plt.rcParams['pdf.fonttype'] = 42
plt.tight_layout()
plt.savefig(sys.argv[1]+".pdf", bbox_inches='tight' ,pad_inches=0.05)

