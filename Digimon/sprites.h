#ifndef SPRITES_H
#define SPRITES_H

/* =========================================================
 * 스프라이트 크기 상수
 *   SPRITE_W, SPRITE_H 는 ui.h 에서도 사용하므로
 *   이 헤더에서 단일 정의(single source of truth)한다.
 *   ui.h 의 중복 정의는 제거했다.
 * ========================================================= */
#define SPRITE_W 16
#define SPRITE_H 16

/* 픽셀 폰트 크기 */
#define FONT_W   8
#define FONT_H   8

/* 숫자 픽셀 폰트 크기 (0~9) */
#define DIGIT_W  4
#define DIGIT_H  7

/* 미니 픽셀 폰트 크기 (A~Z 라벨용, 3×5) */
#define MINI_W   3
#define MINI_H   5

/* 이름 픽셀 폰트 크기 (A~Z 인트로 이름 표시용, 5×8) */
#define NAME_W   5
#define NAME_H   8

/* =========================================================
 * 스프라이트 픽셀 값 의미
 *   PIXEL_BG  (1) : 배경색 → drawSprite()에서 건너뜀
 *   PIXEL_INK (0) : 전경색 → colorMap[0] 색으로 그림
 *
 *   3색 이상이 필요할 경우 PIXEL_* 상수를 추가하고
 *   colorMap[] 크기를 함께 늘린다.
 * ========================================================= */
#define PIXEL_BG  1
#define PIXEL_INK 0

/* =========================================================
 * colorMap
 *   인덱스 = 스프라이트 픽셀 값 (PIXEL_INK=0, PIXEL_BG=1 …)
 *   값     = Windows 콘솔 색상 번호 (0=검정, 15=흰색 등)
 *   크기를 바꾸면 COLORMAP_SIZE 도 함께 수정한다.
 * ========================================================= */
#define COLORMAP_SIZE 2
extern int colorMap[COLORMAP_SIZE];

/* =========================================================
 * 스프라이트 데이터 선언
 *   새 스프라이트를 추가할 때:
 *     1) sprites.c 에 int newBackground[SPRITE_H][SPRITE_W] = { … }; 추가
 *     2) 아래에 extern 선언 추가
 *     3) background_table[]
 * ========================================================= */
extern int sprite_blank[SPRITE_H][SPRITE_W]; // 이름 정하는 배경

/* =========================================================
 * 스프라이트 데이터 선언
 * 1, 2 = 기본 / 3 : 기분좋음 / 4 : 공격 / 5 : 화남 / 6 : 슬픔 / 피격 / 아픔 /7 : 잠 
 *   새 스프라이트를 추가할 때:
 *     1) sprites.c 에 int newSprite[SPRITE_H][SPRITE_W] = { … }; 추가
 *     2) 아래에 extern 선언 추가
 *     3) digimon_table[] 의 해당 항목에 포인터 연결 (digimon.h 참고)
 * ========================================================= */

extern int background_name_1  [16][32];

extern int sprite_blank       [SPRITE_H][SPRITE_W]; // 빈(모두 배경) 스프라이트

extern int sprite_egg_1       [SPRITE_H][SPRITE_W];
extern int sprite_egg_2       [SPRITE_H][SPRITE_W];
extern int sprite_egg_3       [SPRITE_H][SPRITE_W];

extern int sprite_botamon_1   [SPRITE_H][SPRITE_W]; // 먹기 1 
extern int sprite_botamon_2   [SPRITE_H][SPRITE_W]; // 먹기 2 
extern int sprite_botamon_3   [SPRITE_H][SPRITE_W]; // 슬픔, 아픔 
extern int sprite_botamon_4   [SPRITE_H][SPRITE_W]; // 잠 
extern int sprite_botamon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_botamon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_botamon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_botamon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 아구몬 추가 프레임 — TODO: 그려야 함 */

extern int sprite_agumon_1    [SPRITE_H][SPRITE_W]; /* 먹기1     */
extern int sprite_agumon_2    [SPRITE_H][SPRITE_W]; /* 먹기2     */
extern int sprite_agumon_3    [SPRITE_H][SPRITE_W]; /* 아픔/부상 */
extern int sprite_agumon_4    [SPRITE_H][SPRITE_W]; /* 잠        */
extern int sprite_agumon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_agumon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_agumon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_agumon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 코로몬 (BABY2) — TODO: 그려야 함 */

