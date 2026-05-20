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
 * 1, 2 = 기본 / 3 : 기분좋음 / 4 : 공격 / 5 : 화남 / 6 : 슬픔 / 피격 / 아픔 /7 : 잠 
 *   새 스프라이트를 추가할 때:
 *     1) sprites.c 에 int newSprite[SPRITE_H][SPRITE_W] = { … }; 추가
 *     2) 아래에 extern 선언 추가
 *     3) digimon_table[] 의 해당 항목에 포인터 연결 (digimon.h 참고)
 * ========================================================= */
extern int sprite_blank       [SPRITE_H][SPRITE_W]; // 빈(모두 배경) 스프라이트
extern int sprite_egg_1       [SPRITE_H][SPRITE_W]; // 알 애니메이션 프레임 1
extern int sprite_egg_2		  [SPRITE_H][SPRITE_W]; // 알 애니메이션 프레임 2
extern int sprite_egg_3       [SPRITE_H][SPRITE_W]; // 알 애니메이션 프레임 3
extern int sprite_botamon_1   [SPRITE_H][SPRITE_W]; // IDE1 
extern int sprite_botamon_2   [SPRITE_H][SPRITE_W]; // IDE2
extern int sprite_botamon_3   [SPRITE_H][SPRITE_W]; // 기쁨
extern int sprite_botamon_4   [SPRITE_H][SPRITE_W]; // 공격 
extern int sprite_botamon_5   [SPRITE_H][SPRITE_W]; // 먹기 1 
extern int sprite_botamon_6   [SPRITE_H][SPRITE_W]; // 먹기 2 
extern int sprite_botamon_7   [SPRITE_H][SPRITE_W]; // 슬픔, 아픔 
extern int sprite_botamon_8   [SPRITE_H][SPRITE_W]; // 잠 
extern int sprite_agumon_1      [SPRITE_H][SPRITE_W]; // 아구몬

#endif /* SPRITES_H */
