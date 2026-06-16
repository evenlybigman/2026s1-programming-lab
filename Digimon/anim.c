#include "anim.h"
#include "ui.h"
#include <stdlib.h>

/* =========================================================
 * 스프라이트 선택
 *   (level, kind, frame) → 스프라이트 포인터
 *   새 디지몬 추가 시 이 함수에 분기만 추가한다.
 * ========================================================= */
/* get_sprite - (kind, frame, level, table_idx) → 스프라이트 포인터
 *
 * 새 디지몬 스프라이트 추가 방법:
 *   default 블록의 switch (table_idx) 에 case 하나 추가하면 된다.
 *   같은 레벨 내 여러 디지몬도 table_idx로 구분 가능.
 */
static int (*get_sprite(AnimKind kind, int frame, Level level, int table_idx))[SPRITE_W] {
    switch (level) {
    case EGG:
        if (kind == ANIM_HATCH)
            return frame == 2 ? sprite_egg_3 : sprite_egg_1;
        return frame ? sprite_egg_2 : sprite_egg_1;

    case BABY1:
        switch (kind) {
        case ANIM_WALK:
            if (frame == 2) return sprite_botamon_3;  // 멈춤 특수 포즈
            return frame ? sprite_botamon_2 : sprite_botamon_1;
        case ANIM_HAPPY:  return sprite_botamon_3;
        case ANIM_ATTACK: return sprite_botamon_4;
        case ANIM_EAT:    return frame ? sprite_botamon_6 : sprite_botamon_5;
        case ANIM_INJURY: return sprite_botamon_7;
        case ANIM_SLEEP:  return sprite_botamon_8;
        default:          return sprite_botamon_1;
        }

    default:
        /* TODO: table_idx 기반으로 각 디지몬 스프라이트 분기
         *   switch (table_idx) { case IDX_AGUMON: ... } 형태로 추가
         *   현재는 모든 BABY2 이상이 sprite_agumon_1 placeholder */
        (void)table_idx;
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
    anim->y                 = CAGE_START_Y;
    anim->draw_x            = CAGE_MID_X;
    anim->draw_y            = CAGE_START_Y;
    anim->dx                = 1;
    anim->flip              = false;
    anim->move_timer        = 5;
    anim->last_move_ms      = now_ms;
    anim->remaining_ms      = 0;
    anim->state_start_ms    = now_ms;
    anim->last_rand_ms      = now_ms;
    anim->stop_pose_end_ms  = 0;
    anim->last_stop_pose_ms = 0;

    clearSprite(anim->draw_x, anim->draw_y);
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

    /* 진화 깜빡임은 빠른 프레임 간격 사용 */
    if (kind == ANIM_EVOLVE)
        anim->frame_interval_ms = 120;
}

/* =========================================================
 * anim_update
 * ========================================================= */
void anim_update(AnimState *anim, ULONGLONG now_ms, const Digimon *d) {
    Level level     = d->level;
    int   table_idx = d->table_idx;
    bool needs_redraw = false;

    /* 1. 상태 만료 */
    if (anim->remaining_ms > 0 &&
        now_ms - anim->state_start_ms >= anim->remaining_ms) {
        bool was_hatch       = (anim->kind == ANIM_HATCH);
        bool was_evolve      = (anim->kind == ANIM_EVOLVE);
        clearSprite(anim->draw_x, anim->draw_y);
        anim->draw_x         = anim->x;
        anim->draw_y         = anim->y;
        anim->kind           = (level == EGG) ? ANIM_EGG : ANIM_WALK;
        anim->remaining_ms   = 0;
        anim->state_start_ms = now_ms;
        anim->frame          = 0;
        anim->last_rand_ms   = now_ms;  // 복귀 직후 랜덤 행동 즉시 재발생 방지
        anim->stop_pose_end_ms = 0;
        if (was_hatch || was_evolve) return; /* main.c가 다음 루프에서 감지해 evolve_to() 호출 */
        needs_redraw = true;
    }

    /* 2. ANIM_HATCH 전용: 흔들림(0~3초) → 균열(3초~)
     *   오프셋 순서: 중(0) 우(+2) 중(0) 좌(-2) 중(0) 우(+2), 각 500ms
     *   스프라이트: 흔들림 = egg_1 고정, 균열 = egg_3 */
    if (anim->kind == ANIM_HATCH) {
        static const int offsets[] = { 0, 1, 2, 1, 0, -1, -2, -1, 0, 1, 2, 1 };
        ULONGLONG elapsed = now_ms - anim->state_start_ms;
        int new_frame;
        int shake_x;

        if (elapsed < 3000) {
            int phase = (int)(elapsed / 250);
            if (phase > 11) phase = 11;
            shake_x   = anim->x + offsets[phase];
            new_frame = 0; // 항상 egg_1
        } else {
            shake_x   = anim->x;
            new_frame = 2; // egg_3
        }

        if (new_frame != anim->frame || shake_x != anim->draw_x) {
            clearSprite(anim->draw_x, anim->draw_y);
            anim->frame  = new_frame;
            anim->draw_x = shake_x;
            drawSprite(get_sprite(anim->kind, anim->frame, level, table_idx), anim->draw_x, anim->draw_y, false);
        }
        return;
    }

    /* 3. 프레임 교체 */
    if (now_ms - anim->last_frame_ms >= anim->frame_interval_ms) {
        if (anim->kind == ANIM_WALK) {
            if (anim->dx != 0) {
                /* 이동 중: 매 틱마다 WALK_SPRITE1_CHANCE% 확률로 sprite_1, 나머지는 sprite_2 */
                if (rand() % 100 < WALK_SPRITE1_CHANCE)
                    anim->frame = 0;  // sprite_1 (낮은 확률)
                else
                    anim->frame = 1;  // sprite_2 (기본)
            } else {
                /* 정지 중: 전환 모션(sprite_3) 타이머 처리, 기본은 frame=1(sprite_2) */
                if (anim->stop_pose_end_ms > 0) {
                    if (now_ms >= anim->stop_pose_end_ms) {
                        anim->stop_pose_end_ms = 0;
                        anim->frame = 1;  // 복귀: sprite_2
                    }
                    /* 아직 유지 중이면 frame=2(sprite_3) 그대로 */
                } else {
                    anim->frame = 1;  // 기본 정지: sprite_2
                }
            }
        } else {
            anim->frame ^= 1;
        }
        anim->last_frame_ms = now_ms;
        needs_redraw        = true;
    }

    /* 4. 이동 (WALK만, ANIM_MOVE_INTERVAL_MS마다 한 칸) */
    if (anim->kind == ANIM_WALK &&
        now_ms - anim->last_move_ms >= ANIM_MOVE_INTERVAL_MS) {

        int min_x = CAGE_START_X;
        int max_x = CAGE_START_X + CAGE_W - SPRITE_W;

        if (anim->dx != 0) {
            anim->x += anim->dx;
            if (anim->x <= min_x) { anim->x = min_x; anim->dx =  1; }
            if (anim->x >= max_x) { anim->x = max_x; anim->dx = -1; }
            anim->stop_pose_end_ms = 0;  // 이동 중엔 특수 포즈 취소
            needs_redraw = true;
        }

        if (--anim->move_timer <= 0) {
            int r      = rand() % 3;
            int new_dx = (r == 0) ? 0 : (r == 1) ? -1 : 1;

            if (new_dx == 0 && anim->dx != 0) {
                /* 이동 → 정지 전환: 쿨다운이 지난 경우에만 sprite_3 모션 */
                if (now_ms - anim->last_stop_pose_ms >= STOP_POSE_COOLDOWN_MS) {
                    anim->frame             = 2;
                    anim->stop_pose_end_ms  = now_ms + STOP_POSE_DURATION_MS;
                    anim->last_stop_pose_ms = now_ms;
                    needs_redraw            = true;
                }
                /* 쿨다운 중이면 그냥 sprite_2로 정지 */
            } else if (new_dx != 0) {
                /* 정지 → 이동 전환: 전환 모션 즉시 취소 */
                anim->stop_pose_end_ms = 0;
            }

            anim->dx         = new_dx;
            anim->move_timer = 3 + rand() % 5;
        }
        if (anim->dx != 0) anim->flip = (anim->dx > 0);
        anim->last_move_ms = now_ms;
    }

    /* 5. 변화가 있을 때만 그리기 (이전 위치 지우고 새 위치에 그림) */
    if (needs_redraw) {
        clearSprite(anim->draw_x, anim->draw_y);
        anim->draw_x = anim->x;
        anim->draw_y = anim->y;
        if (anim->kind == ANIM_EVOLVE) {
            /* 진화 깜빡임: frame=0 → 스프라이트 표시, frame=1 → 숨김 */
            if (anim->frame == 0)
                drawSprite(get_sprite(ANIM_WALK, 0, level, table_idx), anim->draw_x, anim->draw_y, anim->flip);
            /* frame=1: clearSprite만으로 충분 */
        } else {
            drawSprite(get_sprite(anim->kind, anim->frame, level, table_idx), anim->draw_x, anim->draw_y, anim->flip);
        }
    }
}

/* =========================================================
 * anim_check_random
 * ========================================================= */
void anim_check_random(AnimState *anim, const Digimon *d, ULONGLONG now_ms) {
    /* WALK 상태가 아니면 건너뜀 */
    if (anim->kind != ANIM_WALK) return;

    /* 체크 간격 미달이면 건너뜀 */
    if (now_ms - anim->last_rand_ms < ANIM_RAND_CHECK_MS) return;
    anim->last_rand_ms = now_ms;

    /* 수면 중이면 모든 랜덤 행동 건너뜀 */
    if (d->is_sleep) return;

    /* ── HAPPY: hungry·strength가 높을수록 확률 상승 ── */
    int happy_chance = 0;
    if (MAX_HUNGRY   > 0) happy_chance += (d->hungry   * 15) / MAX_HUNGRY;   // 최대 15%
    if (MAX_STRENGTH > 0) happy_chance += (d->strength * 10) / MAX_STRENGTH; // 최대 10%
    /* 총 최대 25% */

    if (rand() % 100 < happy_chance) {
        anim_play(anim, ANIM_HAPPY, 1500, now_ms);
    }
}
