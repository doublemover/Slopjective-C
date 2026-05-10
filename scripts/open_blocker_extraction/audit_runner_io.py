"""Open-blocker audit runner text output helpers."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import write_text_file
from objc3c_tooling.public_workflow_output import normalize_newlines


def write_text(path: Path, content: str) -> None:
    write_text_file(path, normalize_newlines(content))


def bool_text(value: bool) -> str:
    return "true" if value else "false"
