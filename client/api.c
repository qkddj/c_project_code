// http_client_win.c
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "client_func.h"

#pragma comment(lib, "ws2_32.lib")  // WinSock 라이브러리 링크

#define HOST "127.0.0.1"  // 서버 주소 (로컬호스트)
#define PORT 8080         // 서버 포트 번호

// 로그인 또는 회원가입 요청을 보내고 user_key를 반환
char* send_idpw(char stats, const char* id, const char* pw) {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[2048];
    int bytes_received;
    static char user_key[50];  // 반환할 사용자 키 저장 (정적)

    // WinSock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 실패\n");
        return NULL;
    }

    // TCP 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        return NULL;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    // 서버에 연결 요청
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("서버 연결 실패\n");
        closesocket(sockfd);
        WSACleanup();
        return NULL;
    }

    // 전송할 데이터 구성: stats,id,password(암호화)
    char send_data[1024];
    char *encoding_pw = hash_password(pw);  // 비밀번호 해싱 처리
    snprintf(send_data, sizeof(send_data), "%c,%s,%s", stats, id, encoding_pw);
    free(encoding_pw);

    // 서버로 데이터 전송
    send(sockfd, send_data, strlen(send_data), 0);
    memset(send_data, 0, sizeof(send_data));

    // 서버 응답 수신
    do {
        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            strcpy(user_key, buffer);  // user_key에 저장
        }
    } while (bytes_received > 0);

    // 자원 해제
    closesocket(sockfd);
    WSACleanup();
    return user_key;
}

// 재료 목록 요청 및 줄 단위로 분리
char** get_Ingredients_split(char stats, const char* user_key, int* outCount) {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[2048];
    int bytes_received;

    // WinSock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 실패\n");
        *outCount = 0;
        return NULL;
    }

    // TCP 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        *outCount = 0;
        return NULL;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    // 서버 연결 요청
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("서버 연결 실패\n");
        closesocket(sockfd);
        WSACleanup();
        *outCount = 0;
        return NULL;
    }

    // 요청 전송: stats + user_key
    char send_data[1024];
    snprintf(send_data, sizeof(send_data), "%c,%s", stats, user_key);
    send(sockfd, send_data, strlen(send_data), 0);
    memset(send_data, 0, sizeof(send_data));

    // 응답 전체 수신
    char* result = (char*)malloc(8192);  // 응답을 저장할 버퍼
    if (!result) {
        closesocket(sockfd);
        WSACleanup();
        *outCount = 0;
        return NULL;
    }
    result[0] = '\0';

    // 데이터 수신
    do {
        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            strcat(result, buffer);
        }
    } while (bytes_received > 0);

    // 연결 종료
    closesocket(sockfd);
    WSACleanup();

    // 줄 단위로 분리
    char** lines = (char**)malloc(sizeof(char*) * 100);  // 최대 100줄
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

// 식재료 추가 요청
void add_ingredient_api(const char* user_key, const char* name, const char* date, int qty) {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char send_data[256], recv_buffer[128];
    int bytes_received;

    // WinSock 초기화
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup 실패\n");
        return;
    }

    // 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        return;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    // 서버 연결
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("서버 연결 실패\n");
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // 데이터 구성 및 전송 (3은 추가 명령)
    snprintf(send_data, sizeof(send_data), "3,%s,%s,%s,%d", user_key, name, date, qty);
    send(sockfd, send_data, strlen(send_data), 0);

    // 서버 응답 수신 (선택)
    bytes_received = recv(sockfd, recv_buffer, sizeof(recv_buffer) - 1, 0);
    if (bytes_received > 0) {
        recv_buffer[bytes_received] = '\0';
        printf("서버 응답: %s\n", recv_buffer);
    }

    // 종료
    closesocket(sockfd);
    WSACleanup();
}

// 식재료 삭제 요청
void delete_ingredient_api(const char* user_key, const char* name, int qty) {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char send_data[256], recv_buffer[128];
    int bytes_received;

    // WinSock 초기화
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup 실패\n");
        return;
    }

    // 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        return;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    // 서버 연결
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("서버 연결 실패\n");
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // 데이터 구성 및 전송 (4는 삭제 명령)
    snprintf(send_data, sizeof(send_data), "4,%s,%s,%d", user_key, name, qty);
    send(sockfd, send_data, strlen(send_data), 0);

    // 응답 수신 (선택)
    bytes_received = recv(sockfd, recv_buffer, sizeof(recv_buffer) - 1, 0);
    if (bytes_received > 0) {
        recv_buffer[bytes_received] = '\0';
        printf("서버 응답: %s\n", recv_buffer);
    }

    // 종료
    closesocket(sockfd);
    WSACleanup();
}
