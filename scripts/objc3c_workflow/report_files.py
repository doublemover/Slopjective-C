"""JSON file writing for workflow reports."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from scripts.objc3c_shared.json_io import write_text_file

from .report_policy import report_file_path
from .report_rendering import render_report_json


def write_json_report(path: Path, payload: dict[str, Any]) -> Path:
    owned_path = report_file_path(path)
    write_text_file(owned_path, render_report_json(payload))
    return owned_path


__all__ = ["write_json_report"]
