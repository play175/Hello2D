/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "blit.h"
#include "math2d.h"

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )


//如果只有位移，没有缩放和旋转，则调用MMX指令优化
#define TRANSFORM_MMX(BLEND_FUNCTION) \
	int x1,y1,x2,y2; \
	int x,y; \
	int ox,oy; \
	float ofx,ofy; \
	if(m->a == 1.0f && m->b == 0.0f && m->a == m->d && m->b == m->c) { \
		x1 = m->tx + dx;y1 = m->ty + dy; \
		if(x1 >=  dest_width || y1 >=  dest_height) return; \
		ox = 0;oy = 0; \
		x2 = src_width;y2 = src_height; \
		if(x1<0){ox -= x1;x2 += x1;x1 = 0;} \
		if(y1<0){oy -= y1;y2 += y1;y1 = 0;} \
		if(x1 + x2 >  dest_width) x2 = dest_width - x1; \
		if(y1 + y2 >  dest_height) y2 = dest_height - y1; \
		if(x2 <= 0 || y2 <= 0)return; \
		alphablend32(dest + y1 * dest_pitch + x1, dest_pitch \
		, src + oy * src_pitch + ox, src_pitch, x2, y2); \
		return; \
	} \
	float fx,fy; \
	struct mat mm = *m;\
	m = &mm; \
	mat_translate(m,dx,dy); \
	mat_transform_size(m,src_width, src_height, &x1, &y1, &x2, &y2); \
	if(x1<0)x1 = 0;if(x1 > dest_width)x1 = dest_width; \
	if(y1<0)y1 = 0;if(y1 > dest_height)y1 = dest_height; \
	if(x2<0)x2 = 0;if(x2 > dest_width)x2 = dest_width; \
	if(y2<0)y2 = 0;if(y2 > dest_height)y2 = dest_height; \
	if(!(x1<x2 && y1<y2))return; \
	mat_invert(m); \
	int dst_stride = dest_pitch - (x2 - x1); \
	uint32_t *dst_pix = dest + y1 * dest_pitch + x1; \
	mat_transform(m,x1,y1,&fx,&fy); \
	for(y = y1;y<y2;++y,fx += m->c,fy += m->d) { \
		ofx = fx;ofy = fy; \
		for(x = x1;x<x2;++x,ofx += m->a,ofy += m->b) { \
			ox = (int)ofx;\
			oy = (int)ofy;\
			if (oy >= 0 && oy < src_height && ox >= 0 && ox < src_width) { \
				BLEND_FUNCTION(dst_pix,src + oy * src_pitch + ox); \
			} \
			dst_pix++; \
		} \
		dst_pix += dst_stride; \
	}

#define TRANSFORM(BLEND_FUNCTION) \
	int x1,y1,x2,y2; \
	int x,y; \
	int ox,oy; \
	float ofx,ofy; \
	float fx,fy; \
	struct mat mm = *m;\
	m = &mm; \
	mat_translate(m,dx,dy); \
	mat_transform_size(m,src_width, src_height, &x1, &y1, &x2, &y2); \
	if(x1<0)x1 = 0;if(x1 > dest_width)x1 = dest_width; \
	if(y1<0)y1 = 0;if(y1 > dest_height)y1 = dest_height; \
	if(x2<0)x2 = 0;if(x2 > dest_width)x2 = dest_width; \
	if(y2<0)y2 = 0;if(y2 > dest_height)y2 = dest_height; \
	if(!(x1<x2 && y1<y2))return; \
	mat_invert(m); \
	int dst_stride = dest_pitch - (x2 - x1); \
	uint32_t *dst_pix = dest + y1 * dest_pitch + x1; \
	mat_transform(m,x1,y1,&fx,&fy); \
	for(y = y1;y<y2;++y,fx += m->c,fy += m->d) { \
		ofx = fx;ofy = fy; \
		for(x = x1;x<x2;++x,ofx += m->a,ofy += m->b) { \
			ox = (int)ofx;\
			oy = (int)ofy;\
			if (oy >= 0 && oy < src_height && ox >= 0 && ox < src_width) { \
				BLEND_FUNCTION(dst_pix,src + oy * src_pitch + ox); \
			} \
			dst_pix++; \
		} \
		dst_pix += dst_stride; \
	}

