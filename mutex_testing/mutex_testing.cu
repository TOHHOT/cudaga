#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include <sm_11_atomic_functions.h>

#include "mutex_testing.h"

#include "random.h"

__device__ mutex gl_mutex[1];

__device__ int mutex_lock(void)
{

	//return lock(block);
	return (lock(gl_mutex));

	//return 0;
}

__device__ int lock(mutex *block)
{
	return atomicCAS(block, 0, 1);
}

__device__ int unlock(mutex *block)
{
	//return atomicCAS(block, 1, 0);
	block[0] = 0;
}

__device__ int mutex_unlock(void)
{
	return unlock(gl_mutex);
	//return unlock(lock);
}

__global__ void mutex_init(mutex *lock)
{
	gl_mutex[0] = 0;
	lock[0] = 0;
	lock[1] = 1;
}
