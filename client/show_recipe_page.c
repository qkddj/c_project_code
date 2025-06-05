#include <SDL.h>
#include <SDL_ttf.h>
#include "client_func.h"

int show_recipe_page(SDL_Window* window, SDL_Renderer* renderer, Recipe* recipe) {
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 24);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    return 1;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = 0;
                        return 0;
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderClear(renderer);

        int y = 50;

        // 제목 출력
        renderText(renderer, font, recipe->name, 50, y, NULL);
        y += 50;

        // 재료 출력
        renderText(renderer, font, "[재료]", 50, y, NULL);
        y += 35;
        for (int i = 0; i < recipe->ingCount; i++) {
            renderText(renderer, font, recipe->ingredients[i], 70, y, NULL);
            y += 30;
        }
        y += 20;

        // 절차 출력
        renderText(renderer, font, "[조리 절차]", 50, y, NULL);
        y += 35;
        for (int i = 0; i < recipe->stepCount; i++) {
            renderText(renderer, font, recipe->steps[i], 70, y, NULL);
            y += 30;
        }
        y += 20;

        // 키워드 출력
        renderText(renderer, font, "[키워드]", 50, y, NULL);
        y += 35;
        for (int i = 0; i < recipe->keywordCount; i++) {
            renderText(renderer, font, recipe->keywords[i], 70, y, NULL);
            y += 30;
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    return 0;
}
