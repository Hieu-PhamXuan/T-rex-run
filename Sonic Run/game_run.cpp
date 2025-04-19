#include "Game.h"
#include <SDL2/SDL.h>

void Game::run() {
    if (!init()) return;
    loadAssets();
    resetGame();

    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}
