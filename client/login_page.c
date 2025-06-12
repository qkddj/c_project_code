#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include "client_func.h"
#include <windows.h>

#define MAX_INPUT 20

// 로그인 화면을 실행하고 로그인 성공 시 사용자 키를 반환하는 함수
char* runLoginScreen(SDL_Window* window, SDL_Renderer* renderer) {
    // 폰트 로딩
    TTF_Font* font = TTF_OpenFont("NanumGothic.ttf", 24);
    if (!font) {
        printf("폰트 로딩 실패: %s\n", TTF_GetError());
        return NULL;
    }

    // 사용자 입력 저장용 버퍼 초기화
    char id_input[MAX_INPUT] = "";
    char pw_input[MAX_INPUT] = "";
    int input_focus = 0;  // 0: ID, 1: PW 입력창
    int running = 1;

    SDL_StartTextInput();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;

            // 마우스 클릭 또는 키보드 입력 처리
            if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN) {
                int x = e.button.x, y = e.button.y;
                // ID 입력창 클릭
                if (x >= 150 && x <= 500 && y >= 150 && y <= 190) input_focus = 0;
                // PW 입력창 클릭
                else if (x >= 150 && x <= 500 && y >= 210 && y <= 250) input_focus = 1;
                // TAB 키로 포커스 전환
                else if (e.key.keysym.sym == SDLK_TAB) input_focus = (input_focus == 0) ? 1 : 0;
                // 회원가입 버튼 클릭
                else if (x >= 150 && x <= 320 && y >= 290 && y <= 330) send_idpw('1', id_input, pw_input);
                // 로그인 버튼 클릭 또는 엔터 키
                else if ((x >= 330 && x <= 500 && y >= 290 && y <= 330) || e.key.keysym.sym == SDLK_RETURN) {
                    char* user_key = send_idpw('0', id_input, pw_input);
                    if (strcmp(user_key, "0") != 0) {
                        printf("로그인 성공: %s\n", user_key);
                        return user_key;
                    } else {
                        printf("로그인 실패: %s\n", user_key);
                    }
                }
            }

            // 텍스트 입력 처리
            if (e.type == SDL_TEXTINPUT) {
                char* target = (input_focus == 0) ? id_input : pw_input;
                if (strlen(target) + strlen(e.text.text) < MAX_INPUT - 1)
                    strcat(target, e.text.text);
                printf("입력됨: %s\n", target);
            }

            // 백스페이스 처리
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
                char* target = (input_focus == 0) ? id_input : pw_input;
                size_t len = strlen(target);
                if (len > 0) target[len - 1] = '\0';
                printf("입력됨: %s\n", target);
            }
        }

        // 렌더링 시작 시간 측정
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);

        // 배경색 렌더링
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // 제목 렌더링
        SDL_Color color = {255, 255, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, "로그인", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {290, 100, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // 입력창과 버튼 영역
        SDL_Rect id_box = {150, 150, 350, 40};
        SDL_Rect pw_box = {150, 210, 350, 40};
        SDL_Rect sign_up = {150, 290, 170, 40};
        SDL_Rect login_btn = {330, 290, 170, 40};

        // 입력창 테두리 렌더링
        SDL_SetRenderDrawColor(renderer, input_focus == 0 ? 0 : 255, input_focus == 0 ? 150 : 255, 255, 255);
        SDL_RenderDrawRect(renderer, &id_box);
        SDL_SetRenderDrawColor(renderer, input_focus == 1 ? 0 : 255, input_focus == 1 ? 150 : 255, 255, 255);
        SDL_RenderDrawRect(renderer, &pw_box);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &sign_up);
        SDL_RenderDrawRect(renderer, &login_btn);

        // ID 텍스트 렌더링
        SDL_Surface* id_surf = TTF_RenderUTF8_Blended(font, id_input, color);
        if (id_surf) {
            SDL_Texture* id_tex = SDL_CreateTextureFromSurface(renderer, id_surf);
            SDL_Rect id_rect = {id_box.x + 5, id_box.y + 5, id_surf->w, id_surf->h};
            SDL_RenderCopy(renderer, id_tex, NULL, &id_rect);
            SDL_FreeSurface(id_surf);
            SDL_DestroyTexture(id_tex);
        }

        // PW 입력 마스킹 처리
        char pw_mask[MAX_INPUT] = "";
        for (int i = 0; i < strlen(pw_input); i++) pw_mask[i] = '*';

        SDL_Surface* pw_surf = TTF_RenderUTF8_Blended(font, pw_mask, color);
        if (pw_surf) {
            SDL_Texture* pw_tex = SDL_CreateTextureFromSurface(renderer, pw_surf);
            SDL_Rect pw_rect = {pw_box.x + 5, pw_box.y + 5, pw_surf->w, pw_surf->h};
            SDL_RenderCopy(renderer, pw_tex, NULL, &pw_rect);
            SDL_FreeSurface(pw_surf);
            SDL_DestroyTexture(pw_tex);
        }

        // 로그인 버튼 텍스트 렌더링
        SDL_Surface* btn_surf = TTF_RenderUTF8_Blended(font, "로그인", color);
        SDL_Texture* btn_tex = SDL_CreateTextureFromSurface(renderer, btn_surf);
        SDL_Rect btn_rect = {login_btn.x + 50, login_btn.y + 5, btn_surf->w, btn_surf->h};
        SDL_RenderCopy(renderer, btn_tex, NULL, &btn_rect);
        SDL_FreeSurface(btn_surf);
        SDL_DestroyTexture(btn_tex);

        // 회원가입 버튼 텍스트 렌더링
        SDL_Surface* sign_surf = TTF_RenderUTF8_Blended(font, "회원가입", color);
        SDL_Texture* sign_tex = SDL_CreateTextureFromSurface(renderer, sign_surf);
        SDL_Rect sign_rect = {sign_up.x + 40, sign_up.y + 5, sign_surf->w, sign_surf->h};
        SDL_RenderCopy(renderer, sign_tex, NULL, &sign_rect);
        SDL_FreeSurface(sign_surf);
        SDL_DestroyTexture(sign_tex);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 대략 60fps 유지

        // 프레임 시간 측정 및 출력
        QueryPerformanceCounter(&end);
        double elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
        printf("프레임 시간: %.3f초\n", elapsedTime);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
}






