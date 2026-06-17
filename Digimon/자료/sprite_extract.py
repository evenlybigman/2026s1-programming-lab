"""
sprite_extract.py — Digital Monster Ver.20th 스프라이트 추출기

사용법:
  단일 추출:
    python sprite_extract.py <이미지> <이름> <x> <y> <너비> <높이>

  배치 추출 (스프라이트 목록 파일):
    python sprite_extract.py <이미지> --batch <목록.txt>

목록 파일 형식 (한 줄 = 스프라이트 하나):
  agumon_1  120  45  24  24
  agumon_2  148  45  24  24
  ...

출력: sprites.c 에 바로 붙여넣을 수 있는 C 배열 코드
"""
import sys
from PIL import Image

TARGET_W = 16
TARGET_H = 16

def pixel_to_ink(r, g, b, a):
    """픽셀 → 0(PIXEL_INK) 또는 1(PIXEL_BG)
    투명(alpha < 128) → 배경(1), 불투명 → 잉크(0)"""
    return 1 if a < 128 else 0


def extract(img, name, x, y, w, h):
    region = img.crop((x, y, x + w, y + h))
    region = region.resize((TARGET_W, TARGET_H), Image.NEAREST)
    region = region.convert('RGBA')

    rows = []
    for row in range(TARGET_H):
        cols = []
        for col in range(TARGET_W):
            r, g, b, a = region.getpixel((col, row))
            cols.append(pixel_to_ink(r, g, b, a))
        rows.append(cols)

    # ASCII 미리보기 (터미널 확인용)
    print(f"/* {name} — 미리보기:")
    for row in rows:
        print("   " + "".join("██" if p == 0 else "  " for p in row))
    print("*/")

    # C 배열 출력
    print(f"int sprite_{name}[SPRITE_H][SPRITE_W] = {{")
    for i, row in enumerate(rows):
        comma = "," if i < TARGET_H - 1 else ""
        vals = ",".join(str(p) for p in row)
        print(f"    {{{vals}}}{comma}")
    print("};")
    print()


def main():
    args = sys.argv[1:]

    if len(args) == 0:
        print(__doc__)
        sys.exit(0)

    img_path = args[0]
    try:
        img = Image.open(img_path)
    except Exception as e:
        print(f"이미지 열기 실패: {e}")
        sys.exit(1)

    # 배치 모드
    if len(args) >= 2 and args[1] == "--batch":
        if len(args) < 3:
            print("배치 파일 경로가 필요합니다.")
            sys.exit(1)
        with open(args[2], encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                parts = line.split()
                if len(parts) != 5:
                    print(f"형식 오류 (건너뜀): {line}")
                    continue
                name, x, y, w, h = parts[0], int(parts[1]), int(parts[2]), int(parts[3]), int(parts[4])
                extract(img, name, x, y, w, h)
        return

    # 단일 모드
    if len(args) != 6:
        print("사용법: python sprite_extract.py <이미지> <이름> <x> <y> <너비> <높이>")
        sys.exit(1)

    _, name, x, y, w, h = args
    extract(img, name, int(x), int(y), int(w), int(h))


if __name__ == "__main__":
    main()