void alphablend32(uint32_t *dest_bmp, int dest_pitch, uint32_t *src_bmp, int src_pitch, int blend_width, int blend_height)
{
#ifdef __MINGW32__
        int nextLineOffset_src = (src_pitch - blend_width) * 4;	// 混合完一行像素后，通过加上该值，便可直接定位到下行起始像素
        int nextLineOffset_dst = (dest_pitch - blend_width) * 4;

        asm volatile(
                "xorl %%edx, %%edx;"//已混合的高度
                "movl %2, %%ecx;"//要混合的宽度

                "BLEND_BEGIN:;"
                "cmpl $0x00FFFFFF, (%%esi);"//; 如果alpha为0,则跳过混合部分
                "jna BLEND_END;"//小于等于

                "movd (%%edi), %%mm0;"//		; 把目的像素值移入mm0寄存器的低32位
                "movd (%%esi), %%mm1;"//; 把源像素值移入mm1寄存器的低32位

                //Core Begin
                "pxor %%mm2, %%mm2;" //; 把MM2清0
                "punpcklbw %%mm2, %%mm0;"  //			; src:8 bit到16 bit以容纳结果，32bit expand to 64 bit
                "punpcklbw %%mm2, %%mm1;"  //			; dst:8 bit到16 bit以容纳结果.32bit expand to 64 bit
                "movq %%mm1, %%mm3;"  //			; 因为要用dst的Alpha值
                "punpckhwd	 %%mm3, %%mm3;"  //			; 高字移动到双字
                "punpckhdq	%%mm3, %%mm3;"  //			; 双字移动到四字，现在有八个像素的Alpha了!
                "movq %%mm0, %%mm4;"  //			; mm4 = dst
                "movq %%mm1, %% mm5;"  //			; mm5 = src
                "psubusw %%mm1, %%mm4;"  //			; dst-src，饱和减，小于0为0
                "psubusw %%mm0, %%mm5;"  //			; src-dst，饱和减，小于0为0
                "pmullw %%mm3, %% mm4;"  //			; Alpha * (src-dst)
                "pmullw	 %%mm3, %%mm5;"  //			; Alpha * (dst-src)
                "psrlw $8, %%mm4;"  //				; 除以256，now mm4 get the result，(src-dst)<0 部分
                "psrlw $8, %%mm5;"  //				; 除以256，now mm5 get the result，(dst-src)>0 部分
                "paddusw %%mm5, %%mm0;"  //			; 饱和加到原图象:D=Alpha*(O-S)+S，(src-dst)<0 部分
                "psubusw %%mm4, %% mm0;"  //			; 饱和加到原图象D=S-Alpha*(S-O)，(dst-src)>0 部分
                "packuswb %%mm0, %%mm0;"  //			; 紧缩到低32bit
                //Core End

                "movd %%mm0, (%%edi) ;"  //			; 混合结果写进目的像素

                "BLEND_END:;"
                "addl $4, %%edi;"
                "addl $4, %%esi;"
                "loop BLEND_BEGIN;"//; 循环

                "addl %4, %%esi;"//; 加上偏移量，使定位到下行起始处
                "addl %5, %%edi;"

                "incl %%edx;"
                "movl %2, %%ecx;"

                "cmpl %3, %%edx;"//若edx小于blend_height,则转移到上面继续混合
                "jb BLEND_BEGIN;"

                "EMMS;" //因为从mm0到mm7,这些寄存器是“借用”浮点寄存器的低64位,所以每次在用完MMX指令后一定要用EMMS指令将寄存器清空

        :
        :"S"(src_bmp), "D"(dest_bmp), "m"(blend_width), "m"(blend_height), "m"(nextLineOffset_src), "m"(nextLineOffset_dst)
        );
#else

        // C实现
        int nextLineOffset_src = (src_pitch - blend_width);
        int nextLineOffset_dst = (dest_pitch - blend_width);
        int h,w;

        uint8_t a1,r1,g1,b1;
        uint8_t r2,g2,b2;

        for (h=0, w=0; h<blend_height; h++) {
                for (w=0; w<blend_width; w++) {
                        {
                                b1 = (*src_bmp) & 0x000000ff;
                                g1 = ((*src_bmp) & 0x0000ff00)>>8;
                                r1 = ((*src_bmp) & 0x00ff0000)>>16;
                                a1 = ((*src_bmp) & 0xff000000)>>24;

                                b2 = (*dest_bmp) & 0x000000ff;
                                g2 = ((*dest_bmp) & 0x0000ff00)>>8;
                                r2 = ((*dest_bmp) & 0x00ff0000)>>16;
                                //>>8是近似值，实际应该是除以255
                                *dest_bmp = ((a1 * (r1 - r2) >> 8) + r2) << 16 | ((a1 * (g1 - g2) >> 8) + g2) << 8 | ((a1 * (b1 - b2) >> 8) + b2) << 0 ;
                        }

                        src_bmp++;
                        dest_bmp++;
                }

                src_bmp += nextLineOffset_src;
                dest_bmp += nextLineOffset_dst;
        }
#endif
}

static inline void alpha_blend_replace(uint32_t *dest_bmp,uint32_t *src_bmp)
{
        *dest_bmp = *src_bmp;
}

//#define RMask 0x00ff0000
//#define GMask 0x0000ff00
//#define BMask 0x000000ff

