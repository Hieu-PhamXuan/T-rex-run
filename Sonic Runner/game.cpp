#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

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

bool Game::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL Init Error: " << SDL_GetError() << endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "Failed to initialize SDL_ttf! TTF Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Sonic Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != nullptr;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "IMG_Load Error: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

//load resources
void Game::loadAssets() {
    backgroundMusic = Mix_LoadMUS("assets/sound_game.mp3");
    if (backgroundMusic == nullptr) {
        std::cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    else {
        Mix_PlayMusic(backgroundMusic, -1);
    }

    for (int i = 0; i < 3; ++i) {
        string path = "assets/cloud" + to_string(i + 1) + ".png";
        clouds[i] = loadTexture(path.c_str(), renderer);
    }

    jumpSound = Mix_LoadWAV("assets/sound_jump.mp3");
    if (jumpSound == nullptr) {
        std::cout << "Failed to load jump sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    background = loadTexture("assets/background0.png", renderer);
    gameOverTexture = loadTexture("assets/gameover.png", renderer);

    for (int i = 0; i < 7; i++) {
        string path = "assets/sn" + to_string(i + 1) + ".png";
        sonicJumpFrames[i] = loadTexture(path.c_str(), renderer);
    }

    for (int i = 0; i < 6; i++) {
        string path = "assets/sonic_run" + to_string(i + 1) + ".png";
        sonicRun[i] = loadTexture(path.c_str(), renderer);
    }
    for (int i = 0; i < 10; i++) {
        string path = "assets/sn_" + to_string(i + 1) + ".png";
        sonicRoll[i] = loadTexture(path.c_str(), renderer);
    }
    for (int i = 0; i < 8; i++) {
        string path = "assets/bird_white_" + to_string(i + 1) + ".png";
        birdFrames[i] = loadTexture(path.c_str(), renderer);
    }
    for (int i = 0; i < 8; i++) {
        string path = "assets/bird_black_" + to_string(i + 1) + ".png";
        birdBlackFrames[i] = loadTexture(path.c_str(), renderer);
    }

    font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font! TTF Error: " << TTF_GetError() << std::endl;
    }
    textColor = { 255, 255, 255 };
    scoreTexture = nullptr;
    scoreRect = { 10, 10, 0, 0 };
    score = 0;

    obstacles[0] = loadTexture("assets/vatcan1.png", renderer);
    obstacles[1] = loadTexture("assets/vatcan2.png", renderer);
    obstacles[2] = loadTexture("assets/vatcan3.png", renderer);
}

//collision
bool Game::checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y);
}

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

void Game::updateScoreTexture() {
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }

    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (textSurface) {
        scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        scoreRect.w = textSurface->w;
        scoreRect.h = textSurface->h;
        SDL_FreeSurface(textSurface);
    }
}

//logic game
void Game::update() {
    if (!isGameOver) {
        if (isRolling) {
            sonicPos.h = 50;
            sonicPos.w = 80;
            sonicPos.y = GROUND_Y + 30;
        }
        else if (!isJumping) {
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
                }
                else {
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
        }
        else {
            if (++runFrameDelay % 4 == 0) {
                runFrame = (runFrame + 1) % 6;
            }

            if (isRolling) {
                if (++rollFrameDelay % 4 == 0) {
                    rollFrame = (rollFrame + 1) % 10;
                }
            }
            else {
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
        score++;
        updateScoreTexture();
    }
}

//draw
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
    }
    else if (isJumping) {
        sonicJumpPos.y = sonicPos.y;
        sonicJumpPos.w = 100;
        sonicJumpPos.h = 100;
        SDL_RenderCopy(renderer, sonicJumpFrames[jumpFrame], NULL, &sonicJumpPos);
    }
    else {
        sonicPos.w = 80;
        sonicPos.h = 80;
        SDL_RenderCopy(renderer, sonicRun[runFrame], NULL, &sonicPos);
    }

    SDL_RenderCopy(renderer, currentObstacle, NULL, &obstaclePos);

    if (isGameOver)
        SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    }

    SDL_RenderPresent(renderer);
}

//clean
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

    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();

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

//reset
void Game::resetGame() {
    backgroundPos1 = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    backgroundPos2 = { SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    sonicPos = { 50, GROUND_Y, 80, 80 };
    sonicJumpPos = { 50, GROUND_Y, 100, 100 };
    obstaclePos = { SCREEN_WIDTH, GROUND_Y + 20, 60, 60 };
    gameOverRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    obstacleGap = 20 + rand() % 50;
    currentObstacle = obstacles[rand() % 3];
    obstacleSpeed = 6;
    runFrame = 0;
    jumpFrame = 0;
    velocityY = 0;
    rollFrame = 0;
    rollFrameDelay = 0;
    runFrameDelay = 0;
    birdFrame = 0;
    birdFrameDelay = 0;
    isJumping = false;
    jumpAnimationDone = false;
    score = 0;
    updateScoreTexture();

    for (int i = 0; i < 3; ++i) {
        cloudPos[i].x = SCREEN_WIDTH + i * 250;
        cloudPos[i].y = 50 + rand() % 120;
        cloudPos[i].w = 120;
        cloudPos[i].h = 40;
        cloudSpeed = 2;
    }
}

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


