#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 200
#define MAX_NAME 50
#define MAX_DATE 20
#define MAX_INGREDIENTS 1000
#define CSV_FILE "ingredients.csv"

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct {
    char name[MAX_NAME];
    char expiration_date[MAX_DATE]; // YYYY.MM.DD
    int count;
} Ingredient;

// 공백 제거
void trim(char* str) {
    int start = 0;
    while (isspace((unsigned char)str[start])) start++;
    int end = strlen(str) - 1;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}

// 날짜 비교 (YYYY.MM.DD 기준)
int compare_date(const char* d1, const char* d2) {
    return strcmp(d1, d2);
}

// CSV 읽기
int load_ingredients(Ingredient list[], const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("파일 열기 실패");
        return -1;
    }

    char line[MAX_LINE];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        char name[MAX_NAME], date[MAX_DATE];
        int qty;

        if (sscanf(line, "%[^,],%[^,],%d", name, date, &qty) == 3) {
            trim(name);
            trim(date);
            strcpy(list[count].name, name);
            strcpy(list[count].expiration_date, date);
            list[count].count = qty;
            count++;
        }
    }

    fclose(file);
    return count;
}

// CSV 저장
void save_ingredients(Ingredient list[], int size, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("파일 저장 실패");
        return;
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%s,%s,%d\n", list[i].name, list[i].expiration_date, list[i].count);
    }

    fclose(file);
}

// 정렬 (이름 → 유통기한 오름차순)
void sort_ingredients(Ingredient list[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            int cmp = strcmp(list[i].name, list[j].name);
            if (cmp > 0 || (cmp == 0 && compare_date(list[i].expiration_date, list[j].expiration_date) > 0)) {
                Ingredient temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        }
    }
}

// 숫자 판단
int is_digit_char(char c) {
    return (c >= '0' && c <= '9');
}

// 토큰 파싱 (예: 감자2)
void parse_token(const char* token_raw, char* name_out, int* count_out) {
    char token[MAX_LINE];
    strcpy(token, token_raw);
    token[strcspn(token, "\n\r")] = '\0';
    trim(token);

    int len = strlen(token);
    int i = len - 1;
    while (i >= 0 && is_digit_char(token[i])) i--;

    strncpy(name_out, token, i + 1);
    name_out[i + 1] = '\0';
    trim(name_out);

    if (i < len - 1)
        *count_out = atoi(&token[i + 1]);
    else
        *count_out = 1;
}

// 재고 출력
void print_ingredients(Ingredient list[], int size) {
    if (size == 0) {
        printf("재고가 없습니다.\n");
        return;
    }

    printf("\n[현재 재고 목록]\n");
    printf("------------------------------\n");
    for (int i = 0; i < size; i++) {
        printf("%d. %s (유통기한: %s, 수량: %d)\n", i + 1, list[i].name, list[i].expiration_date, list[i].count);
    }
    printf("------------------------------\n\n");
}

// 재료 삭제 (유통기한 임박순)
void delete_ingredients(Ingredient list[], int* size, const char* target_name, int count_to_delete) {
    sort_ingredients(list, *size);
    int deleted = 0;

    for (int i = 0; i < *size && deleted < count_to_delete; i++) {
        if (strcmp(list[i].name, target_name) == 0) {
            int remove_count = (list[i].count <= (count_to_delete - deleted)) ? list[i].count : (count_to_delete - deleted);
            list[i].count -= remove_count;
            deleted += remove_count;

            if (list[i].count == 0) {
                for (int j = i; j < *size - 1; j++) {
                    list[j] = list[j + 1];
                }
                (*size)--;
                i--;
            }
        }
    }

    printf("\"%s\" %d개 삭제 완료 (요청: %d개)\n", target_name, deleted, count_to_delete);
}

// 메뉴 출력
void show_menu() {
    printf("=== 식재료 관리 시스템 ===\n");
    printf("1. 재고 확인\n");
    printf("2. 재료 소모 입력\n");
    printf("3. 프로그램 종료\n");
    printf("선택: ");
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Ingredient list[MAX_INGREDIENTS];
    int count = load_ingredients(list, CSV_FILE);
    if (count < 0) return 1;

    int choice;
    while (1) {
        show_menu();
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            print_ingredients(list, count);
        } else if (choice == 2) {
            char input_line[500];
            printf("소모한 식재료 입력 (예: 감자2, 양파 3):\n> ");
            fgets(input_line, sizeof(input_line), stdin);

            char* token = strtok(input_line, ",");
            while (token != NULL) {
                char name[MAX_NAME];
                int qty;

                parse_token(token, name, &qty);
                delete_ingredients(list, &count, name, qty);

                token = strtok(NULL, ",");
            }

            save_ingredients(list, count, CSV_FILE);
        } else if (choice == 3) {
            printf("프로그램을 종료합니다.\n");
            break;
        } else {
            printf("잘못된 입력입니다. 다시 선택하세요.\n");
        }
    }

    return 0;
}