# Find Gadget for S-box Round1 Attack

This experiment aims to find a suitable gadget for different processors to carry out the first-round S-box Attack. 

The gadget here represents the number of inserted *NOP*s, such that an attacker could control the ROB resource allocation.

The found gadget could be used to test the second-round S-box attack.
Howeve the success of the second-round S-box attack requires a large enough ROB to hold both first-round and second-round operations.

For more information on the code presented in this folder, please refer to the README in folder *first_round_experiment* and *second_round_experiment*.

# Run Code  
Simply run `bash test_gadget.bash` will start the process of finding a suitable gadget.  
For each number of *NOP*s, we benchmark the success rate with five secret keys.  
With a suitable gadget, we should be able to reconstruct 2MSB of all 16 key bytes.  

You may need to update the *cache_miss_penalty* and *cache_miss_rangfe* to control the range of timing measurement of interest.  
The default values are for 11th Gen processors.