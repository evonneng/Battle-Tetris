#include <stdio.h>
#include <stdlib.h>
#include <tetris.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

struct tetris_level {
	int score;
	int time;
};

struct tetris {
	char **game;
	int w;
	int h;
	int level;
	int gameover;
	int score;
	struct tetromino {
		char data[5][5];
		int w;
		int h;
	} current;
	int x;
	int y;
};

struct tetromino pieces[] = {
	{{"##", 
      "##"},
    2, 2
    },
    {{" # ",
      "###"},
    3, 2
    },
    {{"####"},
        4, 1},
    {{"##",
      "# ",
      "# "},
    2, 3},
    {{"##",
      " #",
      " #"},
    2, 3},
    {{"## ",
      " ##"},
    3, 2}
};

#define TETRIS_PIECES (sizeof(pieces)/sizeof(struct tetromino))

void tetris_init(struct tetris *t, int w, int h) {
	int x, y;
	t->level = 1;
	t->score = 0;
	t->gameover = 0;
	t->w = w;
	t->h = h;
	t->game = malloc(sizeof(char *)h);
	for (x=0; x<w; x++) {
        t->game[x] = malloc(sizeof(char)*h);
        for (y=0; y<h; y++)
            t->game[x][y] = ' ';
    }
}

void genNewPiece(struct tetris *t) {
	t->current = pieces[random()%TETRIS_PIECES];
	t->x = (t->w/2)-(t->current.w/2);
	t->y = 0;
	if (outBound(t)) {
		t->gameover = 1;
	}
}

int outBound(struct tetris *t) {
	int x, y, X, Y;
	struct tetromino curr = t->current;
	for (x = 0; x < curr.w; x++) {
		for (y = 0; y < curr.h; y++) {
			X = t->x + x;
			Y = t->y + y;
			if (X < 0 || X >= t->w) {
				return 1;
			}
			if (curr.data[y][x]!= ' ') {
				if ((Y >= t->h) || (X>=0 && X<t->w && Y>=0 && t->game[X][Y]!=' ')) {
                    return 1;
                }
			}
		}
	}
	return 0;
}