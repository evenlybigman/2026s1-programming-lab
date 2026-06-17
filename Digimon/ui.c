#include "ui.h"
#include "sprites.h"
#include "anim.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

/* =========================================================
 * 전역 상태
 * ========================================================= */

HANDLE hOut;          // 표준 출력 콘솔 핸들
int    menu_selected; // 현재 선택 인덱스 (0 ~ MENU_TOTAL_COUNT-1)

/* 케이지 더블 버퍼 — drawPixel은 여기에 쓰고, flush_cage()가 한 번에 출력 */
static CHAR_INFO cage_buf[CAGE_H][CAGE_W * 2];

/* =========================================================
 * 메뉴 데이터
 *   항목을 추가/변경할 때 아래 두 배열과
 *   ui.h 의 MENU_TOP_COUNT / MENU_BOT_COUNT 만 수정한다.
 * ========================================================= */
static const char *menu_top[MENU_TOP_COUNT] = {
    "상태", "먹이", "훈련", "배틀", "청소"
};
static const char *menu_bot[MENU_BOT_COUNT] = {
    " 잠 ", "치료", "저장"
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
    int bx = x - CAGE_START_X;
    int by = y - CAGE_START_Y;
    if (bx < 0 || bx >= CAGE_W || by < 0 || by >= CAGE_H) return;
    WORD attr = (WORD)((color << 4) | color);
    cage_buf[by][bx * 2    ].Char.AsciiChar = ' ';
    cage_buf[by][bx * 2    ].Attributes     = attr;
    cage_buf[by][bx * 2 + 1].Char.AsciiChar = ' ';
    cage_buf[by][bx * 2 + 1].Attributes     = attr;
}

