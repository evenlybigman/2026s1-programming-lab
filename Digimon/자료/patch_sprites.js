'use strict';
const fs = require('fs');

const spritesSrc = fs.readFileSync('../sprites.c', 'utf8');
const newSrc     = fs.readFileSync('new_extracted.c', 'utf8');

// Parse all arrays from new_extracted.c
const re = /int (sprite_\w+)\[SPRITE_H\]\[SPRITE_W\] = \{[\s\S]*?\};/g;
let m;
const replacements = {};
while ((m = re.exec(newSrc)) !== null) {
    replacements[m[1]] = m[0];
}
console.error(`Found ${Object.keys(replacements).length} new arrays`);

// Replace each matching array in sprites.c
let result = spritesSrc;
let replaced = 0;
for (const [name, newArr] of Object.entries(replacements)) {
    // Match the existing declaration in sprites.c
    const pattern = new RegExp(
        'int ' + name + '\\[SPRITE_H\\]\\[SPRITE_W\\] = \\{[\\s\\S]*?\\};'
    );
    if (pattern.test(result)) {
        result = result.replace(pattern, newArr);
        replaced++;
    } else {
        console.error(`  NOT FOUND: ${name}`);
    }
}
console.error(`Replaced ${replaced} / ${Object.keys(replacements).length} arrays`);

fs.writeFileSync('../sprites.c', result, 'utf8');
console.error('Done.');
