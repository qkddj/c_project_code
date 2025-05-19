#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include "client_func.h"  // user_key 등 외부 정의 사용 시 필요

// 메뉴 선택 영역 안에 클릭이 들어왔는지 판별
int isInside(int x, int y, SDL_Rect rect) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

// 텍스트 렌더링 및 위치 정보 저장
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Rect* outRect) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    outRect->x = x;
    outRect->y = y;
    outRect->w = surface->w;
    outRect->h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, outRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// 메인 메뉴 실행 함수
int runMainMenuScreen(SDL_Window* window, SDL_Renderer* renderer) {
    typedef struct {
        SDL_Rect rect;
        const char* label;
    } MenuItem;

    int running = 1;
    SDL_Event event;

    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    MenuItem menuItems[5] = {
        {{0}, "1. 식재료 보기"},
        {{0}, "2. 식재료 추가/제거"},
        {{0}, "3. 레시피 검색"},
        {{0}, "4. 요리 추천"},
        {{0}, "5. 종료"}
    };

    int selectedMenu = -1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    selectedMenu = 6;  // 종료
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;
                        for (int i = 0; i < 5; i++) {
                            if (isInside(mx, my, menuItems[i].rect)) {
                                printf("%s 클릭됨\n", menuItems[i].label);
                                selectedMenu = i+2;
                                running = 0;
                                break;
                            }
                        }
                    }
                    break;
            }
        }

        // 화면 초기화
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // 메뉴 텍스트 출력
        int startY = 120;
        for (int i = 0; i < 5; i++) {
            renderText(renderer, font, menuItems[i].label, 220, startY + i * 60, &menuItems[i].rect);
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    printf("선택된 메뉴: %d\n", selectedMenu);
    return selectedMenu;  // 선택된 메뉴 인덱스 반환
}
