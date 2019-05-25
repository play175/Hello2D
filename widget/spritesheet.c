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
#include "spritesheet.h"


struct spritesheet *spritesheet_new() {
	struct spritesheet *this = malloc(sizeof(*this));
	spritesheet_init(this);
	return this;
}

void spritesheet_init(struct spritesheet *this) {
	this->interp = 0;
	this->otime = 0;
	this->fps = 24;
	this->frame = 0;
	this->frameidx = 0;
	kv_init(this->frames);
	this->active = NULL;
	this->animap = kh_init(animap);
}

void spritesheet_clone(struct spritesheet *this,struct spritesheet *to) {
	int n = kv_size(this->frames);
	kv_init(to->frames);
	kv_resize(struct rect,to->frames,n);
	kv_size(to->frames) = n;
	int i;
	for (i = 0;i < n;i++) {
		kv_A(to->frames,i) = kv_A(this->frames,i);
	}
	khiter_t k;
	for (k = kh_begin(this->animap); k != kh_end(this->animap); ++k) {
		if (kh_exist(this->animap, k)) {
			khiter_t kk;
			int ret;
			kk = kh_put(animap, to->animap, kh_key(this->animap, k), &ret);
			if (!ret) kh_del(animap, to->animap, kk);

			struct sheet_anim *ani = & kh_value(this->animap, k);
			kh_value(to->animap, kk) = *ani;

			struct sheet_anim *ani2 = & kh_value(to->animap, kk);
			kv_init(ani2->frameidxs);
			kv_resize(uint16_t,ani2->frameidxs,kv_size(ani->frameidxs));
			kv_size(ani2->frameidxs) = kv_size(ani->frameidxs);
			for (i = 0;i<kv_size(ani2->frameidxs);i++) {
				kv_A(ani2->frameidxs,i) = kv_A(ani->frameidxs,i);
			}

			if (this->active == ani) {
				to->active = ani2;
			}
		}
	}

	to->otime = this->otime;
	to->fps = this->fps;
	to->frame = this->frame;
	to->frameidx = this->frameidx;
}

int spritesheet_get_totalframes(struct spritesheet *this) {
	return kv_size(this->frames);
}

void spritesheet_destroy(struct spritesheet *this) {
	spritesheet_clean(this);
	free(this);
}

void spritesheet_clean(struct spritesheet *this) {
	khiter_t k;
	for (k = kh_begin(this->animap); k != kh_end(this->animap); ++k) {
		if (kh_exist(this->animap, k)) {
			struct sheet_anim *ani = &kh_value(this->animap, k);
			kv_destroy(ani->frameidxs);
		}
	}
	kh_destroy(animap, this->animap);
	kv_destroy(this->frames);
}

struct rect *spritesheet_get_rect(struct spritesheet *this) {
	if (kv_size(this->frames) == 0)return NULL;
	return & kv_A(this->frames,this->frame);
}

//frames 会复制，此处不会保留指针
void spritesheet_set_frames(struct spritesheet *this,struct rect *frames,int n) {
	kv_resize(struct rect,this->frames,n);
	kv_size(this->frames) = n;
	int i;
	for (i = 0;i < n;i++) {
		kv_A(this->frames,i) = frames[i];
	}
}

//从sprite sheet 生成纹理集
void spritesheet_set_frames_from_sheet(struct spritesheet *this,float tex_w,float tex_h,int col,int row,int n) {
	if (n > col * row)n = col * row;
	kv_resize(struct rect,this->frames,n);
	kv_size(this->frames) = n;
	struct rect rect;
	rect.w = tex_w/col;
	rect.h = tex_h/row;
	int x,y;
	int ox,oy;
	ox = 0;
	oy = 0;
	int m = 0;
	for (y = 0;y < row;y++) {
		ox = 0;
		for (x = 0;x < col;x++) {
			rect.x = ox;
			rect.y = oy;
			kv_A(this->frames,m) = rect;
			//debug("%f,%f,%f,%f",kv_A(this->frames,m).x,kv_A(this->frames,m).y,kv_A(this->frames,m).w,kv_A(this->frames,m).h);
			m++;
			ox += rect.w;
		}
		oy += rect.h;
	}
}

