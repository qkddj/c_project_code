#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include "client_func.h"  // user_key 등 외부 정의 사용 시 필요

// 마우스 좌표가 주어진 사각형 영역 안에 있는지 확인
int isInside(int x, int y, SDL_Rect rect) {
    return x >= rect.x && x <= rect.x + rect.w &&
           y >= rect.y && y <= rect.y + rect.h;
}

// 텍스트를 화면에 렌더링하고, 해당 텍스트의 사각형 영역 정보를 outRect에 저장
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Rect* outRect) {
    if (!renderer || !font || !text || text[0] == '\0') return;

    SDL_Color color = {255, 255, 255, 255};  // 흰색
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text, color);
    if (!surface) {
        printf("텍스트 surface 생성 실패: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("텍스처 생성 실패\n");
        SDL_FreeSurface(surface);
        return;
    }

    if (outRect) {
        outRect->x = x;
        outRect->y = y;
        outRect->w = surface->w;
        outRect->h = surface->h;
    }

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// 메인 메뉴 화면을 실행하고, 클릭된 메뉴 번호를 반환
int runMainMenuScreen(SDL_Window* window, SDL_Renderer* renderer) {
    typedef struct {
        SDL_Rect rect;        // 텍스트 위치 정보 저장용
        const char* label;    // 메뉴 라벨 문자열
    } MenuItem;

    int running = 1;
    SDL_Event event;

    // 폰트 로딩
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    // 메뉴 항목 초기화
    MenuItem menuItems[5] = {
        {{0}, "1. 식재료 보기"},
        {{0}, "2. 식재료 추가/제거"},
        {{0}, "3. 레시피 검색"},
        {{0}, "4. 요리 추천"},
        {{0}, "5. 로그아웃"}
    };

    int selectedMenu = -1;

    while (running) {
        // 이벤트 루프
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    selectedMenu = 7;  // 7번은 종료 의미로 사용
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;
                        // 메뉴 항목 중 클릭된 항목 찾기
                        for (int i = 0; i < 5; i++) {
                            if (isInside(mx, my, menuItems[i].rect)) {
                                printf("%s 클릭됨\n", menuItems[i].label);
                                selectedMenu = i + 2;  // 선택한 메뉴에 따라 2~6 리턴
                                running = 0;
                                break;
                            }
                        }
                    }
                    break;
            }
        }

        // 배경 초기화
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // 메뉴 항목 렌더링
        int startY = 120;
        for (int i = 0; i < 5; i++) {
            renderText(renderer, font, menuItems[i].label, 220, startY + i * 60, &menuItems[i].rect);
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    printf("선택된 메뉴: %d\n", selectedMenu);
    return selectedMenu;  // 선택된 메뉴 번호 반환
}
