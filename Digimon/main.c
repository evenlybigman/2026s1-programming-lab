#include "digimon.h"
#include "ui.h"
#include "anim.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

int main(void) {
    srand((unsigned)time(NULL));
    ui_init();

    GameData game;
    init_digimon(&game);

    drawBackground(CAGE_START_X, CAGE_START_Y);
    drawMenu();

    ULONGLONG now_ms = GetTickCount64();
    AnimState anim;
    anim_init(&anim, game.current.level, now_ms);

    time_t last_status = time(NULL);

    while (1) {
        now_ms = GetTickCount64();
        time_t now = time(NULL);

        /* 키 입력 */
        if (_kbhit()) {
            int key = _getch();
            switch (key) {
            case 'c':
                menu_selected++;
                if (menu_selected == MENU_TOTAL_COUNT) menu_selected = 0;
                drawMenu();
                break;
            }
        }

        /* 애니메이션 갱신 */
        anim_update(&anim, now_ms, game.current.level);

        /* 게임 수치 갱신: 1초마다 */
        if (now - last_status >= 1) {
            game_tick(&game);
            last_status = now;
        }
    }

    return 0;
}
