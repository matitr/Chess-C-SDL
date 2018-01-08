#include "findMoves.h"

void findMoves(struct Board *board[8][8], moves_t **head, point_t *p) {
	int chessman = board[p->x][p->y]->chessman;

	if (chessman == PAWN)
		findMovesPawn(board, head, p);
	else if (chessman == KNIGHT)
		findMovesKnight(board, head, p);
	else if (chessman == BISHOP)
		findMovesBishop(board, head, p);
	else if (chessman == ROOK)
		findMovesRook(board, head, p);
	else if (chessman == QUEEN) {
		//findMovesQueen(board, head, p);
		findMovesBishop(board, head, p);
		findMovesRook(board, head, p);
	}
	else if (chessman == KING)
		findMovesKing(board, head, p);

	//delCheckMoves(board, &head, p);
}

void findMovesKnight(struct Board *board[8][8], moves_t **head, point_t *p) {
	int color = board[p->x][p->y]->color; // Color of piece in point "p"
	for (int i = -2; i < 5; i += 4) // For -2 and 2 
		for (int j = -1; j < 2; j += 2) {
			if (inBoard(p->x + i) && inBoard(p->y + j)) { // 
				if (!board[p->x + i][p->y + j] || board[p->x + i][p->y + j]->color != color) { // Square is empty or there is enemy piece
					point_t point = { p->x + i, p->y + j };
					tempMove(board, p, &point);
					if (!check(board, color))
						addToList(head, &point);
					undoMove(board);
				}
			}
			if (inBoard(p->x + j) && inBoard(p->y + i)) {
				if (!board[p->x + j][p->y + i] || board[p->x + j][p->y + i]->color != color) { // Square is empty or there is enemy piece
					point_t point = { p->x + j, p->y + i };
					tempMove(board, p, &point);
					if (!check(board, color))
						addToList(head, &point);
					undoMove(board);
				}

			}
		}
}

void findMovesBishop(struct Board *board[8][8], moves_t **head, point_t *p) {
	int color = board[p->x][p->y]->color; // Color of piece in point "p"
	for (int x = -1; x < 2; x += 2)
		for (int y = -1; y < 2; y += 2) { // For -1 and 1
			int tempX = p->x + x, tempY = p->y + y;
			// Check squares, go diagonally
			while (inBoard(tempX) && inBoard(tempY)
				&& !board[tempX][tempY]) {
				point_t point = { tempX, tempY };
				tempMove(board, p, &point);
				if (!check(board, color))
					addToList(head, &point);
				undoMove(board);
				tempX += x; tempY += y;
			}
			if (inBoard(tempX) && inBoard(tempY) && board[tempX][tempY] // Check last square, if exists and can move
				&& board[tempX][tempY]->color != color) {
				point_t point = { tempX, tempY };
				tempMove(board, p, &point);
				if (!check(board, color))
					addToList(head, &point);
				undoMove(board);
			}
		}
}

void findMovesRook(struct Board *board[8][8], moves_t **head, point_t *p) {
	int color = board[p->x][p->y]->color; // Color of piece in point "p"

	for (int i = -1; i < 2; i += 2) { // For -1 and 1
		int tempX = i + p->x;
		// Check squares in row
		while (inBoard(tempX) && !board[tempX][p->y]) {
			point_t point = { tempX, p->y };
			tempMove(board, p, &point);
			if (!check(board, color))
				addToList(head, &point);
			undoMove(board);
			tempX += i;
		}
		if (inBoard(tempX) && board[tempX][p->y]) { // Check last square, if exists and can move
			if (board[tempX][p->y]->color != color) {
				point_t point = { tempX, p->y };
				tempMove(board, p, &point);
				if (!check(board, color))
					addToList(head, &point);
				undoMove(board);
			}
		}
	}

	for (int i = -1; i < 2; i += 2) { // For -1 and 1
		int tempY = i + p->y;
		// Check squares in colums
		while (inBoard(tempY) && !board[p->x][tempY]) {
			point_t point = { p->x, tempY };
			tempMove(board, p, &point);
			if (!check(board, color))
				addToList(head, &point);
			undoMove(board);
			tempY += i;
		}
		if (inBoard(tempY) && board[p->x][tempY]) { // Check last square, if exists and can move
			if (board[p->x][tempY]->color != color) {
				point_t point = { p->x, tempY };
				tempMove(board, p, &point);
				if (!check(board, color))
					addToList(head, &point);
				undoMove(board);
			}
		}
	}
}

void findMovesKing(struct Board *board[8][8], moves_t **head, point_t *p) {
	int color = board[p->x][p->y]->color; // Color of piece in point "p"
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (!(i == 0 && j == 0)) { // For i (-1 and 1) and for j (-1 and 1)
				if ((inBoard(p->x + i) && inBoard(p->y + j)) && (!board[p->x + i][p->y + j] ||
					(board[p->x + i][p->y + j] && board[p->x + i][p->y + j]->color != color
						&& board[p->x + i][p->y + j]->chessman != KING))) {

					struct Board *temp = board[p->x + i][p->y + j];
					board[p->x + i][p->y + j] = board[p->x][p->y]; // Temporary move
					board[p->x][p->y] = NULL;
					point_t point = { p->x + i, p->y + j };
					if (!check(board, color))
						addToList(head, &point);

					board[p->x][p->y] = board[p->x + i][p->y + j]; // Undo move
					board[p->x + i][p->y + j] = temp;
				}
			}
		}
	}
	findMovesCastling(board, head, p);
}

