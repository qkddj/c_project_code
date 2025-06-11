#ifndef OPEN_H
#define OPEN_H

// 재료 정보를 저장하는 구조체
typedef struct {
    char name[64];   // 재료 이름 (예: "상추")
    char date[32];   // 유통기한 또는 날짜 문자열 (예: "2025.06.11")
    int qty;         // 수량 (예: 3개)
} Ingredient;

// 로그인 시 사용자 ID/PW로 user_key 검색
// 성공 시: user_key 문자열 반환 / 실패 시: NULL 또는 오류 문자열
char* Search_user_key(char *input_id, char *input_pw);

// 회원가입 시 사용자 정보 저장 + 재료 CSV 생성
// 성공 시: 1 / 실패 시: 0
int Save_user_key(char *input_id, char *input_pw);

// CSV 파일 내용을 전체 문자열로 읽어오기
// 반환값은 malloc된 문자열 (호출자가 free 해야 함)
// 실패 시: NULL 반환
char* load_ingredients_as_text(const char* filename);

// 재료 저장 함수
// 이미 존재하면 수량 누적, 없으면 새 항목 추가
// 성공 시: 1 / 실패 시: 0
int save_ingredient_to_file(const char* filename, const char* name, const char* date, int qty);

// 재료 삭제 함수
// 동일 이름 재료 중 오래된 항목부터 수량 차감
// 성공 시: 1 / 실패 시: 0
int delete_ingredient_from_file(const char* filename, const char* name, int qty);

#endif  // OPEN_H
