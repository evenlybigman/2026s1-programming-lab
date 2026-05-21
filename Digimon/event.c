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

    if (game->tamer.is_digimon) {
        /* TODO: 저장 데이터 로드 */
        return;
    }

    /* ── 1단계: 이름 입력 ── */
    drawBackground(CAGE_START_X, CAGE_START_Y);
    drawBgSprite(background_name_1, CAGE_START_X, CAGE_START_Y);

    int count     = 0;   // 현재 알파벳 인덱스 (0=A ~ 25=Z)
    int namecount = 0;   // 확정된 글자 수

    /* 첫 번째 슬롯에 초기 알파벳 표시 */
    drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);

    while (namecount < 4) {
        int key = _getch();
        switch (key) {
        case 'z': case 'Z':
            /* 이전 알파벳 (A에서 Z로 순환) */
            count = (count == 0) ? 25 : count - 1;
            clearFont(CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            break;
        case 'x': case 'X':
            /* 다음 알파벳 (Z에서 A로 순환) */
            count = (count == 25) ? 0 : count + 1;
            clearFont(CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            break;
        case 'c': case 'C':
            /* 현재 알파벳 확정, 다음 칸으로 */
            game->tamer.name[namecount] = Alpha[count];
            namecount++;
            count = 0;  // 다음 글자는 A부터
            if (namecount < 4)
                drawFont(Alpha[count], CAGE_START_X + namecount * 8, CAGE_START_Y + 8);
            break;
        }
    }
    game->tamer.name[4] = '\0';

    /* ── 2~3단계: 알 선택 → 디지몬 초기화 ── */
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
    drawBackground(CAGE_START_X, CAGE_START_Y);
    drawSprite(sprite_egg_1, CAGE_MID_X, CAGE_START_Y, false);

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
void hatch(GameData* game, AnimState* anim, bool* hatch_anim_started, ULONGLONG now_ms) {
    if (game->hatch_target_idx < 0) return;

    if (!*hatch_anim_started) {
        anim_play(anim, ANIM_HATCH, 4000, now_ms);
        *hatch_anim_started = true;
    } else if (anim->kind != ANIM_HATCH) {
        /* 애니메이션 종료 → 진화 확정 */
        evolve_to(game, game->hatch_target_idx);
        game->hatch_target_idx = -1;
        *hatch_anim_started    = false;
        anim_init(anim, game->current.level, now_ms);
    }
}
