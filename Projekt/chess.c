#include "chess.h"
#include <time.h>
#include <stdio.h>


void runChess(int argc, char **argv) {
	srand(time(NULL));
	struct Board *board[8][8] = { NULL };
	setupBoard(board, WHITE); 
	int choice = 0;
	if (argc < 1) // To setup it needs 2 argc, first file name and second game type (np. continue)
		choice = NO_CHOICE;

	if (argc > 1 && '1' - argv[1][0] <= II_COMPUTERS && '1' - argv[1][0] >= GET_SAVE) // Check if correct
		choice = '1' - argv[1][0];
	else choice = NO_CHOICE;

	struct WindowApp app = WINDOWAPP_DEFAULT;
	createWindow(&app);
	if (!loadImages(&app)) return;

	int gameReturn = 0;
	eventType(CLEAR);

	while (eventType(GET) != QUIT) {
		eventType(CLEAR);
		gameReturn = game(board, &app, choice);
		pastMoves(NULL, NULL, DELETE_ALL);

		if (eventType(GET) != QUIT && gameReturn != QUIT)  // Process winner
			processWinner(&app, gameReturn - GAME_RETURN_CONST);

		delBoard(board);
		setupBoard(board, WHITE);
		choice = NO_CHOICE;
	}

	// Close and delete everything before quit
	delBoard(board); 
	delWindowApp(&app);
	delBoard(board);
}

void setupBoard(struct Board *board[8][8], int playerColor) { // Set every piece at the right position
	for (int i = 0; i < 8; i++)
		for (int j = 2; j < 6; j++)
			board[i][j] = NULL;

	for (int i = 0; i < 8; i++) {
		board[i][1] = malloc(sizeof(struct Board));
		if (!board[i][1]) eventType(QUIT); // Error during memory alocation, quit program
		board[i][1]->chessman = PAWN;
		board[i][1]->color = !playerColor;
		board[i][6] = malloc(sizeof(struct Board)); // Error during memory alocation, quit program
		if (!board[i][6]) eventType(QUIT);
		board[i][6]->chessman = PAWN;
		board[i][6]->color = playerColor;
	}

	setupChessman(board, ROOK, 0, playerColor);
	setupChessman(board, KNIGHT, 1, playerColor);
	setupChessman(board, BISHOP, 2, playerColor);

	for (int i = 0; i <= 7; i = i + 7) {
		board[3][i] = malloc(sizeof(struct Board)); // Error during memory alocation, quit program
		if (!board[3][i]) eventType(QUIT);
		board[4][i] = malloc(sizeof(struct Board)); // Error during memory alocation, quit program
		if (!board[4][i]) eventType(QUIT);
		if (playerColor == BLACK) {
			board[3][i]->chessman = KING;
			board[4][i]->chessman = QUEEN;
		}
		else {
			board[3][i]->chessman = QUEEN;
			board[4][i]->chessman = KING;
		}
	}

	board[3][0]->color = !playerColor;
	board[4][0]->color = !playerColor;
	board[3][7]->color = playerColor;
	board[4][7]->color = playerColor;

	for (int i = 0; i < 8; i++) {
		board[i][0]->position = TOP;
		board[i][1]->position = TOP;
		board[i][6]->position = BOTTOM;
		board[i][7]->position = BOTTOM;
	}
}

void setupChessman(struct Board *board[8][8], int chessman, int pos, int player) {
	//Setup chessmans on board
	for (int i = 0; i < 2; i++)
		for (int j = 0; j <= 7; j = j + 7) {
			board[pos + (7 - 2*pos)*i][j] = malloc(sizeof(struct Board));
			if (!board[pos + (7 - 2 * pos)*i][j]) eventType(QUIT); // Error during memory alocation, quit program
			board[pos + (7 - 2 * pos)*i][j]->chessman = chessman;

			board[pos + (7 - 2 * pos)*i][j]->color = (j ? player : !player);
		}
}