void findMovesCastling(struct Board *board[8][8], moves_t **head, point_t *p) {
	int color = board[p->x][p->y]->color; // Color of piece in point "p"
										  // Check if can castling
	if (p->x == 4 && (p->y == 0 || p->y == 7) && !check(board, color)) {
		for (int i = -1; i < 2; i = i + 2) { // For -1 and 1, check for two rooks
			pastMoves_t* moves = pastMoves(NULL, NULL, GET);
			float notCheck = TRUE;
			int rookMove = FALSE, kingMove = FALSE;
			int x = i;
			while (p->x + x > 0 && p->x + x < 7 && !board[p->x + x][p->y]) // Go forward rook side, until found piece or border
				x = x + i;
			if (p->x + x == 0 || p->x + x == 7 && board[p->x + x][p->y] && board[p->x + x][p->y]->chessman == ROOK) {
				while (moves && !kingMove) { // Try to find at least one king move in past moves
					if (moves->p1.x == (p->x + x) && moves->p1.y == (p->y + x))
						rookMove = TRUE;
					if (moves->p1.x == p->x && moves->p1.y == p->y)
						kingMove = TRUE;
					moves = moves->prev;
				}
				if (!kingMove && !rookMove) { // Try to find at least one king move in past moves
					point_t point = { 4 + i * 2, p->y };
					tempMove(board, p, &point);
					if (!check(board, color))
						addToList(head, &point);
					undoMove(board);
				}
			}
		}
	}
}

void findMovesPawn(struct Board *board[8][8], moves_t **head, point_t *p) {
	// Find possible moves for pawn
	int color = board[p->x][p->y]->color; // Color of piece in point "p"
	int i = (board[p->x][p->y]->position == TOP) ? 1 : -1; // Check whether the pawn moves up or down
	if (!board[p->x][p->y + i]) { // Check one square forward from pawn
		point_t point = { p->x, p->y + i };
		tempMove(board, p, &point);
		if (!check(board, color))
			addToList(head, &point);
		undoMove(board);
		if (p->y == ((i == 1) ? 1 : 6) && !board[p->x][p->y + 2 * i]) { // Move by 2 squares
			point.y += i;
			tempMove(board, p, &point);
			if (!check(board, color))
				addToList(head, &point);
			undoMove(board);
		}

	}
	if ((p->y + i) > 7 || (p->y + i) < 0) return; // Just in case
	if ((p->x - 1) >= 0 && board[p->x - 1][p->y + i] && // Check if can capture enemy piece
		board[p->x - 1][p->y + i]->color != color) {
		point_t point = { p->x - 1, p->y + i };
		tempMove(board, p, &point);
		if (!check(board, color))
			addToList(head, &point);
		undoMove(board);
	}
	if ((p->x + 1) <= 7 && board[p->x + 1][p->y + i] && // Check if can capture enemy piece
		board[p->x + 1][p->y + i]->color != color) {
		point_t point = { p->x + 1, p->y + i };
		tempMove(board, p, &point);
		if (!check(board, color))
			addToList(head, &point);
		undoMove(board);
	}
}

void addToList(moves_t **head, point_t *point) { // Add point to moves_t **head
	if (!(*head)) {
		(*head) = malloc(sizeof(moves_t));
		if (!(*head)) eventType(QUIT); // Error during memory alocation, quit program
		(*head)->next = NULL;
		(*head)->prev = NULL;
		(*head)->point.x = point->x;
		(*head)->point.y = point->y;
	}
	else {
		moves_t* temp = malloc(sizeof(moves_t));
		if (!temp) eventType(QUIT); // Error during memory alocation, quit program
		temp->prev = NULL;
		temp->next = (*head);
		temp->point.x = point->x;
		temp->point.y = point->y;
		(*head)->prev = temp;
		(*head) = temp;
	}
}

int findInList(moves_t *head, point_t* point) { // Find point in moves_t *head
	moves_t *temp = head;

	while (temp) {
		if (temp->point.x == point->x && temp->point.y == point->y)
			return TRUE;
		temp = temp->next;
	}
	return FALSE;
}

void delFromList(moves_t **head, moves_t **toDel) {
	if ((*toDel) == (*head)) (*head) = (*head)->next;
	if ((*toDel)->next) (*toDel)->next->prev = (*toDel)->prev;
	if ((*toDel)->prev) (*toDel)->prev->next = (*toDel)->next;
	free((*toDel));
	toDel = NULL;
}

void delList(moves_t **head) { // Delete entire list
	moves_t *temp = (*head);
	while (temp) {
		moves_t *toDel = temp;
		temp = temp->next;
		free(toDel);
	}

	head = NULL;
}