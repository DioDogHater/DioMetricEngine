#ifndef DME_UI_H
#define DME_UI_H

enum{
	DM_UI_BOUNDS,
	DM_UI_FRAME,
	DM_UI_WINDOW,
	DM_UI_COLOR_BUTTON,
	DM_UI_TEXT_BUTTON,
	DM_UI_TEXT,
	DM_UI_IMAGE
};

typedef unsigned short UI_Type;
union UI_Element;
typedef union UI_Element UI_Element;

#ifndef MAX_UI_CHILDREN
#define MAX_UI_CHILDREN 10
#endif

// UI Element basic members
#define DM_UI_BASICS \
	UI_Type type;\
	bool visible;\
	Rect rect;

// UI Element parent and children members
// UI Element dependencies
#define DM_UI_DEP \
	UI_Element* parent;\
	UI_Element* children[MAX_UI_CHILDREN+1];

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
} UI_Bounds;
#define NEW_UI_BOUNDS(x,y,w,h) (UI_Bounds){DM_UI_BOUNDS,true,RECT((x),(y),(w),(h)),NULL,{NULL}}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Color bg_color, border_color;
	uchar border_width;
} UI_Frame;
#define NEW_UI_FRAME(x,y,w,h,bgc,bc,bw) (UI_Frame){DM_UI_FRAME,true,RECT((x),(y),(w),(h)),NULL,{NULL},(bgc),(bc),(bw)}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Color bg_color, border_color;
	uchar border_width;
	bool movable, can_close, being_moved;
} UI_Window;
#define NEW_UI_WINDOW(x,y,w,h,bgc,bc,bw) (UI_Window){DM_UI_WINDOW,true,RECT((x),(y),(w),(h)),NULL,{NULL},(bgc),(bc),(bw),true,true,false}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	bool hovered, pressed, can_be_pressed;
	Color color, border_color;
	uchar border_width;	
} UI_ColorButton;
#define NEW_UI_COLOR_BUTTON(x,y,w,h,c,bc,bw) (UI_ColorButton){DM_UI_COLOR_BUTTON,true,RECT((x),(y),(w),(h)),NULL,{NULL},false,false,true,(c),(bc),(bw)}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	bool hovered, pressed, can_be_pressed;
	Color text_color, bg_color, border_color;
	uchar border_width;
	Text src;
} UI_TextButton;
#define NEW_UI_TEXT_BUTTON(x,y,w,h,bgc,bc,bw,tc,t,f) (UI_TextButton){DM_UI_TEXT_BUTTON,true,RECT((x),(y),(w),(h)),NULL,{NULL},false,false,true,(tc),(bgc),(bc),(bw),NEW_TEXT((t),(f))}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Color text_color;
	Text src;
} UI_Text;
#define NEW_UI_TEXT(x,y,w,h,c,t,f) (UI_Text){DM_UI_TEXT,true,RECT((x),(y),(w),(h)),NULL,{NULL},(c),NEW_TEXT((t),(f))}
#define UI_LOAD_TEXT(t) DM_load_text((t).text_color,(t).rect.w,&(t).src)
#define UI_SET_TEXT(d,s) (d).src = (s)

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Rect clip_rect;
	Texture src;
} UI_Image;
#define NEW_UI_IMAGE(x,y,w,h,c) (UI_Image){DM_UI_IMAGE,true,RECT((x),(y),(w),(h)),NULL,{NULL},(c),NEW_TEXTURE()}
#define UI_LOAD_IMAGE(t,s) DM_load_texture((s),&(t).src);
#define UI_SET_IMAGE(d,s) (d).src = (s)

union UI_Element{
	struct{
		DM_UI_BASICS
		DM_UI_DEP
	};
	UI_Bounds bounds;
	UI_Frame frame;
	UI_Window window;
	UI_ColorButton color_button;
	UI_TextButton text_button;
	struct{
		DM_UI_BASICS
		DM_UI_DEP
		bool hovered, pressed, can_be_pressed;
	} button;
	UI_Text text;
	UI_Image image;
};

