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

void check_evolution(GameData *game) {
    Digimon *d   = &game->current;
    time_t   now = time(NULL);

    if (d->level >= MEGA) return;
    if (d->is_sleep && d->level >= BABY2) return; // 낮잠 중 진화 정지 (유년기 이후)

    if (now - d->level_start_time < evolution_time[d->level]) return;

    /* EGG, BABY1 → 자동 진화 */
    if (d->level == EGG) {
        d->level      = BABY1;
        d->table_idx  = 1; // 깜몬
        strncpy_s(d->name, MAX_NAME_LEN, digimon_table[1].name, MAX_NAME_LEN - 1);
        d->type       = digimon_table[1].type;
        d->weight     = digimon_table[1].base_weight;
        d->hungry     = MAX_HUNGRY;
        d->strength   = MAX_STRENGTH;
        d->max_dp     = max_dp_table[BABY1];
        d->level_start_time = now;
        return;
    }
    if (d->level == BABY1) {
        d->level      = BABY2;
        d->table_idx  = 2; // 코로몬
        strncpy_s(d->name, MAX_NAME_LEN, digimon_table[2].name, MAX_NAME_LEN - 1);
        d->type       = digimon_table[2].type;
        d->weight     = digimon_table[2].base_weight;
        d->max_dp     = max_dp_table[BABY2];
        d->level_start_time = now;
        return;
    }

    /* BABY2 이상 → 조건부 진화 (데모: 시간만 체크) */
    // TODO: care_mistakes / effort / overfeed 조건 추가
    int next_idx = d->table_idx + 1;
    if (next_idx >= (int)(sizeof(digimon_table) / sizeof(digimon_table[0]))) return;
    if (digimon_table[next_idx].level != d->level + 1) return;

    d->level     = digimon_table[next_idx].level;
    d->table_idx = next_idx;
    strncpy_s(d->name, MAX_NAME_LEN, digimon_table[next_idx].name, MAX_NAME_LEN - 1);
    d->type      = digimon_table[next_idx].type;
    d->weight    = digimon_table[next_idx].base_weight;
    d->max_dp    = max_dp_table[d->level];
    d->level_start_time = now;
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
    d->hungry_zero_time   = now;
    d->strength_zero_time = now;
    d->injury_time        = now;
    d->last_hungry_tick   = now;
    d->last_strength_tick = now;
    game->last_update = now;
    game->is_call     = false;
    game->call_time   = now;

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
        int tick = info->hungry_tick * (d->is_overfed ? 2 : 1);
        if (now - d->last_hungry_tick >= tick) {
            if (d->hungry > 0) {
                d->hungry--;
                if (d->hungry == 0)
                    d->hungry_zero_time = now;
            }
            if (d->poop < MAX_POOP) d->poop++;
            d->last_hungry_tick = now;
        }

        /* 과식 상태 해제 */
        if (d->is_overfed && d->hungry < MAX_HUNGRY)
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
