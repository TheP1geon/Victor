#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <math.h>

#include "include/Victor.h" 
#include "include/Victor_Color.h"
#include "include/Victor_Math.h"
#include "include/Victor_Shapes.h"
#include "include/Victor_Types.h"

// Windowing stuff
static SDL_Window*   window;
static SDL_Renderer* renderer;
static i32 WINDOW_WIDTH, WINDOW_HEIGHT;

// Game loop stuff
static bool quit;
static SDL_Event e;

static u32 FPS;
static u32 frameStart;
static u32 frameTime;
static u32 frameDelay;

// Colors and other misc stuff
static Color backgroundColor;

void Victor_Init(i32 windowWidth_, i32 windowHeight_, const char* windowTitle) {
    if (window != NULL || renderer != NULL) {
        printf("Window already initialized!\n");
        exit(4);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(windowTitle == NULL ? "Victor Example" : windowTitle, 
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                              windowWidth_, windowHeight_, 
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(2);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(3);
    }

    FPS = 60;

    frameDelay = 1000/FPS;

    backgroundColor = BLACK;

    WINDOW_HEIGHT = windowHeight_;
    WINDOW_WIDTH = windowWidth_;
}

void Victor_GameLoop(void(*display)(void)) {
    Victor_Event tempEvent = e;

    if (window == NULL || renderer == NULL) {
        printf("Victor not initialized. You must initialize Victor before running the game loop\n");
        exit(5);
    }

    while (!quit) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                    break;
                }
            }

            // Store the last value of e
            tempEvent = e;
        }

        // Set the global event varible to the last value of e
        e = tempEvent;

        SDL_SetRenderDrawColor(renderer, ColorParam(backgroundColor));
        SDL_RenderClear(renderer);

        // Call the display function
        display();

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

void Victor_Quit(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Victor_IsPosInWindow(Vector2 pos) {
    return (pos.x >= 0 && pos.x < WINDOW_WIDTH && pos.y >= 0 && pos.y < WINDOW_HEIGHT);
}

void Victor_ClampPointToWindow(Vector2* v) {
    Victor_Clampf(&v->x, 0, WINDOW_WIDTH);
    Victor_Clampf(&v->y, 0, WINDOW_HEIGHT);
}

void Victor_ClampXYToWindow(i32* x, i32* y) {
    Victor_Clamp(x, 0, WINDOW_WIDTH);
    Victor_Clamp(y, 0, WINDOW_HEIGHT);
}

// Getters and setters
Victor_Event Victor_GetEvent(void) { return e;}
Vector2 Victor_GetMousePos(void) { return VECTOR2(e.motion.x, e.motion.y);}
Vector2 Victor_GetWindowDimensions(void) { return VECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT);}

void Victor_SetBackgroundColor(Color c) { backgroundColor = c;}
void Victor_SetFPS(i32 fps) { if (fps > 0 ) { FPS = fps; } else {FPS = 1;} frameDelay = 1000/FPS; }

/*
    * The Shapes module, responsible for drawing primitive shapes
    * to the screen
    *
    * Better ways to draw something on the screen, in the form of a texture, sprite, or image,
    * are coming later, for now you can draw basics stuff
*/

// Basic drawing functions

void Victor_PlacePixel(i32 x, i32 y, Color c) {
    SDL_SetRenderDrawColor(renderer, ColorParam(c));
    Victor_ClampXYToWindow(&x, &y);
    SDL_RenderDrawPoint(renderer, x, y);
}

void Victor_PlacePixelVec(Vector2 pos, Color c) {
    Victor_PlacePixel(pos.x, pos.y, c);
}

void Victor_DrawLine(i32 x1, i32 y1, i32 x2, i32 y2, Color c) {
    Victor_ClampXYToWindow(&x1, &y1);
    Victor_ClampXYToWindow(&x2, &y2);

    SDL_SetRenderDrawColor(renderer, ColorParam(c));
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Victor_DrawLineVec(Vector2 pos1, Vector2 pos2, Color c) {
    Victor_DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, c);
}

