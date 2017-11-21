#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>

// linux kernels
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/fb.h>

#include "game.h"

FILE* driver;

static short* board;

fb_copyarea head_rect;
fb_copyarea tail_rect;
fb_copyarea fruit_rect;

static snake snake_head;
static snake snake_tail;

dir_list* unused_items = NULL;

static struct timer_list screen_timer;

static int fbfd;

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
void update_pos(char dir, short* x, short *y, short **pos){
  switch (dir){
    case 1:
      (*x)--;
      (*pos)--;
      break;
    case 2:
      (*y)--;
      (*pos)-=BOARD_WIDTH;
      break;
    case 4:
      (*x)++;
      (*pos)++;
      break;
    case 8:
      (*y)++;
      (*pos)+=BOARD_WIDTH;
      break;
    default:
      printf("Error: Dir variable is : %d\n",dir);
  }
}

void screen_timer_handler(unsigned long data){
  if (snake_head.move==0){
    update_pos(snake_head.dir,&snake_head.copyarea->dx, \
               &snake_head.copyarea->dy,&snake_head.pos);
    *snake_head.pos = 0x0F10;
    snake_head.list->count++;
    ioctl(fbfd,0x4680,snake_head.copyarea);
  } else {
    snake_head.move--;
  }
  if (snake_tail.move==0){
    update_pos(snake_tail.dir,&snake_tail.copyarea->dx, \
               &snake_tail.copyarea->dy,&snake_tail.pos);
    if (snake_tail.list->count==0){
      snake_tail.list=snake_tail.list->next;
    } else {
      snake_tail.list->count--;
    }
    *snake_tail.pos = 0;
    ioctl(fbfd,0x4680,snake_tail.copyarea);
  } else {
    snake_tail.move--;
  }
}

int main(int argc, char *argv[])
{
	printf("Game starting\n");
	
	driver = fopen("/dev/driver-gamepad", "rb");

  fbfd = open("/dev/fb0", O_RDWR, 0);
  board = (short*) mmap(NULL, BOARD_WIDTH * BOARD_HEIGHT, \
                        PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

  if (driver == NULL) {
		printf("Driver error\n");
		exit(EXIT_SUCCESS);
	}

	// init: set rect sizes
  head_rect.dx = START_X + 2;
  head_rect.dy = START_Y;
	head_rect.width = 3;
	head_rect.height = 3;
  tail_rect.dx = START_X - 2;
  tail_rect.dy = START_Y;
	tail_rect.width = 3;
	tail_rect.height = 3;
	fruit_rect.width = 3;
	fruit_rect.height = 3;

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
  snake_head.copyarea = &head_rect;
  snake_tail.copyearea = &tail_rect;
  snake_head.pos = board + head_rect.dx + \
                   head_rect.dy * BOARD_WIDTH;
  snake_tail.pos = board + tail_rect.dx +
                   tail_rect.dy * BOARD_WIDTH;

  setup_timer( &screen_timer, &screen_timer_handler, 0 );

	signal(SIGIO, &sigio_handler);
	fcntl(fileno(driver), F_SETOWN, getpid());

	long oflags = fcntl(fileno(driver), F_GETFL);
    	fcntl(fileno(driver), F_SETFL, oflags | FASYNC);

  int ret = mod_timer( &screen_timer, jiffies + msecs_to_jiffies(200) );
  if (ret) printf("Error in mod_timer\n");

  while(1){
	    printf("\n");
	    pause();
	};

	exit(EXIT_SUCCESS);
}
