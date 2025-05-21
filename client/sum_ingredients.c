#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[32];
    int year, month, day;
    int quantity;
} Ingredient;

int compareDate(int y1, int m1, int d1, int y2, int m2, int d2) {
    if (y1 != y2) return y1 - y2;
    if (m1 != m2) return m1 - m2;
    return d1 - d2;
}

char** sum_ingredients(const char* raw[], int rawCount, int* outCount) {
    Ingredient temp[100];
    int tempCount = 0;

    for (int i = 0; i < rawCount; i++) {
        char name[32];
        int year, month, day, qty;

        // "상추,2025.06.01,3" 형식 처리
        if (sscanf(raw[i], "%[^,],%d.%d.%d,%d", name, &year, &month, &day, &qty) != 5) {
            printf("파싱 실패: %s\n", raw[i]);
            continue;
        }

        int found = 0;
        for (int j = 0; j < tempCount; j++) {
            if (strcmp(temp[j].name, name) == 0) {
                temp[j].quantity += qty;
                if (compareDate(year, month, day, temp[j].year, temp[j].month, temp[j].day) < 0) {
                    temp[j].year = year;
                    temp[j].month = month;
                    temp[j].day = day;
                }
                found = 1;
                break;
            }
        }

        if (!found) {
            strcpy(temp[tempCount].name, name);
            temp[tempCount].year = year;
            temp[tempCount].month = month;
            temp[tempCount].day = day;
            temp[tempCount].quantity = qty;
            tempCount++;
        }
    }

    // 최종 문자열 포맷 배열 생성
    char** result = malloc(sizeof(char*) * tempCount);
    for (int i = 0; i < tempCount; i++) {
        result[i] = malloc(100);
        sprintf(result[i], "%s/%04d.%02d.%02d/%d개", temp[i].name, temp[i].year, temp[i].month, temp[i].day, temp[i].quantity);
    }

    *outCount = tempCount;
    return result;
}

