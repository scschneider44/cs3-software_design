#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include "vector.h"
#include "color.h"

typedef struct text Text;
Text* text_init(Vector position, int w, int h, char* text, int font_size, \
    char* font_type, RGBColor color);
void free_text(void *w);
SDL_Texture* get_texture_text(Text* t);
Vector get_position_text(Text* t);
int get_height_text(Text* t);
int get_width_text(Text* t);
int get_font_size_text(Text* t);
#endif // #ifndef __TEXT_H__
