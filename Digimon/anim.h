#ifndef ANIM_H
#define ANIM_H

#include <windows.h>
#include "sprites.h"
#include "digimon.h"

/* =========================================================
 * 애니메이션 상태 열거형
 *   우선순위: 값이 클수록 높음 (DEAD > INJURY > ... > EGG)
 *   anim_play()에서 현재 kind보다 낮은 값은 무시된다.
 * ========================================================= */
typedef enum {
    ANIM_EGG,     // 알 (프레임 1↔2, 이동 없음)
    ANIM_WALK,    // 기본 이동
    ANIM_HATCH,   // 부화 모션 (EGG 전용)
    ANIM_HAPPY,   // 기쁨
    ANIM_EAT,     // 먹기
    ANIM_SLEEP,   // 수면
    ANIM_INJURY,  // 부상
    ANIM_ATTACK,  // 공격
    ANIM_EVOLVE,  // 진화 깜빡임 (비-EGG 전용, 완료 후 main.c가 evolve_to 호출)
    ANIM_DEAD,    // 사망
} AnimKind;

/* 이동 간격: 이 ms마다 한 칸 이동 */
#define ANIM_MOVE_INTERVAL_MS   200

/* WALK 이동 중 sprite_1 등장 확률 (%) — 프레임 틱(400ms)마다 판정 */
#define WALK_SPRITE1_CHANCE     5

/* 랜덤 행동 체크 간격 (ms): HAPPY·멈춤 특수 포즈 등 */
#define ANIM_RAND_CHECK_MS      5000

/* 멈춤 전환 모션 (sprite_3) 유지 시간 (ms) */
#define STOP_POSE_DURATION_MS   500

/* 멈춤 전환 모션 재발생 최소 간격 (ms): 이 시간이 지난 뒤 정지할 때만 트리거 */
#define STOP_POSE_COOLDOWN_MS   8000

/* =========================================================
 * 애니메이션 상태 구조체
 * ========================================================= */
typedef struct {
    AnimKind  kind;               // 현재 애니메이션 종류
    int       frame;              // 현재 프레임 인덱스 (ANIM_HATCH는 0/1/2까지 사용)
    ULONGLONG last_frame_ms;      // 마지막 프레임 교체 시각 (ms)
    ULONGLONG frame_interval_ms;  // 프레임 교체 간격 (ms)
    int       x, y;               // 논리 좌표 (이동 계산 기준)
    int       draw_x, draw_y;     // 실제로 그린 좌표 (clearSprite 기준)
    int       dx;                 // 이동 방향: -1(좌), 0(정지), +1(우)
    bool      flip;               // 좌우 반전 여부 (dx < 0 이면 true)
    int       move_timer;         // 방향 전환까지 남은 스텝 수
    ULONGLONG last_move_ms;       // 마지막 이동 시각 (ms)
    ULONGLONG remaining_ms;       // 현재 상태 지속 시간 (0 = 무한)
    ULONGLONG state_start_ms;     // 현재 상태 진입 시각 (ms)
    ULONGLONG last_rand_ms;       // 마지막 랜덤 행동 체크 시각 (ms)
    ULONGLONG stop_pose_end_ms;   // 멈춤 전환 모션(sprite_3) 종료 시각 (0 = 비활성)
    ULONGLONG last_stop_pose_ms;  // 마지막 멈춤 전환 모션 시작 시각 (쿨다운 기준)
} AnimState;

/* =========================================================
 * 함수 선언
 * ========================================================= */

/**
 * anim_init - AnimState를 초기값으로 설정한다.
 * @anim:   애니메이션 상태 포인터
 * @level:  현재 디지몬 레벨 (EGG이면 ANIM_EGG, 나머지면 ANIM_WALK로 시작)
 * @now_ms: 현재 시각 (GetTickCount64)
 */
void anim_init(AnimState *anim, Level level, ULONGLONG now_ms);

/**
 * anim_play - 애니메이션 상태를 전환한다.
 * @anim:        애니메이션 상태 포인터
 * @kind:        전환할 상태 (현재 kind보다 우선순위가 낮으면 무시)
 * @duration_ms: 지속 시간(ms), 0이면 무한
 * @now_ms:      현재 시각 (GetTickCount64)
 */
void anim_play(AnimState *anim, AnimKind kind, ULONGLONG duration_ms, ULONGLONG now_ms);

/**
 * anim_update - 매 루프마다 호출해 프레임/이동/그리기를 갱신한다.
 * @anim:   애니메이션 상태 포인터
 * @now_ms: 현재 시각 (GetTickCount64)
 * @d:      현재 디지몬 상태 (level·table_idx로 스프라이트 선택)
 */
void anim_update(AnimState *anim, ULONGLONG now_ms, const Digimon *d);

/**
 * anim_check_random - 게임 상태에 따라 랜덤 행동을 발생시킨다.
 *   ANIM_RAND_CHECK_MS마다 한 번 체크한다.
 *   hungry·strength 높을수록 ANIM_HAPPY 확률 증가.
 *   수면 중(is_sleep)이면 모든 랜덤 행동을 건너뛴다.
 * @anim:   애니메이션 상태 포인터
 * @d:      현재 디지몬 상태
 * @now_ms: 현재 시각 (GetTickCount64)
 */
void anim_check_random(AnimState *anim, const Digimon *d, ULONGLONG now_ms);

#endif /* ANIM_H */
