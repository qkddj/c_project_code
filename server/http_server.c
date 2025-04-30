// http_server_win.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "server_func.h"

#pragma comment(lib, "ws2_32.lib") // 링커에 ws2_32.lib 연결

#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[2048];

    WSAStartup(MAKEWORD(2, 2), &wsaData); // WinSock 초기화

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("HTTP 서버가 포트 %d에서 대기 중...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            printf("클라이언트 연결 실패\n");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));

        recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        printf("클라이언트 요청:\n%s\n", buffer);

        char *stats = strtok(buffer, ",");
        char *id = strtok(NULL, ",");
        char *pw = strtok(NULL, ",");

        if (strcmp(stats, "0") == 0){
            char *data = Search_user_data(id,pw);

            if (data != NULL){
                const char *response = data;

                send(client_fd, response, (int)strlen(response), 0);
            }
            else{
                const char *response =
                "로그인 실패\n";

                send(client_fd, response, (int)strlen(response), 0);
            }

            closesocket(client_fd);
        }
        
        else if (strcmp(stats, "1") == 0){
            if (Save_user_data(id,pw) == 1){
                const char *response = "회원가입 성공\n";
                send(client_fd, response, (int)strlen(response), 0);
            }
            else{
                const char *response = "회원가입 실패\n";
                send(client_fd, response, (int)strlen(response), 0);
            }

            closesocket(client_fd);
        }
    }

    closesocket(server_fd);
    WSACleanup(); // WinSock 종료
    return 0;
}
