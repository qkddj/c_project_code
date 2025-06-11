#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "client_func.h"

int main(int argc, char* argv[]) {
    int page = 0;         // 현재 페이지 상태를 저장 (0: 로그인, 1: 메뉴 등)
    int running = 1;      // 프로그램 실행 상태 (0이면 종료)

    char* user_key = NULL; // 로그인한 사용자 정보를 저장하는 포인터

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError());
        return 1;
    }

    // TTF (폰트 렌더링) 초기화
    if (TTF_Init() < 0) {
        printf("TTF 초기화 실패: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // 윈도우 생성
    SDL_Window* window = SDL_CreateWindow("Cse Restaurant", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        printf("윈도우 생성 실패: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // 렌더러 생성
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("렌더러 생성 실패: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // 메인 루프: 페이지 번호에 따라 화면 전환
    while (running) {
        switch(page){
            case 0: {
                // 로그인 화면 실행
                user_key = runLoginScreen(window, renderer);

                // "0"이 아니면 로그인 성공, 다음 페이지로
                if (strcmp(user_key, "0") != 0) {
                    page = 1;  // 메인 메뉴로 이동
                }
                else {
                    running = 0;  // 프로그램 종료
                }
                break;
            }
            case 1: {
                // 메인 메뉴 화면 실행 (리턴된 값에 따라 다음 페이지 결정)
                page = runMainMenuScreen(window, renderer);
                break;
            }
            case 2: {
                // 식재료 목록 보기 화면 실행
                page = runIngredientScreen(window, renderer, user_key);
                break;
            }
            case 3: {
                // 식재료 추가/삭제 화면 실행
                page = add_Remove_Ingredients(window, renderer, user_key);
                break;
            }
            case 4: {
                // 레시피 검색 화면 실행
                page = search_recipe_page(window, renderer);
                break;
            }
            case 5: {
                // 키워드를 입력 받아 추천 레시피를 보여주는 화면 실행
                page = keyword_input_screen(window, renderer, user_key);
                break;
            }
            case 6: {
                // 로그아웃 처리 → 로그인 화면으로 이동
                page = 0;
                break;
            }
            case 7: {
                // 종료 선택 시 루프 종료
                running = 0;
                break;
            }
        }
    }

    // 리소스 해제 및 종료 처리
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
