#ifndef DME_RESOURCES_H
#define DME_RESOURCES_H

#include <stdarg.h>
#include "dm_engine.h"

// Texture
typedef struct{
	SDL_Texture* src;
	uint width, height;
} Texture;
#define NEW_TEXTURE() {NULL,0,0}

// Font
typedef TTF_Font* Font;
#define NEW_FONT() (Font)NULL

// Text
#define TEXT_MAX_CHAR 256
typedef union{
	struct{
		Texture loaded;
		char text[TEXT_MAX_CHAR];
		Font* font;
	};
	struct{
		SDL_Texture* src;
		uint width, height;
	};
} Text;
#define NEW_TEXT(t,f) {.loaded=NEW_TEXTURE(),.text=(t),.font=(f)}

// Tileset
typedef struct{
	Texture asset;
	int x_offset, y_offset;
	uint tile_width, tile_height;
	uint rows, cols;
} Tileset;
#define NEW_TILESET(xo,yo,w,h) {NEW_TEXTURE(),(xo),(yo),(w),(h),0,0}

// Resource Loading (RL)
typedef enum{
	RL_Texture,
	RL_Font,
	RL_Text,
	RL_Tileset,
	RL_None
} RL_type;	// Resource Loading Type
typedef union{
	Texture* texture;
	Font* font;
	Text* text;
	Tileset* tileset;
	void* _invalid;
} RL_ptr;	// Resource Loading Pointer
typedef union{
	const char* path;
	struct{
		const char* font_path;
		uint font_size;
	};
	Color color;
} RL_src;
typedef struct{
	RL_type type;
	RL_ptr res;
	RL_src src;
} RL_Res;	// Resource Loading Resource
typedef RL_Res RL_Array[];	// Resource Loading Array
// Macros for convenience
#define RL_ELEM(t,r,s) {(RL_type)(t),(RL_ptr)(r),(RL_src)(s)}
#define RL_TEXTURE(r,s) {(RL_type)(RL_Texture),(RL_ptr){.texture=&(r)},(RL_src){(const char*)(s)}}
#define RL_FONT(r,sz,s) {(RL_type)(RL_Font),(RL_ptr){.font=&(r)},(RL_src){.font_path=(const char*)(s), .font_size=(sz)}}
#define RL_TEXT(r,s) {(RL_type)(RL_Text),(RL_ptr){.text=&(r)},(RL_src){.color=(Color)(s)}}
#define RL_TILESET(r,s) {(RL_type)(RL_Tileset),(RL_ptr){.tileset=&(r)},(RL_src){(const char*)(s)}}
#define RL_NULL() {(RL_type)(RL_None),(RL_ptr){._invalid=NULL},(const char*)NULL}

// Resource handling
DM_FUNC void DM_free_texture(Texture* dest){
	DM_ASSERTV(dest,"free_texture: NULL arg");
	if(dest->src)
		SDL_DestroyTexture(dest->src);
	dest->src = NULL;
	dest->width = 0;
	dest->height = 0;
}

DM_FUNC bool DM_load_texture(const char* src, Texture* dest){
	DM_ASSERT(src && dest,"load_texture: NULL arg");
	SDL_Surface* loaded_surface = IMG_Load(src);
	if(!loaded_surface){
		DM_ERR("IMG_Load(%s) error: %s",src,IMG_GetError());
		return false;
	}
	SDL_Texture* loaded_texture = SDL_CreateTextureFromSurface(renderer,loaded_surface);
	SDL_FreeSurface(loaded_surface);
	if(!loaded_texture){
		DM_ERR("SDL_CreateTextureFromSurface error: %s",SDL_GetError());
		return false;
	}
	if(dest->src)
		DM_free_texture(dest);
	dest->src = loaded_texture;
	SDL_QueryTexture(loaded_texture,NULL,NULL,&dest->width,&dest->height);
	return true;
}

DM_FUNC void DM_free_font(Font* dest){
	DM_ASSERTV(dest,"free_font: NULL arg");
	if(*dest)
		TTF_CloseFont(*dest);
	*dest = NULL;
}

DM_FUNC bool DM_load_font(const char* src, uint size, Font* dest){
	DM_ASSERT(src && dest,"load_font: NULL arg");
	Font loaded_font = TTF_OpenFont(src, size);
	if(!loaded_font){
		DM_ERR("TTF_OpenFont(%s) error: %s",src,TTF_GetError());
		return false;
	}
	if(*dest)
		DM_free_font(dest);
	*dest = loaded_font;
	return true;
}

DM_FUNC void DM_set_font_size(Font dest, uint size){
	DM_ASSERTV(dest,"set_font_size: NULL arg");
	TTF_SetFontSize(dest,size);
}

DM_FUNC void DM_free_text(Text* dest){
	DM_ASSERTV(dest,"free_text: NULL arg");
	DM_free_texture(&dest->loaded);
	dest->font = NULL;
}

