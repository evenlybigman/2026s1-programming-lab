#include <stdio.h>
#include <windows.h>

#define SW 36 //전체 화면 가로 길이
#define SH 48 //전체 화면 세로 길이
#define SPRITE_W 32 // 픽셀 부분 가로 길이
#define SPRITE_H 16 // 픽셀 부분 세로 길이

void gotoxy(int x, int y){
	COORD pos = {x,y}; 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

