#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "client_func.h"

#define SCROLL_SPEED 30
#define MAX_CHARS_PER_LINE 30  // 문자 수 기준으로 자르기

// UTF-8 문자 단위로 줄바꿈 처리
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

int show_recipe_page(SDL_Window* window, SDL_Renderer* renderer, Recipe* recipe) {
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 24);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Event event;
    int running = 1;
    int scrollOffset = 0;
    SDL_Rect backButtonRect;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: return 1;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
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
                        if (mx >= backButtonRect.x && mx <= backButtonRect.x + backButtonRect.w &&
                            my >= backButtonRect.y && my <= backButtonRect.y + backButtonRect.h) {
                            return 0;
                        }
                    }
                    break;
            }
        }

        if (scrollOffset > 0) scrollOffset = 0;

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int y = 50 + scrollOffset;
        renderText(renderer, font, recipe->name, 50, y, NULL);
        y += 50;

        renderText(renderer, font, "[재료]", 50, y, NULL);
        y += 35;
        for (int i = 0; i < recipe->ingCount; i++) {
            renderText(renderer, font, recipe->ingredients[i], 70, y, NULL);
            y += 30;
        }
        y += 20;

        renderText(renderer, font, "[조리 절차]", 50, y, NULL);
        y += 35;
        char stepLines[10][512];
        for (int i = 0; i < recipe->stepCount; i++) {
            char numbered[512];
            snprintf(numbered, sizeof(numbered), "%d. %s", i + 1, recipe->steps[i]);

            int lines = utf8_safe_split(numbered, MAX_CHARS_PER_LINE, stepLines, 10);
            for (int j = 0; j < lines; j++) {
                renderText(renderer, font, stepLines[j], 70, y, NULL);
                y += 30;
            }
            y += 10;  // ✅ 단계 간 여백 추가
        }

        y += 20;
        renderText(renderer, font, "[키워드]", 50, y, NULL);
        y += 35;
        for (int i = 0; i < recipe->keywordCount; i++) {
            renderText(renderer, font, recipe->keywords[i], 70, y, NULL);
            y += 30;
        }

        SDL_Surface* backSurface = TTF_RenderUTF8_Solid(font, "← 뒤로가기", (SDL_Color){255, 255, 255});
        SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
        backButtonRect.x = 460;
        backButtonRect.y = 20;
        backButtonRect.w = backSurface->w;
        backButtonRect.h = backSurface->h;
        SDL_RenderCopy(renderer, backTexture, NULL, &backButtonRect);
        SDL_FreeSurface(backSurface);
        SDL_DestroyTexture(backTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    return 0;
}
