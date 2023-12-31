#ifndef __VICTOR_H
#define __VICTOR_H

#include "Victor_Include.h"
#include <SDL2/SDL.h>

typedef SDL_Event Victor_Event;

// Game stuff
void Victor_Init(i32 windowWidth, i32 windowHeight, const char* windowTitle);
void Victor_GameLoop(void(*display)(void));
void Victor_Quit(i32 code);

bool Victor_IsPosInWindow(Vector2 pos);

void Victor_ClampXYToWindow(i32* x, i32* y);
void Victor_ClampPointToWindow(Vector2* v);

// Getting and setting global variables
Victor_Event Victor_GetEvent(void);
f32 Victor_GetDeltaTime(void);
Vector2 Victor_GetMousePos(void);
Vector2 Victor_GetWindowDimensions(void);

void Victor_SetFPS(i32 fps);
void Victor_SetBackgroundColor(Color c);
void Victor_SetCleanFunc(void(*clean)(void));

#endif  //__VICTOR_H
