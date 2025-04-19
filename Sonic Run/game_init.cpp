#include "Game.h"
#include <iostream>
#include "Constants.h"

using namespace std;

bool Game::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL Init Error: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Sonic Run", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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
    backgroundMusic = Mix_LoadMUS("C:\\Sonic Run\\Sonic\\sound_game.mp3");
    if (backgroundMusic == nullptr) {
        std::cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
        Mix_PlayMusic(backgroundMusic, -1);
    }

    for (int i = 0; i < 3; ++i) {
        string path = "C:\\Sonic Run\\Sonic\\cloud" + to_string(i + 1) + ".png";
        clouds[i] = loadTexture(path.c_str(), renderer);
    }

    jumpSound = Mix_LoadWAV("C:\\Sonic Run\\Sonic\\sound_jump.mp3");
    if (jumpSound == nullptr) {
        std::cout << "Failed to load jump sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    background = loadTexture("C:\\Sonic Run\\Sonic\\background0.png", renderer);
    gameOverTexture = loadTexture("C:\\Sonic Run\\Sonic\\gameover.png", renderer);

    for (int i = 0; i < 7; i++) {
        string path = "C:\\Sonic Run\\Sonic\\sn" + to_string(i + 1) + ".png";
        sonicJumpFrames[i] = loadTexture(path.c_str(), renderer);
    }

    for (int i = 0; i < 6; i++) {
        string path = "C:\\Sonic Run\\Sonic\\sonic_run" + to_string(i + 1) + ".png";
        sonicRun[i] = loadTexture(path.c_str(), renderer);
    }
    for (int i = 0; i < 10; i++) {
        string path = "C:\\Sonic Run\\Sonic\\sn_" + to_string(i + 1) + ".png";
        sonicRoll[i] = loadTexture(path.c_str(), renderer);
    }
    for (int i = 0; i < 8; i++) {
        string path = "C:\\Sonic Run\\Sonic\\bird_white_" + to_string(i + 1) + ".png";
        birdFrames[i] = loadTexture(path.c_str(), renderer);
    }
    for (int i = 0; i < 8; i++) {
        string path = "C:\\Sonic Run\\Sonic\\bird_black_" + to_string(i + 1) + ".png";
        birdBlackFrames[i] = loadTexture(path.c_str(), renderer);
    }

    obstacles[0] = loadTexture("C:/Sonic Run/Sonic/vatcan1.png", renderer);
    obstacles[1] = loadTexture("C:/Sonic Run/Sonic/vatcan2.png", renderer);
    obstacles[2] = loadTexture("C:/Sonic Run/Sonic/vatcan3.png", renderer);
}
