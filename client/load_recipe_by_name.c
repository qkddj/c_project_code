// recipe_loader.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "client_func.h"

// 문자열 좌우 공백 제거 함수
void trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++; // 앞쪽 공백 제거
    if (*str == 0) return; // 빈 문자열이면 종료
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--; // 뒤쪽 공백 제거
    *(end + 1) = 0; // 문자열 종료 문자 추가
}

// 레시피 파일에서 사용자가 입력한 이름과 일치하는 레시피를 로드하는 함수
// - filename: CSV 형식 레시피 파일 경로
// - query: 사용자가 입력한 검색어 (정확한 레시피 이름)
// - result: 검색 결과 레시피를 저장할 구조체 포인터
// - suggestions: query와 유사한 이름들을 저장할 배열
// - suggestCount: suggestions에 저장된 추천 수
// 성공 시 1 반환, 실패 시 0 반환
int load_recipe_by_name(const char* filename, const char* query, Recipe* result, char suggestions[][64], int* suggestCount) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[MAX_LINE_LEN];
    *suggestCount = 0;

    // 파일을 한 줄씩 읽으며 검색
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0'; // 개행 문자 제거

        char* token = strtok(line, ","); // 첫 토큰: 레시피 이름
        if (!token) continue;

        char name[64];
        strncpy(name, token, sizeof(name));
        trim(name); // 공백 제거

        // 추천 후보 저장: query를 포함하지만 정확히 같지는 않은 이름 최대 5개 저장
        if (strstr(name, query)) {
            if (strcmp(name, query) != 0 && *suggestCount < MAX_SUGGESTIONS) {
                strncpy(suggestions[*suggestCount], name, 64);
                (*suggestCount)++;
            }
        }

        // 정확히 일치하는 레시피를 찾은 경우
        if (strcmp(name, query) == 0) {
            strncpy(result->name, name, sizeof(result->name));

            // 재료 목록 파싱 ("0"이 종료 구분자)
            int ingIdx = 0;
            while ((token = strtok(NULL, ",")) && strcmp(token, "0") != 0 && ingIdx < MAX_ING) {
                result->ingredients[ingIdx] = strdup(token);
                ingIdx++;
            }
            result->ingCount = ingIdx;

            // 조리 절차 파싱
            int stepIdx = 0;
            while ((token = strtok(NULL, ",")) && strcmp(token, "0") != 0 && stepIdx < MAX_STEP) {
                result->steps[stepIdx] = strdup(token);
                stepIdx++;
            }
            result->stepCount = stepIdx;

            // 키워드 파싱 (남은 모든 항목)
            int keywordIdx = 0;
            while ((token = strtok(NULL, ",")) && keywordIdx < MAX_KEYWORD) {
                result->keywords[keywordIdx] = strdup(token);
                keywordIdx++;
            }
            result->keywordCount = keywordIdx;

            fclose(file);
            return 1; // 성공적으로 로드함
        }
    }

    fclose(file);
    return 0; // 일치하는 레시피를 찾지 못함
}





/*레시피 확인 -> 사용자에게 요리 이름 입력 받기 -> 입력받은 값과 일치하는 요리가 있는가? yes -> 요리 레시피 띄우기, no-> 관련 검색어 띄우기
char Input_Cooking_Name[15]

input(“%s”,&Input_Cooking_Name); 사용자가 입력한 요리 이름을 문자열로 저장하는 기능

for (int i = 0; i < ingredient_count; i++) {
   if (strcmp(keywords[i], Input_Cooking_Name) == 0){
        cooking = i;
        break;
    }   
}// 레시피를 찾기  입력한 요리 이름(Input_Cooking_Name)과 keywords 배열 안 단어들을 하나씩 비교해서 같으면 해당 인덱스(cooking)를 저장하고 반복을 멈추는 기능이야.
즉, 요리 레시피 번호를 찾는 코드야.

for (int i = 0; i < keyword_count; i++) {
    if (strncmp(keywords[i], input, strlen(input)) == 0) {
        printf(" - %s\n", keywords[i]);
        count++;
        if (count >= 10) break;
    }
}//관련 검색어를 출력// 입력한 문자열 input과 keywords 배열의 각 단어를처음부터 input 길이만큼만 비교해서 같은 단어를 찾고,
최대 10개까지 관련 검색어를 출력하는 기능이야.







*/