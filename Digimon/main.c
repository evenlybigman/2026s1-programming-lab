#include "digimon.h"
#include "ui.h"
#include "anim.h"
#include "event.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

int main(void) {
    srand((unsigned)time(NULL));
    ui_init();

    GameData game;

    /* 게임 시작: 이름 입력 → 알 선택 → init_digimon */
    newGame(&game);

    /* 게임 화면 초기 출력 */
    drawBackground(CAGE_START_X, CAGE_START_Y);
    drawMenu();

    ULONGLONG now_ms = GetTickCount64();
    AnimState anim;
    anim_init(&anim, game.current.level, now_ms);

    time_t last_status        = time(NULL);
    bool   hatch_anim_started = false;

    while (1) {
        now_ms = GetTickCount64();
        time_t now = time(NULL);

        /* 키 입력: Z(이전) / X(다음) / C(확인) */
        if (_kbhit()) {
            int key = _getch();
            switch (key) {
            case 'z': case 'Z':
                menu_selected--;
                if (menu_selected < 0) menu_selected = MENU_TOTAL_COUNT - 1;
                drawMenu();
                break;
            case 'x': case 'X':
                menu_selected++;
                if (menu_selected == MENU_TOTAL_COUNT) menu_selected = 0;
                drawMenu();
                break;
            case 'c': case 'C':
                /* TODO: 메뉴 확인 동작 */
                break;
#ifdef _DEBUG
            case 't': case 'T':
                /* 디버그: 진화 타이머 즉시 만료 */
                game.current.level_start_time -= evolution_time[game.current.level];
                break;
#endif
            }
        }

        /* 부화 애니메이션 처리 */
        hatch(&game, &anim, &hatch_anim_started, now_ms);

        /* 애니메이션 갱신 */
        anim_update(&anim, now_ms, game.current.level);
        anim_check_random(&anim, &game.current, now_ms, game.current.level);

        /* 게임 수치 갱신: 1초마다 */
        if (now - last_status >= 1) {
            bool died = game_tick(&game);
            if (died) {
                hatch_anim_started = false;
                anim_init(&anim, game.current.level, now_ms);
            }
            last_status = now;
        }

        Sleep(10); /* ~100fps 상한 */
    }

    return 0;
}
