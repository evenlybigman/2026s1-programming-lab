#include "digimon.h"

int max_dp_table[] = {0, 0, 0, 14, 28, 42, 56, 70};
// EGG BABY1 BABY2 ROOKIE CHAMPION ULTIMATE MEGA

DigimonInfo digimon_table[] = { // power weight 임시
    // name        level      base_power  base_weight  type
    {"깜몬",       BABY1,     4,          10,          FREE},
    {"코로몬",     BABY2,     8,          10,          FREE},
    {"아구몬",     ROOKIE,    30,         20,          VACCINE},
    {"베타몬",     ROOKIE,    25,         20,          DATA},
    {"그레이몬",   CHAMPION,  60,         35,          VACCINE},
    {"데블몬",     CHAMPION,  55,         30,          VIRUS},
    {"에어드라몬", CHAMPION,  50,         25,          VACCINE},
    {"티라노몬",   CHAMPION,  52,         35,          VIRUS},
    {"메라몬",     CHAMPION,  58,         30,          VIRUS},
    {"시드라몬",   CHAMPION,  56,         30,          DATA},
    {"워매몬",     CHAMPION,  48,         25,          FREE},
    {"메탈그레이몬", ULTIMATE, 100,       40,          VACCINE},
    {"콩알몬",     ULTIMATE,  85,         20,          FREE},
    {"퍼펫몬",     ULTIMATE,  90,         30,          VIRUS},
    {"블리츠그레이몬", MEGA,  150,        45,          VACCINE},
    {"반초콩알몬", MEGA,      130,        25,          FREE},
    {"오메가몬",   MEGA,      200,        50,          VACCINE},
};

int evolution_time[] = { 
    60, // 알 -> 유년기 (1분)
    600, // 유년기1 -> 유년기2 (10분)
    21600, // 유년기2 -> 성장기 (6시간)
    86400, // 성장기 -> 성숙기 (24시간)
    129600, // 성숙기 -> 완전체 (36시간)
    172800 // 완전체 -> 궁극체 (48시간)
};

void update_status(GameData *game) {
    
}