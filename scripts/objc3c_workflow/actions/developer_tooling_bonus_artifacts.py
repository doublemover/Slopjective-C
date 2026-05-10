"""Artifact readiness and publication paths for bonus-tool inspection."""

from __future__ import annotations

from pathlib import Path

from ..action_execution_dispatch import execute_registered_action
from ..environment import ROOT
from .developer_tooling_paths import PUBLIC_WORKFLOW_REPORT_ROOT


def ensure_bonus_artifact_source() -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if native_main.is_file():
        return execute_registered_action("build-native-contracts", [])
    return 0


def bonus_tool_integration_report_path() -> Path:
    return PUBLIC_WORKFLOW_REPORT_ROOT / "bonus-tool-integration.json"