void delBoard(struct Board *board[8][8]) { // Delete all pointers in board ( pieces )
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
			if (board[i][j]) {
				free(board[i][j]);
				board[i][j] = NULL;
			}
		}
}

int game(struct Board *board[8][8], struct WindowApp* app, int choice) {
	int turn = WHITE, rotated = FALSE;
	int players[2] = { PLAYER, PLAYER }; // Default 2 players
	if (!setupBeforeGame(board, app, &rotated, &turn, players, choice)) {
		eventType(QUIT);
		return QUIT;
	}
	point_t p1 = { 0,0 };

	while (1) {
		renderInfo(app, (int)getValue(board,turn), turn); // Render informations about game, (value, turn)
		if (turn == WHITE)
			turnNumber(1);
		renderHistory(app, RENDER); // Render past moves in hstory
		move_t move = { { -1,-1 },{ -1,-1 } };
		int changePlayer = FALSE;
		if (players[turn] == PLAYER) {	// player turn
			do {
				if (processEventsGame(app, &p1) == POINT) // Clicked on board
					changePlayer = humanMove(board, app, &move, &p1, turn, rotated); // Player move, return true when move is correct
				if (eventType(GET) == QUIT || eventType(GET) == BACK) // return to menu or quit program
					return QUIT;
			} while (!changePlayer); // repeat until the move is correct
			rotated = (players[!turn] == PLAYER) ? !rotated : rotated; // Rotate if other player is also human
		}
		else {
			if (maxDepth(GET_DEPTH) < 0) setMaxDepth(board, app, turn); // Set max depth for AI
			computerMove(board, app, &move, turn, 4); // computer turn
		}
		if (eventType(GET) == QUIT || eventType(GET) == BACK) return QUIT;
		pastMoves(&move.point1, &move.point2, ADD); // Add past move
		makeMove(board, &move.point1, &move.point2); 
		
		rotateBoard(app, board, rotated); // Render the board properly rotated

		changePlayer = !changePlayer;
		if (noLegalMove(board, !turn)) {
			pastMoves_t* pastMove;
			if (check(board, !turn)) return turn + GAME_RETURN_CONST; // GAME_RETURN_CONST is to know if there is QUIT(0) or WHITE(0) wins
			else return DRAW + GAME_RETURN_CONST;
		}
		turn = !turn;
	}
}

int setupBeforeGame(struct Board *board[8][8], struct WindowApp* app, int *rotated, int *turn, int players[], int choice) {
	SDL_SetWindowSize(app->window, MAIN_MENU_SIZE, MAIN_MENU_SIZE);
	int menuError = FALSE, menuChoice = 0;
	do {
		menuError = FALSE;
		if (choice == NO_CHOICE)
			menuChoice = menu(app, saveExists()); // 0 to change
		else {
			menuChoice = choice;
			choice = NO_CHOICE;
		}

		if (menuChoice == QUIT) return QUIT;
		else if (menuChoice == I_PLAYER) players[rand() % 2] = COMPUTER;
		else if (menuChoice == II_COMPUTERS) { players[0] = COMPUTER; players[1] = COMPUTER; }
		else if (menuChoice == GET_SAVE)
			if (!getSave(board, players)) { // Get save from file
				menuError = TRUE;
				renderText(app, "Blad pliku z zapisem", DEFAULT_TEXT_SIZE / 2, NULL, MAIN_MENU_CENTER, MAIN_MENU_CENTER + 7 * DEFAULT_TEXT_SIZE);
			}
			else {
				if (pastMoves(NULL, NULL, GET))
					*turn = !(board[pastMoves(NULL, NULL, GET)->p2.x][pastMoves(NULL, NULL, GET)->p2.y]->color);
				else *turn = WHITE;
			}
		
	} while (menuError); // repeat until correct operation is selected

	if (menuChoice != GET_SAVE) saveSetup(players);
	if (players[WHITE] == COMPUTER && players[BLACK] == PLAYER)
		*rotated = TRUE;
	else if (players[WHITE] == PLAYER && players[BLACK] == PLAYER)
		*rotated = (*turn == BLACK) ? TRUE : FALSE;

	// Change windows size and draw side menu
	SDL_SetWindowSize(app->window, MAIN_MENU_SIZE + MENU_SIZE, MAIN_MENU_SIZE);
	drawSideMenu(app);
	renderHistory(app, RENDER);

	rotateBoard(app, board, *rotated);
	
	return TRUE;
}

