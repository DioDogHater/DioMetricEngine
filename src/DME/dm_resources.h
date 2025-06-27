#ifndef DME_RESOURCES_H
#define DME_RESOURCES_H

#include "dm_engine.h"

// Texture
typedef struct{
	SDL_Texture* src;
	uint width, height;
} Texture;
#define NEW_TEXTURE() (Texture){NULL,0,0}

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
#define NEW_TEXT(t,f) (Text){NEW_TEXTURE(),(t),(f)}

// Tileset
typedef struct{
	Texture asset;
	int x_offset, y_offset;
	uint tile_width, tile_height;
	uint rows, cols;
} Tileset;
#define NEW_TILESET(xo,yo,w,h) (Tileset){NEW_TEXTURE(),(xo),(yo),(w),(h),0,0}

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

DM_FUNC void DM_free_text(Text* dest){
	DM_ASSERTV(dest,"free_text: NULL arg");
	DM_free_texture(&dest->loaded);
	dest->font = NULL;
}

DM_FUNC bool DM_load_text(Color c, uint max_width, Text* dest){
	DM_ASSERT(dest,"render_text: NULL arg");
	if(!dest->font || !(*dest->font)) return true;
	SDL_Surface* loaded_surface = TTF_RenderText_Solid_Wrapped(*dest->font, (const char*)dest->text, c, (max_width)?max_width:(99999));
	if(!loaded_surface){
		DM_ERR("TTF_RenderText_Solid(%s) error: %s",dest->text,TTF_GetError());
		return false;
	}
	SDL_Texture* loaded_texture = SDL_CreateTextureFromSurface(renderer,loaded_surface);
	SDL_FreeSurface(loaded_surface);
	if(!loaded_texture){
		DM_ERR("SDL_CreateTextureFRomSurface error: %s",SDL_GetError());
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

DM_FUNC SDL_Rect DM_get_tile(uint col, uint row, Tileset* ts){
	if(!ts){ DM_ERR("get_tile: NULL arg"); return (SDL_Rect){0,0,0,0}; }
	if(row >= ts->rows || col >= ts->cols)
		return (SDL_Rect){0,0,0,0};
	return (SDL_Rect){
		col*ts->tile_width+ts->x_offset,
		row*ts->tile_height+ts->y_offset,
		ts->tile_width,
		ts->tile_height
	};
}

DM_FUNC SDL_Rect DM_get_tile_index(uint index, Tileset* ts){
	if(!ts){ DM_ERR("get_tile_index: NULL arg"); return (SDL_Rect){0,0,0,0}; }
	if(index >= ts->cols * ts->rows)
		return (SDL_Rect){0,0,0,0};
	return (SDL_Rect){
		(index % ts->cols)*ts->tile_width+ts->x_offset,
		(index / ts->cols)*ts->tile_height+ts->y_offset,
		ts->tile_width,
		ts->tile_height
	};
}

#endif
