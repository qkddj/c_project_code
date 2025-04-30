// client_func.h
#ifndef MYFUNC_H
#define MYFUNC_H

int send_idpw(char stats,char id[20],char pw[20]);  // 함수 선언
char* hash_password(const char *password);

#endif
