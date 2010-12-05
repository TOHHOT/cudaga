#ifndef __IND_GA_H__
#define __IND_GA_H__

#ifndef GENE_LENGTH
#define GENE_LENGTH 4
#endif

#ifndef GENE_BYTES 
#define GENE_BYTES 40
#endif

#ifndef TARGET_VALUE
#define TARGET_VALUE 42
#endif

#ifndef MUTATION_RATE
#define MUTATION_RATE .001f
#endif

#ifndef CHROMO_LENGTH
#define CHROMO_LENGTH 300
#endif


//this can be redefined by user to match other applications
//holds chromosomes and fitness for an individual
//must contain member called fitness of type int, higher must be better
typedef struct {
    unsigned char bits[GENE_BYTES];
	float result;
    int fitness;
} chromo;


//following are all functions that can be redefined by a user and GA should work properly

//this function uses 2 parents to create 1 individual and will be called NUM_OFFSPRING times
//for each thread in each threads generation. returns non-zero on error.
__device__ int create_individual(chromo *parents, chromo *child, unsigned *seed);

__device__ void cpy_ind(chromo *ind, chromo *old);

//this is used to conindividual, i.e. make sure fitness is cleared if it has not been calculated
//good for redundancy and avoiding code errors. returns non-zero if individual had previous 
//fitness value.
__device__ int init_individual(chromo *ind, unsigned *seed);

//this calculates fitness for an individual and stores it in the structure
//it will return non-zero if individual has had fitness previously calculated.
__device__ int calc_fitness(chromo *ind);
__device__ int calc_fitness_one(chromo *ind);

#endif //__IND_GA_H__
