"""Workflow report helpers."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from scripts.objc3c_shared.json_io import render_json, write_json_file


def emit_json(payload: object) -> int:
    sys.stdout.write(render_json(payload))
    return 0


def write_json_report(path: Path, payload: dict[str, Any]) -> Path:
    write_json_file(path, payload)
    return path
