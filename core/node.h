/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>
#include "math2d.h"
#include "game.h"

#ifndef _NODE_H
#define _NODE_H

#define NMSK_MAT			1 << 0
#define NMSK_DISABLE		1 << 1
#define NMSK_BOUNDS		1 << 2

struct node;
struct widget;
struct widgettag;

struct widgettag {
	int tagid;
	void (*awake)();
	void (*enable)();
	void (*start)();
	void (*draw)();
	void (*update)();
	void (*disable)();
	void (*destroy)();
	bool (*hittest)(float x,float y,bool *cancel);
};

struct widget {
	struct widget *next;

	struct node *node;
	bool disable;
	bool awaked;
	bool started;

	int tagid;
};

struct node {
	struct node *parent;
	struct node *firstchild;
	struct node *prev;
	struct node *next;
	int numchildren;

	int id;

	struct mat mat;
	struct mat worldmat;

	bool disable;//隐藏
	vec pos;
	vec scale;
	rect bounds;//注意：x,y 在0 - 1 范围内
	rect tarbounds;//注意：x,y 在0 - 1 范围内
	float rot;

	uint8_t mask;

	struct widget *widgets;
};

struct drawcontext {
	void *bits;
	int pitch;
	float w,h;
	int dx,dy;
};

#define node_foreach(i, list) int __lsi = 0;\
    for (i = list ->firstchild; ((__lsi != 0 && i != (list ->firstchild)) || (__lsi == 0 && list ->firstchild != NULL)); i = i->next,__lsi++)
#define node_foreach_r(i, list) int __lsi = 0;\
    for (i = list ->firstchild ? list ->firstchild->prev : NULL; ((__lsi != 0 && i != list ->firstchild->prev) || (__lsi == 0 && list ->firstchild != NULL)); i = i->prev,__lsi++)


struct node *_current_node;//current node
struct widget *_current_widget;//current widget
struct widgettag *_current_widgettag;//current widgettag
struct drawcontext *_current_drawcontext;

struct node *node_new();
void node_init(struct node *this);
void node_addchild(struct node *this,struct node *child);
void node_addchild_at(struct node *this,struct node *child,int index);
struct node *node_getchild_at(struct node *this,int index);
void node_removechild_at(struct node *this,int index);
void node_removeself(struct node *child);
void node_update(struct node *this);
void node_renderall(struct node *root);
void node_markdirty(struct node *this);
void node_redraw(bool show);
bool node_hittest(struct node *this,float stage_x,float stage_y,float *local_x,float *local_y,struct node **out);

struct widget * node_getwidget(struct node *this,int tagid);
void node_addwidget(struct node *this,struct widget *widget);
void node_removewidget(struct node *this,struct widget *widget);
void widget_init(struct widget *this,int tagid);
void widget_destroy(struct widget *widget);

void widgettag_init(struct widgettag *tag,int tagid);
void widgettag_install(struct widgettag *tag);

//设置本地包围盒,所有绘制都必须在此区域内
static inline void node_bounds(struct node *this,rect *bounds) {
	if(this->bounds.x != bounds->x || this->bounds.y != bounds->y
		|| this->bounds.w != bounds->w || this->bounds.h != bounds->h) {
		node_markdirty(this);
		this->tarbounds = *bounds;
		this->mask |= NMSK_BOUNDS;
	}else {
		this->mask ^= NMSK_BOUNDS;
	}
}

static inline void node_size(struct node *this, float w, float h) {
	rect *b = &this->bounds;
	if(this->mask & NMSK_BOUNDS) { //之前已经改变，但还没有执行
		b = &this->tarbounds;
	}
	if(b->w != w || b->h != h) {
		rect bounds = rectf(b->x, b->y, w, h);
		node_bounds(this,&bounds);
	}
}

static inline void node_pivot(struct node *this, float x, float y) {
	rect *b = &this->bounds;
	if(this->mask & NMSK_BOUNDS) { //之前已经改变，但还没有执行
		b = &this->tarbounds;
	}
	if(b->x != x || b->y != y) {
		rect bounds = rectf(x, y, b->w, b->h);
		node_bounds(this,&bounds);
	}
}

static inline void node_pos(struct node *this, float x, float y) {
	if(this->pos.x == x && this->pos.y == y) {
		this->mask ^= NMSK_MAT;
		return;
	}

	this->pos.x = x;
	this->pos.y = y;

	this->mask = this->mask | NMSK_MAT;
}

static inline void node_x(struct node *this, float x) {
	if(this->pos.x == x)return;
	node_pos(this, x, this->pos.y);
}

static inline void node_y(struct node *this, float y) {
	if(this->pos.y == y)return;
	node_pos(this, this->pos.x, y);
}

static inline void node_rot(struct node *this, float rot) {
	this->rot += rot;
	this->mask = this->mask | NMSK_MAT;
}

static inline void node_rotto(struct node *this, float rot) {
	if(this->rot == rot)return;
	this->rot = rot;
	this->mask = this->mask | NMSK_MAT;
}

static inline void node_scale(struct node *this, float x, float y) {
	if(this->scale.x == x && this->scale.y == y)return;
	this->scale.x = x;
	this->scale.y = y;
	this->mask = this->mask | NMSK_MAT;
}

static inline void node_scalexy(struct node *this, float xy) {
	node_scale(this,xy,xy);
}

static inline void node_scalex(struct node *this, float x) {
	if(this->scale.x == x)return;
	node_scale(this, x, this->scale.y);
}

static inline void node_scaley(struct node *this, float y) {
	if(this->scale.y == y)return;
	node_scale(this, this->scale.x, y);
}

static inline void node_updatelocalmat(struct node *this) {
	//update local matrix
	//注意:缩放、旋转、平移 顺序不能乱
	mat_identity(&this->mat);
	mat_translate(&this->mat, -this->bounds.x * this->bounds.w, -this->bounds.y * this->bounds.h);
	mat_scale(&this->mat, this->scale.x, this->scale.y);
	mat_rotate(&this->mat, this->rot);
	mat_translate(&this->mat, this->pos.x, this->pos.y);
}

static inline void node_updateworldmat(struct node *this) {
	//update world matrix
	mat_identity(&this->worldmat);
	mat_mul(&this->worldmat,&this->mat);
	if(this->parent) {
		mat_mul(&this->worldmat,&this->parent->worldmat);
	}
}

static inline void node_draw(uint32_t *bits, int pitch, float bitsw, float bitsh, float x, float y, float w, float h) {
	if(_current_drawcontext == NULL || _current_node == NULL)return;
	if(bitsw <= 0 || bitsh <= 0 || w <= 0 || h <= 0)return;

	mat *m;
	if (x != 0 || y != 0 || bitsw != w || bitsh != h) {
		mat mt;
		mat_identity(&mt);
		mat_translate(&mt,-_current_node->bounds.x * bitsw + x
			, -_current_node->bounds.y * bitsh + y);
		mat_scale(&mt, _current_node->scale.x * w / bitsw, _current_node->scale.y * h / bitsh);
		mat_rotate(&mt, _current_node->rot);
		mat_translate(&mt, _current_node->pos.x, _current_node->pos.y);
		if(_current_node->parent) {
			mat_mul(&mt, &_current_node->parent->worldmat);
		}
		m = &mt;
	} else {
		m = &_current_node->worldmat;
	}

	game_blend(BLEND_NORMAL
		, _current_drawcontext->dx, _current_drawcontext->dy, _current_drawcontext->bits
		, _current_drawcontext->pitch, _current_drawcontext->w, _current_drawcontext->h
		, bits, pitch, bitsw, bitsh
		, m);
}

#endif
