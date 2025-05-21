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

    char send_data[1024];

    char *encoding_pw = hash_password(pw);

    snprintf(send_data, sizeof(send_data),
        "%c,%s,%s",stats,id,encoding_pw);

    free(encoding_pw);

    send(sockfd, send_data, strlen(send_data), 0);

    memset(send_data, 0, sizeof(send_data));

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

char** get_Ingredients_split(char stats, const char* user_key, int* outCount) {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[2048];
    int bytes_received;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 실패\n");
        *outCount = 0;
        return NULL;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        *outCount = 0;
        return NULL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("서버 연결 실패\n");
        closesocket(sockfd);
        WSACleanup();
        *outCount = 0;
        return NULL;
    }

    char send_data[1024];
    snprintf(send_data, sizeof(send_data), "%c,%s", stats, user_key);
    send(sockfd, send_data, strlen(send_data), 0);
    memset(send_data, 0, sizeof(send_data));

    char* result = (char*)malloc(8192);
    if (!result) {
        closesocket(sockfd);
        WSACleanup();
        *outCount = 0;
        return NULL;
    }
    result[0] = '\0';

    do {
        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            printf("받은 데이터: %s\n", buffer);
            buffer[bytes_received] = '\0';
            strcat(result, buffer);
        }
    } while (bytes_received > 0);

    closesocket(sockfd);
    WSACleanup();

    // 문자열 분리
    char** lines = (char** )malloc(sizeof(char*) * 100);  // 최대 100줄 가정
    int count = 0;

    char* line = strtok(result, "\n");
    while (line != NULL) {
        lines[count] = (char*)malloc(strlen(line) + 1);
        strcpy(lines[count], line);
        count++;
        line = strtok(NULL, "\n");
    }

    free(result);
    *outCount = count;
    return lines;
}


