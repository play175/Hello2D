/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef _MATRIX_H
#define _MATRIX_H

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

struct vec {
	float x,y;
};

struct rect {
	float x,y,w,h;
};

struct edge {
	float left,top,right,bottom;
};

struct frame {
	struct vec orign;
	struct rect rect;
};

/*******
*|a b tx|
*|c d ty|
*|u v w|
*******/
//row-major order
struct mat {
	float a,b,tx,c,d,ty;
};

typedef struct vec vec;
typedef struct mat mat;
typedef struct rect rect;

//vec

static inline struct vec vec_zero() {
	struct vec v;
	v.x = 0;
	v.y = 0;
	return v;
}

static inline struct vec vec_one() {
	struct vec v;
	v.x = 1;
	v.y = 1;
	return v;
}

static inline struct vec vecf(float x, float y) {
	struct vec v;
	v.x = x;
	v.y = y;
	return v;
}

static inline void vec_set(struct vec *this,float x,float y) {
	this->x = x;
	this->y = y;
}

static inline float vec_length(struct vec *this) {
	return sqrt(this->x * this->x + this->y * this->y);
}

static inline void vec_offset(struct vec *this,float dx,float dy) {
	this->x += dx;
	this->y += dy;
}

static inline void vec_clone(struct vec *this,struct vec *p) {
	memcpy(p,this,sizeof(*p));
}

static inline bool vec_equals(struct vec *this,struct vec *p) {
	return this->x == p->x && this->y == p->y;
}

static inline void vec_sub(struct vec *this,struct vec *p,struct vec *out) {
	float x = this->x - p->x;
	float y = this->y - p->y;
	vec_set(out,x,y);
}

static inline float vec_distance(struct vec *p1,struct vec *p2) {
	struct vec p;
	vec_sub(p1,p2,&p);
	return vec_length(&p);
}

static inline void vec_interpolate(struct vec *p1,struct vec *p2,struct vec *out,float f) {
	float x = p1->x + (f * (p1->x - p2->x));
	float y = p1->y + (f * (p1->y - p2->y));
	vec_set(out,x,y);
}

static inline void vec_add(struct vec *this,struct vec *p,struct vec *out) {
	float x = this->x + p->x;
	float y = this->y + p->y;
	vec_set(out,x,y);
}

//缩放到指定长度
static inline void vec_normalize(struct vec *this,float thickness) {
	float invD = vec_length(this);
	if (invD > 0) {
		invD = (thickness / invD);
		this->x = (this->x * invD);
		this->y = (this->y * invD);
	}
}

//rect

static inline struct rect rectf(float x, float y, float w, float h) {
	struct rect this;
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
	return this;
}

static inline void rect_print(struct rect *this) {
	printf("rect{x=%f,y=%f,w=%f,h=%f}\n",this->x,this->y,this->w,this->h);
}

static inline bool rect_isempty(struct rect *this) {
	return this->w == 0 && this->h == 0;
}

static inline void rect_empty(struct rect *this) {
	this->x = 0;
	this->y = 0;
	this->w = 0;
	this->h = 0;
}

static inline void rect_union(struct rect *this,struct rect *to_union,struct rect *out) {
	if (rect_isempty(this)) {
		*out = *to_union;
		return;
	}
	if (rect_isempty(to_union)) {
		*out = *this;
		return;
	}
	//不能直接操作out，因为可能out==this
	struct rect rt;
	rt.x = this->x < to_union->x ? this->x : to_union->x;
	rt.y = this->y < to_union->y ? this->y : to_union->y;
	float v1,v2;
	v1 = this->x + this->w;
	v2 = to_union->x + to_union->w;
	rt.w = (v1 > v2 ? v2 : v1) - rt.x;
	v1 = this->y + this->h;
	v2 = to_union->y + to_union->h;
	rt.h = (v1 > v2 ? v2 : v1) - rt.y;
	*out = rt;
}

static inline bool rect_contain(struct rect *this,struct rect *in) {
	return (this->x <= in->x && this->y <= in->y
	        && (this->x + this->w) >= (in->x + in->w)
	        && (this->y + this->h) >= (in->y + in->h));
}

