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


/*1.요리 추천 -> 키워드들 입력 받기 -> 키워드들과 일치하는 요리 찾기 -> 냉장고 안의 식재료로 만들수 있는 요리 찾기 -> 남은 요리 중 랜덤으로 선택 -> 레시피 출력
1.for (int i = 0; i < selectedCount; i++) {
    found = 0;
    for (int j = 0; j < recipe.tag_count; j++) {
        if (strcmp(selectedTags[i], recipe.tags[j]) == 0) {
            found = 1;
            break;
        }
    }
}//태그와 맞는 요리를 탐색
selectedTags = 사용자가 선택한 태그,selectedCount = 사용자가 선택한 태그 수
사용자가 고른 태그(selectedTags) 중 하나라도 요리(recipe)의 태그(recipe.tags)와 같으면 found를 1로 표시해 해당 요리를 찾는 기능이야.
//////////////////////////////////////
typedef struct {
    char name[50] = “김치찌개”;
    char tags[MAX_TAGS][20] = “매운맛”, “얼큰한 맛”;
    int tag_count = 2;
    char ingredients[MAX_INGREDIENTS][20] = “김치”, “물”, 소금;
    int ingredient_count = 3;
    요리법 // 연결리스트로 저장된 요리 법
} Recipe;

void initRecipes(Recipe recipes[], int *count) 
//구조체로 저장된 요리 데이터  구조체 Recipe는 요리 이름, 태그, 재료, 태그 개수, 재료 개수, 그리고 요리법(연결리스트)까지
한 곳에 저장하는 데이터 구조야.
//////////////////////////////////////////
for (int i = 0; i < recipe.ingredient_count; i++) {
    found = 0;
    for (int j = 0; j < fridge_count; j++) {
        if (strcmp(recipe.ingredients[i], fridge[j]) == 0) {
            found = 1;
            break;
        }
    }
}// 냉장고 안에 있는 식재료로 만들 수 있는 요리 탐색
fridge_count = 냉장고 재료 수,fridge = 냉장고 재료
요리 재료 하나씩(recipe.ingredients[i]) 냉장고 재료(fridge)와 비교해,냉장고에 있으면 found를 1로 표시해 “만들 수 있는 요리인지”를 확인하는 기능이야.
*/