#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include <string>

class Game {
public:
    Game();
    ~Game();
    void run();
private:
    bool init();
    void loadAssets();
    void handleEvents();
    void handleJump();
    void handleRestart();
    void update();
    void render();
    void clean();
    void resetGame();
    void updateScoreTexture();
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);

    Mix_Music* backgroundMusic;
    Mix_Chunk* jumpSound;

    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* background;
    SDL_Texture* gameOverTexture;
    SDL_Texture* sonicJumpFrames[7];
    SDL_Texture* sonicRun[6];
    SDL_Texture* sonicRoll[10];
    SDL_Texture* obstacles[3];

    SDL_Rect backgroundPos1, backgroundPos2;
    SDL_Rect sonicPos;
    SDL_Rect sonicJumpPos;
    SDL_Rect obstaclePos;
    SDL_Rect gameOverRect;
    SDL_Texture* clouds[3];
    SDL_Rect cloudPos[3];
    SDL_Texture* currentObstacle;
    SDL_Texture* birdFrames[8];
    SDL_Texture* birdBlackFrames[8];
    SDL_Texture** currentBirdFrames;

    bool isRunning;
    bool isGameOver;
    bool isJumping;
    bool isRolling = false;
    bool isBird = false;
    bool jumpAnimationDone;

    int velocityY;
    int jumpFrame;
    int runFrame;
    int rollFrame = 0;
    int runFrameDelay;
    int rollFrameDelay;
    int frameDelay;
    int cloudSpeed;
    int obstacleSpeed;
    int obstacleGap;
    int birdFrame = 0;
    int birdFrameDelay = 0;
    int birdFloatDirection = 1;
    int birdFloatDelay = 0;
    float birdFloatTime;

    TTF_Font* font;
    SDL_Color textColor;
    SDL_Texture* scoreTexture;
    SDL_Rect scoreRect;
    int score;

    SDL_Event e;
};


