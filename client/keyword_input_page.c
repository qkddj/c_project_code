// ingredient_screen.c
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client_func.h"

// 추천 키워드를 입력받는 화면 처리 함수
int keyword_input_screen(SDL_Window* window, SDL_Renderer* renderer, const char* user_key) {
    // 폰트 로딩
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return 1;
    }

    SDL_StartTextInput();  // 텍스트 입력 활성화

    SDL_Event event;
    int running = 1;
    SDL_Rect inputRect = {150, 150, 350, 50};  // 입력창 위치
    SDL_Rect backButtonRect;  // ← 메뉴로 버튼 위치 저장용

    char inputBuffer[100] = "";  // 사용자 입력 저장
    int inputLen = 0;  // 입력 길이

    // 메인 이벤트 루프
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    SDL_StopTextInput();
                    TTF_CloseFont(font);
                    return 7;  // 종료

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (strlen(inputBuffer) > 0) {
                            if(show_recommendations(window, renderer, inputBuffer, user_key)) {
                                SDL_StopTextInput();
                                TTF_CloseFont(font);
                                return 7;  // 추천 결과 화면에서 종료 요청 시
                            }
                        }
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_StopTextInput();
                        TTF_CloseFont(font);
                        return 1;  // 메뉴로 복귀
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (inputLen > 0) {
                            inputBuffer[--inputLen] = '\0';
                        }
                    }
                    break;

                case SDL_TEXTINPUT:
                    if (inputLen + strlen(event.text.text) < sizeof(inputBuffer) - 1) {
                        strcat(inputBuffer, event.text.text);
                        inputLen += strlen(event.text.text);
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;
                        if (mx >= backButtonRect.x && mx <= backButtonRect.x + backButtonRect.w &&
                            my >= backButtonRect.y && my <= backButtonRect.y + backButtonRect.h) {
                            SDL_StopTextInput();
                            TTF_CloseFont(font);
                            return 1;  // ← 메뉴로 클릭 시
                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);  // 배경색
        SDL_RenderClear(renderer);

        // 입력창 테두리
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &inputRect);

        // 입력 텍스트 렌더링
        if (inputBuffer[0]) {
            SDL_Surface* surface = TTF_RenderUTF8_Solid(font, inputBuffer, (SDL_Color){255, 255, 255});
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {inputRect.x + 5, inputRect.y + 10, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        // ← 메뉴로 텍스트 버튼 출력
        SDL_Surface* labelSurface = TTF_RenderUTF8_Solid(font, "← 메뉴로", (SDL_Color){255, 255, 255});
        SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
        backButtonRect.x = 480;
        backButtonRect.y = 20;
        backButtonRect.w = labelSurface->w;
        backButtonRect.h = labelSurface->h;
        SDL_RenderCopy(renderer, labelTexture, NULL, &backButtonRect);
        SDL_FreeSurface(labelSurface);
        SDL_DestroyTexture(labelTexture);

        // 안내 문구 출력
        renderText(renderer, font, "추천받고 싶은 키워드를 입력하세요", 120, 100, NULL);

        SDL_RenderPresent(renderer);  // 화면 업데이트
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    return 1;  // 기본적으로 메뉴로 복귀
}
