#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "client_func.h"

// 문자열 좌우 공백 제거 함수
void trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
}

int load_recipe_by_name(const char* filename, const char* query, Recipe* result, char suggestions[][64], int* suggestCount) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[MAX_LINE_LEN];
    *suggestCount = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        char* token = strtok(line, ",");
        if (!token) continue;

        char name[64];
        strncpy(name, token, sizeof(name));
        trim(name);

        // 추천 후보 저장
        if (strstr(name, query)) {
            if (strcmp(name, query) != 0 && *suggestCount < MAX_SUGGESTIONS) {
                strncpy(suggestions[*suggestCount], name, 64);
                (*suggestCount)++;
            }
        }

        // 정확히 일치하는 레시피 찾음
        if (strcmp(name, query) == 0) {
            strncpy(result->name, name, sizeof(result->name));

            // 재료
            int ingIdx = 0;
            while ((token = strtok(NULL, ",")) && strcmp(token, "0") != 0 && ingIdx < MAX_ING) {
                result->ingredients[ingIdx] = strdup(token);
                ingIdx++;
            }
            result->ingCount = ingIdx;

            // 절차
            int stepIdx = 0;
            while ((token = strtok(NULL, ",")) && strcmp(token, "0") != 0 && stepIdx < MAX_STEP) {
                result->steps[stepIdx] = strdup(token);
                stepIdx++;
            }
            result->stepCount = stepIdx;

            // 키워드
            int keywordIdx = 0;
            while ((token = strtok(NULL, ",")) && keywordIdx < MAX_KEYWORD) {
                result->keywords[keywordIdx] = strdup(token);
                keywordIdx++;
            }
            result->keywordCount = keywordIdx;

            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0; // 일치 항목 없음
}
