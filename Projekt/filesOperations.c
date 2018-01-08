#include "filesOperations.h"
#include <stdio.h>

void saveSetup(int players[]) { // Save which player play which color and PLAYER or COMPUTER
	FILE* file = NULL;
	fopen_s(&file, "save.csv", "w");
	if (!file) return;
	fprintf(file, "%i,%i\n", players[0], players[1]);
	fclose(file);
}

void saveMove(point_t *p1, point_t *p2) { // Save one move
	FILE* file = NULL;
	fopen_s(&file, "save.csv", "a");
	if (!file) return;
	fprintf(file, "%i%i,%i%i\n", p1->x, p1->y, p2->x, p2->y);
	fclose(file);
}

int saveExists() { // Check if save exists
	FILE* file = NULL;
	fopen_s(&file, "save.csv", "r");
	if (!file) return FALSE;;
	char setup[5] = "";
	fgets(setup, 5, file);
	if ((setup[0] - '0' == 0 || setup[0] - '0' == 1) && (setup[2] - '0' == 0 || setup[2] - '0' == 1)) {
		fclose(file);
		return TRUE;
	}
	fclose(file);
	return FALSE;
}

int getSave(struct Board *board[8][8], float players[2]) { // 
	FILE* file = NULL;
	fopen_s(&file, "save.csv", "r");
	if (!file) return;
	int error = FALSE;
	char setup[5] = "";
	fgets(setup, 5, file);
	if ((setup[0] - '0' == 0 || setup[0] - '0' == 1) && (setup[2] - '0' == 0 || setup[2] - '0' == 1)) {
		players[0] = setup[0] - '0'; players[1] = setup[2] - '0';
	}
	else {
		fclose(file);
		return FALSE;
	}

	while (!feof(file) && !error) {
		char text[7] = "";
		fgets(text, 7, file);
		if (text[0] != 0) { // Check if this is the last line ( empty line )
			point_t point1 = { text[0] - '0', text[1] - '0' };
			point_t point2 = { text[3] - '0', text[4] - '0' };
			if (!correctFileMove(board, &point1, &point2)) // Check if move is correct
				error = TRUE;
			else {
				pastMoves(&point1, &point2, ADD);
				makeMove(board, &point1, &point2);
				turnNumber(1);
			}
		}
	}

	if (error) { // Delete everything, when error occurred
		pastMoves_t* pastMove;
		while (pastMove = pastMoves(NULL, NULL, DELETE)) {
			free(pastMove);
			pastMove = NULL;
			turnNumber(-1);
		}
		fclose(file);
		delBoard(board);
		setupBoard(board, WHITE);
		return FALSE;
	}
	fclose(file);
	return TRUE;
}

int correctFileMove(struct Board *board[8][8], point_t *point1, point_t *point2) {
	// Check if move is possible
	int error = FALSE;
	if (!inBoard(point1->x) || !inBoard(point1->y) || !inBoard(point2->x) || !inBoard(point2->y))
		return FALSE;
	moves_t *moves = NULL;
//	printBoard(board);
	findMoves(board, &moves, point1);
	if (!findInList(moves, point2)) { // Not Find in list
		delList(&moves);
		return FALSE;
	}
	return TRUE;
}