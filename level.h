#pragma once
#include "def.h"

typedef struct square_{
    struct square_* above;
    struct square_* below;
    struct square_* left;
    struct square_* right;
    SDL_Surface* surface;
    SDL_Rect rect;
}square;

typedef struct cam_{
  int scale;
  fvec2 offset;
  ivec2 o_min;
  ivec2 o_max;
  float speed;
  int s_min;
  int s_max;
  
}cam;

typedef struct mouse_{
    ivec2 pos;
    ivec2 prev_pos;
    square* sq;
    square* map_sq;
}mouse;

typedef struct toolbar_{
  square* sq;
  square* tool[4];
}toolbar;

typedef struct level_{
  square* map[L_WIDTH][L_HEIGHT];
  unsigned long int time;
  int frame_time;
} level;

square* square_init(const char* file);

cam* cam_init();

mouse* mouse_init();

toolbar* toolbar_init();

level* level_init();