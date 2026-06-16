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
<<<<<<< HEAD
    if (!load_game(&game)) {
        init_digimon(&game);
    } else {
        apply_offline_time(&game);
    }
=======
>>>>>>> d05f306ab6d5a88498c1550bc095858f089a8999

    /* 게임 시작: 이름 입력 → 알 선택 → init_digimon */
    newGame(&game);

    /* 게임 화면 초기 출력 */
    drawBackground(CAGE_START_X, CAGE_START_Y);
    drawMenu();
    draw_status(&game.current);
    draw_call_alert(game.is_call);

    ULONGLONG now_ms = GetTickCount64();
    AnimState anim;
    anim_init(&anim, game.current.level, now_ms);

<<<<<<< HEAD
    time_t    last_status        = time(NULL);
    time_t    last_save          = time(NULL);
    bool      hatch_anim_started = false;

    /* 콜 깜빡임 */ 
    ULONGLONG last_blink_ms  = 0;
    bool      blink_on       = true;
    bool      prev_is_call   = false;
=======
    time_t last_status        = time(NULL);
    bool   hatch_anim_started = false;
>>>>>>> d05f306ab6d5a88498c1550bc095858f089a8999

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

<<<<<<< HEAD
        /* 진화 애니메이션 처리 */
        if (game.hatch_target_idx >= 0) {
            if (game.current.level == EGG) {
                /* EGG: 부화 (흔들림 3초 + egg_3 1초) */
                if (!hatch_anim_started) {
                    anim_play(&anim, ANIM_HATCH, 4000, now_ms);
                    hatch_anim_started = true;
                } else if (anim.kind != ANIM_HATCH) {
                    evolve_to(&game, game.hatch_target_idx);
                    game.hatch_target_idx = -1;
                    hatch_anim_started    = false;
                    anim_init(&anim, game.current.level, now_ms);
                    save_game(&game);   // 진화 완료 → 즉시 저장
                }
            } else {
                /* 비-EGG: 진화 깜빡임 (1초) */
                if (!hatch_anim_started) {
                    anim_play(&anim, ANIM_EVOLVE, 1000, now_ms);
                    hatch_anim_started = true;
                } else if (anim.kind != ANIM_EVOLVE) {
                    evolve_to(&game, game.hatch_target_idx);
                    game.hatch_target_idx = -1;
                    hatch_anim_started    = false;
                    anim_init(&anim, game.current.level, now_ms);
                    save_game(&game);   // 진화 완료 → 즉시 저장
                }
            }
        }
=======
        /* 부화 애니메이션 처리 */
        hatch(&game, &anim, &hatch_anim_started, now_ms);
>>>>>>> d05f306ab6d5a88498c1550bc095858f089a8999

        /* 애니메이션 갱신 */
        anim_update(&anim, now_ms, game.current.level);
        anim_check_random(&anim, &game.current, now_ms, game.current.level);

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

<<<<<<< HEAD
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
        Sleep(10); /* ~100fps 상한*/
=======
        Sleep(10); /* ~100fps 상한 */
>>>>>>> d05f306ab6d5a88498c1550bc095858f089a8999
    }

    return 0;
}
