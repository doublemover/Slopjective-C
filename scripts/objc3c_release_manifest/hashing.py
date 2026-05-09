"""Hash helpers for release manifest artifacts."""

from __future__ import annotations

import hashlib
from collections.abc import Iterable
from pathlib import Path
from typing import Any


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def sha256_joined_records(records: Iterable[tuple[Any, ...]]) -> str:
    payload = "".join(
        "|".join(str(field) for field in record) + "\n"
        for record in records
    )
    return sha256_text(payload)


def release_payload_digest(entries: Iterable[Any]) -> str:
    return sha256_joined_records(
        (
            entry.path,
            entry.sha256,
            entry.byte_count,
            entry.component_group,
        )
        for entry in entries
    )
