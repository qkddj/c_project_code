#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client_func.h"

int runIngredientScreen(SDL_Window* window, SDL_Renderer* renderer, const char* user_key) {
    int rawCount = 0;
    char** ingredients_raw = get_Ingredients_split('2', user_key, &rawCount);

    int mergedCount = 0;
    char** ingredients = sum_ingredients((const char**)ingredients_raw, rawCount, &mergedCount);
    int ingredientCount = mergedCount;

    int scrollOffset = 0;
    const int itemsPerPage = 7;

    int showingDetail = 0;
    char selectedName[32] = "";

    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        for (int i = 0; i < mergedCount; i++) free(ingredients[i]);
        free(ingredients);
        return 10;
    }

    SDL_Event event;
    int running = 1;

    SDL_Rect itemRects[itemsPerPage];
    SDL_Rect backButtonRect;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    return 7;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (showingDetail == 0) {
                            running = 0;
                            return 1;
                        }
                        showingDetail = 0;
                        scrollOffset = 0;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        scrollOffset++;
                    } else if (event.key.keysym.sym == SDLK_UP && scrollOffset > 0) {
                        scrollOffset--;
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
                                return 1;
                            }
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

                        renderText(renderer, font, displayLine, 175,
                                   startY + (count - scrollOffset) * 50, &itemRects[count - scrollOffset]);
                    }
                    count++;
                }
            }
        }

        if (showingDetail)
            renderText(renderer, font, "← 목록으로", 480, 20, &backButtonRect);
        else
            renderText(renderer, font, "← 메뉴로", 480, 20, &backButtonRect);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    for (int i = 0; i < mergedCount; i++) free(ingredients[i]);
    free(ingredients);
    return 7;
}
