#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>





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
