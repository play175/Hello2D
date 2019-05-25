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
#include <assert.h>
#include "node.h"
#include "dirty.h"
#include "game.h"

struct node *_current_node = NULL;//current node
struct widget *_current_widget = NULL;//current widget
struct widgettag *_current_widgettag = NULL;//current widgettag
struct drawcontext *_current_drawcontext = NULL;

static struct widgettag *tagstore[255];

static int __TAGID__ = 0;
static int __NODEID__ = 0;
static bool _showredraw = false;
static struct drawcontext _rootcontext = {0};
static struct node *_root = NULL;

void node_render_rect(int left, int top, int right, int bottom);

struct node *node_new() {
	struct node *this = malloc(sizeof(struct node));
	node_init(this);
	return this;
}

void node_init(struct node *this) {
	this->parent = NULL;
	this->firstchild = NULL;
	this->prev = NULL;
	this->next = NULL;
	this->numchildren = 0;

	this->id = ++__NODEID__;

	mat_identity(&this->mat);
	mat_identity(&this->worldmat);
	
	this->mask |= NMSK_MAT;

	this->disable = false;
	this->mask = 0;

	rect_empty(&this->bounds);
	rect_empty(&this->tarbounds);

	this->pos = vec_zero();
	this->scale = vec_one();
	rect_empty(&this->bounds);
	this->rot = 0.f;

	this->widgets = NULL;

	if(__NODEID__ == 1) {
		_rootcontext.bits = G->bits;
		_rootcontext.pitch = G->w;
		_rootcontext.w = G->w;
		_rootcontext.h = G->h;

		dirty_init(G->w,G->h,node_render_rect);
		dirty_mark(0, 0, G->w, G->h);
	}
}


void node_resizecanvas() {
	_rootcontext.bits = G->bits;
	_rootcontext.pitch = G->w;
	_rootcontext.w = G->w;
	_rootcontext.h = G->h;

	dirty_init(G->w,G->h,node_render_rect);
	dirty_mark(0, 0, G->w, G->h);
}

struct node *node_getchild_at(struct node *this,int index) {
	if (this->numchildren == 0) return NULL;
	if (index < 0) {
		//-1是最后一个
		index = this->numchildren + index;
	}
	if (index < 0) return NULL;
	if (index >= this->numchildren) return NULL;
	struct node *i = NULL;
	int n = 0;
	node_foreach(i, this) {
		if (index == n)return i;
		++n;
	}
	return NULL;
}

void node_removechild_at(struct node *this,int index) {
	struct node *child = node_getchild_at(this,index);
	if (child != NULL)node_removeself(child);
}

void node_addchild(struct node *this,struct node *child) {
	node_addchild_at(this,child,-1);
}

void node_addchild_at(struct node *this,struct node *child,int index) {
	if (child->parent != NULL)node_removeself(child);

	if (this->numchildren == 0) {
		this->firstchild = child;
		child->prev = child->next = child;
	} else {
		if (index < 0) {
			//-1是最后一个
			index = this->numchildren + index + 1;
		}
		if (index < 0) index = 0;
		if (index > this->numchildren) index = this->numchildren;
		struct node *i = NULL;
		int n = 0;
		node_foreach(i, this) {
			if (index == n)break;
			++n;
		}
		assert(i != NULL);
		child->prev = i->prev;
		child->next = i;
		i->prev->next = child;
		i->prev = child;
		if (index == 0)this->firstchild = child;
	}

	++this->numchildren;
	child->parent = this;
}

void node_removeself(struct node *child) {
	if (child->parent == NULL)return;

	if (child->parent->firstchild == child) {
		if (child->parent->numchildren == 1)
			child->parent->firstchild = NULL;
		else
			child->parent->firstchild = child->next;
	}

	child->prev->next = child->next;
	child->next->prev = child->prev;

	--child->parent->numchildren;
	child->parent = NULL;
}

void node_render(struct node *this,struct drawcontext *context) {
	if (this->disable)return;

	_current_drawcontext = context;
	_current_node = this;

	struct widget *wg = this->widgets;
	while (wg != NULL) {
		struct widgettag *tag = (tagstore[wg->tagid]);

		_current_widget = wg;
		_current_widgettag = tag;

		if (wg->started == false) {
			wg->started = true;
			if (tag->start != NULL)tag->start();
		}
		if (tag->draw != NULL)tag->draw();

		wg = wg->next;
	}

	_current_drawcontext = NULL;

	struct node *i;
	node_foreach (i, this) {
		node_render(i,context);
	}
}

