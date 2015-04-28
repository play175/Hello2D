/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "dirty.h"

#define MAX_DIRTY	5

struct rect {
	int left, top, right, bottom;
};

static struct rect	_dirty_rects[MAX_DIRTY];
static int	_rect_count=0;
static int	SCREEN_WIDTH=0;
static int	SCREEN_HEIGHT=0;

static inline bool rect_contains(struct rect *outer, struct rect *inner) {
	return (outer->left <= inner->left && outer->top <= inner->top && outer->right >= inner->right && outer->bottom >= inner->bottom);
}

static inline struct rect rect_combine(struct rect *r1, struct rect *r2) {
	struct rect r;
	r.left   = (r1->left   < r2->left  ) ? r1->left   : r2->left;
	r.top    = (r1->top    < r2->top   ) ? r1->top    : r2->top;
	r.right  = (r1->right  > r2->right ) ? r1->right  : r2->right;
	r.bottom = (r1->bottom > r2->bottom) ? r1->bottom : r2->bottom;
	return r;
}

static inline int rect_calc_area(struct rect *r) {
	return (r->right - r->left) * (r->bottom - r->top);
}

void dirty_addrect(struct rect *rt) {
	if (rt == NULL) {
		_dirty_rects[0].left	= 0;
		_dirty_rects[0].top	= 0;
		_dirty_rects[0].right	= SCREEN_WIDTH;
		_dirty_rects[0].bottom	= SCREEN_HEIGHT;
		_rect_count		= 1;
		return;
	}

	if (_rect_count==0) {
		_dirty_rects[_rect_count] = *rt;
		++_rect_count;
	} else {
		int i;
		struct rect comb;
		int area1, area2, areac, areaBest;
		int bestidx = -1;

		if (_rect_count>=MAX_DIRTY-1) {
			dirty_addrect(NULL);
			return;
		}

		for (i=0; i<_rect_count; ++i) {
			if (rect_contains(&_dirty_rects[i], rt)) return;
		}
		for (i=0; i<_rect_count; ++i) {
			comb = rect_combine(&_dirty_rects[i], rt);
			area1 = rect_calc_area(&_dirty_rects[i]);
			area2 = rect_calc_area(rt);
			areac = rect_calc_area(&comb);
			if (areac < area1 + area2) {
				if (bestidx < 0 || area1 + area2 - areac > areaBest) {
					areaBest = area1 + area2 - areac;
					bestidx = i;
				}
			}
		}
		if (bestidx >= 0)
			_dirty_rects[bestidx] = rect_combine(&_dirty_rects[bestidx], rt);
		else
			_dirty_rects[_rect_count++] = *rt;
	}
}

static void (*fetch_cb)(int left, int top, int right, int bottom) = NULL;

void dirty_init(int w, int h, void (*cb)(int left, int top, int right, int bottom)) {
	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
	fetch_cb = cb;
}

void dirty_draw() {
	if (fetch_cb == NULL) {
		return;
	}
	int i;
	for (i = 0;i < _rect_count;++i) {
		fetch_cb(_dirty_rects[i].left,_dirty_rects[i].top,_dirty_rects[i].right,_dirty_rects[i].bottom);
	}
}

void dirty_mark(int left, int top, int right, int bottom) {
	struct rect rt;
	rt.left = left;
	rt.top = top;
	rt.right = right;
	rt.bottom = bottom;
	dirty_addrect(&rt);
}

void dirty_reset() {
	_rect_count = 0;
}

bool dirty_isalldirty() {
	return _rect_count == 1 &&  _dirty_rects[0].right	== SCREEN_WIDTH && _dirty_rects[0].bottom	== SCREEN_HEIGHT;
}

int dirty_count() {
	return _rect_count;
}

