#ifndef DME_MATH_H
#define DME_MATH_H

#include <math.h>
#include "dm_engine.h"

// Math macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MIN3(a,b,c) MIN(a,MIN(b,c))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MAX3(a,b,c) MAX(a,MAX(b,c))
#define CLAMP(v,_min,_max) MAX(MIN((v),(_max)),(_min))
#define ABS(x) ((x)<0?-(x):(x))
#define SIGN(x) ((x)>0?1:((x)<0?-1:0))
#define SQR(x) ((x)*(x))

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

// Vec3 binary operation
#define VEC3_BIN_OP(name,op) \
DM_FUNC Vec3 Vec3_##name(Vec3 a, Vec3 b){\
	return (Vec3){a.x op b.x, a.y op b.y, a.z op b.z};\
}
// Vec3 unary operation
#define VEC3_UNA_OP(name,x,y,z,...) \
DM_FUNC Vec3 Vec3_##name(Vec3 v,##__VA_ARGS__){\
	return (Vec3){(x),(y),(z)};\
}

// Vec3 operations
VEC3_BIN_OP(add,+);
VEC3_BIN_OP(sub,-);
VEC3_BIN_OP(div,/);
VEC3_BIN_OP(mul,*);
VEC3_UNA_OP(scale,v.x*scalar,v.y*scalar,v.z*scalar,float scalar);
VEC3_UNA_OP(div_scale,v.x/scalar,v.y/scalar,v.z/scalar,float scalar);
//VEC3_UNA_OP(neg,-v.x,-v.y,-v.z);
//VEC3_UNA_OP(abs,ABS(v.x),ABS(v.y),ABS(v.z));
VEC3_UNA_OP(floor,floor(v.x),floor(v.y),floor(v.z));
DM_FUNC float Vec3_magnitude(Vec3 v){
	return sqrt(SQR(v.x)+SQR(v.y)+SQR(v.z));
}
DM_FUNC Vec3 Vec3_normalize(Vec3 v){
	float m = Vec3_magnitude(v);
	if(m != 0.f)
		return Vec3_div_scale(v,m);
	else
		return VEC3ZERO;
}

// Transform Vec3
DM_FUNC Vec3 Transform_apply(Vec3 v, Transform* t){
	if(!t){ DM_ERR("Transform_apply: NULL arg"); return VEC3ZERO; }
	Vec3 r = v;
	if(t->yaw != 0.f){
		float t_sin = sin(t->yaw), t_cos = cos(t->yaw);
		r.x = v.x * t_cos - v.z * t_sin;
		r.y = v.x * t_sin + v.z * t_cos;
	}
	r = Vec3_add(r,t->pos);
	return r;
}

// Project Vec3 with Projection (orthogonal)
// World to screen coordinates
DM_FUNC Vec2 Projection_apply(Vec3 v, Projection* p){
	if(!p){ DM_ERR("Projection_apply: NULL arg"); return (Vec2){0,0}; }
	uint tile_w2 = (p->tile_width >> 1);
	uint tile_h2 = (p->tile_height >> 1);
	return (Vec2){
		(int)floor((v.x * p->x * (float)tile_w2) + (v.z * -p->x * (float)tile_w2)) - tile_w2 + sw2,
		(int)floor((v.x * p->y * (float)tile_h2) + ((v.z + -v.y/2.f) * p->y * (float)tile_h2))  - tile_h2 + sh2
	};
}

// Screen to world coordinates
DM_FUNC Vec3 Projection_inverse(Vec2 v, float y, Projection* p){
	if(!p){ DM_ERR("Projection_inverse: NULL arg"); return VEC3ZERO; }
	uint tile_w2 = (p->tile_width >> 1);
	uint tile_h2 = (p->tile_height >> 1);
	v.x -= sw2;
	v.y -= sh2 - tile_h2 - (int)(-(float)y/2.f * p->y * (float)tile_h2);
	float a = p->x * (float)tile_w2;
	float b = -p->x * (float)tile_w2;
	float c = p->y * (float)tile_h2;
	float d = p->y * (float)tile_h2;
	float determinant = 1.f/(a*d-b*c);
	float ai = d*determinant;
	float bi = -b*determinant;
	float ci = -c*determinant;
	float di = a*determinant;
	return (Vec3){
		(float)v.x * ai + (float)v.y * bi,
		0.f,
		(float)v.x * ci + (float)v.y * di
	};
}

#endif
