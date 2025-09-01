#!/usr/bin/env python3
import argparse
import struct
import sys
from pathlib import Path

# --- Constants from firmware ---
RESET_ADDR   = 0xA00000          # &_reset
CHECK_ADDR   = 0xA8FFFE          # &rom_checksum (16-bit word, big-endian)
# -------------------------------------------------------------------

def compute_checksum(data: bytes) -> int:
    """
    Emulates the 68k routine:
        D0 = 0
        for b in range(RESET_ADDR, CHECK_ADDR):
            D0b += byte
            if carry: D0w += 0x0100
    This is equivalent to a 16-bit sum of bytes modulo 65536.
    """
    total = 0
    for b in data:
        total = (total + b) & 0xFFFF
    return total

def main():
    p = argparse.ArgumentParser(
        description="Compute/patch Dialog 4 ROM checksum."
    )
    p.add_argument("rom", type=Path, help="Path to ROM image (single, flat binary).")
    p.add_argument(
        "--base",
        type=lambda x: int(x, 0),
        default=0,
        help=("File offset that corresponds to address 0x%X (default: 0). " % RESET_ADDR),
    )
    p.add_argument(
        "--patch",
        action="store_true",
        help="Patch the computed checksum (big-endian) into the ROM image.",
    )
    p.add_argument(
        "--dry-run",
        action="store_true",
        help="Show what would be written without modifying the file.",
    )
    args = p.parse_args()

    rom_path = args.rom
    if not rom_path.is_file():
        print(f"ERROR: file not found: {rom_path}", file=sys.stderr)
        sys.exit(1)

    blob = bytearray(rom_path.read_bytes())

    # Compute file offsets from firmware addresses
    # Bytes included in the sum: [RESET_ADDR, CHECK_ADDR)
    sum_start_off = args.base + (RESET_ADDR - RESET_ADDR)  # == args.base
    sum_end_off   = args.base + (CHECK_ADDR - RESET_ADDR)  # exclusive
    chk_off       = args.base + (CHECK_ADDR - RESET_ADDR)  # location to store 16-bit BE

    # Sanity checks
    if sum_start_off < 0 or sum_end_off < 0 or chk_off < 0:
        print("ERROR: negative offset; check --base.", file=sys.stderr)
        sys.exit(1)

    if len(blob) < sum_end_off + 2:
        print(
            f"ERROR: ROM too small. Need at least 0x{sum_end_off+2: X} bytes "
            f"to read/patch checksum at file offset 0x{chk_off: X}.",
            file=sys.stderr,
        )
        sys.exit(1)

    region = bytes(blob[sum_start_off:sum_end_off])
    computed = compute_checksum(region)

    # Read currently stored checksum (big-endian 16-bit)
    stored_be = struct.unpack_from(">H", blob, chk_off)[0]

    print(f"Summed range: [file 0x{sum_start_off:06X} .. 0x{sum_end_off:06X}) "
          f"== [0x{RESET_ADDR:06X} .. 0x{CHECK_ADDR:06X})")
    print(f"Computed checksum: 0x{computed:04X}")
    print(f" Stored checksum: 0x{stored_be:04X} (big-endian at file 0x{chk_off:06X} == 0x{CHECK_ADDR:06X})")

    if args.patch:
        if args.dry_run:
            print(f"[dry-run] Would write 0x{computed:04X} (big-endian) at file 0x{chk_off:06X}")
            return

        # Make a backup
        backup = rom_path.with_suffix(rom_path.suffix + ".bak")
        if not backup.exists():
            backup.write_bytes(blob)
            print(f"Backup written: {backup.name}")
        else:
            print(f"Backup exists, not overwriting: {backup.name}")

        struct.pack_into(">H", blob, chk_off, computed)
        rom_path.write_bytes(blob)
        print(f"Patched checksum 0x{computed:04X} at file offset 0x{chk_off:06X}.")

if __name__ == "__main__":
    main()
