// http_client_win.c
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "client_func.h"

#pragma comment(lib, "ws2_32.lib")  // WinSock 라이브러리 링크

#define HOST "127.0.0.1"
#define PORT 8080

char* send_idpw(char stats, const char* id, const char* pw) {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[2048];
    int bytes_received;
    static char user_key[50];
    int got_response = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 실패\n");
        return NULL;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        return NULL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("서버 연결 실패\n");
        closesocket(sockfd);
        WSACleanup();
        return NULL;
    }

    char http_get[1024];

    char *encoding_pw = hash_password(pw);

    snprintf(http_get, sizeof(http_get),
        "%c,%s,%s",stats,id,encoding_pw);

    free(encoding_pw);

    send(sockfd, http_get, strlen(http_get), 0);

    memset(http_get, 0, sizeof(http_get));

    do {
        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            strcpy(user_key, buffer);
        }
    } while (bytes_received > 0);

    closesocket(sockfd);
    WSACleanup();
    return user_key;
}
