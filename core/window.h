/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>

#ifndef _WINDOW_H
#define _WINDOW_H

void resize_window(int w,int h);
void begin_movewindow();
void minimize_window();
void restore_window();
bool is_windowminimize();

#endif
