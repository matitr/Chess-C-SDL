#ifndef AI_H
#define AI_H
#include "defines.h"
#include "chess.h"
#include "vld.h"
#include "findMoves.h"


typedef struct Values{
	int color, maxDepth;
	struct WindowApp* app;
} values_t;


void computerMove(struct Board *board[8][8], struct WindowApp *app, move_t *bestMove, int color, int depth);
float alphaBetaPruning(struct Board *board[8][8], values_t* values, float alpha, float beta, int depth, int max);
float getValue(struct Board *board[8][8], int color);
float pieceSquareTable(int chessman, int x, int y);
float getSingleValue(int);

// static
int maxDepth(int);
void setMaxDepth(struct Board *board[8][8], struct WindowApp* app, int color);

#endif // !AI_H

