#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DME/dm_engine.h"

#include "map.h"

#define MAX_FPS 120

Font ps2p = NEW_FONT();

Tileset map_tiles = NEW_TILESET(0,0,32,32);

Transform camera = TRANSFORM_IDENTITY;
Projection isometric = PROJECTION(1.f,0.5f,GRID_SIZE,GRID_SIZE);

Map map = NEW_MAP();

Tileset dummy_idle = NEW_TILESET(0,0,64,64);
Tileset dummy_run = NEW_TILESET(0,0,64,64);

RL_Array resources = {
    RL_FONT(ps2p,	8, "assets/PressStart2P/PressStart2P.ttf"),
    RL_TILESET(map_tiles,	"assets/sandbox32/tileset.png"),
    RL_TILESET(dummy_idle,	"assets/dummy/idle.png"),
    RL_TILESET(dummy_run,	"assets/dummy/run.png"),
    RL_NULL()
};

bool load_resources(){
    return DM_load_resource_array(resources);	
}

void free_resources(){
    DM_free_resource_array(resources);
}

void wave_function(Chunk* c, float time){
    Block* block = c->blocks;
    for(int y = 0; y < CHUNK_ROWS; y++){
	    for(int x = 0; x < CHUNK_COLS; x++, block++){
	        float dist = sqrtf((float)(SQR(x+c->pos.x-CHUNK_COLS/2) + SQR(y+c->pos.z-CHUNK_ROWS/2)))/(float)(CHUNK_COLS);
	        block->height = (short)round(sin(-time*2.f+dist*32.f)*2.f);
	    }
    }
}

static void on_button_pressed(UI_Element* ui){
    ui->button.pressed = false;
    ui->button.can_be_pressed = false;
}

