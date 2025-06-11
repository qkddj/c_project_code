#ifndef LOGIN_MODULE_H
#define LOGIN_MODULE_H

#include <SDL.h>
#include <SDL_ttf.h>

// 상수 정의
#define MAX_LINE_LEN 2048       // 최대 라인 길이 (레시피 파일 등)
#define MAX_ING 20              // 레시피당 최대 재료 수
#define MAX_STEP 20             // 레시피당 최대 조리 단계 수
#define MAX_KEYWORD 10          // 레시피당 최대 키워드 수
#define MAX_SUGGESTIONS 5       // 추천 레시피 이름 최대 개수

// 레시피 구조체 정의
typedef struct {
    char name[64];                  // 레시피 이름
    char* ingredients[MAX_ING];     // 재료 리스트
    int ingCount;                   // 재료 수
    char* steps[MAX_STEP];          // 조리 단계 리스트
    int stepCount;                  // 조리 단계 수
    char* keywords[MAX_KEYWORD];    // 키워드 목록
    int keywordCount;               // 키워드 수
} Recipe;

// 로그인 화면 실행
char* runLoginScreen(SDL_Window* window, SDL_Renderer* renderer);

// 로그인 또는 회원가입 요청 (stats: '1'=로그인, '9'=회원가입)
char* send_idpw(char stats, const char* id, const char* pw);

// 비밀번호 해싱 함수
char* hash_password(const char *password);

// 메인 메뉴 화면 실행
int runMainMenuScreen(SDL_Window* window, SDL_Renderer* renderer);

// 식재료 목록 화면 실행
int runIngredientScreen(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);

// 텍스트 출력 유틸 함수
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Rect* outRect);

// 마우스 좌표가 사각형 내부에 있는지 확인
int isInside(int x, int y, SDL_Rect rect);

// 재료 목록을 이름 기준으로 합치는 함수
char** sum_ingredients(const char* raw[], int rawCount, int* outCount);

// 재료 데이터 요청 함수
char** get_Ingredients_split(char stats, const char* user_key, int* outCount);

// 식재료 추가/삭제 화면 실행
int add_Remove_Ingredients(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);

// 식재료 추가 요청
void add_ingredient_api(const char* user_key, const char* name, const char* date, int qty);

// 식재료 삭제 요청
void delete_ingredient_api(const char* user_key, const char* name, int qty);

// 레시피 검색 화면 실행
int search_recipe_page(SDL_Window* window, SDL_Renderer* renderer);

// 레시피 파일에서 이름 기반으로 검색
int load_recipe_by_name(const char* filename, const char* query, Recipe* result, char suggestions[][64], int* suggestCount);

// 레시피 상세 보기 화면
int show_recipe_page(SDL_Window* window, SDL_Renderer* renderer, Recipe* recipe);

// 추천 키워드 입력 화면
int keyword_input_screen(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);

// 추천 결과 출력 화면
int show_recommendations(SDL_Window* window, SDL_Renderer* renderer, const char* keyword, const char* user_key);

#endif
