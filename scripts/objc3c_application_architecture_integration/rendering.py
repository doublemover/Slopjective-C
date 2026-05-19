"""Report writing and CLI output for application architecture integration."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .contracts import REPORT_PATH


def write_integration_summary(payload: dict[str, Any]) -> Path:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    return REPORT_PATH


def render_summary_path(summary_path: Path = REPORT_PATH) -> None:
    print(f"summary_path: {repo_rel(summary_path)}")


def render_failures(failures: list[str]) -> None:
    print("application-architecture-integration: FAIL", file=sys.stderr)
    for failure in failures:
        print(f"- {failure}", file=sys.stderr)
