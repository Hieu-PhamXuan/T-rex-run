#include "Game.h"
#include <SDL2/SDL_mixer.h>

void Game::clean() {
    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = nullptr;
    for (auto& cloud : clouds) {
        SDL_DestroyTexture(cloud);
    }

    if (jumpSound != nullptr) {
        Mix_FreeChunk(jumpSound);
        jumpSound = nullptr;
    }
    Mix_CloseAudio();

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(gameOverTexture);
    for (auto& tex : sonicJumpFrames) SDL_DestroyTexture(tex);
    for (auto& tex : sonicRun) SDL_DestroyTexture(tex);
    for (auto& tex : obstacles) SDL_DestroyTexture(tex);
    for (auto& tex : birdFrames) SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
