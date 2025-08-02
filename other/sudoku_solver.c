#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

bool check_cell (int board[9][9], int row, int col, int num) {
	// check row
	for (int i = 0; i < 9; i++) {
		if (board[row][i] == num) {
			return false;
		}
	}

	//check col
	for (int i = 0; i < 9; i++) {
		if (board[i][col] == num) {
			return false;
		}
	}

	// check nonant
	int start_row = (row / 3) * 3, start_col = (col / 3) * 3;
	for (int i = start_row; i < start_row + 3; i++) {
		for (int j = start_col; j < start_col + 3; j++) {
			if (i != row && j != col) {
				if (board[i][j] == num) {
					return false;
				}
			}
		}
	}

	return true;
}

int rng (float pzero) {
	float r = (float)rand() / RAND_MAX;
	if (r < pzero) {
		return 0;
	} else {
		return rand() % 9 + 1;
	}
}

void init_board (int board[9][9], float difficulty) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			float p = (float)rand() / RAND_MAX;
			if (p < difficulty) {
				board[i][j] = 0;
			}
		}
	}
}

void display_board (int board[9][9]) {
	for (int i = 0; i < 9; i++) {
		printf("+---+---+---+---+---+---+---+---+---+\n");
		for (int j = 0; j < 9; j++) {
			if (board[i][j] == 0) {
				printf("|   ");
			} else {
				printf("| %d ", board[i][j]);
			}
			if (j == 8) {
				printf("|");
			}
		}
		printf("\n");
	}
	printf("+---+---+---+---+---+---+---+---+---+\n");
}

bool solve_board (int board[9][9]) {	
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (board[i][j] == 0) {
				for (int k = 1; k < 10; k++) {
					if (check_cell(board, i, j, k)) {
						board[i][j] = k;
						// show algo
						system("clear");
						display_board(board);
						if (solve_board(board)) {
							return true;
						}
						board[i][j] = 0;
					}
				}
				return false;
			}
		}
	}
	return true;
}

bool solve_board_rng (int board[9][9]) {	
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (board[i][j] == 0) {
				int k = rng(0);
				do {
					k = rng(0);
					if (check_cell(board, i, j, k)) {
						board[i][j] = k;
						display_board(board);
						if (solve_board(board)) {
							return true;
						}
						board[i][j] = 0;
					}
				} while (!check_cell(board, i, j, k));
				return false;
			}
		}
	}
	return true;
}

void generate_board (int board[9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			board[i][j] = 0;
		}
	}
	
	board[0][0] = rand() % 9 + 1;

	solve_board(board);
}

int main (int argc, char *argv[]) {
	srand(time(NULL));
	
	int solved[9][9];
	int unsolved[9][9];
	
	float difficulty = atof(argv[argc-1]); 
	
	generate_board(solved);
	memcpy(unsolved, solved, sizeof(unsolved));
	init_board(unsolved, difficulty);

	system("clear");
	
	printf("UNSOLVED:\n");
	display_board(unsolved);	
	
	printf("\nSOLVED:\n");
	display_board(solved);

	return 0;
}
