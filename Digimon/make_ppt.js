const pptxgen = require("pptxgenjs");

let pres = new pptxgen();
pres.layout = "LAYOUT_16x9";
pres.title = "디지몽";

const C_DARK     = "0F0E17";
const C_DARK_CARD = "1E1E2E";
const C_ORANGE   = "FF6B2B";
const C_WHITE    = "FFFFFF";
const C_LIGHT_BG = "F8F9FA";
const C_GRAY     = "6B7280";
const C_GREEN    = "10B981";

// ──────────────────────────────────────────────
// Slide 1: Title
// ──────────────────────────────────────────────
{
  let s = pres.addSlide();
  s.background = { color: C_DARK };

  s.addText("디지몽", {
    x: 0.5, y: 1.0, w: 9, h: 1.8,
    fontSize: 80, bold: true, color: C_ORANGE,
    align: "center", fontFace: "Consolas"
  });

  s.addText("C언어 콘솔 디지몬 육성 시뮬레이션", {
    x: 0.5, y: 3.0, w: 9, h: 0.55,
    fontSize: 20, color: "BBBBCC", align: "center", fontFace: "Calibri"
  });

  s.addText("Ver.20th 디지바이스를 PC 터미널에서 재현", {
    x: 0.5, y: 3.65, w: 9, h: 0.45,
    fontSize: 15, color: "666677", align: "center", fontFace: "Calibri"
  });

  s.addText("2026  ·  PC Console  ·  C Language  ·  1인 개발", {
    x: 0.5, y: 5.0, w: 9, h: 0.35,
    fontSize: 12, color: "444455", align: "center", fontFace: "Calibri"
  });
}

// ──────────────────────────────────────────────
// Slide 2: 게임 개요
// ──────────────────────────────────────────────
{
  let s = pres.addSlide();
  s.background = { color: C_LIGHT_BG };

  s.addText("게임 개요", {
    x: 0.5, y: 0.3, w: 9, h: 0.65,
    fontSize: 32, bold: true, color: C_DARK, fontFace: "Calibri"
  });

  // Left card — basic info
  s.addShape(pres.shapes.RECTANGLE, {
    x: 0.4, y: 1.1, w: 4.3, h: 4.0,
    fill: { color: C_WHITE },
    shadow: { type: "outer", blur: 8, offset: 2, angle: 135, color: "000000", opacity: 0.08 }
  });

  s.addText("기본 정보", {
    x: 0.6, y: 1.25, w: 3.9, h: 0.4,
    fontSize: 14, bold: true, color: C_GRAY, fontFace: "Calibri"
  });

  const info = [
    ["장르",   "육성 시뮬레이션"],
    ["플랫폼", "PC 콘솔 터미널"],
    ["언어",   "C  (Visual Studio / MSVC)"],
    ["조작",   "Z / X / C  3버튼"],
    ["목표",   "최종 진화 달성 (자유 플레이)"],
  ];
  info.forEach(([label, val], i) => {
    s.addText(label, {
      x: 0.6, y: 1.82 + i * 0.6, w: 1.3, h: 0.45,
      fontSize: 13, bold: true, color: C_GRAY, fontFace: "Calibri"
    });
    s.addText(val, {
      x: 2.0, y: 1.82 + i * 0.6, w: 2.5, h: 0.45,
      fontSize: 13, color: C_DARK, fontFace: "Calibri"
    });
  });

  // Right card — concept (dark)
  s.addShape(pres.shapes.RECTANGLE, {
    x: 5.1, y: 1.1, w: 4.4, h: 4.0,
    fill: { color: C_DARK },
    shadow: { type: "outer", blur: 8, offset: 2, angle: 135, color: "000000", opacity: 0.15 }
  });

  s.addText("핵심 콘셉트", {
    x: 5.3, y: 1.25, w: 4.0, h: 0.4,
    fontSize: 14, bold: true, color: C_ORANGE, fontFace: "Calibri"
  });

  const concepts = [
    "알에서 시작해 궁극체까지 육성",
    "오프라인 경과 시간도 실시간 반영",
    "케어 방식에 따라 진화 루트가 달라짐",
    "방치하면 죽는다 — 긴장감 있는 육성",
  ];
  concepts.forEach((text, i) => {
    s.addText("▸  " + text, {
      x: 5.3, y: 1.85 + i * 0.75, w: 4.0, h: 0.6,
      fontSize: 13, color: "CCCCDD", fontFace: "Calibri"
    });
  });
}

