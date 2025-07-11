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

// Compare two chunks for depth sorting
int Chunk_compare_depth(const void* p, const void* q){
	Chunk *a = (Chunk*)p, *b = (Chunk*)q;
	float ad = a->pos.x + a->pos.z, bd = b->pos.x + b->pos.z;
	return (ad > bd) - (ad < bd);
}

DM_FUNC void Map_alloc(Map* m, uint count){
	if(!m) return;
	m->size += count;
	m->arr = (Chunk*) realloc(m->arr, sizeof(Chunk) * m->size);
	DM_ASSERTV(m->arr,"Map_alloc: failed to alloc %lu bytes",sizeof(Chunk)*m->size);
	memset(&m->arr[m->size-count],0,sizeof(Chunk) * count);
}

DM_FUNC void Map_sort(Map* m){
	if(!m) return;
	qsort(m->arr, m->size, sizeof(Chunk), Chunk_compare_depth);	
}

DM_FUNC void Map_free(Map* m){
	if(!m) return;
	if(m->arr) free(m->arr);
	m->size = 0;
}

DM_FUNC Chunk* Map_find_chunk(Map* m, float x, float z){
	if(!m || !m->size) return NULL;
	for(uint i = 0; i < m->size; i++){
		if(x >= m->arr[i].pos.x && x <= m->arr[i].pos.x+CHUNK_COLS && z >= m->arr[i].pos.z && z <= m->arr[i].pos.z+CHUNK_ROWS)
			return &m->arr[i];
	}
	return NULL;
}

// Render a chunk
DM_FUNC void Chunk_render(Chunk* c, Tileset* tiles, Transform* camera, Projection* proj){
	if(!c || !tiles || !camera || !proj)
		return;
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

// Get height of block in chunk
DM_FUNC short Chunk_sample_height(Chunk* c, float x, float z){
	if(!c) return 0;
	x -= c->pos.x;
	z -= c->pos.z;
	if(x > 0 && x < CHUNK_COLS && z > 0 && z < CHUNK_ROWS)
		return c->blocks[(int)z*CHUNK_ROWS + (int)x].height;
	else
		return 0;
}

// Render a map
DM_FUNC void Map_render(Map* m, Tileset* tiles, Transform* camera, Projection* proj){
	if(!m || !tiles || !camera || !proj)
		return;
	for(uint i = 0; i < m->size; i++){
		Chunk_render(&m->arr[i],tiles,camera,proj);
	}
}

#endif
