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

struct fb_copyarea head_rect;
struct fb_copyarea tail_rect;
struct fb_copyarea fruit_rect;

snake snake_head;
snake snake_tail;

dir_list* unused_items;

void sigio_handler(int signo)
{
    printf("knapp: %d\n",fgetc(driver));
<<<<<<< HEAD
    snake_head.dir = 
    if (snake_tail.list==unused_items){
      snake_head.list->next = malloc(sizeof(dir_list))
      snake_head.list = snake_head.list->next;
      snake_head.list->dir = snake_head.dir;
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
    } else {
      snake_head.list->next = unused_items;
      snake_head.list = unused_items;
      unused_items = unused_items.next;
      snake_head.list->dir = snake_head.dir;
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
    }
    
=======
>>>>>>> ca40420c3891ac3567ead19276e3003d475bb696
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

	// init: set rect sizes
	head_rect.width=3;
	head_rect.height=3;
	tail_rect.width=3;
	tail_rect.height=3;
	fruit_rect.width=3;
	fruit_rect.height=3;

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
