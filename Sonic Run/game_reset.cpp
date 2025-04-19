#include "Game.h"
#include <cstdlib>
#include "Constants.h"

void Game::resetGame() {
    backgroundPos1 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    backgroundPos2 = {SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    sonicPos = {50, GROUND_Y, 80, 80};
    sonicJumpPos = {50, GROUND_Y, 100, 100};
    obstaclePos = {SCREEN_WIDTH, GROUND_Y + 20, 60, 60};
    gameOverRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    obstacleGap = 20 + rand() % 50;
    currentObstacle = obstacles[rand() % 3];
    obstacleSpeed = 6;
    runFrame = 0;
    jumpFrame = 0;
    velocityY = 0;
    isRolling = false;
    rollFrame = 0;
    rollFrameDelay = 0;
    runFrameDelay = 0;
    birdFrame = 0;
    birdFrameDelay = 0;
    isJumping = false;
    jumpAnimationDone = false;
    for (int i = 0; i < 3; ++i) {
        cloudPos[i].x = SCREEN_WIDTH + i * 250;
        cloudPos[i].y = 50 + rand() % 120;
        cloudPos[i].w = 120;
        cloudPos[i].h = 40;
        cloudSpeed = 2;
    }
}
