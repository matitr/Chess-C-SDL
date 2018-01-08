#ifndef CHESS_H
#define CHESS_H
#include "window.h"
#include "filesOperations.h"
#include "AI.h"
#include "vld.h"
#include "findMoves.h"

#define GAME_RETURN_CONST 3

void runChess(int argc, char **argv);
void setupBoard(struct Board *[8][8], int player);
void setupChessman(struct Board *[8][8], int, int, int);
void delBoard(struct Board *[8][8]);
int game(struct Board *[8][8], struct WindowApp*, int);
int setupBeforeGame(struct Board *board[8][8], struct WindowApp* app, int *rotated, int *turn, int players[], int choice);
int humanMove(struct Board *board[8][8], struct WindowApp* app, move_t* move, point_t *p1, int turn, int rotated);

int check(struct Board *[8][8], int);
int checkRookQueen(struct Board *board[8][8], int color, point_t *king);
void findKing(struct Board *[8][8], int, point_t *);
int noLegalMove(struct Board *[8][8], int);
int inBoard(int);

void setPastMoveText(struct Board *[8][8], point_t*, point_t*);
void setPastMoveText2(struct Board *board[8][8], point_t *p1, point_t *p2);
point_t* otherSamePieceCanMove(struct Board *board[8][8], point_t *p1, point_t *p2);
void makeMove(struct Board *[8][8], point_t*, point_t*);
void tempMove(struct Board *[8][8], point_t*, point_t*);
void undoMove(struct Board *[8][8]);

// static
int turnNumber(int);
pastMoves_t* pastMoves(point_t *p1, point_t *p2, float choice);
pastMoves_t* pastMovesAdd(point_t *p1, point_t *p2, pastMoves_t** moves);
struct Board* pastTempPromotions(struct Board *, int);

#endif