"""Checksum and replay-key helpers for runtime acceptance."""

from __future__ import annotations

import hashlib
import re
from pathlib import Path


def file_sha256_hex(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def optional_file_sha256_hex(path: Path | None) -> str:
    if path is None or not path.is_file():
        return ""
    return file_sha256_hex(path)


def sha256_text_hex(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def replay_key_counter(replay_key: str, counter_name: str) -> int:
    if replay_key == "" or counter_name == "":
        return 0
    match = re.search(re.escape(counter_name) + r"=([0-9]+)", replay_key)
    if match is None:
        return 0
    return int(match.group(1))
