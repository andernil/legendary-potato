#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// need to include this to use the fb_copyarea struct
#include <linux/fb.h>

#include "game.h"

FILE* driver;

short board[BOARD_WIDTH*BOARD_HEIGHT];

fb_copyarea head_rect;
fb_copyarea tail_rect;
fb_copyarea fruit_rect;

snake snake_head;
snake snake_tail;

dir_list* unused_items = NULL;

void sigio_handler(int signo)
{
    snake_head.dir = (char) fgetc(driver);
    if (snake_tail.list==snake_head.list){
      snake_head.list->next = (void*) malloc(sizeof(dir_list));
      snake_head.list = snake_head.list->next;
      snake_head.list->dir = snake_head.dir;
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
    } else {
      snake_head.list->next = unused_items;
      snake_head.list = unused_items;
      unused_items = unused_items->next;
      snake_head.list->dir = snake_head.dir;
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
    }
}

int main(int argc, char *argv[])
{
	printf("Game starting\n");
	
	driver = fopen("/dev/driver-gamepad", "rb");

	if (driver == NULL) {
		printf("Driver error\n");
		exit(EXIT_SUCCESS);
	}

	// init: set start position
	// init: place snake & set tail position
  snake_head.list = (void*) malloc(sizeof(dir_list));
  snake_head.list->count = 5;
  snake_head.list->dir = 4;
  snake_head.list->next = NULL;
  snake_tail.list = unused_items;
  snake_head.move = 0;
  snake_tail.move = 0;
  snake_head.dir = 4;
  snake_tail.dir = 4;
  snake_head.x_pos = START_X + 2;
  snake_head.y_pos = START_Y;
  snake_tail.x_pos = START_X - 2;
  snake_head.y_pos = START_Y;
  snake_head.copyarea = &head_rect;
  snake_tail.copyearea = &tail_rect;
  snake_head.pos[snake_head.x_pos+snake_head.y_pos*BOARD_WIDTH];
  snake_tail.pos[snake_tail.x_pos+snake_tail.y_pos*BOARD_WIDTH];

	// init: set rect sizes
  head_rect.dx = snake_head.x_pos;
  head_rect.dy = snake_head.y_pos;
	head_rect.width = 3;
	head_rect.height = 3;
  tail_rect.dx = snake_tail.x_pos;
  tail_rect.dy = snake_tail.y_pos;
	tail_rect.width = 3;
	tail_rect.height = 3;
	fruit_rect.width = 3;
	fruit_rect.height = 3;

	signal(SIGIO, &sigio_handler);
	fcntl(fileno(driver), F_SETOWN, getpid());

	long oflags = fcntl(fileno(driver), F_GETFL);
    	fcntl(fileno(driver), F_SETFL, oflags | FASYNC);
	
	while(1){
	    printf("\n");
	    pause();
	};

	exit(EXIT_SUCCESS);
}
