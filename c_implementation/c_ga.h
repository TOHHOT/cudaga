#ifndef __C_GA_H__
#define __C_GA_H__

#define NUM_THREADS 32

#define NUM_OFFSPRING 2

#define NUM_INDIVIDUALS (NUM_THREADS * NUM_OFFSPRING)

#define END_FITNESS 999

#define MAX_GENERATIONS 3000

#define CHOMO_LENGTH 300

#define GENE_LENGTH 4

#define GENE_BYTES 40

typedef struct {
	unsigned char gene[(CHROMO_LENGTH / GENE_LENGTH)];
	int fitness;
} chromo;

void run_ga(struct chromo *pool);

int insert_roulette(chromo *pool, chromo *locals, chromo *parents);

int insert(chromo *pool, chromo *locals);

int roulette(chromo *pool, chromo *parents, int sum);

int create_individual(chromo *parents, chromo *child);

int init_individual(chromo *ind);

int calc_fitness(chromo *ind);


#endif //__C_GA_H__
