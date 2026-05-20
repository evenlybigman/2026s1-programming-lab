#ifndef DIGIMON_H
#define DIGIMON_H

#include <time.h>
#include <stdbool.h>

/* =========================================================
 * 상수 정의
 * ========================================================= */

#define MAX_DIGIMON_TABLE_SIZE  100   // digimon_table 최대 항목 수
#define MAX_DEX_SIZE            100   // 도감 최대 항목 수
#define MAX_NAME_LEN            100   // 이름 버퍼 크기

#define MAX_AGE                 99
#define MAX_WEIGHT              99
#define MAX_HUNGRY              4
#define MAX_STRENGTH            4
#define MAX_CARE_MISTAKES       20
#define MAX_POOP                8

/* 시간 단위 (초) */
#define MINUTE                  60
#define HOUR                    3600
#define DAY                     86400

/* 게임 수치 상수 */
#define POWER_PER_STRENGTH      4    // 근력 1칸당 공격력 증가량
#define POWER_BONUS_MAX         16   // 최적 체중+근력 최대 시 공격력 보너스
#define HUNGRY_DEATH_TIMEOUT    (12 * HOUR)  // 배고픔 0 지속 시 사망까지 걸리는 시간
#define INJURY_DEATH_TIMEOUT    (3 * HOUR)   // 부상 미치료 시 사망까지 걸리는 시간
#define SLEEP_POWER_PER_HOUR    10           // 수면 1시간당 DP 회복량
#define SLEEP_FULL_RECOVERY     (3 * HOUR)   // 완전 회복에 필요한 수면 시간

/* =========================================================
 * 열거형
 * ========================================================= */

/**
 * 디지몬 진화 단계.
 * evolution_time[] 인덱스와 1:1 대응 (EGG=0 ~ ULTIMATE=5 까지 진화 시간 존재).
 */
typedef enum {
    EGG      = 0, // 알
    BABY1    = 1, // 유년기1
    BABY2    = 2, // 유년기2
    ROOKIE   = 3, // 성장기
    CHAMPION = 4, // 성숙기
    ULTIMATE = 5, // 완전체
    MEGA     = 6  // 궁극체 (최종 단계, 진화 없음)
} Level;

/**
 * 디지몬 속성.
 * FREE는 유년기 전용으로 타입 미결정 상태를 의미한다.
 */
typedef enum {
    VACCINE = 0, // 백신
    DATA    = 1, // 데이터
    VIRUS   = 2, // 바이러스
    FREE    = 3  // 프리 (유년기 전용)
} DigimonType;

/* =========================================================
 * 구조체
 * ========================================================= */

/**
 * 현재 활성 디지몬의 모든 상태를 담는 구조체.
 *
 * [표시 항목]
 *   age, weight, hungry, strength, poop, care_mistakes, dp, max_dp
 *
 * [숨김 항목]
 *   overfeed, sleep, is_sleep, injuries, is_injuries,
 *   effort, battles, is_old, 각종 time_t 타임스탬프
 */
typedef struct {
    char        name[MAX_NAME_LEN]; // 디지몬 이름
    int         table_idx;          // digimon_table 인덱스
    Level       level;              // 진화 단계
    DigimonType type;               // 속성

    /* --- 표시 정보 --- */
    int  age;           // 나이 (자정마다 +1, 최대 MAX_AGE)
    int  weight;        // 체중(g), 범위: base_weight ~ MAX_WEIGHT
                        //   99g이면서 근력 최대 시 공격력 보너스 +16 효과 무효
    int  hungry;        // 배고픔 0~MAX_HUNGRY (먹이 +1)
                        //   0 상태 HUNGRY_DEATH_TIMEOUT 초 지속 시 사망
                        //   MAX_HUNGRY 초과(만복) 시 배고픔 감소 속도 저하
    int  overfeed;      // 누적 과식 횟수 (진화 조건에 사용)
    int  strength;      // 근력 0~MAX_STRENGTH (훈련/프로틴 +1, 1칸 = 공격력 +POWER_PER_STRENGTH)
    int  poop;          // 똥 개수 (MAX_POOP 도달 시 부상 유발)
    int  care_mistakes; // 케어미스 횟수 (2 이하=규칙적, 3 이상=불규칙, MAX_CARE_MISTAKES=사망)
    int  dp;            // 현재 DP (0이면 전투 불가)
    int  max_dp;        // 최대 DP (진화 단계별 상이)

    /* --- 숨김 정보 --- */
    int  sleep;         // 누적 수면 시간(초), SLEEP_FULL_RECOVERY 도달 시 DP 완전 회복
    bool is_sleep;      // 수면 중 여부 (수면 중 배고픔/체력 감소 없음, 낮잠은 진화 타이머 정지)
    bool is_overfed;    // 현재 과식 상태 (hungry <= 3 되면 해제, 해제 전까지 배고픔 감소 속도 저하)
    int  injuries;      // 누적 부상 횟수
    bool is_injuries;   // 현재 부상 상태 여부
    int  effort;        // 노력치 (훈련 4회마다 +1)
    int  battles;       // 배틀 누적 횟수
    bool is_old;        // 진화 시기 놓침 여부

    time_t injury_time;         // 가장 최근 부상 발생 시각
    time_t hungry_zero_time;    // 배고픔이 0이 된 시각
    time_t strength_zero_time;  // 근력이 0이 된 시각
    time_t last_hungry_tick;    // 마지막 배고픔 감소 시각
    time_t last_strength_tick;  // 마지막 근력 감소 시각
    time_t last_midnight;       // 마지막으로 나이가 증가한 자정 시각
} Digimon;

