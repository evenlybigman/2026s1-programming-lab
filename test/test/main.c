#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define UI1X 10
#define UI1Y 4
#define SW 32 //케이지 가로 길이
#define SH 16 //케이지 세로 길이
#define SPRITE_W 16 //디지몬 스프라이트 X
#define SPRITE_H 16 //디지몬 스프라이트 Y
#define CAGEX 5
#define CAGEY 5

HANDLE hOut;

// 픽셀용 (x*2)
void gotoxyPixel(int x, int y) {
    COORD pos = { x * 2, y };
    SetConsoleCursorPosition(hOut, pos);
}

// 텍스트용 (그대로)
void gotoxy(int x, int y) {
    COORD pos = { x, y };
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

int sprite[SPRITE_H][SPRITE_W] = {
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
    for (int y = 0; y < SH; y++) {
        for (int x = 0; x < SW; x++) {
            drawPixel(startX + x, startY + y, 7);
        }
    }
}

void drawSprite(int startX, int startY) {
    for (int y = 0; y < SPRITE_H; y++) {
        for (int x = 0; x < SPRITE_W; x++) {
            if (sprite[y][x] == 1) continue;  // 추가
            drawPixel(startX + x, startY + y, colorMap[sprite[y][x]]);
        }
    }
}

int main() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    setConsoleFont(24, 24);

    CONSOLE_CURSOR_INFO cci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &cci);

    drawBackground(5, 5);
    drawSprite(5, 5);

    SetConsoleTextAttribute(hOut, 7);
    gotoxy(UI1X, UI1Y);
    printf("밥 먹이기");
    _getch();




    return 0;
}