#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stddef.h>

#include "def.h"
#include "level.h"
#include "input.h"

/*FUNCTIONS*/


void update_surface(level* l, mouse* m, cam* c){
  m->pos.x -= c->offset.x;
  m->pos.y -= c->offset.y;
  l->map[m_sq(m->prev_pos.x, c)][m_sq(m->prev_pos.y, c)]->surface = m->map_sq->surface;
  m->map_sq->surface = l->map[m_sq(m->pos.x, c)][m_sq(m->pos.y, c)]->surface;
  l->map[m_sq(m->pos.x, c)][m_sq(m->pos.y, c)]->surface = m->sq->surface;
}

void update_view(level* l, mouse* m, cam* c, toolbar* t, SDL_Surface* screen){
  if(c->offset.x > c->o_min.x*c->scale){
    c->offset.x = c->o_min.x*c->scale;
  }
  if(c->offset.x < c->o_max.x*c->scale){
    c->offset.x = c->o_max.x*c->scale;
  }
  if(c->offset.y > c->o_min.y*c->scale){
    c->offset.y = c->o_min.y*c->scale;
  }
  if(c->offset.y < c->o_max.y*c->scale){
    c->offset.y = c->o_max.y*c->scale;
  }

  SDL_Rect old_rect = l->map[0][0]->rect;
  for (size_t i = 0; i < L_WIDTH; i++){
    for (size_t j = 0; j < L_HEIGHT; j++){
      l->map[i][j]->rect.h = (int)SQ_SIZE*c->scale;
      l->map[i][j]->rect.w = (int)SQ_SIZE*c->scale;
      l->map[i][j]->rect.x = (int)i*SQ_SIZE*c->scale + c->offset.x;
      l->map[i][j]->rect.y = (int)j*SQ_SIZE*c->scale + c->offset.y;
    }
  }
  if(old_rect.h != l->map[0][0]->rect.h || old_rect.x != l->map[0][0]->rect.x){
    //update_surface(l, m, c);
  }
  SDL_FillRect(screen, NULL, 4294967295); //int = 2^32-1: White

  SDL_BlitScaled(t->sq->surface, NULL, screen, &t->sq->rect);  

  for(int i = 0; i<L_WIDTH; i++){
    for (int j = 0; j < L_HEIGHT; j++){
      SDL_BlitScaled(l->map[i][j]->surface, NULL, screen, &l->map[i][j]->rect);
      if(DEBUG && !(i%16) && !(j%16)){printf("Blit: %d, %d\n", (int)i, (int)j);}
    }
  }
}

/*>>>>> MAIN <<<<<*/
int main() {
  SDL_Renderer* renderer = NULL;
  SDL_Window* window = NULL;
  SDL_Surface* screen = NULL;
  

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  screen = SDL_GetWindowSurface(window);

  SDL_Event event;
  
  level* l = level_init();
  cam* c = cam_init();
  mouse* m = mouse_init();
  toolbar* t = toolbar_init();

  m->sq = square_init("./factory.bmp");//remove once tools are setup


  /******************
   * MAIN GAME LOOP *
   * ^^^^ ^^^^ ^^^^ *
   ******************/

  while (!(event.type == SDL_QUIT)){
    update_clock(l);

    while(SDL_PollEvent(&event)){
      handle_input(event, l, m, c);
    }

    update_view(l, m, c, t, screen);
    SDL_UpdateWindowSurface(window);
  }//end of game loop
}
