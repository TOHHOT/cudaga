#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "c_ga.h"

void run_ga(struct chromo *pool)
{
	int i,j;

#ifdef COPY_CUDA
	chromo locals[NUM_THREADS][NUM_OFFSPRING];
	chromo parens[NUM_THREADS][2];

	for (i = 0; i < NUM_INDIVIDUALS; i++) {
		init_individual(pool + i);
		calc_fitness(pool + i);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		memcpy(locals[i], pool + (i * NUM_OFFSPRING), sizeof(chromo) * NUM_OFFSPRING);
	}

	for (k = 0; k < MAX_GENERATION; k++) {
		for (i = 0; i < NUM_THREADS; i++) {
			memcpy(parents[i], locals[i], 2 * sizeof(chromo));
			for (j = 0; j < NUM_OFFPSRING) {
				create_individual(parents, locals[i] + j);
				calc_fitness(locals[i] + j);
			}
			insert_roulette(pool, locals[i]);
		}
	
		for (i = 0; i < NUM_INDIVIDUALS; i++) {
			if (pool[i].fitness >= END_FITNESS) {
				return;
			}
		}
	}

#else
	chromo parents[2];
	chromo child;

	for (i = 0; i < NUM_INDIVIDUALS; i++) {
		init_individual(pool + i);
		calc_fitness(pool + i);
	}

	for (i = 0; i < MAX_GENERATIONS; i++) {
		for (j = 0; j < NUM_INDIVIDUALS; j++) {
			roulette(pool, parents);
			create_individual(parents, &child);
			calc_fitness(pool, &child);
			insert(pool, &child);
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

int create_individual(chromo *parents, chromo *child);

int init_individual(chromo *ind);

int calc_fitness(chromo *ind);
