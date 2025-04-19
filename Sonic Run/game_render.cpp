#include "Game.h"

void Game::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, &backgroundPos1);
    SDL_RenderCopy(renderer, background, NULL, &backgroundPos2);
    for (int i = 0; i < 3; ++i) {
        SDL_RenderCopy(renderer, clouds[i], NULL, &cloudPos[i]);
    }

    if (isRolling) {
        sonicJumpPos.y = sonicPos.y;
        sonicJumpPos.w = 80;
        sonicJumpPos.h = 60;
        SDL_RenderCopy(renderer, sonicRoll[rollFrame], NULL, &sonicJumpPos);
    } else if (isJumping) {
        sonicJumpPos.y = sonicPos.y;
        sonicJumpPos.w = 100;
        sonicJumpPos.h = 100;
        SDL_RenderCopy(renderer, sonicJumpFrames[jumpFrame], NULL, &sonicJumpPos);
    } else {
        sonicPos.w = 80;
        sonicPos.h = 80;
        SDL_RenderCopy(renderer, sonicRun[runFrame], NULL, &sonicPos);
    }

    SDL_RenderCopy(renderer, currentObstacle, NULL, &obstaclePos);

    if (isGameOver)
        SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

    SDL_RenderPresent(renderer);
}
