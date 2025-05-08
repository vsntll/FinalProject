#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>

#include "431project.h"


int count = 0l

//latency tables

int l1_latency_table(int size_kb, int ways) {
    int base_latency;

    // Base latency for direct-mapped (1-way)
    if      (size_kb == 8)  base_latency = 1;
    else if (size_kb == 16) base_latency = 2;
    else if (size_kb == 32) base_latency = 3;
    else if (size_kb == 64) base_latency = 4;
    else return -1;

    // Associativity adjustment
    if      (ways == 1) return base_latency;
    else if (ways == 2) return base_latency + 1;
    else if (ways == 4) return base_latency + 2;
    else return -1; // invalid associativity
}

int ul2_latency_table(int size_kb, int ways) {
    int base_latency;

    // Base latency (for 4-way set associative)
    if      (size_kb == 128)  base_latency = 7;
    else if (size_kb == 256)  base_latency = 8;
    else if (size_kb == 512)  base_latency = 9;
    else if (size_kb == 1024) base_latency = 10;
    else if (size_kb == 2048) base_latency = 11;
    else return -1;  // invalid size

    int offset;  // invalid by default

    // Associativity adjustment
    if      (ways == 1)   offset = -2;
    else if (ways == 2)   offset = -1;
    else if (ways == 4)   offset = 0;
    else if (ways == 8)   offset = 1;
    else if (ways == 16)  offset = 2;
    else return -1;  // invalid associativity

    return base_latency + offset;
}




/*
 * Not all configurations are inherently valid. (For example, L1 > L2). 
 * Returns 1 if valid, else 0.
 */
int validateConfiguration(std::string configuration){
    // Configuration parameters
    int config_width[4] = {1,2,4,8};
    int config_fetch_speed[2] = {1,2,4};
    std::string config_schedule_type[2] = {"in-order", "out-of-order"};
    int config_ruu_size[6] = {0, 4, 8, 16, 32, 64};
    int config_lsq_size[4] = {0, 4, 8, 16, 32};
    int config_memports[2] = {1,2};

    // Cache parameters
    int config_dl1_sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
    int config_dl1_ways[3] = {1,2,4};
    int config_il1_sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
    int config_il1_ways[3] = {1,2,4};

    // L2 Cache parameters
    int config_ul2_sets[10] = {128,256,512,1024,2048,4096,8192,16384,32768};
    int config_ul2_blocksize[4] = {16,32,64,128};
    int config_ul2_ways[5] = {1,2,4,8,16};

    // TLB parameters
    int tlb_sets[5] = {4,8,16,32,64};
    int config_dl1_lat[7] = {1,2,3,4,5,6,7};
    int config_il1_lat[7] = {1,2,3,4,5,6,7};
    int config_ul2_lat[9] = {5,6,7,8,9,10,11,12,13};
    std::string config_bpredictor[6] = {"Perfect", "NotTaken", "Bimodal", "2 level GAp", "2 level PAg", "Combined"};
    


    // is the configuration at least describing 18 integers/indices?
    if (isan18dimconfiguration(configuration) != 1)
        return 0;

    // if it is, lets convert it to an array of ints for use below
    int configurationDimsAsInts[18];
    int returnValue = 1;  // assume true, set 0 if otherwise
    extractConfiguration(configuration, configurationDimsAsInts); // Configuration parameters now available in array
    
    // 
    // FIXME - YOUR VERIFICATION CODE HERE 
    // ...

    return returnValue;
}




/*
 * Given the current best known configuration for a particular optimization, 
 * the current configuration, and using globally visible map of all previously 
 * investigated configurations, suggest a new, previously unexplored design 
 * point. You will only be allowed to investigate 1000 design points in a 
 * particular run, so choose wisely. Using the optimizeForX variables,
 * propose your next configuration provided the optimiztion strategy.
 */
std::string YourProposalFunction(
    std::string currentconfiguration,
    std::string bestED2Pconfiguration,
    std::string bestEDPconfiguration,
    std::string bestEDAPconfiguration,
    std::string bestED2APconfiguration,
    int optimizeforED2P,
    int optimizeforEDP,
    int optimizeforEDAP,
    int optimizeforED2AP
){
    /*
    * REPLACE THE BELOW CODE WITH YOUR PROPOSAL FUNCTION
    *
    * The proposal function below is extremely unintelligent and
    * will produce configurations that, while properly formatted, 
    * violate specified project constraints
    */    
  
    // produces an essentially random proposal
    int configuration[18];
    for(int dim = 0; dim < 18; dim++){
        configuration[dim] = rand() % GLOB_dimensioncardinality[dim];
    }

    return compactConfiguration(configuration);
}

