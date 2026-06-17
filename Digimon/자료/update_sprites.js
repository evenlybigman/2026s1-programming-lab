'use strict';
// sprites.c의 14개 디지몬 스프라이트를 new_digimon.c 데이터로 교체
// blitzgreymon, banchoukomon은 기존 유지

const fs = require('fs');
const spritesPath  = '../sprites.c';
const newDigiPath  = 'new_digimon.c';

const spritesSrc = fs.readFileSync(spritesPath, 'utf8');
const newDigiSrc = fs.readFileSync(newDigiPath, 'utf8');

// 프리앰블에서 제거할 디지몬 (16개 전부)
const REPLACE = new Set([
    'botamon','koromon','agumon','betamon','greymon','devimon',
    'airdramon','tyrannomon','meramon','seadramon','wormmon',
    'metalgreymon','sukamon','puppetmon','blitzgreymon','banchoukomon'
]);

// ── 1. sprites.c에서 교체 대상 배열 제거
function isReplaceArray(name) {
    const m = name.match(/^sprite_([a-z]+)_(\d+)$/);
    return m && REPLACE.has(m[1]);
}

const re = /int (sprite_\w+)\[(?:SPRITE_H|UI_SPRITE_H)\]\[(?:SPRITE_W|UI_SPRITE_W)\] = \{[\s\S]*?\};/g;
let preamble = spritesSrc;
const toRemove = [];
const seenPre = {};
let m;
while ((m = re.exec(spritesSrc)) !== null) {
    const name = m[1];
    if (isReplaceArray(name) || seenPre[name]) {
        toRemove.push({ start: m.index, end: m.index + m[0].length });
    } else {
        seenPre[name] = true;
    }
}
for (let i = toRemove.length - 1; i >= 0; i--) {
    const { start, end } = toRemove[i];
    preamble = preamble.slice(0, start) + preamble.slice(end);
}
preamble = preamble.replace(/\n{3,}/g, '\n\n').trimEnd();

// ── 2. new_digimon.c에서 56개 배열 수집
const newArrays = [];
const re2 = /int (sprite_(\w+)_(\d+))\[SPRITE_H\]\[SPRITE_W\] = \{[\s\S]*?\};/g;
while ((m = re2.exec(newDigiSrc)) !== null) {
    if (REPLACE.has(m[2])) newArrays.push(m[0]);
}
console.error(`New digimon arrays: ${newArrays.length}`);

// ── 3. blitzgreymon, banchoukomon 기존 배열 추출 (sprite 배열만, 주석 포함 안 함)
const keepNames = ['blitzgreymon', 'banchoukomon'];
const keptArrays = [];
const re3 = /int (sprite_(\w+)_(\d+))\[SPRITE_H\]\[SPRITE_W\] = \{[\s\S]*?\};/g;
while ((m = re3.exec(spritesSrc)) !== null) {
    if (keepNames.includes(m[2])) keptArrays.push(m[0]);
}
console.error(`Kept blitz/bancho arrays: ${keptArrays.length}`);

// ── 4. 합치기
// 순서: preamble (비-디지몬) + 14개 교체 디지몬 + blitz/bancho
const digimonOrder = [
    'botamon','koromon','agumon','betamon','greymon','devimon',
    'airdramon','tyrannomon','meramon','seadramon','wormmon',
    'metalgreymon','sukamon','puppetmon','blitzgreymon','banchoukomon'
];

// 배열들을 이름순 정렬
const allArrays = [...newArrays, ...keptArrays];
const arrayMap = {};
for (const arr of allArrays) {
    const nm = arr.match(/^int (sprite_\w+)\[/)[1];
    arrayMap[nm] = arr;
}

const orderedArrays = [];
for (const digi of digimonOrder) {
    for (let i = 1; i <= 4; i++) {
        const key = `sprite_${digi}_${i}`;
        if (arrayMap[key]) orderedArrays.push(arrayMap[key]);
        else console.error(`MISSING: ${key}`);
    }
}

const result = preamble + '\n\n' + orderedArrays.join('\n\n') + '\n';

// 중복 검사
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
