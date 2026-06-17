/**
 * extract_sprites.js — Node.js PNG sprite extractor for Digital Monster Ver.20th
 * Usage:
 *   node extract_sprites.js info           — print image dimensions, sample grid
 *   node extract_sprites.js preview x y w h — ASCII preview of region
 *   node extract_sprites.js extract name x y w h — output C array
 *   node extract_sprites.js batch list.txt  — batch extract from list file
 *   node extract_sprites.js scan            — scan for non-transparent rows/cols
 */
'use strict';
const fs   = require('fs');
const zlib = require('zlib');

const IMG_PATH = 'C:/WorkSpace/2026_s1_04_prog_lab/Digimon/자료/148724.png';
const TARGET_W = 16;
const TARGET_H = 16;

/* ── PNG parser ── */
function parsePNG(filepath) {
    const buf = fs.readFileSync(filepath);
    const w   = buf.readUInt32BE(16);
    const h   = buf.readUInt32BE(20);
    const bd  = buf[24];
    const ct  = buf[25]; // 6 = RGBA

    if (bd !== 8 || ct !== 6) throw new Error('Only 8-bit RGBA PNG supported');

    // collect IDAT chunks
    const idats = [];
    let pos = 8;
    while (pos < buf.length) {
        const len  = buf.readUInt32BE(pos);
        const type = buf.slice(pos+4, pos+8).toString('ascii');
        if (type === 'IDAT') idats.push(buf.slice(pos+8, pos+8+len));
        if (type === 'IEND') break;
        pos += 12 + len;
    }

    const compressed = Buffer.concat(idats);
    const raw        = zlib.inflateSync(compressed);

    // each scanline: 1 filter byte + w*4 bytes
    const stride = w * 4;
    const pixels = Buffer.alloc(h * stride);

    for (let y = 0; y < h; y++) {
        const filterByte = raw[y * (stride + 1)];
        const src        = raw.slice(y * (stride + 1) + 1, y * (stride + 1) + 1 + stride);
        const dst        = pixels.slice(y * stride, y * stride + stride);
        const prev       = y > 0 ? pixels.slice((y-1)*stride, y*stride) : Buffer.alloc(stride);

        for (let x = 0; x < stride; x++) {
            const a  = src[x];
            const b  = prev[x];
            const c  = x < 4 ? 0 : prev[x-4];
            const pr = x < 4 ? 0 : dst[x-4];
            switch (filterByte) {
            case 0: dst[x] = a; break;
            case 1: dst[x] = (a + pr) & 0xff; break;
            case 2: dst[x] = (a + b) & 0xff; break;
            case 3: dst[x] = (a + Math.floor((pr + b) / 2)) & 0xff; break;
            case 4: dst[x] = (a + paethPredictor(pr, b, c)) & 0xff; break;
            default: throw new Error(`Unknown filter ${filterByte}`);
            }
        }
    }

    return { w, h, pixels, stride };
}

