#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h> 
#include <time.h>

#define CAGE_W 32 //케이지 가로 길이
#define CAGE_H 16 //케이지 세로 길이
#define SPRITE_W 16 //디지몬 스프라이트 X크기
#define SPRITE_H 16 //디지몬 스프라이트 Y크기
#define CAGE_SX 5
#define CAGE_SY 5
#define CAGE_MX 13
#define ANIM_TICK 0.5

HANDLE hOut;

// 텍스트용 (그대로)
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(hOut, pos);
}

// 픽셀용 (x*2)
void gotoxyPixel(int x, int y) {
    COORD pos = { x * 2, y };
    SetConsoleCursorPosition(hOut, pos);
}

void drawPixel(int x, int y, int color) {
    gotoxyPixel(x, y);
    SetConsoleTextAttribute(hOut, (color << 4) | color);
    printf("  ");
}

void setConsoleFont(int width, int height) {
    CONSOLE_FONT_INFOEX cfi = { sizeof(cfi) };
    cfi.dwFontSize.X = width;
    cfi.dwFontSize.Y = height;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
}

int defalt[SPRITE_H][SPRITE_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

int egg_1[SPRITE_H][SPRITE_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1},
    {1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1},
    {1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1},
    {1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1},
    {1,1,1,0,1,1,1,1,1,0,0,0,0,1,1,1},
    {1,1,0,1,1,0,1,1,1,1,0,0,0,0,1,1},
    {1,1,0,1,1,0,0,1,1,1,1,0,0,0,1,1},
    {1,1,0,1,1,0,0,0,1,1,1,1,1,0,1,1},
    {1,1,0,1,1,1,0,0,1,1,0,0,1,0,1,1},
    {1,1,1,0,0,1,1,1,1,0,0,0,0,1,1,1},
    {1,1,1,1,0,0,1,1,1,0,0,0,1,1,1,1},
    {1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1},
};

int egg_2[SPRITE_H][SPRITE_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1},
    {1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1},
    {1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1},
    {1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,0,0,0,1,1},
    {1,0,1,1,0,0,1,1,1,1,1,1,1,1,0,1},
    {1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,1},
    {1,0,0,1,1,0,0,0,1,1,1,0,0,1,0,1},
    {1,1,0,0,1,1,1,1,1,1,0,0,0,0,1,1},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1},
};

int egg_3[SPRITE_H][SPRITE_W] = {
    {1,1,1,1,1,1,0,0,0,0,1,1,1,0,1,1},
    {1,1,0,0,1,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,1,1,0,1,1,0,0,0,0,1,0,0,1,0},
    {0,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0},
    {0,1,1,0,1,1,0,1,1,0,1,1,1,0,1,0},
    {0,1,0,1,1,1,0,0,0,0,1,1,1,1,0,1},
    {1,0,1,1,1,0,1,0,0,1,0,1,1,1,1,1},
    {1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1},
    {1,1,1,1,1,0,0,1,1,0,0,1,1,1,1,1},
    {1,1,1,0,1,0,0,1,1,0,0,1,0,1,1,1},
    {1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1},
    {1,1,0,1,1,0,0,0,1,1,1,1,1,0,1,1},
    {1,1,0,1,1,1,0,0,1,1,0,0,1,0,1,1},
    {1,1,1,0,0,1,1,1,1,0,0,0,0,1,1,1},
    {1,1,1,1,0,0,1,1,1,0,0,0,1,1,1,1},
    {1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1},
};

int agumonsprite[SPRITE_H][SPRITE_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1},
    {1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1},
    {1,1,1,1,0,0,1,0,0,1,1,1,0,1,1,1},
    {1,1,0,0,1,1,1,1,0,0,1,1,0,1,1,1},
    {1,0,1,1,1,1,1,0,0,0,1,1,1,0,1,1},
    {1,0,0,0,0,1,1,1,1,1,1,1,1,0,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1},
    {1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,1},
    {1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1},
    {1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,1,1,0,0,0,1,1,1,0,0,0,1,0,1,1},
    {1,1,1,1,1,0,0,1,1,1,1,0,1,1,0,1},
    {1,1,1,0,0,1,1,0,0,0,0,1,1,1,0,1},
    {1,1,0,1,0,1,1,0,1,0,1,0,1,0,1,0},
    {1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
};