// Reactangle related functions

void Victor_RectangleScale(Victor_Rectangle* rec, f32 scale) {
    rec->height *= scale;
    rec->width *= scale;
}

void Victor_DrawRectangle(i32 x, i32 y, i32 width, i32 height, Color color) {
    SDL_SetRenderDrawColor(renderer, ColorParam(color));

    for (i32 _y = y; _y < y + height; ++_y) {
        for (i32 _x = x; _x < x + width; ++_x) {
            SDL_RenderDrawPoint(renderer, _x, _y);
        }
    }
}

void Victor_DrawRectangleVec(Vector2 pos, Vector2 dim, Color color) {
    Victor_DrawRectangle(pos.x, pos.y, dim.x, dim.y, color);
}

void Victor_DrawRectangleRec(Victor_Rectangle rec) {
    Victor_DrawRectangle(rec.pos.x, rec.pos.y, rec.width, rec.height, rec.color);
}

void Victor_DrawRectangleOutline(i32 x, i32 y, i32 width, i32 height, Color color) {
    Victor_DrawLine(x, y, x+width, y, color);
    Victor_DrawLine(x+width, y, x+width, y+height, color);
    Victor_DrawLine(x+width, y+height, x, y+height, color);
    Victor_DrawLine(x, y+height, x, y, color);
}
void Victor_DrawRectangleOutlineVec(Vector2 pos, Vector2 dim, Color color) {
    Victor_DrawRectangleOutline(pos.x, pos.y, dim.x, dim.y, color);
}
void Victor_DrawRectangleOutlineRec(Victor_Rectangle rec) {
    Victor_DrawRectangleOutline(rec.pos.x, rec.pos.y, rec.width, rec.height, rec.color);
}

// Yay! Circles

void Victor_DrawCircle(i32 x, i32 y, f32 radius, Color c) {
    Victor_Clampf(&radius, 0, sqrt((WINDOW_WIDTH/2.0)*(WINDOW_WIDTH/2.0) + (WINDOW_HEIGHT/2.0)*(WINDOW_HEIGHT/2.0)));

    for (i32 dy =  -radius; dy <= radius; ++dy) {
        i32 dx = sqrt((radius * radius) - (dy * dy));
        Victor_DrawLine(x - dx, y + dy, x + dx, y + dy, c);
    }
}

void Victor_DrawCircleVec(Vector2 pos, f32 radius, Color c) {
    Victor_DrawCircle(pos.x, pos.y, radius, c);
}

void Victor_DrawCircleCircle(Victor_Circle c) {
    Victor_DrawCircle(c.centre.x, c.centre.y, c.radius, c.color);
}

void Victor_DrawCircleOutline(i32 x, i32 y, f32 radius, Color c) {
    Victor_Clampf(&radius, 0, sqrt((WINDOW_WIDTH/2.0)*(WINDOW_WIDTH/2.0) + (WINDOW_HEIGHT/2.0)*(WINDOW_HEIGHT/2.0)));

    for (i32 dy =  -radius; dy <= radius; ++dy) {
        i32 dx = sqrt((radius * radius) - (dy * dy));
        Victor_PlacePixel(x - dx, y + dy, c);
        Victor_PlacePixel(x + dx, y + dy, c);
    }

    for (i32 dx =  -radius; dx <= radius; ++dx) {
        i32 dy = sqrt((radius * radius) - (dx * dx));
        Victor_PlacePixel(x + dx, y - dy, c);
        Victor_PlacePixel(x + dx, y + dy, c);
    }
}

void Victor_DrawCircleOutlineVec(Vector2 pos, f32 radius, Color c) {
    Victor_DrawCircleOutline(pos.x, pos.y, radius, c);
}

void Victor_DrawCircleOutlineCircle(Victor_Circle c) {
    Victor_DrawCircleOutline(c.centre.x, c.centre.y, c.radius, c.color);
}
// Triangles, FINALLY!

