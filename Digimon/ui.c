#include "ui.h"
#include "sprites.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* =========================================================
 * 전역 상태
 * ========================================================= */

HANDLE hOut;          // 표준 출력 콘솔 핸들
int    menu_selected; // 현재 선택 인덱스 (0 ~ MENU_TOTAL_COUNT-1)

/* =========================================================
 * 메뉴 데이터
 *   항목을 추가/변경할 때 아래 두 배열과
 *   ui.h 의 MENU_TOP_COUNT / MENU_BOT_COUNT 만 수정한다.
 * ========================================================= */
static const char *menu_top[MENU_TOP_COUNT] = {
    "먹이", "훈련", "배틀", "청소"
};
static const char *menu_bot[MENU_BOT_COUNT] = {
    " 잠 ", "치료", "앨범", "상태"
};

/* =========================================================
 * ui_init
 * ========================================================= */
void ui_init(void) {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "[오류] 콘솔 핸들을 가져올 수 없습니다.\n");
        exit(1);
    }

    /* 커서 숨기기 */
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &ci);

    menu_selected = 0;
}

/* =========================================================
 * 커서 이동
 * ========================================================= */

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
}

void gotoxyPixel(int x, int y) {
    /* 픽셀 1칸 = 콘솔 문자 2칸 (정사각형 픽셀) */
    COORD pos = { (SHORT)(x * 2), (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
}

/* =========================================================
 * 픽셀 / 스프라이트 / 배경
 * ========================================================= */

void drawPixel(int x, int y, int color) {
    gotoxyPixel(x, y);
    SetConsoleTextAttribute(hOut, (WORD)((color << 4) | color));
    printf("  ");
}

void drawBackground(void) {
    for (int y = 0; y < CAGE_H; y++) {
        for (int x = 0; x < CAGE_W; x++) {
            drawPixel(CAGE_START_X + x, CAGE_START_Y + y, BG_COLOR);
        }
    }
}

void drawSprite(int (*sprite)[SPRITE_W], int startX, int startY, bool flip) {
    for (int y = 0; y < SPRITE_H; y++) {
        for (int x = 0; x < SPRITE_W; x++) {
            int sx    = flip ? (SPRITE_W - 1 - x) : x;
            int pixel = sprite[y][sx];

            /* PIXEL_BG(1)는 투명 처리 — 건너뜀 */
            if (pixel == PIXEL_BG) continue;

            /* colorMap 범위 초과 방어 (OOB 방지) */
            if (pixel < 0 || pixel >= COLORMAP_SIZE) continue;

            drawPixel(startX + x, startY + y, colorMap[pixel]);
        }
    }
}

/* =========================================================
 * 메뉴
 * ========================================================= */

void clearMenuLines(int startTopY, int startBotY) {
    SetConsoleTextAttribute(hOut, BG_COLOR);

    gotoxy(0, startTopY);
    for (int i = 0; i < MENU_LINE_CLEAR_LEN; i++) printf(" ");

    gotoxy(0, startBotY);
    for (int i = 0; i < MENU_LINE_CLEAR_LEN; i++) printf(" ");
}

void drawMenu() {
    //CAGE_START_X, CAGE_START_Y + CAGE_H + 1, CAGE_START_Y + CAGE_H + 2
    /* 이전 내용 지우기 */
    clearMenuLines(CAGE_START_Y + CAGE_H + 1, CAGE_START_Y + CAGE_H + 2);

    SetConsoleTextAttribute(hOut, BG_COLOR);

    int x = CAGE_START_X + MENU_ITEM_PADDING;

    for (int i = 0; i < MENU_TOP_COUNT; i++) {
        /* 상단 행 */
        gotoxy(x, CAGE_START_Y + CAGE_H + 1);
        if (i == menu_selected)
            printf("[ %s ]", menu_top[i]);
        else
            printf("  %s  ", menu_top[i]);

        /* 하단 행 (항목이 있을 때만) */
        int bot_len = 0;
        if (i < MENU_BOT_COUNT) {
            gotoxy(x, CAGE_START_Y + CAGE_H + 2);
            if (i + MENU_TOP_COUNT == menu_selected)
                printf("[ %s ]", menu_bot[i]);
            else
                printf("  %s  ", menu_bot[i]);
            bot_len = (int)strlen(menu_bot[i]);
        }

        /* 다음 항목 X 위치: 두 행 중 긴 쪽 길이 + 여백 */
        int top_len = (int)strlen(menu_top[i]);
        x += (top_len > bot_len ? top_len : bot_len) + MENU_ITEM_PADDING;
    }
}

void drawBgSprite(int (*sprite)[CAGE_W], int startX, int startY) {
    for (int y = 0; y < CAGE_H; y++) {
        for (int x = 0; x < CAGE_W; x++) {
            int pixel = sprite[y][x];
            if (pixel == PIXEL_BG) continue;
            if (pixel < 0 || pixel >= COLORMAP_SIZE) continue;
            drawPixel(startX + x, startY + y, colorMap[pixel]);
        }
    }
}

void drawFont(char c, int startX, int startY) {
    int idx = c - 'A';
    if (idx < 0 || idx > 25) return;
    for (int y = 0; y < FONT_H; y++) {
        for (int x = 0; x < FONT_W; x++) {
            int pixel = font_alpha[idx][y][x];
            if (pixel == PIXEL_BG) continue;
            if (pixel < 0 || pixel >= COLORMAP_SIZE) continue;
            drawPixel(startX + x, startY + y, colorMap[pixel]);
        }
    }
}

void clearFont(int startX, int startY) {
    for (int y = 0; y < FONT_H; y++)
        for (int x = 0; x < FONT_W; x++)
            drawPixel(startX + x, startY + y, BG_COLOR);
}

// 디지몬 스프라이트 지우기
void clearSprite(int startX, int startY) {
    for (int y = 0; y < SPRITE_H; y++) {
        for (int x = 0; x < SPRITE_W; x++) {
            drawPixel(startX + x, startY + y, BG_COLOR);
        }
    }
}

/* =========================================================
 * 상태창
 * ========================================================= */

/* ■□ 막대 출력 헬퍼 (val/max 칸) */
static void print_bar(int val, int max) {
    for (int i = 0; i < max; i++)
        printf(i < val ? "\x02" : ".");  // 0x02 = Windows 콘솔 블록(■)
}

/* 한 줄을 STATUS_X 위치부터 공백 40칸으로 지움 */
static void clear_line(int y) {
    gotoxy(STATUS_X, y);
    printf("%-40s", "");
}

void draw_status(const Digimon *d) {
    SetConsoleTextAttribute(hOut, 15);  // 밝은 흰색

    int y = STATUS_Y;

    /* ─ 이름 / 레벨 ─ */
    gotoxy(STATUS_X, y++);
    printf("%-30s", d->name);
    gotoxy(STATUS_X, y++);
    printf("%-15s", level_names[d->level]);

    clear_line(y++);  // 빈 줄

    /* ─ 배고픔 ─ */
    gotoxy(STATUS_X, y++);
    printf("배고픔 ");
    print_bar(d->hungry, MAX_HUNGRY);

    /* ─ 근력 ─ */
    gotoxy(STATUS_X, y++);
    printf("근 력  ");
    print_bar(d->strength, MAX_STRENGTH);

    clear_line(y++);  // 빈 줄

    /* ─ 나이 / 체중 ─ */
    gotoxy(STATUS_X, y++);
    printf("나이  %2d일", d->age);
    gotoxy(STATUS_X, y++);
    printf("체중  %2dg ", d->weight);

    clear_line(y++);  // 빈 줄

    /* ─ DP ─ */
    gotoxy(STATUS_X, y++);
    printf("DP  %2d/%2d", d->dp, d->max_dp);

    /* ─ 똥 / 케어미스 ─ */
    gotoxy(STATUS_X, y++);
    printf("똥   %d개  ", d->poop);
    gotoxy(STATUS_X, y++);
    printf("케어 %2d회 ", d->care_mistakes);

    clear_line(y++);  // 빈 줄
    clear_line(y++);  // 잔상 지우기 (부상 행)
    clear_line(y++);  // 잔상 지우기 (수면 행)

    SetConsoleTextAttribute(hOut, BG_COLOR);
}

void draw_call_alert(bool is_call) {
    gotoxy(STATUS_X, STATUS_Y + STATUS_LINES - 1);
    if (is_call) {
        SetConsoleTextAttribute(hOut, 0x4E);  // 빨간 배경 + 밝은 노란 글씨
        printf("호출!!");
    } else {
        SetConsoleTextAttribute(hOut, 0);
        printf("      ");  // 지우기 (호출!! = 6칸)
    }
    SetConsoleTextAttribute(hOut, BG_COLOR);
}

