"""Publication side effects for the external validation source-surface checker."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel


def publish_summary(summary_path: Path, summary: dict[str, Any]) -> None:
    write_report_json(summary_path, summary, sort_keys=False)


def print_success(summary_path: Path) -> None:
    print(f"summary_path: {repo_rel(summary_path)}")
    print("external-validation-source-surface: OK")
