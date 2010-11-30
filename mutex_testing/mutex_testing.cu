#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include <sm_11_atomic_functions.h>

#include "mutex_testing.h"

#include "random.h"

__device__ void mutex_lock(mutex *lock, unsigned int *rand_seed)
{
	int i, rand;
	int x;

	while (atomicCAS(lock, 0, 1)) {
		rand = (int)(grand(rand_seed) * 100);
		for (i = 0; i < rand; i++) {
			x = i;
		}
	}

	return;
}

__device__ int mutex_unlock(mutex *lock)
{
	return !atomicCAS(lock, 1, 0);
}

__global__ void mutex_init(mutex *lock)
{
	lock[0] = 0;
	lock[1] = 1;
}
