#ifndef DME_PHYSICS_H
#define DME_PHYSICS_H

#include "dm_engine.h"

typedef struct{
    Vec3* pos;
    Vec3 scale;
} BoxCollider;
#define NEW_BOX_COLLIDER(p,w,h,d) (BoxCollider){&(p),VEC3((w),(h),(d))}

// Checks if both colliders clip with eachother
// if there is collision, Vec3 returned != VEC3_ZERO
// will return translation to apply to a to unclip a and b
DM_FUNC Vec3 BoxCollider_check_collision(BoxCollider* a, BoxCollider* b){
    Vec3 diff = Vec3_sub(*a->pos,*b->pos);
    Vec3 distance = Vec3_abs(diff);
    Vec3 v = VEC3_ZERO;
    if(distance.x <= (a->scale.x + b->scale.x)/2.f)
        v.x = -SIGN(diff.x) * ((a->scale.x + b->scale.x)/2.f - distance.x);
    if(distance.y <= (a->scale.y + b->scale.y)/2.f)
        v.y = -SIGN(diff.y) * ((a->scale.y + b->scale.y)/2.f - distance.y);
    if(distance.z <= (a->scale.z + b->scale.z)/2.f)
        v.z = -SIGN(diff.z) * ((a->scale.z + b->scale.z)/2.f - distance.z);
    return v;
}

#endif