#ifndef __IND_GA_H__
#define __IND_GA_H__

#define NUM_THREADS X
//number of threads to be run, must be valid number for video card
#define NUM_OFFSPRING X
//number of offspring each thread will create each “generation”
//must be at least 2
//NUM_OFFSPRING * NUM_THREADS determines the size of the pool of individuals
#define END_FITNESS X
//this specifies a fitness good enough to complete and return results
#define MAX_GENERATIONS
//this specifies that max generations any individual thread can undergo before
//terminating and returning the best results


struct chromo;//this can be redefined by user to match other applications
//holds chromosomes and fitness for an individual
//must contain member called fitness of type int, higher must be better


//following are all functions that can be redefined by a user and GA should work properly
__device__ int create_individual(struct chromo *parents, struct chromo *child);
//this function uses 2 parents to create 1 individual and will be called NUM_OFFSPRING times
//for each thread in each threads generation. returns non-zero on error.
__device__ int init_individual(struct chromo *ind);
//this is used to construct individual, i.e. make sure fitness is cleared if it has not been calculated
//good for redundancy and avoiding code errors. returns non-zero if individual had previous 
//fitness value.
__device__ int individual_ready(struct chromo *ind);
//makes sure fitness has been calculated and individual is ready for pool insertion
//returns non-zero if individual is not ready
__device__ int calc_fitness(struct chromo *ind);
//this calculates fitness for an individual and stores it in the structure
//it will return non-zero if individual has had fitness previously calculated.

#endif //__IND_GA_H__