// ──────────────────────────────────────────────
// Slide 3: 핵심 시스템
// ──────────────────────────────────────────────
{
  let s = pres.addSlide();
  s.background = { color: C_LIGHT_BG };

  s.addText("핵심 시스템", {
    x: 0.5, y: 0.3, w: 9, h: 0.65,
    fontSize: 32, bold: true, color: C_DARK, fontFace: "Calibri"
  });

  const systems = [
    {
      title: "수치 관리",
      color: "2563EB",
      items: ["배고픔 (0~4)", "근력 (0~4)", "체중 / 나이", "DP (배틀 파워)", "케어미스 카운트"],
      sub: "10분 무응답 → 케어미스"
    },
    {
      title: "진화 시스템",
      color: C_ORANGE,
      items: ["18마리 디지몬", "EGG → MEGA  7단계", "케어미스 · 노력치 · 과식", "조건별 12개 분기 루트"],
      sub: "총 12개 진화 경로"
    },
    {
      title: "방치 패널티",
      color: "DC2626",
      items: ["오프라인 시간 자동 반영", "배고픔 0 → 12시간 후 사망", "케어미스 20회 → 사망", "똥 4개 → 부상 발생"],
      sub: "껐다 켜도 시간은 흐른다"
    },
  ];

  systems.forEach((sys, i) => {
    const cx = 0.35 + i * 3.12;

    s.addShape(pres.shapes.RECTANGLE, {
      x: cx, y: 1.05, w: 2.95, h: 4.2,
      fill: { color: C_WHITE },
      shadow: { type: "outer", blur: 10, offset: 3, angle: 135, color: "000000", opacity: 0.1 }
    });

    // Top color bar
    s.addShape(pres.shapes.RECTANGLE, {
      x: cx, y: 1.05, w: 2.95, h: 0.48,
      fill: { color: sys.color }
    });

    s.addText(sys.title, {
      x: cx, y: 1.05, w: 2.95, h: 0.48,
      fontSize: 15, bold: true, color: C_WHITE,
      align: "center", margin: 0, valign: "middle", fontFace: "Calibri"
    });

    sys.items.forEach((item, j) => {
      s.addText([
        { text: "·  ", options: { color: sys.color, bold: true } },
        { text: item, options: { color: "374151" } }
      ], {
        x: cx + 0.15, y: 1.65 + j * 0.54, w: 2.65, h: 0.44,
        fontSize: 12.5, fontFace: "Calibri"
      });
    });

    // Bottom tag
    s.addShape(pres.shapes.RECTANGLE, {
      x: cx + 0.1, y: 4.73, w: 2.75, h: 0.35,
      fill: { color: sys.color, transparency: 85 }
    });
    s.addText(sys.sub, {
      x: cx + 0.1, y: 4.73, w: 2.75, h: 0.35,
      fontSize: 11, color: sys.color, align: "center",
      bold: true, margin: 0, valign: "middle", fontFace: "Calibri"
    });
  });
}

