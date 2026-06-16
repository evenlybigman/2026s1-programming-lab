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
    /* 저장 파일이 있고 디지몬이 살아있으면 불러오기, 아니면 새 게임 */
    if (!load_game(&game) || !game.tamer.is_digimon) {
        newGame(&game);   // 이름 입력 + 알 선택 + init_digimon
    } else {
        apply_offline_time(&game);
    }

    /* 게임 화면 초기 출력 */
    drawBackground();
    drawMenu();
    draw_status(&game.current);
    draw_call_alert(game.is_call);

    ULONGLONG now_ms = GetTickCount64();
    AnimState anim;
    anim_init(&anim, game.current.level, now_ms);

    time_t    last_status        = time(NULL);
    time_t    last_save          = time(NULL);
    bool      hatch_anim_started = false;

    /* 콜 깜빡임 */
    ULONGLONG last_blink_ms = 0;
    bool      blink_on      = true;
    bool      prev_is_call  = false;

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
                switch (menu_selected) {
                case 0: /* 먹이 */
                    action_feed(&game);
                    anim_play(&anim, ANIM_EAT, 1500, now_ms);
                    break;
                case 1: /* 훈련 */
                    action_train(&game);
                    anim_play(&anim, ANIM_ATTACK, 800, now_ms);
                    break;
                case 2: /* 배틀 — TODO */
                    break;
                case 3: /* 청소 */
                    action_clean(&game);
                    break;
                case 4: /* 잠 */
                    action_sleep_toggle(&game);
                    if (game.current.is_sleep)
                        anim_play(&anim, ANIM_SLEEP, 0, now_ms);
                    else
                        anim_play(&anim, ANIM_WALK,  0, now_ms);
                    break;
                case 5: /* 치료 */
                    action_cure(&game);
                    break;
                case 6: /* 앨범 — TODO */
                    break;
                case 7: /* 상태 — TODO */
                    break;
                }
                break;
#ifdef _DEBUG
            case 't': case 'T':
                /* 디버그: 진화 타이머 즉시 만료 */
                game.current.level_start_time -= evolution_time[game.current.level];
                break;
#endif
            }
        }

        /* 부화/진화 애니메이션 처리 */
        hatch(&game, &anim, &hatch_anim_started, now_ms);

        /* 애니메이션 갱신 */
        anim_update(&anim, now_ms, &game.current);
        anim_check_random(&anim, &game.current, now_ms);

        /* 게임 수치 갱신: 1초마다 */
        if (now - last_status >= 1) {
            bool died = game_tick(&game);
            if (died) {
                hatch_anim_started = false;
                anim_init(&anim, game.current.level, now_ms);
                save_game(&game);   // 사망 → 즉시 저장
            }
            last_status = now;
            draw_status(&game.current);
        }

        /* 자동 저장: 60초마다 */
        if (now - last_save >= 60) {
            save_game(&game);
            last_save = now;
        }

        /* 콜 깜빡임: 500ms 토글 */
        if (game.is_call) {
            if (now_ms - last_blink_ms >= 500) {
                blink_on      = !blink_on;
                last_blink_ms = now_ms;
                draw_call_alert(blink_on);
            }
        } else if (prev_is_call) {
            /* 콜 해제됨 → 알림 지우기 */
            draw_call_alert(false);
            blink_on      = true;
            last_blink_ms = 0;
        }
        prev_is_call = game.is_call;
        Sleep(10); /* ~100fps 상한 */
    }
    return 0;
}
