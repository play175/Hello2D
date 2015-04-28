/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdint.h>
#include <stdbool.h>
#include "h2d.h"
#include "khash.h"
#include "kvec.h"

#ifndef _SPRITESHEET_H
#define _SPRITESHEET_H

struct frame {
	vec orign;
	rect rect;
};

struct sheet_anim {
	bool loop;
	kvec_t(uint16_t) frameidxs;//帧序号数组
};

KHASH_MAP_INIT_INT(animap, struct sheet_anim)

struct spritesheet {
	int otime;
	int interp;
	int fps;//1000/fps
	int frame;//当前帧序号
	int frameidx;//在active中的当前索引
	kvec_t(struct rect) frames;//动画总帧序列
	struct sheet_anim *active;//当前动画
	khash_t(animap) *animap;//动画表，key是动画ID
};

void spritesheet_clone(struct spritesheet *this,struct spritesheet *to);
void spritesheet_update(struct spritesheet *this,int now);
struct spritesheet *spritesheet_new();
void spritesheet_init(struct spritesheet *this);
void spritesheet_clean(struct spritesheet *this);
void spritesheet_destroy(struct spritesheet *this);
void spritesheet_add_ani_batch(struct spritesheet *this,int id,uint16_t start,uint16_t end,bool loop);
void spritesheet_add_ani(struct spritesheet *this,int id,uint16_t *framindexs,int n,bool loop);
void spritesheet_set_frames(struct spritesheet *this,struct rect *frames,int n);
void spritesheet_set_frame(struct spritesheet *this,uint16_t frame);
int spritesheet_get_totalframes(struct spritesheet *this);
void spritesheet_set_frames_from_sheet(struct spritesheet *this,float tex_w,float text_h,int col,int row,int n);
struct rect *spritesheet_get_rect(struct spritesheet *this);

inline float spritesheet_get_width(struct spritesheet *this) {
	return kv_size(this->frames) == 0 ? 0 : kv_A(this->frames,this->frame).w;
}

inline float spritesheet_get_height(struct spritesheet *this) {
	return kv_size(this->frames) == 0 ? 0 : kv_A(this->frames,this->frame).h;
}

//rect 会复制，此处不会保留指针
inline void spritesheet_add_frame(struct spritesheet *this,struct rect *rect) {
	kv_push(struct rect,this->frames,*rect);
}

void spritesheet_play_x(struct spritesheet *this,int id,int start_idx,bool restart);
#define spritesheet_play(this,id) spritesheet_play_x(this,id,0,false)
#define spritesheet_replay(this,id) spritesheet_play_x(this,id,0,true)
#define spritesheet_replay_from(this,id,idx) spritesheet_play_x(this,id,idx,true)

#endif
