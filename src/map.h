#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <string.h>
#include "DME/dm_engine.h"

// Constants
#define GRID_SIZE 32
#define CHUNK_SIZE 1024
#define CHUNK_ROWS 32
#define CHUNK_COLS 32

// Blocks
typedef struct {
		ushort tile;
		short height;
} Block;

// Chunks
typedef struct {
		Vec3 pos;
		Block blocks[CHUNK_SIZE];
} Chunk;

// Map (chunk array)
typedef struct {
		Chunk* arr;
		uint size;
} Map;
#define NEW_MAP() (Map){NULL,0}

DM_FUNC void Map_alloc(Map* m, uint count){
		m->size += count;
		m->arr = (Chunk*) realloc(m->arr, sizeof(Chunk) * m->size);
		if(!m->arr)
			DM_ERR("Map_alloc: failed to alloc %lu bytes",sizeof(Chunk)*m->size);
		else{
			memset(&m->arr[m->size-1],0,sizeof(Chunk));
		}
}

DM_FUNC void Map_free(Map* m){
		free(m->arr);
		m->size = 0;
}

// Render a chunk
DM_FUNC void Chunk_render(Chunk* c, Tileset* tiles, Transform* camera, Projection* proj){
	Block* block = c->blocks;
	for(uint row = 0; row < CHUNK_ROWS; row++){
		for(uint col = 0; col < CHUNK_COLS; col++, block++){
			Vec2 pos = Projection_apply(
				Transform_apply(Vec3_add(VEC3(col,block->height,row), c->pos), camera),
				proj
			);
			DM_RENDER_TILE_ISCALED(
				tiles,
				block->tile,
				pos.x, pos.y,
				GRID_SIZE, GRID_SIZE
			);
		}
	}
}

// Render a map
DM_FUNC void Map_render(Map* m, Tileset* tiles, Transform* camera, Projection* proj){
		for(uint i = 0; i < m->size; i++)
			Chunk_render(&m->arr[i],tiles,camera,proj);
}

#endif