int colorMap[] = { 0, 15 };

void drawBackground(int startX, int startY) {
    for (int y = 0; y < CAGE_H; y++) {
        for (int x = 0; x < CAGE_W; x++) {
            drawPixel(startX + x, startY + y, 7);
        }
    }
}

void drawSprite(int (*sprite)[SPRITE_W], int startX, int startY) {
    for (int y = 0; y < SPRITE_H; y++) {
        for (int x = 0; x < SPRITE_W; x++) {
            if (sprite[y][x] == 1) continue;
            drawPixel(startX + x, startY + y, colorMap[sprite[y][x]]);
        }
    }
}

char* menu_top[] = { "건강", "먹이", "훈련", "배틀", "청소" };
char* menu_bot[] = { "잠 ", "치료", "앨범", "상태", "호출" };
int menu_count = 10;
int selected = 0;  // 현재 선택된 메뉴

void drawMenu(int startX, int startTopY, int startBotY) {
    SetConsoleTextAttribute(hOut, 7);

    // 기존 화면 지우기
    gotoxy(0, startTopY); 
    for (int i = 0; i < 100; i++) printf(" ");
    gotoxy(0, startBotY); 
    for (int i = 0; i < 100; i++) printf(" ");

    int x = startX + 7;
    for (int i = 0; i < 5; i++) {
        // 상단
        gotoxy(x, startTopY);
        if (i == selected)
            printf("[ %s ]", menu_top[i]);
        else
            printf("  %s  ", menu_top[i]);

        // 하단
        gotoxy(x, startBotY);
        if (i + 5 == selected)
            printf("[ %s ]", menu_bot[i]);
        else
            printf("  %s  ", menu_bot[i]);

        // 더 긴 메뉴 기준으로 x 이동
        int top_len = strlen(menu_top[i]);
        int bot_len = strlen(menu_bot[i]);
        x += (top_len > bot_len ? top_len : bot_len) + 7;
    }
}

HANDLE hBuffer;  // hOut 대신 hBuffer 추가

int main() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);  // 기본 핸들 사용

    setConsoleFont(24, 24);

    CONSOLE_CURSOR_INFO cci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &cci);

    // hBuffer, SetConsoleActiveScreenBuffer 제거

    drawBackground(CAGE_SX, CAGE_SY);
    drawSprite(egg_1, CAGE_MX, CAGE_SY);
    drawMenu(CAGE_SX, CAGE_SY - 2, CAGE_SY + CAGE_H + 1);

    int frame = 0;
    int hatch_stage = 0;
    int hatch_timer = 0;
    clock_t last_tick = clock();

    while (1) {
        if ((clock() - last_tick) >= ANIM_TICK * CLOCKS_PER_SEC) {
            if (hatch_stage == 0) {
                frame = !frame;
                hatch_timer++;
                drawBackground(CAGE_SX, CAGE_SY);
                drawSprite(frame ? egg_2 : egg_1, CAGE_MX, CAGE_SY);
                if (hatch_timer >= 6)
                    hatch_stage = 1;
            }
            else if (hatch_stage == 1) {
                drawBackground(CAGE_SX, CAGE_SY);
                drawSprite(egg_3, CAGE_MX, CAGE_SY);
                hatch_stage = 2;
            }
            else if (hatch_stage == 2) {
                drawBackground(CAGE_SX, CAGE_SY);
                drawSprite(agumonsprite, CAGE_MX, CAGE_SY);
            }
            last_tick = clock();
        }

        if (_kbhit()) {
            int key = _getch();
            if (key == 'c') {
                selected = (selected + 1) % 10;
                drawMenu(CAGE_SX, CAGE_SY - 2, CAGE_SY + CAGE_H + 1);
            }
            if (key == 'q') break;
        }
    }
    return 0;
}