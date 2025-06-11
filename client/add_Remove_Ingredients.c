#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client_func.h"

int add_Remove_Ingredients(SDL_Window* window, SDL_Renderer* renderer, const char* user_key) {
    // 원재료 정보 불러오기
    int rawCount = 0;
    char** ingredients_raw = get_Ingredients_split('2', user_key, &rawCount); // 원재료 분리 데이터
    int mergedCount = 0;
    char** ingredients = sum_ingredients((const char**)ingredients_raw, rawCount, &mergedCount); // 이름 기준으로 합쳐진 목록
    int ingredientCount = mergedCount;

    int scrollOffset = 0;
    const int itemsPerPage = 5;

    int showingDetail = 0; // 상세보기 모드 여부
    char selectedName[32] = ""; // 상세보기에서 선택된 재료 이름

    int inputActive = 0; // 입력창 포커스 여부
    SDL_Rect inputBoxRect = {170, 390, 300, 40}; // 입력창 위치 및 크기
    char inputBuffer[64] = ""; // 입력된 문자열
    int inputLength = 0;

    int showWarning = 0; // 경고창 표시 여부

    // 폰트 로딩
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

    SDL_StartTextInput(); // 텍스트 입력 활성화

    // 이벤트 루프
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    return 7; // 프로그램 종료 요청

                case SDL_KEYDOWN:
                    // ESC: 뒤로가기 or 상세보기 해제
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (showingDetail == 0) {
                            running = 0;
                            return 1; // 메인 메뉴로 돌아감
                        }
                        showingDetail = 0;
                        scrollOffset = 0;
                        inputActive = 0;
                        showWarning = 0;
                    } 
                    // Enter: 입력 처리
                    else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (showWarning) {
                            showWarning = 0; // 경고창 닫기
                            break;
                        }

                        if (inputActive) {
                            printf("입력 확정: %s\n", inputBuffer);
                            char name[32], date[32];
                            int y, m, d, qty;

                            // 추가 형식: 이름/yyyy.mm.dd/수량
                            if (sscanf(inputBuffer, "%31[^/]/%d.%d.%d/%d", name, &y, &m, &d, &qty) == 5) {
                                if (y >= 2000 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
                                    snprintf(date, sizeof(date), "%04d.%02d.%02d", y, m, d);
                                    printf("➕ 추가 요청: %s / %s / %d개\n", name, date, qty);
                                    add_ingredient_api(user_key, name, date, qty);
                                } else {
                                    showWarning = 1; // 날짜 범위 오류
                                }
                            } 
                            // 삭제 형식: 이름/수량
                            else if (sscanf(inputBuffer, "%31[^/]/%d", name, &qty) == 2) {
                                printf("➖ 삭제 요청: %s / %d개\n", name, qty);
                                delete_ingredient_api(user_key, name, qty);
                            } 
                            else {
                                showWarning = 1; // 형식 오류
                            }

                            // 기존 상태 저장
                            int prevScroll = scrollOffset;
                            int wasDetail = showingDetail;
                            char prevSelected[32];
                            strcpy(prevSelected, selectedName);

                            // 목록 재로딩
                            for (int i = 0; i < rawCount; i++) free(ingredients_raw[i]);
                            free(ingredients_raw);
                            for (int i = 0; i < mergedCount; i++) free(ingredients[i]);
                            free(ingredients);

                            rawCount = 0;
                            ingredients_raw = get_Ingredients_split('2', user_key, &rawCount);
                            mergedCount = 0;
                            ingredients = sum_ingredients((const char**)ingredients_raw, rawCount, &mergedCount);
                            ingredientCount = mergedCount;

                            // 상태 복구
                            scrollOffset = prevScroll;
                            showingDetail = wasDetail;
                            strcpy(selectedName, prevSelected);

                            inputBuffer[0] = '\0';
                            inputLength = 0;
                        }
                    } 
                    // 입력 중 백스페이스 처리
                    else if (inputActive && event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (inputLength > 0) {
                            inputBuffer[--inputLength] = '\0';
                            printf("입력됨: %s\n", inputBuffer);
                        }
                    } 
                    // 키보드 방향키로 스크롤
                    else if (event.key.keysym.sym == SDLK_DOWN) {
                        scrollOffset++;
                    } else if (event.key.keysym.sym == SDLK_UP && scrollOffset > 0) {
                        scrollOffset--;
                    }
                    break;

                case SDL_TEXTINPUT:
                    // 입력창에 글자 추가
                    if (inputActive && inputLength + strlen(event.text.text) < sizeof(inputBuffer) - 1) {
                        strcat(inputBuffer, event.text.text);
                        inputLength += strlen(event.text.text);
                        printf("입력됨: %s\n", inputBuffer);
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    // 마우스 휠로 스크롤
                    if (event.wheel.y > 0 && scrollOffset > 0) scrollOffset--;
                    else if (event.wheel.y < 0) scrollOffset++;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mx = event.button.x;
                        int my = event.button.y;

                        // ← 버튼 클릭 시 뒤로가기
                        if (isInside(mx, my, backButtonRect)) {
                            if (showingDetail) {
                                showingDetail = 0;
                                scrollOffset = 0;
                            } else {
                                running = 0;
                            }
                        }

                        // 입력창 클릭 시 활성화
                        if (isInside(mx, my, inputBoxRect)) {
                            inputActive = 1;
                        } else {
                            inputActive = 0;
                        }

                        // 재료 클릭 시 상세 보기 진입
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

        // 화면 렌더링 시작
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int startY = 70;
        int count = 0;

        // 목록 화면 or 상세 화면 출력
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

        // 하단 UI 렌더링
        renderText(renderer, font, showingDetail ? "← 목록으로" : "← 메뉴로", 480, 20, &backButtonRect);
        renderText(renderer, font, "이름/유통기한/개수 or 이름/개수", inputBoxRect.x - 45, inputBoxRect.y - 40, NULL);

        // 입력창 표시
        SDL_SetRenderDrawColor(renderer, inputActive ? 0 : 255, inputActive ? 200 : 255, inputActive ? 255 : 255, 255);
        SDL_RenderDrawRect(renderer, &inputBoxRect);
        if (inputBuffer[0]) {
            renderText(renderer, font, inputBuffer, inputBoxRect.x + 5, inputBoxRect.y + 5, NULL);
        }

        // 경고창 렌더링
        if (showWarning) {
            SDL_Rect warningBox = {150, 200, 340, 100};
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            SDL_RenderFillRect(renderer, &warningBox);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &warningBox);
            renderText(renderer, font, "올바른 형식이 아닙니다!", warningBox.x + 25, warningBox.y + 20, NULL);
            renderText(renderer, font, "Enter를 눌러 닫기", warningBox.x + 65, warningBox.y + 50, NULL);
        }

        SDL_RenderPresent(renderer); // 화면 업데이트
    }

    // 자원 정리
    SDL_StopTextInput();
    TTF_CloseFont(font);
    for (int i = 0; i < mergedCount; i++) free(ingredients[i]);
    free(ingredients);
    for (int i = 0; i < rawCount; i++) free(ingredients_raw[i]);
    free(ingredients_raw);

    return 1; // 메인 메뉴로 복귀
}

