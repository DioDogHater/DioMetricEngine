#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DME/dm_engine.h"

#include "map.h"

#define MAX_FPS 120

Font terminal;

Tileset map_tiles = NEW_TILESET(0,0,32,32);

Transform camera = TRANSFORM_IDENTITY;
Projection isometric = PROJECTION(1.f,0.5f,GRID_SIZE);

int main(int argc, char* argv[]){
		if(argc != 2){
				printf("USAGE: mapbuilder <map file>\nEXAMPLE: mapbuilder test.map\n");
				return 1;
		}
		if(!DM_init("DME - Map Builder",500,500,0))
				return 1;


		
		DM_quit();
		return 0;
}
