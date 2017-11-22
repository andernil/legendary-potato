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
#include <string.h>

// linux kernels
#include <linux/fb.h>
#include "game.h"

FILE* driver;
FILE* timer;

static short* board;

fb_copyarea head_rect;
fb_copyarea tail_rect;
fb_copyarea fruit_rect;
fb_copyarea board_rect;
static snake snake_head;
static snake snake_tail;

dir_list* unused_items = NULL;

static int fbfd;


void sigio_handler(int signo)
{
    static int last_button_return = 4;
    int button_return;
    button_return = fgetc(driver);
    if (button_return > 8){
	switch(button_return){
	  case 16: button_return = 1;
		break;
	  case 32: button_return = 2;
		break;
	  case 64: button_return = 4;
		break;
	  case 128: button_return = 8;
		break;
          default: printf("Invalid button return : %d\n",button_return);
 	}
    }

//    printf("button_return: %d last_button_return %d \n", button_return,last_button_return);
    if (button_return == last_button_return || button_return == 0){
	timer_handler(0);
    }else{
    snake_head.dir = button_return;
    if (unused_items==NULL){
      snake_head.list->next = (void*) malloc(sizeof(dir_list));
      snake_head.list->dir = snake_head.dir;
      snake_head.list = snake_head.list->next;
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
    } else {
      snake_head.list->next = unused_items;
      snake_head.list->dir = snake_head.dir;
      snake_head.list = unused_items;
      if (unused_items->next==snake_tail.list){
        unused_items = NULL;
      } else {
        unused_items = unused_items->next;
      }
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
    }
    last_button_return = button_return;
    }
}

void update_pos(char dir, short* x, short *y, short **pos){
  switch (dir){
    case 1:
      if (*x == 0){
        (*x) = (BOARD_WIDTH-1);
        (*pos) += (BOARD_WIDTH-1);
      } else {
        (*x)--;
        (*pos)--;
      }
      break;
    case 2:
      if (*y == 0){
        (*y) = (BOARD_HEIGHT-1);
        (*pos) += (BOARD_HEIGHT-1)*BOARD_WIDTH;
      } else {
        (*y)--;
        (*pos) -= BOARD_WIDTH;
      }
      break;
    case 4:
      if (*x == (BOARD_WIDTH-1)){
        (*x) = 0;
        (*pos)-=(BOARD_WIDTH-1);
      } else {
        (*x)++;
        (*pos)++;
      }
      break;
    case 8:
      if (*y == BOARD_HEIGHT-1) {
        (*y) = 0;
        (*pos) -= (BOARD_HEIGHT-1)*BOARD_WIDTH;
      } else {
        (*y)++;
        (*pos)+=BOARD_WIDTH;
      }
      break;
    default:
      printf("Error: Dir variable is : %d\n",dir);
  }
}

void timer_handler(int signo){
  if (snake_head.move==0){
    update_pos(snake_head.dir,&snake_head.copyarea->dx, \
               &snake_head.copyarea->dy,&snake_head.pos);
    if (*snake_head.pos == 0x0F10){
	printf("You died, get good!!!!\n");
      	clear_board();
      	//inizialise_snake();
    } 

    *snake_head.pos = 0x0F10;
    snake_head.list->count++;
    ioctl(fbfd,0x4680,snake_head.copyarea);
  } else {
    snake_head.move--;
  }
  if (snake_tail.move==0){
    if (snake_tail.list->count==0){
      if (unused_items==NULL)
        unused_items = snake_tail.list;
      snake_tail.dir=snake_tail.list->dir;
      snake_tail.list=snake_tail.list->next;
    } 
    snake_tail.list->count--;
  
    update_pos(snake_tail.dir,&snake_tail.copyarea->dx, \
               &snake_tail.copyarea->dy,&snake_tail.pos);
    *snake_tail.pos = 0;
    ioctl(fbfd,0x4680,snake_tail.copyarea);
  } else {
    snake_tail.move--;
  }
}

int clear_board()
{
  
  printf("Clearing board\n");
  memset(board,0,BOARD_WIDTH*BOARD_HEIGHT*sizeof(short)); 
  
  board_rect.width = BOARD_WIDTH;
  board_rect.height = BOARD_HEIGHT;
  board_rect.dx = 0;
  board_rect.dy = 0;
  ioctl(fbfd,0x4680,&board_rect);
  return 0;
}

int inizialise_snake(){
  // init: draw snake

  head_rect.dy = START_Y;
  head_rect.dx = START_X + SNAKE_MIN_LENGTH/2;
	// init: finish copyarea
  tail_rect.dx = START_X - SNAKE_MIN_LENGTH/2;
  tail_rect.dy = START_Y;
  tail_rect.width = 1;
  tail_rect.height = 1;
  fruit_rect.width = 3;
  fruit_rect.height = 3;

  head_rect.width = 1;
  head_rect.height = 1;
	// init: set tail & head position
  snake_head.list = (void*) malloc(sizeof(dir_list));
  snake_head.list->count = SNAKE_MIN_LENGTH-1;
  snake_head.list->dir = 4;
  snake_head.list->next = NULL;
  snake_tail.list = snake_head.list;
  snake_head.move = 0;
  snake_tail.move = 0;
  snake_head.dir = 4;
  snake_tail.dir = 4;
  snake_head.copyarea = &head_rect;
  snake_tail.copyarea = &tail_rect;
  snake_head.pos = board + head_rect.dx + head_rect.dy * BOARD_WIDTH;
  snake_tail.pos = board + tail_rect.dx + tail_rect.dy * BOARD_WIDTH;
}

int main(int argc, char *argv[])
{
  printf("Game starting, hello snake world\n");
	
  driver = fopen("/dev/driver-gamepad", "rb");
  timer = fopen("/dev/timer-module", "rb");

  fbfd = open("/dev/fb0", O_RDWR, 0);
  board = (short*) mmap(NULL, BOARD_WIDTH * BOARD_HEIGHT, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

  if (driver == NULL) {
    printf("Driver error\n");
    exit(EXIT_SUCCESS);
  }

  if (timer == NULL) {
    printf("Timer error\n");
    exit(EXIT_SUCCESS);
  }


  inizialise_snake();
  clear_board();

  signal(SIGIO, &sigio_handler);
  fcntl(fileno(driver), F_SETOWN, getpid());
  long oflags = fcntl(fileno(driver), F_GETFL);
  fcntl(fileno(driver), F_SETFL, oflags | FASYNC);
  fcntl(fileno(timer), F_SETOWN, getpid());
  oflags = fcntl(fileno(timer), F_GETFL);
  fcntl(fileno(timer), F_SETFL, oflags | FASYNC);

  
  while(1){
	    pause();
	};

	exit(EXIT_SUCCESS);
}
