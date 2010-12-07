#ifndef __C_GA_H__
#define __C_GA_H__

#define NUM_THREADS 32

#define NUM_OFFSPRING 2

#define DEBUG 0

#define NUM_INDIVIDUALS (NUM_THREADS * NUM_OFFSPRING)

#define LEGAL_MOVE_REWARD 0
#define ILLEGAL_MOVE_PENALTY 100
#define WIN_REWARD 20
#define LOSE_PENALTY 5
#define TIE_REWARD 0

#define MAX_GENERATIONS 100000

#define GENE_BYTES 2
#define CHROMO_LENGTH 2048

#define MUTATION_RATE .001

typedef struct {
	// bits[XX][1] stores next state
	// 3 LSb of bits[XX][0] stores the move
	unsigned char bits[CHROMO_LENGTH][2];
	int fitness;
} chromo;

void run_ga(chromo *pool);

void print_complete(chromo *pool);

int insert_roulette(chromo *pool, chromo *locals, chromo *parents);

int insert(chromo *pool, chromo *locals);

int roulette(chromo *pool, chromo *parents, int sum);

int create_individual(chromo *parents, chromo *child);

int init_individual(chromo *ind);

int calc_fitness(chromo *players);

void gaQuickSort(chromo *arr, int elements);

int gameOver(char board[6][7], char column);

void printBoard(char board[6][7]);

void playMe(chromo *pool);

#endif //__C_GA_H__
