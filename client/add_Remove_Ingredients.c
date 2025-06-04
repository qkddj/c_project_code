#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client_func.h"

int add_Remove_Ingredients(SDL_Window* window, SDL_Renderer* renderer, const char* user_key) {
    int rawCount = 0;
    char** ingredients_raw = get_Ingredients_split('2', user_key, &rawCount);
    int mergedCount = 0;
    char** ingredients = sum_ingredients((const char**)ingredients_raw, rawCount, &mergedCount);
    int ingredientCount = mergedCount;

    int scrollOffset = 0;
    const int itemsPerPage = 5;

    int showingDetail = 0;
    char selectedName[32] = "";

    int inputActive = 0;
    SDL_Rect inputBoxRect = {170, 390, 300, 40};
    char inputBuffer[64] = "";
    int inputLength = 0;

    int showWarning = 0;

    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("\n폰트 로딩 실패: %s\n", TTF_GetError());
        for (int i = 0; i < mergedCount; i++) free(ingredients[i]);
        free(ingredients);
        return 10;
    }

    SDL_Event event;
    int running = 1;
    SDL_Rect itemRects[itemsPerPage], backButtonRect;

    SDL_StartTextInput();

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    return 7;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        showingDetail = 0;
                        scrollOffset = 0;
                        inputActive = 0;
                        showWarning = 0;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (showWarning) {
                            showWarning = 0;
                            break;
                        }

                        if (inputActive) {
                            printf("입력 확정: %s\n", inputBuffer);
                            char name[32], date[32];
                            int y, m, d, qty;

                            if (sscanf(inputBuffer, "%31[^/]/%d.%d.%d/%d", name, &y, &m, &d, &qty) == 5) {
                                if (y >= 2000 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
                                    snprintf(date, sizeof(date), "%04d.%02d.%02d", y, m, d);
                                    printf("\u2795 추가 요청: %s / %s / %d개\n", name, date, qty);
                                    // add_ingredient_api(user_key, name, date, qty);
                                } else {
                                    showWarning = 1;
                                }
                            } else if (sscanf(inputBuffer, "%31[^/]/%d", name, &qty) == 2) {
                                printf("\u2796 삭제 요청: %s / %d개\n", name, qty);
                                // delete_ingredient_api(user_key, name, qty);
                            } else {
                                showWarning = 1;
                            }

                            inputBuffer[0] = '\0';
                            inputLength = 0;
                        }
                    } else if (inputActive && event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (inputLength > 0) {
                            inputBuffer[--inputLength] = '\0';
                            printf("입력됨: %s\n", inputBuffer);
                        }
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        scrollOffset++;
                    } else if (event.key.keysym.sym == SDLK_UP && scrollOffset > 0) {
                        scrollOffset--;
                    }
                    break;

                case SDL_TEXTINPUT:
                    if (inputActive && inputLength + strlen(event.text.text) < sizeof(inputBuffer) - 1) {
                        strcat(inputBuffer, event.text.text);
                        inputLength += strlen(event.text.text);
                        printf("입력됨: %s\n", inputBuffer);
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0 && scrollOffset > 0) scrollOffset--;
                    else if (event.wheel.y < 0) scrollOffset++;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;

                        if (isInside(mx, my, backButtonRect)) {
                            if (showingDetail) {
                                showingDetail = 0;
                                scrollOffset = 0;
                            } else {
                                running = 0;
                            }
                        }

                        if (isInside(mx, my, inputBoxRect)) {
                            inputActive = 1;
                        } else {
                            inputActive = 0;
                        }

                        if (!showingDetail) {
                            for (int i = 0; i < itemsPerPage; i++) {
                                int index = scrollOffset + i;
                                if (index < ingredientCount && isInside(mx, my, itemRects[i])) {
                                    sscanf(ingredients[index], "%[^/]", selectedName);
                                    showingDetail = 1;
                                    scrollOffset = 0;
                                    break;
                                }
                            }
                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int startY = 70;
        int count = 0;

        if (!showingDetail) {
            for (int i = 0; i < itemsPerPage; i++) {
                int index = scrollOffset + i;
                if (index < ingredientCount) {
                    renderText(renderer, font, ingredients[index], 175, startY + i * 50, &itemRects[i]);
                }
            }
        } else {
            for (int i = 0; i < rawCount; i++) {
                char name[32], date[32];
                int qty;
                sscanf(ingredients_raw[i], "%[^,],%[^,],%d", name, date, &qty);

                if (strcmp(name, selectedName) == 0) {
                    if (count >= scrollOffset && count < scrollOffset + itemsPerPage) {
                        char displayLine[100];
                        sprintf(displayLine, "%s/%s/%d개", name, date, qty);
                        renderText(renderer, font, displayLine, 175, startY + (count - scrollOffset) * 50, &itemRects[count - scrollOffset]);
                    }
                    count++;
                }
            }
        }

        renderText(renderer, font, showingDetail ? "← 목록으로" : "← 메뉴로", 480, 20, &backButtonRect);
        renderText(renderer, font, "이름/유통기한/개수 or 이름/개수", inputBoxRect.x - 45, inputBoxRect.y - 40, NULL);

        SDL_SetRenderDrawColor(renderer, inputActive ? 0 : 255, inputActive ? 200 : 255, inputActive ? 255 : 255, 255);
        SDL_RenderDrawRect(renderer, &inputBoxRect);
        if (inputBuffer[0]) {
            renderText(renderer, font, inputBuffer, inputBoxRect.x + 5, inputBoxRect.y + 5, NULL);
        }

        if (showWarning) {
            SDL_Rect warningBox = {150, 200, 340, 100};
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            SDL_RenderFillRect(renderer, &warningBox);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &warningBox);
            renderText(renderer, font, "올바른 형식이 아닙니다!", warningBox.x + 25, warningBox.y + 20, NULL);
            renderText(renderer, font, "Enter를 눌러 닫기", warningBox.x + 65, warningBox.y + 50, NULL);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    for (int i = 0; i < mergedCount; i++) free(ingredients[i]);
    free(ingredients);
    return 1;
}
