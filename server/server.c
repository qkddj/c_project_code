// http_server_win.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "server_func.h"

#pragma comment(lib, "ws2_32.lib") // 링커에 ws2_32.lib 연결 (WinSock 사용 시 필수)

#define PORT 8080  // 서버가 열릴 포트 번호

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[2048];  // 클라이언트 요청 수신 버퍼

    // WinSock 초기화
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 로컬 서버의 IP 주소 출력
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct addrinfo hints = {0}, *res, *p;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(hostname, NULL, &hints, &res) == 0) {
            for (p = res; p != NULL; p = p->ai_next) {
                struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(addr->sin_addr), ipstr, sizeof(ipstr));
                printf("서버 IP 주소: %s\n", ipstr);
            }
            freeaddrinfo(res);
        }
    }

    // 서버 소켓 생성
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        return 1;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓 바인딩 및 연결 대기 시작
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    printf("HTTP 서버가 포트 %d에서 대기 중...\n", PORT);

    // 클라이언트 연결 루프
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            printf("클라이언트 연결 실패\n");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));  // 수신 버퍼 초기화

        recv(client_fd, buffer, sizeof(buffer) - 1, 0);  // 요청 수신
        printf("클라이언트 요청: %s\n", buffer);

        // 요청의 첫 번째 필드를 명령 코드로 사용
        char *stats = strtok(buffer, ",");

        // 로그인 요청 (0)
        if (strcmp(stats, "0") == 0){
            char *id = strtok(NULL, ",");
            char *pw = strtok(NULL, ",");
            char* data = Search_user_key(id, pw);

            if (data != NULL){
                send(client_fd, data, (int)strlen(data), 0);
                printf("반환 값: %s\n", data);
            } else {
                const char *response = "0";
                send(client_fd, response, (int)strlen(response), 0);
                printf("반환 값: %s\n", response);
            }

            closesocket(client_fd);
        }

        // 회원가입 요청 (1)
        else if (strcmp(stats, "1") == 0){
            char *id = strtok(NULL, ",");
            char *pw = strtok(NULL, ",");
            if (Save_user_key(id, pw) == 1){
                const char *response = "회원가입 성공\n";
                send(client_fd, response, (int)strlen(response), 0);
            } else {
                const char *response = "회원가입 실패\n";
                send(client_fd, response, (int)strlen(response), 0);
            }

            closesocket(client_fd);
        }

        // 식재료 전체 요청 (2)
        else if (strcmp(stats, "2") == 0){
            char *user_key = strtok(NULL, ",");

            char filename[64];
            sprintf(filename, "%s_ingredients.csv", user_key);

            printf("파일 이름: %s\n", filename);

            const char *response = load_ingredients_as_text(filename);
            send(client_fd, response, (int)strlen(response), 0);
            printf("반환 값: %s\n", response);

            closesocket(client_fd);
        }

        // 식재료 추가 요청 (3)
        else if (strcmp(stats, "3") == 0) {
            char* user_key = strtok(NULL, ",");
            char* name = strtok(NULL, ",");
            char* date = strtok(NULL, ",");
            char* qty_str = strtok(NULL, ",");
            int qty = atoi(qty_str);

            char filename[64];
            sprintf(filename, "%s_ingredients.csv", user_key);

            int result = save_ingredient_to_file(filename, name, date, qty);

            if (result == 1) {
                const char* response = "추가 성공\n";
                send(client_fd, response, (int)strlen(response), 0);
            } else {
                const char* response = "추가 실패\n";
                send(client_fd, response, (int)strlen(response), 0);
            }

            closesocket(client_fd);
        }

        // 식재료 삭제 요청 (4)
        else if (strcmp(stats, "4") == 0) {
            char *user_key = strtok(NULL, ",");
            char *name = strtok(NULL, ",");
            int qty = atoi(strtok(NULL, ","));  // 수량 문자열 → 정수 변환

            char filename[64];
            sprintf(filename, "%s_ingredients.csv", user_key);

            if (delete_ingredient_from_file(filename, name, qty)) {
                const char* response = "삭제 성공\n";
                send(client_fd, response, (int)strlen(response), 0);
            } else {
                const char* response = "삭제 실패\n";
                send(client_fd, response, (int)strlen(response), 0);
            }

            closesocket(client_fd);
        }

        // 그 외 처리 없음
    }

    // 서버 종료
    closesocket(server_fd);
    WSACleanup(); // WinSock 정리
    return 0;
}