DM_FUNC bool DM_load_text(Color c, uint max_width, Text* dest){
	DM_ASSERT(dest,"render_text: NULL arg");
	if(!dest->font || !(*dest->font) || strlen(dest->text) < 1) return true;
	SDL_Surface* loaded_surface = TTF_RenderText_Solid_Wrapped(*dest->font, (const char*)dest->text, c, (max_width)?max_width:(sw));
	if(!loaded_surface){
		DM_ERR("TTF_RenderText_Solid(%s) error: %s",dest->text,TTF_GetError());
		return false;
	}
	SDL_Texture* loaded_texture = SDL_CreateTextureFromSurface(renderer,loaded_surface);
	SDL_FreeSurface(loaded_surface);
	if(!loaded_texture){
		DM_ERR("SDL_CreateTextureFromSurface error: %s",SDL_GetError());
		return false;
	}
	if(dest->loaded.src)
		DM_free_texture(&dest->loaded);
	dest->loaded.src = loaded_texture;
	SDL_QueryTexture(loaded_texture,NULL,NULL,&dest->loaded.width,&dest->loaded.height);
	return true;
}

DM_FUNC void DM_free_tileset(Tileset* dest){
	DM_ASSERTV(dest,"free_tileset: NULL arg");
	DM_free_texture(&dest->asset);
	dest->rows = 0;
	dest->cols = 0;
}

DM_FUNC bool DM_load_tileset(const char* src, Tileset* dest){
	DM_ASSERT(src && dest,"load_tileset: NULL arg");
	if(!DM_load_texture(src,&dest->asset))
		return false;
	if(!dest->tile_width || !dest->tile_height){ DM_ERR("Tile dimension = 0"); return false; }
	dest->cols = (dest->asset.width - dest->x_offset) / dest->tile_width;
	dest->rows = (dest->asset.height - dest->y_offset) / dest->tile_height;
	return true;
}

DM_FUNC Rect DM_get_tile(uint col, uint row, Tileset* ts){
	if(!ts){ DM_ERR("get_tile: NULL arg"); return RECT_ZERO; }
	if(row >= ts->rows || col >= ts->cols)
		return RECT_ZERO;
	return RECT(
		col*ts->tile_width+ts->x_offset,
		row*ts->tile_height+ts->y_offset,
		ts->tile_width,
		ts->tile_height
	);
}

DM_FUNC Rect DM_get_tile_index(uint index, Tileset* ts){
	if(!ts){ DM_ERR("get_tile_index: NULL arg"); return RECT_ZERO; }
	if(index >= ts->cols * ts->rows)
		return RECT_ZERO;
	return RECT(
		(index % ts->cols)*ts->tile_width+ts->x_offset,
		(index / ts->cols)*ts->tile_height+ts->y_offset,
		ts->tile_width,
		ts->tile_height
	);
}

DM_FUNC bool DM_load_resource_array(RL_Array rla){
	DM_ASSERT(rla,"load_RL_Array: NULL arg");
	for(; (rla->type != RL_None) && (rla->res._invalid != NULL); rla++){
		switch(rla->type){
		case RL_Texture:
			if(!DM_load_texture(rla->src.path, rla->res.texture))
				return false;
			break;
		case RL_Font:
			if(!DM_load_font(rla->src.font_path, rla->src.font_size, rla->res.font))
				return false;
			break;
		case RL_Text:
			if(!DM_load_text(rla->src.color, 0, rla->res.text))
				return false;
			break;
		case RL_Tileset:
			if(!DM_load_tileset(rla->src.path, rla->res.tileset))
				return false;
			break;
		default:
			DM_ERR("load_RL_Array: invalid resource");
			return false;
		}
	}

	return true;
}

DM_FUNC void DM_free_resource_array(RL_Array rla){
	DM_ASSERTV(rla,"free_RL_Array: NULL arg");
	for(; (rla->type != RL_None) && (rla->res._invalid != NULL); rla++){
		switch(rla->type){
		case RL_Texture:
			DM_free_texture(rla->res.texture);
			break;
		case RL_Font:
			DM_free_font(rla->res.font);
			break;
		case RL_Text:
			DM_free_text(rla->res.text);
			break;
		case RL_Tileset:
			DM_free_tileset(rla->res.tileset);
			break;
		default:
			DM_ERR("free_RL_Array: invalid resource");
			return;
		}
	}
}

// Fast and easy text formatting without worrying about memory management
#ifndef DM_FORMAT_SIZE
#define DM_FORMAT_SIZE 512
#endif

char _format_tmp[DM_FORMAT_SIZE] = {0};

#define DM_FORMAT(fmt,...) sprintf(_format_tmp,(fmt),##__VA_ARGS__)
#define DM_FORMAT_STR _format_tmp

#endif
