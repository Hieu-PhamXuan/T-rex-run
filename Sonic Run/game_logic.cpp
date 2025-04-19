#include "Game.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "Constants.h"

using namespace std;

bool Game::checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

void Game::update() {
    if (!isGameOver) {
        if (isRolling) {
            sonicPos.h = 50;
            sonicPos.w = 80;
            sonicPos.y = GROUND_Y + 30;
        } else if (!isJumping) {
            sonicPos.h = 80;
            sonicPos.w = 80;
            sonicPos.y = GROUND_Y;
        }

        if (checkCollision(sonicPos, obstaclePos)) {
            isGameOver = true;
        }

        backgroundPos1.x -= obstacleSpeed;
        backgroundPos2.x -= obstacleSpeed;

        if (backgroundPos1.x <= -SCREEN_WIDTH) backgroundPos1.x = backgroundPos2.x + SCREEN_WIDTH;
        if (backgroundPos2.x <= -SCREEN_WIDTH) backgroundPos2.x = backgroundPos1.x + SCREEN_WIDTH;

        obstaclePos.x -= obstacleSpeed;

        if (isBird) {
            if (++birdFrameDelay % 5 == 0) {
                birdFrame = (birdFrame + 1) % 8;
                currentObstacle = currentBirdFrames[birdFrame];
            }
        }

        if (isBird) {
            birdFloatTime += 0.1f;
            int baseY = GROUND_Y - 50;
            int amplitude = 20;
            obstaclePos.y = baseY + static_cast<int>(amplitude * sin(birdFloatTime));
        }

        if (obstaclePos.x + obstaclePos.w < 0) {
            obstaclePos.x = SCREEN_WIDTH + obstacleGap;
            obstacleGap = 20 + rand() % 150;

            int randIndex = rand() % 4;
            if (randIndex == 3) {
                int birdType = rand() % 2;
                birdFrame = 0;
                birdFrameDelay = 0;
                isBird = true;

                if (birdType == 0) {
                    currentBirdFrames = birdFrames;
                } else {
                    currentBirdFrames = birdBlackFrames;
                }

                currentObstacle = currentBirdFrames[birdFrame];
                obstaclePos.y = GROUND_Y - 30;
                obstaclePos.w = 100;
                obstaclePos.h = 50;
            }
            else {
                isBird = false;
                currentObstacle = obstacles[rand() % 3];
                obstaclePos.y = GROUND_Y + 20;
                obstaclePos.w = 60;
                obstaclePos.h = 60;
            }
            obstacleSpeed = std::min(obstacleSpeed + 1, 13);
        }

        if (isJumping) {
            sonicPos.y += velocityY;
            velocityY += 1;
            if (sonicPos.y >= GROUND_Y) {
                sonicPos.y = GROUND_Y;
                isJumping = false;
            }
            if (!jumpAnimationDone) {
                jumpFrame = min(jumpFrame + 1, 6);
                if (jumpFrame == 6) jumpAnimationDone = true;
            }
        } else {
            if (++runFrameDelay % 4 == 0) {
                runFrame = (runFrame + 1) % 6;
            }

            if (isRolling) {
                if (++rollFrameDelay % 4 == 0) {
                    rollFrame = (rollFrame + 1) % 10;
                }
            } else {
                rollFrameDelay = 0;
            }
        }
        for (int i = 0; i < 3; ++i) {
            cloudPos[i].x -= cloudSpeed;
            if (cloudPos[i].x + cloudPos[i].w < 0) {
                cloudPos[i].x = SCREEN_WIDTH + rand() % 100;
                cloudPos[i].y = 60 + rand() % 60;
            }
        }
    }
}

