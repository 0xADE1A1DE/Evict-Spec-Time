# S-box Round2 Attack Benchmark

In this experiment, we proceed to reconstruct the entire key with knowledge if a cache miss happens in the second round.

## Code

### test-second-round.c
It contains the code to collect timing measurements.  
The inputs are chosen ciphertexts such that the evicted cache line is not accessed in the first round.  
The code is structured to facilitate the benchmarking.  

Similay to other experiments, we use variable `REPEAT` to control the number of repeat measurements. By default, this value is one. 

### ./gen_key/generate_key.c
The code generates random keys to facilitate the benchmarking.  

### benchmark.py 
It reads the generated secret key and feed it to the AES decryption.  
It filters the outliers of collected timing measurement multiple time to find the sweet point.  
We naively take the guess candidate that has the highest pearson correlation as a correct key guess.  
To reconstruct the first key byte, we only accept the candidate that has > 0.7 pearson correlation as the correct guess.  
For the rest of key bytes, we accept the candidate that has the highest pearson corrrelation results.  
The entire process is automatic and it stores results in file *round2_benchmark.csv*.

### plot_round2_data.py
It plots the benchmark result and stores result in *round2_benchmark.pdf*.

## Run Code

Mastik and AssemblyLine are required.

### Compile 
- Run `make` under the current folder to compile the attack code
- Run `make` under the folder of  `./gen_key` to compile the key generation process

### Test  
- Run `python3 benchmark.py` under the current folder. By default, we test for 10 samples. It can be chanegd in the main function of the script.

### Plot
- Run `python3 plot_round2_data.py` to plot the results.

### Sample Results  
We provide sample results that are collected on i7-1165G7 which runds Ubuntu 20.04 with default OS configurations.  
