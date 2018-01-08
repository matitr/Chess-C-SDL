#include "AI.h"
#include <stdio.h>
#include <time.h>

void computerMove(struct Board *board[8][8], struct WindowApp* app, move_t* bestMove, int color, int maxDept) {
	bestMove->point1.x = -1; // -1 mean that no move found yet
	float bestValue = -99999.0;

	values_t values = { color,maxDepth(GET_DEPTH),app }; // maxDepth - how deep search best move
	int movesCount = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
			if (eventType(GET) == QUIT || eventType(GET) == BACK) return;
			moves_t* moves = NULL;
			if (board[i][j] && board[i][j]->color == color) {
				point_t point = { i,j };
				findMoves(board, &moves, &point);
				moves_t* movesHead = moves;

				while (moves) {	// For every move		
					tempMove(board, &point, &moves->point);
					int toChange = FALSE;
						float tempValue = alphaBetaPruning(board, &values, bestValue, 99999.0, 1, FALSE);
						if (tempValue == bestValue)
							if (!(rand() % 30)) toChange = TRUE;
						if (tempValue > bestValue || toChange || bestMove->point1.x == -1) {
							bestValue = tempValue;
							bestMove->point1.x = i;
							bestMove->point1.y = j;
							bestMove->point2.x = moves->point.x;
							bestMove->point2.y = moves->point.y;
						}
						
					undoMove(board);
					moves = moves->next;
				}
				delList(&movesHead);
				moves = NULL;
			}
		}
}

float alphaBetaPruning(struct Board *board[8][8], values_t* values, float alpha, float beta, int depth, int max) {
	float bestValue = 0.0;
	if (eventType(GET) == QUIT || eventType(GET) == BACK) return QUIT;
	if (depth >= values->maxDepth) { // Evaluate value if it is depth max
		bestValue = getValue(board, values->color);
		eventType(processOneEvent(values->app, NULL));
		return bestValue;
	}
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
			if (eventType(GET) == QUIT || eventType(GET) == BACK) return QUIT;
			moves_t* moves = NULL;
			if (board[i][j] && ((max == TRUE && board[i][j]->color == values->color) ||
				(max == FALSE && board[i][j]->color != values->color))) {

				point_t point = { i,j };
				findMoves(board, &moves, &point);
				moves_t* movesHead = moves;
				while (moves) { // For every move
					int changed = FALSE;
					tempMove(board, &point, &moves->point);
					if (max == TRUE) {
						bestValue = alpha; // Temp best
						float tempValue = alphaBetaPruning(board, values, alpha, beta, depth + 1, FALSE);
						undoMove(board); 
						if (tempValue >= beta) { // Cut-off, no need to continue searching on this level (depth)
							delList(&movesHead);
							return 	beta;
						}
						if (tempValue > alpha) { // Better value
							bestValue = tempValue;
							alpha = tempValue;
						}
					}
					else { // max == FALSE
						bestValue = beta; // Temp best
						float tempValue = alphaBetaPruning(board, values, alpha, beta, depth + 1, TRUE);
						undoMove(board);
						if (tempValue <= alpha) { // Cut-off, no need to continue searching on this level (depth)
							delList(&movesHead);
							return 	alpha;
						}
						if (tempValue < beta) { // Better value
							beta = tempValue;
							bestValue = tempValue;
						}
					}
					moves = moves->next;
				}
				delList(&movesHead);
				moves = NULL;
			}
		}
	return 	bestValue;
}

float getValue(struct Board *board[8][8], int color) {
	float value = 0;

	if (check(board, color) && noLegalMove(board, color)) value = -100000.0;
	if (check(board, !color) && noLegalMove(board, !color)) value = 100000.0;

	if (value != 0) {
		int numberOfTempMoves = 0;
		pastMoves_t *temp = pastMoves(NULL, NULL, GET);
		while (temp && temp->turn == TEMP_MOVE) {
			temp = temp->prev;
			numberOfTempMoves++;
		}
		if (value < 0)
			return value - numberOfTempMoves;
		else value + numberOfTempMoves;
	}
	
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
			if (board[i][j]) {
				if (board[i][j]->color == color) { // Add to value
					value = value + getSingleValue(board[i][j]->chessman);
					if (board[i][j]->chessman != KING)
						if (color == WHITE)
							value += pieceSquareTable(board[i][j]->chessman, i, j);
						else value += pieceSquareTable(board[i][j]->chessman, 7-i, 7-j); // color == BLACK, have to get mirror ( 7 - x, 7 - y)
				}
				else { //board[i][j]->color != color     ,   Substract from value
					value = value - getSingleValue(board[i][j]->chessman);
					if (board[i][j]->chessman != KING)
						if (color == WHITE)
							value -= pieceSquareTable(board[i][j]->chessman, i, j);
						else value -= pieceSquareTable(board[i][j]->chessman, 7 - i, 7 - j); // color == BLACK, have to get mirror ( 7 - x, 7 - y)
				}
			}
		}

	return value;
}

float pieceSquareTable(int chessman, int x, int y) {
	static float piecesValue[5][8][8] = { // values of every piece on every possition
		0,  0,  0,  0,  0,  0,  0,  0,		//PAWN
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		5,  5, 10, 25, 25, 10,  5,  5,
		0,  0,  0, 20, 25,  0,  0,  0,
		5, -5,-10,  0,  0,-10, -5,  5,
		5, 10, 10,-20,-20, 10, 10,  5,
		0,  0,  0,  0,  0,  0,  0,  0,

		-50,-40,-30,-30,-30,-30,-40,-50,	//KNIGHT
		-40,  0,  5,  5,  5,  5,  0,-40,
		-30,  0, 12, 15, 15, 12,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 12, 15, 15, 12,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50,

		-20,-10,-10,-10,-10,-10,-10,-20,		//BISHOP
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20,

		0,  0,  0,  0,  0,  0,  0,  0,		//ROOK
		5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		0,  0,  0,  5,  5,  0,  0,  0,

		-20,-10,-10, -5, -5,-10,-10,-20,		//QUEEN
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-5,  0,  5,  5,  5,  5,  0, -5,
		0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20 };

	return piecesValue[chessman][x][y];
}

float getSingleValue(int chessman) {
	switch (chessman) {
	case PAWN: return 100;
	case KNIGHT: return 325;
	case BISHOP: return 330;
	case ROOK: return 525;
	case QUEEN: return 950;
	}
	return 300;
}

int maxDepth(int x) {
	static int maxDepth = -1;
	if (x >= 0)
		maxDepth = x;

	return maxDepth;
}

void setMaxDepth(struct Board *board[8][8], struct WindowApp* app, int color) {
	float optimalMaxTime = 0.3;

	if (pastMoves(NULL, NULL, GET)) // second player (takes more time to think) about 3 times
		optimalMaxTime *= 3;

	for (int i = 0, found = FALSE; !found; i++) {
		time_t start = clock();
		move_t move = { { -1,-1 },{ -1,-1 } };

		maxDepth(i);
		computerMove(board, app, &move, color, i);

		time_t end = clock();
		float seconds = (float)(end - start) / CLOCKS_PER_SEC;
		
		if (seconds > optimalMaxTime) {
			found = TRUE;
			maxDepth(i - 1);
		}
	}
}