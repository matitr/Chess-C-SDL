#ifndef DEFINES_H
#define DEFINES_H

#define WINDOWAPP_DEFAULT {NULL,NULL}
#define BOARD_SIZE 704
#define FRAME 30
#define MENU_SIZE 336
#define SQUARE_SIZE (BOARD_SIZE / 8)
#define DEFAULT_TEXT_SIZE (BOARD_SIZE + FRAME) / 20
#define MAIN_MENU_SIZE (2 * FRAME + BOARD_SIZE)
#define MAIN_MENU_CENTER (MAIN_MENU_SIZE / 2)
#define GAME_SIZE (BOARD_SIZE + 2 * FRAME + MENU_SIZE)

#define WHITE 0
#define BLACK 1

#define KING 5
#define QUEEN 4
#define ROOK 3
#define BISHOP 2
#define KNIGHT 1
#define PAWN 0

#define FIELD 6
#define MOVE 7
#define TEXTURES_SIZE 8
#define SELECTED 0
#define POSS_MOVE 1
#define QUIT 0 // eventType
#define CLOSE -1 // close program
#define PLAYER 0
#define ENEMY 1
#define FALSE 0
#define TRUE 1
#define TOP 0 //Position
#define BOTTOM 1 //Position
#define DELETE 0
#define DELETE_ALL 3
#define REMOVE 0
#define ADD 1
#define GET 2
#define PLAYER 0
#define COMPUTER 1
#define DRAW -1

// QUIT 0
#define NO_CHOICE -1
#define GET_SAVE 1
#define I_PLAYER 2
#define II_PLAYERS 3
#define II_COMPUTERS 4


// Drawing menu
#define SEPARATOR_SIZE 3
#define FIELD_SIZE 40
#define FIELD_1 (FRAME + FIELD_SIZE / 2) // Y ready to render text
#define FIELD_2 (FRAME + FIELD_SIZE + SEPARATOR_SIZE + FIELD_SIZE / 2)
#define FIELD_3 (FRAME + 2 * (FIELD_SIZE + SEPARATOR_SIZE) + FIELD_SIZE / 2) 
#define FIELD_4 (FRAME + 3 * (FIELD_SIZE + SEPARATOR_SIZE) + FIELD_SIZE / 2) // *HISTORY FIELD
#define FIELD_4_SEPARATE_SIZE (FIELD_SIZE * 0.79)
#define FIELD_4_SIZE (15 * FIELD_SIZE * 0.79)
#define FIELD_5 (FRAME + 7 * SQUARE_SIZE + SQUARE_SIZE / 2)
#define SCROLLBAR_WIDTH 14
#define SCROLLBAR_MAX_H (FIELD_4_SEPARATE_SIZE * 13 + FIELD_4_SEPARATE_SIZE / 2)
#define SCROLLBAR_MIN_HEIGHT (FIELD_4_SEPARATE_SIZE / 2)

#define UP 1
#define DOWN -1
#define RENDER 0
#define CLEAR 8

#define PAST_MOVES_LETTER 25
#define HISTORY_SIZE 15
#define HISTORY_MOVE_WHITE_X (MAIN_MENU_SIZE + 3 * PAST_MOVES_LETTER)
#define HISTORY_MOVE_BLACK_X (MAIN_MENU_SIZE + 7.5 * PAST_MOVES_LETTER)

// MEnu event type
#define NO_EVENTS -1
#define PROCESSED 1
#define NOT_PROCESSED 3
#define BACK 5
#define POINT 7

#define GET_DEPTH -1
#define TEMP_MOVE -1

struct Board {
	int chessman;
	int color;
	int position;
};

typedef struct Point {
	int x, y;
}point_t;

typedef struct Move {
	point_t point1, point2;
}move_t;

typedef struct PossibleMoves {
	point_t point;
	struct PossibleMoves *next, *prev;
}moves_t;

typedef struct PastMoves {
	point_t p1, p2;
	struct PastMoves *prev, *next;
	struct Board* pointer;
	int turn;
	char text[5];
}pastMoves_t;

typedef struct PastPromotion {
	struct Board* pointer;
	struct PastPromotion *prev;
}pastPromotion_t;

#endif // !DEFINES_H