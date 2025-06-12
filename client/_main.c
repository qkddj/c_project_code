#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "client_func.h"

int main(int argc, char* argv[]) {
    int page = 0;         // 현재 페이지 상태를 저장 (0: 로그인, 1: 메뉴 등)
    int running = 1;      // 프로그램 실행 상태 (0이면 종료)

    char* user_key = NULL; // 로그인한 사용자 정보를 저장하는 포인터

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError());
        return 1;
    }

    // TTF (폰트 렌더링) 초기화
    if (TTF_Init() < 0) {
        printf("TTF 초기화 실패: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // 윈도우 생성
    SDL_Window* window = SDL_CreateWindow("Cse Restaurant", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        printf("윈도우 생성 실패: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // 렌더러 생성
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("렌더러 생성 실패: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // 메인 루프: 페이지 번호에 따라 화면 전환
    while (running) {
        switch(page){
            case 0: {
                // 로그인 화면 실행
                user_key = runLoginScreen(window, renderer);

                // "0"이 아니면 로그인 성공, 다음 페이지로
                if (strcmp(user_key, "0") != 0) {
                    page = 1;  // 메인 메뉴로 이동
                }
                else {
                    running = 0;  // 프로그램 종료
                }
                break;
            }
            case 1: {
                // 메인 메뉴 화면 실행 (리턴된 값에 따라 다음 페이지 결정)
                page = runMainMenuScreen(window, renderer);
                break;
            }
            case 2: {
                // 식재료 목록 보기 화면 실행
                page = runIngredientScreen(window, renderer, user_key);
                break;
            }
            case 3: {
                // 식재료 추가/삭제 화면 실행
                page = add_Remove_Ingredients(window, renderer, user_key);
                break;
            }
            case 4: {
                // 레시피 검색 화면 실행
                page = search_recipe_page(window, renderer);
                break;
            }
            case 5: {
                // 키워드를 입력 받아 추천 레시피를 보여주는 화면 실행
                page = keyword_input_screen(window, renderer, user_key);
                break;
            }
            case 6: {
                // 로그아웃 처리 → 로그인 화면으로 이동
                page = 0;
                break;
            }
            case 7: {
                // 종료 선택 시 루프 종료
                running = 0;
                break;
            }
        }
    }

    // 리소스 해제 및 종료 처리
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
// 1.메인 화면 2-1 식재료 보기 선택 -> 식재료 보기 이동
//             2-2식재료 추가/제거 선택 -> 식재료 추가/제거로 이동
//             2-3레시피 확인 선택 -> 레시피 확인으로 이동
//             2-4요리추천 선택 -> 요리추천으로 이동
//             2-5 나가기 선택 -> 프로그램 종료
//SDL_StopTextInput(); → 텍스트 입력 모드 종료
//TTF_CloseFont(font); → 사용한 폰트 닫기
//SDL_DestroyRenderer(renderer); → 렌더러 제거
//SDL_DestroyWindow(window); → 창 제거
//TTF_Quit(); → TTF 라이브러리 종료
//SDL_Quit(); → SDL 전체 종료

/*식재료 데이터
char** ingredients_raw = get_Ingredients_split('2', user_key, &rawCount);

struct ingredient {
    char Name;
    int amount;
    int use_by_data;
}; 이건 식재료 데이터를 불러와 구조체로 저장할 준비

식재료 추가/제거 -> 식재료 목록 표시 ->사용자에게 입력받기 -> 입력받은 데이터 서버에 전송 -> 추가 or 제거 -> 식재료 추가, 식재료 제거
char name[MAX_NAME_LEN];
int amount, date;

char ingredient_input[20]
Input(ingredient_input);
Sscanf(ingredient_input, “%49[^/]/%d/%d”, name, &amount, &date)
//sscanf을 통해 양식에 맞는지 입력 값 확인, 이건 이름/수량/날짜" 형식으로 받고형식에 맞는지 검사하며 각 값을 분리하는 기능이야

send_data();
{
  char name;
  int amount;
  int date;
};//데이터를 구조체로 전송 이건 데이터를 name, amount, date로 묶어 구조체 형태로 한번에 보내는 코드

strcpy(list[count].Name, name);
list[count].amount = amount;
list[count].use_by_date = date;
count++;
//입력된 식재료 데이터를 저장 

send_data();
{
  char Delete_name;
}; //제거할 대상을 제거

/기능   상세기능 원천 데이터  데이터구조 제약조건
회원가입 아이디와비번 입력하면 회원가입 가능   사용자입력  저장,탐색,알고리즘
로그인  회원가입이 된 아이디와 비밀번호를 입력하면 로그인 가능함 , 사용자입력 / 탐색 알고리즘, 구조체
비번 암호화    pw는 해시로 암호화 되어 서버에 전송 , 사용자 입력 ,해시
식재료 이름 확인,	냉장고 안의 식재료의 이름을 확인 가능해야함	,	사용자 입력/		탐색 알고리즘, 구조체, 문자 배열					
식재료 유통기한 확인,	냉장고 안의 식재료의 유통기한을 확인 가능해야함,		사용자 입력/		구조체, 정수형 배열/ 같은 식재료가 여러 개 일시 제일 적게 남은 식재료의 유통기한만 표시			
식재료 재고량 확인,	냉장고 안의 식재료의 재고량을 확인 가능해야함		사용자 입력		구조체, 정수형 배열					
식재료 추가,  냉장고 안에 식재료를 추가 할 수 있어야 함		사용자 입력		저장, 정렬		센서가 없어 사용자가 직접 입력해 줘야 함			
식재료 제거,  냉장고 안의 식재료를 제거 할 수 있어야 함		사용자 입력		삭제		센서가 없어 사용자가 직접 입력해 줘야 함			
필요한 재료 확인,	요리에 필요한 재료를 확인 할 수 있어야 함,		클라이언트 프로그램	,	문자 배열		클라이언트 프로그램에 데이터가 저장되어 있어야 함			
요리 방법 확인,	요리 방법을 확인 할 수 있어야 함,		클라이언트 프로그램,		연결 리스트	,	클라이언트 프로그램에 데이터가 저장되어 있어야 함			
관련 검색어,	검색시 관련 검색어가 떠야함	,	클라이언트 프로그램,	문자 배열,	클라이언트 프로그램에 데이터가 저장되어 있어야 함			
키워드 요리 추천,	키워드를 입력하면 그 키워드에 맞는 요리를 추천 할 수 있어야 함,		서버에 저장된 데이터,		구조체 탐색 알고리즘,		키워드가 정확해야 함			
요리 추천,  조건모든 요리 추천은 냉장고 안에 있는 식재료로 가능한 요리만 추천해 줘야함 		,서버에 저장된 데이터		,구조체 탐색 알고리즘					
식재료 이름 데이터 정렬, 식재료 이름 데이터 표기 및 저장할 시 국어사전 순으로 정렬 되어야 함,		사용자 입력		,정렬					
식재료 유통기한 데이터 정렬	, 식재료 유통기한 데이터 표기 및 저장 시 유통기한이 적게 남은 순으로 정렬 되어야 함,	사용자 입력		,정렬					
서버에 데이터 전송, 	사용자가 서버에 아이디, 비밀번호 등 데이터를 전송 할 수 있어야 함/		사용자 입력	,	구조체		,아이디 비밀번호 등을 암호화 해서 전송 해야함			
사용자에게 데이터 전송,	서버가 사용자에게 냉장고 속 식재료, 레시피 등 데이터를 전송 할 수 있어야 함/	서버에 저장된 데이터,		구조체					
소켓 통신,	사용자와 서버의 통신은 소켓으로 이루어져야함,		http 프로토콜	,	구조체	


저장: 데이터를 메모리에 보관하는 작업으로, 자료형과 구조에 따라 다르게 저장됨.

탐색 알고리즘: 원하는 데이터를 찾는 방법으로 선형 탐색, 이진 탐색 등이 있음.

구조체: 서로 다른 자료형을 하나로 묶어 저장할 수 있는 사용자 정의 자료형.

해시: 데이터를 빠르게 찾기 위해 키를 해시 함수로 변환하여 저장하는 구조.

문자 배열: 여러 문자를 순서대로 저장하는 배열, 문자열 처리에 주로 사용됨.

정수형 배열: 정수 데이터를 순서대로 저장하는 배열 구조.

정렬: 데이터를 오름차순이나 내림차순으로 재배열하는 작업 (예: 버블, 퀵 정렬).

삭제: 배열이나 리스트 등에서 특정 데이터를 제거하는 작업.

연결 리스트: 노드들이 포인터로 연결된 자료 구조로, 삽입·삭제에 유리함.

구조체 탐색 알고리즘: 구조체 배열에서 특정 조건을 만족하는 항목을 찾는 방법.




