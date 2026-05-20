#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define CAGE_W 32 //케이지 가로 길이
#define CAGE_H 16 //케이지 세로 길이
#define SPRITE_W 16 //디지몬 스프라이트 X크기
#define SPRITE_H 16 //디지몬 스프라이트 Y크기
#define CAGE_SW 5 * 2
#define CAGE_SH 5

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

// 고기 아이콘
int icon_meat[8][8] = {
    {0,0,1,1,0,0,0,0},
    {0,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0},
};

// 트로피 아이콘
int icon_trophy[8][8] = {
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0},
};

// 역기 아이콘
int icon_train[8][8] = {
    {0,0,0,0,0,0,0,0},
    {1,1,0,1,1,0,1,1},
    {1,1,0,1,1,0,1,1},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {1,1,0,1,1,0,1,1},
    {1,1,0,1,1,0,1,1},
    {0,0,0,0,0,0,0,0},
};

int colorMap[] = { 0, 15 };

void drawBackground(int startX, int startY) {
    for (int y = 0; y < CAGE_H; y++) {
        for (int x = 0; x < CAGE_W; x++) {
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

char* menu_top[] = { "건강", "먹이", "훈련", "배틀", "청소" };
char* menu_bot[] = { "잠 ", "치료", "앨범", "상태", "호출" };
int menu_count = 10;
int selected = 4;  // 현재 선택된 메뉴

void drawMenu(int startX, int startTopY, int startBotY) {
    SetConsoleTextAttribute(hOut, 7);
    int x = startX;
    for (int i = 0; i < 5; i++) {
        // 상단
        gotoxy(x, startTopY);
        if (i == selected)
            printf("[ %s ]", menu_top[i]);
        else
            printf(" %s ", menu_top[i]);

        // 하단
        gotoxy(x, startBotY);
        if (i + 5 == selected)
            printf("[ %s ]", menu_bot[i]);
        else
            printf(" %s ", menu_bot[i]);

        // 더 긴 메뉴 기준으로 x 이동
        int top_len = strlen(menu_top[i]);
        int bot_len = strlen(menu_bot[i]);
        x += (top_len > bot_len ? top_len : bot_len) + 7;
    }
}

int main() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    setConsoleFont(24, 24);

    CONSOLE_CURSOR_INFO cci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &cci);

    drawBackground(5, 5);
    drawSprite(5, 5);
    drawMenu(CAGE_SW, CAGE_SH - 2, CAGE_SH + CAGE_H + 2);
    _getch();






    return 0;
}