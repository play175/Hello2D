/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>

#ifndef _TEXTURE_H
#define _TEXTURE_H


struct texture {
	uint32_t *data;
	int w,h;
};

struct texture *texture_from(char *path);

#endif
