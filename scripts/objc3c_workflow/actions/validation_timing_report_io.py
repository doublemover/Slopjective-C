"""Validation timing report path lookup and JSON loading."""

from __future__ import annotations

import json
from collections.abc import Iterator, Sequence
from pathlib import Path

from ..environment import ROOT


def iter_step_report_paths(
    steps: Sequence[dict[str, object]],
) -> Iterator[tuple[dict[str, object], str]]:
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if isinstance(raw_path, str):
                yield step, raw_path


def load_json_report(raw_path: str) -> dict[str, object] | None:
    return load_report_path(ROOT / raw_path)


def load_report_path(path: Path | None) -> dict[str, object] | None:
    if path is None or not path.is_file():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    return payload if isinstance(payload, dict) else None


def load_surface_from_report(
    steps: Sequence[dict[str, object]], surface_key: str
) -> dict[str, object] | None:
    for _, raw_path in iter_step_report_paths(steps):
        payload = load_json_report(raw_path)
        if payload is None:
            continue
        surface = payload.get(surface_key)
        if isinstance(surface, dict):
            return surface
    return None


def latest_json_file(root: Path) -> Path | None:
    if root.is_file():
        return root
    if not root.exists():
        return None
    candidates = sorted(
        (candidate for candidate in root.rglob("*.json") if candidate.is_file()),
        key=lambda candidate: candidate.stat().st_mtime,
        reverse=True,
    )
    return candidates[0] if candidates else None


def relative_path_or_none(path: Path | None) -> str | None:
    if path is None:
        return None
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


def load_latest_report_payload(path: Path | None) -> dict[str, object] | None:
    return load_report_path(path)
