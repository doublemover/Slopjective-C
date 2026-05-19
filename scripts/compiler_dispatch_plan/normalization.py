from __future__ import annotations

from pathlib import Path

from compiler_dispatch_plan.constants import ROOT


def normalize_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())
