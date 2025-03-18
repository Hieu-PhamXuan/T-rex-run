#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;
const int GROUND_Y = 250;

bool init(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL không thể khởi tạo! Lỗi: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("T-Rex Run", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Không thể tạo cửa sổ! Lỗi: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != nullptr;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (!loadedSurface) {
        cout << "Không thể tải ảnh! Lỗi: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return texture;
}

void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* background, SDL_Texture* sonicJumpFrames[], SDL_Texture* sonicRun[], SDL_Texture* obstacles[], SDL_Texture* gameOverTexture) {
    SDL_DestroyTexture(background);
    for (int i = 0; i < 7; i++) {
        SDL_DestroyTexture(sonicJumpFrames[i]);
    }
    for (int i = 0; i < 6; i++) {
        SDL_DestroyTexture(sonicRun[i]);
    }
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(obstacles[i]);
    }
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

int main(int argc, char* argv[]) {
    srand(time(0));

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init(window, renderer)) return -1;

    SDL_Texture* background = loadTexture("D:\\Sonic\\background0.png", renderer);
    SDL_Texture* gameOverTexture = loadTexture("D:\\Sonic\\gameover.png", renderer);
    if (!background || !gameOverTexture) {
        close(window, renderer, background, nullptr, nullptr, nullptr, gameOverTexture);
        return -1;
    }

    SDL_Texture* sonicJumpFrames[7];
    for (int i = 0; i < 7; i++) {
        string path = "D:/Sonic/sn" + to_string(i + 1) + ".png";
        sonicJumpFrames[i] = loadTexture(path.c_str(), renderer);
        if (!sonicJumpFrames[i]) {
            close(window, renderer, background, sonicJumpFrames, nullptr, nullptr, gameOverTexture);
            return -1;
        }
    }

    SDL_Texture* sonicRun[6];
    for (int i = 0; i < 6; i++) {
        string path = "D:/Sonic/sonic_run" + to_string(i + 1) + ".png";
        sonicRun[i] = loadTexture(path.c_str(), renderer);
        if (!sonicRun[i]) {
            close(window, renderer, background, sonicJumpFrames, sonicRun, nullptr, gameOverTexture);
            return -1;
        }
    }

    SDL_Texture* obstacles[3];
    obstacles[0] = loadTexture("D:/Sonic/vatcan1.png", renderer);
    obstacles[1] = loadTexture("D:/Sonic/vatcan2.png", renderer);
    obstacles[2] = loadTexture("D:/Sonic/vatcan3.png", renderer);

    SDL_Rect backgroundPos1 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect backgroundPos2 = {SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_Rect sonicPos = {50, GROUND_Y, 100, 100};

    SDL_Rect obstaclePos = {SCREEN_WIDTH, GROUND_Y + 20, 60, 60};
    SDL_Texture* currentObstacle = obstacles[rand() % 3];
    SDL_Rect gameOverRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

    int obstacleGap = 20 + rand() % 50;

    bool quit = false;
    bool isGameOver = false;
    SDL_Event e;

    int velocityY = 0;
    bool isJumping = false;
    int jumpFrame = 0;
    bool jumpAnimationDone = false;

    int runFrame = 0;
    int obstacleSpeed = 6;
    int frameDelay = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && !isJumping) {
                isJumping = true;
                velocityY = -20;
                jumpFrame = 0;
                jumpAnimationDone = false;
            }
        }
        if (!isGameOver) {
            if (obstaclePos.x < -50) {
                obstaclePos.x = SCREEN_WIDTH + obstacleGap;
                currentObstacle = obstacles[rand() % 3];
            }

            if (checkCollision(sonicPos, obstaclePos)) {
                isGameOver = true;
            }
        }
        if (isGameOver) {
                SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
                SDL_RenderPresent(renderer);
                SDL_Delay(2000);
                quit = true;
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
            if (++frameDelay % 2 == 0) {
                runFrame = (runFrame + 1) % 6;
            }
        }

        backgroundPos1.x -= obstacleSpeed;
        backgroundPos2.x -= obstacleSpeed;

        if (backgroundPos1.x <= -SCREEN_WIDTH) backgroundPos1.x = backgroundPos2.x + SCREEN_WIDTH;
        if (backgroundPos2.x <= -SCREEN_WIDTH) backgroundPos2.x = backgroundPos1.x + SCREEN_WIDTH;

        obstaclePos.x -= obstacleSpeed;
        if (obstaclePos.x < -50) {
            obstaclePos.x = SCREEN_WIDTH + obstacleGap;
            obstacleGap = 20 + rand() % 50;
            currentObstacle = obstacles[rand() % 3];
            obstacleSpeed = min(obstacleSpeed + 1, 13);
        }

        if (checkCollision(sonicPos, obstaclePos)) {
                isGameOver = true;
            }


        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, &backgroundPos1);
        SDL_RenderCopy(renderer, background, NULL, &backgroundPos2);
        SDL_RenderCopy(renderer, sonicRun[runFrame], NULL, &sonicPos);
        SDL_RenderCopy(renderer, currentObstacle, NULL, &obstaclePos);
        SDL_RenderPresent(renderer);

        if (isGameOver) {
            SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
        }

        SDL_Delay(16);
    }

    close(window, renderer, background, sonicJumpFrames, sonicRun, obstacles, gameOverTexture);

    return 0;
}
