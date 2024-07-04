# T-table Attack

In this experiment, we present T-table attack which is corresponding to Section 5. 

## Code
We intergrate the E+S+T and E+T attacks into one file: `main.c`.  
To control which attack to test, you need to pass `EST=` to the Makefile.  

Overall, we collect 10000 samples and we only measure the execution time once for each sample.  
We filter outliers by setting the threshold to be 1000. The attack also works by removing the threshold, but the efficiency of both E+S+T and E+T attack drop a bit.  
To improve the E+T results, we preload four tables before each measurement. We note that it is not required for our E+S+T attack.  

We plot the result with `plot.py`. We pre-highlight the expected result for Byte 0 of the given key in the script (to generate the graph presented in the paper). When recovering other bytes (of random keys), please ignore the *correct guess* shown in the graph. Ideally, the correct guess should have the highest pearson correlation value.

## Run Code

To run the code, Mastik and AssemblyLine are required.

### Compile
To perform E+S+T attack, you need to compile the code with command `make clean && make EST=1`.  
To perform E+T attack, you need to compile the code with command `make clean && make EST=0`.

### Execute
To collect data of E+S+T, you should use command `./crun $byte > EST.csv` where $byte is the byte to recover.  
For example, if you want to recover Byte 0. You should run `./crun 0 > EST.csv`.   
To plot the graph for E+S+T, you should run command `python3 plot.py EST 1`. *EST* is the file name that stores the results. *1* makes the script report positive pearson correlation.

On the other hand, to collect data of E+T, you should use command `./crun $byte > ET.csv` where $byte is the byte to recover.
To plot the graph for E+T, you should run command `python3 plot.py ET 0`. *ET* is the file name that stores the result. *0* makes the script report positive pearson correlation.

#### Sample Command
- For E+S+T:  
    `make clean && make EST=1`  
    `./crun 0 > EST.csv`  
    `python3 plot.py EST 1`

- For E+T:
    `make clean && make EST=0`  
    `./crun 0 > ET.csv`  
    `python3 plot.py ET 0`

## Sample Results
We provide sample results obtained on i7-1165G7, running Ubuntu 20.04.  
The results are presented in folder `./sample_results/`