int humanMove(struct Board *board[8][8], struct WindowApp* app, move_t* move, point_t *p1, int turn, int rotated) {
	int changePlayer = FALSE;
	static moves_t *headMoves = NULL;

	if (rotated) { // If board is rotated then rotate point
		p1->x = 7 - p1->x;
		p1->y = 7 - p1->y;
	}
	if (move->point1.x == -1 && board[p1->x][p1->y] && // First point
		board[p1->x][p1->y]->color == turn) {
		move->point1.x = p1->x;
		move->point1.y = p1->y;
		displaySelected(app, &move->point1, rotated);
		findMoves(board, &headMoves, &move->point1);
		displayPossMoves(app, headMoves, rotated);
	}
	else if (move->point1.x != -1) { // Second point
		if (findInList(headMoves, p1)) { // If found in possible moves 
			move->point2.x = p1->x;
			move->point2.y = p1->y;
			changePlayer = TRUE;

			delList(&headMoves);
			headMoves = NULL;
		}
		else { // Not found is possible moves, clear first point
			move->point1.x = -1;
			move->point1.y = -1;
			delList(&headMoves);
			headMoves = NULL;
			rotateBoard(app, board, rotated);
		}
	}
	return changePlayer; // Return TRUE(1) when the selected move is correct
}









int check(struct Board *board[8][8], int color) {
	point_t king = { -1,-1 };
	findKing(board, color, &king);
	int posY = 1;
	if (board[king.x][king.y]->position == BOTTOM)
		posY = -1;

	if (inBoard(king.y + posY)) {   // PAWN
		if (inBoard(king.x - 1) && board[king.x - 1][king.y + posY] && board[king.x - 1][king.y + posY]->chessman == PAWN
			&& board[king.x - 1][king.y + posY]->color != color)
			return TRUE;
		if (inBoard(king.x + 1) && board[king.x + 1][king.y + posY] && board[king.x + 1][king.y + posY]->chessman == PAWN
			&& board[king.x + 1][king.y + posY]->color != color)
			return TRUE;
	}

	for (int i = -2; i < 5; i += 4) // KNIGHT
		for (int j = -1; j < 2; j += 2) {
			if (inBoard(king.x + i) && inBoard(king.y + j)) {
				if (board[king.x + i][king.y + j] && board[king.x + i][king.y + j]->chessman == KNIGHT &&
					board[king.x + i][king.y + j]->color != color)
					return TRUE;
			}
			if (inBoard(king.x + j) && inBoard(king.y + i)) {
				if (board[king.x + j][king.y + i] && board[king.x + j][king.y + i]->chessman == KNIGHT &&
					board[king.x + j][king.y + i]->color != color)
					return TRUE;
			}
		}

	for (int x = -1; x < 2; x += 2) // BISHOP / QUEEN
		for (int y = -1; y < 2; y += 2) {
			int tempX = king.x + x, tempY = king.y + y;
			while (inBoard(tempX) && inBoard(tempY) && !board[tempX][tempY]) {
				tempX += x; tempY += y;
			}
			if (inBoard(tempX) && inBoard(tempY) && board[tempX][tempY]
				&& board[tempX][tempY]->color != color &&
				(board[tempX][tempY]->chessman == BISHOP || board[tempX][tempY]->chessman == QUEEN))
				return TRUE;
		}
	return checkRookQueen(board, color, &king);
}