// ──────────────────────────────────────────────
// Slide 4: 시스템 아키텍처
// ──────────────────────────────────────────────
{
  let s = pres.addSlide();
  s.background = { color: C_LIGHT_BG };

  s.addText("시스템 아키텍처", {
    x: 0.5, y: 0.3, w: 9, h: 0.65,
    fontSize: 32, bold: true, color: C_DARK, fontFace: "Calibri"
  });

  // Left — file structure (dark terminal)
  s.addShape(pres.shapes.RECTANGLE, {
    x: 0.4, y: 1.1, w: 4.1, h: 4.1,
    fill: { color: C_DARK_CARD }
  });

  s.addText("파일 구조", {
    x: 0.6, y: 1.22, w: 3.7, h: 0.38,
    fontSize: 13, bold: true, color: C_ORANGE, fontFace: "Consolas"
  });

  const files = [
    ["main.c",       "게임 루프 · 입력 처리"],
    ["digimon.c/h",  "수치 · 진화 · 사망 로직"],
    ["ui.c/h",       "콘솔 렌더링"],
    ["sprites.c/h",  "픽셀 스프라이트 데이터"],
    ["anim.c/h",     "애니메이션 제어"],
  ];

  files.forEach(([file, desc], i) => {
    s.addText(file, {
      x: 0.6, y: 1.75 + i * 0.64, w: 1.8, h: 0.4,
      fontSize: 12, bold: true, color: "00FF88", fontFace: "Consolas"
    });
    s.addText(desc, {
      x: 2.45, y: 1.75 + i * 0.64, w: 1.85, h: 0.4,
      fontSize: 11.5, color: "AABBCC", fontFace: "Calibri"
    });
  });

  // Right — game loop
  s.addShape(pres.shapes.RECTANGLE, {
    x: 5.0, y: 1.1, w: 4.5, h: 4.1,
    fill: { color: C_WHITE },
    shadow: { type: "outer", blur: 8, offset: 2, angle: 135, color: "000000", opacity: 0.08 }
  });

  s.addText("게임 루프 흐름  (매 1초 틱)", {
    x: 5.15, y: 1.22, w: 4.2, h: 0.38,
    fontSize: 13, bold: true, color: C_DARK, fontFace: "Calibri"
  });

  const loop = [
    { label: "입력 감지",       desc: "Z · X · C 키",              color: "7C3AED" },
    { label: "update_status",   desc: "배고픔 · 근력 · 똥 갱신",    color: "2563EB" },
    { label: "check_call",      desc: "콜 발생 · 케어미스 판정",     color: "0891B2" },
    { label: "check_evolution", desc: "진화 조건 확인 → 부화",       color: C_ORANGE },
    { label: "check_death",     desc: "사망 조건 확인",              color: "DC2626" },
    { label: "렌더링",          desc: "스프라이트 · 메뉴 출력",      color: "059669" },
  ];

  loop.forEach((step, i) => {
    s.addShape(pres.shapes.RECTANGLE, {
      x: 5.15, y: 1.72 + i * 0.55, w: 0.07, h: 0.38,
      fill: { color: step.color }
    });
    s.addText(step.label, {
      x: 5.28, y: 1.72 + i * 0.55, w: 1.9, h: 0.38,
      fontSize: 11.5, bold: true, color: "1F2937",
      fontFace: "Consolas", valign: "middle"
    });
    s.addText(step.desc, {
      x: 7.2, y: 1.72 + i * 0.55, w: 2.15, h: 0.38,
      fontSize: 11, color: C_GRAY, fontFace: "Calibri", valign: "middle"
    });
  });
}

