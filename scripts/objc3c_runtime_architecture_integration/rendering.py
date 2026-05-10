"""Output rendering for runtime architecture integration."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .contracts import INTEGRATION_SUMMARY_PATH


def write_integration_summary(payload: dict[str, Any]) -> Path:
    INTEGRATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(INTEGRATION_SUMMARY_PATH, payload)
    return INTEGRATION_SUMMARY_PATH


def render_summary_path(summary_path: Path = INTEGRATION_SUMMARY_PATH) -> None:
    print(f"summary_path: {repo_rel(summary_path)}")
