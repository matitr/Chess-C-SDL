#include "inGameMenu.h"
#include "SDL_ttf.h"

void drawSideMenu(struct WindowApp* app) {
	static SDL_Color color = { 40, 40, 44, 255 };
	SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(app->renderer);

	SDL_Rect box = { // Rectangle
		MAIN_MENU_SIZE, // x
		FRAME, // y
		MENU_SIZE - FRAME, // w
		BOARD_SIZE }; // h

	SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(app->renderer, &box);
	SDL_RenderPresent(app->renderer); // Draw white rectangle

	int fieldSize = 40;
	for (int i = 0; i < 3; i++) { // Draw 3 fields
		drawLines(app, MAIN_MENU_SIZE, FRAME + i * SEPARATOR_SIZE + (i + 1) * fieldSize, MENU_SIZE - FRAME, &color);
	}

	int endHistory = FRAME + 3 * (SEPARATOR_SIZE + fieldSize) + 12 * fieldSize;// coord y of ending history moves field

	drawLines(app, MAIN_MENU_SIZE, endHistory, MENU_SIZE - FRAME, &color);
	renderText(app, "MENU", PAST_MOVES_LETTER * 2, &color, MAIN_MENU_SIZE + (MENU_SIZE - FRAME) / 2, FIELD_5);

	renderNotations(app, FALSE);
	renderScrollbar(app);

	color.r = 50; color.g = 50; color.b = 50;
	static button_t menuButton = { "MENU", MAIN_MENU_SIZE + (MENU_SIZE - FRAME) / 2, FIELD_5, // text, x, y
		PAST_MOVES_LETTER * 2, &color, -1, -1 };
	app->menuButton = &menuButton;
	renderButton(app, app->menuButton);
}

void renderNotations(struct WindowApp* app, int rotated) { // Render board notations, 1 2 3..., A B C...
	static int prevRotated = -1;
	if (rotated == CLEAR) { prevRotated = CLEAR; return; }
	SDL_SetRenderDrawColor(app->renderer, 40, 40, 44, 255);

	SDL_Rect box = { // Render frame around board
		0, // x
		FRAME + BOARD_SIZE, // y
		MAIN_MENU_SIZE, // w
		FRAME }; // h
	if (prevRotated != rotated) { // Don't render notations, when they are rendered correctly
		SDL_RenderFillRect(app->renderer, &box);
		box.y = 0;
		SDL_RenderFillRect(app->renderer, &box);
		box.w = FRAME; box.h = MAIN_MENU_SIZE;
		SDL_RenderFillRect(app->renderer, &box);
		box.x = FRAME + BOARD_SIZE;
		SDL_RenderFillRect(app->renderer, &box);
		SDL_RenderPresent(app->renderer); // Render cleared frame

		if (!rotated)
			for (int i = 0; i < 8; i++) {
				char *text = 'A' + i;
				renderText(app, &text, 20, NULL, FRAME + i * SQUARE_SIZE + SQUARE_SIZE / 2, FRAME / 2);
				renderText(app, &text, 20, NULL, FRAME + i * SQUARE_SIZE + SQUARE_SIZE / 2, FRAME + BOARD_SIZE + FRAME / 2);

				char *text2 = '1' + i;
				renderText(app, &text2, 20, NULL, FRAME / 2, FRAME + i * SQUARE_SIZE + SQUARE_SIZE / 2);
				renderText(app, &text2, 20, NULL, FRAME + BOARD_SIZE + FRAME / 2, FRAME + i * SQUARE_SIZE + SQUARE_SIZE / 2);
			}
		else
			for (int i = 7; i >= 0; i--) {
				char *text = 'A' + i;
				renderText(app, &text, 20, NULL, FRAME + (7 - i) * SQUARE_SIZE + SQUARE_SIZE / 2, FRAME / 2);
				renderText(app, &text, 20, NULL, FRAME + (7 - i) * SQUARE_SIZE + SQUARE_SIZE / 2, FRAME + BOARD_SIZE + FRAME / 2);

				char *text2 = '1' + i;
				renderText(app, &text2, 20, NULL, FRAME / 2, FRAME + (7 - i) * SQUARE_SIZE + SQUARE_SIZE / 2);
				renderText(app, &text2, 20, NULL, FRAME + BOARD_SIZE + FRAME / 2, FRAME + (7 - i) * SQUARE_SIZE + SQUARE_SIZE / 2);
			}
		prevRotated = rotated;
	}
}

void drawLines(struct WindowApp* app, int x, int y, int size, SDL_Color* color) {
	SDL_Color white = { 255,255,255,255 }; // Default color white
	if (!color) color = &white; // Change to white when there is no color passed to funtion (NULL)

	SDL_SetRenderDrawColor(app->renderer, color->r, color->g, color->b, color->a);
	for (int i = 0; i < SEPARATOR_SIZE; i++)
		SDL_RenderDrawLine(app->renderer, x, y + i, x + size, y + i);
}

