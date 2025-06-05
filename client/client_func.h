#ifndef LOGIN_MODULE_H
#define LOGIN_MODULE_H

#include <SDL.h>
#include <SDL_ttf.h>

#define MAX_LINE_LEN 2048
#define MAX_ING 20
#define MAX_STEP 20
#define MAX_KEYWORD 10
#define MAX_SUGGESTIONS 5

typedef struct {
    char name[64];
    char* ingredients[MAX_ING];
    int ingCount;
    char* steps[MAX_STEP];
    int stepCount;
    char* keywords[MAX_KEYWORD];
    int keywordCount;
} Recipe;

char* runLoginScreen(SDL_Window* window, SDL_Renderer* renderer);

char* send_idpw(char stats, const char* id, const char* pw);
char* hash_password(const char *password);
int runMainMenuScreen(SDL_Window* window, SDL_Renderer* renderer);
int runIngredientScreen(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Rect* outRect);
int isInside(int x, int y, SDL_Rect rect);
char** sum_ingredients(const char* raw[], int rawCount, int* outCount);
char** get_Ingredients_split(char stats, const char* user_key, int* outCount);
int add_Remove_Ingredients(SDL_Window* window, SDL_Renderer* renderer, const char* user_key);
void add_ingredient_api(const char* user_key, const char* name, const char* date, int qty);
void delete_ingredient_api(const char* user_key, const char* name, int qty);
int search_recipe_page(SDL_Window* window, SDL_Renderer* renderer);
int load_recipe_by_name(const char* filename, const char* query, Recipe* result, char suggestions[][64], int* suggestCount);
int show_recipe_page(SDL_Window* window, SDL_Renderer* renderer, Recipe* recipe);

#endif
