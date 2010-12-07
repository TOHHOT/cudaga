#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "c_ga.h"

void gaQuickSort(chromo *arr, int elements);
int gameOver(char board[6][7], char column);
void printBoard(char board[6][7]);

void print_complete(chromo *pool) {
	printf("Something Happened, I guess\n");
	return;
}

void run_ga(chromo *pool) {
	int i, j, k;
	int sum;
	double complete = 0;

	chromo parents[2];
	chromo child[NUM_OFFSPRING];

	sum = 0;
	for (i = 0; i < NUM_INDIVIDUALS; i++) {
		init_individual(pool + i);
	}
	for (i = 0; i < NUM_INDIVIDUALS; i+=2) {
		sum += calc_fitness(pool + i);
	}
	if (NUM_INDIVIDUALS & 0x01)
		calc_fitness(pool + NUM_INDIVIDUALS - 1);


	printf("               ");
	for (i = 0; i < MAX_GENERATIONS; i++) {
		while (i > (complete * MAX_GENERATIONS)) {
			for (j = 0; j < 16; j++)
				printf("\b");
			if (complete < .1)
				printf(" ");
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
			if (pool[j].fitness == END_FITNESS) {
				for (k = 0; k < 16; k++)
					printf("\b");
				printf("%lf%% Complete\n", 100.0);
				printf("Finished Early %d\n", pool[j].fitness);
				return;
			}
		}
	}
	for (j = 0; j < 16; j++)
		printf("\b");
	printf("%lf%% Complete\n", 100.0);

	return;
}

int insert_roulette(chromo *pool, chromo *locals, chromo *parents) {
	int fitness_sum;

	fitness_sum = insert(pool, locals);

	roulette(pool, parents, fitness_sum);

	return 0;
}
// inserts given chromosomes into the pool if fit enough
int insert(chromo *pool, chromo *locals) {
	int i, j, sumFitness = 0, chromo_num = 0;
	chromo leastFit, tmp;

	// 1. find the N lowest fit chromos in *pool, where N is the number of *locals (NUM_OFFSPRING)
	// 2. compare these lowest fit chromos to each of *locals
	// 3. overwrite FIRST of lowest fit chromos with FIRST of *locals IF *locals is more fit
	gaQuickSort(pool, NUM_INDIVIDUALS);
	for (i = 0; i < NUM_OFFSPRING; i++) { // iterate through sorted *pool only up through size of *locals
		leastFit = pool[i];
		for (j = chromo_num; j < NUM_OFFSPRING; j++) { // iterate through *locals
			tmp = locals[j];
			if (tmp.fitness > leastFit.fitness) { // current chromo has better fitness
				// insert this chromo into the pool, overwriting old chromo
				memcpy(&pool[i], &tmp, sizeof(chromo));
				chromo_num++; // this chromo has been inserted, don't check it next iteration
				break; // move to next iteration through *pool
			}
		}
	}

	for (i = 0; i < NUM_INDIVIDUALS; i++) {
		sumFitness = pool[i].fitness;
	}

	return sumFitness;
}

// non-recursive quicksort
void gaQuickSort(chromo *arr, int elements) {

	int MAX_LEVELS = 300; // control

	int i = 0;
	int swap, piv, L, R, beg[MAX_LEVELS], end[MAX_LEVELS];

	beg[0] = 0;
	end[0] = elements;
	while (i >= 0) {
		L = beg[i];
		R = end[i] - 1;
		if (L < R) {
			piv = arr[L].fitness;
			while (L < R) {
				while (arr[R].fitness >= piv && L < R)
					R--;
				if (L < R)
					arr[L++].fitness = arr[R].fitness;
				while (arr[L].fitness <= piv && L < R)
					L++;
				if (L < R)
					arr[R--].fitness = arr[L].fitness;
			}
			arr[L].fitness = piv;
			beg[i + 1] = L + 1;
			end[i + 1] = end[i];
			end[i++] = L;
			if (end[i] - beg[i] > end[i - 1] - beg[i - 1]) {
				swap = beg[i];
				beg[i] = beg[i - 1];
				beg[i - 1] = swap;
				swap = end[i];
				end[i] = end[i - 1];
				end[i - 1] = swap;
			}
		} else {
			i--;
		}
	}
}


