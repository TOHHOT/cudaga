#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include "ind_ga.h"

#define DEBUG 0

__device__ int parseBits(unsigned char* bits, unsigned char* buffer);
__device__ void fillGeneRandom(unsigned char* bits, unsigned *seed);
__device__ int gameOver(char board[6][7], char column);


__host__ void print_complete(chromo *pool) {
	FILE *fp;
	fp = fopen("final_chromo.txt", "w");
	int i, j;
	int fitness;
	if (!fp) {
		printf("Error writing chromosome out\n");
		return;
	}

	i = 0;
	fitness = 0;
	for (j = 0; j < NUM_INDIVIDUALS; j++) {
		if (pool[j].fitness > fitness) {
			fitness = pool[j].fitness;
			i = j;
		}
	}

	for (j = 0; j < CHROMO_LENGTH; j++) {
		fprintf(fp, "%d %d \n", pool[i].bits[j][0],pool[i].bits[j][1]);
	}
	fprintf(fp, "\n");
	fclose(fp);


}

__device__ int create_individual(chromo *parents, chromo *child, unsigned *seed)
{
	int xpoint = (grand(seed)) * CHROMO_LENGTH;
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
			if ((grand(seed)) < MUTATION_RATE) {
				child->bits[x][i / 8] ^= (1 << (i % 8));
			}
		}
	}

	return 0;
}



__device__ int init_individual(chromo *ind, unsigned *seed)
{	
	if (!ind)
		return -1;
	int i, j;

	ind->fitness = 0;

	for (j = 0; j < CHROMO_LENGTH; j++) {
		for (i = 0; i < GENE_BYTES; i++) {
			ind->bits[j][i] = (grand(seed)) * 255;
		}
	}

	return 0;
}

__device__ void cpy_ind(chromo *ind, chromo *old)
{
	int i,j;
	ind->fitness = old->fitness;
	for (j = 0; j < CHROMO_LENGTH; j++) {
		for (i = 0; i < GENE_BYTES; i++) {
			ind->bits[j][i] = old->bits[j][i];
		}
	}

	return;
}

__device__ float gabs(float val) 
{
	if (val > 0) {
		return val;
	} else {
		return -val;
	}
}

__device__ int calc_fitness(chromo *players, unsigned *seed)
{
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
	nextPlay = (int) ((grand(seed) / (float) RAND_MAX) * 7);
	// This is the first play so don't check board bounds
	theBoard[0][nextPlay] = 1; // Black plays here
	lastMove[turn] = nextPlay;
	theBoard[0][nextPlay] = (((1 & 7) << 2) | (theBoard[0][nextPlay] & 3));
	turn ^= 1;

	gamestate = 0;
	while (!gamestate) {

		lowByte = players[turn].bits[((((int)state[turn] << 3) | lastMove[turn ^ 1])) & 0x07ff][0];
		hiByte = players[turn].bits[((((int)state[turn] << 3) | lastMove[turn ^ 1])) & 0x07ff][1];

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
		fitness[turn ^ 1] += WIN_REWARD; // gamestate moved - switching rewards
		fitness[turn] -= LOSE_PENALTY;

#if DEBUG
		printf("%s won:\n", turn ? "Red":"Black");
#endif
	} else if (gamestate == 2) { // It was a tie
		fitness[0] += TIE_REWARD;
		fitness[1] += TIE_REWARD;
#if DEBUG
		printf("It was a tie:\n");
#endif
	}

#if DEBUG
	printBoard(theBoard);
#endif

	players[0].fitness += fitness[0];
	if (players[0].fitness < 1) players[0].fitness = 1;
	players[1].fitness += fitness[1];
	if (players[1].fitness < 1) players[1].fitness = 1;

	return 0;
}

__device__ 
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
#if DEBUG
		printf("H Winning move: R%d C%d\n",height, column);
#endif
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
#if DEBUG
			printf("V Winning move: R%d C%d\n",height, column);
#endif
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
#if DEBUG
		printf("D/ Winning move: R%d C%d\n",height, column);
#endif
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
#if DEBUG
		printf("D\\ Winning move: R%d C%d\n",height, column);
#endif
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
#if DEBUG
		printf("Tieing move: R%d C%d\n",height, column);
#endif
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
