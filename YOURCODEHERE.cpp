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


int count = 0;

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
    // config params
    
    int config_width[4] = {1,2,4,8};
    int config_fetch_speed[3] = {1,2,4};
    std::string config_schedule_type[2] = {"in-order", "out-of-order"};
    int config_ruu_size[6] = {4,8,16,32,64,128};
    int config_lsq_size[5] = {4,8,16,32,64};
    int config_memports[3] = {1,2,4};
    
    // cache params
    int config_dl1_sets[7] = {128,256,512,1024,2048,4096,8192};
    int config_dl1_ways[5] = {1,2,4,8,16};
    int config_il1_sets[7] = {128,256,512,1024,2048,4096,8192};
    int config_il1_ways[5] = {1,2,4,8,16};

    // ul2 params
    int config_ul2_sets[9] = {128,256,512,1024,2048,4096,8192,16384,32768};
    int config_ul2_blocksize[7] = {16,32,64,128,256,512,1024};
    int config_ul2_ways[5] = {1,2,4,8,16};

    // tlb params
    int config_tlb_sets[5] = {4,8,16,32,64};
    int config_dl1_lat[7] = {1,2,3,4,5,6,7};
    int config_il1_lat[7] = {1,2,3,4,5,6,7};
    int config_ul2_lat[9] = {5,6,7,8,9,10,11,12,13};






    // int config_dl1_lat[7] = {1,2,3,4,5,6,7};
    // int config_il1_lat[7] = {1,2,3,4,5,6,7};
    // int config_ul2_lat[9] = {5,6,7,8,9,10,11,12,13};
    std::string config_bpredictor[6] = {"Perfect", "NotTaken", "Bimodal", "2 level GAp", "2 level PAg", "Combined"};
    


    // is the configuration at least describing 18 integers/indices?
    if (isan18dimconfiguration(configuration) != 1)
        return 0;

    // if it is, lets convert it to an array of ints for use below
    int configurationDimsAsInts[18];
    int returnValue = 1;  // assume true, set 0 if otherwise
    extractConfiguration(configuration, configurationDimsAsInts); // Configuration parameters now available in array
    
    //decode
    int width = config_width[configurationDimsAsInts[0]];
    int fetch_speed = config_fetch_speed[configurationDimsAsInts[1]];
    std::string sched_type = config_schedule_type[configurationDimsAsInts[2]];
    int ruu_size = config_ruu_size[configurationDimsAsInts[3]];
    int lsq_size = config_lsq_size[configurationDimsAsInts[4]];
    int memports = config_memports[configurationDimsAsInts[5]];

    int dl1_sets = config_dl1_sets[configurationDimsAsInts[6]];
    int dl1_ways = config_dl1_ways[configurationDimsAsInts[7]];
    int il1_sets = config_il1_sets[configurationDimsAsInts[8]];
    int il1_ways = config_il1_ways[configurationDimsAsInts[9]];

    int ul2_sets = config_ul2_sets[configurationDimsAsInts[10]];
    int ul2_blocksize = config_ul2_blocksize[configurationDimsAsInts[11]];
    int ul2_ways = config_ul2_ways[configurationDimsAsInts[12]];

    int tlb_sets = config_tlb_sets[configurationDimsAsInts[13]];
    int dl1_lat = config_dl1_lat[configurationDimsAsInts[14]];
    int il1_lat = config_il1_lat[configurationDimsAsInts[15]];
    int ul2_lat = config_ul2_lat[configurationDimsAsInts[16]];
    std::string bpredictor = config_bpredictor[configurationDimsAsInts[17]];
    

    // 
    // FIXME - YOUR VERIFICATION CODE HERE 
    // ...


    //rule 1
    int ifq_size = width * 8;
    int il1_blocksize = ifq_size;
    int dl1_blocksize = il1_blocksize;

    int il1_size = il1_sets * il1_ways * il1_blocksize;
    int dl1_size = dl1_sets * dl1_ways * dl1_blocksize;
    int ul2_size = ul2_sets * ul2_ways * ul2_blocksize;

    // rule 2    
    if (ul2_blocksize < 2 * il1_blocksize || ul2_blocksize > 128){{
        return 0;
    }}
    if (ul2_size < (il1_size + dl1_size)){
        return 0;
    }

    //rule 3
    int il1_lat_expected = l1_latency_table(il1_size / 1024, il1_ways);
    int dl1_lat_expected = l1_latency_table(dl1_size / 1024, dl1_ways);
    if (il1_lat != il1_lat_expected || dl1_lat != dl1_lat_expected){
        return 0;
    }

    //rule 4
    int ul2_lat_expected = ul2_latency_table(ul2_size / 1024, ul2_ways);
    if (ul2_lat != ul2_lat_expected){
        return 0;
    }

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
    algorithm objective:
    1. choose random config
    2. check if valid, if not, choose diff

    config rules:
    1. EDP
    fixed params: width = 1, speed = 2, in-order, perfect predictor
    all other variables will be randomized
    
    2. EDP2
    fixed params: width = 8, seed = 2, out of order, RUU size = 128, LSQ size = 32, Memport = 2, perfect predictor
    all other variables are randomized

    3. EDAP
    fixed params: width = 1, speed = 2 , in-order, RUU size = 5, LSQ size = 4
    random predictor except perfect
    all other variables are randomized

    4. ED2AP
    fixed params: width = 8, speed = 2, out of order, memport = 2
    predict: random except perfect
    all other variables are randomized

    */    
  
    // produces an essentially random proposal
    int configuration[18];
    do {
        for(int dim = 0; dim < 18; dim++){
            configuration[dim] = rand() % GLOB_dimensioncardinality[dim];
        }

        //EDP
        if (optimizeforEDP) {
            configuration[0] = 0; // width = 1
            configuration[1] = 1; // speed = 2
            configuration[2] = 0; // in-order
            configuration[17] = 0; // perfect predictor
        }
        //ED2P
        else if (optimizeforED2P) {
            configuration[0] = 3; // width = 8
            configuration[1] = 1; // speed = 2
            configuration[2] = 1; // out-of-order
            configuration[3] = 5; // RUU size = 128
            configuration[4] = 3; // LSQ size = 32
            configuration[5] = 1; // memport = 2
            configuration[17] = 0; // perfect predictor
        }
        //EDAP
        else if (optimizeforEDAP) {
            configuration[0] = 0; // width = 1
            configuration[1] = 1; // speed = 2
            configuration[2] = 0; // in-order
            configuration[3] = 0; // RUU size = 4
            configuration[4] = 0; // LSQ size = 4
            if (configuration[17] == 0) {
                configuration[17] = 1; // random predictor except perfect
            }
        }
        //ED2AP
        else if (optimizeforED2AP) {
            configuration[0] = 3; // width = 8
            configuration[1] = 1; // speed = 2
            configuration[2] = 1; // out-of-order
            configuration[5] = 1; // memport = 2
            if (configuration[17] == 0) {
                configuration[17] = 1; // random predictor except perfect
            }
        }
        // If no optimization is specified, return the baseline configuration
        else {
            return GLOB_baseline;
        }
    } while (validateConfiguration(compactConfiguration(configuration)) == 0);

    return compactConfiguration(configuration);
}