void flush_cage(void) {
    COORD      bufSize  = { (SHORT)(CAGE_W * 2), (SHORT)CAGE_H };
    COORD      bufCoord = { 0, 0 };
    SMALL_RECT region   = {
        (SHORT)(CAGE_START_X * 2),
        (SHORT)CAGE_START_Y,
        (SHORT)(CAGE_START_X * 2 + CAGE_W * 2 - 1),
        (SHORT)(CAGE_START_Y + CAGE_H - 1)
    };
    WriteConsoleOutputA(hOut, (CHAR_INFO *)cage_buf, bufSize, bufCoord, &region);
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

void drawUiSprite(int (*sprite)[UI_SPRITE_W], int startX, int startY) {
    for (int y = 0; y < UI_SPRITE_H; y++) {
        for (int x = 0; x < UI_SPRITE_W; x++) {
            int pixel = sprite[y][x];
            if (pixel == PIXEL_BG) continue;
            if (pixel < 0 || pixel >= COLORMAP_SIZE) continue;
            drawPixel(startX + x, startY + y, colorMap[pixel]);
        }
    }
}

void draw_cage_ui(const Digimon *d) {
    if (d->level == EGG) return;

    static int  prev_poop      = 0;
    static bool prev_sleep     = false;
    static bool prev_injuries  = false;

    /* 똥 아이콘: 개수가 줄었으면 사라진 슬롯만 지움, 늘었으면 새 슬롯만 그림 */
    if (d->poop < prev_poop) {
        for (int i = d->poop; i < prev_poop && i < MAX_POOP; i++) {
            int px = CAGE_START_X + (i % 4) * UI_SPRITE_W;
            int py = CAGE_START_Y + CAGE_H - UI_SPRITE_H - (i / 4) * UI_SPRITE_H;
            for (int ey = 0; ey < UI_SPRITE_H; ey++)
                for (int ex = 0; ex < UI_SPRITE_W; ex++)
                    drawPixel(px + ex, py + ey, BG_COLOR);
        }
    } else if (d->poop > prev_poop) {
        for (int i = prev_poop; i < d->poop && i < MAX_POOP; i++) {
            int px = CAGE_START_X + (i % 4) * UI_SPRITE_W;
            int py = CAGE_START_Y + CAGE_H - UI_SPRITE_H - (i / 4) * UI_SPRITE_H;
            drawUiSprite((i % 2 == 0) ? sprite_poop1 : sprite_poop2, px, py);
        }
    }
    prev_poop = d->poop;

    /* Zzz: 상태 변화 시에만 그리거나 지움 */
    if (d->is_sleep && !prev_sleep) {
        drawUiSprite(sprite_zzz, CAGE_START_X + CAGE_W - UI_SPRITE_W, CAGE_START_Y);
    } else if (!d->is_sleep && prev_sleep) {
        int zx = CAGE_START_X + CAGE_W - UI_SPRITE_W;
        for (int ey = 0; ey < UI_SPRITE_H; ey++)
            for (int ex = 0; ex < UI_SPRITE_W; ex++)
                drawPixel(zx + ex, CAGE_START_Y + ey, BG_COLOR);
    }
    prev_sleep = d->is_sleep;

    /* 약/주사: 상태 변화 시에만 그리거나 지움 */
    if (d->is_injuries && !prev_injuries) {
        drawUiSprite(sprite_medicine, CAGE_START_X, CAGE_START_Y);
    } else if (!d->is_injuries && prev_injuries) {
        for (int ey = 0; ey < UI_SPRITE_H; ey++)
            for (int ex = 0; ex < UI_SPRITE_W; ex++)
                drawPixel(CAGE_START_X + ex, CAGE_START_Y + ey, BG_COLOR);
    }
    prev_injuries = d->is_injuries;
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

/* =========================================================
 * drawDigit
 * ========================================================= */
void drawDigit(int digit, int startX, int startY) {
    if (digit < 0 || digit > 9) return;
    for (int y = 0; y < DIGIT_H; y++) {
        for (int x = 0; x < DIGIT_W; x++) {
            int pixel = font_digit[digit][y][x];
            if (pixel == PIXEL_BG) continue;
            drawPixel(startX + x, startY + y, colorMap[PIXEL_INK]);
        }
    }
}

/* 숫자를 ndigits 자리로 그린다 (우측 정렬, 1픽셀 자간). */
static void draw_number(int num, int x, int y, int ndigits) {
    if (num < 0) num = 0;
    for (int i = ndigits - 1; i >= 0; i--) {
        drawDigit(num % 10, x + i * (DIGIT_W + 1), y);
        num /= 10;
    }
}

/* "/" 구분자: 2×5픽셀 사선 */
static void draw_slash(int x, int y) {
    drawPixel(x + 1, y + 0, colorMap[PIXEL_INK]);
    drawPixel(x + 1, y + 1, colorMap[PIXEL_INK]);
    drawPixel(x + 0, y + 2, colorMap[PIXEL_INK]);
    drawPixel(x + 0, y + 3, colorMap[PIXEL_INK]);
    drawPixel(x + 0, y + 4, colorMap[PIXEL_INK]);
}

/* =========================================================
 * show_status_screen  —  스탯 1개씩 페이지로 표시
 *   케이지 상단 5px : 3×5 미니 폰트 라벨 (풀네임)
 *   y=CAGE_START_Y+6 : 구분선 (1px)
 *   케이지 하단 9px : 값 (하트 바 또는 숫자)
 *   Z/X: 이전/다음 페이지, C: 닫기
 *
 *   3×5 + 1px 자간 = 4px/글자 → 8글자가 32px 케이지에 꼭 맞음
 * ========================================================= */

/* 라벨이 끝난 직후 y (구분선 위치) */
#define STATUS_SEP_Y     (CAGE_START_Y + MINI_H + 1)          /* = CAGE_START_Y + 6 */
/* 값 영역 시작 y (구분선 다음 행) */
#define STATUS_VAL_TOP   (STATUS_SEP_Y + 1)                   /* = CAGE_START_Y + 7 */
/* 값 영역 높이 */
#define STATUS_VAL_H     (CAGE_H - MINI_H - 2)                /* = 9 */
/* 값 y: 하트(8px) 중앙 */
#define STATUS_VAL_Y_HEARTS (STATUS_VAL_TOP + (STATUS_VAL_H - UI_SPRITE_H) / 2)
/* 값 y: 숫자(7px) 중앙 */
#define STATUS_VAL_Y_DIGITS (STATUS_VAL_TOP + (STATUS_VAL_H - DIGIT_H)     / 2)

/* 케이지 상단에 3×5 미니 폰트로 문자열을 중앙 정렬해 그린다. */
static void draw_mini_label(const char *str) {
    int len = 0;
    while (str[len]) len++;
    int total_w = len * MINI_W + (len > 1 ? len - 1 : 0);
    int x0 = CAGE_START_X + (CAGE_W - total_w) / 2;
    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (c >= 'a' && c <= 'z') c = (char)(c - 32);
        if (c < 'A' || c > 'Z') continue;
        int idx = c - 'A';
        int x   = x0 + i * (MINI_W + 1);
        for (int dy = 0; dy < MINI_H; dy++)
            for (int dx = 0; dx < MINI_W; dx++)
                if (font_mini[idx][dy][dx] == PIXEL_INK)
                    drawPixel(x + dx, CAGE_START_Y + dy, colorMap[PIXEL_INK]);
    }
    /* 구분선 */
    for (int sx = 0; sx < CAGE_W; sx++)
        drawPixel(CAGE_START_X + sx, STATUS_SEP_Y, colorMap[PIXEL_INK]);
}

/* 값 영역 중앙에 ndigits 자리 숫자를 그린다. */
static void draw_status_number(int val, int ndigits) {
    int w = ndigits * DIGIT_W + (ndigits - 1);
    int x = CAGE_START_X + (CAGE_W - w) / 2;
    draw_number(val, x, STATUS_VAL_Y_DIGITS, ndigits);
}

/* 값 영역 중앙에 "XX/XX" 형식 DP를 그린다. */
static void draw_status_dp(int dp, int max_dp) {
    /* 2digits(9) + slash(2) + gap(1) + 2digits(9) = 21px */
    int x = CAGE_START_X + (CAGE_W - 21) / 2;
    int y = STATUS_VAL_Y_DIGITS;
    draw_number(dp,     x,      y, 2);
    draw_slash(         x + 10, y + 1);
    draw_number(max_dp, x + 13, y, 2);
}

/* 값 영역 중앙에 하트 바를 그린다. */
static void draw_status_hearts(int val, int max_val) {
    int total_w = max_val * UI_SPRITE_W;
    int x = CAGE_START_X + (CAGE_W - total_w) / 2;
    for (int i = 0; i < max_val; i++)
        drawUiSprite(i < val ? font_heart : font_heart_empty,
                     x + i * UI_SPRITE_W, STATUS_VAL_Y_HEARTS);
}

/* =========================================================
 * show_status_screen  —  스탯 페이지 표시
 *   페이지 0 : 디지몬 스프라이트(좌) + 이름 5×8 픽셀 자동스크롤(우하단)
 *   페이지 1~8: 3×5 미니라벨 + 스탯값
 *   Z/X: 페이지 이동, C: 닫기
 * ========================================================= */

static void draw_name_char(char c, int startX, int startY) {
    if (c >= 'a' && c <= 'z') c = (char)(c - 32);
    if (c < 'A' || c > 'Z') return;
    int idx = c - 'A';
    for (int y = 0; y < NAME_H; y++)
        for (int x = 0; x < NAME_W; x++)
            if (font_name[idx][y][x] == PIXEL_INK)
                drawPixel(startX + x, startY + y, colorMap[PIXEL_INK]);
}

static void draw_name_scroll(const char *name, int name_len, int offset) {
    int rx = CAGE_START_X + CAGE_W / 2;
    int ry = CAGE_START_Y + CAGE_H / 2;
    for (int y = ry; y < CAGE_START_Y + CAGE_H; y++)
        for (int x = rx; x < CAGE_START_X + CAGE_W; x++)
            drawPixel(x, y, BG_COLOR);
    for (int i = 0; i < 3; i++) {
        int ci = offset + i;
        if (ci >= name_len) break;
        draw_name_char(name[ci], rx + i * (NAME_W + 1), ry);
    }
}

void show_status_screen(const Digimon *d) {
    if (d->level == EGG) return;

    static const char *labels[] = {
        "HUNGRY", "STRENGTH", "AGE", "WEIGHT", "DP", "POOP", "CAREMISS", "EFFORT"
    };
    static const int PAGES = 9;  /* 0=이름/스프라이트, 1~8=스탯 */
    int page = 0;

    int (*walk_sprite)[SPRITE_W] = anim_get_walk_sprite(d->level, d->table_idx);
    int name_len = (int)strlen(d->name);
    int name_max_offset = name_len > 3 ? name_len - 3 : 0;
    int name_offset = 0;
    ULONGLONG last_scroll_ms = GetTickCount64();
    bool need_redraw = true;

    while (1) {
        ULONGLONG now_ms = GetTickCount64();

        /* 페이지 0: 이름 스크롤 자동 진행 */
        if (page == 0 && name_offset < name_max_offset
                && now_ms - last_scroll_ms >= 400) {
            name_offset++;
            last_scroll_ms = now_ms;
            need_redraw = true;
        }

        if (need_redraw) {
            drawBackground();

            if (page == 0) {
                drawSprite(walk_sprite, CAGE_START_X, CAGE_START_Y, false);
                draw_name_scroll(d->name, name_len, name_offset);
            } else {
                draw_mini_label(labels[page - 1]);
                switch (page) {
                case 1: draw_status_hearts(d->hungry,        MAX_HUNGRY);   break;
                case 2: draw_status_hearts(d->strength,      MAX_STRENGTH); break;
                case 3: draw_status_number(d->age,           2);            break;
                case 4: draw_status_number(d->weight,        2);            break;
                case 5: draw_status_dp(d->dp, d->max_dp);                   break;
                case 6: draw_status_number(d->poop,          1);            break;
                case 7: draw_status_number(d->care_mistakes, 2);            break;
                case 8: draw_status_number(d->effort,        2);            break;
                }
            }

            flush_cage();

            SetConsoleTextAttribute(hOut, 15);
            gotoxy(CAGE_START_X * 2, CAGE_START_Y + CAGE_H + 1);
            printf("[%d/%d] Z:이전  X:다음  C:닫기          ", page + 1, PAGES);
            SetConsoleTextAttribute(hOut, BG_COLOR);

            need_redraw = false;
        }

        if (_kbhit()) {
            int key = _getch();
            if (key == 'c' || key == 'C') break;
            else if (key == 'z' || key == 'Z') {
                page = (page - 1 + PAGES) % PAGES;
                if (page == 0) { name_offset = 0; last_scroll_ms = GetTickCount64(); }
                need_redraw = true;
            }
            else if (key == 'x' || key == 'X') {
                page = (page + 1) % PAGES;
                if (page == 0) { name_offset = 0; last_scroll_ms = GetTickCount64(); }
                need_redraw = true;
            }
        }

        Sleep(10);
    }

    drawBackground();
    flush_cage();
    drawMenu();
    draw_status(d);
}

