#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct curr {
	int r, c;
};

struct cell {
	int value;
	bool init;	// true means that its an initial value, false means that its empty
};

int rng (float pzero) {
	float r = (float)rand() / RAND_MAX;
	if (r < pzero) {
		return 0;
	} else {
		return rand() % 9 + 1;
	}
}

bool check_cell (struct cell board[9][9], int row, int col, int num) {
	// check row
	for (int i = 0; i < 9; i++) {
		if (board[row][i].value == num) {
			return false;
		}
	}

	//check col
	for (int i = 0; i < 9; i++) {
		if (board[i][col].value == num) {
			return false;
		}
	}

	// check nonant
	int start_row = (row / 3) * 3, start_col = (col / 3) * 3;
	for (int i = start_row; i < start_row + 3; i++) {
		for (int j = start_col; j < start_col + 3; j++) {
			if (i != row && j != col) {
				if (board[i][j].value == num) {
					return false;
				}
			}
		}
	}

	return true;
}

void shuffle (int *array) {
	int n = 9;
	for (int i = 0; i < n - 1; i++) {
		int j = i + rand() / (RAND_MAX / (n - i) + 1);
		int t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

bool solve_board (struct cell board[9][9]) {	
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (board[i][j].value == 0) {
				int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
				shuffle(numbers); 
				for (int k = 0; k < 9; k++) {
					if (check_cell(board, i, j, numbers[k])) {
						board[i][j].value = numbers[k];
						if (solve_board(board)) {
							return true;
						}
						board[i][j].value = 0;
					}
				}
				return false;
			}
		}
	}
	return true;
}

void generate_board (struct cell board[9][9]) {
	int r = rng(0), c = rng(0);
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			board[r][c].value = 0;
		}
	}
	
	board[0][0].value = rand() % 9 + 1;

	solve_board(board);
}

void init_board (struct cell board[9][9], float difficulty) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			float p = (float)rand() / RAND_MAX;
			if (p < difficulty) {
				board[i][j].value = 0;
				board[i][j].init = false;
			}
		}
	}
}

void draw_info(WINDOW *win) {
	wattron(win, A_STANDOUT);
	mvwprintw(win, 1, 9, " WELCOME TO CDOKU ");
	wattroff(win, A_STANDOUT);
	mvwprintw(win, 3, 1, "- move with arrow keys");
	mvwprintw(win, 5, 1, "- 'enter' to insert number");
	mvwprintw(win, 7, 1, "- 'backspace' to delete number");
	mvwprintw(win, 9, 1, "- 'q' to quit");
	box(win, 0, 0);
	refresh();
	wrefresh(win);
}

void draw_board (WINDOW *win, WINDOW *info, struct cell board[9][9], struct curr cursor, int attr) {
	clear();
	draw_info(info);

	
	for (int i = 0; i < 9; i++) {
		mvwprintw(win, i * 2, 0, "+---+---+---+---+---+---+---+---+---+");
		for (int j = 0; j < 9; j++) {
			mvwprintw(win, i * 2 + 1,  j * 4, "| ");
			if (cursor.r == i && cursor.c == j) {
				if (attr == 0) {
					wattron(win, A_STANDOUT);	
				} else if (attr == 1) {
					wattron(win, A_UNDERLINE);
				} else if (attr == 2) {
					wattron(win, A_NORMAL);
				}
				
			}
			if (board[i][j].init == false) {
				if (board[i][j].value == 0) {
					mvwprintw(win, i * 2 + 1,  j * 4 + 2, " ");
				} else {
					mvwprintw(win, i * 2 + 1,  j * 4 + 2, "%d", board[i][j].value);
				}
			} else {
				wattron(win, COLOR_PAIR(1));
				mvwprintw(win, i * 2 + 1,  j * 4 + 2, "%d", board[i][j].value);
				wattroff(win, COLOR_PAIR(1));
			}
			
			wattroff(win, A_STANDOUT);
			wattroff(win, A_UNDERLINE);
			if (j == 8) {mvwprintw(win, i * 2 + 1,  9 * 4, "|");}
		}
	}
	mvwprintw(win, 9 * 2, 0, "+---+---+---+---+---+---+---+---+---+");
	refresh();
	wrefresh(win);
}

