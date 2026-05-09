"""JSON file writing for workflow reports."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from scripts.objc3c_shared.json_io import write_text_file

from .report_rendering import render_report_json


def write_json_report(path: Path, payload: dict[str, Any]) -> Path:
    write_text_file(path, render_report_json(payload))
    return path


__all__ = ["write_json_report"]
