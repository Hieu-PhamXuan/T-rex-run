#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <string>

class Game {
public:
    Game();
    ~Game();

    bool init();
    void loadAssets();
    void handleEvents();
    void update();
    void render();
    void clean();
    void resetGame();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    SDL_Texture* gameOverTexture;

    SDL_Texture* clouds[3];
    SDL_Texture* sonicJumpFrames[7];
    SDL_Texture* sonicRun[6];
    SDL_Texture* sonicRoll[10];
    SDL_Texture* birdFrames[8];
    SDL_Texture* birdBlackFrames[8];
    SDL_Texture* obstacles[3];
    SDL_Texture* currentObstacle;
    SDL_Texture** currentBirdFrames;

    Mix_Music* backgroundMusic;
    Mix_Chunk* jumpSound;

    SDL_Rect backgroundPos1, backgroundPos2;
    SDL_Rect cloudPos[3];
    SDL_Rect sonicPos, sonicJumpPos;
    SDL_Rect obstaclePos;
    SDL_Rect gameOverRect;

    SDL_Event e;

    bool isRunning;
    bool isGameOver;
    bool isJumping;
    bool isRolling;
    bool jumpAnimationDone;
    int velocityY;
    int frameDelay;
    int birdFloatDelay;
    int jumpFrame;
    int runFrame;
    int rollFrame;
    int runFrameDelay;
    int rollFrameDelay;
    int obstacleSpeed;
    int obstacleGap;
    int birdFrame;
    int birdFrameDelay;
    bool isBird;
    float birdFloatTime;
    int birdFloatDirection;
    int cloudSpeed;

    void handleJump();
    void handleRestart();
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);
};

#endif
