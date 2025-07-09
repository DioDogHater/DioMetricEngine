#ifndef DME_EVENTS_H
#define DME_EVENTS_H

// Handle events
// EVENT(KEYDOWN) instead of SDL_KEYDOWN
#define EVENT(x) SDL_##x
#define IS_EVENT(e,x) ((e).type == SDL_##x)
// KEY(a) instead of SDLK_a
#define KEY(x) SDLK_##x
DM_FUNC bool DM_poll_event(Event* event){
	return (bool)SDL_PollEvent(event);
}

// SCAN(A) instead of SDL_SCANCODE_A
#define SCAN(x) SDL_SCANCODE_##x
DM_FUNC bool DM_keydown(SDL_Scancode c){
	if(!keyboard_state) return false;
	return (bool)keyboard_state[c];
}
DM_FUNC bool DM_keyup(SDL_Scancode c){
	if(!keyboard_state) return false;
	return !(bool)keyboard_state[c];
}

// Handle input for a player
typedef struct {
	uchar k_up, k_down, k_right, k_left;	// Scancode, so please use SCAN(x)
	bool up, down, right, left;
	Vec3 direction;
} PlayerController;
#define NEW_PLAYER_CONTROLLER(u,d,r,l) (PlayerController){(u),(d),(r),(l),false,false,false,false,VEC3ZERO}

DM_FUNC void PlayerController_update(PlayerController* pc){
	DM_ASSERTV(pc,"PlayerController_update: NULL arg");
	pc->up = DM_keydown(pc->k_up);
	pc->down = DM_keydown(pc->k_down);
	pc->left = DM_keydown(pc->k_left);
	pc->right = DM_keydown(pc->k_right);
	pc->direction = Vec3_normalize(VEC3(pc->right - pc->left + pc->down - pc->up, 0.f, pc->down - pc->up + pc->left - pc->right));
}

DM_FUNC Vec2 DM_mouse_pos(bool scaled){
	Vec2 mp = {0, 0};
	SDL_GetMouseState(&mp.x,&mp.y);
	if(scaled){
		mp.x *= _scaling_factor;
		mp.y *= _scaling_factor;
	}
	return mp;
}

#endif
