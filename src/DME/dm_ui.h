#ifndef DME_UI_H
#define DME_UI_H

enum{
	DM_UI_BOUNDS,
	DM_UI_FRAME,
	DM_UI_WINDOW,
	DM_UI_COLOR_BUTTON,
	DM_UI_TEXT_BUTTON,
	DM_UI_TEXT,
	DM_UI_IMAGE,
	DM_UI_SLIDER,
	DM_UI_GRID,
	DM_UI_SCROLL,
	DM_UI_CONSOLE,
	DM_UI_INPUT_FIELD
};

typedef unsigned short UI_Type;
union UI_Element;
typedef union UI_Element UI_Element;
typedef void (*UI_Callback)(UI_Element*);

#ifndef MAX_UI_CHILDREN
#define MAX_UI_CHILDREN 20
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
	UI_Callback on_press, on_release;
	Color color, border_color;
	uchar border_width;	
} UI_ColorButton;
#define NEW_UI_COLOR_BUTTON(x,y,w,h,c,bc,bw) (UI_ColorButton){DM_UI_COLOR_BUTTON,true,RECT((x),(y),(w),(h)),NULL,{NULL},false,false,true,NULL,NULL,(c),(bc),(bw)}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	bool hovered, pressed, can_be_pressed;
	UI_Callback on_press, on_release;
	Color text_color, bg_color, border_color;
	uchar border_width;
	Text src;
	bool copy;
} UI_TextButton;
#define NEW_UI_TEXT_BUTTON(x,y,w,h,bgc,bc,bw,tc,t,f) (UI_TextButton){DM_UI_TEXT_BUTTON,true,RECT((x),(y),(w),(h)),NULL,{NULL},false,false,true,NULL,NULL,(tc),(bgc),(bc),(bw),NEW_TEXT((t),(f))}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Color text_color;
	Text src;
	bool copy;
} UI_Text;
#define NEW_UI_TEXT(x,y,w,h,c,t,f) (UI_Text){DM_UI_TEXT,true,RECT((x),(y),(w),(h)),NULL,{NULL},(c),NEW_TEXT((t),(f)),false}
#define UI_LOAD_TEXT(t) ({if((t).copy){(t).src=(Text)NEW_TEXTURE();(t).copy = false;}DM_load_text((t).text_color,(t).rect.w,&(t).src);})
#define UI_SET_TEXT(d,s) ({(d).src=(s);(d).copy=true;})

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Rect clip_rect;
	Texture src;
	bool copy;
} UI_Image;
#define NEW_UI_IMAGE(x,y,w,h,c) (UI_Image){DM_UI_IMAGE,true,RECT((x),(y),(w),(h)),NULL,{NULL},(c),NEW_TEXTURE(),false}
#define UI_LOAD_IMAGE(t,s) ({if((t).copy){(t).src=(Texture)NEW_TEXTURE();}DM_load_texture((s),&(t).src);})
#define UI_SET_IMAGE(d,s) ({(d).src=(s);(d).copy=true;})

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	Color line_color, handle_color;
	uchar line_width, handle_width;
	float minimum, maximum;
	float value, step;
	bool selected;
} UI_Slider;
#define NEW_UI_SLIDER(x,y,w,h,lc,hc,_min,_max,v,s) (UI_Slider){DM_UI_SLIDER,true,RECT((x),(y),(w),(h)),NULL,{NULL},(lc),(hc),2,3,(_min),(_max),(v),(s),false}

#ifndef MAX_UI_GRID_CHILDREN
#define MAX_UI_GRID_CHILDREN 32
#endif

