#define _CRT_SECURE_NO_WARNINGS
#include "event.h"
#include "digimon.h"
#include "sprites.h"
#include "ui.h"
#include <conio.h>
#include <string.h>

/* 대문자 알파벳 A~Z */
char Alpha[26] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};

/* =========================================================
 * newGame
 *   게임 최초 시작 또는 저장 데이터 없을 때 호출.
 *   1) 테이머 이름 입력 (4글자, A~Z)
 *   2) 알 선택 (현재 1종류, C로 확정)
 *   3) is_digimon = true, init_digimon 호출
 * ========================================================= */
void newGame(GameData* game) {
    /* 테이머 기본 초기화 */
    memset(game->tamer.name, 0, MAX_NAME_LEN);
    game->tamer.is_digimon = false;
    game->tamer.battles    = 0;

    /* ── 1단계: 이름 입력 ── */
    drawBackground();
    drawBgSprite(background_name_1, CAGE_START_X, CAGE_START_Y);

    int count     = 0;   // 현재 알파벳 인덱스 (0=A ~ 25=Z)
    int namecount = 0;   // 확정된 글자 수

    /* 첫 번째 슬롯에 초기 알파벳 표시 */
    drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
    flush_cage();

    while (namecount < 4) {
        int key = _getch();
        switch (key) {
        case 'z': case 'Z':
            count = (count == 0) ? 25 : count - 1;
            clearFont(CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            flush_cage();
            break;
        case 'x': case 'X':
            count = (count == 25) ? 0 : count + 1;
            clearFont(CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            flush_cage();
            break;
        case 'c': case 'C':
            game->tamer.name[namecount] = Alpha[count];
            namecount++;
            count = 0;
            if (namecount < 4) {
                drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
                flush_cage();
            }
            break;
        }
    }
    game->tamer.name[4] = '\0';

    selectEgg(game);
}

/* =========================================================
 * selectEgg
 *   2단계: 알 선택 (현재는 1종류 → C로 즉시 확정)
 *   3단계: 알 확정 후 is_digimon = true, init_digimon 호출
 *
 *   TODO: 알이 여러 종류일 때 Z/X로 선택 후 C로 확정
 * ========================================================= */
void selectEgg(GameData* game) {
    /* ── 2단계: 알 선택 ── */
    drawBackground();
    drawSprite(sprite_egg_1, CAGE_MID_X, CAGE_START_Y, false);
    flush_cage();

    while (1) {
        int key = _getch();
        if (key == 'c' || key == 'C') break;
    }

    /* ── 3단계: 알 확정 → 디지몬 초기화 ── */
    game->tamer.is_digimon = true;
    init_digimon(game);
}

/* =========================================================
 * hatch
 *   매 프레임 호출.
 *   game->hatch_target_idx >= 0 일 때 부화 애니메이션을 재생하고,
 *   애니메이션이 끝나면 evolve_to() 로 실제 진화를 확정한다.
 *
 *   hatch_anim_started : 애니메이션 시작 여부 플래그 (main 에서 관리,
 *                        사망 시 false 로 리셋해야 함)
 * ========================================================= */
/* =========================================================
 * battle_event
 *   배틀 버튼(메뉴 2번) 에서 호출.
 *
 *   레이아웃:
 *     케이지 좌반(16px): 플레이어 스프라이트
 *     케이지 우반(16px): 상대 스프라이트 (좌우 반전)
 *     행 22:             DP 바 + 이름
 *     행 23:             회피/피격/공격 프롬프트
 *
 *   회피 성공률 = 자기 파워 / (플레이어파워 + CPU파워)
 *   - 플레이어: C키를 눌러 회피 시도 (500ms 창)
 *   - CPU:      파워 비례 자동 회피
 * ========================================================= */
void battle_event(GameData* game, AnimState* anim) {
    Digimon *d = &game->current;
    if (d->level < ROOKIE || d->dp <= 0 || d->is_sleep) return;

    BattleOpponent opp;
    gen_cpu_opponent(&opp, d->level);

    int pp    = calc_power(d);
    int cp    = opp.power;
    int total = pp + cp;
    if (total == 0) total = 1;

    int p_dp   = d->dp;
    int c_dp   = max_dp_table[digimon_table[opp.table_idx].level];
    int max_dp = d->max_dp;
    if (max_dp < 1) max_dp = 1;

    /* 위치 상수 (로컬) */
    int p_x      = CAGE_START_X;          /* 플레이어 스프라이트 x (케이지 좌반) */
    int c_x      = CAGE_START_X + 16;     /* 상대 스프라이트 x (케이지 우반) */
    int sp_y     = CAGE_START_Y;          /* 스프라이트 y: 케이지 상단, 스프라이트가 16px로 꽉 채움 */
    int info_y   = CAGE_START_Y + CAGE_H + 1;
    int prompt_y = CAGE_START_Y + CAGE_H + 2;

    /* 스프라이트 포인터 */
    Level opp_level = digimon_table[opp.table_idx].level;
    int (*p_walk)[SPRITE_W] = anim_get_sprite(ANIM_WALK,   0, d->level,  d->table_idx);
    int (*p_atk )[SPRITE_W] = anim_get_sprite(ANIM_ATTACK, 0, d->level,  d->table_idx);
    int (*c_walk)[SPRITE_W] = anim_get_sprite(ANIM_WALK,   0, opp_level, opp.table_idx);

    /* ── 초기 화면 설정 ── */
    clear_status();
    clearMenuLines(info_y, prompt_y);
    drawBackground();
    drawSprite(p_walk, p_x, sp_y, false);  /* 플레이어: 오른쪽 바라봄 */
    drawSprite(c_walk, c_x, sp_y, true);   /* 상대:     왼쪽 바라봄 (반전) */
    flush_cage();
    draw_battle_info(p_dp, c_dp, max_dp, opp.name, info_y);

    /* ── 배틀 루프: 한 쪽 DP 0 될 때까지 ── */
    while (p_dp > 0 && c_dp > 0) {

        /* 이전 키 입력 비우기 */
        while (_kbhit()) _getch();

        /* 회피 프롬프트 */
        SetConsoleTextAttribute(hOut, 15);
        gotoxy(CAGE_START_X * 2, prompt_y);
        printf("%-50s", "C: 회피!");
        SetConsoleTextAttribute(hOut, BG_COLOR);

        /* ── CPU가 공격: 500ms 회피 입력 창 ── */
        bool dodged = false;
        ULONGLONG deadline = GetTickCount64() + 500;
        ULONGLONG now_ms;
        while ((now_ms = GetTickCount64()) < deadline) {
            if (_kbhit()) {
                int k = _getch();
                if (k == 'c' || k == 'C') {
                    dodged = (rand() % total < pp);
                    break;
                }
            }
            Sleep(16);
        }

        if (!dodged) {
            p_dp--;
            /* 플레이어 피격: 스프라이트 3회 깜빡임 */
            for (int i = 0; i < 3; i++) {
                clearSprite(p_x, sp_y);
                flush_cage();
                Sleep(60);
                drawSprite(p_walk, p_x, sp_y, false);
                flush_cage();
                Sleep(60);
            }
        }

        /* 회피/피격 결과 */
        SetConsoleTextAttribute(hOut, 15);
        gotoxy(CAGE_START_X * 2, prompt_y);
        printf("%-50s", dodged ? "회피!" : "피격!");
        SetConsoleTextAttribute(hOut, BG_COLOR);
        draw_battle_info(p_dp, c_dp, max_dp, opp.name, info_y);
        Sleep(300);

        if (p_dp <= 0) break;

        /* ── 플레이어 공격: 공격 스프라이트 표시 (400ms) ── */
        clearSprite(p_x, sp_y);
        drawSprite(p_atk, p_x, sp_y, false);
        flush_cage();
        Sleep(200);

        /* CPU 자동 회피: 파워 비례 */
        bool cpu_dodged = (rand() % total < cp);
        if (!cpu_dodged) {
            c_dp--;
            /* 상대 피격: 스프라이트 3회 깜빡임 */
            for (int i = 0; i < 3; i++) {
                clearSprite(c_x, sp_y);
                flush_cage();
                Sleep(60);
                drawSprite(c_walk, c_x, sp_y, true);
                flush_cage();
                Sleep(60);
            }
        }

        /* 공격 모션 종료: walk 스프라이트로 복귀 */
        clearSprite(p_x, sp_y);
        drawSprite(p_walk, p_x, sp_y, false);
        flush_cage();

        /* DP 갱신 표시 + CPU 회피/피격 결과 */
        SetConsoleTextAttribute(hOut, 15);
        gotoxy(CAGE_START_X * 2, prompt_y);
        printf("%-50s", cpu_dodged ? "상대 회피!" : "상대 피격!");
        SetConsoleTextAttribute(hOut, BG_COLOR);
        draw_battle_info(p_dp, c_dp, max_dp, opp.name, info_y);
        Sleep(300);
    }

    /* ── 결과 처리 ── */
    BattleResult result;
    if      (p_dp > 0 && c_dp <= 0) result = BATTLE_WIN;
    else if (p_dp <= 0 && c_dp > 0) result = BATTLE_LOSE;
    else                             result = BATTLE_DRAW;

    d->battles++;
    game->tamer.battles++;
    if (result == BATTLE_WIN) d->wins++;
    d->dp = p_dp > 0 ? p_dp : 0;

    /* ── 결과 표시 (1500ms) ── */
    SetConsoleTextAttribute(hOut, 15);
    gotoxy(CAGE_START_X * 2, info_y);
    printf("%-50s", result == BATTLE_WIN  ? "WIN!" :
                    result == BATTLE_LOSE ? "LOSE" : "DRAW");
    gotoxy(CAGE_START_X * 2, prompt_y);
    printf("%-50s", "");
    SetConsoleTextAttribute(hOut, BG_COLOR);
    Sleep(1500);

    /* ── 화면 복원 ── */
    drawBackground();
    flush_cage();
    drawMenu();
    draw_status(d);
    draw_call_alert(game->is_call);
    anim_play(anim, ANIM_WALK, 0, GetTickCount64());
}

void hatch(GameData* game, AnimState* anim, bool* hatch_anim_started, ULONGLONG now_ms) {
    if (game->hatch_target_idx < 0) return;

    if (game->current.level == EGG) {
        /* EGG: 부화 흔들림 + 균열 (4초) */
        if (!*hatch_anim_started) {
            anim_play(anim, ANIM_HATCH, 4000, now_ms);
            *hatch_anim_started = true;
        } else if (anim->kind != ANIM_HATCH) {
            evolve_to(game, game->hatch_target_idx);
            game->hatch_target_idx = -1;
            *hatch_anim_started    = false;
            anim_init(anim, game->current.level, now_ms);
            save_game(game);   // 부화 완료 → 즉시 저장
        }
    } else {
        /* 비-EGG: 진화 깜빡임 (1초) */
        if (!*hatch_anim_started) {
            anim_play(anim, ANIM_EVOLVE, 1000, now_ms);
            *hatch_anim_started = true;
        } else if (anim->kind != ANIM_EVOLVE) {
            evolve_to(game, game->hatch_target_idx);
            game->hatch_target_idx = -1;
            *hatch_anim_started    = false;
            anim_init(anim, game->current.level, now_ms);
            save_game(game);   // 진화 완료 → 즉시 저장
        }
    }
}