//所有物体按脏矩形区域为画布渲染
void node_render_rect(int left, int top, int right, int bottom) {
	if(left < 0)left = 0;if(top < 0)top = 0;
	if(right > G->w) right = G->w;
	if(bottom > G->h) bottom = G->h;

	int w = right - left,h = bottom - top;
	if(w<=0 || h <=0)return;

	//debug("%d,%d,%d,%d",left,top,w,h);

	uint32_t *dest = ((uint32_t *)G->bits) + left + top * _rootcontext.pitch;

	game_fillrect(dest,_rootcontext.pitch,w,h,G->bgcolor);

	_rootcontext.bits = dest;
	_rootcontext.w = w;
	_rootcontext.h = h;
	_rootcontext.dx = 0 - left;
	_rootcontext.dy = 0 - top;
	if(_root != NULL)node_render(_root,&_rootcontext);

	if (_showredraw) {
		int i;
		for(i=left;i<right;++i) {
			((uint32_t *)G->bits)[i + top * _rootcontext.pitch] = 0xffff0000;
			((uint32_t *)G->bits)[i + (bottom - 1) * _rootcontext.pitch] = 0xffff0000;
		}
		for(i=top;i<bottom;++i) {
			((uint32_t *)G->bits)[left + i * _rootcontext.pitch] = 0xffff0000;
			((uint32_t *)G->bits)[(right - 1) + i * _rootcontext.pitch] = 0xffff0000;
		}
	}
}

//把物体当前区域标记为脏
void node_markdirty(struct node *this) {
	int left,top,right,bottom;
	mat_transform_size(&this->worldmat,this->bounds.w, this->bounds.h, &left,&top, &right,&bottom); 

	if(left < 0)left = 0;if(top < 0)top = 0;
	if(right > G->w) right = G->w;
	if(bottom > G->h) bottom = G->h;

	int w = right - left,h = bottom - top;
	if(w<=0 || h <=0) {
		//printf("%d:%d,%d\n",this->id,this->bounds.w, this->bounds.h);
		return;
	}

	dirty_mark((int)left,(int)top,(int)right,(int)bottom);
	//printf("%d,%d,%d,%d\n",(int)left,(int)top,(int)right,(int)bottom);
}

//检测物体脏了的区域
void node_checkdirty(struct node *this, bool parent_mat_changed) {
	bool my_mat_changed = false;
	bool dirty = false;
	bool olddirty = false;

	if(this->disable && this->mask & NMSK_DISABLE == 0) {
		return;
	}

	if(this->mask & NMSK_BOUNDS) {
		olddirty = true;
	}

	if(this->mask & NMSK_MAT) {
		this->mask ^= NMSK_MAT;
		my_mat_changed = true;
	}

	if(this->mask & NMSK_DISABLE) {
		this->mask ^= NMSK_DISABLE;
		olddirty = true;
	}

	if(parent_mat_changed || my_mat_changed || olddirty) {
		node_markdirty(this);//原来的位置
	}

	if(this->mask & NMSK_BOUNDS) {
		this->mask ^= NMSK_BOUNDS;
		if(this->bounds.x != this->tarbounds.x
			|| this->bounds.y != this->tarbounds.y) {
			//pivot changed
			my_mat_changed = true;
		}
		this->bounds = this->tarbounds;
		dirty = true;
	}

	if(parent_mat_changed || my_mat_changed) {
		dirty = true;

		if(my_mat_changed) {
			node_updatelocalmat(this);
		}

		node_updateworldmat(this);

		my_mat_changed = true;
	}

	if(dirty) {
		//printf("%d\n",this->id);
		node_markdirty(this);//新位置
	}

	struct node *i;
	node_foreach (i, this) {
		node_checkdirty(i, my_mat_changed);
	}
}

void node_update(struct node *this) {
	if (this->disable)return;

	_current_node = this;
	struct widget *wg = this->widgets;
	while (wg != NULL) {
		struct widgettag *tag = (tagstore[wg->tagid]);

		_current_widget = wg;
		_current_widgettag = tag;

		if (wg->started == false) {
			wg->started = true;
			if (tag->start != NULL)tag->start();
		}
		if (tag->update != NULL)tag->update();

		wg = wg->next;
	}

	struct node *i;
	node_foreach (i, this) {
		node_update(i);
	}
}

