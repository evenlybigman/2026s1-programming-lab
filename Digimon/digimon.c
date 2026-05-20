#include "digimon.h"
#include <string.h>

/* =========================================================
 * max_dp_table
 *   인덱스 = Level 열거값 (EGG=0 ~ MEGA=6)
 *   EGG/BABY1/BABY2 는 전투 불가이므로 0
 * ========================================================= */
int max_dp_table[] = {
    0,   // EGG      (알)
    0,   // BABY1    (유년기1)
    0,   // BABY2    (유년기2)
    14,  // ROOKIE   (성장기)
    28,  // CHAMPION (성숙기)
    42,  // ULTIMATE (완전체)
    56,  // MEGA     (궁극체) — 원본 70에서 56으로 수정 (인덱스 6)
    70   // (예비 슬롯, 조그레스 구현시 사용)
};

/* =========================================================
 * digimon_table
 *   새 디지몬 추가 시 이 테이블 끝에 행 하나만 추가하면 된다.
 *   열 순서: name, level, base_power, base_weight, type,
 *            hungry_tick, strength_tick,
 *            sleep_hour, wake_hour
 * ========================================================= */
DigimonInfo digimon_table[] = { //데이터 임시 수정 필요
    // ── 디지에그 ────────────────────────────────────
    // name          level     power  weight  type    hungry  strength  sleep  wake
    {"디지에그1",     EGG,      0,      0,     FREE,      0,      0,      0,    0},
    {"깜몬",          BABY1,    0,     10,     FREE,   1800,   1800,    21,    7},
    {"코로몬",        BABY2,    0,     10,     FREE,   1800,   1800,    21,    7},
    // ── 성장기 ────────────────────────────────────
    {"아구몬",        ROOKIE,   18,    20,     VACCINE,1800,   1800,    21,    7},
    {"베타몬",        ROOKIE,   10,    20,     VIRUS,  1800,   1800,    21,    7},
    // ── 성숙기 ────────────────────────────────────
    {"그레이몬",      CHAMPION, 75,    35,     VACCINE,1800,   1800,    22,    7},
    {"데블몬",        CHAMPION, 65,    30,     VIRUS,  1800,   1800,    22,    7},
    {"에어드라몬",    CHAMPION, 55,    25,     VACCINE,1800,   1800,    22,    7},
    {"티라노몬",      CHAMPION, 70,    35,     DATA,   1800,   1800,    22,    7},
    {"메라몬",        CHAMPION, 60,    30,     DATA,   1800,   1800,    22,    7},
    {"시드라몬",      CHAMPION, 50,    30,     DATA,   1800,   1800,    22,    7},
    {"워매몬",        CHAMPION, 40,    25,     VIRUS,  1800,   1800,    22,    7},
    // ── 완전체 ────────────────────────────────────
    {"메탈그레이몬",  ULTIMATE, 126,   40,     VIRUS,  1800,   1800,    23,    7},
    {"콩알몬",        ULTIMATE, 118,   20,     DATA,   1800,   1800,    23,    7},
    {"퍼펫몬",        ULTIMATE, 107,   30,     VACCINE,1800,   1800,    23,    7},
    // ── 궁극체 ────────────────────────────────────
    {"블리츠그레이몬",MEGA,     188,   45,     VIRUS,  1800,   1800,    23,    7},
    {"반초콩알몬",    MEGA,     176,   25,     DATA,   1800,   1800,    23,    7},
    {"오메가몬",      MEGA,     200,   50,     VACCINE,1800,   1800,    23,    7},
};

/* =========================================================
 * evolution_time
 *   인덱스 = 현재 Level (EGG=0 ~ ULTIMATE=5)
 *   MEGA(6)는 최종 단계이므로 항목 없음
 * ========================================================= */
