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
#include "drawable.h"

#ifndef _DRAWABLEWIDGET_H
#define _DRAWABLEWIDGET_H

struct widget_drawable {
	struct widget super;
	struct drawable *src;
	bool reset;
};

void drawablewidget_install();
struct widget_drawable *drawablewidget_addto(struct node *this);

#endif