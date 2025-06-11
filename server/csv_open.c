#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_func.h"  // Ingredient 구조체 정의 포함되어야 함

#define MAX_LINE 1024
#define USER_KEY_LEN 16

// 로그인 시 사용자의 ID/PW를 CSV에서 검색하여, 일치하는 user_key 반환
char* Search_user_key(char *input_id, char *input_pw) {
    FILE *fp = fopen("c_project_data.csv", "r");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return "파일 열기 실패";
    }

    char line[MAX_LINE];

    // 첫 줄은 헤더. 읽고 무시
    if (fgets(line, sizeof(line), fp)) {}

    // 각 줄을 읽으며 ID/PW 확인
    while (fgets(line, sizeof(line), fp)) {
        char *id = strtok(line, ",");
        char *pw = strtok(NULL, ",");
        char *user_key = strtok(NULL, ",");

        // 일치하는 ID/PW 찾으면 해당 user_key 반환
        if (strcmp(id, input_id) == 0 && strcmp(pw, input_pw) == 0) {
            return user_key;
        }
    }

    fclose(fp);
    return NULL;  // 찾지 못함
}

// 회원가입 시 사용자 정보 저장 + 재료 CSV 파일 생성
int Save_user_key(char *input_id, char *input_pw) {
    FILE *fp = fopen("c_project_data.csv", "a+");  // 읽기/쓰기 모두 가능
    if (fp == NULL) {
        perror("파일 열기 실패");
        return 0;
    }

    // 기존 ID/PW가 있는지 확인
    char line[MAX_LINE];
    fseek(fp, 0, SEEK_SET);  // 파일 처음으로 이동

    while (fgets(line, sizeof(line), fp)) {
        char temp_line[MAX_LINE];
        strcpy(temp_line, line);  // strtok은 파괴적이므로 복사

        char *id = strtok(temp_line, ",");
        char *pw = strtok(NULL, ",");

        if (strcmp(id, input_id) == 0 && strcmp(pw, input_pw) == 0) {
            fclose(fp);
            return 0;  // 이미 존재함
        }
    }

    // 유저 키 생성: ID_PW 조합으로 간단하게
    char user_key[MAX_LINE];
    snprintf(user_key, sizeof(user_key), "%s_%s", input_id, input_pw);

    // CSV에 정보 추가
    fprintf(fp, "%s,%s,%s,\n", input_id, input_pw, user_key);
    fclose(fp);

    // [user_key]_ingredients.csv 생성
    char filename[128];
    snprintf(filename, sizeof(filename), "%s_ingredients.csv", user_key);

    FILE *ingredient_fp = fopen(filename, "w");  // 새로 만듦
    if (ingredient_fp == NULL) {
        perror("재료 파일 생성 실패");
        return 0;
    }
    // fprintf(ingredient_fp, "name,date,qty\n"); // 필요 시 헤더 추가 가능
    fclose(ingredient_fp);

    return 1;
}

// CSV 재료 목록을 한 개 문자열로 변환 (클라이언트 전송용)
char* load_ingredients_as_text(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("CSV 파일 열기 실패: %s\n", filename);
        return NULL;
    }

    char buffer[256];
    char* result = malloc(8192);  // 최대 8KB 할당
    if (!result) {
        fclose(file);
        return NULL;
    }
    result[0] = '\0';  // 초기화

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';  // 개행 제거
        strcat(result, buffer);
        strcat(result, "\n");
    }

    fclose(file);
    return result;  // 호출자가 free() 해야 함
}

// 재료를 이름→날짜 순으로 정렬하는 비교 함수 (qsort용)
int compare_ingredients(const void* a, const void* b) {
    const Ingredient* i1 = (const Ingredient*)a;
    const Ingredient* i2 = (const Ingredient*)b;

    int cmp = strcmp(i1->name, i2->name);
    if (cmp != 0) return cmp;
    return strcmp(i1->date, i2->date);
}

// 재료 저장 함수 (기존 재료와 병합)
int save_ingredient_to_file(const char* filename, const char* name, const char* date, int qty) {
    Ingredient list[100];
    int count = 0;

    // 기존 항목 로드
    FILE* fp = fopen(filename, "r");
    if (fp) {
        while (fscanf(fp, "%63[^,],%31[^,],%d\n", list[count].name, list[count].date, &list[count].qty) == 3) {
            count++;
        }
        fclose(fp);
    }

    // 기존 항목과 병합 (같은 이름+날짜면 수량 누적)
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].name, name) == 0 && strcmp(list[i].date, date) == 0) {
            list[i].qty += qty;
            found = 1;
            break;
        }
    }

    if (!found) {
        strncpy(list[count].name, name, sizeof(list[count].name));
        strncpy(list[count].date, date, sizeof(list[count].date));
        list[count].qty = qty;
        count++;
    }

    // 정렬 후 저장
    qsort(list, count, sizeof(Ingredient), compare_ingredients);

    fp = fopen(filename, "w");
    if (!fp) return 0;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%d\n", list[i].name, list[i].date, list[i].qty);
    }
    fclose(fp);
    return 1;
}

// 재료 삭제 함수 (이름 기준 수량 차감)
int delete_ingredient_from_file(const char* filename, const char* name, int qty_to_remove) {
    Ingredient list[100];
    int count = 0;

    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;

    while (fscanf(fp, "%63[^,],%31[^,],%d\n", list[count].name, list[count].date, &list[count].qty) == 3) {
        count++;
    }
    fclose(fp);

    // 정렬
    qsort(list, count, sizeof(Ingredient), compare_ingredients);

    // 수량 줄이기 (앞에 있는 항목부터 소진)
    for (int i = 0; i < count && qty_to_remove > 0; i++) {
        if (strcmp(list[i].name, name) == 0) {
            if (list[i].qty > qty_to_remove) {
                list[i].qty -= qty_to_remove;
                qty_to_remove = 0;
            } else {
                qty_to_remove -= list[i].qty;
                list[i].qty = 0;
            }
        }
    }

    // 0개 항목 제거
    Ingredient newList[100];
    int newCount = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].qty > 0) {
            newList[newCount++] = list[i];
        }
    }

    // 다시 저장
    fp = fopen(filename, "w");
    if (!fp) return 0;
    for (int i = 0; i < newCount; i++) {
        fprintf(fp, "%s,%s,%d\n", newList[i].name, newList[i].date, newList[i].qty);
    }
    fclose(fp);
    return 1;
}
