#include "game_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
typedef struct gameInfo {
    Scene* scene;
    List* sprites;
    List* text;
    void* additional_info;
    FreeFunc free;
} GameInfo;

void game_info_free(GameInfo* game_info) {
    scene_free(game_info->scene);
    list_free(game_info->sprites);
    list_free(game_info->text);
    if (game_info->free) {
        game_info->free(game_info->additional_info);
    }
    free(game_info);
}

GameInfo* game_info_init(Scene* scene, void* info, FreeFunc freer) {
    GameInfo* gameInfo = malloc(sizeof(GameInfo));
    assert(gameInfo);
    gameInfo->scene = scene;
    gameInfo->sprites = list_init(1, free_sprite);
    gameInfo->text = list_init(1, free_text);
    gameInfo->additional_info = info;
    gameInfo->free = freer;
    return gameInfo;
}

Scene* get_scene(GameInfo* game_info) {
    assert(game_info);
    return game_info->scene;
}

List* get_sprites(GameInfo* game_info) {
    assert(game_info);
    return game_info->sprites;
}

List* get_text(GameInfo* game_info) {
    assert(game_info);
    return game_info->text;
}

void* get_additional_info(GameInfo* game_info) {
    assert(game_info);
    return game_info->additional_info;
}
