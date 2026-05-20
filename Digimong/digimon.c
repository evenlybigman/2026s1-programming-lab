#include "digimon.h"

int max_dp_table[] = {0, 0, 0, 14, 28, 42, 56, 70};

DigimonInfo digimon_table[] = {
// name         level     power  weight  type   hungry  strength  poop   sleep  wake
{"깜몬",         BABY1,     0,     10,    FREE,    1800,   1800,    3600,  21,    7},
{"코로몬",       BABY2,     0,     10,    FREE,    1800,   1800,    3600,  21,    7},
{"아구몬",       ROOKIE,    18,    20,    VACCINE, 1800,   1800,    3600,  21,    7},
{"베타몬",       ROOKIE,    10,    20,    VIRUS,   1800,   1800,    3600,  21,    7},
{"그레이몬",     CHAMPION,  75,    35,    VACCINE, 1800,   1800,    3600,  22,    7},
{"데블몬",       CHAMPION,  65,    30,    VIRUS,   1800,   1800,    3600,  22,    7},
{"에어드라몬",   CHAMPION,  55,    25,    VACCINE, 1800,   1800,    3600,  22,    7},
{"티라노몬",     CHAMPION,  70,    35,    DATA,    1800,   1800,    3600,  22,    7},
{"메라몬",       CHAMPION,  60,    30,    DATA,    1800,   1800,    3600,  22,    7},
{"시드라몬",     CHAMPION,  50,    30,    DATA,    1800,   1800,    3600,  22,    7},
{"워매몬",       CHAMPION,  40,    25,    VIRUS,   1800,   1800,    3600,  22,    7},
{"메탈그레이몬",  ULTIMATE,  126,   40,    VIRUS,   1800,   1800,    3600,  23,    7},
{"콩알몬",       ULTIMATE,  118,   20,    DATA,    1800,   1800,    3600,  23,    7},
{"퍼펫몬",       ULTIMATE,  107,   30,    VACCINE, 1800,   1800,    3600,  23,    7},
{"블리츠그레이몬", MEGA,     188,   45,    VIRUS,   1800,   1800,    3600,  23,    7},
{"반초콩알몬",    MEGA,      176,   25,    DATA,    1800,   1800,    3600,  23,    7},
{"오메가몬",      MEGA,      200,   50,    VACCINE, 1800,   1800,    3600,  23,    7},
};

int evolution_time[] = { 
    60, // 알 -> 유년기 (1분)
    600, // 유년기1 -> 유년기2 (10분)
    21600, // 유년기2 -> 성장기 (6시간)
    86400, // 성장기 -> 성숙기 (24시간)
    129600, // 성숙기 -> 완전체 (36시간)
    172800 // 완전체 -> 궁극체 (48시간)
};


void handle_death(GameData *game);     // 사망 처리
bool check_call(GameData *game);       // 콜 체크
void update_status(GameData *game);    // 매 틱 수치 변화
void apply_offline_time(GameData *game); // 껐다 켤 때 경과 시간 반영
void check_evolution(GameData *game);  // 진화 조건 체크
void init_digimon(GameData *game);     // 새 디지몬 초기화
