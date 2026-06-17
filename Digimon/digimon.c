#include "digimon.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    {"디지에그",     EGG,      0,      0,     FREE,      0,      0,      0,    0},
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
const char *level_names[] = {
    "알", "유년기I", "유년기II", "성장기", "성숙기", "완전체", "궁극체"
};

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

    //배고픔 0 지속
    if (d->hungry == 0 && now - d->hungry_zero_time >= HUNGRY_DEATH_TIMEOUT)
        return true;
    //케어미스 20 누적
    if (d->care_mistakes >= MAX_CARE_MISTAKES)
        return true;
    //부상 방치
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
    int max_mistakes;  // 케어미스 허용 최대치  (d->care_mistakes <= max_mistakes)
    int min_effort;    // 노력치 필요 최소치    (d->effort        >= min_effort)
    int max_overfeed;  // 과식 허용 최대치      (d->overfeed      <= max_overfeed)
    int min_overfeed;  // 과식 필요 최소치      (d->overfeed      >= min_overfeed, 0=조건없음)
    int min_wins;      // 배틀 승리 필요 최소치 (d->wins          >= min_wins,     0=조건없음)
                       //   ※ 배틀 시스템 구현 후 실제 값 설정 예정
} EvoRule;

/* =========================================================
 * evo_rules  —  진화 규칙 테이블 (디지털몬스터 Ver.20th V1 기준)
 *
 * 검사 순서: 위에서부터 순회해 첫 번째로 통과한 규칙 적용.
 * 조건: care_mistakes <= max_mistakes
 *       effort        >= min_effort
 *       overfeed      <= max_overfeed
 *
 * effort 스케일: train_count / 4
 *   (원작 훈련 횟수 → 대략 4배 압축: 32회 ≈ effort 8, 48회 ≈ effort 12)
 * ========================================================= */
