#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;
const int GROUND_Y = 290;

Game::Game() : window(nullptr), renderer(nullptr), background(nullptr), backgroundNight(nullptr), gameOverTexture(nullptr),
isRunning(true), isGameOver(false), isJumping(false), jumpAnimationDone(false),
velocityY(0), jumpFrame(0), isBird(0), runFrame(0), frameDelay(0), obstacleSpeed(6), obstacleGap(0), lastTicks(0) {
    srand(static_cast<unsigned>(time(0)));
    runFrameDelay = 0;
    rollFrameDelay = 0;
    birdFloatDirection = 1;
    birdFloatDelay = 0;
    birdFloatTime = 0.0f;
	cloudSpeed = 2;
    isPaused = false;
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

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // tải con trỏ 
    SDL_Surface* cursorSurface = IMG_Load("C:/Sonic Runner/assets/mycursor.png");
    if (!cursorSurface) {
        std::cout << "Failed to load cursor image: " << IMG_GetError() << std::endl;
    }
    else {
        int newWidth = cursorSurface->w / 2;
        int newHeight = cursorSurface->h / 2;

        SDL_Surface* scaledSurface = SDL_CreateRGBSurface(0, newWidth, newHeight,
            cursorSurface->format->BitsPerPixel,
            cursorSurface->format->Rmask, cursorSurface->format->Gmask,
            cursorSurface->format->Bmask, cursorSurface->format->Amask);

        if (scaledSurface) {
            SDL_Rect srcRect = { 0, 0, cursorSurface->w, cursorSurface->h };
            SDL_Rect dstRect = { 0, 0, newWidth, newHeight };

            SDL_BlitScaled(cursorSurface, &srcRect, scaledSurface, &dstRect);

            menuCursor = SDL_CreateColorCursor(scaledSurface, 0, 0);
            SDL_FreeSurface(scaledSurface);
        }
        SDL_FreeSurface(cursorSurface);
    }

    // tạo cửa sổ game
    window = SDL_CreateWindow("Sonic Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    // tạo renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != nullptr;
}

//load texture
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "IMG_Load Error: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!tex) {
        cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
    }
    SDL_FreeSurface(surface);
    return tex;
}

//load resources
void Game::loadAssets() {
    menuButtonTexture = loadTexture("C:/Sonic Runner/assets/menu.png", renderer);
    playButtonTexture = loadTexture("C:/Sonic Runner/assets/play.png", renderer);
    exitButtonTexture = loadTexture("C:/Sonic Runner/assets/exit.png", renderer);

    playButtonRect = { (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 30, 200, 50 };
    exitButtonRect = { (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 90, 200, 50 };

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
	backgroundNight = loadTexture("C:/Sonic Runner/assets/background_night.png", renderer);
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

    font = TTF_OpenFont("assets/font.ttf", 22);
    highScoreFont = TTF_OpenFont("assets/font.ttf", 21);
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
    SDL_Rect shrinkA = { a.x + a.w / 8, a.y + a.h / 8, a.w * 3 / 4, a.h * 3 / 4 };
    SDL_Rect shrinkB = { b.x + b.w / 8, b.y + b.h / 8, b.w * 3 / 4, b.h * 3 / 4 };

    return (shrinkA.x < shrinkB.x + shrinkB.w && shrinkA.x + shrinkA.w > shrinkB.x &&
        shrinkA.y < shrinkB.y + shrinkB.h && shrinkA.y + shrinkA.h > shrinkB.y);
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

void Game::setMenuCursor() {
    if (menuCursor) {       
        SDL_SetCursor(menuCursor);
    }
}

void Game::setDefaultCursor() {
    if (defaultCursor) {
        SDL_SetCursor(defaultCursor);
    }
}


//control
void Game::handleEvents() {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) isRunning = false;

        if (isInMenu) {
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w &&
                    y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {

                    isInMenu = false;
                    resetGame();
                }
                else if (x >= exitButtonRect.x && x <= exitButtonRect.x + exitButtonRect.w &&
                    y >= exitButtonRect.y && y <= exitButtonRect.y + exitButtonRect.h) {
                    isRunning = false;
                }
            }
        }
        else {
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
                case SDLK_p:
                    isPaused = !isPaused;
                    break;
                }
            }

            if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_DOWN) {
                isRolling = false;
            }
        }
    }
}

//menu
void Game::renderMenu() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menuButtonTexture, NULL, NULL);
    SDL_RenderCopy(renderer, playButtonTexture, NULL, &playButtonRect);
    SDL_RenderCopy(renderer, exitButtonTexture, NULL, &exitButtonRect);
    SDL_RenderPresent(renderer);
}


//score 
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