static inline void alpha_blend(uint32_t *dest_bmp,uint32_t *src_bmp)
{
	uint8_t a1,r1,g1,b1;
	uint8_t r2,g2,b2;
	a1 = ((*src_bmp) & 0xff000000)>>24;
	if (a1 == 255) {
			*dest_bmp = *src_bmp;
			return;
	}
	if (a1 == 0) {
			return;
	}
	b1 = (*src_bmp) & 0x000000ff;
	g1 = ((*src_bmp) & 0x0000ff00)>>8;
	r1 = ((*src_bmp) & 0x00ff0000)>>16;

	b2 = (*dest_bmp) & 0x000000ff;
	g2 = ((*dest_bmp) & 0x0000ff00)>>8;
	r2 = ((*dest_bmp) & 0x00ff0000)>>16;

	*dest_bmp = a1 << 24 | ((a1 * (r1 - r2) >> 8) + r2) << 16 | ((a1 * (g1 - g2) >> 8) + g2) << 8 | ((a1 * (b1 - b2) >> 8) + b2);

}

static inline void alpha_blend_add(uint32_t *dest_bmp,uint32_t *src_bmp)
{
	uint8_t a1,r1,g1,b1;
	uint8_t r2,g2,b2;
	b1 = (*src_bmp) & 0x000000ff;
	g1 = ((*src_bmp) & 0x0000ff00)>>8;
	r1 = ((*src_bmp) & 0x00ff0000)>>16;
	a1 = ((*src_bmp) & 0xff000000)>>24;

	b2 = (*dest_bmp) & 0x000000ff;
	g2 = ((*dest_bmp) & 0x0000ff00)>>8;
	r2 = ((*dest_bmp) & 0x00ff0000)>>16;

	b2 = min(255, max(0, (b2 + 2 * b1) - 1));
	g2 = min(255, max(0, (g2 + 2 * g1) - 1));
	r2 = min(255, max(0, (r2 + 2 * r1) - 1));

	*dest_bmp = a1 << 24 | r2 << 16 | g2 << 8 | b2;
}

static inline void alpha_blend_overly(uint32_t *dest_bmp,uint32_t *src_bmp)
{
	uint8_t a1,r1,g1,b1;
	uint8_t r2,g2,b2;
	b1 = (*src_bmp) & 0x000000ff;
	g1 = ((*src_bmp) & 0x0000ff00)>>8;
	r1 = ((*src_bmp) & 0x00ff0000)>>16;
	a1 = ((*src_bmp) & 0xff000000)>>24;

	b2 = (*dest_bmp) & 0x000000ff;
	g2 = ((*dest_bmp) & 0x0000ff00)>>8;
	r2 = ((*dest_bmp) & 0x00ff0000)>>16;


	b2 = (b2 < 128) ? (2 * b1 * b2 / 255) : (255 - 2 * (255 - b1) * (255 - b2) / 255);
	g2 = (g2 < 128) ? (2 * g1 * g2 / 255) : (255 - 2 * (255 - g1) * (255 - g2) / 255);
	r2 = (r2 < 128) ? (2 * r1 * r2 / 255) : (255 - 2 * (255 - r1) * (255 - r2) / 255);

	*dest_bmp = a1 << 24 | r2 << 16 | g2 << 8 | b2;
}

static inline void alpha_blend_lighten(uint32_t *dest_bmp,uint32_t *src_bmp)
{
	uint8_t a1,r1,g1,b1;
	uint8_t r2,g2,b2;
	b1 = (*src_bmp) & 0x000000ff;
	g1 = ((*src_bmp) & 0x0000ff00)>>8;
	r1 = ((*src_bmp) & 0x00ff0000)>>16;
	a1 = ((*src_bmp) & 0xff000000)>>24;

	b2 = (*dest_bmp) & 0x000000ff;
	g2 = ((*dest_bmp) & 0x0000ff00)>>8;
	r2 = ((*dest_bmp) & 0x00ff0000)>>16;

	b2 = b2 > b1 ? b2 : b1;
	g2 = g2 > g1 ? g2 : g1;
	r2 = r2 > r1 ? r2 : r1;

	*dest_bmp = a1 << 24 | r2 << 16 | g2 << 8 | b2;
}

void transform_blend_replace(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
               , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m)
{
        TRANSFORM(alpha_blend_replace)
}

void transform_blend(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                     , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m)
{
        TRANSFORM_MMX(alpha_blend)
}

void transform_blend_add(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                         , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m)
{

        TRANSFORM(alpha_blend_add)
}

void transform_blend_lighten(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                             , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m)
{

        TRANSFORM(alpha_blend_lighten)
}

void transform_blend_overly(int dx,int dy,uint32_t *dest,int dest_pitch,int dest_width,int dest_height
                            , uint32_t *src,int src_pitch,int src_width,int src_height, struct mat *m)
{

        TRANSFORM(alpha_blend_overly)
}