static inline bool rect_containpoint(struct rect *this,float x,float y) {
	return (this->x <= x && this->y <= y
	        && (this->x + this->w) > x
	        && (this->y + this->h) > y);
}

//mat3x3

static inline void mat_mul(struct mat *this,struct mat *mat) {
	float result_a;
	float result_b;
	float result_c;
	float result_d;
	float result_tx;
	float result_ty;
	result_a = (this->a * mat->a);
	result_d = (this->d * mat->d);
	result_c = 0;
	result_b = result_c;
	result_tx = ((this->tx * mat->a) + mat->tx);
	result_ty = ((this->ty * mat->d) + mat->ty);
	if (((((((!((this->b == 0))) || (!((this->c == 0))))) || (!((mat->b == 0))))) || (!((mat->c == 0))))) {
		result_a = (result_a + (this->b * mat->c));
		result_d = (result_d + (this->c * mat->b));
		result_b = (result_b + ((this->a * mat->b) + (this->b * mat->d)));
		result_c = (result_c + ((this->c * mat->a) + (this->d * mat->c)));
		result_tx = (result_tx + (this->ty * mat->c));
		result_ty = (result_ty + (this->tx * mat->b));
	}
	this->a = result_a;
	this->b = result_b;
	this->c = result_c;
	this->d = result_d;
	this->tx = result_tx;
	this->ty = result_ty;
}

//世界坐标位移
static inline void mat_translate(struct mat *this,float dx,float dy) {
	this->tx += dx;
	this->ty += dy;
}

//不考虑位移的变换
static inline void mat_delta_transform(struct mat *this,float x,float y,float *out_x,float *out_y) {
	*out_x = (this->a * x) + (this->c * y);
	*out_y = (this->d * y) + (this->b * x);
}

static inline void mat_transform(struct mat *this,float x,float y,float *out_x,float *out_y) {
	*out_x = (this->a * x) + (this->c * y) + this->tx;
	*out_y = (this->d * y) + (this->b * x) + this->ty;
}

static inline void mat_print(struct mat *this) {
	printf("\n%f\t%f\t%f\n%f\t%f\t%f\n",this->a,this->b,this->tx,this->c,this->d,this->ty);
}

static inline void mat_identity(struct mat *this) {
	this->a = 1.0f;
	this->b = 0.0f;
	this->c = 0.0f;
	this->d = 1.0f;
	this->tx = 0.0f;
	this->ty = 0.0f;
}

static inline void mat_set(struct mat *this,float a,float b,float c,float d,float tx,float ty) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
	this->tx = tx;
	this->ty = ty;
}

static inline void mat_invert(struct mat *this) {
	float a0,a1,a2,a3;
	double det;
	float vec_x,vec_y;
	if ((((this->b == 0.0f)) && ((this->c == 0.0f)))) {
		this->a = (1.0f / this->a);
		this->d = (1.0f / this->d);
		this->b = (this->c = 0.0f);
		this->tx = (-(this->a) * this->tx);
		this->ty = (-(this->d) * this->ty);
	} else {
		a0 = this->a;
		a1 = this->b;
		a2 = this->c;
		a3 = this->d;
		det = ((a0 * a3) - (a1 * a2));
		if (det == 0) {
			mat_identity(this);
			return;
		};
		det = (1 / det);
		this->a = (a3 * det);
		this->b = (-(a1) * det);
		this->c = (-(a2) * det);
		this->d = (a0 * det);
		mat_delta_transform(this,this->tx, this->ty,&vec_x,&vec_y);
		this->tx = -(vec_x);
		this->ty = -(vec_y);
	}
}

static inline void mat_setrotate(struct mat *this,float angle) {
	angle = angle * M_PI / 180.0f;
	float a = cos(angle);
	float b = sin(angle);
	mat_set(this,a, b, -(b), a, 0, 0);
}

