/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */

#include "widget.h"

#include "stb_image.h"



void widget_image_destroy() {
	//debug("destroy:%d",_current_node->id);
}

void widget_image_enable() {
	//debug("enable:%d",_current_node->id);
}

void widget_image_disable() {
	//debug("disable:%d",_current_node->id);
}

void widget_image_start() {
	//debug("start:%d",_current_node->id);
}

void widget_image_draw() {
	struct node *this = _current_node;
	struct widget_image *wgimg = (struct widget_image *)_current_widget;
	
	if(wgimg->tex != NULL) {
		node_draw((uint32_t *)wgimg->tex->data , wgimg->tex->w
			, 0, 0
			, this->bounds.w <= wgimg->tex->w ? this->bounds.w : wgimg->tex->w
			, this->bounds.h <= wgimg->tex->h ? this->bounds.h : wgimg->tex->h);
	}
}

void widget_image_update() {
	struct node *this = _current_node;
	struct widget_image *wgimg = (struct widget_image *)_current_widget;

	if(wgimg->reset) {
		wgimg->reset = false;

		//标记整个区域需要重绘
		this->mask |= NMSK_BOUNDS;
	}

	if(wgimg->tex != NULL) {
		node_size(this,wgimg->tex->w, wgimg->tex->h);
	} else {
		node_size(this,0,0);
	}
}

struct widgettag _tag_image = {
	.tagid = WGTAG_IMAGE
	,.start = widget_image_start
	,.draw = widget_image_draw
	,.update = widget_image_update
	,.destroy = widget_image_destroy
	,.enable = widget_image_enable
	,.disable = widget_image_disable
};

void imagewidget_install() {
	widgettag_install(&_tag_image);
}

struct widget_image *imagewidget_addto(struct node *this) {
	struct widget_image *wgimg = malloc(sizeof(struct widget_image));
	widget_init(&wgimg->super,WGTAG_IMAGE);

	wgimg->tex = NULL;
	wgimg->reset = true;

	node_addwidget(this,&wgimg->super);
	return wgimg;
}