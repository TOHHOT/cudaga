#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include "random.h"

// Setup seed for a given thread, pass the location of local seed storage,
// the global array of seeds and the absolute thread location
__device__ void gsrand(unsigned int *val, unsigned int *seeds, int thread)
{
    *val = seeds[thread];

    return;
}

// This will generate a random float between 0 and 1, must pass it the local
// location of the seed value for this thread.
__device__ float grand(unsigned int *seed)
{
    *seed = ((*seed * 1664525) + 1013904223);

    return (((float)*seed) / (4294967295.0f));
}

// This function allocates memory for seeds on the gpu, generates them,
// and copies them over.
unsigned int *gen_seeds(int num_threads)
{
    unsigned int *s_d;
    unsigned int *s_h;
    int i;

    srand(time(NULL));

    s_h = (unsigned int *)malloc(num_threads * sizeof(unsigned int));

    if (!s_h) {
        return NULL;
    }

    cudaMalloc((void **) &s_d, num_threads * sizeof(unsigned int));

    if (!s_d) {
        free(s_h);
        return NULL;
    }
                             
    for (i = 0; i < num_threads; i++) {
        s_h[i] = rand();
    }

    printf("memcpy %d\n", cudaMemcpy(s_d, s_h, num_threads * sizeof(unsigned int), cudaMemcpyHostToDevice));

    free(s_h);

    return s_d;
}

