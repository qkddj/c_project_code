#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 재료 정보 구조체
typedef struct {
    char name[32];     // 재료 이름
    int year, month, day; // 유통기한
    int quantity;      // 수량
} Ingredient;

// 날짜 비교 함수: (y1,m1,d1)가 더 빠르면 음수 반환
int compareDate(int y1, int m1, int d1, int y2, int m2, int d2) {
    if (y1 != y2) return y1 - y2;
    if (m1 != m2) return m1 - m2;
    return d1 - d2;
}

// 문자열 배열에서 재료별로 수량을 합산하고, 가장 빠른 유통기한만 유지하는 함수
char** sum_ingredients(const char* raw[], int rawCount, int* outCount) {
    Ingredient temp[100];    // 중간 저장용 배열
    int tempCount = 0;

    for (int i = 0; i < rawCount; i++) {
        char name[32];
        int year, month, day, qty;

        // 입력 문자열 파싱: 예) "상추,2025.06.01,3"
        if (sscanf(raw[i], "%[^,],%d.%d.%d,%d", name, &year, &month, &day, &qty) != 5) {
            printf("파싱 실패: %s\n", raw[i]);
            continue;
        }

        int found = 0;

        // 기존 재료 목록에 같은 이름이 있으면 수량 합산, 유통기한 최신화
        for (int j = 0; j < tempCount; j++) {
            if (strcmp(temp[j].name, name) == 0) {
                temp[j].quantity += qty;

                // 더 이른 유통기한으로 갱신
                if (compareDate(year, month, day, temp[j].year, temp[j].month, temp[j].day) < 0) {
                    temp[j].year = year;
                    temp[j].month = month;
                    temp[j].day = day;
                }

                found = 1;
                break;
            }
        }

        // 처음 등장한 재료라면 새로 추가
        if (!found) {
            strcpy(temp[tempCount].name, name);
            temp[tempCount].year = year;
            temp[tempCount].month = month;
            temp[tempCount].day = day;
            temp[tempCount].quantity = qty;
            tempCount++;
        }
    }

    // 결과 문자열 포맷으로 변환
    char** result = malloc(sizeof(char*) * tempCount);
    for (int i = 0; i < tempCount; i++) {
        result[i] = malloc(100);
        // 예: "상추/2025.06.01/5개"
        sprintf(result[i], "%s/%04d.%02d.%02d/%d개",
                temp[i].name, temp[i].year, temp[i].month, temp[i].day, temp[i].quantity);
    }

    *outCount = tempCount;  // 결과 개수 저장
    return result;          // 결과 배열 반환
}
