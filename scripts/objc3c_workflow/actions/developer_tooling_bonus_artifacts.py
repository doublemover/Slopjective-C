"""Artifact readiness and publication paths for bonus-tool inspection."""

from __future__ import annotations

from pathlib import Path

from .developer_tooling_bonus_inputs import ensure_bonus_source_of_truth
from .developer_tooling_paths import PUBLIC_WORKFLOW_REPORT_ROOT


def ensure_bonus_artifact_source() -> int:
    ensure_bonus_source_of_truth()
    return 0


def bonus_tool_integration_report_path() -> Path:
    return PUBLIC_WORKFLOW_REPORT_ROOT / "bonus-tool-integration.json"