void renderHistory(struct WindowApp* app, int move) { // Render past moves in history
	static pastMoves_t *pastMove = NULL;

	if (!pastMove) { // Set first past move
		pastMove = pastMoves(NULL, NULL, GET);
		while (pastMove && pastMove->prev) pastMove = pastMove->prev;
	}

	if (!pastMove) return;

	if (move == RENDER) { // && pastMove->turn + HISTORY_SIZE == turnNumber(0)
		if (pastMoves(NULL, NULL, GET)->prev && pastMoves(NULL, NULL, GET)->turn != pastMoves(NULL, NULL, GET)->prev->turn &&
			pastMove->turn + HISTORY_SIZE == turnNumber(0))
			pastMove = pastMove->next->next;
	}
	else if (move == UP && pastMove->prev) 
		pastMove = pastMove->prev->prev;
	else if (move == DOWN && pastMove->next && pastMove->turn + HISTORY_SIZE - 1 != turnNumber(0))
		pastMove = pastMove->next->next;
	else if (move == CLEAR) { pastMove = NULL; return; }

	clearHistory(app); // Render white rectangle
	renderScrollbar(app); // Render scrollbar after clearing

	SDL_Color color = { 50,50,50,255 }; // Gray color
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("font.ttf", PAST_MOVES_LETTER);
	if (!font) return;

	pastMoves_t *temp = pastMove;
	int lineNumber = 0;

	while (temp && lineNumber < HISTORY_SIZE) { // Render all past moves that can be shown on screen
		int Y = FRAME + 3 * (FIELD_SIZE + SEPARATOR_SIZE) + FIELD_4_SEPARATE_SIZE / 2 + lineNumber * FIELD_4_SEPARATE_SIZE;
		char turnString[5] = "";
		SDL_itoa(temp->turn, turnString, 10);
		strcat(turnString, ".");
		renderTextHistory(app, turnString, PAST_MOVES_LETTER, &color, MAIN_MENU_SIZE + SCROLLBAR_WIDTH, Y);
		renderTextHistory(app, &temp->text, PAST_MOVES_LETTER, &color, HISTORY_MOVE_WHITE_X, Y);

		temp = temp->next;
		if (temp) {
			renderTextHistory(app, &temp->text, PAST_MOVES_LETTER, &color, HISTORY_MOVE_BLACK_X, Y);
			temp = temp->next;
		}
		lineNumber++;
	}
	SDL_RenderPresent(app->renderer);
}

void clearHistory(struct WindowApp* app) {
	SDL_Rect box = { // Rectangle
		MAIN_MENU_SIZE, // x
		FRAME + 3 * (FIELD_SIZE + SEPARATOR_SIZE), // y
		MENU_SIZE - FRAME, // w
		FIELD_4_SIZE }; // h

	SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(app->renderer, &box);
//	SDL_RenderPresent(app->renderer); // Draw white rectangle
}

void renderTextHistory(struct WindowApp* app, char* text, int letterSize, SDL_Color* color, int x, int y) {
	SDL_Color white = { 255,255,255,255 }; // Default color white
	if (!color) color = &white; // Change to white when there is no color passed to funtion (NULL)

	TTF_Init();
	TTF_Font* font = TTF_OpenFont("font.ttf", PAST_MOVES_LETTER);
	if (!font) return;

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, *color);
	SDL_Texture* messageTxt = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);

	int width = 0, height = 0;
	SDL_QueryTexture(messageTxt, NULL, NULL, &width, &height); // Get width and height of the text

	SDL_Rect messageRect = {
		x, // x
		y - height / 2, // y
		width, // w
		height }; // h

	SDL_RenderCopy(app->renderer, messageTxt, NULL, &messageRect);

	// Free memory
	TTF_CloseFont(font);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(messageTxt);
	
	TTF_Quit();
}

void renderScrollbar(struct WindowApp* app) {

	if (turnNumber(0) <= 15 || (turnNumber(0) == 16 &&
		pastMoves(NULL, NULL, GET)->turn == pastMoves(NULL, NULL, GET)->prev->turn)) return; // No need to show scrollbar

	if (app->scrollbar->y + app->scrollbar->h == FIELD_4 + (int)SCROLLBAR_MAX_H &&
		pastMoves(NULL, NULL, GET)->turn != pastMoves(NULL, NULL, GET)->prev->turn) { // Scrollbar is on bottom

		app->scrollbar->h = SCROLLBAR_MAX_H * (HISTORY_SIZE / (float)turnNumber(0));
		app->scrollbar->y = FIELD_4 + (int)SCROLLBAR_MAX_H - app->scrollbar->h;
	}
	else {
		// Correct scrollbar height
		if (pastMoves(NULL, NULL, GET)->turn != pastMoves(NULL, NULL, GET)->prev->turn)
			app->scrollbar->h = SCROLLBAR_MAX_H * (HISTORY_SIZE / (float)turnNumber(0));
		else app->scrollbar->h = SCROLLBAR_MAX_H * (HISTORY_SIZE / ((float)turnNumber(0) - 1));
	}



	SDL_SetRenderDrawColor(app->renderer, 150, 150, 150, 255);
	SDL_RenderFillRect(app->renderer, app->scrollbar);
	SDL_RenderPresent(app->renderer);
}

