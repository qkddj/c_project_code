#ifndef LOGIN_MODULE_H
#define LOGIN_MODULE_H

#include <SDL.h>

// 화면 렌더링 함수
char* runLoginScreen(SDL_Window* window, SDL_Renderer* renderer);

// 로그인/회원가입 서버 통신 함수
char* send_idpw(char stats, const char* id, const char* pw);
char* hash_password(const char *password);
int runMainMenuScreen(SDL_Window* window, SDL_Renderer* renderer);

#endif
