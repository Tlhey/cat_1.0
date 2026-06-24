#!/usr/bin/env python3
"""
preview_cat_slots.py
Overlay a numbered grid on a sprite-sheet PNG so you can identify frame positions.

Usage:
  python tools/preview_cat_slots.py \
      --input  "raw_assets/Free pack/cat 1.png" \
      --slot   32 \
      --output tools/preview_slots.png
"""

import argparse
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont


def preview(input_path: str, slot: int, output_path: str) -> None:
    src = Image.open(input_path).convert("RGBA")
    w, h = src.size
    cols = w // slot
    rows = h // slot
    print(f"Sheet: {w}×{h}  →  {cols} cols × {rows} rows  (slot={slot})")

    # White background so transparent areas show as white
    bg = Image.new("RGBA", (w, h), (255, 255, 255, 255))
    bg.paste(src, mask=src.split()[3])
    out = bg.convert("RGB")

    draw = ImageDraw.Draw(out)

    # Try to load a small font; fall back to default if unavailable.
    font = None
    for path in ["/System/Library/Fonts/Helvetica.ttc",
                 "/System/Library/Fonts/Arial.ttf",
                 "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"]:
        try:
            font = ImageFont.truetype(path, 9)
            break
        except Exception:
            pass
    if font is None:
        font = ImageFont.load_default()

    for row in range(rows):
        for col in range(cols):
            x0, y0 = col * slot, row * slot
            x1, y1 = x0 + slot - 1, y0 + slot - 1
            # Grid line
            draw.rectangle([x0, y0, x1, y1], outline=(200, 60, 60), width=1)
            # Label "r,c"
            draw.text((x0 + 2, y0 + 2), f"{row},{col}", fill=(200, 60, 60), font=font)

    out.save(output_path)
    print(f"Saved: {output_path}  ({cols}×{rows} grid)")


def main():
    ap = argparse.ArgumentParser(description="Sprite-sheet grid preview")
    ap.add_argument("--input",  required=True)
    ap.add_argument("--slot",   type=int, default=32)
    ap.add_argument("--output", required=True)
    args = ap.parse_args()
    preview(args.input, args.slot, args.output)


if __name__ == "__main__":
    main()
