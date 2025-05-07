#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include "client_func.h"

#define MAX_INPUT 20

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() < 0) {
        printf("TTF 초기화 실패: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("로그인 예제", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 24);  // 폰트 파일이 같은 폴더에 있어야 함
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return 1;
    }

    char id_input[MAX_INPUT] = "";
    char pw_input[MAX_INPUT] = "";
    int input_focus = 0; // 0: ID, 1: PW
    int running = 1;

    SDL_StartTextInput();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                printf("입력된 키: %d\n", e.key.keysym.sym);
            }            

            if (e.type == SDL_QUIT) running = 0;

            if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN) {
                int x = e.button.x, y = e.button.y;
                    if (x >= 150 && x <= 500 && y >= 150 && y <= 190) input_focus = 0;
                    else if (x >= 150 && x <= 500 && y >= 210 && y <= 250) input_focus = 1;
                    else if (e.key.keysym.sym == SDLK_TAB) {
                        input_focus = (input_focus == 0) ? 1 : 0; // 탭 키로 포커스 전환
                    }
                    else if (x >= 150 && x <= 320 && y >= 290 && y <= 330){ //회원가입
                        send_idpw('1',id_input,pw_input);
                    }
                    else if ((x >= 330 && x <= 500 && y >= 290 && y <= 330) || (e.key.keysym.sym == SDLK_RETURN)) { //로그인
                        char* user_key = send_idpw('0',id_input,pw_input);
                        printf("%s",user_key);
                }
            }

            if (e.type == SDL_TEXTINPUT) {
                char* target = (input_focus == 0) ? id_input : pw_input;
                if (strlen(target) + strlen(e.text.text) < MAX_INPUT - 1) {
                    strcat(target, e.text.text);
                    printf("입력됨: %s\n", target);
                }
            }

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
                char* target = (input_focus == 0) ? id_input : pw_input;
                size_t len = strlen(target);
                if (len > 0) target[len - 1] = '\0';
                printf("입력됨: %s\n", target);
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_Color color = {255, 255, 255};

        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, "로그인", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {290, 100, surface->w, surface->h};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        SDL_Rect id_box = {150, 150, 350, 40};
        SDL_Rect pw_box = {150, 210, 350, 40};
        SDL_Rect Sign_up = {150, 290, 170, 40};
        SDL_Rect login_btn = {330, 290, 170, 40};

        // ID 입력 박스 그리기
        if (input_focus == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255); // 파란색
        }    
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 흰색
        }
        SDL_RenderDrawRect(renderer, &id_box);

        // PW 입력 박스 그리기
        if (input_focus == 1) {
            SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255); // 파란색
        } 
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 흰색
        }
        SDL_RenderDrawRect(renderer, &pw_box);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &Sign_up);
        SDL_RenderDrawRect(renderer, &login_btn);

        // // ID 텍스트: * 표시
        SDL_Surface* id_surf = TTF_RenderUTF8_Blended(font, id_input, color);
        if (id_surf != NULL) {
            SDL_Texture* id_tex = SDL_CreateTextureFromSurface(renderer, id_surf);
            SDL_Rect id_text_rect = {id_box.x + 5, id_box.y + 5, id_surf->w, id_surf->h};
            SDL_RenderCopy(renderer, id_tex, NULL, &id_text_rect);
            SDL_FreeSurface(id_surf);
            SDL_DestroyTexture(id_tex);
        }

        // // PW 텍스트: * 표시
        char pw_mask[MAX_INPUT] = "";
        for (int i = 0; i < strlen(pw_input); i++) pw_mask[i] = '*';
        
        SDL_Surface* pw_surf = TTF_RenderUTF8_Blended(font, pw_mask, color);
        if (pw_surf != NULL) {
            SDL_Texture* pw_tex = SDL_CreateTextureFromSurface(renderer, pw_surf);
            SDL_Rect pw_text_rect = {pw_box.x + 5, pw_box.y + 5, pw_surf->w, pw_surf->h};
            SDL_RenderCopy(renderer, pw_tex, NULL, &pw_text_rect);
            SDL_FreeSurface(pw_surf);
            SDL_DestroyTexture(pw_tex);
        }
        
        

        // 로그인 버튼 텍스트
        SDL_Surface* btn_surf = TTF_RenderUTF8_Blended(font, "로그인", color);
        SDL_Texture* btn_tex = SDL_CreateTextureFromSurface(renderer, btn_surf);
        SDL_Rect btn_text_rect = {login_btn.x + 50, login_btn.y + 5, btn_surf->w, btn_surf->h};
        SDL_RenderCopy(renderer, btn_tex, NULL, &btn_text_rect);
        SDL_FreeSurface(btn_surf);
        SDL_DestroyTexture(btn_tex);

        //회원가입 버튼 텍스트
        SDL_Surface* Sign_surf = TTF_RenderUTF8_Blended(font, "회원가입", color);
        SDL_Texture* Sign_tex = SDL_CreateTextureFromSurface(renderer, Sign_surf);
        SDL_Rect Sign_text_rect = {Sign_up.x + 40, Sign_up.y + 5, Sign_surf->w, Sign_surf->h};
        SDL_RenderCopy(renderer, Sign_tex, NULL, &Sign_text_rect);
        SDL_FreeSurface(Sign_surf);
        SDL_DestroyTexture(Sign_tex);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}