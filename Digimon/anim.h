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
    ANIM_HAPPY,   // 기쁨
    ANIM_EAT,     // 먹기
    ANIM_SLEEP,   // 수면
    ANIM_INJURY,  // 부상
    ANIM_ATTACK,  // 공격
    ANIM_DEAD,    // 사망
} AnimKind;

/* =========================================================
 * 애니메이션 상태 구조체
 * ========================================================= */
typedef struct {
    AnimKind  kind;               // 현재 애니메이션 종류
    int       frame;              // 현재 프레임 인덱스 (0 또는 1)
    ULONGLONG last_frame_ms;      // 마지막 프레임 교체 시각 (ms)
    ULONGLONG frame_interval_ms;  // 프레임 교체 간격 (ms)
    int       x, y;              // 케이지 내 픽셀 좌표
    int       dx;                 // 이동 방향: -1(좌), 0(정지), +1(우)
    int       move_timer;         // 다음 행동 결정까지 남은 초
    ULONGLONG remaining_ms;       // 현재 상태 지속 시간 (0 = 무한)
    ULONGLONG state_start_ms;     // 현재 상태 진입 시각 (ms)
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
 * @level:  현재 디지몬 레벨 (스프라이트 선택에 사용)
 */
void anim_update(AnimState *anim, ULONGLONG now_ms, Level level);

#endif /* ANIM_H */
