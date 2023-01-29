#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/*DEFINITIONS*/
#define L_HEIGHT 64 //level height
#define L_WIDTH 64 //level width
#define HEIGHT 500 
#define WIDTH 700 
#define SQ_SIZE 16

int DEBUG = 0;

/*STRUCTURES & FUNCS*/

typedef struct ivec2_{
  int x;
  int y;
}ivec2;

typedef struct fvec2_{
  float x;
  float y;
}fvec2;

typedef struct square_{
    struct square_* above;
    struct square_* below;
    struct square_* left;
    struct square_* right;
    SDL_Surface* surface;
    SDL_Rect rect;
}square;

square* square_init(const char* file){
  square* sq = (square*)calloc(1, sizeof(square));
  sq->surface = SDL_LoadBMP(file);
  return sq;
}

typedef struct cam_{
  int scale;
  fvec2 offset;
  ivec2 o_min;
  ivec2 o_max;
  float speed;
  int s_min;
  int s_max;
  
}cam;

cam* cam_init(){
  cam* c = (cam*)calloc(1, sizeof(cam));
  c->speed = .5;

  c->o_max.x = -(L_WIDTH*16 - WIDTH);
  c->o_max.y = -(L_HEIGHT*16 - HEIGHT);

  c->scale = 1;
  c->s_min = 1;
  c->s_max = 4;
  return c;
}

typedef struct mouse_{
    ivec2 pos;
    ivec2 prev_pos;
    square* sq;
    square* map_sq;
  }mouse;

mouse* mouse_init(){
  mouse* m = (mouse*)calloc(1, sizeof(mouse));
  m->pos.x = -1;
  m->pos.y = -1;
  m->map_sq = (square*)calloc(1, sizeof(square));
  m->sq =(square*)calloc(1, sizeof(square));
  return m;
}

typedef struct toolbar_{
  square* sq;
  square* tool[4];
}toolbar;

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

typedef struct level_{
  square* map[L_WIDTH][L_HEIGHT];
  unsigned long int time;
  int frame_time;
} level;

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
/*FUNCTIONS*/

int m_sq(int x, cam* c){ //Calc current mouse square
  return (x-x%SQ_SIZE*c->scale)/(SQ_SIZE*c->scale);
}

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

  SDL_BlitScaled(t->sq->surface, NULL, screen, &t->sq->rect); //SEGFAULT 

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

  m->sq = square_init("./factory.bmp");

  int shift = 0;
  int zoom_in = 0;
  int zoom_out = 0;

  /******************
   * MAIN GAME LOOP *
   * ^^^^ ^^^^ ^^^^ *
   ******************/

  while (!(event.type == SDL_QUIT)){
    int prev_time = l->time;
    l->time = SDL_GetTicks();
    l->frame_time = (l->time - prev_time);

    SDL_PollEvent(&event); //Get events 
    switch(event.type){
      case SDL_MOUSEMOTION:
        if(m->map_sq->surface == NULL){
          m->map_sq->surface = l->map[m_sq(m->pos.x, c)][m_sq(m->pos.y, c)]->surface;
        }
        m->prev_pos = m->pos;
        m->pos.x = event.motion.x;
        m->pos.y = event.motion.y;
        update_surface(l, m, c);
        break;

      case SDL_MOUSEBUTTONDOWN:
        l->map[m_sq(m->pos.x, c)][m_sq(m->pos.y, c)]->surface = SDL_LoadBMP("./factory.bmp");
        m->map_sq->surface = l->map[m_sq(m->pos.x, c)][m_sq(m->pos.y, c)]->surface;
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym){
          case SDLK_EQUALS:
            zoom_in = 1;
            break;

          case SDLK_MINUS:
            zoom_out = 1;
            break;

          case SDLK_RIGHT:
            if(c->offset.x > c->o_max.x*c->scale){
              c->offset.x -= c->speed*l->frame_time;
            }
            break;

          case SDLK_LEFT:
            if(c->offset.x < c->o_min.x*c->scale){
              c->offset.x += c->speed*l->frame_time;
            }
            break;

          case SDLK_UP:
            if(c->offset.y < c->o_min.y*c->scale){
              c->offset.y += c->speed*l->frame_time;
            }
            break;

          case SDLK_DOWN:
            if(c->offset.y > c->o_max.y*c->scale){
              c->offset.y -= c->speed*l->frame_time;
            }
            break;

          case SDLK_LSHIFT:
            shift = 1;
            break;

          case SDLK_RSHIFT:
            shift = 1;
            break;
        }
        break;

      case SDL_KEYUP:
        switch (event.key.keysym.sym){
          case SDLK_EQUALS:
            if(zoom_in && c->s_max > c->scale){
              c->scale += 1;
              //to-do: Fix zoom texture glitch
              zoom_in = 0; 
            }
            break;

          case SDLK_MINUS:
            if(zoom_out && c->s_min < c->scale ){
              c->scale -= 1;
              zoom_out = 0;
            }
            break;

          case SDLK_LSHIFT:
            shift = 0;
            break;

          case SDLK_RSHIFT:
            shift = 0;
            break;
        }
        break;
    }

    update_view(l, m, c, t, screen);
    SDL_UpdateWindowSurface(window);
  }//end of game loop
}
