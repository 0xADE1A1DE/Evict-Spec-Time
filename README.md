# Evict + Spec + Time: Exploiting Out-of-Order Execution to Improve Cache-Timing Attacks  

This repo contains code to reproduce experiments presented in the paper ([link](https://eprint.iacr.org/2024/149.pdf)), which is accepted by CHES 2024.  

## Experiments

### cacheHit_Miss 
We demonstrate how out-of-order execution hides the cache-miss penalty of load operations.  
It is corresponding to the claims in Section 3.1 of the paper.  

### exhaust_backend   
We demonstrate how to exploit limited resources in the backend to hide the cache-miss penalty of load operations.  
It is corresponding to the claims in Section 3.2 of the paper.  

### T-table  
We demonstrate the T-table attack in this experiment.  
It is corresponding to the claims in Section 5 of the paper. 

### S-box  
We demonstrate the S-box attack in this experiment.  
It is corresponding to the claims in Section 6 of the paper.

## Libraries and Experiment Environment
The test script requires Mastik and AssemblyLine.  

Mastik is available at https://github.com/0xADE1A1DE/Mastik.git

AssemblyLine is available at https://github.com/0xADE1A1DE/AssemblyLine

## License
The code under this repo is under Apache-2.0 License