static void sort_points_by_y(i32* x1, i32* y1, i32* x2, i32* y2, i32* x3, i32* y3) {
    if (*y1 > *y2) {
        i32 t = *y1;
        *y1 = *y2;
        *y2 = t;
        t = *x1;
        *x1 = *x2;
        *x2 = t;
    } 
    if (*y2 > *y3) {
        i32 t = *y2;
        *y2 = *y3;
        *y3 = t;
        t = *x2;
        *x2 = *x3;
        *x3 = t;
    } 
    if (*y1 > *y2) {
        i32 t = *y1;
        *y1 = *y2;
        *y2 = t;
        t = *x1;
        *x1 = *x2;
        *x2 = t;
    } 
}

void Victor_DrawTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) {
    // Definitely stole this algorithm from Tsoding :)
    sort_points_by_y(&x1, &y1, &x2, &y2, &x3, &y3);
    Victor_ClampXYToWindow(&x1, &y1);
    Victor_ClampXYToWindow(&x2, &y2);
    Victor_ClampXYToWindow(&x3, &y3);

    i32 dx12 = x2 - x1;
    i32 dy12 = y2 - y1;
    i32 dx13 = x3 - x1;
    i32 dy13 = y3 - y1;

    for (i32 y = y1; y <= y2; ++y) {

        i32 s1 = dy12 != 0 ? (y - y1) * dx12/dy12 + x1 : x1;
        i32 s2 = dy13 != 0 ? (y - y1) * dx13/dy13 + x1: x1;
        if (s1 > s2) { i32 t = s1; s1 = s2; s2 = t; }

        for (i32 x = s1; x <= s2; ++x) {
            Victor_PlacePixel(x, y, c);
        }
    }

    i32 dx32 = x2 - x3;
    i32 dy32 = y2 - y3;
    i32 dx31 = x1 - x3;
    i32 dy31 = y1 - y3;

    for (i32 y = y2; y <= y3; ++y) {
        i32 s1 = dy32 != 0 ? (y - y3) * dx32/dy32 + x3 : x3;
        i32 s2 = dy31 != 0 ? (y - y3) * dx31/dy31 + x3: x3;
        if (s1 > s2) { i32 t = s1; s1 = s2; s2 = t; }

        for (i32 x = s1; x <= s2; ++x) {
            Victor_PlacePixel(x, y, c);
        }
    }
}

void Victor_DrawTriangleVec(Vector2 v1, Vector2 v2, Vector2 v3, Color c) {
    Victor_DrawTriangle(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, c);
}
void Victor_DrawTriangleTri(Victor_Triangle tri) {
    Victor_DrawTriangle(tri.v1.x, tri.v1.y, tri.v2.x, tri.v2.y, tri.v3.x, tri.v3.y, tri.c);
}

void Victor_DrawTriangleOutline(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) {
    Victor_DrawLine(x1, y1, x2, y2, c);
    Victor_DrawLine(x2, y2, x3, y3, c);
    Victor_DrawLine(x3, y3, x1, y1, c);
}

void Victor_DrawTriangleOutlineVec(Vector2 v1, Vector2 v2, Vector2 v3, Color c) {
    Victor_DrawLineVec(v1, v2, c);
    Victor_DrawLineVec(v2, v3, c);
    Victor_DrawLineVec(v3, v1, c);
}
void Victor_DrawTriangleOutlineTri(Victor_Triangle tri) {
    Victor_DrawLineVec(tri.v1, tri.v2, tri.c);
    Victor_DrawLineVec(tri.v2, tri.v3, tri.c);
    Victor_DrawLineVec(tri.v3, tri.v1, tri.c);
}

/* Math Module
*  
*  A handful of math related functions that could be used in conjunction
*  with the rest of the library
*/

void Victor_Clamp(i32* num, i32 low, i32 high) {
    if (*num < low) {*num = low;}
    if (*num >= high) {*num = high;}
}

void Victor_Clampf(f32* num, f32 low, f32 high) {
    if (*num < low) {*num = low;}
    if (*num >= high) {*num = high;}
}
