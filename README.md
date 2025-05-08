In this project, you are going to use SimpleScalar as the evaluation engine to perform a design space exploration, using the provided framework code, over an 18- dimensional processor pipeline and memory hierarchy design space. You will use a 5-benchmark suite as the workload. This is an individual project, and submitted artifacts include BOTH a) your code implementations of replacements for two stub functions within the provided framework and b)  a project report discussing your chosen heuristics for exploring the design space that summarizes your findings and how they confirm or run counter to intuitions developed in the discussion of material in class and assigned readings. The project report, the data contained within and their analysis will be the primary means of assessing this project, but a submission of your code modifications in YOURCODEHERE.cpp is required.
 
The provided framework already handles the invocation of SimpleScalar, the recording of evaluated points, and the collection of data from each evaluation. The set of possible points within the design space to be considered are constrained by the provided shell script wrapper used to employ SimpleScalar as a design point evaluation tool. All allowed configuration parameters for each dimension of the design space are described in the provided shell script. The shell script takes 18 integer arguments, one for each configuration dimension, which expresses the index of the selected parameter for each dimension. All reported results should be normalized against a baseline design with configuration
parameters: 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 2, 2, 2, 3, 0, 0, 3, 0 (This baseline is hard-coded into the framework).
 
Note that not all possible parameter settings represent a valid combination. One of your tasks will be to write a configuration validation function based upon restrictions described later in this document. Further, note that this design space is too large to efficiently search in an exhaustive manner, so a key project goal will be to develop heuristics in prioritizing which design points to explore first.
 
Your assignment is to, with an evaluation count limit of 1000 design points, explore the design space in order to select the best performing design under a set of four different optimization functions. These are
1) the “best” performance-oriented design (ED2P) (Js^2) (in term of the minimum geometric mean of normalized energy-delay-squared product across all benchmarks),
2) the most energy-efficient design (EDP) (Js) (as measured by the lowest geometric mean of normalized energy-delay product across all benchmarks).
3) the "best" performance and area oriented design (ED2AP) (Js^2mm^s)
4) the most energy-efficient, area, and performance oriented design (EDAP) (Jsmm^2)
 
The code to perform this exploration is incomplete in the following two ways:

    The validation function that checks if a given configuration complies with project/simplescalar/logical constraints is not fully implemented (it currently only checks that each dimension is within bounds independently, but does not check their interactions). You will need to write the validation function to enforce these interdimensional checks (i.e. the L2 must be bigger than the L1, etc.)
    The proposal function that generates the next configuration to explore (i.e. moves to the next of the 1000 sample points) currently generates a completely random next configuration, thereby not exploring the space of possible configurations with any intelligence or efficiency. Use your knowledge of performance and efficiency tradeoffs to heuristically narrow the search space such that the performance (ED^2P) and efficiency (EDP) options explore a different (overlapping is fine) set of 1000 designs

 
Modeling Considerations:
 
The IC (instruction count) for each benchmark is a constant. Thus, for performance, you will be trying to optimize sim_IPC and the clock cycle time. We will use the following very simplistic model for clock cycle time: The clock cycle time is determined by the fetch width and whether the machine is in-order, or dynamic as follows:
 
Dynamic, fetch width = 1 means a 115 ps clock cycle
Static, fetch width = 1 means a 100 ps clock cycle
Dynamic, fetch width = 2 means a 125 ps clock cycle
Static, fetch width = 2 means a 120 ps clock cycle
Dynamic, fetch width = 4 means a 150 ps clock cycle
Static, fetch width = 4 means a 140 ps clock cycle
Dynamic, fetch width = 8 means a 175 ps clock cycle
Static, fetch width = 8 means a 165 ps clock cycle
 
Power and energy settings are as follows:

    Core Leakage power:

Dynamic, fetch width = 1 : 1.5 mW
Static, fetch width = 1 : 1mW
Dynamic, fetch width = 2 : 2 mW
Static, fetch width = 2 : 1.5 mW
Dynamic, fetch width = 4 : 8 mW
Static, fetch width = 4 : 7 mW
Dynamic, fetch width = 8 : 32 mW
Static, fetch width = 8 : 30 mW
 

    Cache and memory access energy, leakage/refresh power (don’t forget instruction fetch when calculating access energy!):