extern int sprite_koromon_1   [SPRITE_H][SPRITE_W]; /* 먹기1     */
extern int sprite_koromon_2   [SPRITE_H][SPRITE_W]; /* 먹기2     */
extern int sprite_koromon_3   [SPRITE_H][SPRITE_W]; /* 아픔/부상 */
extern int sprite_koromon_4   [SPRITE_H][SPRITE_W]; /* 잠        */
extern int sprite_koromon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_koromon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_koromon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_koromon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 베타몬 (ROOKIE) — TODO: 그려야 함 */

extern int sprite_betamon_1   [SPRITE_H][SPRITE_W]; /* 먹기1     */
extern int sprite_betamon_2   [SPRITE_H][SPRITE_W]; /* 먹기2     */
extern int sprite_betamon_3   [SPRITE_H][SPRITE_W]; /* 아픔/부상 */
extern int sprite_betamon_4   [SPRITE_H][SPRITE_W]; /* 잠        */
extern int sprite_betamon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_betamon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_betamon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_betamon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 그레이몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_greymon_1   [SPRITE_H][SPRITE_W]; extern int sprite_greymon_2[SPRITE_H][SPRITE_W];
extern int sprite_greymon_3   [SPRITE_H][SPRITE_W]; extern int sprite_greymon_4[SPRITE_H][SPRITE_W];
extern int sprite_greymon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_greymon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_greymon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_greymon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 데블몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_devimon_1   [SPRITE_H][SPRITE_W]; extern int sprite_devimon_2[SPRITE_H][SPRITE_W];
extern int sprite_devimon_3   [SPRITE_H][SPRITE_W]; extern int sprite_devimon_4[SPRITE_H][SPRITE_W];
extern int sprite_devimon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_devimon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_devimon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_devimon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 에어드라몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_airdramon_1 [SPRITE_H][SPRITE_W]; extern int sprite_airdramon_2[SPRITE_H][SPRITE_W];
extern int sprite_airdramon_3 [SPRITE_H][SPRITE_W]; extern int sprite_airdramon_4[SPRITE_H][SPRITE_W];
extern int sprite_airdramon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_airdramon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_airdramon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_airdramon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 티라노몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_tyrannomon_1[SPRITE_H][SPRITE_W]; extern int sprite_tyrannomon_2[SPRITE_H][SPRITE_W];
extern int sprite_tyrannomon_3[SPRITE_H][SPRITE_W]; extern int sprite_tyrannomon_4[SPRITE_H][SPRITE_W];
extern int sprite_tyrannomon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_tyrannomon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_tyrannomon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_tyrannomon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 메라몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_meramon_1   [SPRITE_H][SPRITE_W]; extern int sprite_meramon_2[SPRITE_H][SPRITE_W];
extern int sprite_meramon_3   [SPRITE_H][SPRITE_W]; extern int sprite_meramon_4[SPRITE_H][SPRITE_W];
extern int sprite_meramon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_meramon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_meramon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_meramon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 시드라몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_seadramon_1 [SPRITE_H][SPRITE_W]; extern int sprite_seadramon_2[SPRITE_H][SPRITE_W];
extern int sprite_seadramon_3 [SPRITE_H][SPRITE_W]; extern int sprite_seadramon_4[SPRITE_H][SPRITE_W];
extern int sprite_seadramon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_seadramon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_seadramon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_seadramon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 워매몬 (CHAMPION) — TODO: 그려야 함 */

extern int sprite_wormmon_1   [SPRITE_H][SPRITE_W]; extern int sprite_wormmon_2[SPRITE_H][SPRITE_W];
extern int sprite_wormmon_3   [SPRITE_H][SPRITE_W]; extern int sprite_wormmon_4[SPRITE_H][SPRITE_W];
extern int sprite_wormmon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_wormmon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_wormmon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_wormmon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 메탈그레이몬 (ULTIMATE) — TODO: 그려야 함 */

extern int sprite_metalgreymon_1[SPRITE_H][SPRITE_W]; extern int sprite_metalgreymon_2[SPRITE_H][SPRITE_W];
extern int sprite_metalgreymon_3[SPRITE_H][SPRITE_W]; extern int sprite_metalgreymon_4[SPRITE_H][SPRITE_W];
extern int sprite_metalgreymon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_metalgreymon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_metalgreymon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_metalgreymon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 콩알몬 (ULTIMATE) — TODO: 그려야 함 */

