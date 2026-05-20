from __future__ import annotations

import struct
from pathlib import Path

from scripts.normalize_coff_archive_timestamps import normalize_archive


ARCHIVE_MAGIC = b"!<arch>\n"


def member_header(name: str, size: int, timestamp: str) -> bytes:
    return (
        name.encode("ascii").ljust(16, b" ")
        + timestamp.encode("ascii").ljust(12, b" ")
        + b"0     "
        + b"0     "
        + b"100644  "
        + str(size).encode("ascii").ljust(10, b" ")
        + b"`\n"
    )


def coff_member(timestamp: int) -> bytes:
    data = bytearray(20)
    struct.pack_into("<H", data, 0, 0x8664)
    struct.pack_into("<H", data, 2, 1)
    struct.pack_into("<I", data, 4, timestamp)
    return bytes(data)


def test_archive_normalizer_zeroes_archive_and_coff_member_timestamps(
    tmp_path: Path,
) -> None:
    archive = tmp_path / "sample.lib"
    first = coff_member(0x12345678)
    second = b"not-a-coff-member"
    archive.write_bytes(
        ARCHIVE_MAGIC
        + member_header("first.obj/", len(first), "1234567890")
        + first
        + member_header("second/", len(second), "9999999999")
        + second
        + (b"\n" if len(second) % 2 else b"")
    )

    member_count, coff_member_count = normalize_archive(archive)
    data = archive.read_bytes()

    assert member_count == 2
    assert coff_member_count == 1
    assert data[len(ARCHIVE_MAGIC) + 16 : len(ARCHIVE_MAGIC) + 28] == b"0           "
    first_payload = len(ARCHIVE_MAGIC) + 60
    assert struct.unpack_from("<I", data, first_payload + 4)[0] == 0
    second_header = first_payload + len(first)
    assert data[second_header + 16 : second_header + 28] == b"0           "
