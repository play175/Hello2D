/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef _H2D_H
#define _H2D_H

#define TOUCH_BEGIN 0
#define TOUCH_END 1
#define TOUCH_MOVE 2
#define TOUCH_OVER 3
#define TOUCH_OUT 4

#define WINDOWNAME L"Hello2D"

#ifdef NDEBUG
#define debug(fmt, ...)	((void)0)
#define error(fmt, ...)	((void)0)
#else
#define debug(fmt, ...)                    \
					printf("[DEBUG] %s:%d <%s>: " fmt "\n", \
								 __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define error(fmt, ...)                    \
					printf("[ERROR] %s:%d <%s>: " fmt "\n", \
								 __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif

#include "game.h"
#include "window.h"
#include "node.h"
#include "dirty.h"
#include "widget.h"

#endif
