# S-box Round2 Attack Test

In this experiment, we test if the second-round Sbox attack works on the target machine.  

## Code 
### test-second-round-single-byte.c
To test if the attack works on the target machine, we fix the secret key and only reconstruct the first key byte.  

### plot-r2-single-byte.py  
It plots the pearson-correlation results for all key guesses.  

## Run Code

Mastik and AssemblyLine are required.

### Compile
- Run `make` under the current folder to compile the project.  

### Execute  
- Run `./crun` to collect timing results. The results are stored in *data1.csv*.  
- Run `./scripts/plot-r2-single-byte.py` to plot the results.  

### Sample Results  
We provide sample results that are collected on i7-1165G7 which runds Ubuntu 20.04 with default OS configurations.  

If the attack works, we should see a guess candidate has significantly high pearson correlation result. 

## Debug 
To successfully run the second-round attack, the ROB buffer on the target machine should be large enough to hold all two rounds instructions.  
To find a suitable gadget, you will need to adjust the number of inserted instructions.  