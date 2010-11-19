#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "c_ga.h"

int parseBits(unsigned char* bits, unsigned char* buffer);
void fillGeneRandom(unsigned char* bits);

void print_complete(chromo *pool)
{
	int numElements = 0;
	unsigned char * buffer;
	int i;

	for (i = 0; i < NUM_INDIVIDUALS; i++) {
		if (pool[i].fitness >= END_FITNESS) {
			buffer = malloc(2 * GENE_BYTES);
			numElements = parseBits(pool[i].bits, buffer);
			break;
		} else {
			printf("%d\n", pool[i].fitness);
		}
	}

	for (i = 0; i < numElements; i += 2) {
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

	if (!numElements) {
		printf("There was not Solution\n");
	}

	return;
}

void run_ga(chromo *pool)
{
    int i,j,k;
    int sum;
    double complete = 0;

    chromo parents[2];
    chromo child[NUM_OFFSPRING];

    sum = 0;
    for (i = 0; i < NUM_INDIVIDUALS; i++) {
        init_individual(pool + i);
        calc_fitness(pool + i);
        sum += pool[i].fitness;
    }
	return;

    printf("               ");
    for (i = 0; i < MAX_GENERATIONS; i++) {
        while (i > (complete * MAX_GENERATIONS)) {
            for (j = 0; j < 16; j++) printf("\b");
            if (complete < .1) printf(" ");
            printf("%3.1lf%% Complete ", complete * 100);
            fflush(stdout);
            complete += .01;
        }
        for (j = 0; j < NUM_THREADS; j++) {
            roulette(pool, parents, sum);
            for (k = 0; k < NUM_OFFSPRING; k++) {
                create_individual(parents, &child[k]);
                calc_fitness(&child[k]);
            }
            sum = insert(pool, child);
        }

        for (j = 0; j < NUM_INDIVIDUALS; j++) {
            if (pool[i].fitness >= END_FITNESS) {
    			for (j = 0; j < 16; j++) printf("\b");
    			printf("%lf%% Complete\n", 100.0);
                return;
            }
        }
    }
    for (j = 0; j < 16; j++) printf("\b");
    printf("%lf%% Complete\n", 100.0);

    return;
}

int insert_roulette(chromo *pool, chromo *locals, chromo *parents)
{
	int fitness_sum;

	fitness_sum = insert(pool, locals);

	roulette(pool, parents, fitness_sum);

	return 0;
}

int insert(chromo *pool, chromo *locals)
{
	signed int i,j,k;
	int fit_sum = 0;
	int worst[NUM_OFFSPRING];
	int flag;

	for (i = 0; i < NUM_OFFSPRING; i++) {
		worst[i] = 250000;
	}
		
	for (j = 0; j < NUM_OFFSPRING; j++) {
		fit_sum = 0;
		for (i = 0; i < NUM_INDIVIDUALS; i++) {
			fit_sum += pool[i].fitness;

			if ((worst[j] == 250000) || 
					(pool[i].fitness < pool[worst[j]].fitness)) {
				flag = 0;
				for (k = j - 1; k >= 0; k--) {
					if (worst[k] == i) {
						flag = 1;
						break;
					}
				}
	
				if (!flag) {
					worst[j] = i;
				}
			}
		}
	}

	for (i = 0; i < NUM_OFFSPRING; i++) {
		for (j = NUM_OFFSPRING; j > 0; j--) {
			if ((pool[worst[j - 1]].fitness < locals[i].fitness)) break;
		}

		if (!j) continue;
		for (k = 0; k < j; k++) {
			memcpy(&pool[worst[k]], &pool[worst[k + 1]], sizeof(chromo));
		}

		fit_sum -= pool[worst[j]].fitness;
		memcpy(pool + worst[j], locals + i, sizeof(chromo));
		fit_sum += pool[worst[j]].fitness;
	}

	return fit_sum;
}

int roulette(chromo *pool, chromo *parents, int sum)
{
	int rand_val = (((float)rand())/RAND_MAX) * sum;
	int total_fit = 0;
	int i;

	for (i = 0; i < NUM_INDIVIDUALS - 1; i++) {
		total_fit += pool[i].fitness;
		if (total_fit > rand_val) break;
	}

	memcpy(parents, &pool[i], sizeof(chromo));
	rand_val = (((float)rand())/RAND_MAX) * sum;
	total_fit = 0;

	for (i = 0; i < NUM_INDIVIDUALS - 1; i++) {
		total_fit += pool[i].fitness;
		if (total_fit > rand_val) break;
	}

	memcpy(parents + 1, &pool[i], sizeof(chromo));

	return 0;
}

int create_individual(chromo *parents, chromo *child)
{
	int xpoint = (rand()/(float)RAND_MAX) * GENE_BYTES;
	int i;
	if (!parents || !child)
		return -1;
	
	child->fitness = 0;

	for (i = 0; i < xpoint; i++) {
		child->bits[i] = parents[0].bits[i];
	}

	for (; i < GENE_BYTES; i++) {
		child->bits[i] = parents[1].bits[i];
	}

	return 0;
}

int init_individual(chromo *ind)
{
	if (!ind) return -1;
	
	ind->fitness = 0;
	fillGeneRandom(ind->bits);

	return 0;
}

int calc_fitness(chromo *ind)
{
	int result = 0;
	int numElements;
	unsigned char * buffer;
	int i;

	if (!ind) return -1;
	
	if (ind->fitness)
		return ind->fitness;

	// Relegate the parsing of the bits to a specific function.
	// In this case, each byte of genes can hold two mathematical
	// things (operators or numbers).  Allocate for the worst case.
	
	// Now the buffer should have a correct series of number/op/number etc
	buffer = malloc(2 * GENE_BYTES);
	numElements = parseBits(ind->bits, buffer);
	for (i = 0; i < numElements - 1; i++) {
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

	// Now calculate the fitness
	// They use a very high fitness to say solution has been found.
	// Go with that for now
	if (result == TARGET_VALUE)
		return 9999;
	else
		return ((int)1000.0/abs(TARGET_VALUE - result));	
}


// ////////////////////
// Supporting Functions
// ////////////////////

// Parses the bits of an individual into a valid string of mathematical
// operators.  This is number, operator, number, etc.  Anything else is
// thrown out.  Fills the passed buffer with the ops and returns the
// number of elements
int parseBits(unsigned char* bits, unsigned char* buffer)
{
	char isOperator = 1;	// Want an operator next
	char temp;
	int index = 0;
	int i;

	for (i = 0; i < GENE_BYTES; i++) {
		temp = (bits[i] & 0x0F);
		if (isOperator) {
			if ((temp < 10) || (temp > 13))
				continue;
			else {
				isOperator = 0;
				buffer[index++] = temp;
				continue;
			}
		} else {
			if (temp > 9)
				continue;
			else {
				isOperator = 1;
				buffer[index++] = temp;
				continue;
			}
		}
		// Now do it again for the other half of this char
		temp = (bits[i] & 0xF0) >> 4;
		if (isOperator) {
			if ((temp < 10) || (temp > 13))
				continue;
			else {
				isOperator = 0;
				buffer[index++] = temp;
				continue;
			}
		}
		else {
			if (temp > 9)
				continue;
			else {
				isOperator = 1;
				buffer[index++] = temp;
				continue;
			}
		}
	}
	
	// Now the original authors removed any divide by zero risks
	// by replacing any '/' that's followed by a '0' with a '+'

	for (i = 0; i < index; i++) {
		if ((buffer[i] == 13) && (buffer[i+1] == 0))
			buffer[i] = 10;
	}

	return index;
}

void fillGeneRandom(unsigned char* bits)
{
	int i;

	if (!bits) return;

	for (i = 0; i < GENE_BYTES; i++) {
		bits[i] = (rand() / (float)RAND_MAX) * 255;
	}
	return;
}