function paethPredictor(a, b, c) {
    const p  = a + b - c;
    const pa = Math.abs(p - a);
    const pb = Math.abs(p - b);
    const pc = Math.abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

function getPixel(img, x, y) {
    const off = y * img.stride + x * 4;
    return { r: img.pixels[off], g: img.pixels[off+1], b: img.pixels[off+2], a: img.pixels[off+3] };
}

function isInk(p) {
    if (p.a < 128) return false;
    /* 컬러 스프라이트 시트: 검정(윤곽선)만 PIXEL_INK, 색깔 채움은 배경 처리 */
    return Math.max(p.r, p.g, p.b) < 80;
}

/* nearest-neighbour resize region → TARGET_W×TARGET_H, returns 2D array 0=ink 1=bg */
function extractRegion(img, x, y, w, h) {
    const rows = [];
    for (let ty = 0; ty < TARGET_H; ty++) {
        const row = [];
        for (let tx = 0; tx < TARGET_W; tx++) {
            const sx = Math.floor(x + (tx + 0.5) * w / TARGET_W);
            const sy = Math.floor(y + (ty + 0.5) * h / TARGET_H);
            const p  = getPixel(img, sx, sy);
            row.push(isInk(p) ? 0 : 1);
        }
        rows.push(row);
    }
    return rows;
}

function asciiPreview(rows) {
    return rows.map(r => r.map(v => v === 0 ? '██' : '  ').join('')).join('\n');
}

function toCArray(name, rows) {
    const lines = [];
    lines.push(`int sprite_${name}[SPRITE_H][SPRITE_W] = {`);
    for (let i = 0; i < TARGET_H; i++) {
        const vals  = rows[i].join(',');
        const comma = i < TARGET_H - 1 ? ',' : '';
        lines.push(`    {${vals}}${comma}`);
    }
    lines.push(`};`);
    return lines.join('\n');
}

/* ── commands ── */
const args = process.argv.slice(2);
const cmd  = args[0];

if (!cmd || cmd === 'info') {
    const img = parsePNG(IMG_PATH);
    console.log(`Image: ${img.w} × ${img.h} RGBA`);
    // scan for pixel activity in grid cells
    console.log('\nScanning for non-empty rows every 24px (sample):');
    for (let gy = 0; gy < Math.min(img.h, 700); gy += 24) {
        let hasPixel = false;
        for (let gx = 0; gx < img.w && !hasPixel; gx++) {
            if (getPixel(img, gx, gy).a > 0) hasPixel = true;
        }
        if (hasPixel) console.log(`  row y=${gy} has content`);
    }
    process.exit(0);
}

if (cmd === 'preview') {
    const [, , x, y, w, h] = args.map((a,i) => i < 2 ? a : parseInt(a));
    const img   = parsePNG(IMG_PATH);
    const rows  = extractRegion(img, x, y, w, h);
    console.log(`Preview (${x},${y}) ${w}×${h}:`);
    console.log(asciiPreview(rows));
    process.exit(0);
}

if (cmd === 'extract') {
    const [, name, x, y, w, h] = args.map((a,i) => i < 2 ? a : parseInt(a));
    const img  = parsePNG(IMG_PATH);
    const rows = extractRegion(img, parseInt(args[2]), parseInt(args[3]), parseInt(args[4]), parseInt(args[5]));
    console.log(`/* ${name} preview:`);
    console.log(asciiPreview(rows));
    console.log('*/');
    console.log(toCArray(args[1], rows));
    process.exit(0);
}

if (cmd === 'batch') {
    const listPath = args[1];
    const lines    = fs.readFileSync(listPath, 'utf8').split('\n');
    const imgPath  = args[2] || IMG_PATH;
    const img      = parsePNG(imgPath);
    for (const line of lines) {
        const t = line.trim();
        if (!t || t.startsWith('#')) continue;
        const parts = t.split(/\s+/);
        if (parts.length !== 5) { console.error(`// skip: ${t}`); continue; }
        const [name, x, y, w, h] = parts;
        const rows = extractRegion(img, parseInt(x), parseInt(y), parseInt(w), parseInt(h));
        console.log(`/* ${name} preview:`);
        console.log(asciiPreview(rows));
        console.log('*/');
        console.log(toCArray(name, rows));
        console.log();
    }
    process.exit(0);
}

if (cmd === 'scan') {
    // scan the image for sprite boundaries by looking for non-empty regions
    const img = parsePNG(IMG_PATH);
    console.log(`Image: ${img.w}×${img.h}`);
    console.log('\nNon-empty horizontal bands (rows with any opaque pixel):');
    let inBand = false, bandStart = 0;
    for (let y = 0; y < img.h; y++) {
        let hasContent = false;
        for (let x = 0; x < img.w && !hasContent; x++) {
            if (getPixel(img, x, y).a > 0) hasContent = true;
        }
        if (hasContent && !inBand) { inBand = true; bandStart = y; }
        if (!hasContent && inBand)  { inBand = false; console.log(`  y=${bandStart}..${y-1} (h=${y-bandStart})`); }
    }
    if (inBand) console.log(`  y=${bandStart}..${img.h-1} (h=${img.h-bandStart})`);

    console.log('\nNon-empty vertical bands (cols with any opaque pixel):');
    let inVBand = false, vBandStart = 0;
    for (let x = 0; x < img.w; x++) {
        let hasContent = false;
        for (let y = 0; y < img.h && !hasContent; y++) {
            if (getPixel(img, x, y).a > 0) hasContent = true;
        }
        if (hasContent && !inVBand) { inVBand = true; vBandStart = x; }
        if (!hasContent && inVBand)  { inVBand = false; console.log(`  x=${vBandStart}..${x-1} (w=${x-vBandStart})`); }
    }
    if (inVBand) console.log(`  x=${vBandStart}..${img.w-1} (w=${img.w-vBandStart})`);
    process.exit(0);
}

console.log('Usage: node extract_sprites.js [info|preview|extract|batch|scan] ...');
