#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "vector.h"


typedef struct sprite Sprite;
Sprite* sprite_init(Vector position, char* filename, int w, int h);
void free_sprite(void *sprite);
SDL_Texture* get_texture_sprite(Sprite* s);
Vector get_position_sprite(Sprite *s);
int get_width_sprite(Sprite* s);
int get_height_sprite(Sprite* s);
#endif // #ifndef __SPRITE_H__
