#include "text.h"
#include "sdl_wrapper.h"
#include <assert.h>
struct text {
    SDL_Texture* texture;
    Vector position;
    int width;
    int height;
    char* text;
    int font_size;
    char* font_type;
    RGBColor color;
};

Text* text_init(Vector position, int w, int h, char* text, int font_size, \
    char* font_type, RGBColor color) {
    SDL_Renderer* renderer = get_renderer();
    SDL_Color col = {color.r * 255, color.g * 255, color.b * 255};
    TTF_Font* font = TTF_OpenFont(font_type, font_size);
    if (!font) {
        printf("text_init failed: %s\n", SDL_GetError());
    }
    assert(font);
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, col);

    if (!surface) {
        printf("text_init failed: %s\n", SDL_GetError());
    }
    assert(surface);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    assert(texture);
    SDL_FreeSurface(surface);
    Text* t = malloc(sizeof(Text));
    assert(t);
    t->texture = texture;
    t->position = position;
    t->width = w;
    t->height = h;
    t->text = text;
    t->font_size = font_size;
    t->font_type = font_type;
    t->color = color;
    return t;
}

void free_text(void *w) {
    Text* t = w;
    SDL_DestroyTexture(t->texture);
    //free(t->text);
    free(t);
}


SDL_Texture* get_texture_text(Text* t) {
    assert(t);
    return t->texture;
}

Vector get_position_text(Text *t) {
    assert(t);
    return t->position;
}

int get_width_text(Text* t) {
    assert(t);
    return t->width;
}

int get_height_text(Text* t) {
    assert(t);
    return t->height;
}

int get_font_size(Text* t) {
    assert(t);
    return t->font_size;
}
