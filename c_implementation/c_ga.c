#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "c_ga.h"

int parseBits(char* bits, char* buffer);
void fillGeneRandom(char* bits);

void run_ga(struct chromo *pool)
{
	int i,j,k;

	chromo parents[2];
	chromo child[NUM_OFFSPRING];

	for (i = 0; i < NUM_INDIVIDUALS; i++) {
		init_individual(pool + i);
		calc_fitness(pool + i);
	}

	for (i = 0; i < MAX_GENERATIONS; i++) {
		for (j = 0; j < NUM_THREADS; j++) {
			roulette(pool, parents);
			for (k = 0; k < NUM_OFFSPRING; k++) {
				create_individual(parents, &child[k]);
				calc_fitness(pool, &child[k]);
			}
			insert(pool, child);
		}

		for (j = 0; j < NUm_INDIVIDUALS; j++) {
			if (pool[i].fitness >= END_FITNESS) {
				return;
			}
		}
	}

	return;
}

int insert_roulette(chromo *pool, chromo *locals);

int insert(chromo *pool, chromo *locals);

int create_individual(chromo *parents, chromo *child)
{
	if (!parents || !child)
		return -1;
	
	child->fitness = 0;
	int xpoint = (rand()/(float)RAND_MAX) * GENE_BYTES;
	for (int i = 0; i < xpoint; i++) {
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
	ind->bits = malloc(GENE_BYTES);
	fillGeneRandom(ind->bits);
	return 0;
}

int calc_fitness(chromo *ind)
{
	if (!ind) return -1;
	
	if (ind->fitness)
		return ind->fitness;

	// Relegate the parsing of the bits to a specific function.
	// In this case, each byte of genes can hold two mathematical
	// things (operators or numbers).  Allocate for the worst case.
	char * buffer = malloc(2 * GENE_BYTES);
	int numElements = parseBits(ind->bits, buffer);
	
	// Now the buffer should have a correct series of number/op/number etc
		
	int result = 0;
	for (int i = 0; i < numElements - 1; i++) {
		switch (buffer[i]) {
		  case 10:
			result += buffer[i+1];
			break;
		  case 11:
			result -= buffer[i+1];
			breakl
		  case 12:
			result *= buffer[i+1];
			break;
		  case 13:
			result /= buffer[i+1];
			break;
		  default:
		}
	}

	// Now calculate the fitness
	// They use a very high fitness to say solution has been found.
	// Go with that for now
	if (result == target)
		return 9999;
	else
		return 1000.0/abs(target - result);	
}


// ////////////////////
// Supporting Functions
// ////////////////////

// Parses the bits of an individual into a valid string of mathematical
// operators.  This is number, operator, number, etc.  Anything else is
// thrown out.  Fills the passed buffer with the ops and returns the
// number of elements
int parseBits(char* bits, char* buffer)
{
	char isOperator = 1;	// Want an operator next
	char temp;
	char index = 0;
	for (int i = 0; i < GENE_BYTES; i++) {
		temp = (bits[i] & 0x0F);
		if (isOperator) {
			if ( (temp < 10) || (temp > 13))
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
				isOperator = true;
				buffer[index++] = temp;
				continue;
			}
		// Now do it again for the other half of this char
		temp = (bits[i] & 0xF0) >> 4;
		if (isOperator) {
			if ( (temp < 10) || (temp > 13))
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
				isOperator = true;
				buffer[index++] = temp;
				continue;
			}
		}
	}
	
	// Now the original authors removed any divide by zero risks
	// by replacing any '/' that's followed by a '0' with a '+'

	for (int i = 0; i < index; i++) {
		if ((buffer[i] == 13) && (buffer[i+1] == 0))
			buffer[i] = 10;
	}

	return index;
}

void fillGeneRandom(char* bits)
{
	if (!bits) return;
	int i;
	for (i = 0; i < GENE_BYTES; i++) {
		bits[i] = (rand() / (float)RAND_MAX) * 255;
	}
	return;
}
