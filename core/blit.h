/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#ifndef BLIT_H
#define BLIT_H

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "math2d.h"


static inline void memset32(void *dest, uint32_t data, int count) {
#ifdef __MINGW32__
	asm volatile(
	    "cld;"
	    "rep stosl;"//stosl指令为从eax保存四字节到edi
    :
    :"D"(dest),"c"(count),"a"(data)
	);
#else
	while (count--) {
		*dest = data;
		((uint32_t *)dest)++;
	}
#endif
}

void alphablend32(uint32_t *dest_bmp, int dst_width, uint32_t *src_bmp, int src_width, int blend_width, int blend_height);
void zoom(uint32_t *dest,int dw,int dh,int dst_width,uint32_t *src,int sw,int sh,int src_width);
void transform_blend_replace(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
               , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *mat);

void transform_blend(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                     , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m);
void transform_blend_add(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                         , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m);
void transform_blend_lighten(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                             , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m);
void transform_blend_overly(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                            , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m);

#endif