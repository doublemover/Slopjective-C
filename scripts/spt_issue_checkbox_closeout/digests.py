from __future__ import annotations

import hashlib
import json
from typing import Any


def compute_source_line_hash(raw_line: str) -> str:
    digest = hashlib.sha256(raw_line.encode("utf-8")).hexdigest()
    return f"sha256:{digest}"


def compute_file_digest_bytes(data: bytes) -> str:
    digest = hashlib.sha256(data).hexdigest()
    return f"sha256:{digest}"


def normalize_source_line_hash(value: str) -> str:
    candidate = value.strip()
    if candidate.lower().startswith("sha256:"):
        return candidate.split(":", maxsplit=1)[1].lower()
    return candidate.lower()


def canonical_json(payload: dict[str, Any]) -> str:
    return json.dumps(payload, sort_keys=True, separators=(",", ":"), ensure_ascii=False)


def compute_plan_digest(payload: dict[str, Any]) -> str:
    digest = hashlib.sha256(canonical_json(payload).encode("utf-8")).hexdigest()
    return f"sha256:{digest}"