void renderInfo(struct WindowApp* app, int value, int turn) {
	SDL_Rect box = { // Rectangle
		MAIN_MENU_SIZE, // x
		FRAME + (FIELD_SIZE + SEPARATOR_SIZE), // y
		MENU_SIZE - FRAME, // w
		FIELD_SIZE }; // h

	SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(app->renderer, &box);
	SDL_RenderPresent(app->renderer); // Clear second field

	box.y = FRAME + 2 * (FIELD_SIZE + SEPARATOR_SIZE);
	SDL_RenderFillRect(app->renderer, &box);
	SDL_RenderPresent(app->renderer); // Clear third field

	SDL_Color color = { 50,50,50,255 };

	char text[17] = "Wartosc: ";
	char stringFromInt[8] = "";
	SDL_itoa(value, stringFromInt, 10);
	strcat(text, stringFromInt);
	renderText(app, text, PAST_MOVES_LETTER * 1.3, &color, MAIN_MENU_SIZE + (MENU_SIZE - FRAME) / 2,
		FRAME + (FIELD_SIZE + SEPARATOR_SIZE) + FIELD_SIZE / 2); // y

	if (turn == WHITE)
		strcpy(text, "Tura: Biale");
	else strcpy(text, "Tura: Czarne");

	renderText(app, text, PAST_MOVES_LETTER * 1.3, &color, MAIN_MENU_SIZE + (MENU_SIZE - FRAME) / 2,
		FRAME + 2 * (FIELD_SIZE + SEPARATOR_SIZE) + FIELD_SIZE / 2); // y
}

int processEventMenu(struct WindowApp* app, SDL_Event *event) {
	// Returns NOT_PROCESSED when event is not processed by side menu
	static point_t pressPoint = { -1,-1 }; // Save mouse button down coordinates
	static int buttonDown = FALSE; // Save if the mouse button is pressed
	point_t eventPoint = { event->button.x, event->button.y };


	if (event->type == SDL_MOUSEMOTION) { // Only use for scrollbar
		if (buttonDown) {
			changeScrollbar(app, &pressPoint.y, event->button.y);
			return PROCESSED;
		}
	}
	else if (event->type == SDL_MOUSEBUTTONDOWN) {
		int x = 0;
		if (event->button.x >= app->scrollbar->x && event->button.x <= app->scrollbar->x + SCROLLBAR_WIDTH &&
			event->button.y >= app->scrollbar->y && event->button.y <= app->scrollbar->y + app->scrollbar->h) { // Click on scrollbar
			pressPoint.y = event->button.y;
			buttonDown = TRUE;
			return PROCESSED;
		}		
		else if (pointOnButton(&eventPoint,app->menuButton)) { // Click on menu button
			eventType(BACK);
			return PROCESSED;
		}
		return NOT_PROCESSED;
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		pressPoint.y = -1; // Not pressed
		buttonDown = FALSE; 
		return PROCESSED;
	}

	return PROCESSED;
}

void changeScrollbar(struct WindowApp* app, int *pressedPoint, int movePoint) {
	if (turnNumber(0) <= 15) return; // No need to change

	if (*pressedPoint > movePoint) { // Moved up
		if (*pressedPoint - movePoint > app->scrollbar->h / HISTORY_SIZE && // Moved by H / HISTORY_SIZE
			app->scrollbar->y - 4 > FIELD_4) { 

			*pressedPoint = movePoint;
			app->scrollbar->y -= app->scrollbar->h / HISTORY_SIZE;
			renderHistory(app, UP);
		}
	}
	else { // Moved down app->scrollbar->y + app->scrollbar->h == FIELD_4 + (int)SCROLLBAR_MAX_H
		if (movePoint - *pressedPoint > app->scrollbar->h / HISTORY_SIZE && // Moved by H / HISTORY_SIZE
			app->scrollbar->y + app->scrollbar->h + 4 < FIELD_4 + SCROLLBAR_MAX_H ) {

			*pressedPoint = movePoint;
			app->scrollbar->y += app->scrollbar->h / HISTORY_SIZE;
			renderHistory(app, DOWN);
		}
	}
}

int processWinner(struct WindowApp* app, int winner) {
	SDL_Color color = { 50,50,50,255 };

	char text[25] = "Zwyciesca: ";
	
	if (winner == WHITE) {
		char winnerString[10] = "Biale";
		strcat(text, winnerString);
	}
	else if (winner == BLACK){
		char winnerString[10] = "Czarne";
		strcat(text, winnerString);
	}
	else { // draw
		char winnerString[10] = "Remis";
		strcpy(text, winnerString);
	}

	point_t p = { 0,0 };
	renderText(app, text, PAST_MOVES_LETTER, &color, MAIN_MENU_SIZE + (MENU_SIZE - FRAME) / 2, FIELD_1); // Show winner

	while (eventType(GET) != QUIT && eventType(GET) != BACK) // Process menu until event is QUIT or BACK ( back to menu)
		processEventsGame(app, &p);
}