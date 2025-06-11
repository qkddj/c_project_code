// http_client_win.c
#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "client_func.h"

// SHA-256 해시 함수를 이용한 비밀번호 해싱
char* hash_password(const char *password) {
    EVP_MD_CTX *mdctx;                       // 메시지 다이제스트 컨텍스트
    const EVP_MD *md = EVP_sha256();        // SHA-256 알고리즘 선택
    unsigned char *hash = malloc(EVP_MAX_MD_SIZE);  // 해시 결과 저장
    unsigned int hash_len;

    if (!hash) return NULL;

    mdctx = EVP_MD_CTX_new();               // 컨텍스트 생성
    EVP_DigestInit_ex(mdctx, md, NULL);     // 초기화
    EVP_DigestUpdate(mdctx, password, strlen(password)); // 데이터 추가
    EVP_DigestFinal_ex(mdctx, hash, &hash_len); // 해싱 완료
    EVP_MD_CTX_free(mdctx);                 // 컨텍스트 해제

    // 해시 결과를 16진수 문자열로 변환
    char *hex = malloc(hash_len * 2 + 1);
    if (!hex) {
        free(hash);
        return NULL;
    }

    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(hex + i * 2, "%02x", hash[i]);
    }
    hex[hash_len * 2] = '\0';

    free(hash);
    return hex;  // 힙에 할당된 문자열 (사용 후 free 필요)
}
