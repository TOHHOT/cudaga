#ifndef __RAND_H__
#define __RAND_H__

#include <cuda_runtime.h>

// This function allocates memory for seeds on the gpu, generates them,
// and copies them over.
__host__ unsigned int *gen_seeds(int num_threads);

// Setup seed for a given thread, pass the location of local seed storage,
// the global array of seeds and the absolute thread location
__device__ void gsrand(unsigned int *val, unsigned int *seeds, int thread);

// This will generate a random float between 0 and 1, must pass it the local
// location of the seed value for this thread.
__device__ float grand(unsigned int *val);

#endif //__RAND_H__
