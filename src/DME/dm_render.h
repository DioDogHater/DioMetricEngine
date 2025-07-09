#ifndef DME_RENDER_H
#define DME_RENDER_H

#include "dm_engine.h"

// Screen refreshing
#define SET_COLOR(r,g,b,a) SDL_SetRenderDrawColor(renderer,(r),(g),(b),(a))
DM_FUNC void DM_clear_screen(){
	SET_COLOR(0,0,0,255);
	SDL_RenderClear(renderer);
}
DM_FUNC void DM_update_screen(){
	SDL_RenderPresent(renderer);
}

// Rendering

// DM_RENDER_TEXTURE(texture, x, y)
#define DM_RENDER_TEXTURE(t,x,y) ({\
	DM_render_texture((t),NULL,(Rect){(x),(y),(t).width,(t).height});\
})
// DM_RENDER_TEXTURE_SCALED(texture, x, y, w, h)
#define DM_RENDER_TEXTURE_SCALED(t,x,y,w,h) ({\
	DM_render_texture((t),NULL,(Rect){(x),(y),(w),(h)});\
})
// DM_RENDER_TEXTURE_CLIPPED(texture, cx, cy, cw, ch, x, y, w, h)
#define DM_RENDER_TEXTURE_CLIPPED(t,cx,cy,cw,ch,x,y,w,h) ({\
	Rect _cr = (Rect){(cx),(cy),(cw),(ch)};\
	DM_render_texture((t),&_cr,(Rect){(x),(y),(w),(h)});\
})

DM_FUNC void DM_render_texture(Texture t, Rect* c, Rect r){
	if(!t.src) return;
	SDL_RenderCopy(renderer,t.src,c,&r);
}

// Flipping constants
#define FLIP_X SDL_FLIP_HORIZONTAL
#define FLIP_Y SDL_FLIP_VERTICAL
#define FLIP_XY (FLIP_X & FLIP_Y)
#define FLIP_NONE SDL_FLIP_NONE

// DM_RENDER_TEXTURE_ROT(texture, x, y, w, h, angle)
#define DM_RENDER_TEXTURE_ROT(t,x,y,w,h,a) ({\
	DM_render_texture_ex((t),NULL,(Rect){(x),(y),(w),(h)},(a),NULL,FLIP_NONE);\
})
// DM_RENDER_TEXTURE_FLIP(texture, x, y, w, h, flip)
#define DM_RENDER_TEXTURE_FLIP(t,x,y,w,h,f) ({\
	DM_render_texture_ex((t),NULL,(Rect){(x),(y),(w),(h)},0.f,NULL,(f));\
})
// DM_RENDER_TEXTURE_CFLIP(texture, cx, cy, cw, ch, x, y, w, h, flip)
#define DM_RENDER_TEXTURE_CFLIP(t,cx,cy,cw,ch,x,y,w,h,f) ({\
	Rect _cr = (Rect){(cx),(cy),(cw),(ch)};\
	DM_render_texture_ex((t),&_cr,(Rect){(x),(y),(w),(h)},0.f,NULL,(f));\
})

DM_FUNC void DM_render_texture_ex(Texture t, Rect* c, Rect r, double angle, Vec2* center, SDL_RendererFlip flip){
	if(!t.src) return;
	SDL_RenderCopyEx(renderer,t.src,c,&r,angle,center,flip);
}

DM_FUNC void DM_render_text(Text t, int x, int y, float scale){
	if(!t.loaded.src) return;
	DM_RENDER_TEXTURE_SCALED(t.loaded,x,y,(int)((float)t.width*scale),(int)((float)t.height*scale));
}

DM_FUNC void DM_render_temp_text(const char* txt, Font* font, Color c, int x, int y, float scale){
	DM_ASSERTV(txt,"render_temp_text: NULL arg");
	Text tmp = NEW_TEXT(" ",font);
	strcpy(tmp.text,txt);
	DM_load_text(c, 0, &tmp);
	DM_render_text(tmp,x,y,scale);
	DM_free_text(&tmp);
}

// DM_RENDER_TILE(tileset, column, row, x, y)
#define DM_RENDER_TILE(t,c,r,x,y) ({\
	DM_render_tile((t),(c),(r),(SDL_Rect){(x),(y),(t).tile_width,(t).tile_height},0.f,NULL,FLIP_NONE);\
})

