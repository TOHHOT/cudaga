#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "random.h"

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

void print_complete(chromo *pool);
int hparseBits(unsigned char* bits, unsigned char* buffer);

int main(int argc, char * argv[])
{
    chromo pool[NUM_INDIVIDUALS];
	chromo *d_pool;
	unsigned *seeds;
	int i;
	mutex *lock;

    if (cudaSetDevice(1) != cudaSuccess) {
        printf("Error: Unable to set device\n");
        return 3;
    }

    srand(time(NULL));

	cudaMalloc((void **) &d_pool, NUM_INDIVIDUALS * sizeof(chromo));
	if (!d_pool) {
		printf("Unable to allocate memory for pool\n");
		return -1;
	}

	seeds = gen_seeds(NUM_THREADS);
	if (!seeds) {
		printf("Unable to allocate memory for seeds\n");
		return -2;
	}

	cudaMalloc((void **) &lock, 2 * sizeof(int));
	if (!lock) {
		printf("Unable to allocate memory for lock\n");
		return -3;
	}

	mutex_init<<<1, 1>>>(lock);
	cudaThreadSynchronize();
	printf("Initialized mutex\n");

	for (i = 0; i < 10; i++) {
    	run_ga<<<1, NUM_THREADS>>>(lock, d_pool, seeds);
		cudaThreadSynchronize();
		printf("Iteration\n");
	}

	cudaMemcpy(pool, d_pool, NUM_INDIVIDUALS * sizeof(chromo), cudaMemcpyDeviceToHost);
	printf("Memcpy Complete\n");

    print_complete(pool);
	printf("Printing Complete\n");

    return 0;
}

void print_complete(chromo *pool)
{
    int numElements = 0;
    unsigned char buffer[2 * GENE_BYTES];
    int i;

    for (i = 0; i < NUM_INDIVIDUALS; i++) {
        numElements = hparseBits(pool[i].bits, buffer);
        if (pool[i].fitness == END_FITNESS) {
            printf(" %d\n", pool[i].fitness);
            break;
        }
    }
    if (i == NUM_INDIVIDUALS) printf("None - Last: %d\n", pool[NUM_INDIVIDUALS - 1].fitness);

    for (i = 0; i < (numElements - 1); i += 2) {
        switch (buffer[i]) {
        case 10:
            printf("+ %d ", buffer[i + 1]);
            break;
        case 11:
            printf("- %d ", buffer[i + 1]);
            break;
        case 12:
            printf("* %d ", buffer[i + 1]);
            break;
        case 13:
            printf("/ %d ", buffer[i + 1]);
            break;
        default:
            printf("Error ");
            break;
        }
    }
    printf("\n");

    if (!numElements) {
        printf("There was not Solution\n");
    }

    return;
}

int hparseBits(unsigned char* bits, unsigned char* buffer)
{
    unsigned char isOperator = 1;   // Want an operator next
    unsigned char temp;
    int index = 0;
    int i;

    for (i = 0; i < GENE_BYTES; i++) {
        temp = (bits[i] & 0x0F);
        if (isOperator) {
            if (!((temp < 10) || (temp > 13))) {
                isOperator = 0;
                buffer[index++] = temp;
            }
        } else {
            if (!(temp > 9)) {
                isOperator = 1;
                buffer[index++] = temp;
            }
        }
        temp = (bits[i] & 0xF0) >> 4;
        if (isOperator) {
            if (!((temp < 10) || (temp > 13))) {
                isOperator = 0;
                buffer[index++] = temp;
            }
        }
        else {
            if (!(temp > 9)) {
                isOperator = 1;
                buffer[index++] = temp;
            }
        }
    }


    for (i = 0; i < index; i++) {
        if ((buffer[i] == 13) && (buffer[i+1] == 0))
            buffer[i] = 10;
    }

    return index;
}