#define UI_ELEM(x) (UI_Element*)&(x)

DM_FUNC void UI_add_child(UI_Element* parent, UI_Element* child){
	DM_ASSERTV(parent && child,"UI_add_child: NULL arg");
	child->parent = parent;
	UI_Element** ptr = parent->children;
	for(;*ptr;ptr++);
	*(ptr++) = child;
	*ptr = NULL;
}

DM_FUNC void UI_remove_child(UI_Element* parent, UI_Element* child){
	DM_ASSERTV(parent && child,"UI_remove_child: NULL arg");
	for(UI_Element** ptr = parent->children; *ptr; ptr++){
		if(*ptr != child)
			continue;
		*(ptr++) = NULL;
		for(;*ptr;ptr++)
			*(ptr-1) = *ptr;
	}
}

DM_FUNC Rect UI_get_rect(UI_Element* ui){
	if(!ui) return RECT_ZERO;
	Rect b = RECT_ZERO;
	for(UI_Element* ptr = ui; ptr; ptr = ptr->parent){
		b.x += ptr->rect.x;
		b.y += ptr->rect.y;
	}
	b.w = ui->rect.w;
	b.h = ui->rect.h;
	return b;
}

DM_FUNC void UI_free(UI_Element* ui){
	if(!ui) return;
	for(uint i = 0; i < MAX_UI_CHILDREN && ui->children[i]; i++)
		UI_free(ui->children[i]);
	switch(ui->type){
	case DM_UI_TEXT_BUTTON:
		DM_free_text(&ui->text_button.src);
		break;
	case DM_UI_TEXT:
		DM_free_text(&ui->text.src);
		break;
	case DM_UI_IMAGE:
		DM_free_texture(&ui->image.src);
		break;
	}
}

#define UI_WINDOW_BUTTON_W 7
#define UI_WINDOW_BUTTON_H 5

DM_FUNC void UI_render(UI_Element* ui){
	if(!ui || !ui->visible) return;
	Rect rect = UI_get_rect(ui);
	switch(ui->type){
	case DM_UI_FRAME:
		DM_border_box(
			rect.x, rect.y,
			rect.w, rect.h,
			ui->frame.border_width,
			ui->frame.bg_color,
			ui->frame.border_color
		);
		break;
	case DM_UI_WINDOW:
		DM_border_box(
			rect.x, rect.y,
			rect.w, rect.h,
			ui->window.border_width,
			ui->window.bg_color,
			ui->window.border_color
		);
		DM_fill_rect(
			rect.x-ui->window.border_width,
			rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width-1,
			rect.w+ui->window.border_width*2,
			UI_WINDOW_BUTTON_H+2,
			ui->window.border_color
		);
		if(ui->window.can_close)
			DM_border_box(
				rect.x+rect.w+ui->window.border_width-UI_WINDOW_BUTTON_W-1,
				rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width,
				UI_WINDOW_BUTTON_W,
				UI_WINDOW_BUTTON_H,
				1, LIGHT_RED, BLACK
			);
		break;
	case DM_UI_COLOR_BUTTON:{
		float color_scale = ((ui->color_button.pressed || !ui->color_button.can_be_pressed) ? 0.5f : ((ui->color_button.hovered) ? 0.75f : 1.f));
		DM_border_box(
			rect.x, rect.y,
			rect.w, rect.h,
			ui->color_button.border_width,
			SCALE_COLOR(ui->color_button.color,color_scale),
			ui->color_button.border_color
		);
		break;
	}
	case DM_UI_TEXT_BUTTON:{
		float color_scale = ((ui->text_button.pressed || !ui->text_button.can_be_pressed) ? 0.5f : ((ui->text_button.hovered) ? 0.75f : 1.f));
		DM_border_box(
			rect.x, rect.y,
			rect.w, rect.h,
			ui->text_button.border_width,
			SCALE_COLOR(ui->text_button.bg_color,color_scale),
			ui->text_button.border_color
		);
		DM_render_text(ui->text_button.src,rect.x-ui->text_button.src.width/2+rect.w/2,rect.y-ui->text_button.src.height/2+rect.h/2,1.f);
		break;
	}
	case DM_UI_TEXT:
		DM_render_text(ui->text.src,rect.x,rect.y,1.f);
		break;
	case DM_UI_IMAGE:
		DM_render_texture(ui->image.src,(RECT_CMP(ui->image.clip_rect,RECT_ZERO) ? NULL : &ui->image.clip_rect),rect);
		break;
	}
	for(uint i = 0; i < MAX_UI_CHILDREN && ui->children[i]; i++)
		UI_render(ui->children[i]);
}

