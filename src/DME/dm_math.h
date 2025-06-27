#ifndef DME_MATH_H
#define DME_MATH_H

#include <math.h>
#include "dm_engine.h"

// Math related types
typedef struct{
	float x, y, z;
} Vec3;
#define VEC3(x,y,z) (Vec3){(x),(y),(z)}
#define VEC3ZERO VEC3(0,0,0)

typedef struct{
	Vec3 pos;
	float yaw;
} Transform;
#define TRANSFORM(x,y,z,a) (Transform){VEC3((x),(y),(z)),(a)}
#define TRANSFORM_IDENTITY (Transform){VEC3ZERO,0.f}

typedef struct{
	float x, y;
	uint tile_width, tile_height;
} Projection;
#define PROJECTION(x,y,w,h) (Projection){(x),(y),(w),(h)}

DM_FUNC Vec2 DM_project(Vec3 v, Projection* p){
	if(!p){ DM_ERR("project: NULL arg"); return (Vec2){0,0}; }
	return (Vec2){
		(int)((v.x * p->x * (float)(p->tile_width >> 1)) + (-v.y * p->x * (float)(p->tile_width >> 1))) - (p->tile_width >> 1) + sw2,
		(int)((v.x * p->y * (float)(p->tile_height >> 1)) + ((v.z + v.y) * p->y * (float)(p->tile_height >> 1))) - (p->tile_height >> 1) + sh2
	};
}

#endif
