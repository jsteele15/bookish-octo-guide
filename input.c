void handle_input(SDL_event event){
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
}