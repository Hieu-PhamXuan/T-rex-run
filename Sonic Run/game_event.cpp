#include "Game.h"

void Game::handleJump() {
    if (!isJumping && !isGameOver) {
        isJumping = true;
        velocityY = -20;
        jumpFrame = 0;
        jumpAnimationDone = false;
        Mix_PlayChannel(-1, jumpSound, 0);
    }
}

void Game::handleRestart() {
    if (isGameOver) {
        resetGame();
        isGameOver = false;
    }
}

//control
void Game::handleEvents() {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) isRunning = false;

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    handleJump();
                    break;
                case SDLK_SPACE:
                    handleRestart();
                    break;
                case SDLK_DOWN:
                    if (!isJumping) {
                        isRolling = true;
                        rollFrame = 0;
                    }
                    break;
            }
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_DOWN) {
            isRolling = false;
        }
    }
}
