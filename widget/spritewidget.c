/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */

#include "widget.h"

#include "stb_image.h"


void widget_sprite_destroy() {
}

void widget_sprite_start() {
	struct widget_sprite *wgspr = (struct widget_sprite *)_current_widget;
}

void widget_sprite_draw() {
	struct node *this = _current_node;
	struct widget_sprite *wgspr = (struct widget_sprite *)_current_widget;

	if(wgspr->tex == NULL)return;
	if(wgspr->sheet == NULL)return;

	struct rect *rect = spritesheet_get_rect(wgspr->sheet);
	if(rect != NULL) {
		int sw = (int)rect->w;
		int sh = (int)rect->h;
		if (sw + (int)rect->x > wgspr->tex->w) sw -= (int)rect->x - wgspr->tex->w;
		if (sh + (int)rect->y > wgspr->tex->w) sh -= (int)rect->y - wgspr->tex->h;

		node_draw((uint32_t *)wgspr->tex->data + (wgspr->tex->w * (int)rect->y + (int)rect->x)
			, wgspr->tex->w
			, sw
			, sh
			, 0, 0
			, this->bounds.w
			, this->bounds.h);
	}
}

void widget_sprite_update() {
	struct node *this = _current_node;
	struct widget_sprite *wgspr = (struct widget_sprite *)_current_widget;
	

	if(wgspr->reset) {
		wgspr->reset = false;

		//标记整个区域需要重绘
		this->mask |= NMSK_BOUNDS;
	}

	if(wgspr->sheet != NULL) {
		spritesheet_update(wgspr->sheet,G->now);
		rect *current = spritesheet_get_rect(wgspr->sheet);
		if(wgspr->current != current) {
			wgspr->current = current;
			//标记整个区域需要重绘
			this->mask |= NMSK_BOUNDS;
		}
	}

	if(wgspr->sheet != NULL && wgspr->current != NULL) {
		node_size(this,wgspr->current->w, wgspr->current->h);
	} else {
		node_size(this,0,0);
	}
}

struct widgettag _tag_sprite = {
	.tagid = WGTAG_SPRITE
	,.start = widget_sprite_start
	,.draw = widget_sprite_draw
	,.update = widget_sprite_update
	,.destroy = widget_sprite_destroy
};

void spritewidget_install() {
	widgettag_install(&_tag_sprite);
}

struct widget_sprite *spritewidget_addto(struct node *this) {
	struct widget_sprite *wgspr = malloc(sizeof(struct widget_sprite));
	widget_init(&wgspr->super,WGTAG_SPRITE);

	wgspr->tex = NULL;
	wgspr->sheet = NULL;
	wgspr->current = NULL;
	wgspr->reset = true;

	node_addwidget(this,&wgspr->super);
	return wgspr;
}