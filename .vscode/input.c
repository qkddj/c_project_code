#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#define INITIAL_CAPACITY 4
#define DATE_LEN 11
#define CSV_PATH "ingredients.csv"

typedef struct {
    char name[50];
    int quantity;
    char expiry[DATE_LEN];  // YYYY.MM.DD
} Ingredient;

typedef struct {
    Ingredient *items;
    int size;
    int capacity;
} IngredientList;

void initList(IngredientList *list) {
    list->items = malloc(sizeof(Ingredient) * INITIAL_CAPACITY);
    if (!list->items) {
        perror("메모리 할당 실패");
        exit(EXIT_FAILURE);
    }
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
}

void resizeList(IngredientList *list) {
    int newCap = list->capacity * 2;
    Ingredient *tmp = realloc(list->items, sizeof(Ingredient) * newCap);
    if (!tmp) {
        perror("메모리 재할당 실패");
        exit(EXIT_FAILURE);
    }
    list->items = tmp;
    list->capacity = newCap;
}

int findIngredient(const IngredientList *list, const char *name, const char *expiry) {
    for (int i = 0; i < list->size; i++) {
        if (strcmp(list->items[i].name, name) == 0 && strcmp(list->items[i].expiry, expiry) == 0) {
            return i;
        }
    }
    return -1;
}

void addIngredient(IngredientList *list, const char *name, const char *expiry, int qty) {
    int idx = findIngredient(list, name, expiry);
    if (idx != -1) {
        list->items[idx].quantity += qty;
        return;
    }

    if (list->size >= list->capacity) {
        resizeList(list);
    }
    strncpy(list->items[list->size].name, name, sizeof(list->items[list->size].name) - 1);
    list->items[list->size].name[sizeof(list->items[list->size].name) - 1] = '\0';

    strncpy(list->items[list->size].expiry, expiry, DATE_LEN - 1);
    list->items[list->size].expiry[DATE_LEN - 1] = '\0';

    list->items[list->size].quantity = qty;
    list->size++;
}

void loadFromCSV(IngredientList *list) {
    FILE *fp = fopen(CSV_PATH, "r");
    if (!fp) return;

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        char name[50], expiry[DATE_LEN];
        int qty;
        if (sscanf(line, "%49[^,],%10[^,],%d", name, expiry, &qty) == 3) {
            addIngredient(list, name, expiry, qty);
        }
    }
    fclose(fp);
}

void saveToCSV(const IngredientList *list) {
    FILE *fp = fopen(CSV_PATH, "w");
    if (!fp) {
        perror("CSV 저장 실패");
        return;
    }
    for (int i = 0; i < list->size; i++) {
        fprintf(fp, "%s,%s,%d\n",
            list->items[i].name,
            list->items[i].expiry,
            list->items[i].quantity);
    }
    fclose(fp);
}

int compareByNameThenExpiry(const void *a, const void *b) {
    const Ingredient *ia = a;
    const Ingredient *ib = b;
    int nameCmp = strcoll(ia->name, ib->name);
    if (nameCmp != 0) return nameCmp;
    return strcmp(ia->expiry, ib->expiry);  // 유통기한 오름차순
}

void formatDate(const char *input, char *output) {
    if (strlen(input) == 8) {
        snprintf(output, DATE_LEN, "%.4s.%.2s.%.2s", input, input + 4, input + 6);
    } else {
        output[0] = '\0';
    }
}

int isValidDateFormat(const char *str) {
    if (strlen(str) != 8) return 0;
    for (int i = 0; i < 8; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    int month = (str[4]-'0')*10 + (str[5]-'0');
    int day = (str[6]-'0')*10 + (str[7]-'0');
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    return 1;
}

int isValidQuantity(const char *str) {
    if (str[0] == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return atoi(str) > 0;
}

void printSeparator() {
    printf("========================================\n");
}

int main() {
    setlocale(LC_COLLATE, "ko_KR.UTF-8");

    IngredientList list;
    initList(&list);
    loadFromCSV(&list);

    printf("=== 재료 입력 프로그램 ===\n");
    printf("재료 이름을 입력하세요. 빈 줄 입력 시 종료합니다.\n");

    while (1) {
        char name[50];
        char expiryInput[20];
        char expiry[DATE_LEN];
        char qtyInput[20];
        int quantity;

        printSeparator();

        printf("재료 이름: ");
        if (!fgets(name, sizeof(name), stdin)) break;
        name[strcspn(name, "\n")] = '\0';
        if (strlen(name) == 0) {
            printf("입력 종료합니다.\n");
            break;
        }

        while (1) {
            printf("유통기한 (YYYYMMDD): ");
            if (!fgets(expiryInput, sizeof(expiryInput), stdin)) break;
            expiryInput[strcspn(expiryInput, "\n")] = '\0';

            if (isValidDateFormat(expiryInput)) {
                formatDate(expiryInput, expiry);
                break;
            } else {
                printf(">>> 잘못된 유통기한 형식입니다! 8자리 숫자(YYYYMMDD)를 입력하세요.\n");
            }
        }

        while (1) {
            printf("수량 (양수 정수): ");
            if (!fgets(qtyInput, sizeof(qtyInput), stdin)) break;
            qtyInput[strcspn(qtyInput, "\n")] = '\0';

            if (isValidQuantity(qtyInput)) {
                quantity = atoi(qtyInput);
                break;
            } else {
                printf(">>> 잘못된 수량입니다! 양수 정수를 입력하세요.\n");
            }
        }

        addIngredient(&list, name, expiry, quantity);

        // 이름 + 유통기한 정렬 후 실시간 저장
        qsort(list.items, list.size, sizeof(Ingredient), compareByNameThenExpiry);
        saveToCSV(&list);

        printf("▶ %s, %d개, 유통기한 %s 가 성공적으로 추가되었습니다.\n", name, quantity, expiry);
    }

    printSeparator();
    printf("총 %d개의 재료가 저장됩니다.\n", list.size);
    printf("재료 목록이 CSV 파일에 저장되었습니다. 프로그램을 종료합니다.\n");
    printSeparator();

    free(list.items);
    return 0;
}
