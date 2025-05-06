#pragma once
#include "Game.h"

void loadAssets(Game* game);
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