int evolution_time[] = {
    1  * MINUTE,        // EGG      -> BABY1    (1분)
    10 * MINUTE,        // BABY1    -> BABY2    (10분)
    6  * HOUR,          // BABY2    -> ROOKIE   (6시간)
    24 * HOUR,          // ROOKIE   -> CHAMPION (24시간)
    36 * HOUR,          // CHAMPION -> ULTIMATE (36시간)
    48 * HOUR           // ULTIMATE -> MEGA     (48시간)
};

bool check_death(GameData *game) {
    Digimon *d   = &game->current;
    time_t   now = time(NULL);

    if (d->hungry == 0 && now - d->hungry_zero_time >= HUNGRY_DEATH_TIMEOUT)
        return true;
    if (d->care_mistakes >= MAX_CARE_MISTAKES)
        return true;
    if (d->is_injuries && now - d->injury_time >= INJURY_DEATH_TIMEOUT)
        return true;
    return false;
}

void handle_death(GameData *game) {
    init_digimon(game);
}

/* =========================================================
 * EvoRule - 진화 규칙 한 항목
 *   evo_rules[]를 조건이 까다로운 순(높은 품질 → 낮은 품질)으로
 *   정렬해두면 check_evolution()이 위에서부터 순회하며
 *   조건을 충족하는 첫 번째 규칙을 적용한다.
 *   폴백(마지막 항목)은 max_mistakes=99, min_effort=0, max_overfeed=99
 *   로 설정해 항상 통과하도록 한다.
 * ========================================================= */
typedef struct {
    int from_idx;      // 진화 전 digimon_table 인덱스 (DigimonIdx 사용)
    int to_idx;        // 진화 후 digimon_table 인덱스
    int max_mistakes;  // 허용 케어미스 최대치
    int min_effort;    // 최소 노력치
    int max_overfeed;  // 허용 과식 횟수 최대치
} EvoRule;

static const EvoRule evo_rules[] = { //정확한 구현 필요
    //  from                  to                   care  effort  feed
    /* EGG → BABY1 (자동) */
    {  IDX_EGG1,             IDX_BOTAMON,           99,    0,    99 },
    /* BABY1 → BABY2 (자동) */
    {  IDX_BOTAMON,          IDX_KOROMON,           99,    0,    99 },
    /* BABY2 → ROOKIE */
    {  IDX_KOROMON,          IDX_AGUMON,             3,    2,     3 },
    {  IDX_KOROMON,          IDX_BETAMON,           99,    0,    99 }, // 폴백
    /* ROOKIE → CHAMPION */
    {  IDX_AGUMON,           IDX_GREYMON,            3,    5,     3 },
    {  IDX_AGUMON,           IDX_AIRDRAMON,          5,    3,     5 },
    {  IDX_AGUMON,           IDX_TYRANNOMON,        99,    0,    99 }, // 폴백
    {  IDX_BETAMON,          IDX_DEVIMON,            3,    5,     3 },
    {  IDX_BETAMON,          IDX_SEADRAMON,          5,    3,     5 },
    {  IDX_BETAMON,          IDX_WORMMON,           99,    0,    99 }, // 폴백
    /* CHAMPION → ULTIMATE */
    {  IDX_GREYMON,          IDX_METALGREYMON,       3,    8,     3 },
    {  IDX_GREYMON,          IDX_PUPPETMON,         99,    0,    99 }, // 폴백
    {  IDX_DEVIMON,          IDX_METALGREYMON,       3,    8,     3 },
    {  IDX_DEVIMON,          IDX_SUKAMON,           99,    0,    99 }, // 폴백
    {  IDX_AIRDRAMON,        IDX_SUKAMON,            3,    8,     3 },
    {  IDX_AIRDRAMON,        IDX_PUPPETMON,         99,    0,    99 }, // 폴백
    {  IDX_TYRANNOMON,       IDX_SUKAMON,            3,    8,     3 },
    {  IDX_TYRANNOMON,       IDX_PUPPETMON,         99,    0,    99 }, // 폴백
    {  IDX_MERAMON,          IDX_SUKAMON,            3,    8,     3 },
    {  IDX_MERAMON,          IDX_PUPPETMON,         99,    0,    99 }, // 폴백
    {  IDX_SEADRAMON,        IDX_SUKAMON,            3,    8,     3 },
    {  IDX_SEADRAMON,        IDX_PUPPETMON,         99,    0,    99 }, // 폴백
    {  IDX_WORMMON,          IDX_SUKAMON,            3,    8,     3 },
    {  IDX_WORMMON,          IDX_PUPPETMON,         99,    0,    99 }, // 폴백
    /* ULTIMATE → MEGA */
    {  IDX_METALGREYMON,     IDX_OMEGAMON,           1,   12,     1 },
    {  IDX_METALGREYMON,     IDX_BLITZGREYMON,       3,    8,     3 },
    {  IDX_METALGREYMON,     IDX_BANCHOUKOMON,      99,    0,    99 }, // 폴백
    {  IDX_SUKAMON,          IDX_BANCHOUKOMON,       3,    8,     3 },
    {  IDX_SUKAMON,          IDX_OMEGAMON,          99,    0,    99 }, // 폴백
    {  IDX_PUPPETMON,        IDX_BLITZGREYMON,       3,    8,     3 },
    {  IDX_PUPPETMON,        IDX_OMEGAMON,          99,    0,    99 }, // 폴백
};