typedef struct{
	DM_UI_BASICS
	UI_Element* parent;
	UI_Element* children[MAX_UI_GRID_CHILDREN];
	uint cell_width, cell_height;
	Vec2 padding;
} UI_Grid;
#define NEW_UI_GRID(x,y,w,h,cw,ch,px,py) (UI_Grid){DM_UI_GRID,true,RECT((x),(y),(w),(h)),NULL,{NULL},(cw),(ch),VEC2((px),(py))}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	uint bar_width, content_height;
	Color bar_color;
	int scroll;
} UI_Scroll;
#define NEW_UI_SCROLL(x,y,w,h,bw,bc,ch) (UI_Scroll){DM_UI_SCROLL,true,RECT((x),(y),(w),(h)),NULL,{NULL},(bw),(ch),(bc),0}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	ushort cursor;
	Text src;
} UI_Console;
#define NEW_UI_CONSOLE(x,y,w,h,f) (UI_Console){DM_UI_CONSOLE,true,RECT((x),(y),(w),(h)),NULL,{NULL},0,NEW_TEXT("\0",(f))}

typedef struct{
	DM_UI_BASICS
	DM_UI_DEP
	UI_Callback on_change, on_enter;
	Text src;
} UI_InputField;
#define NEW_UI_INPUT_FIELD(x,y,w,h,f) (UI_InputField){DM_UI_INPUT_FIELD,true,RECT((x),(y),(w),(h)),NULL,{NULL},NULL,NULL,NEW_TEXT("\0",(f))}
#define UI_GET_FIELD(f) ((f).src.text)
#define UI_CLEAR_FIELD(f) ({(f).src.text[0]='\0';})

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
		UI_Callback on_press, on_release;
	} button;
	UI_Text text;
	UI_Image image;
	UI_Slider slider;
	UI_Grid grid;
	UI_Scroll scroll;
	UI_Console console;
	UI_InputField field;
};

#define UI_ELEM(x) (UI_Element*)&(x)

DM_FUNC void UI_update_grid(UI_Element* ui){
	uint columns = (ui->rect.w + ui->grid.padding.x) / (ui->grid.cell_width + ui->grid.padding.x);
	for(uint i = 0; i < MAX_UI_GRID_CHILDREN && ui->children[i]; i++){
		ui->children[i]->rect = RECT(
			(i % columns) * (ui->grid.cell_width + ui->grid.padding.x),
			(i / columns) * (ui->grid.cell_height + ui->grid.padding.y),
			ui->grid.cell_width,
			ui->grid.cell_height
		);
	}
}

DM_FUNC void UI_add_child(UI_Element* parent, UI_Element* child){
	DM_ASSERTV(parent && child,"UI_add_child: NULL arg");
	child->parent = parent;
	UI_Element** ptr = parent->children;
	for(;*ptr;ptr++);
	*(ptr++) = child;
	*ptr = NULL;
	if(parent->type == DM_UI_GRID)
		UI_update_grid(parent);
}

DM_FUNC int UI_find_child(UI_Element* parent, UI_Element* child){
	if(!parent || !child){ DM_ERR("UI_find_child: NULL arg"); return -1; }
	for(UI_Element** ptr = parent->children; *ptr; ptr++){
		if(*ptr == child)
			return ptr - parent->children;
	}
	return -1;
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
	if(parent->type == DM_UI_GRID)
		UI_update_grid(parent);
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
	for(UI_Element** ptr = ui->children; *ptr; ptr++)
		UI_free(*ptr);
	switch(ui->type){
	case DM_UI_TEXT_BUTTON:
		if(!ui->image.copy)
			DM_free_text(&ui->text_button.src);
		break;
	case DM_UI_TEXT:
		if(!ui->image.copy)
			DM_free_text(&ui->text.src);
		break;
	case DM_UI_IMAGE:
		if(!ui->image.copy)
			DM_free_texture(&ui->image.src);
		break;
	}
}

#define UI_SCROLL_BAR_HEIGHT 15
DM_FUNC void UI_render_scroll(UI_Element* ui, Rect rect){
	for(UI_Element** ptr = ui->children; *ptr; ptr++)
		ui_render(*ptr, VEC2(rect.x, rect.y-ui->scroll.scroll));
	DM_fill_rect(
		0, (rect.h-UI_SCROLL_BAR_HEIGHT)*ui->scroll.content_height/ui->scroll.scroll,
		UI_SCROLL_BAR_HEIGHT, ui->scroll.bar_width,
		ui->scroll.bar_color
	);
}

