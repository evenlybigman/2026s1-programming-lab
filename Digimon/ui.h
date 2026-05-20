#ifndef UI_H
#define UI_H

#include <windows.h>
#include <stdbool.h>
#include "sprites.h" // SPRITE_W, SPRITE_H 는 sprites.h 에서 단일 정의

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
 * 메뉴 레이아웃 상수
 * ========================================================= */
#define MENU_TOP_COUNT      5    // 상단 메뉴 항목 수
#define MENU_BOT_COUNT      4    // 하단 메뉴 항목 수
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
 * @startX: 케이지 왼쪽 상단 픽셀 X
 * @startY: 케이지 왼쪽 상단 픽셀 Y
 */
void drawBackground(int startX, int startY);

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

#endif /* UI_H */
