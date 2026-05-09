"""JSON file writing for workflow reports."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from scripts.objc3c_shared.json_io import write_json_file


def write_json_report(path: Path, payload: dict[str, Any]) -> Path:
    write_json_file(path, payload)
    return path


__all__ = ["write_json_report"]
