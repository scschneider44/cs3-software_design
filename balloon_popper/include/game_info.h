#ifndef __GAME_INFO_H__
#define __GAME_INFO_H__
#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "vector.h"
#include "scene.h"
#include "sprite.h"
#include "text.h"

typedef struct gameInfo GameInfo;

void game_info_free(GameInfo* game_info);
GameInfo* game_info_init(Scene* scene, void* info, FreeFunc freer);
Scene* get_scene(GameInfo* game_info);
List* get_sprites(GameInfo* game_info);
List* get_text(GameInfo* game_info);
void* get_additional_info(GameInfo* game_info);
#endif // #ifndef __GAME_INFO_H__
