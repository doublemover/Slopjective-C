from __future__ import annotations

import hashlib

from .constants import OUTPUT_PATH


def read_output_bytes() -> bytes:
    return OUTPUT_PATH.read_bytes() if OUTPUT_PATH.is_file() else b""


def write_output_bytes(data: bytes) -> None:
    OUTPUT_PATH.write_bytes(data)


def output_digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()[:16]