8KB : 20pJ, 0.125mW
16KB : 28pJ, 0.25mW
32KB : 40pJ, 0.5mW
64KB : 56pJ, 1mW
128KB : 80pJ, 2mW
256KB : 112pJ, 4mW
512KB : 160pJ, 8mW
1024KB : 224pJ, 16mW
2048KB : 360pJ, 32mW
Main Memory : 2nJ, 512mW
 

    Energy per committed instruction:

Dynamic, fetch width = 1 : 10pJ
Static, fetch width = 1 : 8pJ
Dynamic, fetch width = 2 : 12pJ
Static, fetch width = 2 : 10pJ
Dynamic, fetch width = 4 : 18pJ
Static, fetch width = 4 : 14pJ
Dynamic, fetch width = 8 : 27pJ
Static, fetch width = 8 : 20pJ
 
Area cost values:
Static scheduled processor (base): fetch width2/2 * 1 sq-mm
Dynamically scheduled processor (base): 4 + fetch width2/3 * 1 sq-mm
Cache: 1 sq-mm/32 KB capacity, all cache levels
LSQ: (size/2)2 * 1/128 sq-mm
RUU: (size/4)2 * 1/128 sq-mm
Branch predictors:
Perfect: 1234567890 sq-mm 
Not-taken : 0 sq-mm
Bimodal: 0.25 sq-mm
2-level: 0.5 sq-mm
comb: 0.75 sq-mm
 
Other setting constraints (for validation) are as follows:

    The il1 block size must match the ifq size (e.g., for the baseline machine the ifqsize is set to 1 word (8B) then the il1 block size is also set to 8B). The dl1 should have the same block size as your il1.
    The ul2 block size must be at least twice your il1 (dl1) block size with a maximum block size of 128B. Your ul2 must be at least as large as il1+dl1 in order to be inclusive.
    The il1 sizes and il1 latencies are linked as follows (the same linkages hold for the dl1 size and dl1 latency):
    il1 = 8 KB (baseline, minimum size) means il1lat = 1
    il1 = 16 KB means il1lat = 2
    il1 = 32 KB means il1lat = 3
    il1 = 64 KB (maximum size) means il1lat = 4
    The above are for direct mapped caches. For 2-way set associative add an additional cycle of latency to each of the above; for 4-way (maximum) add two additional cycles.

     The ul2 sizes and ul2 latencies are linked as follows:
    ul2 = 128 KB (minimum size) means ul2lat = 7
    ul2 = 256 KB (baseline) means ul2lat = 8
    ul2 = 512 KB means ul2 lat = 9
    ul2 = 1024 KB (1 MB) means ul2lat = 10
    ul2 = 2 MB (maximum size) means ul2lat = 11
    The above are for 4-way set associative caches. For 8-way set associative add one additional cycle of latency to each of the above; for 16 way (maximum) add two additional cycles; for 2-way set associative subtract 1 cycle; for direct mapped subtract 2 cycles.
    Miscellaneous
    mplat is fixed at 3
    fetch:speed ratio of no more than 2
    ifqsize can be set to a maximum of 8 words (64B)
    decode:width and issue:width equal to your fetch:ifqsize
    mem:width is fixed at 8B (memory bus width)
    memport can be set to a maximum of 2
    mem:lat is fixed at 51 + 7 cycles for 8 word
    tlb:lat is fixed at 30, maximum tlb size of 512 entries for a 4-way set associative tlb
    ruu:size maximum of 128 (assume must be a power of two)
    lsq:size maximum of 32 (assume must be a power of two)

 
The framework code will evaluate a fixed number of design points per run. This parameter cannot be changed. The key part of your task in this project is to develop the (set of/parameterized) heuristic search function(s) that select(s) the next design point to consider, given either a performance, or an energy efficiency goal, etc.  When generating plots for the final report, the output for each plot should correspond to the framework code being run only once for each of the optimization function options.