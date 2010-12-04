#include "connect4.h"

int play_game(chromo *players) {

	char gamestate = 0; // 0 for ongoing, 1 for just won, 2 for tie

	// Player 1 is black, player 2 is red
	char theBoard[6][7] = { { 0 } }; // 0 is empty, 1 is black, 2 is red
	// The bottom row is row 0, the left column is col 0

	// The bottom cell in each column contains, in bits 4-2, the index
	// of the bottom most empty cell in that column (therefore if bits
	// 4-2 contain 6, the column is FULL

	char state[2] = { 0 };

	char lastMove[2] = { 0 };

	char nextPlay;

	char lowByte = 0;
	char hiByte = 0;

	int turn;

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
	lastMove[1] = nextPlay;
	theBoard[0][nextPlay] = (((1 & 7) << 2) | (theBoard[0][nextPlay] & 3));

	while (1) {//!(gamestate = gameOver(theBoard, nextPlay))) {
		turn = 0;

		lowByte = players[0].bits[((state[0] << 3) | lastMove[1])][0];
		hiByte = players[0].bits[((state[0] << 3) | lastMove[1])][1];

		state[turn] = hiByte;
		nextPlay = (7 & lowByte);

		// Check for invalid move
		height = ((theBoard[0][nextPlay] & (7 << 2)) >> 2);
		lastMove[turn] = nextPlay;
		if (height > 5) {
			// Illegal move (nub)
			printIllegal();
		} else { // Legal move, record it
			// Update the bookkeeping
			theBoard[0][nextPlay] = ((height + 1) << 2) | (theBoard[0][nextPlay] & 3);
			// Black played
			theBoard[height][nextPlay] = (theBoard[height][nextPlay] & ~3) | 1;
		}

		if (seq_illegal_turns == 100) {
			gamestate = 2;
			break;
		}

		printBoard(theBoard);
		if (gamestate = gameOver(theBoard, nextPlay)) break;
		turn = 1;
	
		nextPlay = getNextMove(theBoard);

		// Check for invalid move
		height = ((theBoard[0][nextPlay] & (7 << 2)) >> 2);	//printBoard(theBoard);
		
		// Update the bookkeeping
		theBoard[0][nextPlay] = ((height + 1) << 2) | (theBoard[0][nextPlay] & 3);
		// Player Moved
		theBoard[height][nextPlay] = (theBoard[height][nextPlay] & ~3) | 2;
		lastMove[turn] = nextPlay;

		printBoard(theBoard);
		if (gamestate = gameOver(theBoard, nextPlay)) break;
	}

	if (gamestate == 1) { // Someone just won
		printf("%s won:\n", turn ? "Red":"Black");
	} else if (gamestate == 2) { // It was a tie
		printf("It was a tie:\n");
	}

	printBoard(theBoard);

	return 0;
}

void readPlayer(chromo *player) {
	int i, r1, r2;

	for (i = 0; i < CHROMO_LENGTH; i++) {
		scanf("%d %d", &r1, &r2);
		player->bits[i][0] = r1;
		player->bits[i][1] = r2;
	}

	return;
}

void printPlayer(chromo *player) {
	int i, r1, r2;

	for (i = 0; i < CHROMO_LENGTH; i++) {
		printf("%d %d\n", player->bits[i][0], player->bits[i][0]);
	}

	return;
}



void printIllegal(void) {
	printf("Computer Made an Illegal Move\n");

	return;
}

char getNextMove(char board[6][7]) {
	int i = 15;

	printf("Enter your move (columns 0-6): ");

	while (isIllegal(board, i)) scanf("%d", &i);

	return (char)i;
}

int isIllegal(char board[6][7], int i) {
	if (i > ((theBoard[0][nextPlay] & (7 << 2)) >> 2)) {
		printf("Illegal Move\n");
		return 1;
	}

	return 0;
}



int gameOver(char board[6][7], char column) {
	char col, row;
	char count;
	char height = ((board[0][column] & (7 << 2)) >> 2) - 1;

	// Check horizontally first
	count = 1;
	col = column;
	while (col > 0) {
		if ((board[height][col] & 3) == (board[height][--col] & 3))
			count++;
		else
			break;
	}
	col = column;
	while (col < 6) {
		if ((board[height][col] & 3) == (board[height][++col] & 3))
			count++;
		else
			break;
	}
	if (count >= 4)
		return 1;	// This move won

	//Check vertically next
	count = 1;
	row = height;
	if (height >= 3) {	// Need at least 4 checkers in this column
		while (row > 0) {
			if ((board[row][column] & 3) == (board[--row][column] & 3))
				count++;
			else
				break;
		}
		if (count >= 4)
			return 1;	// This move won
	}

	// Check for diagonal wins here
	// First check for  /  diagonals
	count = 1;
	col = column;
	row = height;
	while (row > 0 && col > 0) {		// check down,left first
		if ((board[row][col] & 3) == (board[--row][--col] & 3))
			count++;
		else
			break;
	}
	col = column;
	row = height;
	while (row < 5 && col < 6) {		// check up, right next
		if ((board[row][col] & 3) == (board[++row][++col] & 3))
			count++;
		else
			break;
	}
	if (count >= 4)
		return 1;	// This move won

	// Now check for  \  diagonals
	count = 1;
	col = column;
	row = height;
	while (row > 0 && col < 6) {		// check down,right first
		if ((board[row][column] & 3) == (board[--row][++col] & 3))
			count++;
		else
			break;
	}
	col = column;
	row = height;
	while (row < 5 && col > 0) {		// check up, left next
		if ((board[row][col] & 3) == (board[++row][--col] & 3))
			count++;
		else
			break;
	}
	if (count >= 4)
		return 1;	// This move won

	// Check for a tie (full board)
	count = 1;
	for (col = 0; col < 7; col++) {
		height = ((board[0][col] & (7 << 2)) >> 2);
		if (height < 6) {
			count = 0;
			break;
		}
	}
	if (count)
		return 2;	//it was a tie!
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


