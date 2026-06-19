#ifndef UI_H
#define UI_H

#include <windows.h>
#include <stdbool.h>
#include "sprites.h"  // SPRITE_W, SPRITE_H 는 sprites.h 에서 단일 정의
#include "digimon.h"  // Digimon 구조체 (draw_status 인자)

/* MinGW 6.x 헤더에 누락된 선언 */
#ifndef GetTickCount64
ULONGLONG WINAPI GetTickCount64(VOID);
#endif

/* =========================================================
 * 콘솔 레이아웃 상수
 * ========================================================= */

/* 케이지(디지몬 표시 영역) 크기 (픽셀 단위) */
#define CAGE_W          32   // 케이지 가로 길이
#define CAGE_H          16   // 케이지 세로 길이

/* 케이지 위치 (문자 좌표 기준) */
#define CAGE_START_X    5    // 케이지 왼쪽 상단 X (이전: CAGE_SX)
#define CAGE_START_Y    5    // 케이지 왼쪽 상단 Y (이전: CAGE_SY)
#define CAGE_MID_X      13   // 케이지 중앙 X (스프라이트 배치 기준점)

/* 배경색 (Windows 콘솔 색상 번호) */
#define BG_COLOR        7    // 케이지 배경: 밝은 회색

/* =========================================================
 * 상태창 레이아웃 상수
 *   케이지 픽셀 오른쪽 끝 = (CAGE_START_X + CAGE_W) * 2 = 74 (문자 열)
 *   상태창은 74 + 2 = 76 열부터, 케이지 상단과 같은 높이에서 시작
 * ========================================================= */
#define STATUS_X        ((CAGE_START_X + CAGE_W) * 2 + 2)  // 케이지 오른쪽 2칸 여백
#define STATUS_Y        (CAGE_START_Y)                      // 케이지 상단과 동일 높이
#define STATUS_LINES    (CAGE_H)                            // 케이지 높이만큼 사용

/* =========================================================
 * 메뉴 레이아웃 상수
 * ========================================================= */
#define MENU_TOP_COUNT      5    // 상단 메뉴 항목 수
#define MENU_BOT_COUNT      3    // 하단 메뉴 항목 수
#define MENU_TOTAL_COUNT    (MENU_TOP_COUNT + MENU_BOT_COUNT)
#define MENU_ITEM_PADDING   7    // 항목 사이 여백 (문자 단위)
#define MENU_LINE_CLEAR_LEN 100  // 메뉴 줄 지우기용 공백 수

/* =========================================================
 * UI 전역 상태
 * ========================================================= */
extern HANDLE hOut;         // 표준 출력 콘솔 핸들 (ui.c 에서 초기화)
extern int    menu_selected; // 현재 선택된 메뉴 인덱스 (0 ~ MENU_TOTAL_COUNT-1)

/* =========================================================
 * 함수 선언
 * ========================================================= */

/**
 * ui_init - UI 시스템을 초기화한다.
 *
 * hOut 을 GetStdHandle(STD_OUTPUT_HANDLE)로 설정하고
 * 커서를 숨긴다. 프로그램 시작 시 반드시 한 번 호출해야 한다.
 * 핸들 취득 실패 시 에러 메시지를 출력하고 exit(1) 한다.
 */
void ui_init(void);

/**
 * gotoxy - 콘솔 커서를 문자 좌표 (x, y) 로 이동한다.
 * @x: 열 (0-based)
 * @y: 행 (0-based)
 */
void gotoxy(int x, int y);

/**
 * gotoxyPixel - 콘솔 커서를 픽셀 좌표 (x, y) 로 이동한다.
 * @x: 픽셀 열 (실제 콘솔 열 = x * 2, 2칸 = 1픽셀)
 * @y: 픽셀 행 (실제 콘솔 행 = y)
 */
void gotoxyPixel(int x, int y);

/**
 * drawPixel - 픽셀 좌표 (x, y) 에 배경색 블록을 그린다.
 * @x:     픽셀 열
 * @y:     픽셀 행
 * @color: Windows 콘솔 색상 번호 (0~15)
 */
void drawPixel(int x, int y, int color);

/**
 * drawBackground - 케이지 전체를 BG_COLOR 로 채운다.
 */
void drawBackground(void);

/**
 * drawSprite - 스프라이트를 지정 위치에 그린다.
 * @sprite: SPRITE_H × SPRITE_W 정수 배열 포인터
 * @startX: 스프라이트 왼쪽 상단 픽셀 X
 * @startY: 스프라이트 왼쪽 상단 픽셀 Y
 *
 * PIXEL_BG(1) 픽셀은 건너뛰고(투명), 나머지 픽셀은
 * colorMap[] 에서 색상을 조회해 그린다.
 * colorMap 범위를 초과하는 픽셀 값은 무시한다(OOB 방어).
 * @flip: true 면 좌우 반전해서 그린다.
 */