int roulette(chromo *pool, chromo *parents, int sum) {
	int rand_val = (((float) rand()) / RAND_MAX) * sum;
	int total_fit = 0;
	int i;

	for (i = 0; i < NUM_INDIVIDUALS - 1; i++) {
		total_fit += pool[i].fitness;
		if (total_fit > rand_val)
			break;
	}

	memcpy(parents, &pool[i], sizeof(chromo));
	rand_val = (((float) rand()) / RAND_MAX) * sum;
	total_fit = 0;

	for (i = 0; i < NUM_INDIVIDUALS - 1; i++) {
		total_fit += pool[i].fitness;
		if (total_fit > rand_val)
			break;
	}

	memcpy(parents + 1, &pool[i], sizeof(chromo));

	return 0;
}

int create_individual(chromo *parents, chromo *child) {
	int xpoint = (rand() / (float) RAND_MAX) * CHROMO_LENGTH;
	int i, x;
	if (!parents || !child)
		return -1;

	child->fitness = 0;

	for (i = 0; i < xpoint; i++) {
		child->bits[i][0] = parents[0].bits[i][0];
		child->bits[i][1] = parents[0].bits[i][1];
	}

	for (; i < GENE_BYTES; i++) {
		child->bits[i][0] = parents[1].bits[i][0];
		child->bits[i][1] = parents[1].bits[i][1];
	}

	for (x = 0; x < CHROMO_LENGTH; x++) {
		for (i = 0; i < (GENE_BYTES * 8); i++) {
			if ((((float) rand()) / RAND_MAX) < MUTATION_RATE) {
				child->bits[x][i / 8] ^= (1 << (i % 8));
			}
		}
	}

	return 0;
}

int init_individual(chromo *ind) {
	if (!ind)
		return -1;
	int i, j;

	ind->fitness = 0;

		for (j = 0; j < CHROMO_LENGTH; j++) {
		for (i = 0; i < GENE_BYTES; i++) {
			ind->bits[j][i] = (rand() / (float) RAND_MAX) * 255;
		}
	}

	return 0;
}

int calc_fitness(chromo *players) {

	char gamestate = 0; // 0 for ongoing, 1 for just won, 2 for tie

	// Player 1 is black, player 2 is red
	char theBoard[6][7] = { { 0 } }; // 0 is empty, 1 is black, 2 is red
	// The bottom row is row 0, the left column is col 0

	// The bottom cell in each column contains, in bits 4-2, the index
	// of the bottom most empty cell in that column (therefore if bits
	// 4-2 contain 6, the column is FULL

	char state[2] = { 0 };

	char lastMove[2] = { 0 };

	int fitness[2] = { 0 };

	char nextPlay;
	char turn = 0; // 0 for player1, 1 for player2

	char lowByte = 0;
	char hiByte = 0;

	char height = 0;
	char seq_illegal_turns = 0;

	if (!players)
		return -1;

	// This function overwrites players' previous
	// fitness, if any

	// First player one must go randomly
	nextPlay = (int) ((rand() / (float) RAND_MAX) * 7);
	// This is the first play so don't check board bounds
	theBoard[0][nextPlay] = 1; // Black plays here
	lastMove[turn] = nextPlay;
	theBoard[0][nextPlay] = (((1 & 7) << 2) | (theBoard[0][nextPlay] & 3));
	turn ^= 1;

	gamestate = 0;
	while (!gamestate) {


		lowByte = players[turn].bits[((state[turn] << 3) | lastMove[turn ^ 1])][0];
		hiByte = players[turn].bits[((state[turn] << 3) | lastMove[turn ^ 1])][1];

		state[turn] = hiByte;
		nextPlay = (7 & lowByte);

		// Check for invalid move
		height = ((theBoard[0][nextPlay] & (7 << 2)) >> 2);
		lastMove[turn] = nextPlay;
		if (height > 5) {
			// Illegal move (nub)
			seq_illegal_turns++;
			fitness[turn] -= ILLEGAL_MOVE_PENALTY;
		} else { // Legal move, record it
			seq_illegal_turns = 0;
			fitness[turn] += LEGAL_MOVE_REWARD;
			// Update the bookkeeping
			theBoard[0][nextPlay] = ((height + 1) << 2) | (theBoard[0][nextPlay] & 3);
			// Place the token in the board
			if (turn) // Red played
				theBoard[height][nextPlay] = (theBoard[height][nextPlay] & ~3) | 2;
			else
				// Black played
				theBoard[height][nextPlay] = (theBoard[height][nextPlay] & ~3) | 1;

			gamestate = gameOver(theBoard, nextPlay);
		}

		//printBoard(theBoard);

		turn ^= 1;		// Change turns
		if (seq_illegal_turns == 100) {
			gamestate = 2;
			break;
		}
	}



	if (gamestate == 1) { // Someone just won
		fitness[turn] += WIN_REWARD;
		fitness[turn ^ 1] -= LOSE_PENALTY;

		printf("%s won:\n", turn ? "Red":"Black");
	} else if (gamestate == 2) { // It was a tie
		fitness[0] += TIE_REWARD;
		fitness[1] += TIE_REWARD;
		printf("It was a tie:\n");
	}

	printBoard(theBoard);

	players[0].fitness = fitness[0];
	players[1].fitness = fitness[1];

	return 0;
}

