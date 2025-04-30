#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "client_func.h"

char* hash_password(const char *password) {
    EVP_MD_CTX *mdctx;
    const EVP_MD *md = EVP_sha256();
    unsigned char *hash = malloc(EVP_MAX_MD_SIZE);
    unsigned int hash_len;

    if (!hash) return NULL;

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, password, strlen(password));
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);

    // 해시값을 문자열로 바꿈 (16진수)
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
    return hex;
}
