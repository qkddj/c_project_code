#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "client_func.h"

#define SCROLL_SPEED 30
#define MAX_CHARS_PER_LINE 29  // 한 줄에 표시할 최대 문자 수 (UTF-8 기준)

// UTF-8 문자열을 안전하게 일정 길이로 잘라 여러 줄로 나누는 함수
int utf8_safe_split(const char* input, int maxChars, char lines[][512], int maxLines) {
    int count = 0;
    const char* ptr = input;
    while (*ptr && count < maxLines) {
        int charCount = 0;
        const char* lineStart = ptr;
        while (*ptr && charCount < maxChars) {
            unsigned char c = (unsigned char)*ptr;
            int charLen = 1;
            if (c >= 0xF0) charLen = 4;
            else if (c >= 0xE0) charLen = 3;
            else if (c >= 0xC0) charLen = 2;

            ptr += charLen;
            charCount++;
        }
        int byteLen = ptr - lineStart;
        strncpy(lines[count], lineStart, byteLen);
        lines[count][byteLen] = '\0';
        count++;
    }
    return count;
}

// 레시피 상세 정보 화면 출력 함수
int show_recipe_page(SDL_Window* window, SDL_Renderer* renderer, Recipe* recipe) {
    // 폰트 로딩
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 24);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    // 레시피 이미지 로드
    SDL_Texture* recipeImage = NULL;
    char imagePath[128];
    snprintf(imagePath, sizeof(imagePath), "image/%s.png", recipe->name);
    SDL_Surface* imageSurface = IMG_Load(imagePath);
    if (imageSurface) {
        recipeImage = SDL_CreateTextureFromSurface(renderer, imageSurface);
        SDL_FreeSurface(imageSurface);
    } else {
        printf("이미지 로딩 실패 또는 없음: %s\n", imagePath);
    }

    SDL_Event event;
    int running = 1;
    int scrollOffset = 0;  // 스크롤 위치
    SDL_Rect backButtonRect;  // ← 뒤로가기 버튼 위치

    while (running) {
        // 프레임 시간 측정 시작
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);

        // 이벤트 처리 루프
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return 1;  // 종료 요청 시 메인메뉴 종료 신호
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) return 0;  // ESC → 뒤로
                    else if (event.key.keysym.sym == SDLK_DOWN) scrollOffset -= SCROLL_SPEED;
                    else if (event.key.keysym.sym == SDLK_UP) scrollOffset += SCROLL_SPEED;
                    break;
                case SDL_MOUSEWHEEL:
                    scrollOffset += event.wheel.y * SCROLL_SPEED;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;
                        // 뒤로가기 버튼 클릭 처리
                        if (mx >= backButtonRect.x && mx <= backButtonRect.x + backButtonRect.w &&
                            my >= backButtonRect.y && my <= backButtonRect.y + backButtonRect.h) {
                            return 0;
                        }
                    }
                    break;
            }
        }

        // 스크롤 상한 제한
        if (scrollOffset > 0) scrollOffset = 0;

        // 배경 렌더링
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int y = 50 + scrollOffset;  // y 오프셋 적용 시작점

        // 레시피 이름
        renderText(renderer, font, recipe->name, 50, y, NULL);
        y += 50;

        // 재료 제목
        renderText(renderer, font, "[재료]", 50, y, NULL);
        y += 35;

        int ingredientStartY = y;
        int ingredientHeight = recipe->ingCount * 30;

        // 재료 목록 출력
        for (int i = 0; i < recipe->ingCount; i++) {
            renderText(renderer, font, recipe->ingredients[i], 70, y, NULL);
            y += 30;
        }

        // 재료 오른쪽에 이미지 출력
        if (recipeImage) {
            SDL_Rect imgRect = {
                330,                      // X 위치 (재료 오른쪽)
                ingredientStartY - 30,   // Y 위치는 재료 시작과 맞춤
                250, 250                 // 이미지 크기 고정
            };
            SDL_RenderCopy(renderer, recipeImage, NULL, &imgRect);
        }

        // 조리 절차 제목
        y = ingredientStartY + (ingredientHeight > 150 ? ingredientHeight : 150) + 20;
        renderText(renderer, font, "[조리 절차]", 50, y, NULL);
        y += 35;

        // 조리 절차 출력 (자동 줄바꿈 포함)
        char stepLines[10][512];
        for (int i = 0; i < recipe->stepCount; i++) {
            char numbered[512];
            snprintf(numbered, sizeof(numbered), "%d. %s", i + 1, recipe->steps[i]);

            int lines = utf8_safe_split(numbered, MAX_CHARS_PER_LINE, stepLines, 10);
            for (int j = 0; j < lines; j++) {
                renderText(renderer, font, stepLines[j], 70, y, NULL);
                y += 30;
            }
            y += 10;  // 절차 간 간격
        }

        // 키워드 제목
        y += 20;
        renderText(renderer, font, "[키워드]", 50, y, NULL);
        y += 35;

        // 키워드 목록 출력
        for (int i = 0; i < recipe->keywordCount; i++) {
            renderText(renderer, font, recipe->keywords[i], 70, y, NULL);
            y += 30;
        }

        // ← 뒤로가기 버튼 렌더링
        SDL_Surface* backSurface = TTF_RenderUTF8_Solid(font, "← 뒤로가기", (SDL_Color){255, 255, 255});
        SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
        backButtonRect.x = 460;
        backButtonRect.y = 20;
        backButtonRect.w = backSurface->w;
        backButtonRect.h = backSurface->h;
        SDL_RenderCopy(renderer, backTexture, NULL, &backButtonRect);
        SDL_FreeSurface(backSurface);
        SDL_DestroyTexture(backTexture);

        // 프레임 시간 측정
        QueryPerformanceCounter(&end);
        double elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
        printf("프레임 시간: %.3f초\n", elapsedTime);

        SDL_RenderPresent(renderer);  // 화면 업데이트
        SDL_Delay(16);  // 약 60fps 유지
    }

    // 리소스 정리
    if (recipeImage) SDL_DestroyTexture(recipeImage);
    TTF_CloseFont(font);
    return 0;
}