int checkRookQueen(struct Board *board[8][8], int color, point_t *king) {
	for (int i = -1; i < 2; i += 2) { // ROOK / QUEEN
		int tempX = i + king->x;
		while (inBoard(tempX) && !board[tempX][king->y]) {

			tempX += i;
		}
		if (inBoard(tempX) && board[tempX][king->y] && board[tempX][king->y]->color != color &&
			(board[tempX][king->y]->chessman == ROOK || board[tempX][king->y]->chessman == QUEEN))
			return TRUE;
	}

	for (int i = -1; i < 2; i += 2) { // ROOK / QUEEN
		int tempY = i + king->y;
		while (inBoard(tempY) && !board[king->x][tempY]) {

			tempY += i;
		}
		if (inBoard(tempY) && board[king->x][tempY] && board[king->x][tempY]->color != color &&
			(board[king->x][tempY]->chessman == ROOK || board[king->x][tempY]->chessman == QUEEN))
			return TRUE;
	}

	for (int i = -1; i < 2; i++) { // KING
		for (int j = -1; j < 2; j++) {
			if ((inBoard(king->x + i) && inBoard(king->y + j)) && board[king->x + i][king->y + j] &&
				board[king->x + i][king->y + j]->color != color && board[king->x + i][king->y + j]->chessman == KING)
				return TRUE;
		}
	}

	return FALSE;
}

void delCheckMoves(struct Board *board[8][8], moves_t **head, point_t *p) {
	return;
	if (!(*head)) return; // Return when nothing to delete
	int color = board[p->x][p->y]->color;
	moves_t *move = (*head);
	while (move) { // For every move
		int del = FALSE;
		tempMove(board, p, &move->point);
		if (check(board, color)) { // If check then delete this move
			moves_t *toDel = move;
			move = move->next;
			delFromList(head, &toDel);
		}
		else move = move->next;
		undoMove(board);
	}
}

void findKing(struct Board *board[8][8], int color, point_t *point) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) // For every square in board
			if (board[i][j] && board[i][j]->chessman == KING &&
				board[i][j]->color == color) {
				point->x = i;
				point->y = j;
				return;
			}
}

int noLegalMove(struct Board *board[8][8], int color) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) { // For every square in board
			if (board[i][j] && board[i][j]->color == color) {
				moves_t *head = NULL;
				point_t p = { i,j };
				findMoves(board, &head, &p);
				if (head) { // When found move delList and return FALSE
					delList(&head);
					return FALSE;
				}
			}
		}
	return TRUE;
}

int inBoard(int i) { // Check if coordinate is in board <0,7>
	if (i >= 0 && i <= 7)
		return 1;
	else return 0;
}



void setPastMoveText(struct Board *board[8][8], point_t *p1, point_t *p2) {// Before makeMove,
	pastMoves_t *pastMove = pastMoves(NULL, NULL, GET);
	pastMove->turn = turnNumber(0);
	memset(pastMove->text, 0, 5);
	int piece = board[p1->x][p1->y]->chessman;

	if (piece == KING) // Castling
		if (p1->x - p2->x == 2) { // To left
			strcpy(pastMove->text, "O-O-O");
			return;
		}
		else if (p1->x - p2->x == -2) { // to right
			strcpy(pastMove->text, "O-O");
			return;
		}
		
	static char pieceSymbol[6] = { 0,'N','B','R','Q','K' }; // PAWN-0, KNIGHT-N,BISHOP-B,ROOK QUEEN KING
	char text[2] = "A";

	if (piece == PAWN) // Piece
		pastMove->text[0] = 'a' + p1->x;
	else pastMove->text[0] = pieceSymbol[piece];

	point_t *point = otherSamePieceCanMove(board, p1, p2);
	if (point) {
		if (point->x != p1->x) {
			text[0] = 'a' + p1->x;
			strcat(pastMove->text, text);
		} 
		else {
			text[0] = '1' + p1->y;
			strcat(pastMove->text, text);
		}
	}
	setPastMoveText2(board, p1, p2);
}

