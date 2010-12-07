#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "random.h"

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

void print_complete(chromo *pool);
int hparseBits(unsigned char* bits, unsigned char* buffer);
float hcalc_fitness(chromo *ind);

int main(int argc, char * argv[])
{
    chromo pool[NUM_INDIVIDUALS];
	chromo *d_pool;
	unsigned *seeds;
	int i,j;
	double complete = 0;
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

	init_ga<<<1, 1>>>(d_pool, seeds);
	cudaThreadSynchronize();
	printf("Initialized individuals\n");


    printf("               ");
	for (i = 0; i < 10000; i++) {
        if (i > (complete * 1000)) {
            for (j = 0; j < 16; j++) printf("\b");
            if (complete < .1) printf(" ");
            printf("%3.1lf%% Complete ", (complete - (i / 1000)) * 100);
            fflush(stdout);
 
			cudaMemcpy(pool, d_pool, NUM_INDIVIDUALS * sizeof(chromo), cudaMemcpyDeviceToHost);
			cudaThreadSynchronize();

		    print_complete(pool);
			complete += .0025;
        }

	
    	run_ga<<<1, NUM_THREADS>>>(lock, d_pool, seeds);
		cudaThreadSynchronize();
	}

	cudaMemcpy(pool, d_pool, NUM_INDIVIDUALS * sizeof(chromo), cudaMemcpyDeviceToHost);
	printf("Memcpy Complete\n");
	cudaThreadSynchronize();

    print_complete(pool);
	printf("Printing Complete\n");

    return 0;
}

float hcalc_fitness(chromo *ind)
{
    float result = 0;
    int numElements;
    unsigned char buffer[2 * GENE_BYTES];
    int i;

    if (!ind) return -1;

    numElements = hparseBits((unsigned char *)ind->bits, buffer);
    for (i = 0; (i < numElements - 1) && ((i + 1) < (2 * GENE_BYTES)); i+=2) {
        switch (buffer[i]) {
          case 10:
            result += buffer[i+1];
            break;
          case 11:
            result -= buffer[i+1];
            break;
          case 12:
            result *= buffer[i+1];
            break;
          case 13:
            result /= buffer[i+1];
            break;
          default:
            break;
        }
    }

	return result;
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


    for (i = 0; i < (index - 1); i++) {
        if ((buffer[i] == 13) && (buffer[i+1] == 0))
            buffer[i] = 10;
    }

    return index;
}

