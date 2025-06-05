#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DATE_LEN 11
#define CSV_PATH "ingredients.csv"

typedef struct {
    char name[50];
    int quantity;
    char expiry[DATE_LEN];
} Ingredient;

void formatExpiry(char *dest, const char *src) {
    // src: YYYYMMDD, dest: YYYY.MM.DD (null-terminated)
    snprintf(dest, DATE_LEN, "%.4s.%.2s.%.2s", src, src + 4, src + 6);
}

int isValidQuantity(const char *str) {
    if (str[0] == '-' || str[0] == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

int isValidExpiry(const char *str) {
    if (strlen(str) != 8) return 0;
    for (int i = 0; i < 8; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

void appendToCSV(const Ingredient *ing) {
    FILE *fp = fopen(CSV_PATH, "a");
    if (!fp) {
        perror("파일 열기 실패");
        return;
    }
    fprintf(fp, "%s,%s,%d\n", ing->name, ing->expiry, ing->quantity);
    fclose(fp);
}

int main() {
    char inputName[50];
    char inputQty[20];
    char inputExpiry[20];
    Ingredient ing;

    printf("재료 이름 입력 (종료하려면 빈칸 엔터):\n");

    while (1) {
        printf("이름: ");
        if (!fgets(inputName, sizeof(inputName), stdin)) break;
        // 줄바꿈 제거
        inputName[strcspn(inputName, "\n")] = '\0';

        if (strlen(inputName) == 0) break;

        // 수량 입력
        while (1) {
            printf("수량: ");
            if (!fgets(inputQty, sizeof(inputQty), stdin)) return 1;
            inputQty[strcspn(inputQty, "\n")] = '\0';
            if (isValidQuantity(inputQty)) break;
            printf("올바른 양수를 입력하세요.\n");
        }
        ing.quantity = atoi(inputQty);

        // 유통기한 입력
        while (1) {
            printf("유통기한 (YYYYMMDD): ");
            if (!fgets(inputExpiry, sizeof(inputExpiry), stdin)) return 1;
            inputExpiry[strcspn(inputExpiry, "\n")] = '\0';
            if (isValidExpiry(inputExpiry)) break;
            printf("YYYYMMDD 형식으로 숫자 8자리만 입력하세요.\n");
        }
        formatExpiry(ing.expiry, inputExpiry);

        strncpy(ing.name, inputName, sizeof(ing.name) - 1);
        ing.name[sizeof(ing.name) - 1] = '\0';

        // 입력 완료 후 즉시 CSV에 추가
        appendToCSV(&ing);
        printf("추가 완료!\n\n");
    }

    printf("입력 종료. 프로그램 종료.\n");
    return 0;
}
