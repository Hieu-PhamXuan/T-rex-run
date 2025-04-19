#include "Game.h"
#include <cstdlib>
#include <ctime>


using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;
const int GROUND_Y = 290;

Game::Game() : window(nullptr), renderer(nullptr), background(nullptr), gameOverTexture(nullptr),
               isRunning(true), isGameOver(false), isJumping(false), jumpAnimationDone(false),
               velocityY(0), jumpFrame(0), isBird(0), runFrame(0), frameDelay(0), obstacleSpeed(6), obstacleGap(0) {
    srand(static_cast<unsigned>(time(0)));
        runFrameDelay = 0;
        rollFrameDelay = 0;
        birdFloatDirection = 1;
        birdFloatDelay = 0;
        birdFloatTime = 0.0f;
}

Game::~Game() {
    clean();
}

