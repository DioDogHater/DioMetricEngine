#ifndef DME_ANIMATIONS_H
#define DME_ANIMATIONS_H

#include "dm_engine.h"

enum{
	DM_ANIM_REPEAT = 0x00,
	DM_ANIM_ONCE = 0x01,
	DM_ANIM_REBOUND = 0x02,
	DM_ANIM_REVERSE = 0x04,
	DM_ANIM_2X_SPEED = 0x08,
	DM_ANIM_STOP = 0x10
};
typedef unsigned char AnimFlags;

typedef struct{
	Ticks_t interval;
	Ticks_t last_frame;
	int start_frame, end_frame;
	int frame;
	AnimFlags flags;
} Animation;
#define NEW_ANIMATION(s,e,i,f) (Animation){(Ticks_t)(i),0,(int)(s),(int)(e),(int)(s),(AnimFlags)(f)}

typedef struct{
	Ticks_t interval;
	Ticks_t last_completion;
	float start, end;
	float current;
	AnimFlags flags;
} Animationf;
#define NEW_ANIMATIONF(s,e,i,f) (Animationf){(Ticks_t)(i),0,(float)(s),(float)(e),(float)(s),(AnimFlags)(f)}

void DM_reset_animation(Animation* anim){
	DM_ASSERTV(anim,"reset_animation: NULL arg");
	anim->last_frame = DM_get_ticks();
	anim->frame = ((anim->flags & DM_ANIM_REVERSE)?anim->end_frame:anim->start_frame);
}

void DM_update_animation(Animation* anim){
	DM_ASSERTV(anim,"update_animation: NULL arg");
	Ticks_t now = DM_get_ticks();
	if(anim->flags & DM_ANIM_STOP){
		anim->last_frame = now;
		return;
	}	
	if(now - anim->last_frame >= anim->interval / ((anim->flags & DM_ANIM_2X_SPEED)?2:1)){
		anim->frame += ((anim->flags & DM_ANIM_REVERSE)?-1:1);
		
		if(anim->flags & DM_ANIM_REVERSE){
			if(anim->frame < anim->start_frame){
				if(anim->flags & DM_ANIM_REBOUND){
					anim->flags ^= DM_ANIM_REVERSE;
					anim->frame = anim->start_frame;
				}else if(anim->flags & DM_ANIM_ONCE){
					anim->flags |= DM_ANIM_STOP;
					anim->frame = anim->start_frame;
				}else
					anim->frame = anim->end_frame;
			}
		}else{
			if(anim->frame > anim->end_frame){
				if(anim->flags & DM_ANIM_REBOUND){
					anim->flags ^= DM_ANIM_REVERSE;
					anim->frame = anim->end_frame;
				}else if(anim->flags & DM_ANIM_ONCE){
					anim->flags |= DM_ANIM_STOP;
					anim->frame = anim->end_frame;
				}else
					anim->frame = anim->start_frame;
			}
		}
		anim->last_frame = now;
	}
}

void DM_reset_animationf(Animationf* anim){
	DM_ASSERTV(anim,"reset_animationf: NULL arg");
	anim->last_completion = DM_get_ticks();
	anim->current = ((anim->flags & DM_ANIM_REVERSE)?anim->end:anim->start);
}

void DM_update_animationf(Animationf* anim){
	DM_ASSERTV(anim,"update_animationf: NULL arg");
	Ticks_t now = DM_get_ticks();
	if(anim->flags & DM_ANIM_STOP){
		anim->last_completion = now;
		return;
	}
	if(now - anim->last_completion > anim->interval / ((anim->flags & DM_ANIM_2X_SPEED)?2:1)){
		if(anim->flags & DM_ANIM_REBOUND)
			anim->flags ^= DM_ANIM_REVERSE;
		else if(anim->flags & DM_ANIM_ONCE)
			anim->flags |= DM_ANIM_STOP;
		else
			anim->current = ((anim->flags & DM_ANIM_REVERSE)?anim->end:anim->start);
		anim->last_completion = now;
	}
	if(anim->flags & DM_ANIM_REVERSE)
		anim->current = LERP(
			anim->start,
			anim->end,
			1.f - (float)(now - anim->last_completion)/(float)anim->interval * ((anim->flags & DM_ANIM_2X_SPEED)?2.f:1.f)
		);
	else
		anim->current = LERP(
			anim->start,
			anim->end,
			(float)(now - anim->last_completion)/(float)anim->interval * ((anim->flags & DM_ANIM_2X_SPEED)?2.f:1.f)
		);
}

#endif