#define EVO_RULES_SIZE ((int)(sizeof(evo_rules) / sizeof(evo_rules[0])))

/* =========================================================
 * do_evolve - 진화 공통 처리
 *   레벨/테이블/이름/타입/체중/DP/수치를 일괄 갱신한다.
 *   check_evolution()에서만 호출한다.
 * ========================================================= */
static void do_evolve(GameData *game, int next_idx) {
    Digimon *d      = &game->current;
    d->level        = digimon_table[next_idx].level;
    d->table_idx    = next_idx;
    strncpy_s(d->name, MAX_NAME_LEN, digimon_table[next_idx].name, MAX_NAME_LEN - 1);
    d->type         = digimon_table[next_idx].type;
    d->weight       = digimon_table[next_idx].base_weight;
    d->max_dp       = max_dp_table[d->level];
    d->hungry       = MAX_HUNGRY;
    d->strength     = MAX_STRENGTH;
    d->level_start_time = time(NULL);
}

void evolve_to(GameData *game, int next_idx) {
    do_evolve(game, next_idx);
}

void check_evolution(GameData *game) {
    Digimon *d   = &game->current;
    time_t   now = time(NULL);

    if (d->level >= MEGA)            return;
    if (game->hatch_target_idx >= 0) return; // 부화 애니메이션 대기 중
    if (d->is_sleep && d->level >= BABY2) return;
    if (now - d->level_start_time < evolution_time[d->level]) return;

    /* 규칙 테이블 순회: 조건을 충족하는 첫 번째 규칙 적용 */
    for (int i = 0; i < EVO_RULES_SIZE; i++) {
        const EvoRule *r = &evo_rules[i];
        if (r->from_idx      != d->table_idx)   continue;
        if (d->care_mistakes  > r->max_mistakes) continue;
        if (d->effort         < r->min_effort)   continue;
        if (d->overfeed       > r->max_overfeed) continue;

        if (d->level == EGG) {
            /* EGG: 부화 애니메이션 예약, 실제 진화는 애니메이션 완료 후 */
            game->hatch_target_idx = r->to_idx;
        } else {
            do_evolve(game, r->to_idx);
        }
        return;
    }
}

bool check_call(GameData *game) {
    Digimon *d   = &game->current;
    time_t   now = time(NULL);

    bool need_call = d->hungry    <= 1
                  || d->strength  <= 1
                  || d->is_injuries;

    if (need_call && !game->is_call) {
        game->is_call  = true;
        game->call_time = now;
    } else if (!need_call && game->is_call) {
        game->is_call = false;
    }

    /* 콜 발생 후 10분 무응답 → 케어미스 */
    if (game->is_call && now - game->call_time >= 10 * MINUTE) {
        d->care_mistakes++;
        game->call_time = now;  // 타이머 리셋 (중복 방지)
    }

    return game->is_call;
}

