/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */

#include "widget.h"

void widget_drawable_destroy() {
	//debug("destroy:%d",_current_node->id);
}

void widget_drawable_enable() {
	//debug("enable:%d",_current_node->id);
}

void widget_drawable_disable() {
	//debug("disable:%d",_current_node->id);
}

void widget_drawable_start() {
	//debug("start:%d",_current_node->id);
}

void widget_drawable_draw() {
	struct node *this = _current_node;
	struct widget_drawable *wg = (struct widget_drawable *)_current_widget;
	
	if(wg->src != NULL) {
		if(this->scale.x == 1 && this->scale.y == 1) {
			node_draw(drawable_topleft(wg->src)
				, wg->src->pitch
				, wg->src->w
				, wg->src->h
				, 0, 0
				, this->bounds.w
				, this->bounds.h);
		} else if(this->scale.x == 0 || this->scale.y == 0){
		} else {
			//top left
			node_draw(drawable_topleft(wg->src)
				, wg->src->pitch
				, wg->src->border.left
				, wg->src->border.top
				, 0, 0
				, wg->src->border.left / this->scale.x 
				, wg->src->border.top / this->scale.y );
			//top center
			node_draw(drawable_topcenter(wg->src)
				, wg->src->pitch
				, wg->src->w - wg->src->border.left - wg->src->border.right
				, wg->src->border.top
				, wg->src->border.left / this->scale.x
				, 0
				, this->bounds.w - wg->src->border.left - wg->src->border.right
				, wg->src->border.top / this->scale.y);
			//top right
			node_draw(drawable_topright(wg->src)
				, wg->src->pitch
				, wg->src->border.right
				, wg->src->border.top
				, this->bounds.w - wg->src->border.right / this->scale.x
				, 0
				, wg->src->border.right / this->scale.x
				, wg->src->border.top / this->scale.y);
			//middle left
			node_draw(drawable_middleleft(wg->src)
				, wg->src->pitch
				, wg->src->border.left
				, wg->src->h - wg->src->border.top - wg->src->border.bottom
				, 0
				, wg->src->border.top / this->scale.y
				, wg->src->border.left / this->scale.x
				, this->bounds.h - wg->src->border.top - wg->src->border.bottom);
			//middle right
			node_draw(drawable_middleright(wg->src)
				, wg->src->pitch
				, wg->src->border.right
				, wg->src->h - wg->src->border.top - wg->src->border.bottom
				, wg->src->w - wg->src->border.right
				, wg->src->border.top
				, wg->src->border.right
				, wg->src->h - wg->src->border.top - wg->src->border.bottom);
			//middle center
			node_draw(drawable_middlecenter(wg->src)
				, wg->src->pitch
				, wg->src->w - wg->src->border.left - wg->src->border.right
				, wg->src->h - wg->src->border.top - wg->src->border.bottom
				, wg->src->border.left
				, wg->src->border.top
				, wg->src->w - wg->src->border.left - wg->src->border.right
				, wg->src->h - wg->src->border.top - wg->src->border.bottom);
			//bottom left
			node_draw(drawable_bottomleft(wg->src)
				, wg->src->pitch
				, wg->src->border.left
				, wg->src->border.bottom
				, 0
				, wg->src->h - wg->src->border.bottom
				, wg->src->border.left
				, wg->src->border.bottom);
			//bottom center
			node_draw(drawable_bottomcenter(wg->src)
				, wg->src->pitch
				, wg->src->w - wg->src->border.left - wg->src->border.right
				, wg->src->border.bottom
				, wg->src->border.left
				, wg->src->h - wg->src->border.bottom
				, wg->src->w - wg->src->border.left - wg->src->border.right
				, wg->src->border.bottom);
			//bottom right
			node_draw(drawable_bottomright(wg->src)
				, wg->src->pitch
				, wg->src->border.right
				, wg->src->border.bottom
				, wg->src->w - wg->src->border.right
				, wg->src->h - wg->src->border.bottom
				, wg->src->border.right
				, wg->src->border.bottom);
		}
	}
}

void widget_drawable_update() {
	struct node *this = _current_node;
	struct widget_drawable *wg = (struct widget_drawable *)_current_widget;

	if(wg->reset) {
		wg->reset = false;

		//标记整个区域需要重绘
		this->mask |= NMSK_BOUNDS;
	}

	if(wg->src != NULL) {
		node_size(this,wg->src->w, wg->src->h);
	} else {
		node_size(this,0,0);
	}
}

struct widgettag _tag_drawable = {
	.tagid = WGTAG_DRAWABLE
	,.start = widget_drawable_start
	,.draw = widget_drawable_draw
	,.update = widget_drawable_update
	,.destroy = widget_drawable_destroy
	,.enable = widget_drawable_enable
	,.disable = widget_drawable_disable
};

void drawablewidget_install() {
	widgettag_install(&_tag_drawable);
}

struct widget_drawable *drawablewidget_addto(struct node *this) {
	struct widget_drawable *wg = malloc(sizeof(struct widget_drawable));
	widget_init(&wg->super,WGTAG_DRAWABLE);

	wg->src = NULL;
	wg->reset = true;

	node_addwidget(this,&wg->super);
	return wg;
}