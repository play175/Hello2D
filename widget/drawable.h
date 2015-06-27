/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>

#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include "math2d.h"

struct drawable {
	uint32_t *data;
	int pitch;
	int w,h;
	bool is9png;
	struct edge border;
	struct edge padding;
};

struct drawable *drawable_from(char *path);

uint32_t *drawable_topleft(struct drawable *this);
uint32_t *drawable_topcenter(struct drawable *this);
uint32_t *drawable_topright(struct drawable *this);
uint32_t *drawable_middleleft(struct drawable *this);
uint32_t *drawable_middlecenter(struct drawable *this);
uint32_t *drawable_middleright(struct drawable *this);
uint32_t *drawable_bottomleft(struct drawable *this);
uint32_t *drawable_bottomcenter(struct drawable *this);
uint32_t *drawable_bottomright(struct drawable *this);

#endif
