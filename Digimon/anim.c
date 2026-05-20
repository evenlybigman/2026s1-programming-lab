#include "anim.h"
#include "ui.h"
#include <stdlib.h>

/* =========================================================
 * 스프라이트 선택
 *   (level, kind, frame) → 스프라이트 포인터
 *   새 디지몬 추가 시 이 함수에 분기만 추가한다.
 * ========================================================= */
static int (*get_sprite(AnimKind kind, int frame, Level level))[SPRITE_W] {
    switch (level) {
    case EGG:
        return frame ? sprite_egg_2 : sprite_egg_1;

    case BABY1:
        switch (kind) {
        case ANIM_WALK:   return frame ? sprite_botamon_2 : sprite_botamon_1;
        case ANIM_HAPPY:  return sprite_botamon_3;
        case ANIM_ATTACK: return sprite_botamon_4;
        case ANIM_EAT:    return frame ? sprite_botamon_6 : sprite_botamon_5;
        case ANIM_INJURY: return sprite_botamon_7;
        case ANIM_SLEEP:  return sprite_botamon_8;
        default:          return sprite_botamon_1;
        }

    default:
        return sprite_agumon_1;
    }
}

/* =========================================================
 * anim_init
 * ========================================================= */
void anim_init(AnimState *anim, Level level, ULONGLONG now_ms) {
    anim->kind              = (level == EGG) ? ANIM_EGG : ANIM_WALK;
    anim->frame             = 0;
    anim->last_frame_ms     = now_ms;
    anim->frame_interval_ms = (level == EGG) ? 500 : 400;
    anim->x                 = CAGE_MID_X;
    anim->y                 = CAGE_START_Y + 2;
    anim->dx                = 1;
    anim->move_timer        = 5;
    anim->last_move_ms      = now_ms;
    anim->remaining_ms      = 0;
    anim->state_start_ms    = now_ms;

    /* 진화 직후 이전 스프라이트를 지운다 */
    clearSprite(anim->x, anim->y);
}

/* =========================================================
 * anim_play
 * ========================================================= */
void anim_play(AnimState *anim, AnimKind kind, ULONGLONG duration_ms, ULONGLONG now_ms) {
    if (anim->kind > kind) return;  // 낮은 우선순위는 무시

    anim->kind           = kind;
    anim->frame          = 0;
    anim->remaining_ms   = duration_ms;
    anim->state_start_ms = now_ms;
    anim->last_frame_ms  = now_ms;
}

/* =========================================================
 * anim_update
 * ========================================================= */
void anim_update(AnimState *anim, ULONGLONG now_ms, Level level) {
    bool needs_redraw = false;

    /* 1. 상태 만료 → WALK(또는 EGG)로 복귀 */
    if (anim->remaining_ms > 0 &&
        now_ms - anim->state_start_ms >= anim->remaining_ms) {
        anim->kind           = (level == EGG) ? ANIM_EGG : ANIM_WALK;
        anim->remaining_ms   = 0;
        anim->state_start_ms = now_ms;
        anim->frame          = 0;
        needs_redraw         = true;
    }

    /* 2. 프레임 교체 */
    if (now_ms - anim->last_frame_ms >= anim->frame_interval_ms) {
        anim->frame ^= 1;
        anim->last_frame_ms = now_ms;
        needs_redraw        = true;
    }

    /* 3. 이동 (WALK만, ANIM_MOVE_INTERVAL_MS마다 한 칸) */
    if (anim->kind == ANIM_WALK &&
        now_ms - anim->last_move_ms >= ANIM_MOVE_INTERVAL_MS) {

        int min_x = CAGE_START_X;
        int max_x = CAGE_START_X + CAGE_W - SPRITE_W;

        if (anim->dx != 0) {
            anim->x += anim->dx;
            if (anim->x <= min_x) { anim->x = min_x; anim->dx =  1; }
            if (anim->x >= max_x) { anim->x = max_x; anim->dx = -1; }
            needs_redraw = true;
        }

        if (--anim->move_timer <= 0) {
            int r = rand() % 3;
            anim->dx         = (r == 0) ? 0 : (r == 1) ? -1 : 1;
            anim->move_timer = 3 + rand() % 5;
        }
        anim->last_move_ms = now_ms;
    }

    /* 4. 변화가 있을 때만 그리기 */
    if (needs_redraw) {
        clearSprite(anim->x, anim->y);
        drawSprite(get_sprite(anim->kind, anim->frame, level), anim->x, anim->y);
    }
}
