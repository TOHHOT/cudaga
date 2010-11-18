#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "c_ga.h"

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

int create_individual(chromo *parents, chromo *child);

int init_individual(chromo *ind);

int calc_fitness(chromo *ind);
