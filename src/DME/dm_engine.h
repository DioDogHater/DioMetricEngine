#ifndef DIOMETRIC_ENGINE_H
#define DIOMETRIC_ENGINE_H

// Standard libraries
#include <stdio.h>
#include <stdlib.h>

// SDL libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Get rid of the main() macro
#undef main

// Boolean type
typedef _Bool bool;
#define true 1
#define false 0

// Unsigned integer types
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

// SDL related types
typedef SDL_Color Color;
#define COLOR(r,g,b) (Color){(r),(g),(b),255}
#define COLORA(r,g,b,a) (Color){(r),(g),(b),(a)}
#define SCALE_COLOR(c,s) (Color){(c).r*(s),(c).g*(s),(c).b*(s),(c).a}
#define BLACK COLOR(0,0,0)
#define WHITE COLOR(255,255,255)
#define RED COLOR(128,0,0)
#define LIGHT_RED COLOR(255,0,0)
#define DARK_RED COLOR(86,0,0)
#define GREEN COLOR(0,128,0)
#define LIGHT_GREEN COLOR(0,255,0)
#define DARK_GREEN COLOR(0,86,0)
#define LIGHT_BLUE COLOR(0,0,255)
#define BLUE COLOR(0,0,192)
#define DARK_BLUE COLOR(0,0,86)
#define GRAY COLOR(128,128,128)
#define LIGHT_GRAY COLOR(192,192,192)
#define DARK_GRAY COLOR(64,64,64)
typedef SDL_Event Event;
typedef SDL_Point Vec2;
#define VEC2(x,y) (Vec2){(x),(y)}
#define VEC2_ZERO VEC2(0,0)
#define VEC2_CMP(a,b) ((a).x == (b).x && (a).y == (b).y)
typedef SDL_Rect Rect;
#define RECT(x,y,w,h) (Rect){(x),(y),(w),(h)}
#define RECT_ZERO RECT(0,0,0,0)
#define RECT_CMP(a,b) ((a).x == (b).x && (a).y == (b).y && (a).w == (b).w && (a).h == (b).h)
#define VEC2_IN_RECT(v,r) ((v).x >= (r).x && (v).x <= (r).x+(r).w && (v).y >= (r).y && (v).y <= (r).y+(r).h)
typedef Uint32 Ticks_t;

// Misc types
// --

// Global variables
SDL_Window* window = NULL;
uint sw = 0, sh = 0;
uint sw2 = 0, sh2 = 0;
float _scaling_factor = 1.f;
SDL_Renderer* renderer = NULL;
int keyboard_state_size = 0;
const bool* keyboard_state = NULL;

// Function type
#define DM_FUNC static inline

// Error printing
#define DM_ERR(fmt,...) printf("DME - " fmt "\n",##__VA_ARGS__)
#define DM_ASSERT(cond,fmt,...) if(!(cond)){ printf("DME - " fmt "\n",##__VA_ARGS__); return false; }
#define DM_ASSERTV(cond,fmt,...) if(!(cond)){ printf("DME - " fmt "\n",##__VA_ARGS__); return; }

// Debug printing
#define DM_DEBUG(stmt,fmt) printf(#stmt " = " fmt "\n",(stmt))

// Initialise the engine
DM_FUNC bool DM_init(char* caption, uint width, uint height, SDL_WindowFlags flags){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		DM_ERR("SDL_Init error: %s",SDL_GetError());
		return false;
	}
	if(IMG_Init(IMG_INIT_PNG) < 0){
		DM_ERR("IMG_Init error: %s",IMG_GetError());
		return false;
	}
	if(TTF_Init() < 0){
		DM_ERR("TTF_Init error: %s",TTF_GetError());
		return false;
	}
	window = SDL_CreateWindow(
			(caption)?caption:"DioMetric Engine",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, flags);
	if(!window){
		DM_ERR("SDL_CreateWindow error: %s",SDL_GetError());
		return false;
	}
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
	if(!renderer){
		DM_ERR("SDL_CreateRenderer error: %s",SDL_GetError());
		return false;
	}
	sw = width;
	sh = height;
	sw2 = sw / 2;
	sh2 = sh / 2;
	keyboard_state = (const bool*) SDL_GetKeyboardState(&keyboard_state_size);
	return true;
}

// Changes the scaling of the rendered window
// Without changing the logical resolution
DM_FUNC void DM_scale_window(uint width, uint height){
	DM_ASSERTV(window && renderer && sw && sh,"scale_window: Library not initialized")
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	SDL_RenderSetLogicalSize(renderer,sw,sh);
	SDL_SetWindowSize(window,width,height);
	SDL_SetWindowPosition(window,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
	_scaling_factor = (float)sw/(float)width;
}

// Quit the engine
DM_FUNC void DM_quit(){
	if(renderer)
		SDL_DestroyRenderer(renderer);
	renderer = NULL;
	if(window)
		SDL_DestroyWindow(window);
	window = NULL;
	keyboard_state = NULL;
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

DM_FUNC Ticks_t DM_get_ticks(){
	return SDL_GetTicks();
}

// Math
#include "dm_math.h"

// Resources
#include "dm_resources.h"

// Input
#include "dm_input.h"

// Rendering
#include "dm_render.h"

// Animations
#include "dm_animations.h"

// User Interface (UI / GUI)
#include "dm_ui.h"

#endif
