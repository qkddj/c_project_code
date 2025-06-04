#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include "client_func.h"

int search_recipe_page(SDL_Window* window, SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    SDL_StartTextInput();

    SDL_Event event;
    int running = 1;
    int inputActive = 1;
    SDL_Rect inputRect = {150, 150, 350, 50};
    SDL_Rect backButtonRect;

    char inputBuffer[100] = "";
    int inputLen = 0;

    char suggestions[MAX_SUGGESTIONS][64];
    int suggestCount = 0;

    char lastValidSuggestions[MAX_SUGGESTIONS][64];
    int lastValidSuggestCount = 0;

    Recipe dummy;

    Uint32 lastInputTime = 0;
    Uint32 debounceDelay = 300;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    return 7;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (strlen(inputBuffer) > 0) {
                            printf("검색어 입력됨: %s\n", inputBuffer);
                            if (!load_recipe_by_name("recipes.csv", inputBuffer, &dummy, suggestions, &suggestCount)) {
                                printf("추천어 수: %d\n", suggestCount);
                            } else {
                                // TODO: 정확히 일치하는 레시피로 이동 구현 예정
                            }
                        }
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_StopTextInput();
                        TTF_CloseFont(font);
                        return 1;
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (inputLen > 0) {
                            inputBuffer[--inputLen] = '\0';
                            lastInputTime = SDL_GetTicks();
                            printf("입력됨: %s\n", inputBuffer);
                        }
                    }
                    break;

                case SDL_TEXTINPUT:
                    if (inputActive && inputLen + strlen(event.text.text) < sizeof(inputBuffer) - 1) {
                        strcat(inputBuffer, event.text.text);
                        inputLen += strlen(event.text.text);
                        lastInputTime = SDL_GetTicks();
                        printf("입력됨: %s\n", inputBuffer);
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;

                        if (isInside(mx, my, inputRect)) {
                            inputActive = 1;
                        } else {
                            inputActive = 0;
                        }

                        if (isInside(mx, my, backButtonRect)) {
                            SDL_StopTextInput();
                            TTF_CloseFont(font);
                            return 1;
                        }
                    }
                    break;
            }
        }

        if (lastInputTime && SDL_GetTicks() - lastInputTime > debounceDelay) {
            if (strlen(inputBuffer) > 0) {
                load_recipe_by_name("recipes.csv", inputBuffer, &dummy, suggestions, &suggestCount);

                if (suggestCount > 0) {
                    lastValidSuggestCount = suggestCount;
                    for (int i = 0; i < suggestCount; i++) {
                        strncpy(lastValidSuggestions[i], suggestions[i], 64);
                    }
                }
            } else {
                suggestCount = 0;
                lastValidSuggestCount = 0;  // <- 모두 지웠을 때 추천어도 제거
            }
            lastInputTime = 0;
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &inputRect);

        if (inputBuffer[0]) {
            SDL_Surface* surface = TTF_RenderUTF8_Solid(font, inputBuffer, (SDL_Color){255, 255, 255});
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {inputRect.x + 5, inputRect.y + 10, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        int displayCount = (suggestCount > 0) ? suggestCount : lastValidSuggestCount;
        char (*displaySuggestions)[64] = (suggestCount > 0) ? suggestions : lastValidSuggestions;

        if (strlen(inputBuffer) > 0) {
            for (int i = 0; i < displayCount; i++) {
                SDL_Rect bgRect = {inputRect.x + 10, 230 + i * 40, 330, 35};
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
                SDL_RenderFillRect(renderer, &bgRect);

                SDL_Surface* s = TTF_RenderUTF8_Solid(font, displaySuggestions[i], (SDL_Color){255, 255, 255});
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect r = {bgRect.x + 10, bgRect.y + 5, s->w, s->h};
                SDL_RenderCopy(renderer, t, NULL, &r);
                SDL_FreeSurface(s);
                SDL_DestroyTexture(t);
            }
        }

        SDL_Surface* labelSurface = TTF_RenderUTF8_Solid(font, "← 메뉴로", (SDL_Color){255, 255, 255});
        SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
        backButtonRect.x = 460;
        backButtonRect.y = 20;
        backButtonRect.w = labelSurface->w;
        backButtonRect.h = labelSurface->h;
        SDL_RenderCopy(renderer, labelTexture, NULL, &backButtonRect);
        SDL_FreeSurface(labelSurface);
        SDL_DestroyTexture(labelTexture);

        renderText(renderer, font, "레시피 이름이나 키워드를 입력해 주세요", 85, 100, NULL);

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    return 0;
}
