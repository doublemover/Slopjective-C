"""Digest helpers for deterministic replay evidence."""

from __future__ import annotations

import hashlib


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


__all__ = ["sha256_bytes"]