void setPastMoveText2(struct Board *board[8][8], point_t *p1, point_t *p2) {
	pastMoves_t *pastMove = pastMoves(NULL, NULL, GET);
	int piece = board[p1->x][p1->y]->chessman;
	char text[2] = "A";

	if (board[p2->x][p2->y]) // Piece capture
		strcat(pastMove->text, "x");

	if (!(piece == PAWN && !board[p2->x][p2->y])) {
		text[0] = 'a' + p2->x;
		strcat(pastMove->text, text);
	}
	text[0] = '1' + p2->y;
	strcat(pastMove->text, text);

	tempMove(board, p1, p2);
	if (check(board, !(board[p2->x][p2->y]->color))) { // Check or checkmate
		if (noLegalMove(board, !(board[p2->x][p2->y]->color))) {
			text[0] = '#'; // checkmate
			strcat(pastMove->text, text);
		}
		else {
			text[0] = '+'; // check
			strcat(pastMove->text, text);
		}
	}
	undoMove(board);
}

point_t* otherSamePieceCanMove(struct Board *board[8][8], point_t *p1, point_t *p2) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (board[i][j] && board[i][j]->chessman == board[p1->x][p1->y]->chessman &&
				p1->x != i && p1->y != j) {
				moves_t *posMoves = NULL;
				point_t point = { i,j };
				findMoves(board, &posMoves, &point);
				if (findInList(posMoves, p2)) {
					delList(&posMoves);
					return &point;
				}
				delList(&posMoves);
			}
	return NULL;
}

void makeMove(struct Board *board[8][8], point_t *point1, point_t *point2) {
	setPastMoveText(board, point1, point2);

	if (board[point1->x][point1->y]->chessman == KING) { // 2 razy ruch czasami
		if (point1->x - point2->x == 2) { // To left
			board[3][point1->y] = board[0][point1->y];
			board[0][point1->y] = NULL;
		}
		else if (point1->x - point2->x == -2) { // to right
			board[5][point1->y] = board[7][point1->y];
			board[7][point1->y] = NULL;
		}
	}
	else if (board[point1->x][point1->y]->chessman == PAWN && (point2->y == 0 || point2->y == 7)) {
		board[point2->x][point2->y] = board[point1->x][point1->y];
		board[point1->x][point1->y] = NULL;
		board[point2->x][point2->y]->chessman = QUEEN;
		return;
	}

	if (board[point2->x][point2->y])
		free(board[point2->x][point2->y]);

	board[point2->x][point2->y] =
		board[point1->x][point1->y];

	board[point1->x][point1->y] = NULL;

	saveMove(point1, point2);
}

void tempMove(struct Board *board[8][8], point_t *point1, point_t *point2) {
	if (board[point1->x][point1->y]->chessman == KING) { // Castling
		if (point1->x - point2->x == 2){ // To left
			board[3][point1->y] = board[0][point1->y];
			board[0][point1->y] = NULL;
		}
		else if (point1->x - point2->x == -2) { // to right
			board[5][point1->y] = board[7][point1->y];
			board[7][point1->y] = NULL;
		}
	}
	else if (board[point1->x][point1->y]->chessman == PAWN && (point2->y == 0 || point2->y == 7)) { // Promotion 
		pastMoves(point1, point2, ADD)->pointer = board[point1->x][point1->y];
		pastMoves(NULL, NULL, GET)->turn = -1;
		pastTempPromotions(board[point2->x][point2->y], ADD); // Add to past promotions and save pointer to captured piece
		board[point2->x][point2->y] = board[point1->x][point1->y];
		board[point1->x][point1->y] = NULL;
		board[point2->x][point2->y]->chessman = QUEEN; // Promotion to QUEEN
		return;
	}

	pastMoves(point1, point2, ADD)->pointer = board[point2->x][point2->y]; // Add to past moves and save pointer to captured piece
	pastMoves(NULL, NULL, GET)->turn = -1;

	board[point2->x][point2->y] = board[point1->x][point1->y];
	board[point1->x][point1->y] = NULL;
}

