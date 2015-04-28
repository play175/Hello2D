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
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "game.h"
#include "blit.h"

struct game *G = NULL;

void game_init() {
	if (G != NULL)return;
	G = malloc(sizeof(struct game));
	G->bits = NULL;
	G->running = true;
	G->singleton = false;
	G->alpha = false;
	G->noborder = false;
	G->mousedown = false;
	G->w = 320;
	G->h = 240;
	G->fixeddelta = 1.0f / 60.0f;
	G->delta = 0;
	G->frame = 0;
	G->now = clock();
	G->bgcolor = 0xff668899;
}

void game_free() {
	if (G != NULL) {
		free(G);
		G = NULL;
	}
}