// ──────────────────────────────────────────────
// Slide 5: 구현 현황
// ──────────────────────────────────────────────
{
  let s = pres.addSlide();
  s.background = { color: C_LIGHT_BG };

  s.addText("구현 현황", {
    x: 0.5, y: 0.3, w: 9, h: 0.65,
    fontSize: 32, bold: true, color: C_DARK, fontFace: "Calibri"
  });

  // Done
  s.addShape(pres.shapes.RECTANGLE, {
    x: 0.4, y: 1.1, w: 4.3, h: 4.1,
    fill: { color: C_WHITE },
    shadow: { type: "outer", blur: 8, offset: 2, angle: 135, color: "000000", opacity: 0.08 }
  });
  s.addShape(pres.shapes.RECTANGLE, {
    x: 0.4, y: 1.1, w: 4.3, h: 0.45,
    fill: { color: C_GREEN }
  });
  s.addText("구현 완료", {
    x: 0.4, y: 1.1, w: 4.3, h: 0.45,
    fontSize: 14, bold: true, color: C_WHITE,
    align: "center", margin: 0, valign: "middle", fontFace: "Calibri"
  });

  const done = [
    "수치 시스템 (배고픔 · 근력 · 체중 · 나이 · DP)",
    "케어미스 & 콜 알림 시스템",
    "진화 트리 — 18마리 · 12개 루트",
    "수면 · 부상 · 사망 조건",
    "픽셀 스프라이트 렌더링",
    "부화 애니메이션",
  ];
  done.forEach((item, i) => {
    s.addText([
      { text: "✓  ", options: { color: C_GREEN, bold: true } },
      { text: item, options: { color: "374151" } }
    ], {
      x: 0.55, y: 1.68 + i * 0.55, w: 3.98, h: 0.45,
      fontSize: 12.5, fontFace: "Calibri"
    });
  });

  // Planned
  s.addShape(pres.shapes.RECTANGLE, {
    x: 5.2, y: 1.1, w: 4.3, h: 4.1,
    fill: { color: C_WHITE },
    shadow: { type: "outer", blur: 8, offset: 2, angle: 135, color: "000000", opacity: 0.08 }
  });
  s.addShape(pres.shapes.RECTANGLE, {
    x: 5.2, y: 1.1, w: 4.3, h: 0.45,
    fill: { color: "94A3B8" }
  });
  s.addText("구현 예정", {
    x: 5.2, y: 1.1, w: 4.3, h: 0.45,
    fontSize: 14, bold: true, color: C_WHITE,
    align: "center", margin: 0, valign: "middle", fontFace: "Calibri"
  });

  const planned = [
    "밥주기 · 훈련 · 청소 · 치료 (C키 동작)",
    "상태보기 화면",
    "저장 / 불러오기 (오프라인 시간 반영)",
    "전투 시스템",
    "도감",
  ];
  planned.forEach((item, i) => {
    s.addText([
      { text: "○  ", options: { color: "94A3B8", bold: true } },
      { text: item, options: { color: "6B7280" } }
    ], {
      x: 5.35, y: 1.68 + i * 0.65, w: 3.98, h: 0.55,
      fontSize: 12.5, fontFace: "Calibri"
    });
  });
}

// ──────────────────────────────────────────────
// Slide 6: Demo / 마무리
// ──────────────────────────────────────────────
{
  let s = pres.addSlide();
  s.background = { color: C_DARK };

  s.addText("DEMO", {
    x: 0.5, y: 1.2, w: 9, h: 1.5,
    fontSize: 72, bold: true, color: C_ORANGE,
    align: "center", fontFace: "Consolas"
  });

  s.addText("프로토타입 시연", {
    x: 0.5, y: 2.85, w: 9, h: 0.55,
    fontSize: 22, color: "BBBBCC", align: "center", fontFace: "Calibri"
  });

  s.addText("EGG → BABY1 → BABY2  진화 확인 가능", {
    x: 0.5, y: 3.55, w: 9, h: 0.45,
    fontSize: 15, color: "666677", align: "center", fontFace: "Calibri"
  });

  s.addText("수치 시스템 · 스프라이트 렌더링 · 부화 애니메이션", {
    x: 0.5, y: 4.1, w: 9, h: 0.4,
    fontSize: 13, color: "555566", align: "center", fontFace: "Calibri"
  });
}

pres.writeFile({ fileName: "C:\\Workspace\\2026_s1_04_prog_lab\\Digimon\\디지몽_발표자료.pptx" })
  .then(() => console.log("Done: 디지몽_발표자료.pptx"))
  .catch(e => console.error(e));
