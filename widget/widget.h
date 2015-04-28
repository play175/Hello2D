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
#include "h2d.h"

#ifndef _WIDGET_H
#define _WIDGET_H

enum wgtag {
	WGTAG_MIN = 0,
	WGTAG_IMAGE,
	WGTAG_SPRITE,
	WGTAG_TEST,
	WGTAG_MAX
};


#include "imagewidget.h"
#include "spritewidget.h"
#include "texture.h"
#include "spritesheet.h"

static inline void widget_installall() {
	imagewidget_install();//image组件初始化
	spritewidget_install();//sprite组件初始化
}

#endif
