#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include "anim.h"
#include "digimon.h"
#include "sprites.h"
#include "ui.h"

extern char Alpha[26]; // 대문자 알파벳 A~Z

/**
 * newGame - 게임 시작 처리.
 *   is_digimon == false 인 경우 테이머 이름 입력 → selectEgg 순으로 진행.
 *   is_digimon == true 인 경우 저장 데이터 로드 (추후 구현).
 */
void newGame(GameData* game);

/**
 * selectEgg - 알 선택 및 디지몬 초기화.
 *   2단계: 알 선택 (현재 1종류, C로 확정 / 추후 Z·X로 선택 확장 예정)
 *   3단계: is_digimon = true 설정 후 init_digimon 호출
 */
void selectEgg(GameData* game);

/**
 * hatch - 매 프레임 호출하여 부화 애니메이션과 진화 확정을 처리한다.
 * @game              : 게임 상태
 * @anim              : 현재 애니메이션 상태
 * @hatch_anim_started: 애니메이션 시작 여부 플래그 (main 에서 관리)
 * @now_ms            : 현재 시각 (GetTickCount64)
 *
 * game->hatch_target_idx < 0 이면 즉시 반환한다.
 * 애니메이션 완료 후 evolve_to() 를 호출하고 anim_init() 으로 상태를 재초기화한다.
 */
void hatch(GameData* game, AnimState* anim, bool* hatch_anim_started, ULONGLONG now_ms);

/**
 * battle_event - 배틀 이벤트 처리.
 *   전제조건 미충족(루키 미만, DP 없음, 수면 중) 시 즉시 반환.
 *   CPU 상대를 생성하고 배틀을 실행한 뒤 결과를 표시한다.
 */
void battle_event(GameData* game, AnimState* anim);

#endif /* EVENT_H */
