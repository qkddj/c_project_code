#include <stdio.h>
#include <string.h>

#define MAX_SUGGESTIONS 10

const char* keywords[] = {
    "카카오", "카메라", "카페", "캠핑카", "카트라이더",
    "카드", "카레", "카우보이", "카센터", "카디건"
};
const int keyword_count = sizeof(keywords) / sizeof(keywords[0]);

void suggest(const char* input) {
    // printf("추천 검색어:\n");
    int count = 0;
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(keywords[i], input) == 0){
            printf("최종 검색어:%s\n", keywords[i]);
            break;
        }

        if (strncmp(keywords[i], input, strlen(input)) == 0) {
            printf(" - %s\n", keywords[i]);
            count++;
            if (count >= MAX_SUGGESTIONS) break;
        }
    }
}

int main() {
    char input[100];
    printf("검색어 입력 (끝내려면 'exit'):\n");

    while (1) {
        printf(">> ");
        scanf("%s", input);

        if (strcmp(input, "exit") == 0) break;

        suggest(input);
    }

    printf("프로그램 종료.\n");
    return 0;
}