bool check_end(struct cell board[9][9], struct cell solved[9][9]) {
	bool check = true;
    for (int i = 0; i < 9; i++) {
    	for (int j = 0; j < 9; j++) {
    		if (board[i][j].value != solved[i][j].value) {
    			check = false;
    		}
    	}
    }
    return check;
}

void handle_key(int key, struct curr *cursor, struct cell board[9][9], WINDOW *win, WINDOW *info) {
	switch (key) {
		case KEY_UP:
			if (cursor->r != 0) cursor->r--;
			break;
			
		case KEY_DOWN:
			if (cursor->r != 8) cursor->r++;
			break;

		case KEY_LEFT:
			if (cursor->c != 0) cursor->c--;
			break;

		case KEY_RIGHT:
			if (cursor->c != 8) cursor->c++;
			break;

		case 113:	// q
			endwin();
			exit(0);

		case 10:	// enter
			if (board[cursor->r][cursor->c].init == false) {
				draw_board(win, info, board, *cursor, 1);
				int num = getch() - '0';
				if (num >= 0 && num <= 9) {
					board[cursor->r][cursor->c].value = num;
				}
			}
			break;

		case KEY_BACKSPACE:
			if (board[cursor->r][cursor->c].init == false) {
				board[cursor->r][cursor->c].value = 0;
			}
						
		default: break;
	}
}

float get_difficulty (WINDOW *win) {
	wattron(win, A_STANDOUT);
	mvwprintw(win, 1, 9, " WELCOME TO CDOKU ");
	wattroff(win, A_STANDOUT);
	mvwprintw(win, 3, 1, "ENTER DIFFICULTY WITH KEYBOARD");
	mvwprintw(win, 5, 1, "- Easy: 1");
	mvwprintw(win, 7, 1, "- Medium: 2");
	mvwprintw(win, 9, 1, "- Hard: 3");
	mvwprintw(win, 11, 1, "- Impossible: 4");
	box(win, 0, 0);
	refresh();
	wrefresh(win);

	int value = getch() - '0';
	
	if (value == 1) {
		return 0.3;
	} else if (value == 2) {
		return 0.5;
	} else if (value == 3) {
		return 0.7;
	} else if (value == 4) {
		return 0.99;
	} else return 0;
}

void end_game(WINDOW *win) {
	clear();
	wattron(win, A_STANDOUT);
	mvwprintw(win, 9, 4, " CONGRATULATIONS YOU WIN!!! ");
	wattroff(win, A_STANDOUT);
	box(win, 0, 0);
	refresh();
	wrefresh(win);
	getch();
}

int main(void) {
	srand(time(NULL));

    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 

	WINDOW *intro = newwin(9 * 2 + 1, 9 * 4 + 1, 0, 0);
    float difficulty = get_difficulty(intro);

	struct cell solved[9][9];

	// Initialize board
    struct cell board[9][9];
    for (int i = 0; i < 9; i++) {
    	for (int j = 0; j < 9; j++) {
    		board[i][j].value = 0;
    		board[i][j].init = true;
    	}
    }
	
    generate_board(board);

    // Copy board to solved
    for (int i = 0; i < 9; i++) {
    	for (int j = 0; j < 9; j++) {
    		solved[i][j].value = board[i][j].value;
    		solved[i][j].init = board[i][j].init;
    	}
    }

    // Make board ready to be played
    init_board(board, difficulty);
    
    struct curr cursor = {0, 0};
    int key;
	bool gameover = false;

	WINDOW *game_win = newwin(9 * 2 + 1, 9 * 4 + 1, 0, 0);
	WINDOW *info_win = newwin(9 * 2 + 1, 9 * 4 + 1, 0, 9 * 4 + 2);
	WINDOW *victory = newwin(9 * 2 + 1, 9 * 4 + 1, 0, 0);

	// Game loop
	while (!gameover) {
		draw_board(game_win, info_win, board, cursor, 0);
		key = getch();
		handle_key(key, &cursor, board, game_win, info_win);
		gameover = check_end(board, solved);
	}
	
	// draw_board(game_win, info_win, board, cursor, 2);
	end_game(victory);
	clear();
	endwin();
    
    return 0;
}
