#include "window.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <vld.h>

void createWindow(struct WindowApp* windowApp) {
	SDL_Init(SDL_INIT_VIDEO);
	windowApp->window = SDL_CreateWindow("Chess",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		BOARD_SIZE + 2 * FRAME, BOARD_SIZE + 2 * FRAME, 0);
	
	windowApp->renderer = SDL_CreateRenderer(windowApp->window, -1, SDL_RENDERER_ACCELERATED);
}
//Wwg&
int loadImages(struct WindowApp* app) { // Load images/textures from files, return FALSE when something is missing
	app->textures[PAWN][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/pawnLight.png"));
	app->textures[PAWN][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/pawnDark.png"));
	app->textures[KNIGHT][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/knightLight.png"));
	app->textures[KNIGHT][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/knightDark.png"));
	app->textures[BISHOP][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/bishopLight.png"));
	app->textures[BISHOP][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/bishopDark.png"));
	app->textures[ROOK][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/rookLight.png"));
	app->textures[ROOK][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/rookDark.png"));
	app->textures[QUEEN][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/queenLight.png"));
	app->textures[QUEEN][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/queenDark.png"));
	app->textures[KING][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/kingLight.png"));
	app->textures[KING][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/kingDark.png"));
	app->textures[FIELD][WHITE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/light.png"));
	app->textures[FIELD][BLACK] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/dark.png"));
	app->textures[MOVE][SELECTED] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/selected.png"));
	app->textures[MOVE][POSS_MOVE] =
		SDL_CreateTextureFromSurface(app->renderer, IMG_Load("Textures/possibleMove.png"));

	for (int i = 0; i < 8; i++) // Check if there is missing texture
		for (int j = 0; j < 2; j++) {
			if (app->textures[i][j] == NULL)
				return FALSE;
		}
	
	return TRUE;
}

int menu(struct WindowApp* app, int saveAvailable) {
	int letterSize = (2 *FRAME + BOARD_SIZE) / 20;

	SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
	SDL_RenderClear(app->renderer);

	initScrollbar(app);

	while (1) {
		SDL_Color color = { 70, 70, 70, 255 }; // Gray color

		if (saveAvailable) {
			color.b = 255; color.g = 255; color.r = 255;
		}

		SDL_RenderClear(app->renderer);
		button_t buttonA2 = { "Kontynuuj", MAIN_MENU_CENTER, MAIN_MENU_CENTER, letterSize, &color, -1, -1 }; // Create button
		moveButtonCoords(&buttonA2, 0, 1.5 * letterSize); // Set additional button values
		renderButton(app, &buttonA2);

		color.b = 255; color.g = 255; color.r = 255;
		button_t buttonA1 = { "Nowa gra", MAIN_MENU_CENTER, MAIN_MENU_CENTER, letterSize, &color, -1, -1 };
		moveButtonCoords(&buttonA1, 0, -1.5 * letterSize); 
		renderButton(app, &buttonA1);

		point_t point = { 0, 0 };
		int menuChoice = -99;
		while (menuChoice != BACK && processEvents(app, &point) != QUIT) { 
			if (pointOnButton(&point, &buttonA1)) {
				if ((menuChoice = newGameMenu(app)) != BACK)
					return menuChoice;
			}
			else if (saveAvailable && pointOnButton(&point, &buttonA2)) {
				return GET_SAVE;
			}
		}
	}
}

void initScrollbar(struct WindowApp* app) {
	if (app->scrollbar) return; // Already initialized

	app->scrollbar = malloc(sizeof(SDL_Rect)); // Create scrollbar
	if (!app->scrollbar) eventType(QUIT); // Error during memory alocation, quit program
	app->scrollbar->x = GAME_SIZE - FRAME - SCROLLBAR_WIDTH - 4; // Set default values
	app->scrollbar->y = FIELD_4;
	app->scrollbar->w = SCROLLBAR_WIDTH;
	app->scrollbar->h = SCROLLBAR_MAX_H;
}

int newGameMenu(struct WindowApp* app) {
	SDL_RenderClear(app->renderer);
	SDL_Color color = { 255, 255, 255, 255 }; // White color
	int letterSize = (2 * FRAME + BOARD_SIZE) / 20;

	button_t buttonB1 = { "I Gracz", MAIN_MENU_CENTER, MAIN_MENU_CENTER, letterSize, &color, -1, -1 };
	moveButtonCoords(&buttonB1, 0, -3.0 * letterSize);
	renderButton(app, &buttonB1);

	button_t buttonB2 = { "II Gracze", MAIN_MENU_CENTER, MAIN_MENU_CENTER, letterSize, &color, -1, -1 };
	moveButtonCoords(&buttonB2, 0, -1.0 * letterSize);
	renderButton(app, &buttonB2);

	button_t buttonB3 = { "II Komputery", MAIN_MENU_CENTER, MAIN_MENU_CENTER, letterSize, &color, -1, -1 };
	moveButtonCoords(&buttonB3, 0, 1.0 * letterSize);
	renderButton(app, &buttonB3);

	button_t buttonB4 = { "Cofnij", MAIN_MENU_CENTER, MAIN_MENU_CENTER, letterSize, &color, -1, -1 };
	moveButtonCoords(&buttonB4, 0, 3.0 * letterSize);
	renderButton(app, &buttonB4);

	point_t point = { 0, 0 };
	while (processEvents(app, &point) != QUIT) { 
		if (pointOnButton(&point, &buttonB1)) // I Gracz
			return I_PLAYER;
		else if (pointOnButton(&point, &buttonB2)) // II Gracze
			return II_PLAYERS;
		else if (pointOnButton(&point, &buttonB3)) // II Komputery
			return II_COMPUTERS;
		else if (pointOnButton(&point, &buttonB4)) // BACK
			return BACK;
	}
	return QUIT;
}

void moveButtonCoords(button_t* button, int x, int y) {
	button->x += x;
	button->y += y;

	if (strlen(button->text)) { // If the length of the text is bigger than zero
		button->halfHeight = button->letterSize / 2;
		button->halfWidth = (button->letterSize * strlen(button->text)) / 2; // 
	}
}

void renderButton(struct WindowApp* app, button_t *button) {
	
		SDL_Color white = { 255,255,255,255 }; // Default color (WHITE)
	if (!button->color) button->color = &white;
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("font.ttf", button->letterSize * 0.9);
	if (!font) return;
	TTF_SetFontStyle(font, TTF_STYLE_BOLD);

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, button->text, *button->color);
	SDL_Texture* messageTxt = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);

	SDL_QueryTexture(messageTxt, NULL, NULL, &button->halfWidth, &button->halfHeight); // Get width and height of the text
	button->halfWidth = button->halfWidth / 2;
	button->halfHeight = button->halfHeight / 2;

	SDL_Rect messageRect = { // Create rect with text size
		button->x - button->halfWidth, // x
		button->y - button->halfHeight, // y
		button->halfWidth * 2, // w
		button->halfHeight * 2 }; // h

	SDL_RenderCopy(app->renderer, messageTxt, NULL, &messageRect);

	// Free memory
	TTF_CloseFont(font);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(messageTxt);
	SDL_RenderPresent(app->renderer);
	TTF_Quit();
}

void renderText(struct WindowApp* app, char* text, int letterSize, SDL_Color* color, int x, int y) {
	SDL_Color white = { 255,255,255,255 }; // Default color (WHITE)
	if (!color) color = &white;
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("font.ttf", letterSize * 0.9);
	if (!font) return;
	TTF_SetFontStyle(font, TTF_STYLE_BOLD);

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, *color);
	SDL_Texture* messageTxt = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);

	int width = 0, height = 0;
	SDL_QueryTexture(messageTxt, NULL, NULL, &width, &height); // Get width and height of the text

	SDL_Rect messageRect = { // Create rect with text size
		x - width / 2, // x
		y - height / 2, // y
		width, // w
		height }; // h

	SDL_RenderCopy(app->renderer, messageTxt, NULL, &messageRect);

	// Free memory
	TTF_CloseFont(font);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(messageTxt);
	SDL_RenderPresent(app->renderer);
	TTF_Quit();
}

int pointOnButton(point_t* point, button_t* button) { // Check if point is on button
	if (!button) return FALSE;
	if (button->halfHeight <= 0) return FALSE;

	if (point->x >= (button->x - button->halfWidth) && point->x <= (button->x + button->halfWidth) &&
		point->y >= (button->y - button->halfHeight) && point->y <= (button->y + button->halfHeight)) {

		return TRUE;
	}
	return FALSE;
}

void rotateBoard(struct WindowApp* app, struct Board *board[8][8], int rotated) { // Render board
	renderNotations(app, rotated); // Render board notations, 1 2 3..., A B C...
	// Render the board properly rotated
	int field = WHITE;
	for (int i = 0; i < 8; i++) {
		field = (i % 2) ? BLACK : WHITE; // field = WHITE or BLACK
		for (int j = 0; j < 8; j++, field = !field) {
			SDL_Rect boardRect = { FRAME + i*BOARD_SIZE / 8, FRAME + j*BOARD_SIZE / 8,
				BOARD_SIZE / 8, BOARD_SIZE / 8 };
			SDL_RenderCopy(app->renderer, app->textures[FIELD][field], NULL, &boardRect);
			if (rotated) {
				if (board[7 - i][7 - j]) { // Rotate board coords
					int chessman = board[7 - i][7 - j]->chessman;
					int color = board[7 - i][7 - j]->color;
					SDL_RenderCopy(app->renderer, app->textures[chessman][color], NULL, &boardRect);
				}
			}
			else {
				if (board[i][j]) {
					int chessman = board[i][j]->chessman;
					int color = board[i][j]->color;

					SDL_RenderCopy(app->renderer, app->textures[chessman][color], NULL, &boardRect);
				}
			}
		}
	}
	SDL_RenderPresent(app->renderer); // Render all board
}

int processEventsGame(struct WindowApp* app, point_t* point) {
	int eventType = processEvents(app, point);
	if (eventType == POINT) { // If mouse click is on board
		point->x -= FRAME;	  // Set point to board coords from window coords  ( 0-7 )
		point->y -= FRAME;
		point->x /= BOARD_SIZE / 8;
		point->y /= BOARD_SIZE / 8;
	}
	return eventType;
}

int processEvents(struct WindowApp* app, point_t* point) {
	int x = 0, y = 0, eventValue;
	SDL_Event event;
	while (1) { // Repeat when NO_EVENTS
		if ((eventValue = processOneEvent(app, point)) != NO_EVENTS)
			return eventValue;
	}
}

int processOneEvent(struct WindowApp* app, point_t* point) {
	point_t p = { 0,0 };
	if (!point) point = &p;
	SDL_Event event;
	int event_type = 0;
	static int buttonDown = FALSE;
	if (!SDL_PollEvent(&event)) return NO_EVENTS;

	if (event.type == SDL_QUIT) {
		eventType(QUIT);
		return QUIT;
	}
	else if (event.type == SDL_MOUSEMOTION ) // Mouse motion event is only used by side menu
		processEventMenu(app, &event); 
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		buttonDown = TRUE;
		if (event.button.clicks == SDL_BUTTON_LEFT) {
			point->x = event.button.x;
			point->y = event.button.y;
			if ((event_type = processEventMenu(app, &event)) == NOT_PROCESSED) // Try to use this event in game menu
				if (point->x >= FRAME && point->x < (BOARD_SIZE + FRAME) && // If not used is side menu 
					point->y >= FRAME && point->y < (BOARD_SIZE + FRAME)) { // and point in board then return POINT

					return POINT;
				}
			return event_type;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP) { // Mouse motion event is only used by side menu
		buttonDown = FALSE;
		processEventMenu(app, &event);
	}

	return NO_EVENTS;
}

void displaySelected(struct WindowApp* app, point_t* p, int rotated) { // Display selected square
	if (!rotated) { // Rotate coords
		SDL_Rect boardRect = { FRAME + p->x *BOARD_SIZE / 8, FRAME + p->y * BOARD_SIZE / 8,
			BOARD_SIZE / 8, BOARD_SIZE / 8 };
		SDL_RenderCopy(app->renderer, app->textures[MOVE][SELECTED], NULL, &boardRect);
	}
	else {
		SDL_Rect boardRect = { 
			FRAME + (7 - p->x) *BOARD_SIZE / 8, 
			FRAME + (7 - p->y) * BOARD_SIZE / 8,
			BOARD_SIZE / 8, BOARD_SIZE / 8 };
		SDL_RenderCopy(app->renderer, app->textures[MOVE][SELECTED], NULL, &boardRect);
	}
	SDL_RenderPresent(app->renderer);
}

void displayPossMoves(struct WindowApp* app, moves_t *head, int rotated) { 
	// Display possible moves
	while (head){ // For each move
		if (!rotated) {
			SDL_Rect boardRect = { 
				FRAME + head->point.x*BOARD_SIZE / 8,
				FRAME + head->point.y * BOARD_SIZE / 8,
				BOARD_SIZE / 8, BOARD_SIZE / 8 };
			SDL_RenderCopy(app->renderer, app->textures[MOVE][POSS_MOVE], NULL, &boardRect);
		}
		else {
			SDL_Rect boardRect = {
				FRAME + (7 - head->point.x) * BOARD_SIZE / 8,
				FRAME + (7 - head->point.y) * BOARD_SIZE / 8,
				BOARD_SIZE / 8, BOARD_SIZE / 8 };
			SDL_RenderCopy(app->renderer, app->textures[MOVE][POSS_MOVE], NULL, &boardRect);
		}
		head = head->next;
	}
	SDL_RenderPresent(app->renderer);
}

void displayMove(struct WindowApp* app, struct Move* move, int rotated) {
	int field = BLACK;
	if (!rotated) { // Rotate coords
		SDL_Rect boardRect = {
			FRAME + move->point1.x * BOARD_SIZE / 8,
			FRAME + move->point1.y * BOARD_SIZE / 8,
			BOARD_SIZE / 8, BOARD_SIZE / 8 };
		field = (move->point1.x * move->point1.y % 2) ? BLACK : WHITE;
		SDL_RenderCopy(app->renderer, app->textures[FIELD][field], NULL, &boardRect); // First square
		SDL_RenderPresent(app->renderer);

		boardRect.x = FRAME + move->point2.x * BOARD_SIZE / 8;
		boardRect.y = FRAME + move->point2.y * BOARD_SIZE / 8;
		field = (move->point2.x * move->point2.y % 2) ? BLACK : WHITE;
		SDL_RenderCopy(app->renderer, app->textures[FIELD][field], NULL, &boardRect); // Second square
		SDL_RenderPresent(app->renderer);
	}
	else {
		SDL_Rect boardRect = {
			FRAME + (7 - move->point1.x) * BOARD_SIZE / 8, // x
			FRAME + (7 - move->point1.y) * BOARD_SIZE / 8, // y
			BOARD_SIZE / 8, BOARD_SIZE / 8 };			   // w,  h
		field = (move->point1.x * move->point1.y % 2) ? BLACK : WHITE;
		SDL_RenderCopy(app->renderer, app->textures[FIELD][field], NULL, &boardRect); // First square
		SDL_RenderPresent(app->renderer);

		boardRect.x = FRAME + (7 - move->point2.x) * BOARD_SIZE / 8;
		boardRect.y = FRAME + (7 - move->point2.y) * BOARD_SIZE / 8;
		field = (move->point2.x * move->point2.y % 2) ? BLACK : WHITE;
		SDL_RenderCopy(app->renderer, app->textures[FIELD][field], NULL, &boardRect); // Second square
		SDL_RenderPresent(app->renderer);
	}
	SDL_RenderPresent(app->renderer);
}

void delWindowApp(struct WindowApp* app) { //Free memory
	for (int i = 0; i < TEXTURES_SIZE; i++)
		for (int j = 0; j < 2; j++) {
			if (app->textures[i][j]) // Free all saved textures
				SDL_DestroyTexture(app->textures[i][j]);
		}

	if (app->renderer) // Free renderer
		SDL_DestroyRenderer(app->renderer);

	if (app->window) // Free window
		SDL_DestroyWindow(app->window);

	free(app->scrollbar);  // Free scrollbar

	SDL_Quit();
}

int eventType(int x) { // To remember when back to menu and when quit the program
	static int quit = 1;
	if (x == QUIT || x == BACK || x == CLEAR)
		quit = x;

	return quit;
}