"""Path ownership for the remaining-task extraction CLI."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import ROOT

DEFAULT_INPUT = ROOT / "tmp" / "reports" / "remaining_task_review_catalog.json"

__all__ = [
    "DEFAULT_INPUT",
    "ROOT",
    "resolve_input_path",
]


def resolve_input_path(raw_input: Path) -> Path:
    if raw_input.is_absolute():
        return raw_input
    return ROOT / raw_input