bool game_tick(GameData *game) {
    update_status(game);
    check_call(game);
    check_evolution(game);
    if (check_death(game)) {
        handle_death(game);
        return true;
    }
    return false;
}

void init_digimon(GameData* game) {
    Digimon* d = &game->current;

    // digimon_table[0] = 디지에그
    d->table_idx = 0;
    strncpy_s(d->name, MAX_NAME_LEN, digimon_table[0].name, MAX_NAME_LEN - 1);
    d->level = EGG;
    d->type = FREE;

    // 수치 초기화
    d->age = 0;
    d->weight = 0;
    d->hungry = MAX_HUNGRY;
    d->strength = MAX_STRENGTH;
    d->poop = 0;
    d->care_mistakes = 0;
    d->dp = 0;
    d->max_dp = max_dp_table[EGG];
    d->overfeed = 0;
    d->effort = 0;
    d->battles = 0;
    d->injuries = 0;
    d->sleep = 0;

    // 플래그
    d->is_sleep    = false;
    d->is_overfed  = false;
    d->is_injuries = false;
    d->is_old      = false;

    // 시간
    time_t now = time(NULL);
    d->overfeed_time      = 0;
    d->hungry_zero_time   = now;
    d->strength_zero_time = now;
    d->injury_time        = now;
    d->last_hungry_tick   = now;
    d->last_strength_tick = now;
    game->last_update      = now;
    game->is_call          = false;
    game->call_time        = now;
    game->hatch_target_idx = -1;

    d->level_start_time = now;

    struct tm t = *localtime(&now);
    t.tm_hour = t.tm_min = t.tm_sec = 0;
    d->last_midnight = mktime(&t);
}

void update_status(GameData* game) {
    Digimon*     d    = &game->current;
    DigimonInfo* info = &digimon_table[d->table_idx];
    time_t       now  = time(NULL);

    /* 나이 증가: 자정 통과 시 */
    struct tm t = *localtime(&now);
    t.tm_hour = t.tm_min = t.tm_sec = 0;
    time_t today_midnight = mktime(&t);
    if (today_midnight > d->last_midnight) {
        if (d->age < MAX_AGE) d->age++;
        d->last_midnight = today_midnight;
    }

    /* 수면 중에는 배고픔/근력/똥 변화 없음 */
    if (!d->is_sleep) {
        /* 배고픔 감소 + 똥 증가 */
        /* 과식 후 1시간은 배고픔 완전 차단, 이후 일반 감소 재개 */
        bool overfed_block = d->is_overfed &&
                             (now - d->overfeed_time < HOUR);

        if (overfed_block) {
            /* 차단 중: 타이머를 계속 갱신해 해제 직후 즉시 감소 방지 */
            d->last_hungry_tick = now;
        } else if (now - d->last_hungry_tick >= info->hungry_tick) {
            if (d->hungry > 0) {
                d->hungry--;
                if (d->hungry == 0)
                    d->hungry_zero_time = now;
            }
            if (d->poop < MAX_POOP) d->poop++;
            d->last_hungry_tick = now;
        }

        /* 과식 상태 해제: hungry <= 3이 되면 재과식 허용 */
        if (d->is_overfed && d->hungry <= 3)
            d->is_overfed = false;

        /* 근력 감소 */
        if (now - d->last_strength_tick >= info->strength_tick) {
            if (d->strength > 0) d->strength--;
            d->last_strength_tick = now;
        }
    }

    /* 부상 발생: 똥 4개 이상, 신규 부상만 트리거 */
    if (d->poop >= 4 && !d->is_injuries) {
        d->is_injuries = true;
        d->injury_time = now;
    }

    /* 수면 중 DP 회복: 1시간마다 */
    if (d->is_sleep) {
        d->sleep++;
        if (d->sleep % HOUR == 0) {
            d->dp += SLEEP_POWER_PER_HOUR;
            if (d->dp > d->max_dp) d->dp = d->max_dp;
        }
    }
}
