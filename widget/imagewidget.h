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

#ifndef _IMAGEWIDGET_H
#define _IMAGEWIDGET_H

struct widget_image {
	struct widget super;
	struct texture *tex;
	bool reset;
};

void imagewidget_install();
struct widget_image *imagewidget_addto(struct node *this);

#endif