void undoMove(struct Board *board[8][8]) {
	pastMoves_t *move = pastMoves(NULL, NULL, DELETE);

	if (board[move->p2.x][move->p2.y]->chessman == KING) {
		if (move->p1.x - move->p2.x == 2) { // To left
			board[0][move->p1.y] = board[3][move->p1.y];
			board[3][move->p1.y] = NULL;
		}
		else if (move->p1.x - move->p2.x == -2) { // to right
			board[7][move->p1.y] = board[5][move->p1.y];
			board[5][move->p1.y] = NULL;
		}
	}
	else if (board[move->p2.x][move->p2.y] == move->pointer) { // PROMOTION
		board[move->p1.x][move->p1.y] = board[move->p2.x][move->p2.y];
		board[move->p2.x][move->p2.y] = pastTempPromotions(NULL,REMOVE);
		board[move->p1.x][move->p1.y]->chessman = PAWN;
		free(move);
		return;
	}

	board[move->p1.x][move->p1.y] = board[move->p2.x][move->p2.y];
	board[move->p2.x][move->p2.y] = move->pointer; // Set past captured piece from pointer to oryginal position

	free(move);
	move = NULL;
}

int turnNumber(int x) {
	static int turn = 0;
	turn = turn + x;
	return turn;
}

pastMoves_t* pastMoves(point_t *p1, point_t *p2, float choice) {
	static pastMoves_t* moves = NULL;
	if (p1 == NULL) {
		if (choice == DELETE) { // Remove one element from list, do not free memory
			if (!moves) return NULL;
			pastMoves_t* temp = moves;
			moves = moves->prev;
			if (moves)
				moves->next = NULL;
			return temp;
		}
		else if (choice == DELETE_ALL) { // Free list
			pastMoves_t* temp = moves;
			if (!temp) return NULL;
			int counter = 0;
			while (moves) {
				temp = moves;
				moves = moves->prev;
				free(temp);
				if (counter % 2) // Every 2 moves undo turn
					turnNumber(-1);
				counter++;
			}
			moves = NULL;
			turnNumber(-1); // One more time
			renderHistory(NULL, CLEAR);
			renderNotations(NULL, CLEAR);
			return NULL;
		}
		else return moves; // choice == GET
	}
	return pastMovesAdd(p1, p2, &moves);
}

pastMoves_t* pastMovesAdd(point_t *p1, point_t *p2, pastMoves_t** moves) {
	if (!(*moves)) {
		(*moves) = malloc(sizeof(pastMoves_t));
		if (!(*moves)) eventType(QUIT); // Error during memory alocation, quit program
		(*moves)->prev = NULL;
		(*moves)->next = NULL;
	}
	else {
		pastMoves_t* temp;
		temp = malloc(sizeof(pastMoves_t));
		if (!temp) eventType(QUIT); // Error during memory alocation, quit program
		(*moves)->next = temp;
		temp->prev = (*moves);
		(*moves) = temp;
	}
	(*moves)->p1.x = p1->x;
	(*moves)->p1.y = p1->y;
	(*moves)->p2.x = p2->x;
	(*moves)->p2.y = p2->y;
	(*moves)->next = NULL;
	return (*moves);
}

struct Board* pastTempPromotions(struct Board *pastPiece, int choice) { // Only in temp moves
	static pastPromotion_t* head = NULL;

	if (choice == REMOVE) {
		pastPromotion_t* temp = head;
		struct Board *piecePointer = temp->pointer;
		head = head->prev;
		free(temp);
		return piecePointer;
	}

	// ADD
	if (!head) {
		head = malloc(sizeof(pastPromotion_t));
		if (!head) eventType(QUIT); // Error during memory alocation, quit program
		head->prev = NULL;
	}
	else {
		pastMoves_t* temp;
		temp = malloc(sizeof(pastPromotion_t));
		if (!temp) eventType(QUIT); // Error during memory alocation, quit program
		temp->prev = head;
		head = temp;
	}
	head->pointer = pastPiece;

	return NULL;
}
