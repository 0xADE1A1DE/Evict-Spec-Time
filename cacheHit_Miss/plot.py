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
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd
from scipy import stats
import statistics as sta
import sys
import seaborn as sb

df = pd.read_csv(sys.argv[1]+".csv", sep=' ')
dflush = df['flush'].tolist()
dcache = df['cache'].tolist()

xaxis = np.arange(len(dflush))

plt.figure(figsize=(5,3))
result = stats.ttest_ind(dflush, dcache)
print(result)

print(sta.mean(dflush), sta.mean(dcache))

sb.kdeplot(dflush, label="cache miss")
sb.kdeplot(dcache, label="cache hit")

plt.legend(fontsize=12, loc="upper right")
plt.xlabel("Execution Time (Cycles)", fontsize=14)
plt.ylabel("Density", fontsize=14)

plt.ylim(0,0.06)

plt.rcParams['pdf.fonttype'] = 42
plt.tight_layout()
plt.savefig(sys.argv[1]+".pdf")
