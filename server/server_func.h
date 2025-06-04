#ifndef OPEN_H
#define OPEN_H

typedef struct {
    char name[64];
    char date[32];
    int qty;
} Ingredient;

char* Search_user_key(char *input_id,char *input_pw);
int Save_user_key(char *input_id,char *input_pw);
char* load_ingredients_as_text(const char* filename);
int save_ingredient_to_file(const char* filename, const char* name, const char* date, int qty);
int delete_ingredient_from_file(const char* filename, const char* name, int qty);

#endif