#!/usr/bin/env python3
"""
generate_font_png.py – Render MC6845 character ROMs into PNGs.

New in v2:
  * --labelled-grid  : Creates a 16×16 tile sheet (first 256 glyphs) with a
    surrounding header that labels rows/columns 0‑F (hex high‑/low‑nibbles).
    Great for quickly spotting glyph codes.

General usage
-------------
$ python generate_font_png.py FONT.ROM out.png

Optional flags:
  --width   Glyph width   (default 8)
  --height  Glyph height  (default 8)
  --cols    Glyphs/row    (default 16)
  --labelled-grid        Produce labelled 16×16 sheet (ignores --cols)

Examples
--------
# Plain 8×8 font, default layout
$ python generate_font_png.py font.bin font.png

# 8×16 font, wider sheet (32 glyphs/row)
$ python generate_font_png.py font.bin font32.png --height 16 --cols 32

# Labelled reference sheet of first 256 glyphs
$ python generate_font_png.py font.bin sheet.png --height 16 --labelled-grid
"""

import argparse
import math
from pathlib import Path
from typing import Sequence

from PIL import Image, ImageDraw, ImageFont  # pip install pillow

DEFAULT_COLS = 16
LABELLED_LIMIT = 256
GRID_THICKNESS = 1


def render_plain(rom_bytes: bytes, char_width: int, char_height: int, cols: int) -> Image.Image:
    """Return a plain tiled image with no labels."""
    bytes_per_char = char_height
    num_chars = len(rom_bytes) // bytes_per_char
    rows = math.ceil(num_chars / cols)

    img = Image.new("RGB", (cols * char_width, rows * char_height), (255, 255, 255))

    for idx in range(num_chars):
        glyph = rom_bytes[idx * bytes_per_char : (idx + 1) * bytes_per_char]
        cx = (idx % cols) * char_width
        cy = (idx // cols) * char_height
        _blit_glyph(img, glyph, cx, cy, char_width, char_height)

    return img


def render_labelled(rom_bytes: bytes, char_width: int, char_height: int) -> Image.Image:
    """Render first 256 glyphs into a 16×16 grid with hex nibble headers."""
    cols = rows = 16
    bytes_per_char = char_height
    needed = LABELLED_LIMIT * bytes_per_char
    if len(rom_bytes) < needed:
        raise ValueError("ROM too small for 256 glyphs – need at least {} bytes.".format(needed))

    grid = GRID_THICKNESS
    cell_w = char_width + grid
    cell_h = char_height + grid
    w = (cols + 1) * cell_w + grid
    h = (rows + 1) * cell_h + grid
    img = Image.new("RGB", (w, h), (255, 255, 255))
    draw = ImageDraw.Draw(img)

    font = ImageFont.load_default()

    _draw_grid(draw, w, h, cell_w, cell_h, cols, rows, line_color=(255, 0, 0))

    for col in range(cols):
        x_center = (col + 1) * cell_w + char_width // 2
        _center_text(draw, x_center, grid + char_height // 2, f"{col:X}", font)

    for row in range(rows):
        y_center = (row + 1) * cell_h + char_height // 2
        _center_text(draw, grid + char_width // 2, y_center, f"{row:X}", font)

    for idx in range(LABELLED_LIMIT):
        glyph = rom_bytes[idx * bytes_per_char : (idx + 1) * bytes_per_char]
        cx = (idx % cols + 1) * cell_w + grid
        cy = (idx // cols + 1) * cell_h + grid
        _blit_glyph(img, glyph, cx, cy, char_width, char_height)

    return img


def _blit_glyph(img: Image.Image, glyph: bytes, x0: int, y0: int, char_width: int, char_height: int) -> None:
    for y, byte in enumerate(glyph):
        if y + y0 >= img.height:
            continue
        for x in range(char_width):
            if x + x0 >= img.width:
                continue
            if byte & (1 << (7 - x)):
                img.putpixel((x0 + x, y0 + y), (0, 0, 0))


def _center_text(draw: ImageDraw.ImageDraw, x: int, y: int, text: str, font: ImageFont.ImageFont) -> None:
    bbox = font.getbbox(text)
    w = bbox[2] - bbox[0]
    h = bbox[3] - bbox[1]
    draw.text((x - w // 2, y - h // 2), text, font=font, fill=(0, 0, 0))


def _draw_grid(draw: ImageDraw.ImageDraw, w: int, h: int, cw: int, ch: int, cols: int, rows: int, line_color=(0, 0, 0)) -> None:
    for col in range(cols + 2):
        x = col * cw
        draw.line([(x, 0), (x, h)], fill=line_color)
    for row in range(rows + 2):
        y = row * ch
        draw.line([(0, y), (w, y)], fill=line_color)


def main(argv: Sequence[str] | None = None) -> None:
    parser = argparse.ArgumentParser(description="Render MC6845 font ROMs to PNG.")
    parser.add_argument("rom", type=Path, help="Path to raw ROM binary dump")
    parser.add_argument("png", type=Path, help="Output PNG path")
    parser.add_argument("--width", type=int, default=8, help="Glyph width (px)")
    parser.add_argument("--height", type=int, default=8, help="Glyph height (px)")
    parser.add_argument("--cols", type=int, default=DEFAULT_COLS, help="Glyphs per row in plain mode")
    parser.add_argument("--labelled-grid", action="store_true", help="Generate 16×16 sheet with hex headers")

    args = parser.parse_args(argv)

    data = args.rom.read_bytes()
    if len(data) % args.height != 0:
        raise ValueError("ROM size ({} bytes) is not a multiple of char height ({}).".format(len(data), args.height))

    if args.labelled_grid:
        img = render_labelled(data, args.width, args.height)
    else:
        img = render_plain(data, args.width, args.height, args.cols)

    img.save(args.png)
    print(f"Saved {args.png} ({img.width}×{img.height})")


if __name__ == "__main__":
    main()