void spritesheet_add_ani_batch(struct spritesheet *this,int id,uint16_t start,uint16_t end,bool loop) {
	int n = abs(end - start);
	if (n<=0) {
		debug("n:%d must gt 0",n);
		return;
	}

	struct sheet_anim ani;

	int is_missing;
	int ret;
	khiter_t k;

	k = kh_get(animap, this->animap, id);
	is_missing = (k == kh_end(this->animap));
	if (!is_missing) {
		ani = kh_value(this->animap, k);
		kv_resize(uint16_t,ani.frameidxs,kv_size(ani.frameidxs) + n);
	} else {
		kv_init(ani.frameidxs);
		kv_resize(uint16_t,ani.frameidxs,n);
		kv_size(ani.frameidxs) = 0;
	}
	ani.loop = loop;

	int i,step = start>end?-1:1;
	for (i = 0;i < n;i++) {
		kv_push(uint16_t,ani.frameidxs,(start + step * i));
	}

	if (!is_missing) {
		kh_value(this->animap, k) = ani;
	} else {
		k = kh_put(animap, this->animap, id, &ret);
		if (!ret) kh_del(animap, this->animap, k);
		kh_value(this->animap, k) = ani;
	}
}

//id不能==0，framindexs 会复制，此处不会保留指针
void spritesheet_add_ani(struct spritesheet *this,int id,uint16_t *framindexs,int n,bool loop) {
	if (n<=0) {
		debug("n:%d must gt 0",n);
		return;
	}

	int is_missing;
	khiter_t k;
	k = kh_get(animap, this->animap, id);
	is_missing = (k == kh_end(this->animap));
	if (!is_missing) {
		debug("id:%d was aready exists",id);
		return;
	}

	struct sheet_anim ani;
	ani.loop = loop;
	kv_init(ani.frameidxs);
	kv_resize(uint16_t,ani.frameidxs,n);
	kv_size(ani.frameidxs) = n;
	int i;
	for (i = 0;i < n;i++) {
		kv_A(ani.frameidxs,i) = framindexs[i];
	}

	int ret;
	k = kh_put(animap, this->animap, id, &ret);
	if (!ret) kh_del(animap, this->animap, k);
	kh_value(this->animap, k) = ani;
}

void spritesheet_play_x(struct spritesheet *this,int id,int start_idx,bool restart) {
	int is_missing;
	khiter_t k;
	k = kh_get(animap, this->animap, id);
	is_missing = (k == kh_end(this->animap));
	if (is_missing) {
		debug("id:%d not exists",id);
		return;
	}
	struct sheet_anim *ani = &(kh_value(this->animap, k));
	if (restart || this->active != ani) {
		this->active = ani;
		if (start_idx<0)start_idx = 0;
		if (start_idx >= kv_size(this->active->frameidxs)) {
			start_idx = kv_size(this->active->frameidxs) - 1;
		}
		this->frameidx = start_idx;
		this->frame = kv_A(this->active->frameidxs,this->frameidx);
	}
}

void spritesheet_set_frame(struct spritesheet *this,uint16_t frame) {
	if (frame < 0 || frame > kv_size(this->frames)) {
		debug("frame:%d not exists",frame);
		return;
	}
	this->frame = frame;
}

void spritesheet_update(struct spritesheet *this,int now) {
	if (!this->active)return;
	if (this->fps == 0) {
		debug("fps:%d",this->fps);
	} else {
		if (this->otime>0) {
			this->interp += this->fps * (now - this->otime);
			if (this->interp >= 1000.0) {
				this->frameidx += this->interp/1000;
				this->interp = this->interp%1000;
			}
		}
		if (this->interp<0) {
			debug("interp:%d",this->interp);
			this->interp = 0;
		}
		this->otime = now;
		if (this->frameidx >= kv_size(this->active->frameidxs)) {
			if (this->active->loop) {
				this->frameidx %= kv_size(this->active->frameidxs);
			} else {
				this->frameidx = kv_size(this->active->frameidxs) - 1;
			}
		}
		this->frame = kv_A(this->active->frameidxs,this->frameidx);
		//debug("%d,%d",this->frame,this->interp);
	}
}