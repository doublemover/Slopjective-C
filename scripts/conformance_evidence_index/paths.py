from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any, Sequence

from conformance_evidence_index.constants import ROOT


def normalize_repo_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def resolve_repo_path(raw_path: str) -> Path:
    candidate = Path(raw_path)
    if not candidate.is_absolute():
        candidate = ROOT / candidate
    return candidate.resolve()


def normalize_pattern_list(
    patterns: Sequence[str] | None, defaults: Sequence[str]
) -> list[str]:
    if not patterns:
        return list(defaults)
    normalized = [pattern.strip() for pattern in patterns if pattern.strip()]
    return normalized or list(defaults)


def detect_media_type(path: Path) -> str:
    suffix = path.suffix.lower()
    if suffix == ".json":
        return "application/json"
    if suffix == ".md":
        return "text/markdown"
    if suffix == ".txt":
        return "text/plain"
    return "application/octet-stream"


def file_sha256(path: Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(131072), b""):
            hasher.update(chunk)
    return f"sha256:{hasher.hexdigest()}"


def load_json_object(path: Path) -> dict[str, Any] | None:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError):
        return None

    if isinstance(payload, dict):
        return payload
    return None


def path_sort_key(path_string: str) -> str:
    return path_string.casefold()


def collect_artifact_paths(
    *,
    input_root: Path,
    globs: Sequence[str],
    excluded_paths: set[Path],
) -> list[Path]:
    paths: set[Path] = set()
    for pattern in globs:
        for candidate in input_root.glob(pattern):
            resolved = candidate.resolve()
            if not candidate.is_file():
                continue
            if resolved in excluded_paths:
                continue
            paths.add(resolved)
    return sorted(paths, key=lambda candidate: path_sort_key(normalize_repo_path(candidate)))
