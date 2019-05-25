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
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


struct texture *texture_from(char *path) {

	struct texture *texture = malloc(sizeof(struct texture));
	texture->data = NULL;
	texture->w = texture->h = 0;

	int n,w,h;
	uint8_t *data = stbi_load(path, &w, &h, &n, 4);

	if(data != NULL) {
		//0xAARRGGBB变成0xAABBGGRR
		int x,y;
		uint8_t *src = (uint8_t *)(data);
		uint32_t ind;
		uint8_t tmp;
		for(y = 0;y < h;y++){
			for(x = 0;x < w;x++){
				ind = (x + y * w) * 4;
				tmp = src[ind + 0];
				src[ind + 0] = src[ind + 2];
				src[ind + 2] = tmp;
			}
		}

		texture->data = (uint32_t *)data;
		texture->w = w;
		texture->h = h;
	}
	return texture;
}