/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "h2d.h"

static struct node *_root = NULL;
static struct texture *_tex = NULL;
static struct spritesheet *_sheet = NULL;

struct widget_test {
	struct widget super;
};

void widget_test_start() {
	struct node *this = _current_node;

	if (this->id == 2 || this->id == 3) {
		//增加一个image组件
		struct widget_image *wgimg = imagewidget_addto(this);
		if (this->id == 2) {
			wgimg->tex = texture_from("res/close.png");
			wgimg->reset = true;
		} else {
			wgimg->tex = texture_from("res/attack.png");
			wgimg->reset = true;
		}
	} else {
		struct widget_sprite *wgspr = spritewidget_addto(this);
		
		if(_tex == NULL) {
			_tex = texture_from("res/role.png");
		}
		if(_sheet == NULL) {
			_sheet = spritesheet_new();
			spritesheet_init(_sheet);
			spritesheet_set_frames_from_sheet(_sheet,_tex->w,_tex->h,5,4,20);
			spritesheet_add_ani_batch(_sheet,1,0,19,true);
			spritesheet_play(_sheet,1);
		}

		wgspr->tex = _tex;
		wgspr->sheet = _sheet;
	}
}

void widget_test_update() {
	struct node *this = _current_node;

	if (this->id==2 || this->id==3) {
		//修改零点为图片中心点(零点默认在左上角)
		node_pivot(this, 0 - this->bounds.w /2, 0 - this->bounds.h /2);
	}

	if (this->id % 4 == 0 || this->id % 6 == 0) {
		float x = this->pos.x;
		x -=  30.0f * G->delta;

		if(x < -20)x = G->w;
		if(x > G->w)x = 0;
		node_x(this,x);
	}

	if (this->id % 5 == 0 || this->id % 7 == 0) {
		node_scalex(this,-1);

		float x = this->pos.x;
		x += 30.0f * G->delta;

		if(x < -20)x = G->w;
		if(x > G->w)x = 0;
		node_x(this,x);
	}

	if (this->id==3) {
		node_rot(this, -60.0f * G->delta);
	}

	if (this->id==2) {
		float sc = this->scale.x;
		sc -= 0.03f * 60.0f * G->delta;
		if (sc < -4) {
			sc = 1;
		}
		node_scalexy(this,sc);

		node_rot(this, -60.0f * G->delta);

		node_x(this,this->pos.x + 60.0f * G->delta);
		node_x(this,(int)(this->pos.x) % G->w);
	}
}

struct widgettag _tag_test = {
	.tagid = WGTAG_TEST
	,.update = widget_test_update
	,.start = widget_test_start
};

struct widget_test *testwidget_addto(struct node *this) {
	struct widget_test *wgtest = malloc(sizeof(struct widget_test));
	widget_init(&wgtest->super,WGTAG_TEST);
	node_addwidget(this,&wgtest->super);
	return wgtest;
}

bool game_create(int argc, char **argv) {
	widget_installall();
	widgettag_install(&_tag_test);//test组件初始化

	_root = node_new();

	int ii=0;
	for (ii=0;ii<3;++ii) {
		struct node *node1 = node_new();
		node_pos(node1,rand() % 320,rand() % 400);
		node_addchild(_root,node1);

		//测试，增删改节点
		//struct node *node2 = node_new();
		//struct node *node3 = node_new();

		//node_addchild_at(_root,node2,0);
		//node_addchild_at(_root,node3,0);
		//node_removechild_at(_root,0);
		//node_addchild_at(_root,node3,0);

		//需要什么功能就加什么组件
		struct widget_test *wgtest = testwidget_addto(node1);
	}

	//遍历子节点
	/*struct node *i;
	node_foreach (i, _root) {
		printf("id:%d\n",i->id);
	}*/

	//G->w = 640;
	G->h = 480;
	//G->noborder = true;
	//G->alpha = true;
	//G->bgcolor = 0xff668899;

	return true;
}

void game_destroy() {
}

void game_update() {
	node_update(_root);
}

void game_frame() {
	node_renderall(_root);
	game_showfps();
}

void game_touch(int x, int y,int touch) {
	//debug("touch:%d,%d,%d",touch,x,y);
	if (touch == TOUCH_BEGIN) {
		struct node *out = NULL;
		float lx,ly;
		if (node_hittest(_root,x,y,&lx,&ly,&out) == false) {
			begin_movewindow();
		}
	}
}

void game_wheel(int delta,int x,int y) {
	//debug("whee:%d,%d,%d",delta,x,y);
}

void game_resizing(int neww, int newh, int oldw, int oldh) {

}

void game_resized(int w,int h) {
	
}

void game_dropfile(char *file,int index,int total) {
	//debug("dropfile:%s(%d/%d)",file,index + 1,total);
}

void game_keydown(int key, bool ctrl, bool alt, bool shift,short repeat) {
	//debug("game_keydown:%d,%d,%d,%d,%d",key,ctrl,alt,shift,repeat);
	//按下任意键显示重绘区域
	node_redraw(true);
}

void game_keyup(int key, bool ctrl, bool alt, bool shift,short repeat) {
	//debug("game_keyup:%d,%d,%d,%d,%d",key,ctrl,alt,shift,repeat);
	node_redraw(false);
	if (key == 27)G->running = false;//esc
}