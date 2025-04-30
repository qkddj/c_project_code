#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_func.h"

#define MAX_LINE 1024
#define USER_KEY_LEN 16

char* Search_user_data(char *input_id,char *input_pw) {
    FILE *fp = fopen("c_project_data.csv", "r");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return "파일 열기 실패";
    }

    char line[MAX_LINE];

    // 첫 줄은 헤더(출력 안하고 넘기기기)
    if (fgets(line, sizeof(line), fp)) {}

    // 각 줄을 읽고 파싱
    while (fgets(line, sizeof(line), fp)) {
        char *id = strtok(line, ",");
        char *pw = strtok(NULL, ",");
        char *user_key = strtok(NULL, ",\n");

        if (strcmp(id, input_id) == 0 && strcmp(pw, input_pw) == 0) {
            strcat(user_key, "\n"); 
            return user_key;
        }
    }

    fclose(fp);
    return NULL;
}

int Save_user_data(char *input_id,char *input_pw) {
    FILE *fp = fopen("c_project_data.csv", "a+");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return 0;
    }

    char line[MAX_LINE];

    fseek(fp, 0, SEEK_SET);

    while (fgets(line, sizeof(line), fp)) {
        char *id = strtok(line, ",");
        char *pw = strtok(NULL, ",");
        // char *user_key = strtok(NULL, ",\n");

        if (strcmp(id, input_id) == 0 && strcmp(pw, input_pw) == 0) {
            fclose(fp);
            return 0;
        }
    }

    char user_key[MAX_LINE];
    snprintf(user_key, sizeof(user_key), "%s_%s", input_id, input_pw);

    fprintf(fp, "%s,%s,%s\n", input_id, input_pw, user_key);

    fclose(fp); 
    return 1;
}
