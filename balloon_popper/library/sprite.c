#include "sprite.h"
#include "sdl_wrapper.h"
#include <assert.h>

struct sprite {
    SDL_Texture* texture;
    Vector position;
    int width;
    int height;
};

Sprite* sprite_init(Vector position, char* filename, int w, int h) {
    SDL_Renderer* renderer = get_renderer();
    SDL_Surface *image = SDL_LoadBMP(filename);
    if (!image) {
        printf("show_sprite failed: %s\n", SDL_GetError());
    }
    assert(image);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    assert(texture);
    SDL_FreeSurface(image);
    Sprite* s = malloc(sizeof(Sprite));
    assert(s);
    s->texture = texture;
    s->position = position;
    s->width = w;
    s->height = h;
    return s;
}

void free_sprite(void *sprite) {
    Sprite* s = sprite;
    SDL_DestroyTexture(s->texture);
    free(s);
}

SDL_Texture* get_texture_sprite(Sprite* s) {
    assert(s);
    return s->texture;
}

Vector get_position_sprite(Sprite *s) {
    assert(s);
    return s->position;
}

int get_width_sprite(Sprite* s) {
    assert(s);
    return s->width;
}

int get_height_sprite(Sprite* s) {
    assert(s);
    return s->height;
}
