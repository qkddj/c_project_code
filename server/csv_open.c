#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_func.h"

#define MAX_LINE 1024
#define USER_KEY_LEN 16

char* Search_user_key(char *input_id,char *input_pw) {
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
        char *user_key = strtok(NULL, ",");

        if (strcmp(id, input_id) == 0 && strcmp(pw, input_pw) == 0) {
            return user_key;
        }
    }

    fclose(fp);
    return NULL;
}

int Save_user_key(char *input_id, char *input_pw) {
    FILE *fp = fopen("c_project_data.csv", "a+");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return 0;
    }

    char line[MAX_LINE];
    fseek(fp, 0, SEEK_SET);

    while (fgets(line, sizeof(line), fp)) {
        // strtok은 원본 문자열을 파괴하므로 복사본을 사용
        char temp_line[MAX_LINE];
        strcpy(temp_line, line);

        char *id = strtok(temp_line, ",");
        char *pw = strtok(NULL, ",");

        if (strcmp(id, input_id) == 0 && strcmp(pw, input_pw) == 0) {
            fclose(fp);
            return 0;
        }
    }

    char user_key[MAX_LINE];
    snprintf(user_key, sizeof(user_key), "%s_%s", input_id, input_pw);

    // 사용자 정보 저장
    fprintf(fp, "%s,%s,%s,\n", input_id, input_pw, user_key);
    fclose(fp);

    // [user_key]_ingredients.csv 파일 생성
    char filename[128];
    snprintf(filename, sizeof(filename), "%s_ingredients.csv", user_key);

    FILE *ingredient_fp = fopen(filename, "w");  // 새 파일 생성
    if (ingredient_fp == NULL) {
        perror("재료 파일 생성 실패");
        return 0;
    }

    // 필요 시 초기 헤더 작성 가능
    // fprintf(ingredient_fp, "ingredient,expiration_date\n");

    fclose(ingredient_fp);

    return 1;
}


char* load_ingredients_as_text(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("CSV 파일 열기 실패: %s\n", filename);
        return NULL;
    }

    char buffer[256];
    char* result = malloc(8192);  // 최대 8KB까지 문자열 저장
    if (!result) {
        fclose(file);
        return NULL;
    }
    result[0] = '\0';  // 초기화

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';  // 줄바꿈 제거
        strcat(result, buffer);
        strcat(result, "\n");  // 줄 구분
    }

    fclose(file);
    return result;  // 호출자가 free() 해야 함
}

int compare_ingredients(const void* a, const void* b) {
    const Ingredient* i1 = (const Ingredient*)a;
    const Ingredient* i2 = (const Ingredient*)b;

    int cmp = strcmp(i1->name, i2->name);
    if (cmp != 0) return cmp;
    return strcmp(i1->date, i2->date);
}

int save_ingredient_to_file(const char* filename, const char* name, const char* date, int qty) {
    Ingredient list[100];
    int count = 0;

    // 1. 기존 파일 읽기
    FILE* fp = fopen(filename, "r");
    if (fp) {
        while (fscanf(fp, "%63[^,],%31[^,],%d\n", list[count].name, list[count].date, &list[count].qty) == 3) {
            count++;
        }
        fclose(fp);
    }

    // 2. 이미 있는 항목이면 수량 누적
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].name, name) == 0 && strcmp(list[i].date, date) == 0) {
            list[i].qty += qty;
            found = 1;
            break;
        }
    }

    // 3. 새 항목이면 추가
    if (!found) {
        strncpy(list[count].name, name, sizeof(list[count].name));
        strncpy(list[count].date, date, sizeof(list[count].date));
        list[count].qty = qty;
        count++;
    }

    // 4. 이름 → 유통기한 기준 정렬
    qsort(list, count, sizeof(Ingredient), compare_ingredients);

    // 5. 다시 파일에 저장
    fp = fopen(filename, "w");
    if (!fp) return 0;

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%d\n", list[i].name, list[i].date, list[i].qty);
    }

    fclose(fp);
    return 1;
}

int delete_ingredient_from_file(const char* filename, const char* name, int qty_to_remove) {
    Ingredient list[100];
    int count = 0;

    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;

    while (fscanf(fp, "%63[^,],%31[^,],%d\n", list[count].name, list[count].date, &list[count].qty) == 3) {
        count++;
    }
    fclose(fp);

    qsort(list, count, sizeof(Ingredient), compare_ingredients);

    // 수량 차감
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

    // 0개 된 항목 제거
    Ingredient newList[100];
    int newCount = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].qty > 0) {
            newList[newCount++] = list[i];
        }
    }

    // 파일 덮어쓰기
    fp = fopen(filename, "w");
    if (!fp) return 0;
    for (int i = 0; i < newCount; i++) {
        fprintf(fp, "%s,%s,%d\n", newList[i].name, newList[i].date, newList[i].qty);
    }
    fclose(fp);
    return 1;
}
