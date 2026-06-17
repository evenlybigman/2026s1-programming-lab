'use strict';
// sprites.c 재구성:
//   1) 프리앰블 (colorMap, background, eggs, font, BLANK_SP 매크로, UI 아이콘, dead 스프라이트) → lines 1-720
//   2) new_extracted.c 에서 agumon_1 가져와 삽입
//   3) 기존 sprites.c lines 723-2939 유지 (agumon_2..8, botamon_1..8, ..., banchoukomon_8)
//   4) 이후 중복 블록 전부 삭제
//   5) omegamon 은 포함하지 않음

const fs = require('fs');

const spritesPath = '../sprites.c';
const newExtPath  = 'new_extracted.c';

const spritesSrc = fs.readFileSync(spritesPath, 'utf8');
const newExtSrc  = fs.readFileSync(newExtPath,  'utf8');

// ── 1. new_extracted.c 에서 sprite_agumon_1 추출
const agumon1Match = newExtSrc.match(
    /int sprite_agumon_1\[SPRITE_H\]\[SPRITE_W\] = \{[\s\S]*?\};/
);
if (!agumon1Match) { console.error('agumon_1 not found in new_extracted.c'); process.exit(1); }
const agumon1 = agumon1Match[0];

// ── 2. sprites.c 앞부분 (lines 1-720) — sprite_dead 까지 유지
// sprite_dead 블록 끝 위치 찾기
const deadEnd = spritesSrc.indexOf('\n};\n', spritesSrc.indexOf('int sprite_dead[SPRITE_H][SPRITE_W]')) + 4;
const preamble = spritesSrc.slice(0, deadEnd);

// ── 3. lines 723-2939: agumon_2..8 부터 banchoukomon_8 까지
// "아구몬 추가 프레임" 주석부터 시작, banchoukomon_8 블록 끝까지
const agumon2Comment = '\n/* ── 아구몬 추가 프레임 (_1 은 아래에 있음) ── TODO: 그려야 함 */';
const agumon2Start = spritesSrc.indexOf(agumon2Comment);

// banchoukomon_8 블록 끝 찾기 (lines 2922+18 = 2940)
const banchouEnd = spritesSrc.indexOf('\n};\n', spritesSrc.lastIndexOf('int sprite_banchoukomon_8[SPRITE_H][SPRITE_W]')) + 4;
const agumonToBanchou = spritesSrc.slice(agumon2Start, banchouEnd);

// ── 4. 합치기
// agumon_1 주석 업데이트 (더 이상 "아래에 있음"이 아니므로)
const digimonSection = agumonToBanchou.replace(
    '/* ── 아구몬 추가 프레임 (_1 은 아래에 있음) ── TODO: 그려야 함 */',
    ''
);

const result = preamble
    + '\n\n/* ── 아구몬 (CHILD) ── */\n'
    + agumon1
    + '\n'
    + digimonSection;

fs.writeFileSync(spritesPath, result, 'utf8');

// ── 5. 검증
const lines = result.split('\n').length;
const arrCount = (result.match(/^int sprite_\w+\[/gm) || []).length;
console.log(`Done. Lines: ${lines}, sprite arrays: ${arrCount}`);

// 중복 검사
const names = {};
let dupes = 0;
for (const m of result.matchAll(/^int (sprite_\w+)\[/gm)) {
    if (names[m[1]]) { console.error('  DUPE:', m[1]); dupes++; }
    names[m[1]] = true;
}
if (dupes === 0) console.log('No duplicates.');