#define UI_WINDOW_BUTTON_W 7
#define UI_WINDOW_BUTTON_H 5

DM_FUNC void UI_render(UI_Element* ui, Vec2 parent_pos){
	if(!ui || !ui->visible) return;
	Rect rect = RECT(parent_pos.x,parent_pos.y,ui->rect.w,ui->rect.h);
	switch(ui->type){
	// UI_Frame
	case DM_UI_FRAME:
		DM_border_box(
			rect.x, rect.y,
			rect.w, rect.h,
			ui->frame.border_width,
			ui->frame.bg_color,
			ui->frame.border_color
		);
		break;
	// UI_Window
	case DM_UI_WINDOW:
		DM_border_box(	// Frame of the window
			rect.x, rect.y,
			rect.w, rect.h,
			ui->window.border_width,
			ui->window.bg_color,
			ui->window.border_color
		);
		DM_fill_rect(	// Space at the top of the window
			rect.x-ui->window.border_width,
			rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width-1,
			rect.w+ui->window.border_width*2,
			UI_WINDOW_BUTTON_H+2,
			ui->window.border_color
		);
		// Close button
		if(ui->window.can_close)
			DM_border_box(
				rect.x+rect.w+ui->window.border_width-UI_WINDOW_BUTTON_W-1,
				rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width,
				UI_WINDOW_BUTTON_W,
				UI_WINDOW_BUTTON_H,
				1, LIGHT_RED, BLACK
			);
		break;
	// UI_ColorButton
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
	// UI_TextButton
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
	// UI_Text
	case DM_UI_TEXT:
		DM_render_text(ui->text.src,rect.x,rect.y,1.f);
		break;
	// UI_Image
	case DM_UI_IMAGE:
		DM_render_texture(ui->image.src,(RECT_CMP(ui->image.clip_rect,RECT_ZERO) ? NULL : &ui->image.clip_rect),rect);
		break;
	// UI_Slider
	case DM_UI_SLIDER:
		// Line
		DM_fill_rect(rect.x,rect.y+rect.h/2-ui->slider.line_width/2,rect.w,ui->slider.line_width,ui->slider.line_color);
		// Slider "handle" (Cursor)
		DM_fill_rect(rect.x+(int)((ui->slider.value-ui->slider.minimum)/ui->slider.maximum*(float)rect.w)-ui->slider.handle_width/2,rect.y,ui->slider.handle_width,rect.h,ui->slider.handle_color);
		break;
	case DM_UI_SCROLL:
		return UI_render_scroll(ui,rect);
	case DM_UI_CONSOLE:
		DM_RENDER_TEXTURE_CLIPPED(ui->console.src.loaded,0,ui->console.src.height-rect.h,rect.w,rect.h,rect.x,rect.y,rect.w,rect.h);
		break;
	case DM_UI_INPUT_FIELD:
		DM_render_text(ui->field.src,rect.x+rect.w/2-ui->field.src.width/2,rect.y,1.f);
		break;
	}
	// Render all children next
	for(UI_Element** ptr = ui->children; *ptr; ptr++)
		UI_render(*ptr, VEC2(parent_pos.x+ui->rect.x,parent_pos.y+ui->rect.y));
}

DM_FUNC void UI_update_slider_value(UI_Element* ui, int mouse_x, Rect rect){
	ui->slider.value = LERP(ui->slider.minimum,ui->slider.maximum,(float)(mouse_x - rect.x) / (float)rect.w);
	ui->slider.value = CLAMP(ui->slider.value,ui->slider.minimum,ui->slider.maximum);
	if(ui->slider.step != 0.f)
		ui->slider.value = roundf(ui->slider.value / ui->slider.step) * ui->slider.step;
}

