#include "digimon.h"
#include "ui.h"
#include "anim.h"
#include "event.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

/* =========================================================
 * 스프라이트 뷰어
 *   V 키로 진입. Z/X 로 이전/다음. C 로 게임 화면 복귀.
 * ========================================================= */
static void sprite_viewer(const Digimon *d) {
    struct SpEnt { const char *name; int (*sp16)[SPRITE_W]; int (*sp8)[UI_SPRITE_W]; };
    static const struct SpEnt list[] = {
        /* ── 알 ── */
        {"egg_1",               sprite_egg_1,              NULL},
        {"egg_2",               sprite_egg_2,              NULL},
        {"egg_3",               sprite_egg_3,              NULL},
        /* ── 보타몬 ── */
        {"botamon_1 walk1",      sprite_botamon_1,           NULL},
        {"botamon_2 walk2",      sprite_botamon_2,           NULL},
        {"botamon_3 happy",      sprite_botamon_3,           NULL},
        {"botamon_4 angry",      sprite_botamon_4,           NULL},
        {"botamon_5 eat1",       sprite_botamon_5,           NULL},
        {"botamon_6 eat2",       sprite_botamon_6,           NULL},
        {"botamon_7 injury",     sprite_botamon_7,           NULL},
        {"botamon_8 sleep",      sprite_botamon_8,           NULL},
        /* ── 코로몬 ── */
        {"koromon_1 walk1",      sprite_koromon_1,           NULL},
        {"koromon_2 walk2",      sprite_koromon_2,           NULL},
        {"koromon_3 happy",      sprite_koromon_3,           NULL},
        {"koromon_4 angry",      sprite_koromon_4,           NULL},
        {"koromon_5 eat1",       sprite_koromon_5,           NULL},
        {"koromon_6 eat2",       sprite_koromon_6,           NULL},
        {"koromon_7 injury",     sprite_koromon_7,           NULL},
        {"koromon_8 sleep",      sprite_koromon_8,           NULL},
        /* ── 아구몬 ── */
        {"agumon_1 walk1",       sprite_agumon_1,            NULL},
        {"agumon_2 walk2",       sprite_agumon_2,            NULL},
        {"agumon_3 happy",       sprite_agumon_3,            NULL},
        {"agumon_4 angry",       sprite_agumon_4,            NULL},
        {"agumon_5 eat1",        sprite_agumon_5,            NULL},
        {"agumon_6 eat2",        sprite_agumon_6,            NULL},
        {"agumon_7 injury",      sprite_agumon_7,            NULL},
        {"agumon_8 sleep",       sprite_agumon_8,            NULL},
        /* ── 베타몬 ── */
        {"betamon_1 walk1",      sprite_betamon_1,           NULL},
        {"betamon_2 walk2",      sprite_betamon_2,           NULL},
        {"betamon_3 happy",      sprite_betamon_3,           NULL},
        {"betamon_4 angry",      sprite_betamon_4,           NULL},
        {"betamon_5 eat1",       sprite_betamon_5,           NULL},
        {"betamon_6 eat2",       sprite_betamon_6,           NULL},
        {"betamon_7 injury",     sprite_betamon_7,           NULL},
        {"betamon_8 sleep",      sprite_betamon_8,           NULL},
        /* ── 그레이몬 ── */
        {"greymon_1 walk1",      sprite_greymon_1,           NULL},
        {"greymon_2 walk2",      sprite_greymon_2,           NULL},
        {"greymon_3 happy",      sprite_greymon_3,           NULL},
        {"greymon_4 angry",      sprite_greymon_4,           NULL},
        {"greymon_5 eat1",       sprite_greymon_5,           NULL},
        {"greymon_6 eat2",       sprite_greymon_6,           NULL},
        {"greymon_7 injury",     sprite_greymon_7,           NULL},
        {"greymon_8 sleep",      sprite_greymon_8,           NULL},
        /* ── 데블몬 ── */
        {"devimon_1 walk1",      sprite_devimon_1,           NULL},
        {"devimon_2 walk2",      sprite_devimon_2,           NULL},
        {"devimon_3 happy",      sprite_devimon_3,           NULL},
        {"devimon_4 angry",      sprite_devimon_4,           NULL},
        {"devimon_5 eat1",       sprite_devimon_5,           NULL},
        {"devimon_6 eat2",       sprite_devimon_6,           NULL},
        {"devimon_7 injury",     sprite_devimon_7,           NULL},
        {"devimon_8 sleep",      sprite_devimon_8,           NULL},
        /* ── 에어드라몬 ── */
        {"airdramon_1 walk1",    sprite_airdramon_1,         NULL},
        {"airdramon_2 walk2",    sprite_airdramon_2,         NULL},
        {"airdramon_3 happy",    sprite_airdramon_3,         NULL},
        {"airdramon_4 angry",    sprite_airdramon_4,         NULL},
        {"airdramon_5 eat1",     sprite_airdramon_5,         NULL},
        {"airdramon_6 eat2",     sprite_airdramon_6,         NULL},
        {"airdramon_7 injury",   sprite_airdramon_7,         NULL},
        {"airdramon_8 sleep",    sprite_airdramon_8,         NULL},
        /* ── 티라노몬 ── */
        {"tyrannomon_1 walk1",   sprite_tyrannomon_1,        NULL},
        {"tyrannomon_2 walk2",   sprite_tyrannomon_2,        NULL},
        {"tyrannomon_3 happy",   sprite_tyrannomon_3,        NULL},
        {"tyrannomon_4 angry",   sprite_tyrannomon_4,        NULL},
        {"tyrannomon_5 eat1",    sprite_tyrannomon_5,        NULL},
        {"tyrannomon_6 eat2",    sprite_tyrannomon_6,        NULL},
        {"tyrannomon_7 injury",  sprite_tyrannomon_7,        NULL},
        {"tyrannomon_8 sleep",   sprite_tyrannomon_8,        NULL},
        /* ── 메라몬 ── */
        {"meramon_1 walk1",      sprite_meramon_1,           NULL},
        {"meramon_2 walk2",      sprite_meramon_2,           NULL},
        {"meramon_3 happy",      sprite_meramon_3,           NULL},
        {"meramon_4 angry",      sprite_meramon_4,           NULL},
        {"meramon_5 eat1",       sprite_meramon_5,           NULL},
        {"meramon_6 eat2",       sprite_meramon_6,           NULL},
        {"meramon_7 injury",     sprite_meramon_7,           NULL},
        {"meramon_8 sleep",      sprite_meramon_8,           NULL},
        /* ── 시드라몬 ── */
        {"seadramon_1 walk1",    sprite_seadramon_1,         NULL},
        {"seadramon_2 walk2",    sprite_seadramon_2,         NULL},
        {"seadramon_3 happy",    sprite_seadramon_3,         NULL},
        {"seadramon_4 angry",    sprite_seadramon_4,         NULL},
        {"seadramon_5 eat1",     sprite_seadramon_5,         NULL},
        {"seadramon_6 eat2",     sprite_seadramon_6,         NULL},
        {"seadramon_7 injury",   sprite_seadramon_7,         NULL},
        {"seadramon_8 sleep",    sprite_seadramon_8,         NULL},
        /* ── 워매몬 ── */
        {"wormmon_1 walk1",      sprite_wormmon_1,           NULL},
        {"wormmon_2 walk2",      sprite_wormmon_2,           NULL},
        {"wormmon_3 happy",      sprite_wormmon_3,           NULL},
        {"wormmon_4 angry",      sprite_wormmon_4,           NULL},
        {"wormmon_5 eat1",       sprite_wormmon_5,           NULL},
        {"wormmon_6 eat2",       sprite_wormmon_6,           NULL},
        {"wormmon_7 injury",     sprite_wormmon_7,           NULL},
        {"wormmon_8 sleep",      sprite_wormmon_8,           NULL},
        /* ── 메탈그레이몬 ── */
        {"metalgreymon_1 walk1", sprite_metalgreymon_1,      NULL},
        {"metalgreymon_2 walk2", sprite_metalgreymon_2,      NULL},
        {"metalgreymon_3 happy", sprite_metalgreymon_3,      NULL},
        {"metalgreymon_4 angry", sprite_metalgreymon_4,      NULL},
        {"metalgreymon_5 eat1",  sprite_metalgreymon_5,      NULL},
        {"metalgreymon_6 eat2",  sprite_metalgreymon_6,      NULL},
        {"metalgreymon_7 injury", sprite_metalgreymon_7,      NULL},
        {"metalgreymon_8 sleep", sprite_metalgreymon_8,      NULL},
        /* ── 콩알몬 ── */
        {"sukamon_1 walk1",      sprite_sukamon_1,           NULL},
        {"sukamon_2 walk2",      sprite_sukamon_2,           NULL},
        {"sukamon_3 happy",      sprite_sukamon_3,           NULL},
        {"sukamon_4 angry",      sprite_sukamon_4,           NULL},
        {"sukamon_5 eat1",       sprite_sukamon_5,           NULL},
        {"sukamon_6 eat2",       sprite_sukamon_6,           NULL},
        {"sukamon_7 injury",     sprite_sukamon_7,           NULL},
        {"sukamon_8 sleep",      sprite_sukamon_8,           NULL},
        /* ── 퍼펫몬 ── */
        {"puppetmon_1 walk1",    sprite_puppetmon_1,         NULL},
        {"puppetmon_2 walk2",    sprite_puppetmon_2,         NULL},
        {"puppetmon_3 happy",    sprite_puppetmon_3,         NULL},
        {"puppetmon_4 angry",    sprite_puppetmon_4,         NULL},
        {"puppetmon_5 eat1",     sprite_puppetmon_5,         NULL},
        {"puppetmon_6 eat2",     sprite_puppetmon_6,         NULL},
        {"puppetmon_7 injury",   sprite_puppetmon_7,         NULL},
        {"puppetmon_8 sleep",    sprite_puppetmon_8,         NULL},
        /* ── 블리츠그레이몬 ── */
        {"blitzgreymon_1 walk1", sprite_blitzgreymon_1,      NULL},
        {"blitzgreymon_2 walk2", sprite_blitzgreymon_2,      NULL},
        {"blitzgreymon_3 happy", sprite_blitzgreymon_3,      NULL},
        {"blitzgreymon_4 angry", sprite_blitzgreymon_4,      NULL},
        {"blitzgreymon_5 eat1",  sprite_blitzgreymon_5,      NULL},
        {"blitzgreymon_6 eat2",  sprite_blitzgreymon_6,      NULL},
        {"blitzgreymon_7 injury", sprite_blitzgreymon_7,      NULL},
        {"blitzgreymon_8 sleep", sprite_blitzgreymon_8,      NULL},
        /* ── 반초콩알몬 ── */
        {"banchoukomon_1 walk1", sprite_banchoukomon_1,      NULL},
        {"banchoukomon_2 walk2", sprite_banchoukomon_2,      NULL},
        {"banchoukomon_3 happy", sprite_banchoukomon_3,      NULL},
        {"banchoukomon_4 angry", sprite_banchoukomon_4,      NULL},
        {"banchoukomon_5 eat1",  sprite_banchoukomon_5,      NULL},
        {"banchoukomon_6 eat2",  sprite_banchoukomon_6,      NULL},
        {"banchoukomon_7 injury", sprite_banchoukomon_7,      NULL},
        {"banchoukomon_8 sleep", sprite_banchoukomon_8,      NULL},
        /* ── 사망 (16×16) ── */
        {"dead",                sprite_dead,               NULL},
        /* ── UI 아이콘 (8×8) ── */
        {"food",                NULL,                      sprite_food},
        {"poop1",               NULL,                      sprite_poop1},
        {"poop2",               NULL,                      sprite_poop2},
        {"zzz",                 NULL,                      sprite_zzz},
        {"medicine",            NULL,                      sprite_medicine},
    };
    static const int TOTAL = (int)(sizeof(list) / sizeof(list[0]));

    int idx = 0;

    /* 뷰어 루프 */
    while (1) {
        /* 케이지 영역 배경색으로 초기화 */
        for (int py = CAGE_START_Y; py < CAGE_START_Y + CAGE_H; py++)
            for (int px = CAGE_START_X; px < CAGE_START_X + CAGE_W; px++)
                drawPixel(px, py, BG_COLOR);

        const struct SpEnt *e = &list[idx];
        if (e->sp16) {
            /* 16×16 — 케이지 중앙 배치 */
            int sx = CAGE_START_X + (CAGE_W - SPRITE_W) / 2;
            int sy = CAGE_START_Y + (CAGE_H - SPRITE_H) / 2;
            drawSprite(e->sp16, sx, sy, false);
        } else {
            /* 8×8 — 2배 확대해 중앙 배치 */
            int sx = CAGE_START_X + (CAGE_W - UI_SPRITE_W * 2) / 2;
            int sy = CAGE_START_Y + (CAGE_H - UI_SPRITE_H * 2) / 2;
            for (int y = 0; y < UI_SPRITE_H; y++) {
                for (int x = 0; x < UI_SPRITE_W; x++) {
                    int pix = e->sp8[y][x];
                    if (pix == PIXEL_BG) continue;
                    int c = (pix >= 0 && pix < COLORMAP_SIZE) ? colorMap[pix] : 15;
                    drawPixel(sx + x*2,     sy + y*2,     c);
                    drawPixel(sx + x*2 + 1, sy + y*2,     c);
                    drawPixel(sx + x*2,     sy + y*2 + 1, c);
                    drawPixel(sx + x*2 + 1, sy + y*2 + 1, c);
                }
            }
        }

        /* 버퍼 → 화면 출력 */
        flush_cage();

        /* 이름 / 네비게이션 텍스트 */
        SetConsoleTextAttribute(hOut, 15);
        gotoxy(0, CAGE_START_Y + CAGE_H + 1);
        printf("[%3d/%d] %-20s  Z:이전  X:다음  C:종료", idx + 1, TOTAL, e->name);

        /* 블로킹 키 입력 */
        int key = _getch();
        if      (key == 'z' || key == 'Z') idx = (idx - 1 + TOTAL) % TOTAL;
        else if (key == 'x' || key == 'X') idx = (idx + 1) % TOTAL;
        else if (key == 'c' || key == 'C') break;
    }

    /* 게임 화면 복구 */
    drawBackground();
    flush_cage();
    drawMenu();
    draw_status(d);
}

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

    /* 게임 화면 초기 출력 (EGG 중엔 메뉴/상태창 숨김) */
    drawBackground();
    flush_cage();
    if (game.current.level != EGG) {
        drawMenu();
        draw_status(&game.current);
        draw_call_alert(game.is_call);
    }

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
    Level     prev_level    = game.current.level;

    while (1) {
        now_ms = GetTickCount64();
        time_t now = time(NULL);

        /* 키 입력: Z(이전) / X(다음) / C(확인) — EGG 중엔 메뉴 조작 무시 */
        if (_kbhit()) {
            int key = _getch();
            if (game.current.level != EGG) {
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
                case 'v': case 'V':
                    sprite_viewer(&game.current);
                    break;
                case 'c': case 'C':
                    switch (menu_selected) {
                    case 0: /* 상태 */
                        show_status_screen(&game.current);
                        break;
                    case 1: /* 먹이 */
                        if (action_feed(&game))
                            anim_play(&anim, ANIM_EAT, 2400, now_ms);
                        else if (game.current.level != EGG && !game.current.is_sleep)
                            anim_play(&anim, ANIM_REFUSE, 800, now_ms);
                        draw_status(&game.current);
                        break;
                    case 2: /* 훈련 */
                        if (action_train(&game))
                            anim_play(&anim, ANIM_ATTACK, 800, now_ms);
                        draw_status(&game.current);
                        break;
                    case 3: /* 배틀 */
                        battle_event(&game, &anim);
                        break;
                    case 4: /* 청소 */
                        action_clean(&game);
                        draw_status(&game.current);
                        break;
                    case 5: /* 잠 */
                        action_sleep_toggle(&game);
                        draw_status(&game.current);
                        if (game.current.is_sleep)
                            anim_play(&anim, ANIM_SLEEP, 0, now_ms);
                        else
                            anim_play(&anim, ANIM_WALK,  0, now_ms);
                        break;
                    case 6: /* 치료 */
                        action_cure(&game);
                        draw_status(&game.current);
                        break;
                    case 7: /* 저장 */
                        save_game(&game);
                        break;
                    }
                    check_call(&game);
                    draw_call_alert(game.is_call);
                    break;
                }
            }
#ifdef _DEBUG
            if ((key == 't' || key == 'T') && game.current.level != MEGA)
                game.current.level_start_time -= evolution_time[game.current.level];
#endif
        }

        /* 부화/진화 애니메이션 처리 */
        hatch(&game, &anim, &hatch_anim_started, now_ms);

        /* EGG → 비-EGG 전환 또는 진화 시 UI 갱신 + 티커 리셋 */
        if (prev_level != game.current.level) {
            if (prev_level == EGG) {
                drawMenu();
                draw_status(&game.current);
            }
        }
        prev_level = game.current.level;

        /* 애니메이션 갱신 → 버퍼에 쓰고 한 번에 출력 */
        anim_update(&anim, now_ms, &game.current);
        draw_cage_ui(&game.current);
        anim_check_random(&anim, &game.current, now_ms);

        flush_cage();

        /* 게임 수치 갱신: 1초마다 */
        if (now - last_status >= 1) {
            bool was_sleep = game.current.is_sleep;
            bool died = game_tick(&game);
            if (died) {
                hatch_anim_started = false;
                anim_init(&anim, game.current.level, now_ms);
                save_game(&game);   // 사망 → 즉시 저장
                /* 사망 후 EGG로 돌아가므로 인터페이스 지우기 */
                clearMenuLines(CAGE_START_Y + CAGE_H + 1, CAGE_START_Y + CAGE_H + 2);
                clear_status();
                draw_call_alert(false);
                prev_level = EGG;
            }
            /* 자동 취침/기상 시 애니메이션 전환 */
            if (!died && game.current.is_sleep != was_sleep) {
                if (game.current.is_sleep)
                    anim_play(&anim, ANIM_SLEEP, 0, now_ms);
                else
                    anim_play(&anim, ANIM_WALK,  0, now_ms);
            }
            last_status = now;
            if (game.current.level != EGG)
                draw_status(&game.current);
        }

        /* 자동 저장: 60초마다 */
        if (now - last_save >= 60) {
            save_game(&game);
            last_save = now;
        }

        /* 콜 깜빡임: 500ms 토글 — EGG 중엔 표시하지 않음 */
        if (game.current.level != EGG) {
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
        }
        prev_is_call = game.is_call;
        Sleep(10); /* ~100fps 상한 */
    }
    return 0;
}
