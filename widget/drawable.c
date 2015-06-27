/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "drawable.h"
#include "stb_image.h"


struct drawable *drawable_from(char *path) {

	struct drawable *this = malloc(sizeof(struct drawable));
	this->data = NULL;
	this->w = this->h = 0;
	this->is9png = false;

	edge_empty(&this->border);
	edge_empty(&this->padding);

	int n,w,h;
	uint8_t *data = stbi_load(path, &w, &h, &n, 4);

	int x,y;
	uint8_t *src;
	uint32_t ind;
	uint8_t tmp;
	if(data != NULL) {
		//0xAARRGGBB变成0xAABBGGRR
		src = (uint8_t *)(data);
		for(y = 0;y < h;++y){
			for(x = 0;x < w;++x){
				ind = (x + y * w) * 4;
				tmp = src[ind + 0];
				src[ind + 0] = src[ind + 2];
				src[ind + 2] = tmp;
			}
		}

		this->data = (uint32_t *)data;
		this->pitch = w;
		this->w = w;
		this->h = h;
	}

	int len;
	if((len = strlen(path)) > 6 && !strcmp(path + len - 6, ".9.png")) {
		
		this->is9png = true;

		bool fill = false;

		//border

		for(x = 0;x < w;++x){
			if( ((uint32_t *)this->data)[x]) {
				if(fill == false) {
					fill = true;
					this->border.left = x;
				}
			} else {
				if(fill == true) {
					fill = false;
					this->border.right = w - x;
					break;
				}
			}
		}
		if(fill == true) {
			this->border.right = 0;
		}

		fill = false;
		for(y = 0;y < h;++y){
			if( ((uint32_t *)this->data)[0 + y * w]) {
				if(fill == false) {
					fill = true;
					this->border.top = y;
				}
			} else {
				if(fill == true) {
					fill = false;
					this->border.bottom = h - y;
					break;
				}
			}
		}
		if(fill == true) {
			this->border.bottom = 0;
		}

		//padding

		fill = false;		
		for(x = 0;x < w;++x){
			if( ((uint32_t *)this->data)[x + (h-1) * w]) {
				if(fill == false) {
					fill = true;
					this->padding.left = x;
				}
			} else {
				if(fill == true) {
					fill = false;
					this->padding.right = w - x;
					break;
				}
			}
		}
		if(fill == true) {
			this->padding.right = 0;
		}

		fill = false;
		for(y = 0;y < h;++y){
			if( ((uint32_t *)this->data)[(w - 1) + y * w]) {
				if(fill == false) {
					fill = true;
					this->padding.top = y;
				}
			} else {
				if(fill == true) {
					fill = false;
					this->padding.bottom = h - y;
					break;
				}
			}
		}
		if(fill == true) {
			this->padding.bottom = 0;
		}

		//去掉边缘1个像素
		edge_offset(&this->border,-1);
		edge_offset(&this->padding,-1);
		this->w-=2;
		this->h-=2;
	}

	edge_print(&this->border);
	edge_print(&this->padding);
	return this;
}

uint32_t *drawable_topleft(struct drawable *this) {
	if(this->is9png) {
		return this->data + 1 + 1 * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_topcenter(struct drawable *this) {
	if(this->is9png) {
		return this->data + 1 + (uint32_t)this->border.left + 1 * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_topright(struct drawable *this) {
	if(this->is9png) {
		return this->data + this->w + 2 - (uint32_t)this->border.right - 1 + 1 * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_middleleft(struct drawable *this) {
	if(this->is9png) {
		return this->data + 1 + ((uint32_t)this->border.top + 1) * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_middlecenter(struct drawable *this) {
	if(this->is9png) {
		return this->data + 1 + (uint32_t)this->border.left + ((uint32_t)this->border.top + 1) * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_middleright(struct drawable *this) {
	if(this->is9png) {
		return this->data + this->w + 2 - (uint32_t)this->border.right - 1 + ((uint32_t)this->border.top + 1) * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_bottomleft(struct drawable *this) {
	if(this->is9png) {
		return this->data + 1 + (this->h + 2 - (uint32_t)this->border.bottom - 1) * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_bottomcenter(struct drawable *this) {
	if(this->is9png) {
		return this->data + 1 + (uint32_t)this->border.left + (this->h + 2 - (uint32_t)this->border.bottom - 1) * this->pitch;
	}
	return this->data;
}

uint32_t *drawable_bottomright(struct drawable *this) {
	if(this->is9png) {
		return this->data + this->w + 2 - (uint32_t)this->border.right - 1 + (this->h + 2 - (uint32_t)this->border.bottom - 1) * this->pitch;
	}
	return this->data;
}
