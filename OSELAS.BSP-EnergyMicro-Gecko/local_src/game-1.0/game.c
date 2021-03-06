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
static int last_button_return = 4;

void sigio_handler(int signo)
{

  int button_return;
  button_return = fgetc(driver);
  // Can use either set of 4 buttons.
  if (button_return > 8)
    button_return >>= 4;

    //printf("button_return: %d last_button_return %d \n", button_return,last_button_return);
    // if the button is the same, it is a timer call
    if (button_return == last_button_return || button_return == 0){
	    timer_handler(0);
    } else {
      // set the new direction of the snake
      snake_head.dir = button_return;
      if (unused_items==NULL){ // no old elements, allocate new
        snake_head.list->next = (void*) malloc(sizeof(dir_list));
        snake_head.list->dir = snake_head.dir;
        snake_head.list = snake_head.list->next;
      } else { // reuse old elements of the list
        snake_head.list->next = unused_items;
        snake_head.list->dir = snake_head.dir;
        snake_head.list = unused_items;
        // no more unused elements, set it to NULL
        if (unused_items->next==snake_tail.list){
          unused_items = NULL;
        } else {
          unused_items = unused_items->next;
        }
      }
      snake_head.list->count = 0;
      snake_head.list->next = NULL;
      last_button_return = button_return;
    }
}

// this makes sure the snake apear on the other side
// of the board when crossing the edge
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

    if (*snake_head.pos == -5888) {
      printf("You got good!\n");
      snake_tail.move+=15;
      remove_fruit();
      draw_fruit();
    }

    if (*snake_head.pos == 0x0F10){
	    printf("You died, get good!!!!\n");
	    last_button_return = 4;
      clear_board();
      draw_fruit();
      inizialise_snake();
      update_pos(snake_head.dir,&snake_head.copyarea->dx, \
                 &snake_head.copyarea->dy,&snake_head.pos);
    }
    *snake_head.pos = 0x0F10;
    snake_head.list->count++;
    ioctl(fbfd,0x4680,snake_head.copyarea);
  } else { // the head is frozen, count down till it moves
    snake_head.move--;
  }
  if (snake_tail.move==0){
    // if the count is 0, pop this element
    // and set the new direction of the tail
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
  } else { // the tail is frozen, cont down till it moves
    snake_tail.move--;
  }
}

int draw_fruit(){
  printf("Drawing fruit\n");
  short* fruit_pos;
  fruit_rect.dx = (rand() % (BOARD_WIDTH  - FRUIT_SIZE));
  fruit_rect.dy = (rand() % (BOARD_HEIGHT - FRUIT_SIZE));
  
  int i, j;
  for(i=0; i < FRUIT_SIZE; i++){
  fruit_pos = board + fruit_rect.dx + ((fruit_rect.dy + i) * BOARD_WIDTH);
    for(j=0; j < FRUIT_SIZE; j++){
      *(fruit_pos+j) = 0xE900;
    }
  }

  ioctl(fbfd,0x4680,&fruit_rect);
  return 0;
}

int remove_fruit(){
  short* fruit_pos;
  
  int i, j;
  for(i=0; i < FRUIT_SIZE; i++){
  fruit_pos = board + fruit_rect.dx + ((fruit_rect.dy + i) * BOARD_WIDTH);
    for(j=0; j < FRUIT_SIZE; j++){
      *(fruit_pos+j) = 0;
    }
  }

  ioctl(fbfd,0x4680,&fruit_rect);
  return 0;
}

int clear_board()
{
  printf("Clearing board\n");
  memset(board,0,BOARD_WIDTH*BOARD_HEIGHT*sizeof(short)); 
  ioctl(fbfd,0x4680,&board_rect);
  return 0;
}

void inizialise_snake(){
  // Set the head and tail start positions
  head_rect.dy = START_Y;
  head_rect.dx = START_X + SNAKE_MIN_LENGTH/2;
  tail_rect.dx = START_X - SNAKE_MIN_LENGTH/2;
  tail_rect.dy = START_Y;

	// set all variables that needs to reset on death
  snake_head.list->count = SNAKE_MIN_LENGTH-1;
  snake_head.list->dir = 4;
  snake_tail.list = snake_head.list;
  snake_head.move = 0;
  snake_tail.move = 0;
  snake_head.dir = 4;
  snake_tail.dir = 4;
  snake_head.pos = board + head_rect.dx + head_rect.dy * BOARD_WIDTH;
  snake_tail.pos = board + tail_rect.dx + tail_rect.dy * BOARD_WIDTH;

  // freeze the snake at start (not needed)
  snake_tail.move =5;
  snake_head.move =5;
}

int main(int argc, char *argv[])
{
  printf("Game starting, hello snake world\n");
  driver = fopen("/dev/driver-gamepad", "rb");
  timer = fopen("/dev/timer-module", "wb");

  fbfd = open("/dev/fb0", O_RDWR, 0);
  // memory map the board
  board = (short*) mmap(NULL, BOARD_WIDTH * BOARD_HEIGHT, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

  if (driver == NULL) {
    printf("Driver error\n");
    exit(EXIT_SUCCESS);
  }

  if (timer == NULL) {
    printf("Timer error\n");
    exit(EXIT_SUCCESS);
  }

  // init all 4 rects
  board_rect.width = BOARD_WIDTH;
  board_rect.height = BOARD_HEIGHT;
  board_rect.dx = 0;
  board_rect.dy = 0;
  fruit_rect.width = FRUIT_SIZE;
  fruit_rect.height = FRUIT_SIZE;
  tail_rect.width = 1;
  tail_rect.height = 1;
  head_rect.width = 1;
  head_rect.height = 1;
  snake_head.copyarea = &head_rect;
  snake_tail.copyarea = &tail_rect;
  // allocate first element of the linked list
  snake_head.list = (void*) malloc(sizeof(dir_list));
  snake_head.list->next = NULL;
  inizialise_snake();
  clear_board();
  draw_fruit();

  // init the gamepad and timer handler
  signal(SIGIO, &sigio_handler);
  fcntl(fileno(driver), F_SETOWN, getpid());
  long oflags = fcntl(fileno(driver), F_GETFL);
  fcntl(fileno(driver), F_SETFL, oflags | FASYNC);

  fcntl(fileno(timer), F_SETOWN, getpid());
  oflags = fcntl(fileno(timer), F_GETFL);
  fcntl(fileno(timer), F_SETFL, oflags | FASYNC);

  fputc(50, timer);
  while(1){
	  pause();
  };

	exit(EXIT_SUCCESS);
}
