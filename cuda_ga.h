#ifndef __CUDA_GA_H__
#define __CUDA_GA_H__

#include "ind_ga.h"

typedef mutex int;
//makes coding easier

__global__ run_ga(mutex *lock, struct chromo *pool);
//this runs the main loop of all the threads which will call insert_roulette, init_individual, and 
//calc_fitness until a solution is found or the max number of “generations” is reached for one 
//of the threads.
//this function assumes that pool has sizeof(struct chromo) * POOL_SIZE bytes allocated in gpu
//memory.
//when this returns the last “generation” of individuals will be stored in the pool in order of fitness
//best first.

__device__ void mutex_lock(mutex *lock);
//lock a global/shared mutex located at lock
__device__ int mutex_unlock(mutex *lock);
//unlock a global/shared mutex located at lock, returns nonzero on fail.
__global__ void mutex_init(mutex *lock);
//this initializes the mutex to be unlocked. this must be a separate call from the cpu to ensure 
//no race conditions from initialization and first lock of other threads.

__device__ int insert_roulette(mutex *lock, struct chromo *pool, struct chromo *locals);
//locks the mutex, then inserts the new individuals to the pool if fit enough
//then selects new individuals using roulette wheel, storing in locals, then unlocks the mutex
//number of individuals inserted into pool is based on NUM_OFFSPRING

__device__ int insert(struct chromo *pool, struct chromo *locals);
//this is called by insert_roulette, good for code separation
__device__ int roulette(struct chromo *pool, struct chromo *locals);
//this is called by insert_roulette, good for code separation

#endif //__CUDA_GA_H__
