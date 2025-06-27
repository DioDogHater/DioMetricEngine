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

#endif
