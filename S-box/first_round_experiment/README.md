# S-box Round1 Attack -- Reconstruct 2 MSBs of each byte

In this experiment, we demonstrate how to reconstruct 2 MSBs of each byte from S-box based AES.  

## Code
### first_round.c
`first_round.c` contains the code to perform the attack.  
It first generates a random key and store in file *key_round1.csv*.  
It then randomly generate 20000 ciphertexts and mount the attack. The number of ciphertexts can be controlled by changing the value of *SAMPLES*.  
By default, each measurement is only done once. The number of repeats can be controlled by changing the value of *REPEAT*.

We store the corresponding ciphertexts and the timing measurement to file *data_round1.csv*.  

For the purpose of debugging, we also print whether the evicted cache line is accessed in the first round computation or not. This information is not used in the result analysis. However, it is useful to debug the program on a new machine.  


### test_accuracy_round1.bash
This script is used to benchmark the accuracy of the first-round attack. 

### scripts/plot_round1_distribution.py
This script is used to plot the execution time of different ciphertexts. It predefines a threshold (*cache_miss_penalty* at line 25) to highlight the results that we have interest. Note that this predefined threshold is various from processors to processors. Furthermore, we define a *cache_miss_range* to control the range of timing measurement of interest.  
Ideally, these two thresholds are easy to observe from the generated graph.  

The result is stored in *round1_singlebyte.pdf*.

### scripts/benchmark_round1.py
This script is used to reconstruct 2 MSBs of all 16 bytes. It is used for benchmarking the accuracy.  
Similarly, we also use two thresholds to determine the timing measurement of interest.  
It reports the number of correctly reconstructed keys.
The results are stored in *round1_benchmark_result.csv*.

### scripts/plot_round1_accuracy.py 
This script is used to compute the relationship between the number of ciphertexts and the success rate.  
The result is stored in *round1_accuracy.pdf*. 

## Run Code  

To run the code, Mastik and AssemblyLine are required.

- Compile  
    `make`

- To plot figure of reconstructing the 2 MSBs of key byte 0:  
    You should run `./crun 0`, the first parameter represents the byte to reconstruct. It stores timing results to file *data_round1.csv*.  
    You could plot the figure with command `python3 ./scripts/plot_round1_distribution.py`. You are expected to adjust two thresholds with this experiment. The default setting we provided is for 11th Gen processors.
- To benchmark the first-round attack and plot the benchmark result:  
    `bash test_accuracy_round1.bash`. The script randomly generate 10 keys and reconstruct 2 MSBs of all key bytes.   The number of tested keys could be updated in the bash script.  

## Sample Results
We present the samples results in `./sample_results/`. The results are collected on i7-1165G7, running Ubuntu 20.04 with default OS configurations.  

## Debug  
- The attack relies on the existence of a gadget that allows an attacker to affect backend resources. Since the size of backend resources are different from machines, the number of inserted *NOP*s should be updated accordingly.  
You could either find a suitable gadget manually or run the automatic script under folder `../create_gadget`.