DM_FUNC bool UI_update(UI_Element* ui, Event* e){
	if(!ui || !e || !ui->visible)
		return false;	

	for(uint i = 0; i < MAX_UI_CHILDREN && ui->children[i]; i++){
		if(UI_update(ui->children[i],e))
			return true;
	}

	if(	ui->type != DM_UI_WINDOW &&
		ui->type != DM_UI_TEXT_BUTTON &&
		ui->type != DM_UI_COLOR_BUTTON)
		return false;

	switch(e->type){
	case EVENT(MOUSEBUTTONDOWN):
		if(e->button.button != 1)
			break;
		Rect rect = UI_get_rect(ui);
		if(ui->type == DM_UI_WINDOW){
			Rect test_rect = RECT(
				rect.x+rect.w+ui->window.border_width-UI_WINDOW_BUTTON_W-1,
				rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width,
				UI_WINDOW_BUTTON_W,
				UI_WINDOW_BUTTON_H
			);
			if(VEC2_IN_RECT(e->button,test_rect)){
				ui->visible = false;
				printf("Closed window!\n");
				return true;
			}
			test_rect = RECT(
				rect.x-ui->window.border_width,
				rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width-1,
				rect.w+ui->window.border_width*2,
				UI_WINDOW_BUTTON_H+2
			);
			if(VEC2_IN_RECT(e->button,test_rect)){
				printf("Moving window!\n");
				ui->window.being_moved = true;
				return true;
			}
		}else if((ui->type == DM_UI_TEXT_BUTTON || ui->type == DM_UI_COLOR_BUTTON) && ui->button.can_be_pressed && VEC2_IN_RECT(e->button,rect)){
			printf("Clicked on me!\n");
			ui->button.pressed = true;
			return true;
		}
		break;
	case EVENT(MOUSEBUTTONUP):
		if((ui->type == DM_UI_TEXT_BUTTON || ui->type == DM_UI_COLOR_BUTTON) && ui->button.pressed)
			ui->button.pressed = false;
		else if(ui->type == DM_UI_WINDOW && ui->window.being_moved)
			ui->window.being_moved = false;
		else
			break;
		return true;
	case EVENT(MOUSEMOTION):
		if((ui->type == DM_UI_TEXT_BUTTON || ui->type == DM_UI_COLOR_BUTTON) && ui->button.can_be_pressed){
			Rect rect = UI_get_rect(ui);
			ui->button.hovered = VEC2_IN_RECT(e->motion, rect);
		}else if(ui->type == DM_UI_WINDOW && ui->window.being_moved){
			ui->rect.x += e->motion.xrel;
			ui->rect.y += e->motion.yrel;
			ui->rect.x = CLAMP(ui->rect.x, 0, ui->parent->rect.w - ui->rect.w);
			ui->rect.y = CLAMP(ui->rect.y, UI_WINDOW_BUTTON_H, ui->parent->rect.h - ui->rect.h);
			return true;
		}
	}	
	
	return false;
}

#endif
