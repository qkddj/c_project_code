#ifndef LOGIN_MODULE_H
#define LOGIN_MODULE_H

#include <SDL.h>
#include <SDL_ttf.h>

// 화면 렌더링 함수
char* runLoginScreen(SDL_Window* window, SDL_Renderer* renderer);

// 로그인/회원가입 서버 통신 함수
char* send_idpw(char stats, const char* id, const char* pw);
char* hash_password(const char *password);
int runMainMenuScreen(SDL_Window* window, SDL_Renderer* renderer);
int runIngredientScreen(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Rect* outRect);
int isInside(int x, int y, SDL_Rect rect);
char** sum_ingredients(const char* raw[], int rawCount, int* outCount);
char** get_Ingredients_split(char stats, const char* user_key, int* outCount);
int add_Remove_Ingredients(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);

#endif
