#ifndef _FILESOPERATIONS_H
#define _FILESOPERATIONS_H

#include "defines.h"
#include "chess.h"
#include "findMoves.h"
#include <vld.h>

void saveSetup(int[]);
void saveMove(point_t *p1, point_t *p2);
int saveExists();
int getSave(struct Board *board[8][8], float players[2]);
int correctFileMove(struct Board *board[8][8], point_t *point1, point_t *point2);

#endif // !_FILESOPERATIONS_H