int main(int argc, char *argv[]){
    // Create a 640x360 window
    if(!DM_init(NULL,640,360,0))
        return 1;
	
    // Scale the window to 1280x720
    // Without changing internal resolution
    DM_scale_window(1280,720);
	
    if(!load_resources())
        return 1;
	
    // Player movement variables
    float pl_speed = 20.f;
    PlayerController pl_controller = NEW_PLAYER_CONTROLLER(SCAN(W), SCAN(S), SCAN(D), SCAN(A));

    // Player run animation
    Animation player_run = NEW_ANIMATION(0,4,100,DM_ANIM_REPEAT);
    DM_reset_animation(&player_run);
    Animation player_rotate = NEW_ANIMATION(0,7,1000,DM_ANIM_REPEAT);
    DM_reset_animation(&player_rotate);
    
    // Create chunks
    Map_alloc(&map,9);
    map.arr[1].pos = VEC3(CHUNK_COLS,0.f,CHUNK_ROWS);
    map.arr[2].pos = VEC3(-CHUNK_COLS,0.f,CHUNK_ROWS);
    map.arr[3].pos = VEC3(CHUNK_COLS,0.f,-CHUNK_ROWS);
    map.arr[4].pos = VEC3(-CHUNK_COLS,0.f,-CHUNK_ROWS);
    map.arr[5].pos = VEC3(CHUNK_COLS,0.f,0.f);
    map.arr[6].pos = VEC3(-CHUNK_COLS,0.f,0.f);
    map.arr[7].pos = VEC3(0.f,0.f,CHUNK_ROWS);
    map.arr[8].pos = VEC3(0.f,0.f,-CHUNK_ROWS);
    Map_sort(&map);
    
    // Time related variables
    Uint32 last_frame = SDL_GetTicks();
    Text fps_counter = NEW_TEXT("- FPS",&ps2p);
    float time = 0.f;
    
    UI_Bounds ui_screen = NEW_UI_BOUNDS(0,0,sw,sh);

    UI_Window ui_window = NEW_UI_WINDOW(100,100,200,100,GRAY,DARK_BLUE,2);
    UI_add_child(UI_ELEM(ui_screen),UI_ELEM(ui_window));

    UI_Frame ui_frame = NEW_UI_FRAME(1,1,50,50,LIGHT_GRAY,BLACK,1);
    UI_add_child(UI_ELEM(ui_window),UI_ELEM(ui_frame));
	
    UI_Image ui_image = NEW_UI_IMAGE(0,0,50,50,DM_get_tile(0,0,&dummy_idle));
    UI_LOAD_IMAGE(ui_image,"assets/dummy/dialogue.png");
    UI_add_child(UI_ELEM(ui_frame),UI_ELEM(ui_image));

    UI_Text ui_text = NEW_UI_TEXT(52,0,149,0,WHITE,"Hello world! I love hawt dawgs and cheezbargurs wit fryes!",&ps2p);
    UI_LOAD_TEXT(ui_text);
    UI_add_child(UI_ELEM(ui_window),UI_ELEM(ui_text));

    UI_TextButton ui_text_button = NEW_UI_TEXT_BUTTON(1,53,50,10,GREEN,BLACK,1,WHITE,"PLAY",&ps2p);
    UI_LOAD_TEXT(ui_text_button);
    ui_text_button.on_press = on_button_pressed;
    UI_add_child(UI_ELEM(ui_window),UI_ELEM(ui_text_button));

    UI_Slider ui_slider = NEW_UI_SLIDER(0,ui_window.rect.h-25,ui_window.rect.w,25,DARK_BLUE,RED,0.f,5.f,2.5f,0.1f);
    UI_add_child(UI_ELEM(ui_window),UI_ELEM(ui_slider));

    UI_Text ui_slider_text = NEW_UI_TEXT(0,-10,200,0,WHITE,"SLIDER VALUE:",&ps2p);
    UI_add_child(UI_ELEM(ui_slider),UI_ELEM(ui_slider_text));

    UI_Window ui_window2 = NEW_UI_WINDOW(sw-260,sh-260,256,256,BLACK,DARK_BLUE,2);
    UI_add_child(UI_ELEM(ui_screen),UI_ELEM(ui_window2));

    UI_Grid ui_grid = NEW_UI_GRID(0,0,256,256,64,64,0,0);
    UI_add_child(UI_ELEM(ui_window2),UI_ELEM(ui_grid));

    UI_Image ui_image_copies[16];
    for(uint i = 0; i < 16; i++){
        ui_image_copies[i] = NEW_UI_IMAGE(0,0,0,0,RECT_ZERO);
        UI_SET_IMAGE(ui_image_copies[i],ui_image.src);
        UI_add_child(UI_ELEM(ui_grid),UI_ELEM(ui_image_copies[i]));
    }

    // Game loop
    bool running = true;
    while(running){
    	// Regulate FPS
    	Uint32 now = SDL_GetTicks();
    	if(now-last_frame < 1000/MAX_FPS){
    	    SDL_Delay(1000/MAX_FPS-(SDL_GetTicks()-last_frame));
    	    now = SDL_GetTicks();
    	}
	
	    // deltaTime calculation
	    double deltaTime = (double)(now-last_frame)/1000.f;
	    time += deltaTime;
	    last_frame = now;

	    // Update FPS counter
	    sprintf(fps_counter.text,"%.1f FPS\nCamera: %.1f, %.1f, %.1f",1.f/deltaTime,-camera.pos.x,-camera.pos.y,-camera.pos.z);
	    DM_load_text(WHITE,0,&fps_counter);

	    // Handle events
	    Event e;
	    while(DM_poll_event(&e)){
	        if(IS_EVENT(e,QUIT))
	        	running = false;
	        if(UI_update(UI_ELEM(ui_screen),&e))
		    continue;
	    }
	
	    // Move camera
	    PlayerController_update(&pl_controller);
	    camera.pos = Vec3_sub(camera.pos, Vec3_scale(pl_controller.direction, pl_speed * deltaTime));
	
	    short player_y = Chunk_sample_height(Map_find_chunk(&map,-camera.pos.x,-camera.pos.z),-camera.pos.x,-camera.pos.z);
	
	    DM_clear_screen();
	
	    // Render map
	    Map_render(&map, &map_tiles, &camera, &isometric);

	    // Animate map
	    for(int i = 0; i < map.size; i++)
	        wave_function(&map.arr[i],time);

	    // Render player
	    DM_update_animation(&player_run);
	    DM_update_animation(&player_rotate);
	    DM_RENDER_TILE_SCALED(dummy_run,player_run.frame,player_rotate.frame,sw2-32,sh2-64-player_y*4,64,64);
		
	    // Update the slider text if needed
        if(ui_slider.selected){
            sprintf(ui_slider_text.src.text,"SLIDER VALUE: %g",ui_slider.value);
            UI_LOAD_TEXT(ui_slider_text);
        }

        // Render the UI
	    UI_render(UI_ELEM(ui_screen));
	
	    // Display FPS counter
	    DM_render_text(fps_counter,0,0,1.f);

    	DM_update_screen();
    }
    
    UI_free(UI_ELEM(ui_screen));
    Map_free(&map);

    free_resources();
    
    DM_quit();

    return 0;
}