bool node_hittest(struct node *this,float stage_x,float stage_y,float *local_x,float *local_y,struct node **out) {
	float lx=0,ly=0;
	if(this->disable) {
		return false;
	}

	mat_invert(&this->worldmat);
	mat_transform(&this->worldmat,stage_x,stage_y,&lx,&ly);
	mat_invert(&this->worldmat);

	struct node *i = NULL;	
	node_foreach (i, this) {
		if(node_hittest(i,stage_x,stage_y,local_x,local_y,out)) {
			*local_x = lx;
			*local_y = ly;
			*out = this;
			return true;
		}
	}

	/*if(this == _root) {
		//如果是_root,则一定成立
		*out = this;
		*local_x = lx;
		*local_y = ly;
		return true;
	}*/

	_current_node = this;
	struct widget *wg = this->widgets;
	while (wg != NULL) {
		struct widgettag *tag = (tagstore[wg->tagid]);

		_current_widget = wg;
		_current_widgettag = tag;

		if (tag->hittest != NULL) {
			bool cancel = false;
			bool ret = tag->hittest(lx,ly,&cancel);
			if(ret) {
				*local_x = lx;
				*local_y = ly;
				*out = this;
				return true;
			}
			if(cancel)return false;
		}

		wg = wg->next;
	}

	return lx >= 0 && ly >= 0 && lx < this->bounds.w && ly < this->bounds.h;
}

void node_redraw(bool show) {
	if(_showredraw == show)return;
	_showredraw = show;
	if(!show) {
		dirty_mark(0,0,G->w,G->h);
	}
}

void node_renderall(struct node *root) {
	if(!root)return;
	_root = root;

	if(_rootcontext.w != G->w || _rootcontext.h != G->h) {
		node_resizecanvas();
	}

	node_checkdirty(_root,false);
	//printf("dirty_count:%d\n",dirty_count());
	dirty_draw();
	dirty_reset();
}

void node_addwidget(struct node *this,struct widget *widget) {
	if (widget->node != NULL) {
		node_removewidget(widget->node,widget);
	}
	widget->node = this;

	if (this->widgets == NULL) {
		this->widgets = widget;
	} else {
		struct widget *wg = this->widgets;
		while (wg != NULL) {
			if (wg->next == NULL) {
				wg->next = widget;
				break;
			}
			wg = wg->next;
		}
	}
	widget->next = NULL;

	struct widgettag *tag = (tagstore[widget->tagid]);
	_current_node = this;
	_current_widget = widget;
	_current_widgettag = tag;
	if (widget->awaked == false) {
		widget->awaked = true;
		if (tag->awake!= NULL)tag->awake();
	}
	if (tag->enable!= NULL)tag->enable();
}

struct widget * node_getwidget(struct node *this,int tagid) {
	struct widget *wg = this->widgets;
	while (wg != NULL) {
		if (wg->tagid == tagid) {
			return wg;
		}
		wg = wg->next;
	}
}

void node_removewidget(struct node *this,struct widget *widget) {
	if (widget->node != this)return;

	if (this->widgets == widget) {
		this->widgets = widget->next;
	} else {
		struct widget *wg = this->widgets;
		while (wg != NULL) {
			if (wg->next == widget) {
				wg->next = widget->next;
				break;
			}
			wg = wg->next;
		}
	}

	struct widgettag *tag = (tagstore[widget->tagid]);
	_current_node = this;
	_current_widget = widget;
	_current_widgettag = tag;
	if (tag->disable!= NULL)tag->disable();

	widget->node = NULL;
}

void widget_destroy(struct widget *widget) {
	if (widget->node != NULL) {
		node_removewidget(widget->node,widget);
	}
	struct widgettag *tag = (tagstore[widget->tagid]);
	_current_widget = widget;
	_current_widgettag = tag;
	if (tag->destroy!= NULL)tag->destroy();
	free(widget);
}

void widget_init(struct widget *this,int tagid) {
	this->next = NULL;
	this->node = NULL;
	this->disable = false;
	this->tagid = tagid;

	this->started = false;
	this->awaked = false;
}

void widgettag_init(struct widgettag *tag,int tagid) {
	tag->tagid = tagid;
	tag->awake = NULL;
	tag->enable = NULL;
	tag->start = NULL;
	tag->draw = NULL;
	tag->update = NULL;
	tag->disable = NULL;
	tag->destroy = NULL;
	tag->hittest = NULL;
}

void widgettag_install(struct widgettag *tag) {
	tagstore[tag->tagid] = tag;
}