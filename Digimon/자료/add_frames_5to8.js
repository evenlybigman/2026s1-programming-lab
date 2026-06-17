'use strict';
// sprites.c에 _5-_8 플레이스홀더 추가, sprites.h에 extern 선언 추가, main.c 뷰어 재생성
const fs = require('fs');

const DIGIMON = [
    'botamon','koromon','agumon','betamon','greymon','devimon',
    'airdramon','tyrannomon','meramon','seadramon','wormmon',
    'metalgreymon','sukamon','puppetmon','blitzgreymon','banchoukomon'
];

const LABELS = {
    1:'walk1', 2:'walk2', 3:'happy', 4:'angry',
    5:'eat1',  6:'eat2',  7:'injury', 8:'sleep'
};

// ── 1. sprites.c: 각 디지몬 _4 배열 뒤에 _5-_8 플레이스홀더 삽입 ──
let spritesC = fs.readFileSync('../sprites.c', 'utf8');

for (const d of DIGIMON) {
    const re = new RegExp(
        `(int sprite_${d}_4\\[SPRITE_H\\]\\[SPRITE_W\\] = \\{[\\s\\S]*?\\};)`,
        'g'
    );
    let replaced = false;
    spritesC = spritesC.replace(re, (m) => {
        if (replaced) return m; // 중복 치환 방지
        replaced = true;
        const adds = [5,6,7,8].map(n =>
            `int sprite_${d}_${n}[SPRITE_H][SPRITE_W] = BLANK_SP; /* ${LABELS[n]} */`
        ).join('\n');
        return m + '\n' + adds;
    });
    if (!replaced) console.error(`WARN: _4 not found for ${d}`);
}

// 중복 검사
const seen = {};
let dupes = 0;
for (const mm of spritesC.matchAll(/^int (sprite_\w+)\[/gm)) {
    if (seen[mm[1]]) { console.error('DUPE:', mm[1]); dupes++; }
    seen[mm[1]] = true;
}
if (dupes > 0) { console.error('Aborting - duplicates'); process.exit(1); }

fs.writeFileSync('../sprites.c', spritesC, 'utf8');
const total = (spritesC.match(/^int [\w]+\[/gm)||[]).length;
console.error(`sprites.c: ${total} arrays, ${spritesC.split('\n').length} lines`);

// ── 2. sprites.h: 각 디지몬 _4 extern 뒤에 _5-_8 추가 ──
let spritesH = fs.readFileSync('../sprites.h', 'utf8');

for (const d of DIGIMON) {
    // _4 선언 라인을 찾아 뒤에 _5-_8 추가 (이미 있으면 건너뜀)
    if (spritesH.includes(`sprite_${d}_5`)) {
        console.error(`SKIP: ${d}_5 already in sprites.h`);
        continue;
    }
    const re = new RegExp(
        `(extern int sprite_${d}_4[^\\n]+)`,
        'g'
    );
    let replaced = false;
    spritesH = spritesH.replace(re, (m) => {
        if (replaced) return m;
        replaced = true;
        const adds = [5,6,7,8].map(n =>
            `extern int sprite_${d}_${n}[SPRITE_H][SPRITE_W]; /* ${LABELS[n]} */`
        ).join('\n');
        return m + '\n' + adds;
    });
    if (!replaced) console.error(`WARN: _4 extern not found for ${d}`);
}

fs.writeFileSync('../sprites.h', spritesH, 'utf8');
console.error('sprites.h updated');

// ── 3. main.c: 뷰어 list[] 재생성 ──
let mainC = fs.readFileSync('../main.c', 'utf8');

// 뷰어 list[] 안의 Digimon 항목 전체를 새로 생성
// 알 항목과 dead/UI 항목은 유지
const eggEntries = `        /* ── 알 ── */
        {"egg_1",               sprite_egg_1,              NULL},
        {"egg_2",               sprite_egg_2,              NULL},
        {"egg_3",               sprite_egg_3,              NULL},`;

let digiEntries = '';
for (const d of DIGIMON) {
    const korean = {
        botamon:'보타몬', koromon:'코로몬', agumon:'아구몬', betamon:'베타몬',
        greymon:'그레이몬', devimon:'데블몬', airdramon:'에어드라몬', tyrannomon:'티라노몬',
        meramon:'메라몬', seadramon:'시드라몬', wormmon:'워매몬', metalgreymon:'메탈그레이몬',
        sukamon:'콩알몬', puppetmon:'퍼펫몬', blitzgreymon:'블리츠그레이몬', banchoukomon:'반초콩알몬'
    }[d];
    digiEntries += `        /* ── ${korean} ── */\n`;
    for (let n = 1; n <= 8; n++) {
        digiEntries += `        {"${d}_${n} ${LABELS[n]}",${' '.repeat(Math.max(1, 20 - d.length - 2 - LABELS[n].length))}sprite_${d}_${n},${' '.repeat(Math.max(1, 20 - d.length - 2))}NULL},\n`;
    }
}

const tailEntries = `        /* ── 사망 (16×16) ── */
        {"dead",                sprite_dead,               NULL},
        /* ── UI 아이콘 (8×8) ── */
        {"food",                NULL,                      sprite_food},
        {"poop1",               NULL,                      sprite_poop1},
        {"poop2",               NULL,                      sprite_poop2},
        {"zzz",                 NULL,                      sprite_zzz},
        {"medicine",            NULL,                      sprite_medicine},`;

const newList = eggEntries + '\n' + digiEntries + tailEntries;

// list[] 내부를 교체
mainC = mainC.replace(
    /(static const struct SpEnt list\[\] = \{)([\s\S]*?)(\};[\s\n]*static const int TOTAL)/,
    (_, open, _body, close) => open + '\n' + newList + '\n    ' + close
);

fs.writeFileSync('../main.c', mainC, 'utf8');
console.error('main.c viewer updated');
console.error('Done.');
