#ifndef OPEN_H
#define OPEN_H

char* Search_user_key(char *input_id,char *input_pw);
int Save_user_key(char *input_id,char *input_pw);
char* load_ingredients_as_text(const char* filename);

#endif