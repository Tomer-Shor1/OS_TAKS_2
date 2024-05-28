/*
    This file contains the implementation of the tic-tac-toe game.
    The game is played between the computer and the user.
    

    The computer plays as "X" and the user plays as "O".


    FFLUSH!!!!!!!!!!!!!!!!!!!!!!!


*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to insert the strategy array.
int validate_strategy(const char *strategy) {
    if (strlen(strategy) != 9) return 0;
    
    int digits[10] = {0}; //
    for (int i = 0; i < 9; i++) {
        if (strategy[i] < '1' || strategy[i] > '9') return 0;
        if (digits[strategy[i] - '0']++) return 0; // ספרה שהופיעה כבר
    }
    return 1;
}

// Function to print the board.
void print_board(const char board[9]) {
    for (int i = 0; i < 9; i++) {
        printf("%c", board[i]);
        fflush(stdout);
        if ((i + 1) % 3 == 0) printf("\n");
    }
}

// Function to check if the player has won.
int check_win(const char board[9], char mark) {
    const int win_conditions[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // 
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // 
        {0, 4, 8}, {2, 4, 6}             // 
    };
    for (int i = 0; i < 8; i++) {
        if (board[win_conditions[i][0]] == mark &&
            board[win_conditions[i][1]] == mark &&
            board[win_conditions[i][2]] == mark) {
            return 1;
        }
    }
    return 0;
}

void ttt(const char *strategy) {
    if (!validate_strategy(strategy)) {
        printf("Error\n");
        fflush(stdout); 
        return;
    }

    int priority[9];
    for (int i = 0; i < 9; i++) {
        priority[i] = strategy[i] - '1'; // adjust for matrix indexing.   
    }

    char board[9] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

    print_board(board);
    printf("\n");
    fflush(stdout);

    for (int turn = 0; turn < 9; turn++) {
        if (turn % 2 == 0) { // if its the computer's turn.
            for (int i = 0; i < 9; i++) {
                int pos = priority[i];
                if (board[pos] == ' ') { // if the position is empty.
                    board[pos] = 'X';
                    printf("%d\n", pos + 1); //   
                    fflush(stdout);
                    break;
                }
            }
        } else { // player's turn.
            int move;
            while (1) {
            if (scanf("%d", &move) != 1 || move < 1 || move > 9 || board[move - 1] != ' ') {  // if the input is invalid.
                printf("Invalid move. Please try again.\n");
                fflush(stdout);
            } else {
                board[move - 1] = 'O';
                break;
            }
            }
        }

        print_board(board);
        printf("\n");

        if (check_win(board, 'X')) {
            printf("I win\n");
            fflush(stdout);
            return;
        } else if (check_win(board, 'O')) {
            printf("I lost\n");
            fflush(stdout);
            return;
        }
    }

    printf("DRAW\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error\n");
        fflush(stdout);
        return 1;
    }
    ttt(argv[1]);
    return 0;
}
