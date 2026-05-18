#ifndef DIGIMON_H
#define DIGIMON_H

#define MAX_DIGIMON_SIZE 100
#define SPRITE_W 32
#define SPRITE_H 16

#include <time.h>

typedef enum {
    EGG, // 0 - 알
    BABY1, // 1 - 유년기1
    BABY2, // 2 - 유년기2
    ROOKIE, // 3 - 성장기
    CHAMPION, // 4 - 성숙기
    ULTIMATE, // 5 - 완전체
    MEGA // 6 - 궁극체
} Level;

int evolution_time[] = { 
    60, // 알 -> 유년기 (1분)
    600, // 유년기1 -> 유년기2 (10분)
    21600, // 유년기2 -> 성장기 (6시간)
    86400, // 성장기 -> 성숙기 (24시간)
    129600, // 성숙기 -> 완전체 (36시간)
    172800 // 완전체 -> 궁극체 (48시간)
};

typedef struct {
    char name[10]; // 디지몬 이름
    Level level; // 진화 단계
    int age; // 나이 (자정 0시 마다 + 1)
    int weight; // 체중 (g) (최저 체중 디지몬 마다 다름, 최대 체중은 99) 99g일시 근력 최대시 공격력 최대 +16 효과 무효
    int hungry; // 배고픔 0~4 (먹이 먹으면 + 1) 4에서 더먹여서 만복 시 배 느리게 꺼짐, 진화 조건 배고픔 0 12시간 사망 + 공격력 최대 +16 효과 무효
    int strength; // 근력 0~4 (훈련하거나 프로틴 먹이면 증가) 1칸당 공격력 + 4
    int happy; // 
    int fatigue;
    int effort;
    int battles;
    int type;
    int dp;
    int atk;
}Digimon;

typedef struct {
    char name[10];
    int battles;

}Tamer;

#endif