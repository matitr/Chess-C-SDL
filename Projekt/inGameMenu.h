#ifndef _INGAMEMENU_H
#define _INGAMEMENU_H
#include "window.h"
#include <vld.h>

// Render
void drawSideMenu(struct WindowApp*);
void renderNotations(struct WindowApp*, int);
void drawLines(struct WindowApp* app, int x, int y, int size, SDL_Color* color);
void renderHistory(struct WindowApp* app, int move);
void clearHistory(struct WindowApp* app);
void renderTextHistory(struct WindowApp* app, char* text, int letterSize, SDL_Color* color, int x, int y);
void renderScrollbar(struct WindowApp*);
void renderInfo(struct WindowApp* app, int value, int turn);

// Menu
int processEventMenu(struct WindowApp* app, SDL_Event *event);
void changeScrollbar(struct WindowApp* app, int*, int);
int processWinner(struct WindowApp* app, int winner);

#endif // !_INGAMEMENU_H