//设置按角度angle沿原点旋转,angle是角度不是弧度
static inline void mat_rotate(struct mat *this,float angle) {
	angle = angle * M_PI / 180.0f;
	float a = cos(angle);
	float b = sin(angle);
	struct mat m;
	mat_set(&m,a, b, -(b), a, 0, 0);
	mat_mul(this,&m);
}

//设置按角度angle沿中心点center_x, center_y旋转
static inline void mat_rotate_at(struct mat *this,float angle,float center_x,float center_y) {
	mat_translate(this,center_x, center_y);
	mat_rotate(this,angle);
	mat_translate(this,-center_x, -center_y);
}

//设置斜切，注意不要将shear_x, shear_y同时设置为1
static inline void mat_shear(struct mat *this,float shear_x, float shear_y) {
	struct mat m;
	mat_identity(&m);
	m.c = shear_x;
	m.b = shear_y;
	mat_mul(this,&m);
}

static inline void mat_scale(struct mat *this,float sx,float sy) {
	struct mat m;
	mat_identity(&m);
	m.a = sx;
	m.d = sy;
	mat_mul(this,&m);
}

//本地位移
static inline void mat_offset(struct mat *this,float dx,float dy) {
	mat_invert(this);
	mat_translate(this,-dx,-dy);
	mat_invert(this);
}

static inline void mat_transform_rect(struct mat *this,int w, int h, struct rect *rect) {
	float fx,fy;
	float left,top,right,bottom;
	mat_transform(this,0.0f,0.0f,&fx,&fy);
	right = fx;
	bottom = fy;
	left = fx;
	top = fy;
	mat_transform(this,w,0.0f,&fx,&fy);
	if (left>fx)left = fx;
	if (top>fy)top = fy;
	if (right<fx)right = fx;
	if (bottom<fy)bottom = fy;
	mat_transform(this,w,h,&fx,&fy);
	if (left>fx)left = fx;
	if (top>fy)top = fy;
	if (right<fx)right = fx;
	if (bottom<fy)bottom = fy;
	mat_transform(this,0.0f,h,&fx,&fy);
	if (left>fx)left = fx;
	if (top>fy)top = fy;
	if (right<fx)right = fx;
	if (bottom<fy)bottom = fy;

	rect->x = left;
	rect->y = top;
	rect->w = right - left;
	rect->h = bottom - top;
}

// 按给定的大小计算并返回实施变换后的尺寸
static inline void mat_transform_size(struct mat *this,int w, int h, int *x1, int *y1, int *x2, int *y2) {
	float fx,fy;
	float left,top,right,bottom;
	mat_transform(this,0.0f,0.0f,&fx,&fy);//left top
	right = fx;
	bottom = fy;
	left = fx;
	top = fy;
	mat_transform(this,w,0.0f,&fx,&fy);//right top
	if (left>fx)left = fx;
	if (top>fy)top = fy;
	if (right<fx)right = fx;
	if (bottom<fy)bottom = fy;
	mat_transform(this,w,h,&fx,&fy);//right bottom
	if (left>fx)left = fx;
	if (top>fy)top = fy;
	if (right<fx)right = fx;
	if (bottom<fy)bottom = fy;
	mat_transform(this,0.0f,h,&fx,&fy);//left bottom
	if (left>fx)left = fx;
	if (top>fy)top = fy;
	if (right<fx)right = fx;
	if (bottom<fy)bottom = fy;

	*x1 = (int)(left);
	*y1 = (int)(top);
	*x2 = (int)(right);
	*y2 = (int)(bottom);
}

static inline void mat_create_box(struct mat *this,float sx, float sy, float rotation, float dx, float dy) {
	mat_identity(this);
	mat_rotate(this,rotation);
	mat_scale(this,sx,sy);
	this->tx = dx;
	this->ty = dy;
}

static inline void mat_create_gradient_box(struct mat *this,float w, float h, float rotation, float dx, float dy) {
	mat_create_box(this,(w / 1638.4f), (h / 1638.4f), rotation, (dx + (w / 2.0f)), (dy + (h / 2.0f)));
}
#endif
