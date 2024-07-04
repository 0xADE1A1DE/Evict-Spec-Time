# Latency-Hiding Capability

In this experiment, we exploit limited backend resources to hide the cache-miss penalty of accessing a victim address.  
This experiment reproduces the results presented in Section 3.2.



## Code 
`main.c` contains code to reproduce results presented in Figure 4.  
The code first measure the execution time for reloading evicted victim data. Then it measures the execution time for accessing cached victim data.   

We use variable `NOPDEF` to control which backend buffer to exhaust.  
When the variable is set to be 1, we exhaust the ROB.  
Otherwise, we exhaust the Reservation Station.

By default, we insert 800 instructions (NOP or CMP) to exhaust backend resources. To change the value, please edit it in `main.c` file.  

We repeat each measurement 10000 times to obtain stable results while having a relatively short execution time. It can be changed in `main.c` file.

We plot the graph with `plot.py`. It accepts a file name and create a pdf with the same name.

## Required Libraries
Mastik and AssemblyLine are required.

## Run Code
To run the code, simply execute `bash test.bash $NOPDEF $FileName` in the terminal.  
To exhaust the ROB, we set NOPDEF to be 1, and you can use command `bash test.bash 1 nop` to run the experiment.  
To exhaust the RS, we set NOPDEF to be 0, and you can use command `bash test.bash 0 cmp` to run the experiment. 

## Sample Results
We provide sample results obtained on i7-1165G7, running Ubuntu 20.04.  
The results are presented in folder `./sample_results/`