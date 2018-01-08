#ifndef FIND_MOVES_H
#define FIND_MOVES_H
#include "defines.h"
#include "vld.h"
#include "chess.h"

void findMoves(struct Board *[8][8], moves_t **, point_t *);
void findMovesPawn(struct Board *[8][8], moves_t **, point_t *);
void findMovesKnight(struct Board *[8][8], moves_t **, point_t *);
void findMovesBishop(struct Board *[8][8], moves_t **, point_t *);
void findMovesRook(struct Board *[8][8], moves_t **, point_t *);
void findMovesKing(struct Board *[8][8], moves_t **, point_t *);
void findMovesCastling(struct Board *[8][8], moves_t **, point_t *);

void addToList(moves_t **, point_t*);
int findInList(moves_t *, point_t*);
void delFromList(moves_t **, moves_t **);
void delList(moves_t **);

#endif // ! FIND_MOVES_H
