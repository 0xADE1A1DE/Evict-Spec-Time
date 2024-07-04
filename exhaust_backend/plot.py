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
import matplotlib.pyplot as plt
import pandas as pd
import sys

df = pd.read_csv(sys.argv[1]+".csv", sep=' ')
dflush = df['flush'].tolist()
dcache = df['cache'].tolist()

plt.figure(figsize=(5,3))

xaxis = np.arange(len(dflush))

plt.plot(xaxis, dflush, label="cache miss")
plt.plot(xaxis, dcache, label="cache hit")
# plt.ylim(500,850)

plt.legend(fontsize=12)
plt.xlabel("Number of Inserted Instructions", fontsize=14)
plt.ylabel("Execution Time (Cycles)", fontsize=14)
plt.tight_layout()
plt.savefig(sys.argv[1]+".pdf")
