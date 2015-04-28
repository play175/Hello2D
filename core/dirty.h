/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>

#ifndef _DIRTY_H
#define _DIRTY_H

void dirty_draw();
void dirty_reset();
int dirty_count();
bool dirty_isalldirty();
void dirty_init(int screen_w, int screen_h, void (*cb)(int x, int y, int w, int h));
void dirty_mark(int x, int y, int w, int h);

#endif
