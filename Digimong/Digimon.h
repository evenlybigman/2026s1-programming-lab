#ifndef DIGIMON_H
#define DIGIMON_H

#include <time.h>
#include <stdbool.h>

#define MAX_DIGIMON_SIZE 100
#define MAX_AGE 99
#define MAX_WEIGHT 99
#define MAX_HUNGRY 4
#define MAX_STRENGTH 4
#define MAX_CARE_MISTAKES 20
#define MAX_POOP 8

typedef enum {
    EGG, // 0 - 알
    BABY1, // 1 - 유년기1
    BABY2, // 2 - 유년기2
    ROOKIE, // 3 - 성장기
    CHAMPION, // 4 - 성숙기
    ULTIMATE, // 5 - 완전체
    MEGA // 6 - 궁극체
} Level;

typedef enum {
    VACCINE,  // 백신
    DATA,     // 데이터
    VIRUS,    // 바이러스
    FREE      // 프리
} DigimonType;

typedef struct {
    char name[100]; // 디지몬 이름
    Level level; // 진화 단계
    DigimonType type; // 타입
    // 보이는 정보
    int age; // 나이 (자정 0시 마다 + 1)
    int weight; // 체중 (g) 최대99 (최저 체중 디지몬 마다 다름, 최대 체중은 99) 99g일시 근력 최대시 공격력 최대 +16 효과 무효
    int hungry; // 배고픔 0~4 (먹이 먹으면 + 1) 4에서 더먹여서 만복 시 배 느리게 꺼짐, 진화 조건 배고픔 0 12시간 사망 + 공격력 최대 +16 효과 무효
    int overfeed; // 과식 고기 거부시 +1 이후 다시 과식 시킬려면 배고픔 3이하
    int strength; // 근력 0~4 (훈련하거나 프로틴 먹이면 증가) 1칸당 공격력 + 4
    int poop; // 똥 8개 쌓이면 부상
    int care_mistakes; // 케어미스 2이하 규칙적 3이상 불규칙 20 사망 
    int sleep; // 수면 1시간 수면 파워 10증가 3시간 수면 후 파워 완전 회복 유아기 잠 불가 유년기2는 파워 증가 x
    bool is_sleep;  // 자는동안 허기 체력 감소 x 낮잠은 진화시간 멈춤
    int injuries; // 부상 조건 
    bool is_injuries; // 현재 부상상태 
    int effort; // 노력 훈련 4마다 1증가
    int battles; // 디지몬 배틀 횟수
    int dp; // 없으면 전투 불가 
    int max_dp; // 최대 dp (진화 단계마다 다름)
    //안보이는 정보
    bool is_old;  // 진화 시기 놓친 상태
    time_t injury_time;
}Digimon;

typedef struct DigimonDex{ // 디지몬 도감
    char name[100];
    bool clear; // 1이면 찾음 0이면 못찾음
}DigimonDex;

typedef struct {
    char name[100]; // 테이머 이름
    int battles; // 전적 
    DigimonDex dex[MAX_DIGIMON_SIZE];
}Tamer;

typedef struct GameData {
    Tamer tamer;
    Digimon current;
    time_t last_update; 
}GameData;

typedef struct {
    char name[100];
    Level level;
    int base_power;
    int base_weight;
    DigimonType type;
    int hungry_tick;
    int strength_tick;
    int poop_tick;
    int sleep_hour;   // 취침 시각 (시)
    int wake_hour;    // 기상 시각 (시)
} DigimonInfo;

extern int max_dp_table[];
extern DigimonInfo digimon_table[];

bool check_death(GameData *game);      // 사망 조건 체크 → 1 or 0
void handle_death(GameData *game);     // 사망 처리
bool check_call(GameData *game);       // 콜 체크
void update_status(GameData *game);    // 매 틱 수치 변화
void apply_offline_time(GameData *game); // 껐다 켤 때 경과 시간 반영
void check_evolution(GameData *game);  // 진화 조건 체크
void init_digimon(GameData *game);     // 새 디지몬 초기화

#endif