#ifndef __CUDA_GA_H__
#define __CUDA_GA_H__

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include "ind_ga.h"
#include "mutex_testing/mutex_testing.h"

#ifndef NUM_THREADS
#define NUM_THREADS 256
#endif

#ifndef NUM_OFFSPRING
#define NUM_OFFSPRING 2
#endif

#ifndef END_FITNESS
#define END_FITNESS 9999
#endif

#ifndef MAX_GENERATIONS
#define MAX_GENERATIONS 20
#endif

#define NUM_INDIVIDUALS (NUM_THREADS * NUM_OFFSPRING)

//this runs the main loop of all the threads which will call insert_roulette, init_individual, and 
//calc_fitness until a solution is found or the max number of “generations” is reached for one 
//of the threads.
//this function assumes that pool has sizeof(chromo) * POOL_SIZE bytes allocated in gpu
//memory.
//when this returns the last “generation” of individuals will be stored in the pool in order of fitness
//best first.
__global__ void run_ga(mutex *lock, chromo *pool, unsigned *seed);

//locks the mutex, then inserts the new individuals to the pool if fit enough
//then selects new individuals using roulette wheel, storing in locals, then unlocks the mutex
//number of individuals inserted into pool is based on NUM_OFFSPRING
__device__ int insert_roulette(mutex *lock, chromo *pool, chromo *locals, 
									chromo *parents, unsigned *seed);

//this is called by insert_roulette, good for code separation
__device__ int insert(chromo *pool, chromo *locals);
//this is called by insert_roulette, good for code separation
__device__ int roulette(chromo *pool, chromo *parents, int sum, unsigned *seed);

#endif //__CUDA_GA_H__