extern int sprite_sukamon_1   [SPRITE_H][SPRITE_W]; extern int sprite_sukamon_2[SPRITE_H][SPRITE_W];
extern int sprite_sukamon_3   [SPRITE_H][SPRITE_W]; extern int sprite_sukamon_4[SPRITE_H][SPRITE_W];
extern int sprite_sukamon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_sukamon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_sukamon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_sukamon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 퍼펫몬 (ULTIMATE) — TODO: 그려야 함 */

extern int sprite_puppetmon_1 [SPRITE_H][SPRITE_W]; extern int sprite_puppetmon_2[SPRITE_H][SPRITE_W];
extern int sprite_puppetmon_3 [SPRITE_H][SPRITE_W]; extern int sprite_puppetmon_4[SPRITE_H][SPRITE_W];
extern int sprite_puppetmon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_puppetmon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_puppetmon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_puppetmon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 블리츠그레이몬 (MEGA) — TODO: 그려야 함 */

extern int sprite_blitzgreymon_1[SPRITE_H][SPRITE_W]; extern int sprite_blitzgreymon_2[SPRITE_H][SPRITE_W];
extern int sprite_blitzgreymon_3[SPRITE_H][SPRITE_W]; extern int sprite_blitzgreymon_4[SPRITE_H][SPRITE_W];
extern int sprite_blitzgreymon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_blitzgreymon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_blitzgreymon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_blitzgreymon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* ── 반초콩알몬 (MEGA) — TODO: 그려야 함 */

extern int sprite_banchoukomon_1[SPRITE_H][SPRITE_W]; extern int sprite_banchoukomon_2[SPRITE_H][SPRITE_W];
extern int sprite_banchoukomon_3[SPRITE_H][SPRITE_W]; extern int sprite_banchoukomon_4[SPRITE_H][SPRITE_W];
extern int sprite_banchoukomon_5[SPRITE_H][SPRITE_W]; /* eat1 */
extern int sprite_banchoukomon_6[SPRITE_H][SPRITE_W]; /* eat2 */
extern int sprite_banchoukomon_7[SPRITE_H][SPRITE_W]; /* injury */
extern int sprite_banchoukomon_8[SPRITE_H][SPRITE_W]; /* sleep */

/* =========================================================
 * UI 아이콘 스프라이트 (8×8)
 *   케이지 안에 표시되는 작은 아이콘들.
 *   drawUiSprite() 로 그린다.
 * ========================================================= */
#define UI_SPRITE_W 8
#define UI_SPRITE_H 8

extern int sprite_food    [UI_SPRITE_H][UI_SPRITE_W]; /* 음식 아이콘 */
extern int sprite_poop1   [UI_SPRITE_H][UI_SPRITE_W]; /* 똥 아이콘 1 */
extern int sprite_poop2   [UI_SPRITE_H][UI_SPRITE_W]; /* 똥 아이콘 2 */
extern int sprite_zzz     [UI_SPRITE_H][UI_SPRITE_W]; /* 수면 Zzz           — TODO: 그려야 함 */
extern int sprite_medicine[UI_SPRITE_H][UI_SPRITE_W]; /* 부상 약/주사기     — TODO: 그려야 함 */
extern int sprite_dead    [SPRITE_H][SPRITE_W];        /* 사망 화면 (16×16) — TODO: 그려야 함 */

/* =========================================================
 * 픽셀 폰트 / UI 아이콘
 *   font_alpha[26][FONT_H][FONT_W] : A~Z 8×8 픽셀 폰트
 *   font_heart[FONT_H][FONT_W]     : 하트 (꽉 참) — 배고픔/근력 표시용
 *   font_heart_empty[FONT_H][FONT_W]: 하트 (빈)
 * ========================================================= */
extern int font_alpha      [26][FONT_H][FONT_W];
extern int font_heart          [FONT_H][FONT_W];
extern int font_heart_empty    [FONT_H][FONT_W];
extern int font_digit      [10][DIGIT_H][DIGIT_W];
extern int font_mini       [26][MINI_H][MINI_W];
extern int font_name       [26][NAME_H][NAME_W];

#endif /* SPRITES_H */
