#include "connect4.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(int argc, char * argv[]) {
	chromo computer[1];
	srand(time(NULL));
	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		exit(0);
	}
	readPlayer(computer, argv[1]);

	playMe(computer);


}

void readPlayer(chromo *player, char *filename) {
	int i, r1, r2;
	FILE *fp;


	fp=fopen(filename,"r");

	if(!fp) {
		    printf("Cannot open file.\n");
		    exit(1);
	}

	for (i = 0; i < CHROMO_LENGTH; i++) {
		fscanf(fp, "%d %d", &r1, &r2);
			  player->bits[i][0] = r1;
			  player->bits[i][1] = r2;
	}

	return;
}


void playMe(chromo *pool) {

	chromo *players = &pool[0];

	char gamestate = 0; // 0 for ongoing, 1 for just won, 2 for tie

	// Player 1 is PC, player 2 is Human
	char theBoard[6][7] = { { 0 } }; // 0 is empty, 1 is black, 2 is red
	// The bottom row is row 0, the left column is col 0

	// The bottom cell in each column contains, in bits 4-2, the index
	// of the bottom most empty cell in that column (therefore if bits
	// 4-2 contain 6, the column is FULL

	char state[2] = { 0 };

	char lastMove[2] = { 0 };

	int fitness[2] = { 0 };

	char nextPlay;
	char turn = 0; // 0 for PC, 1 for human

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
	printBoard(theBoard);

	while (!gamestate) {

		if (turn == 0) { 	//PC's turn
			lowByte	= players[turn].bits[((state[turn] << 3) | lastMove[turn ^ 1])][0];
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
				theBoard[0][nextPlay] = ((height + 1) << 2)
						| (theBoard[0][nextPlay] & 3);
				// Place the token in the board
				// Black played
				theBoard[height][nextPlay] = (theBoard[height][nextPlay] & ~3) | 1;

				gamestate = gameOver(theBoard, nextPlay);
			}
		} else {	// Human turn
			printf("Enter move, column 1-7: ");
			scanf("%d",&nextPlay);
			nextPlay--;
			printf("\n");

			// Check for invalid move
			height = ((theBoard[0][nextPlay] & (7 << 2)) >> 2);
			lastMove[turn] = nextPlay;
			if (height > 5) {
				// Illegal move (nub)
				printf("Illegal move \n");
			} else { // Legal move, record it
				// Update the bookkeeping
				theBoard[0][nextPlay] = ((height + 1) << 2) | (theBoard[0][nextPlay] & 3);
				// Place the token in the board
				// Black played
				theBoard[height][nextPlay] = (theBoard[height][nextPlay] & ~3) | 2;

				gamestate = gameOver(theBoard, nextPlay);
			}
		}

		printBoard(theBoard);

		turn ^= 1; // Change turns
		if (seq_illegal_turns == 100) {
			gamestate = 2;
			break;
		}
	}

	if (gamestate == 1) { // Someone just won
		fitness[turn] += WIN_REWARD;
		fitness[turn ^ 1] -= LOSE_PENALTY;

		if (DEBUG)
			printf("%s won:\n", turn ? "Red" : "Black");
	} else if (gamestate == 2) { // It was a tie
		fitness[0] += TIE_REWARD;
		fitness[1] += TIE_REWARD;
		if (DEBUG)
			printf("It was a tie:\n");
	}

	if (DEBUG)
		printBoard(theBoard);

	return;
}


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
		if (DEBUG) printf("H Winning move: R%d C%d\n",height, column);
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
			if (DEBUG) printf("V Winning move: R%d C%d\n",height, column);
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
		if (DEBUG) printf("D/ Winning move: R%d C%d\n",height, column);
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
		if (DEBUG) printf("D\\ Winning move: R%d C%d\n",height, column);
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
		if (DEBUG) printf("Tieing move: R%d C%d\n",height, column);
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