static const EvoRule evo_rules[] = {
    //  from                  to                  care  effort  ovfeed_max  ovfeed_min  wins

    /* ── EGG → BABY1 (자동, 조건 없음) ── */
    {  IDX_EGG1,              IDX_BOTAMON,         99,   0,    99,   0,   0 },

    /* ── BABY1 → BABY2 (자동, 조건 없음) ── */
    {  IDX_BOTAMON,           IDX_KOROMON,         99,   0,    99,   0,   0 },

    /* ── BABY2 → ROOKIE ──────────────────────────────────────
     *   케어미스 0~3 → 아구몬 / 4+ → 베타몬                  */
    {  IDX_KOROMON,           IDX_AGUMON,           3,   0,    99,   0,   0 },
    {  IDX_KOROMON,           IDX_BETAMON,         99,   0,    99,   0,   0 }, // 폴백

    /* ── ROOKIE → CHAMPION  (아구몬 계열) ───────────────────
     *   [원작 조건]
     *   그레이몬  : 케어미스 0~3,  훈련 32+          → effort ≥ 8
     *   데블몬    : 케어미스 0~3,  훈련 0~31 (폴백)
     *   메라몬    : 케어미스 4+,   훈련 16+           → effort ≥ 4
     *   티라노몬  : 케어미스 4+,   훈련 5~15          → effort ≥ 1
     *   워매몬    : 폴백 (케어미스 4+, 훈련 0~4)      */
    {  IDX_AGUMON,            IDX_GREYMON,          3,   8,    99,   0,   0 },
    {  IDX_AGUMON,            IDX_DEVIMON,          3,   0,    99,   0,   0 }, // 폴백 (케어 우수)
    {  IDX_AGUMON,            IDX_MERAMON,         99,   4,    99,   0,   0 },
    {  IDX_AGUMON,            IDX_TYRANNOMON,      99,   1,    99,   0,   0 },
    {  IDX_AGUMON,            IDX_WORMMON,         99,   0,    99,   0,   0 }, // 폴백

    /* ── ROOKIE → CHAMPION  (베타몬 계열) ───────────────────
     *   [원작 조건]
     *   데블몬    : 케어미스 0~3,  훈련 48+                    → effort ≥ 12
     *   메라몬    : 케어미스 0~3,  훈련 0~47 (폴백)
     *   에어드라몬: 케어미스 4+,   훈련 8+, 과식 0~3           → effort ≥ 2, overfeed ≤ 3
     *   시드라몬  : 케어미스 4+,   훈련 8+, 과식 4+            → effort ≥ 2, overfeed ≥ 4
     *   워매몬    : 폴백 (케어미스 4+, 훈련 0~7)               */
    {  IDX_BETAMON,           IDX_DEVIMON,          3,  12,    99,   0,   0 },
    {  IDX_BETAMON,           IDX_MERAMON,          3,   0,    99,   0,   0 }, // 폴백 (케어 우수)
    {  IDX_BETAMON,           IDX_AIRDRAMON,       99,   2,     3,   0,   0 }, // 과식 ≤ 3
    {  IDX_BETAMON,           IDX_SEADRAMON,       99,   2,    99,   4,   0 }, // 과식 ≥ 4
    {  IDX_BETAMON,           IDX_WORMMON,         99,   0,    99,   0,   0 }, // 폴백

    /* ── CHAMPION → ULTIMATE ────────────────────────────────
     *   원작: 배틀 승리 조건 → 성숙기 종류로 완전체 결정
     *   좋은 경로(그레이몬/데블몬/에어드라몬) → 메탈그레이몬
     *   나쁜 경로(티라노몬/메라몬/시드라몬)  → 콩알몬
     *   최악 경로(워매몬)                    → 퍼펫몬
     *   ※ 배틀 시스템 구현 후 min_wins=12 로 변경 예정             */
    {  IDX_GREYMON,           IDX_METALGREYMON,    99,   0,    99,   0,  12 },
    {  IDX_DEVIMON,           IDX_METALGREYMON,    99,   0,    99,   0,  12 },
    {  IDX_AIRDRAMON,         IDX_METALGREYMON,    99,   0,    99,   0,  12 },
    {  IDX_TYRANNOMON,        IDX_SUKAMON,         99,   0,    99,   0,  12 },
    {  IDX_MERAMON,           IDX_SUKAMON,         99,   0,    99,   0,  12 },
    {  IDX_SEADRAMON,         IDX_SUKAMON,         99,   0,    99,   0,  12 },
    {  IDX_WORMMON,           IDX_PUPPETMON,       99,   0,    99,   0,  12 },

    /* ── ULTIMATE → MEGA ────────────────────────────────────
     *   메탈그레이몬 + 완벽 케어(≤1) + 최고 노력(≥15) + 과식 없음 → 오메가몬
     *   메탈그레이몬 + 케어미스 0~3                              → 블리츠그레이몬
     *   메탈그레이몬 폴백                                        → 반초콩알몬
     *   콩알몬                                                   → 반초콩알몬
     *   퍼펫몬                                                   → 반초콩알몬   */
    {  IDX_METALGREYMON,      IDX_BLITZGREYMON,     3,   0,    99,   0,   0 }, // 좋은 케어
    {  IDX_METALGREYMON,      IDX_BANCHOUKOMON,    99,   0,    99,   0,   0 }, // 폴백
    {  IDX_SUKAMON,           IDX_BANCHOUKOMON,    99,   0,    99,   0,   0 },
    {  IDX_PUPPETMON,         IDX_BANCHOUKOMON,    99,   0,    99,   0,   0 },
};

#define EVO_RULES_SIZE ((int)(sizeof(evo_rules) / sizeof(evo_rules[0])))

/* =========================================================
 * do_evolve - 진화 공통 처리
 *   레벨/테이블/이름/타입/체중/DP/수치를 일괄 갱신한다.
 *   check_evolution()에서만 호출한다.
 * ========================================================= */
static void do_evolve(GameData *game, int next_idx) {
    Digimon *d      = &game->current;
    time_t   now    = time(NULL);
    d->level        = digimon_table[next_idx].level;
    d->table_idx    = next_idx;
    strncpy_s(d->name, MAX_NAME_LEN, digimon_table[next_idx].name, MAX_NAME_LEN - 1);
    d->type         = digimon_table[next_idx].type;
    d->weight       = digimon_table[next_idx].base_weight;
    d->max_dp       = max_dp_table[d->level];
    d->dp           = d->max_dp;   /* 진화 시 DP 꽉 채움 */

    /* 태어날 때 배고픔·근력 0 → 즉시 콜 발생 */
    d->hungry             = 0;
    d->strength           = 0;
    d->hungry_zero_time   = now;
    d->strength_zero_time = now;

    /* 틱 타이머 리셋 (진화 직후 즉시 또 감소하는 것 방지) */
    d->last_hungry_tick   = now;
    d->last_strength_tick = now;

    d->level_start_time   = now;
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
        if (d->overfeed       < r->min_overfeed) continue;
        if (d->wins           < r->min_wins)     continue;

        /* 진화 예약: 애니메이션 완료 후 main.c에서 evolve_to() 호출
         *   EGG   → ANIM_HATCH (부화 흔들림 4초)
         *   비-EGG → ANIM_EVOLVE (깜빡임 1초) */
        game->hatch_target_idx = r->to_idx;
        return;
    }
}

