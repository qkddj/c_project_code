#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "client_func.h"

#define MAX_RECOMMEND 50

int show_recommendations(SDL_Window* window, SDL_Renderer* renderer, const char* keyword, const char* user_key) {
    printf("키워드 입력됨: %s\n", keyword);

    // 레시피 전체 불러오기 (이름만 추출)
    FILE* file = fopen("recipes.csv", "r");
    if (!file) return 0;

    Recipe recipes[MAX_RECOMMEND];
    int recipeCount = 0;

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file) && recipeCount < MAX_RECOMMEND) {
        line[strcspn(line, "\r\n")] = '\0';
        char* token = strtok(line, ",");
        if (!token) continue;
        strncpy(recipes[recipeCount].name, token, sizeof(recipes[recipeCount].name));
        recipeCount++;
    }
    fclose(file);

    // 사용자 식재료 불러오기 및 병합
    int rawCount = 0;
    char** ingredients_raw = get_Ingredients_split('2', user_key, &rawCount);

    // 필터링된 추천 리스트
    Recipe matched[MAX_RECOMMEND];
    int matchedCount = 0;

    printf("🔍 키워드와 일치하는 레시피 목록:\n");

    for (int i = 0; i < recipeCount; i++) {
        Recipe temp;
        char dummy[64][64];
        int dummyCount;

        if (!load_recipe_by_name("recipes.csv", recipes[i].name, &temp, dummy, &dummyCount)) continue;

        // 키워드와 일치하는지 확인
        int found = 0;
        for (int k = 0; k < temp.keywordCount; k++) {
            if (strstr(temp.keywords[k], keyword)) {
                found = 1;
                break;
            }
        }
        if (!found) continue;

        printf("- %s\n", temp.name);

        // 재료 보유 여부 확인
        int canMake = 1;
        for (int j = 0; j < temp.ingCount; j++) {
            int hasIng = 0;

            // "계란 1개" → "계란"
            char temp_ing_name[64];
            strncpy(temp_ing_name, temp.ingredients[j], sizeof(temp_ing_name));
            temp_ing_name[sizeof(temp_ing_name) - 1] = '\0';
            char* unit_ptr = strrchr(temp_ing_name, ' ');
            if (unit_ptr != NULL) *unit_ptr = '\0';

            for (int u = 0; u < rawCount; u++) {
                char user_ing_name[64];
                strncpy(user_ing_name, ingredients_raw[u], sizeof(user_ing_name));
                user_ing_name[strcspn(user_ing_name, ",")] = '\0';

                if (strcmp(user_ing_name, temp_ing_name) == 0) {
                    hasIng = 1;
                    break;
                }
            }

            if (!hasIng) {
                printf("  ❌ 부족한 재료: %s\n", temp_ing_name);
                canMake = 0;
                break;
            }
        }

        if (canMake && matchedCount < MAX_RECOMMEND) {
            matched[matchedCount++] = temp;
        }
    }

    for (int i = 0; i < rawCount; i++) free(ingredients_raw[i]);
    free(ingredients_raw);

    // 하나 랜덤으로 골라 바로 레시피 화면으로 이동
    if (matchedCount > 0) {
        srand((unsigned int)time(NULL));
        int randIndex = rand() % matchedCount;

        // 선택된 레시피 이름을 다시 불러와서 show_recipe_page로 전달
        Recipe selected;
        char dummy[64][64];
        int dummyCount;
        if (load_recipe_by_name("recipes.csv", matched[randIndex].name, &selected, dummy, &dummyCount)) {
            return show_recipe_page(window, renderer, &selected);
        }
    }

    // 추천 레시피가 없는 경우 SDL 텍스트만 출력
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 28);
    if (!font) return 0;

    SDL_Event event;
    int running = 1;
    SDL_Rect backButtonRect;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return 1;
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return 0;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;
                if (mx >= backButtonRect.x && mx <= backButtonRect.x + backButtonRect.w &&
                    my >= backButtonRect.y && my <= backButtonRect.y + backButtonRect.h) {
                    return 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        renderText(renderer, font, "추천 가능한 레시피가 없습니다.", 50, 30, NULL);
        renderText(renderer, font, "← 메뉴로", 480, 20, &backButtonRect);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    return 0;
}
