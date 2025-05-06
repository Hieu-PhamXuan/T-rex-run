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
    void clean();

    void handleEvents();
    void handleJump();
    void handleRestart();

    void update();
    void render();
    void updateScoreTexture();
    void updateHighScoreTexture();
    void renderMenu();
    void setMenuCursor();
    void setDefaultCursor();

    void resetGame();
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);

    Mix_Music* backgroundMusic;
    Mix_Chunk* jumpSound;

    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Cursor* menuCursor = nullptr;
    SDL_Cursor* defaultCursor = nullptr;

    SDL_Texture* background;
    SDL_Texture* backgroundNight;
    SDL_Texture* gameOverTexture;
    SDL_Texture* sonicJumpFrames[7];
    SDL_Texture* sonicRun[6];
    SDL_Texture* sonicRoll[10];
    SDL_Texture* obstacles[3];
    SDL_Texture* clouds[3];
    SDL_Texture* birdFrames[8];
    SDL_Texture* birdBlackFrames[8];
    SDL_Texture** currentBirdFrames;
    SDL_Texture* scoreTexture;
    SDL_Texture* highScoreTexture;
    SDL_Texture* currentObstacle;
    SDL_Texture* playButtonTexture;
    SDL_Texture* exitButtonTexture;
    SDL_Texture* menuButtonTexture;

    SDL_Rect backgroundPos1, backgroundPos2;
    SDL_Rect sonicPos;
    SDL_Rect sonicJumpPos;
    SDL_Rect obstaclePos;
    SDL_Rect gameOverRect;
    SDL_Rect cloudPos[3];
    SDL_Rect scoreRect;
    SDL_Rect highScoreRect;
    SDL_Rect playButtonRect;
    SDL_Rect exitButtonRect;
    SDL_Rect menuButtonRect;

    bool isInMenu;
    bool isRunning;
    bool isGameOver;
    bool isJumping;
    bool isRolling = false;
    bool isBird = false;
    bool isNight = false;
    int scoreThreshold = 0;
    bool jumpAnimationDone;
    bool isPaused;

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
    float deltaTime;
    float birdFloatTime;
    Uint32 startTime;
    Uint32 lastTime;
    Uint32 lastTicks;
    Uint32 lastScoreUpdateTime;

    int score;
    int highScore;

    TTF_Font* font;
    TTF_Font* highScoreFont;
    SDL_Color textColor;

    SDL_Event e;
};