void drawSprite(int (*sprite)[SPRITE_W], int startX, int startY, bool flip);

/**
 * clearMenuLines - 메뉴가 표시되는 두 줄을 공백으로 지운다.
 * @startTopY: 상단 메뉴 행
 * @startBotY: 하단 메뉴 행
 */
void clearMenuLines(int startTopY, int startBotY);

/**
 * drawMenu - 상/하단 메뉴를 그린다.
 * @startX:    메뉴 시작 문자 열
 * @startTopY: 상단 메뉴 행
 * @startBotY: 하단 메뉴 행
 *
 * menu_selected 와 일치하는 항목에 [ ] 강조를 표시한다.
 */
void drawMenu();
// 스프라이트 지우기
void clearSprite(int startX, int startY);

/**
 * draw_status - 상태창을 케이지 오른쪽에 항상 출력한다.
 * @d: 현재 디지몬 상태 포인터
 */
void draw_status(const Digimon *d);

/**
 * draw_call_alert - 호출 알림을 상태창 마지막 줄에 표시/지운다.
 * @is_call: true이면 "호출!!" 출력, false이면 지움
 */
void draw_call_alert(bool is_call);

/**
 * drawUiSprite - 8×8 UI 아이콘 스프라이트를 그린다.
 */
void drawUiSprite(int (*sprite)[UI_SPRITE_W], int startX, int startY);

/**
 * draw_cage_ui - 케이지 안에 UI 아이콘을 그린다.
 *   똥(poop), 수면(zzz), 부상(medicine) 아이콘.
 *   anim_update() 호출 직후 매 루프마다 호출한다.
 */
void draw_cage_ui(const Digimon *d);

/**
 * drawBgSprite - CAGE_H × CAGE_W 크기 배경 스프라이트를 그린다.
 */
void drawBgSprite(int (*sprite)[CAGE_W], int startX, int startY);

/**
 * drawFont - 8×8 픽셀 폰트 문자 하나를 그린다. ('A'~'Z')
 */
void drawFont(char c, int startX, int startY);

/**
 * clearFont - 8×8 픽셀 영역을 BG_COLOR 로 지운다.
 */
void clearFont(int startX, int startY);

/**
 * drawDigit - 4×7 픽셀 숫자 하나를 그린다. (0~9)
 */
void drawDigit(int digit, int startX, int startY);

/**
 * show_status_screen - 상태 정보를 케이지 안에 픽셀로 표시한다.
 *   9페이지: 0=이름/스프라이트(자동스크롤), 1=배고픔, 2=근력,
 *            3=나이, 4=체중, 5=DP, 6=똥, 7=케어미스, 8=노력치
 *   Z/X: 페이지 이동, C: 닫기
 */
void show_status_screen(const Digimon *d);

/**
 * clear_status - 상태창 영역을 공백으로 지운다.
 * EGG 상태 진입 시(신규 게임, 사망) 호출한다.
 */
void clear_status(void);

/**
 * draw_battle_info - 배틀 화면 정보 행(DP 바·이름)을 출력한다.
 * @p_dp:     플레이어 현재 DP
 * @c_dp:     상대 현재 DP
 * @max_dp:   양측 공통 최대 DP
 * @opp_name: 상대 이름 (한글)
 * @row:      출력 콘솔 행 (gotoxy Y)
 */
void draw_battle_info(int p_dp, int c_dp, int max_dp, const char *opp_name, int row);

/**
 * draw_name_ticker - 상태창 상단에 영문 이름을 픽셀 스크롤로 표시한다.
 * @eng_name: 대문자 영문 이름 (A~Z)
 * @scroll_x:  픽셀 단위 스크롤 오프셋 (0 = 첫 글자부터)
 *
 * font_name (5×8) 을 사용해 3글자 분량(17px)의 창을 슬라이드한다.
 * 스크롤 값이 바뀔 때만 호출한다 (매 프레임 호출 불필요).
 */
void draw_name_ticker(const char *eng_name, int scroll_x);

/**
 * flush_cage - 케이지 버퍼를 콘솔에 한 번에 출력한다.
 *   drawPixel / drawSprite / drawBackground 등은 내부 CHAR_INFO 버퍼에만 쓰고,
 *   이 함수를 호출해야 실제 화면에 반영된다.
 *   메인 루프에서 매 프레임 1회 호출한다.
 */
void flush_cage(void);

#endif /* UI_H */
