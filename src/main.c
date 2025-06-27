#include <stdio.h>
#include <string.h>
#include "DME/dm_engine.h"

Font terminal;
Text test_text = NEW_TEXT("The quick brown fox jumps over the lazy dog.",&terminal);

Tileset map_tiles = NEW_TILESET(0,0,32,32);

Projection isometric = PROJECTION(1.f,0.5f,32,32);

bool load_resources(){
	if(!DM_load_font("assets/terminal.ttf",20,&terminal))
		return false;
	if(!DM_load_text(WHITE,0,&test_text))
		return false;

	if(!DM_load_tileset("assets/sandbox32/tileset.png",&map_tiles))
		return false;

	return true;
}

int main(int argc, char *argv[]){
	if(!DM_init(NULL,800,500,0))
		return 1;
	
	if(!load_resources())
		return 1;

	// Game loop
	bool running = true;
	while(running){

		// Handle events
		Event e;
		while(DM_poll_event(&e)){
			if(IS_EVENT(e,QUIT))
				running = false;
		}

		DM_clear_screen();
		
		

		DM_update_screen();
	}
	
	DM_quit();

	return 0;
}
