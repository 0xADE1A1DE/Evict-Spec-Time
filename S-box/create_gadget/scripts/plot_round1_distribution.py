from scipy import stats
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import sys


df = pd.read_csv('data_round1.csv', sep=' ')
cts_data = df['ciphertext'].tolist()
times_data = df['time'].tolist()
access_data = df['access'].tolist()
cts_original = np.array(cts_data)
timing_result_original = np.array(times_data)
access_original = np.array(access_data)
data_len = len(timing_result_original)

####### Plot the first byte data
catgorey_time = []
samples =  10000
byte = 0
median_value = np.median(timing_result_original[:samples])
print(median_value)

### Below three variables should be updated according to the target machine. Values below are tested for i7-1165G7.
cache_miss_penalty = 200
upper_limit = median_value + cache_miss_penalty
lower_limit = median_value + cache_miss_penalty - 50

score = [0] * 4
raw = [0] * 4
byte0 = []
byte0_highlight = []
byte1_highlight = []
counter = 0
byte1 = []
byte2 = []
byte3 = []

### We update new array with timing result and if the monitored cache line is not accessed in the first round
for x,y,z in zip(timing_result_original[:samples], cts_original[:samples], access_original[:samples]):
    byte_value = int(y[byte*2:byte*2+2],16) >> 6
    if byte_value == 0:
        byte0.append([x,z])
    elif byte_value == 1:
        byte1.append([x,z])
    elif byte_value == 2:
        byte2.append([x,z])
    elif byte_value == 3:
        byte3.append([x,z])


check_point_byte0 = len(byte0)
check_point_byte1 = len(byte1)
check_point_byte2 = len(byte2)
check_point_byte3 = len(byte3)

### Merge byte0, byte1, highlight
overall_result = []
overall_access_result = []
overall_highlight_result = []
overall_error_result = []
overall_error_xaxis = []
overall_outlier = []

counter_x_error = 0

## Group results by byte value
for val in byte0:
    if val[1] == 1:
        overall_highlight_result.append([len(overall_result), val[0]])
    overall_result.append(val[0])
    overall_access_result.append(val[1])
    if val[1] == 0 and (val[0] < upper_limit) and (val[0] > lower_limit):
        overall_error_result.append(val[0])
        overall_error_xaxis.append(counter_x_error)
    if val[1] == 0 and (val[0] > 800):
        overall_outlier.append(val[0])


    counter_x_error += 1
    
for val in byte1:
    if val[1] == 1:
        overall_highlight_result.append([len(overall_result), val[0]])
    overall_result.append(val[0])
    overall_access_result.append(val[1])
    if val[1] == 0 and (val[0] < upper_limit) and (val[0] > lower_limit):
        overall_error_result.append(val[0])
        overall_error_xaxis.append(counter_x_error)
    if val[1] == 0 and (val[0] > 800):
        overall_outlier.append(val[0])
        
    counter_x_error += 1

for val in byte2:
    if val[1] == 1:
        overall_highlight_result.append([len(overall_result), val[0]])
    overall_result.append(val[0])
    overall_access_result.append(val[1])
    if val[1] == 0 and (val[0] < upper_limit) and (val[0] > lower_limit):
        overall_error_result.append(val[0])
        overall_error_xaxis.append(counter_x_error)
    if val[1] == 0 and (val[0] > 800):
        overall_outlier.append(val[0])

    counter_x_error += 1

for val in byte3:
    if val[1] == 1:
        overall_highlight_result.append([len(overall_result), val[0]])
    overall_result.append(val[0])
    overall_access_result.append(val[1])
    if val[1] == 0 and (val[0] < upper_limit) and (val[0] > lower_limit):
        overall_error_result.append(val[0])
        overall_error_xaxis.append(counter_x_error)
    if val[1] == 0 and (val[0] > 800):
        overall_outlier.append(val[0])

    counter_x_error += 1

x_axis = np.arange(len(overall_result))

array_plot_hit = overall_result.copy()
x_axis_hit = x_axis.copy().tolist()
array_plot_miss = overall_result.copy()
x_axis_miss = x_axis.copy().tolist()

tmp_index = np.where(np.array(overall_access_result) > 0)[0] ### Find miss in the first round
for index in reversed(tmp_index):
    array_plot_hit.pop(index)
    x_axis_hit.pop(index)

tmp_index = np.where(np.array(overall_access_result) < 1)[0] ### Find hit in the first round
for index in reversed(tmp_index):
    array_plot_miss.pop(index)
    x_axis_miss.pop(index)


plt.figure(figsize=(6, 2.5), dpi=80)
plt.rcParams['pdf.fonttype'] = 42
plt.rcParams['ps.fonttype'] = 42

plt.fill_between(x=x_axis, y1=upper_limit, y2=lower_limit, color='lightblue',  interpolate=True, alpha=.3)
plt.axvline(check_point_byte0, color='k', linestyle='--', linewidth=1, zorder=0)
plt.axvline(check_point_byte0 + check_point_byte1, color='k', linestyle='--', linewidth=1, zorder=0)
plt.axvline(check_point_byte0 + check_point_byte1 + check_point_byte2, color='k', linestyle='--', linewidth=1, zorder=0)

plt.scatter(x_axis_hit, array_plot_hit, s=1, c="lightgrey")
plt.scatter(x_axis_miss, array_plot_miss, s=1, c="green", marker="2", label="witness")
plt.scatter(overall_error_xaxis, overall_error_result, s=1, c="red", marker="x", label="false positive")

# print(len(overall_outlier))

plt.xticks([int(check_point_byte0/2), check_point_byte0 + int(check_point_byte1/2), check_point_byte0 + check_point_byte1 + int(check_point_byte2/2), check_point_byte0 + check_point_byte1 + check_point_byte2 + int(check_point_byte3/2)], ["0x0", "0x1", "0x2", "0x3"])
plt.ylim(700, 1200)
plt.xlabel("Corresponding Cyphertext Byte Value")
plt.ylabel("Timing Result (Cycles)")
plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.2), ncol=3, fancybox=True, shadow=True, prop={'size': 10})
plt.savefig('round1_singlebyte.pdf', bbox_inches='tight', pad_inches=0.03)
