# (In)distinguishable Cache Hit and Miss

In this experiment, we demonstrate that the cache miss penalty can be hidden by the execution time overlap.  
This experiment reproduces the results presented in Section 3.1.

## Code
`main.c` contains code to reproduce results presented in Figure 3.  
The code first measure the execution time for reloading evicted victim data. Then it measures the execution time for accessing cached victim data. 

We use variable `HIDE` to control if we create long execution time overlap. This variable is set at the compile time.  

We use variable `threshold` to filter outliers when ploting the graph. This variable can either be removed or adjusted according to the target machine.

We plot the graph with `plot.py`. It accepts a file name and create a pdf with the same name.

## Required Libraries
Mastik and AssemblyLine are required.

## Run Code
To run the code, simply execute `bash test.bash $HIDE $FileName` in the terminal.  
To create long execution time overlap to hide the cache-miss penalty, please run `bash test.bash 1 flush`. Ideally, it should generate a graph similar to Figure 3 (b).  
On the other hand, you could run `bash test.bash 0 cache` to have no (few) execution time overlap. Ideally, it should generate a graph similar to Figure 3 (a). 

To plot the figure, run `python3 plot.py $FileName`.

## Sample Results
We provide sample results obtained on i7-1165G7, running Ubuntu 20.04.  
The results are presented in folder `./sample_results/`
