#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include "client_func.h"  // load_recipe_by_name, show_recipe_page 등 외부 함수 정의 필요

// 레시피 검색 화면을 실행하고, 검색 결과 또는 뒤로 가기 동작을 처리함
int search_recipe_page(SDL_Window* window, SDL_Renderer* renderer) {
    // 폰트 로드
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    SDL_StartTextInput();  // 텍스트 입력 시작

    SDL_Event event;
    int running = 1;
    int inputActive = 1;

    // 입력창 및 뒤로가기 버튼 위치 정의
    SDL_Rect inputRect = {150, 150, 350, 50};
    SDL_Rect backButtonRect;

    char inputBuffer[100] = "";    // 사용자 입력 버퍼
    int inputLen = 0;

    // 추천어 및 결과 저장용 변수
    char suggestions[MAX_SUGGESTIONS][64];
    int suggestCount = 0;

    // 마지막으로 유효했던 추천어 저장 (입력 초기화되면 복원용)
    char lastValidSuggestions[MAX_SUGGESTIONS][64];
    int lastValidSuggestCount = 0;

    Recipe dummy;  // 검색 결과 임시 저장용 구조체

    // 디바운싱: 입력 후 일정 시간 기다린 뒤 추천어 로딩
    Uint32 lastInputTime = 0;
    Uint32 debounceDelay = 300;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    return 7;  // 프로그램 종료 명령

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        // 엔터 입력 시 검색 실행
                        if (strlen(inputBuffer) > 0) {
                            printf("검색어 입력됨: %s\n", inputBuffer);
                            if (load_recipe_by_name("recipes.csv", inputBuffer, &dummy, suggestions, &suggestCount)) {
                                // 검색 결과가 존재하면 상세 페이지로 이동
                                if (show_recipe_page(window, renderer, &dummy)) {
                                    SDL_StopTextInput();
                                    TTF_CloseFont(font);
                                    running = 0;
                                    return 7;
                                }
                            } else {
                                printf("추천어 수: %d\n", suggestCount);
                            }
                        }
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        // ESC 키로 뒤로가기
                        SDL_StopTextInput();
                        TTF_CloseFont(font);
                        return 1;
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        // 백스페이스 입력
                        if (inputLen > 0) {
                            inputBuffer[--inputLen] = '\0';
                            lastInputTime = SDL_GetTicks();  // 디바운싱 초기화
                            printf("입력됨: %s\n", inputBuffer);
                        }
                    }
                    break;

                case SDL_TEXTINPUT:
                    // 텍스트 입력
                    if (inputActive && inputLen + strlen(event.text.text) < sizeof(inputBuffer) - 1) {
                        strcat(inputBuffer, event.text.text);
                        inputLen += strlen(event.text.text);
                        lastInputTime = SDL_GetTicks();  // 디바운싱 초기화
                        printf("입력됨: %s\n", inputBuffer);
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;

                        // 입력창 클릭 시 포커스 활성화
                        if (isInside(mx, my, inputRect)) {
                            inputActive = 1;
                        } else {
                            inputActive = 0;
                        }

                        // 뒤로가기 버튼 클릭 시 종료
                        if (isInside(mx, my, backButtonRect)) {
                            SDL_StopTextInput();
                            TTF_CloseFont(font);
                            return 1;
                        }
                    }
                    break;
            }
        }

        // 디바운싱: 마지막 입력 이후 일정 시간이 지났을 때 추천어 갱신
        if (lastInputTime && SDL_GetTicks() - lastInputTime > debounceDelay) {
            if (strlen(inputBuffer) > 0) {
                load_recipe_by_name("recipes.csv", inputBuffer, &dummy, suggestions, &suggestCount);

                if (suggestCount > 0) {
                    // 유효한 추천어 복사해두기 (빈 입력 시 보존)
                    lastValidSuggestCount = suggestCount;
                    for (int i = 0; i < suggestCount; i++) {
                        strncpy(lastValidSuggestions[i], suggestions[i], 64);
                    }
                }
            } else {
                // 입력이 모두 지워졌을 경우 추천어도 제거
                suggestCount = 0;
                lastValidSuggestCount = 0;
            }
            lastInputTime = 0;
        }

        // 화면 초기화
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // 입력창 렌더링
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &inputRect);

        // 입력된 문자열 렌더링
        if (inputBuffer[0]) {
            SDL_Surface* surface = TTF_RenderUTF8_Solid(font, inputBuffer, (SDL_Color){255, 255, 255});
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {inputRect.x + 5, inputRect.y + 10, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        // 추천어 목록 렌더링 (현재 추천이 있으면 그것을, 없으면 마지막 추천어를 사용)
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

        // 뒤로가기 버튼 렌더링
        SDL_Surface* labelSurface = TTF_RenderUTF8_Solid(font, "← 메뉴로", (SDL_Color){255, 255, 255});
        SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
        backButtonRect.x = 460;
        backButtonRect.y = 20;
        backButtonRect.w = labelSurface->w;
        backButtonRect.h = labelSurface->h;
        SDL_RenderCopy(renderer, labelTexture, NULL, &backButtonRect);
        SDL_FreeSurface(labelSurface);
        SDL_DestroyTexture(labelTexture);

        // 안내 문구 렌더링
        renderText(renderer, font, "레시피 이름을 입력해 주세요", 160, 100, NULL);

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    return 0;
}
