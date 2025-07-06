#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DME/dm_engine.h"

#include "map.h"

#define MAX_FPS 120

Font terminal = NEW_FONT();

Tileset map_tiles = NEW_TILESET(0,0,32,32);

Transform camera = TRANSFORM_IDENTITY;
Projection isometric = PROJECTION(1.f,0.5f,GRID_SIZE,GRID_SIZE);

Map map = NEW_MAP();

Tileset dummy_idle = NEW_TILESET(0,0,64,64);
Tileset dummy_run = NEW_TILESET(0,0,64,64);

RL_Array resources = {
	RL_FONT(terminal,	20, "assets/terminal.ttf"),
	RL_TILESET(map_tiles,	"assets/sandbox32/tileset.png"),
	RL_TILESET(dummy_idle,	"assets/dummy/idle.png"),
	RL_TILESET(dummy_run,	"assets/dummy/run.png"),
	RL_NULL()
};

bool load_resources(){
	return DM_load_RL_Array(resources);	
}

void free_resources(){
	DM_free_RL_Array(resources);
}

int main(int argc, char *argv[]){
	// Create a 800x500 window
	if(!DM_init(NULL,800,500,0))
		return 1;
	
	if(!load_resources())
		return 1;
	
	// Player movement variables
	float pl_speed = 20.f;
	PlayerController pl_controller = NEW_PLAYER_CONTROLLER(
		SCAN(W), SCAN(S), SCAN(D), SCAN(A)
	);
	
	// Fill test_chunk with stuff
	Map_alloc(&map,1);
	printf("%d chunks allocated!\n",map.size);
	
	// Time related variables
	Uint32 last_frame = SDL_GetTicks();
	Text fps_counter = NEW_TEXT("- FPS",&terminal);

	float time = 0.f;

	// Game loop
	bool running = true;
	while(running){
		// Regulate FPS
		Uint32 now = SDL_GetTicks();
		if(now-last_frame < 1000/MAX_FPS){
			SDL_Delay(1000/MAX_FPS-(SDL_GetTicks()-last_frame));
			now = SDL_GetTicks();
		}

		// deltaTime calculation
		double deltaTime = (double)(now-last_frame)/1000.f;
		time += deltaTime;
		last_frame = now;

		// Update FPS counter
		sprintf(fps_counter.text,"%g FPS",1.f/deltaTime);
		DM_load_text(WHITE,0,&fps_counter);

		// Handle events
		Event e;
		while(DM_poll_event(&e)){
			if(IS_EVENT(e,QUIT))
				running = false;
		}
		
		// Move camera
		PlayerController_update(&pl_controller);
		camera.pos = Vec3_sub(camera.pos, Vec3_scale(pl_controller.direction, pl_speed * deltaTime));
		
		DM_clear_screen();
		
		// Render map
		Map_render(&map, &map_tiles, &camera, &isometric);

		// Animate map
		Block* block = map.arr[0].blocks;
		for(int y = 0; y < CHUNK_ROWS; y++){
			for(int x = 0; x < CHUNK_COLS; x++, block++){
				float dist = (float)(SQR(x-CHUNK_COLS/2) + SQR(y-CHUNK_ROWS/2))/(float)(CHUNK_SIZE);
				block->height = sin(time*4.f+dist*42.f)*4.f*(1.f-dist);
			}
		}
		
		// Display FPS counter
		DM_render_text(fps_counter,0,0,1.f);

		DM_update_screen();
	}

	Map_free(&map);

	free_resources();
	
	DM_quit();

	return 0;
}