// ////////////////////
// Supporting Functions
// ////////////////////

int gameOver(char board[6][7], char column) {
	char col, row;
	char count;
	char height = ((board[0][column] & (7 << 2)) >> 2) - 1;

	// Check horizontally first
	count = 1;
	col = column;
	while (col > 0) {
		if ((board[height][col] & 3) == (board[height][col - 1] & 3)) {
			col--;
			count++;
		}
		else
			break;
	}
	col = column;
	while (col < 5) {
		if ((board[height][col] & 3) == (board[height][col + 1] & 3)) {
			col++;
			count++;
	}
		else
			break;
	}
	if (count >= 4) {
		printf("H Winning move: R%d C%d\n",height, column);
		return 1;	// This move won
	}

	//Check vertically next
	count = 1;
	row = height;
	if (height >= 3) {	// Need at least 4 checkers in this column
		while (row > 0) {
			if ((board[row][column] & 3) == (board[row - 1][column] & 3)) {
				row--;
				count++;
			}
			else
				break;
		}
		if (count >= 4) {
			printf("V Winning move: R%d C%d\n",height, column);
			return 1;	// This move won
		}
	}

	// Check for diagonal wins here
	// First check for  /  diagonals
	count = 1;
	col = column;
	row = height;
	while (row > 0 && col > 0) {		// check down,left first
		if ((board[row][col] & 3) == (board[row - 1][col - 1] & 3)) {
			row--;
			col--;
			count++;
		}
		else
			break;
	}
	col = column;
	row = height;
	while (row < 4 && col < 5) {		// check up, right next
		if ((board[row][col] & 3) == (board[row + 1][col + 1] & 3)) {
			row++;
			col++;
			count++;
		}
		else
			break;
	}
	if (count >= 4) {
		printf("D/ Winning move: R%d C%d\n",height, column);
		return 1;	// This move won
	}

	// Now check for  \  diagonals
	count = 1;
	col = column;
	row = height;
	while (row > 0 && col < 5) {		// check down,right first
		if ((board[row][col] & 3) == (board[row - 1][col + 1] & 3)) {
			row--;
			col++;
			count++;
		}
		else
			break;
	}
	col = column;
	row = height;
	while (row < 4 && col > 0) {		// check up, left next
		if ((board[row][col] & 3) == (board[row + 1][col - 1] & 3)) {
			row++;
			col--;
			count++;
		}
		else
			break;
	}
	if (count >= 4) {
		printf("D\\ Winning move: R%d C%d\n",height, column);
		return 1;	// This move won
	}

	// Check for a tie (full board)
	count = 1;
	for (col = 0; col < 7; col++) {
		height = ((board[0][col] & (7 << 2)) >> 2);
		if (height < 6) {
			count = 0;
			break;
		}
	}
	if (count) {
		printf("Tieing move: R%d C%d\n",height, column);
		return 2;	// It was a tie!
	}
	return 0;

}

void printBoard(char board[6][7]) {
	int row, col;
	for (row = 5; row >= 0; row--) {
		printf("|");
		for (col = 0; col < 7; col++) {
			if ((board[row][col] & 3) == 0)
				printf(" |");
			else if ((board[row][col] & 3) == 1)
				printf("x|");
			else if ((board[row][col] & 3) == 2)
				printf("o|");
			else
				printf("?|");
		}
		printf("\n");
	}
	printf(" -+-+-+-+-+-+- \n\n");
}
