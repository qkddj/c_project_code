#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "client_func.h"

int main(int argc, char* argv[]) {
    int page = 0;
    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        printf("TTF 초기화 실패: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Cse Restaurant", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        printf("윈도우 생성 실패: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("렌더러 생성 실패: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    while (running) {
        switch(page){
            case 0: {
                // 로그인 화면
                char* user_key = runLoginScreen(window, renderer);

                if (strcmp(user_key, "0") != 0) {
                    page = 1;
                }
                else {
                    running = 0;
                }
                break;
            }
            case 1: {
                // 메뉴 화면
                page = runMainMenuScreen(window, renderer);
                break;
            }
            case 6: {
                running = 0;
                break;
            }

        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