DM_FUNC bool UI_update(UI_Element* ui, Event* e){
	// Check if we should update
	if(!ui || !e || !ui->visible)
		return false;

	// Update children first
	for(UI_Element** ptr = ui->children; *ptr; ptr++){
		if(UI_update(*ptr,e))
			return true;
	}

	switch(e->type){
	// Click start
	case EVENT(MOUSEBUTTONDOWN):
		if(e->button.button != 1)
			break;
		Rect rect = UI_get_rect(ui);
		
		// UI_Window
		if(ui->type == DM_UI_WINDOW){
			Rect test_rect = RECT(
				rect.x+rect.w+ui->window.border_width-UI_WINDOW_BUTTON_W-1,
				rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width,
				UI_WINDOW_BUTTON_W,
				UI_WINDOW_BUTTON_H
			);
			// Close button
			if(VEC2_IN_RECT(e->button,test_rect) && ui->window.can_close){
				ui->visible = false;
				return true;
			}
			test_rect = RECT(
				rect.x-ui->window.border_width,
				rect.y-UI_WINDOW_BUTTON_H-ui->window.border_width-1,
				rect.w+ui->window.border_width*2,
				UI_WINDOW_BUTTON_H+2
			);
			// Moving window
			if(VEC2_IN_RECT(e->button,test_rect)){
				ui->window.being_moved = true;
				return true;
			}
		} // UI_TextButton or UI_ColorButton
		else if((ui->type == DM_UI_TEXT_BUTTON || ui->type == DM_UI_COLOR_BUTTON) && ui->button.can_be_pressed && VEC2_IN_RECT(e->button,rect)){
			if(ui->button.on_press)
				ui->button.on_press(ui);
			ui->button.pressed = true;
			return true;
		} // UI_Slider
		else if((ui->type == DM_UI_SLIDER) && VEC2_IN_RECT(e->button,rect)){
			UI_update_slider_value(ui,e->button.x,rect);
			ui->slider.selected = true;
			return true;
		}
		break;
	// Click end
	case EVENT(MOUSEBUTTONUP):
		// UI_TextButton or UI_ColorButton
		if((ui->type == DM_UI_TEXT_BUTTON || ui->type == DM_UI_COLOR_BUTTON) && ui->button.pressed){
			if(ui->button.on_release)
				ui->button.on_release(ui);
			ui->button.pressed = false;
		} // UI_Window
		else if(ui->type == DM_UI_WINDOW && ui->window.being_moved)
			ui->window.being_moved = false;
		// UI_Slider
		else if(ui->type == DM_UI_SLIDER && ui->slider.selected)
			ui->slider.selected = false;
		else
			break;
		return true;
	// Mouse movement
	case EVENT(MOUSEMOTION):
		// UI_TextButton or UI_ColorButton
		if((ui->type == DM_UI_TEXT_BUTTON || ui->type == DM_UI_COLOR_BUTTON) && ui->button.can_be_pressed){
			Rect rect = UI_get_rect(ui);
			ui->button.hovered = VEC2_IN_RECT(e->motion, rect);
		} // UI_Window
		else if(ui->type == DM_UI_WINDOW && ui->window.being_moved){
			ui->rect.x += e->motion.xrel;
			ui->rect.y += e->motion.yrel;
			ui->rect.x = CLAMP(ui->rect.x, 0, ui->parent->rect.w - ui->rect.w);
			ui->rect.y = CLAMP(ui->rect.y, UI_WINDOW_BUTTON_H, ui->parent->rect.h - ui->rect.h);
			return true;
		} // UI_Slider
		else if(ui->type == DM_UI_SLIDER && ui->slider.selected){
			UI_update_slider_value(ui,e->motion.x,UI_get_rect(ui));
			return true;
		}
		break;
	case EVENT(MOUSEWHEEL):
		if(ui->type == DM_UI_SCROLL){
			Rect rect = UI_get_rect(ui);
			Vec2 mouse = VEC2(e->wheel.mouseX,e->wheel.mouseY);
			if(VEC2_IN_RECT(mouse,rect)){
				ui->scroll.scroll += e->wheel.y;
				ui->scroll.scroll = CLAMP(ui->scroll.scroll, 0, ui->scroll.content_height-rect.h);
			}
		}
		break;
	}
	
	return false;
}

#endif
