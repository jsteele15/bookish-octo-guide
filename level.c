#include <SDL2/SDL.h>

#include "level.h"

square* square_init(const char* file){
  square* sq = (square*)calloc(1, sizeof(square));
  sq->surface = SDL_LoadBMP(file);
  return sq;
}

cam* cam_init(){
  cam* c = (cam*)calloc(1, sizeof(cam));
  c->speed = 10;

  c->o_max.x = -(L_WIDTH*16 - WIDTH);
  c->o_max.y = -(L_HEIGHT*16 - HEIGHT);

  c->scale = 1;
  c->s_min = 1;
  c->s_max = 4;
  return c;
}

mouse* mouse_init(){
  mouse* m = (mouse*)calloc(1, sizeof(mouse));
  m->pos.x = -1;
  m->pos.y = -1;
  m->map_sq = (square*)calloc(1, sizeof(square));
  m->sq =(square*)calloc(1, sizeof(square));
  return m;
}

toolbar* toolbar_init(){
  toolbar* t = (toolbar*)calloc(1, sizeof(toolbar));
  t->sq = (square*)calloc(1, sizeof(square));
  for(int i = 0; i < 4; i++){
    t->tool[i] = (square*)calloc(4, sizeof(square));
  }
  t->sq = square_init("./factory.bmp");
  t->sq->rect.h = HEIGHT;
  t->sq->rect.w = 40;
  return t;
}


level* level_init(){
  level* l = (level*)calloc(1, sizeof(level));
  for (size_t i = 0; i < L_WIDTH; i++){
      for (size_t j = 0; j < L_HEIGHT; j++){
          l->map[i][j] = square_init("./grass.bmp");
          l->map[i][j]->rect.x = i*SQ_SIZE;
          l->map[i][j]->rect.y = j*SQ_SIZE;
          l->map[i][j]->rect.w = SQ_SIZE;
          l->map[i][j]->rect.h = SQ_SIZE;
      }
  }
  return l;
}

int m_sq(int x, cam* c){ //Calc current mouse square
  return (x-x%SQ_SIZE*c->scale)/(SQ_SIZE*c->scale);
}

void update_clock(level* l){
  int prev_time = l->time;
  l->time = SDL_GetTicks();
  l->frame_time = (l->time - prev_time);
}