// DM_RENDER_TILE_SCALED(tileset, column, row, x, y, w, h)
#define DM_RENDER_TILE_SCALED(t,c,r,x,y,w,h) ({\
	DM_render_tile((t),(c),(r),(SDL_Rect){(x),(y),(w),(h)},0.f,NULL,FLIP_NONE);\
})

// DM_RENDER_TILE_ROT(tileset, column, row, x, y, w, h, angle)
#define DM_RENDER_TILE_ROT(t,c,r,x,y,w,h,a) ({\
	DM_render_tile((t),(c),(r),(SDL_Rect){(x),(y),(w),(h)},(a),NULL,FLIP_NONE);\
})

// DM_RENDER_TILE_FLIP(tileset, column, row, x, y, w, h, flip)
#define DM_RENDER_TILE_FLIP(t,c,r,x,y,w,h,f) ({\
	DM_render_tile((t),(c),(r),(SDL_Rect){(x),(y),(w),(h)},0.f,NULL,(f));\
})

DM_FUNC void DM_render_tile(Tileset ts, int col, int row, Rect r, double angle, Vec2* center, SDL_RendererFlip flip){
	if(!ts.asset.src) return;
	SDL_Rect cr = DM_get_tile(col,row,&ts);
	DM_render_texture_ex(ts.asset,&cr,r,angle,center,flip);
}

// DM_RENDER_ITILE(tileset, column, row, x, y)
#define DM_RENDER_TILE_I(t,i,x,y) ({\
	DM_render_tile_index((t),(i),(SDL_Rect){(x),(y),(t).tile_width,(t).tile_height},0.f,NULL,FLIP_NONE);\
})

// DM_RENDER_TILE_ISCALED(tileset, column, row, x, y, w, h)
#define DM_RENDER_TILE_ISCALED(t,i,x,y,w,h) ({\
	DM_render_tile_index((t),(i),(SDL_Rect){(x),(y),(w),(h)},0.f,NULL,FLIP_NONE);\
})

// DM_RENDER_TILE_IROT(tileset, column, row, x, y, w, h, angle)
#define DM_RENDER_TILE_IROT(t,c,r,x,y,w,h,a) ({\
	DM_render_tile_index((t),(i),(SDL_Rect){(x),(y),(w),(h)},(a),NULL,FLIP_NONE);\
})

// DM_RENDER_TILE_IFLIP(tileset, column, row, x, y, w, h, flip)
#define DM_RENDER_TILE_IFLIP(t,c,r,x,y,w,h,f) ({\
	DM_render_tile_index((t),(i),(SDL_Rect){(x),(y),(w),(h)},0.f,NULL,(f));\
})

DM_FUNC void DM_render_tile_index(Tileset* ts, uint index, Rect r, double angle, Vec2* center, SDL_RendererFlip flip){
	if(!ts->asset.src) return;
	SDL_Rect cr = DM_get_tile_index(index,ts);
	DM_render_texture_ex(ts->asset,&cr,r,angle,center,flip);
}

// Primitive shapes

DM_FUNC void DM_draw_rect(int x, int y, int w, int h, Color c){
	SET_COLOR(c.r,c.g,c.b,c.a);
	Rect r = (Rect){x,y,w,h};
	SDL_RenderDrawRect(renderer,&r);
}
DM_FUNC void DM_draw_border(int x, int y, int w, int h, int thick, Color c){
	for(; thick > 0; thick--)
		DM_draw_rect(x-thick,y-thick,w+(thick<<1),h+(thick<<1),c);
}
DM_FUNC void DM_fill_rect(int x, int y, int w, int h, Color c){
	SET_COLOR(c.r,c.g,c.b,c.a);
	Rect r = (Rect){x,y,w,h};
	SDL_RenderFillRect(renderer,&r);
}
DM_FUNC void DM_border_box(int x, int y, int w, int h, int thick, Color fg, Color bg){
	DM_fill_rect(x-thick,y-thick,w+(thick<<1),h+(thick<<1),bg);
	DM_fill_rect(x,y,w,h,fg);
}
DM_FUNC void DM_draw_line(int x0, int y0, int x1, int y1, Color c){
	SET_COLOR(c.r,c.g,c.b,c.a);
	SDL_RenderDrawLine(renderer,x0,y0,x1,y1);
}

#endif
