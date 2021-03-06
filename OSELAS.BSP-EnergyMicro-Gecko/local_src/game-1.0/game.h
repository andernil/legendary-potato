#ifndef GAME_H
#define GAME_H

// need to include this to use the fb_copyarea struct
#include <linux/fb.h>

#define BOARD_WIDTH 320
#define BOARD_HEIGHT 240
#define START_X ((BOARD_WIDTH)/2)
#define START_Y ((BOARD_HEIGHT)/2)
#define SNAKE_MIN_LENGTH 5
#define SNAKE_MAX_LENGTH 1000
#define FRUIT_SIZE 3

typedef struct {
  short count;
  char dir;
  struct dir_list* next;
} dir_list;

typedef struct fb_copyarea fb_copyarea;

typedef struct {
  short* pos;
  fb_copyarea* copyarea;
  dir_list* list;
  char move; // move when 0, counts down
  char dir;  // 1 left, 2 up, 4 right, 8 down
} snake;


#endif
