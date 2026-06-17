'use strict';
// sprites.c 완전 재구성:
//   프리앰블 (비-디지몬 내용) + new_extracted.c 의 128개 디지몬 스프라이트 (오메가몬 제외)

const fs = require('fs');
const spritesPath = '../sprites.c';
const spritesSrc  = fs.readFileSync(spritesPath,   'utf8');
const newExtSrc   = fs.readFileSync('new_extracted.c', 'utf8');

// 디지몬 이름 목록 (오메가몬 제외)
const DIGIMON = new Set([
    'botamon','koromon','agumon','betamon','greymon','devimon',
    'airdramon','tyrannomon','meramon','seadramon','wormmon',
    'metalgreymon','sukamon','puppetmon','blitzgreymon','banchoukomon'
]);

// ── 1. 현재 sprites.c 에서 "비-디지몬" 부분만 추출
//   방법: 전체를 라인별로 처리하며 디지몬 `int sprite_X_N[...]` 블록만 건너뜀
function isDigimonArray(name) {
    // sprite_agumon_1, sprite_botamon_3 등 패턴: sprite_{digimon}_{digit}
    const m = name.match(/^sprite_([a-z]+)_(\d+)$/);
    return m && DIGIMON.has(m[1]);
}

// 모든 int sprite_* 블록을 찾아서 디지몬이거나 중복이면 제거
const re = /int (sprite_\w+)\[(?:SPRITE_H|UI_SPRITE_H)\]\[(?:SPRITE_W|UI_SPRITE_W)\] = \{[\s\S]*?\};/g;
let preamble = spritesSrc;
const toRemove = [];
const seenPre = {};
let m;
while ((m = re.exec(spritesSrc)) !== null) {
    const name = m[1];
    if (isDigimonArray(name) || seenPre[name]) {
        toRemove.push({ start: m.index, end: m.index + m[0].length });
    } else {
        seenPre[name] = true;
    }
}
// 뒤에서부터 제거 (인덱스 유지)
for (let i = toRemove.length - 1; i >= 0; i--) {
    const { start, end } = toRemove[i];
    preamble = preamble.slice(0, start) + preamble.slice(end);
}
// 연속 빈 줄 정리
preamble = preamble.replace(/\n{3,}/g, '\n\n').trimEnd();

// ── 2. new_extracted.c 에서 128개 수집
const digimon = [];
const re2 = /int (sprite_(\w+)_(\d+))\[SPRITE_H\]\[SPRITE_W\] = \{[\s\S]*?\};/g;
while ((m = re2.exec(newExtSrc)) !== null) {
    if (!DIGIMON.has(m[2])) continue;
    digimon.push(m[0]);
}
console.error(`Digimon arrays: ${digimon.length}`);

// ── 3. 합치기 및 중복 검사
const result = preamble + '\n\n' + digimon.join('\n\n') + '\n';

const seen = {};
let dupes = 0;
for (const mm of result.matchAll(/^int (sprite_\w+)\[/gm)) {
    if (seen[mm[1]]) { console.error('DUPE:', mm[1]); dupes++; }
    seen[mm[1]] = true;
}
const total = (result.match(/^int [\w\[\]]+\[/gm) || []).length;
console.error(`Total arrays: ${total}, dupes: ${dupes}, lines: ${result.split('\n').length}`);

if (dupes > 0) { process.exit(1); }
fs.writeFileSync(spritesPath, result, 'utf8');
console.error('Done.');
