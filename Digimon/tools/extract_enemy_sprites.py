"""
Enemy Digimon sprite extractor
이미지에서 적군 디지몬 스프라이트를 잘라내어 저장합니다.

사용법:
  python extract_enemy_sprites.py <이미지경로> [--debug]

  --debug  : 추출 영역을 이미지에 표시해서 debug_preview.png 저장 (위치 확인용)

조정이 필요한 경우 아래 PARAMS 섹션을 수정하세요.
"""

import sys
import os
from PIL import Image, ImageDraw

# ─────────────────────────────────────────────
# PARAMS  (이미지 실측 후 조정)
# ─────────────────────────────────────────────

# 한 스프라이트 셀 크기 (테두리 포함)
CELL_W = 38
CELL_H = 38

# 왼쪽 컬럼: 첫 번째 스프라이트 좌상단 (x, y)
LEFT_X0  = 283
LEFT_Y0  = 82

# 오른쪽 컬럼: 첫 번째 스프라이트 좌상단
RIGHT_X0 = 874
RIGHT_Y0 = 82

# 같은 캐릭터의 4개 프레임 간 x 간격
FRAME_GAP_X = 40   # 셀 너비보다 약간 클 수 있음

# 캐릭터 행 간격 (y)
ROW_GAP_Y   = 72

# ─────────────────────────────────────────────
# 이름 목록  (파일명용, 순서 = 이미지 위→아래)
# ─────────────────────────────────────────────

LEFT_NAMES = [
    "imperialdramon_paladin",
    "dukemon",
    "magnamon",
    "dynasmon",
    "lord_knightmon",
    "ulforce_vdramon",
    "craniummon",
    "sleipmon",
    "duftmon",
    "gankoomon",
]

RIGHT_NAMES = [
    "lucemon_falldown",
    "leviamon",
    "beelzebumon",
    "lilithmon",
    "belphemon_rage",
    "barbamon",
    "demon",
    "belial_vamdemon",
    "murmukusmon",
    "deathmon",
]

FRAME_NAMES = ["idle1", "idle2", "atk", "hit"]

# ─────────────────────────────────────────────

def extract(img_path: str, debug: bool = False):
    img = Image.open(img_path).convert("RGB")
    out_dir = os.path.join(os.path.dirname(img_path), "enemy_sprites")
    os.makedirs(out_dir, exist_ok=True)

    draw = ImageDraw.Draw(img) if debug else None
    colors = ["#FF0000", "#00CC00", "#0080FF", "#FF8800"]

    count = 0
    for names, x0, y0 in [(LEFT_NAMES, LEFT_X0, LEFT_Y0),
                           (RIGHT_NAMES, RIGHT_X0, RIGHT_Y0)]:
        for row, name in enumerate(names):
            y = y0 + row * ROW_GAP_Y
            for fi, frame in enumerate(FRAME_NAMES):
                x = x0 + fi * FRAME_GAP_X
                box = (x, y, x + CELL_W, y + CELL_H)
                sprite = img.crop(box)
                out_path = os.path.join(out_dir, f"{name}_{frame}.png")
                sprite.save(out_path)
                count += 1
                if debug:
                    draw.rectangle(box, outline=colors[fi], width=2)

    if debug:
        preview_path = os.path.join(os.path.dirname(img_path), "debug_preview.png")
        img.save(preview_path)
        print(f"Debug preview saved: {preview_path}")

    print(f"Done. {count} sprites saved to: {out_dir}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python extract_enemy_sprites.py <image_path> [--debug]")
        sys.exit(1)
    img_path = sys.argv[1]
    debug = "--debug" in sys.argv
    extract(img_path, debug)
