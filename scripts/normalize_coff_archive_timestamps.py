#!/usr/bin/env python3
"""Normalize COFF archive and member object timestamps in-place."""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path
from typing import Sequence

ARCHIVE_MAGIC = b"!<arch>\n"
HEADER_SIZE = 60
HEADER_END = b"`\n"
COFF_MACHINES = {0x014C, 0x8664, 0xAA64}


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path)
    return parser.parse_args(argv)


def normalized_header(header: bytes) -> bytes:
    if len(header) != HEADER_SIZE or header[-2:] != HEADER_END:
        raise ValueError("invalid archive member header")
    return header[:16] + b"0           " + header[28:]


def member_size(header: bytes) -> int:
    raw_size = header[48:58].decode("ascii", errors="strict").strip()
    if not raw_size.isdigit():
        raise ValueError(f"invalid archive member size: {raw_size!r}")
    return int(raw_size)


def normalize_coff_member_timestamp(data: bytearray, start: int, size: int) -> bool:
    if size < 20:
        return False
    machine = struct.unpack_from("<H", data, start)[0]
    section_count = struct.unpack_from("<H", data, start + 2)[0]
    if machine not in COFF_MACHINES or section_count == 0:
        return False
    data[start + 4 : start + 8] = b"\0\0\0\0"
    return True


def normalize_archive(path: Path) -> tuple[int, int]:
    data = bytearray(path.read_bytes())
    if not data.startswith(ARCHIVE_MAGIC):
        raise ValueError(f"not a COFF archive: {path}")

    member_count = 0
    coff_member_count = 0
    offset = len(ARCHIVE_MAGIC)
    while offset < len(data):
        if offset + HEADER_SIZE > len(data):
            raise ValueError(f"truncated archive member header at offset {offset}")
        header = bytes(data[offset : offset + HEADER_SIZE])
        size = member_size(header)
        data[offset : offset + HEADER_SIZE] = normalized_header(header)
        payload_start = offset + HEADER_SIZE
        payload_end = payload_start + size
        if payload_end > len(data):
            raise ValueError(f"truncated archive member payload at offset {offset}")
        member_count += 1
        if normalize_coff_member_timestamp(data, payload_start, size):
            coff_member_count += 1
        offset = payload_end + (size % 2)

    path.write_bytes(data)
    return member_count, coff_member_count


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    member_count, coff_member_count = normalize_archive(args.archive)
    print(
        "[ok] normalized COFF archive timestamps: "
        f"{args.archive} members={member_count} coff_members={coff_member_count}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