/**
 * 도감 항목 하나.
 * digimon_table 인덱스와 대응하도록 관리한다.
 */
typedef struct {
    char name[MAX_NAME_LEN]; // 디지몬 이름 (digimon_table과 동일)
    bool found;              // true=도감 등록 완료, false=미등록
} DigimonDex;

/**
 * 테이머(플레이어) 정보.
 */
typedef struct {
    char       name[MAX_NAME_LEN];    // 테이머 이름
    int        battles;               // 총 배틀 전적
    DigimonDex dex[MAX_DEX_SIZE];     // 디지몬 도감
} Tamer;

/**
 * 게임 전체 상태. 저장/불러오기 단위.
 */
typedef struct {
    Tamer  tamer;
    Digimon current;
    time_t  last_update; // 마지막으로 update_status()가 호출된 시각
} GameData;

/**
 * 디지몬 기본 스탯 테이블 항목 (digimon_table에 저장).
 * 현재 활성 디지몬(Digimon)과 달리 고정된 원본 데이터만 보관한다.
 */
typedef struct {
    char        name[MAX_NAME_LEN]; // 디지몬 이름
    Level       level;              // 진화 단계
    int         base_power;         // 기본 공격력
    int         base_weight;        // 기본(최저) 체중(g)
    DigimonType type;               // 속성
    int         hungry_tick;        // 배고픔 1 감소 + 똥 1 증가 주기(초)
    int         strength_tick;      // 근력 1 감소 주기(초)
    int         sleep_hour;         // 취침 시각 (0~23)
    int         wake_hour;          // 기상 시각 (0~23)
} DigimonInfo;

/* =========================================================
 * 외부 공개 데이터
 * ========================================================= */

extern int        max_dp_table[];   // 진화 단계별 최대 DP (인덱스 = Level)
extern DigimonInfo digimon_table[]; // 디지몬 기본 스탯 테이블
extern int        evolution_time[]; // 각 단계 진화 소요 시간(초) (인덱스 = 현재 Level)

/* =========================================================
 * 함수 선언
 * ========================================================= */

/**
 * check_death - 사망 조건을 검사한다.
 * @game: 게임 상태 포인터
 * @return: 사망 조건 충족 시 true
 *
 * 사망 조건:
 *   - 배고픔 0 상태로 HUNGRY_DEATH_TIMEOUT 초 경과
 *   - 케어미스 MAX_CARE_MISTAKES 도달
 */
bool check_death(GameData *game);

/**
 * handle_death - 사망 처리를 수행한다.
 * @game: 게임 상태 포인터
 *
 * 현재 디지몬을 초기화하고 사망 연출을 출력한다.
 * check_death()가 true를 반환한 직후 호출해야 한다.
 */
void handle_death(GameData *game);

/**
 * check_call - 콜(알림) 발생 여부를 검사한다.
 * @game: 게임 상태 포인터
 * @return: 콜 발생 시 true
 *
 * 콜 조건: 배고픔 1 이하, 근력 1 이하, 부상, 수면 시간 등
 */
bool check_call(GameData *game);

/**
 * update_status - 매 틱(1초)마다 디지몬 수치를 갱신한다.
 * @game: 게임 상태 포인터
 *
 * last_update 기준으로 경과 시간을 계산해 배고픔, 근력, 똥 등을 감소/증가시킨다.
 * 함수 종료 시 last_update를 현재 시각으로 갱신한다.
 */
void update_status(GameData *game);

/**
 * apply_offline_time - 프로그램 종료 후 재시작 시 경과 시간을 일괄 반영한다.
 * @game: 게임 상태 포인터
 *
 * last_update와 현재 시각의 차이만큼 update_status()를 압축 적용한다.
 */
void apply_offline_time(GameData *game);

/**
 * check_evolution - 진화 조건을 검사하고 조건 충족 시 진화시킨다.
 * @game: 게임 상태 포인터
 *
 * evolution_time[]에 따른 시간 조건과 care_mistakes, effort 등의
 * 수치 조건을 모두 확인한다.
 */
void check_evolution(GameData *game);

/**
 * game_tick - 1초마다 호출되는 게임 로직 통합 업데이트.
 * @game: 게임 상태 포인터
 *
 * update_status → check_evolution → check_death 순서로 실행한다.
 * @return: 사망이 발생하면 true
 */
bool game_tick(GameData *game);

/**
 * init_digimon - 새 디지몬을 EGG 상태로 초기화한다.
 * @game: 게임 상태 포인터
 *
 * Digimon 구조체의 모든 필드를 초기값으로 설정하고
 * 시간 관련 필드는 time(NULL)로 초기화한다.
 */
void init_digimon(GameData *game);

#endif /* DIGIMON_H */