bool check_call(GameData *game) {
    Digimon *d   = &game->current;
    time_t   now = time(NULL);

    /* 콜 조건: 배고픔 0 / 부상 / 똥 가득 (알은 제외) */
    if (d->level == EGG) { game->is_call = false; return false; }
    bool need_call = d->hungry == 0
                  || d->is_injuries
                  || d->poop >= MAX_POOP;

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
    //디지몬 추가
    game->tamer.is_digimon = true;
    Digimon* d = &game->current;

    // digimon_table[0] = 디지에그
    d->table_idx = 0;
    strncpy_s(d->name, MAX_NAME_LEN, digimon_table[0].name, MAX_NAME_LEN - 1);
    d->level = EGG;
    d->type = FREE;

    // 수치 초기화
    d->age = 0;
    d->weight = 0;
    d->hungry = 0;
    d->strength = 0;
    d->poop = 0;
    d->care_mistakes = 0;
    d->dp = 0;
    d->max_dp = max_dp_table[EGG];
    d->overfeed = 0;
    d->effort      = 0;
    d->train_count = 0;
    d->battles     = 0;
    d->wins        = 0;
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

/* =========================================================
 * 플레이어 액션
 * ========================================================= */

void action_feed(GameData *game) {
    Digimon *d = &game->current;
    if (d->level == EGG || d->is_sleep) return;

    if (d->hungry >= MAX_HUNGRY) {
        /* 과식: 배고픔 이미 최대 → 과식 카운트 */
        d->overfeed++;
        d->is_overfed    = true;
        d->overfeed_time = time(NULL);
    } else {
        d->hungry++;
    }
    /* 먹이줄 때마다 체중 +1 */
    if (d->weight < MAX_WEIGHT) d->weight++;
}

void action_train(GameData *game) {
    Digimon *d = &game->current;
    /* 성장기 미만, 수면 중, DP 없으면 훈련 불가 */
    if (d->level < ROOKIE || d->is_sleep || d->dp <= 0) return;

    d->dp--;
    if (d->strength < MAX_STRENGTH) d->strength++;

    /* 훈련으로 체중 감소 (base_weight 미만으로는 내려가지 않음) */
    int base_w = digimon_table[d->table_idx].base_weight;
    if (d->weight > base_w) d->weight--;

    /* 4회마다 노력치 +1 */
    d->train_count++;
    if (d->train_count % 4 == 0) d->effort++;
}

void action_clean(GameData *game) {
    if (game->current.poop > 0) game->current.poop--;
}

void action_cure(GameData *game) {
    Digimon *d = &game->current;
    if (!d->is_injuries) return;
    d->is_injuries = false;
    d->injuries++;
}

void action_sleep_toggle(GameData *game) {
    Digimon *d = &game->current;
    if (d->level == EGG) return;
    d->is_sleep = !d->is_sleep;
}

/* =========================================================
 * 배틀 시스템
 * ========================================================= */

int calc_power(const Digimon *d) {
    int base     = digimon_table[d->table_idx].base_power;
    int str_b    = d->strength * POWER_PER_STRENGTH;
    int wt_bonus = 0;
    /* 근력 최대 + 체중 최대 미만이면 보너스 +16 */
    if (d->strength == MAX_STRENGTH && d->weight < MAX_WEIGHT)
        wt_bonus = POWER_BONUS_MAX;
    return base + str_b + wt_bonus;
}

void gen_cpu_opponent(BattleOpponent *opp, Level level) {
    int cands[DIGIMON_TABLE_SIZE];
    int cnt = 0;
    for (int i = 0; i < DIGIMON_TABLE_SIZE; i++) {
        if (digimon_table[i].level == level && digimon_table[i].base_power > 0)
            cands[cnt++] = i;
    }
    if (cnt == 0) {
        strncpy_s(opp->name, MAX_NAME_LEN, "???", 3);
        opp->power = 10;
        return;
    }
    int idx  = cands[rand() % cnt];
    strncpy_s(opp->name, MAX_NAME_LEN, digimon_table[idx].name, MAX_NAME_LEN - 1);
    int base = digimon_table[idx].base_power;
    int var  = base / 5;   /* ±20% */
    opp->power = base + (rand() % (var * 2 + 1)) - var;
    if (opp->power < 1) opp->power = 1;
}


/* crossed_hour - from~to 구간 안에 target_hour 정각이 포함되어 있으면 true.
 *   update_status()의 1초 틱마다 취침/기상 시각 통과 여부를 판정한다. */
static bool crossed_hour(time_t from, time_t to, int target_hour) {
    struct tm t  = *localtime(&from);
    t.tm_hour    = target_hour;
    t.tm_min     = 0;
    t.tm_sec     = 0;
    time_t candidate = mktime(&t);
    if (candidate <= from) candidate += DAY;  // 이미 지난 경우 다음 날로
    return candidate <= to;
}

/* next_hour_after - base 이후 처음 맞는 target_hour 시각을 반환한다. */
static time_t next_hour_after(time_t base, int target_hour) {
    struct tm t = *localtime(&base);
    t.tm_hour   = target_hour;
    t.tm_min    = 0;
    t.tm_sec    = 0;
    time_t candidate = mktime(&t);
    if (candidate <= base) candidate += DAY;
    return candidate;
}

/* =========================================================
 * apply_offline_time
 *   종료 후 재시작 시 last_update ~ 현재까지 경과 시간을
 *   배고픔·근력·나이·부상에 일괄 반영한다.
 *   load_game() 직후 한 번만 호출.
 * ========================================================= */
void apply_offline_time(GameData *game) {
    Digimon     *d    = &game->current;
    DigimonInfo *info = &digimon_table[d->table_idx];
    time_t       now  = time(NULL);
    time_t       from = game->last_update;

    if (now <= from || d->level == EGG) {
        game->last_update = now;
        return;
    }

    /* ── 오프라인 중 자동 기상 ──────────────────────────────────
     *   수면 중 종료 → 기상 시각(wake_hour)이 오프라인 구간 안에
     *   있었으면 자동 기상하고, 배고픔·근력 타이머를 기상 시각으로
     *   앞당긴다. 이후 hunger/strength 블록이 기상~now 구간을 계산. */
    if (d->is_sleep) {
        time_t wake_at = next_hour_after(from, info->wake_hour);
        if (wake_at < now) {
            d->is_sleep           = false;
            d->last_hungry_tick   = wake_at;
            d->last_strength_tick = wake_at;
        }
    }

    /* ── 배고픔 / 똥 ─────────────────────────────────── */
    if (info->hungry_tick > 0 && !d->is_sleep) {
        long long ticks = (long long)(now - d->last_hungry_tick) / info->hungry_tick;
        for (long long i = 0; i < ticks; i++) {
            time_t t = d->last_hungry_tick + (time_t)((i + 1) * info->hungry_tick);
            if (d->hungry > 0) {
                d->hungry--;
                if (d->hungry == 0) d->hungry_zero_time = t;
            }
            if (d->poop < MAX_POOP) d->poop++;
        }
        if (ticks > 0)
            d->last_hungry_tick += (time_t)(ticks * info->hungry_tick);
    }

    /* ── 근력 ────────────────────────────────────────── */
    if (info->strength_tick > 0 && !d->is_sleep) {
        long long ticks = (long long)(now - d->last_strength_tick) / info->strength_tick;
        if (ticks > 0) {
            d->strength -= (int)ticks;
            if (d->strength < 0) d->strength = 0;
            d->last_strength_tick += (time_t)(ticks * info->strength_tick);
        }
    }

    /* ── 나이 (자정 통과 횟수) ──────────────────────── */
    {
        struct tm tm_from = *localtime(&from);
        tm_from.tm_hour = tm_from.tm_min = tm_from.tm_sec = 0;
        time_t midnight = mktime(&tm_from) + DAY;
        while (midnight <= now) {
            if (d->age < MAX_AGE) d->age++;
            d->last_midnight = midnight;
            midnight += DAY;
        }
    }

    /* ── 부상 (오프라인 중 똥 4개 이상) ─────────────── */
    if (d->poop >= 4 && !d->is_injuries) {
        d->is_injuries = true;
        d->injury_time = from;  // 대략적 시각 (오프라인 중 발생)
    }

    game->last_update = now;
}

/* =========================================================
 * 저장 / 불러오기  —  텍스트 key=value 포맷
 *   파일: digimon.txt  (직접 열어서 확인·수정 가능)
 *   헤더: [DIGIMON_SAVE v<VERSION>]
 *   버전 불일치 시 load_game()은 false → 새 게임 시작.
 * ========================================================= */
bool save_game(const GameData *game) {
    FILE *fp;
    if (fopen_s(&fp, SAVE_FILE, "w") != 0) return false;

    const Tamer   *t = &game->tamer;
    const Digimon *d = &game->current;

    fprintf(fp, "[DIGIMON_SAVE v%u]\n", SAVE_VERSION);

    /* 테이머 */
    fprintf(fp, "tamer_name=%s\n",        t->name);
    fprintf(fp, "tamer_battles=%d\n",     t->battles);
    fprintf(fp, "is_digimon=%d\n",        (int)t->is_digimon);

    /* 디지몬 */
    fprintf(fp, "dname=%s\n",             d->name);
    fprintf(fp, "table_idx=%d\n",         d->table_idx);
    fprintf(fp, "level=%d\n",             (int)d->level);
    fprintf(fp, "type=%d\n",              (int)d->type);
    fprintf(fp, "age=%d\n",               d->age);
    fprintf(fp, "weight=%d\n",            d->weight);
    fprintf(fp, "hungry=%d\n",            d->hungry);
    fprintf(fp, "overfeed=%d\n",          d->overfeed);
    fprintf(fp, "strength=%d\n",          d->strength);
    fprintf(fp, "poop=%d\n",              d->poop);
    fprintf(fp, "care_mistakes=%d\n",     d->care_mistakes);
    fprintf(fp, "dp=%d\n",                d->dp);
    fprintf(fp, "max_dp=%d\n",            d->max_dp);
    fprintf(fp, "effort=%d\n",            d->effort);
    fprintf(fp, "train_count=%d\n",       d->train_count);
    fprintf(fp, "battles=%d\n",           d->battles);
    fprintf(fp, "wins=%d\n",              d->wins);
    fprintf(fp, "sleep=%d\n",             d->sleep);
    fprintf(fp, "is_sleep=%d\n",          (int)d->is_sleep);
    fprintf(fp, "is_overfed=%d\n",        (int)d->is_overfed);
    fprintf(fp, "injuries=%d\n",          d->injuries);
    fprintf(fp, "is_injuries=%d\n",       (int)d->is_injuries);
    fprintf(fp, "is_old=%d\n",            (int)d->is_old);
    fprintf(fp, "overfeed_time=%I64d\n",       (long long)d->overfeed_time);
    fprintf(fp, "injury_time=%I64d\n",         (long long)d->injury_time);
    fprintf(fp, "hungry_zero_time=%I64d\n",    (long long)d->hungry_zero_time);
    fprintf(fp, "strength_zero_time=%I64d\n",  (long long)d->strength_zero_time);
    fprintf(fp, "last_hungry_tick=%I64d\n",    (long long)d->last_hungry_tick);
    fprintf(fp, "last_strength_tick=%I64d\n",  (long long)d->last_strength_tick);
    fprintf(fp, "last_midnight=%I64d\n",       (long long)d->last_midnight);
    fprintf(fp, "level_start_time=%I64d\n",    (long long)d->level_start_time);

    /* GameData */
    fprintf(fp, "last_update=%I64d\n",     (long long)game->last_update);
    fprintf(fp, "is_call=%d\n",           (int)game->is_call);
    fprintf(fp, "call_time=%I64d\n",       (long long)game->call_time);
    fprintf(fp, "hatch_target_idx=%d\n",  game->hatch_target_idx);

    fclose(fp);
    return true;
}

bool load_game(GameData *game) {
    FILE *fp;
    if (fopen_s(&fp, SAVE_FILE, "r") != 0) return false;

    char line[512];

    /* 헤더 확인 */
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return false; }
    unsigned ver = 0;
    if (sscanf_s(line, "[DIGIMON_SAVE v%u]", &ver) != 1 || ver != SAVE_VERSION) {
        fclose(fp); return false;
    }

    memset(game, 0, sizeof(GameData));
    game->hatch_target_idx = -1;

    Tamer   *t = &game->tamer;
    Digimon *d = &game->current;

    while (fgets(line, sizeof(line), fp)) {
        /* \r\n 제거 */
        for (char *p = line; *p; p++) {
            if (*p == '\r' || *p == '\n') { *p = '\0'; break; }
        }
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        const char *key = line;
        const char *val = eq + 1;

        int      iv = 0;
        long long lv = 0;

#define KS(k, f, n) if (strcmp(key,(k))==0){strncpy_s((f),(n),(val),(n)-1);continue;}
#define KI(k, f)    if (strcmp(key,(k))==0){sscanf_s((val),"%d",&iv);(f)=iv;continue;}
#define KB(k, f)    if (strcmp(key,(k))==0){sscanf_s((val),"%d",&iv);(f)=(iv!=0);continue;}
#define KE(k, f, T) if (strcmp(key,(k))==0){sscanf_s((val),"%d",&iv);(f)=(T)iv;continue;}
#define KL(k, f)    if (strcmp(key,(k))==0){sscanf_s((val),"%I64d",&lv);(f)=(time_t)lv;continue;}

        KS("tamer_name",        t->name,           MAX_NAME_LEN)
        KI("tamer_battles",     t->battles)
        KB("is_digimon",        t->is_digimon)
        KS("dname",             d->name,            MAX_NAME_LEN)
        KI("table_idx",         d->table_idx)
        KE("level",             d->level,           Level)
        KE("type",              d->type,            DigimonType)
        KI("age",               d->age)
        KI("weight",            d->weight)
        KI("hungry",            d->hungry)
        KI("overfeed",          d->overfeed)
        KI("strength",          d->strength)
        KI("poop",              d->poop)
        KI("care_mistakes",     d->care_mistakes)
        KI("dp",                d->dp)
        KI("max_dp",            d->max_dp)
        KI("effort",            d->effort)
        KI("train_count",       d->train_count)
        KI("battles",           d->battles)
        KI("wins",              d->wins)
        KI("sleep",             d->sleep)
        KB("is_sleep",          d->is_sleep)
        KB("is_overfed",        d->is_overfed)
        KI("injuries",          d->injuries)
        KB("is_injuries",       d->is_injuries)
        KB("is_old",            d->is_old)
        KL("overfeed_time",     d->overfeed_time)
        KL("injury_time",       d->injury_time)
        KL("hungry_zero_time",  d->hungry_zero_time)
        KL("strength_zero_time",d->strength_zero_time)
        KL("last_hungry_tick",  d->last_hungry_tick)
        KL("last_strength_tick",d->last_strength_tick)
        KL("last_midnight",     d->last_midnight)
        KL("level_start_time",  d->level_start_time)
        KL("last_update",       game->last_update)
        KB("is_call",           game->is_call)
        KL("call_time",         game->call_time)
        KI("hatch_target_idx",  game->hatch_target_idx)

#undef KS
#undef KI
#undef KB
#undef KE
#undef KL
    }

    fclose(fp);
    return t->is_digimon;
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

    /* ── 자동 취침 / 기상 ───────────────────────────────────────
     *   sleep_hour 통과: 플레이어가 이미 재웠으면 케어미스 없음
     *   wake_hour  통과: 강제 기상
     *   hungry_tick==0(EGG)은 수면 스케줄 없음 → 자동 제외      */
    if (info->hungry_tick > 0) {
        time_t prev = game->last_update;
        if (crossed_hour(prev, now, info->sleep_hour)) {
            if (!d->is_sleep) d->care_mistakes++;
            d->is_sleep = true;
        }
        if (crossed_hour(prev, now, info->wake_hour)) {
            d->is_sleep = false;
        }
    }

    /* 수면 중에는 배고픔/근력/똥 변화 없음 */
    if (!d->is_sleep) {
        /* 배고픔 감소 + 똥 증가
         * hungry_tick == 0 이면 "먹지 않음" → 갱신 자체를 건너뜀 */
        if (info->hungry_tick > 0) {
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
        }

        /* 근력 감소
         * strength_tick == 0 이면 "감소 없음" → 건너뜀 */
        if (info->strength_tick > 0 &&
            now - d->last_strength_tick >= info->strength_tick) {
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

    game->last_update = now;
}