//high score
void Game::updateHighScoreTexture() {
    if (highScoreTexture) {
        SDL_DestroyTexture(highScoreTexture);
        highScoreTexture = nullptr;
    }

    std::string hsText = "High Score: " + std::to_string(highScore);
    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(highScoreFont, hsText.c_str(), textColor);
    if (highScoreSurface) {
        highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
        highScoreRect = { 10, 40, highScoreSurface->w, highScoreSurface->h };
        SDL_FreeSurface(highScoreSurface);
    }
}

//logic game
void Game::update() {
    if (isPaused || isGameOver) {
        return;
    }
    if (!isGameOver) {
        static float timeElapsed = 0.0f;
        Uint32 currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;
        timeElapsed += deltaTime;

        //speed game
        if (timeElapsed >= 5.0f) {
            obstacleSpeed = std::min(obstacleSpeed + 1, 30);
            cloudSpeed += static_cast <int>(0.1f);
            timeElapsed = 0.0f;
        }

        if (score >= scoreThreshold) {
            scoreThreshold += 200; 
            isNight = !isNight;
        }

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

        backgroundPos1.x -= static_cast<int>(obstacleSpeed * deltaTime * 50);
        backgroundPos2.x -= static_cast<int>(obstacleSpeed * deltaTime * 50);

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
            obstacleGap = 50 + rand() % 151;

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
        //speed cloud
        for (int i = 0; i < 3; ++i) {
            cloudPos[i].x -= static_cast<int>(obstacleSpeed * 0.5f);
            if (cloudPos[i].x + cloudPos[i].w < 0) {
                cloudPos[i].x = SCREEN_WIDTH + rand() % 100;
                cloudPos[i].y = 60 + rand() % 60;
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastScoreUpdateTime >= 100) {
            score++;
            lastScoreUpdateTime = currentTime;
            updateScoreTexture();
        }
        if (isGameOver && score > highScore) {
            highScore = score;
            std::ofstream outFile("highscore.txt");
            if (outFile.is_open()) {
                outFile << highScore;
                outFile.close();
            }
            updateHighScoreTexture();
        }
    }
}

//draw
void Game::render() {
    SDL_RenderClear(renderer);

    if (isInMenu) {
        setMenuCursor();
        renderMenu();
        return;
    }
    else {
        setDefaultCursor();
    }

    if (isNight) {
        SDL_RenderCopy(renderer, background, NULL, &backgroundPos1);
        SDL_RenderCopy(renderer, background, NULL, &backgroundPos2);
    }
    else {
        SDL_RenderCopy(renderer, backgroundNight, NULL, &backgroundPos1);
        SDL_RenderCopy(renderer, backgroundNight, NULL, &backgroundPos2);
    }
    
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

    if (highScoreTexture) {
        SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);
    }

    if (isPaused) {
        SDL_Surface* pauseSurface = TTF_RenderText_Solid(font, "Press P to continue", { 255, 255, 0 });
        if (pauseSurface) {
            SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
            SDL_Rect pauseRect = { (SCREEN_WIDTH - pauseSurface->w) / 2, SCREEN_HEIGHT / 2 - 50, pauseSurface->w, pauseSurface->h };
            SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect);
            SDL_DestroyTexture(pauseTexture);
            SDL_FreeSurface(pauseSurface);
        }
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
    TTF_CloseFont(highScoreFont);
    TTF_Quit();

    SDL_DestroyTexture(background);
	SDL_DestroyTexture(backgroundNight);
    SDL_DestroyTexture(gameOverTexture);
    for (auto& tex : sonicJumpFrames) SDL_DestroyTexture(tex);
    for (auto& tex : sonicRun) SDL_DestroyTexture(tex);
    for (auto& tex : obstacles) SDL_DestroyTexture(tex);
    for (auto& tex : birdFrames) SDL_DestroyTexture(tex);
    for (auto& tex : birdBlackFrames) SDL_DestroyTexture(tex);
    for (auto& tex : sonicRoll) SDL_DestroyTexture(tex);
    if (highScoreTexture) SDL_DestroyTexture(highScoreTexture);

    if (menuCursor) SDL_FreeCursor(menuCursor);
    if (defaultCursor) SDL_FreeCursor(defaultCursor);

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
    isGameOver = false;
    isJumping = false;
    jumpAnimationDone = false;
    score = 0;
    startTime = SDL_GetTicks();
    lastScoreUpdateTime = startTime;
    updateScoreTexture();

    std::ifstream inFile("highscore.txt");
    if (inFile.is_open()) {
        inFile >> highScore;
        inFile.close();
    }
    else {
        highScore = 0;
    }

    updateHighScoreTexture();

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

    isInMenu = true;

    while (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}
