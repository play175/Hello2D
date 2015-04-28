/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
 #include <stdint.h>
#include <stdbool.h>

#include "widget.h"
#include "texture.h"
#include "spritesheet.h"

#ifndef _SPRITEWIDGET_H
#define _SPRITEWIDGET_H

struct widget_sprite {
	struct widget super;

	struct texture *tex;
	struct spritesheet *sheet;
	rect *current;
	bool reset;
};

void spritewidget_install();
struct widget_sprite *spritewidget_addto(struct node *this);

#endif