// 시작-> 사용자로부터 아이디,비번 입력받기 1-1 회원가입 선택 -> 사용자가 입력한 아이디,비번 서버에 전송-> 전송 받은 아이디 비번 저장
//                                          2-1 로그인 선택 -> 사용자가 입력한 아이디,비번 서버에 전송-> 아이디와 비번 있는가? -> 없으면 1-1로 있으면 서버에서
//인식키 전송 -> 메인화면으로 전송
/*시작 및 로그인 
char id_input[20]
char pw_input[20] 
input(“%s”,&id_input);
input(“%s”,&pw_input);이건 로그인 기능에 입력 부분

//
input(“%s”,&id_input);
input(“%s”,&pw_input);
#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


CreatLoginWindow() 이건 로그인 화면을  띄우는 함수

if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN) {
   int x = e.button.x, y = e.button.y;
   if (x==로그인 위치 && y==로그인 위치){
      struct login { // 아이디와 패스워드를 구조체에 저장
       char id = id_input;
       char pw = pw_input;
   }; //마우스 위치를 SDL의 이벤트를 이용해 파악하여 클릭한 것을 파악 
      send_data(login); 비밀번호를 해시로 암호화 하여 전송  이코드는 로그인 버튼을 클릭하면,
입력한 아이디(ID)와 비밀번호(PW)를 구조체에 담아 서버로 전송하는 역할을 해.

input() 받기
if (e.type == SDL_TEXTINPUT) {
    char* target = input_data;
    if (strlen(target) + strlen(e.text.text) < MAX_INPUT - 1) {
        strcat(target, e.text.text);
        printf("입력됨: %s\n", target);
    }
} // SDL의 이벤트를 이용해 입력 값 받기   위에 함수는 사용자가 키보드에 입력한 텍스트를 저장하는 역활

send_Data()
#define MAX_PAYLOAD 1024
int send_data(data) {
    char buf[MAX_PAYLOAD];
    memset(data, 0, sizeof(input));
    memset(buf, 0, sizeof(buf));
    strcpy(&buf[LWS_PRE], data); // buf에 데이터 삽입
    lws_write(wsi, (unsigned char *)&buf[LWS_PRE], strlen(input), LWS_WRITE_TEXT); 
    // 데이터 전송
    break;
} 소켓 통신을 통해 데이터를 전송 위에 함수는 입력된 데이터를 소켓을 통해 서버에 전송

//Search(login);
{
 char id = id_input;
 char pw = pw_input;}
// 순차탐색 알고리즘
으로 사용자 데이터 찾기

//if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN) {
   int x = e.button.x, y = e.button.y;
   if (x==회원가입 위치 && y==회원가입 위치){
      struct sign_up {
       char id = id_input;
       char pw = pw_input;
   }; //마우스 위치를 SDL의 이벤트를 이용해 파악하여 클릭한 것을 파악 
      send_data(sign_up); //해시로 비밀번호를 암호화 후 서버에 전송 이건 회원가입 버튼 클릭 시 동작하는 로직 쉽게 말해 회원가입 요청 처리 

//char *id = strtok(line, ",");
char *pw = strtok(NULL, ",");
char *user_key = strtok(NULL, ",");

struct Member members[1000];
memberCount = len(Member members);

int new_sign_up(){
     strcpy(members[memberCount]. User_IDid, sign_up.id);
     strcpy(members[memberCount]. User_PWpw, sign_up.pw)
     memberCount++;
} //전송 받은 아이디와 비밀번호를 구조체로 서버에 저장  이건 회원가입한 정보를 서버에 저장해주는 기능