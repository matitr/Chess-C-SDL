#ifndef WINDOW_H
#define WINDOW_H
#include <SDL.h>
#include <SDL_image.h>
#include "vld.h"
#include "defines.h"
#include "chess.h"
#include "inGameMenu.h"

typedef struct Button {
	char *text;
	int x, y, letterSize;
	SDL_Color *color;
	int halfWidth, halfHeight;
}button_t;

struct WindowApp {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture *textures[TEXTURES_SIZE][2];
	SDL_Rect *scrollbar;
	button_t *menuButton;
};


void createWindow(struct WindowApp*);
int loadImages(struct WindowApp*);
int menu(struct WindowApp*, int saveAvailable);
void initScrollbar(struct WindowApp* app);
int newGameMenu(struct WindowApp* app);
void moveButtonCoords(button_t*, int x, int y);
void renderButton(struct WindowApp*, button_t*);
void renderText(struct WindowApp* app, char* text, int letterSize, SDL_Color* color, int x, int y);
int pointOnButton(point_t*, button_t*);
void rotateBoard(struct WindowApp*, struct Board *[8][8], int rotated);

int processEventsGame(struct WindowApp*, point_t*);
int processEvents(struct WindowApp*, point_t*);
int processOneEvent(struct WindowApp* app, point_t* point);
void displaySelected(struct WindowApp*, point_t*, int);
void displayPossMoves(struct WindowApp*, moves_t*, int);
void displayMove(struct WindowApp*, struct Move*, int);
void delWindowApp(struct WindowApp*);

//static
int eventType(int);
#endif