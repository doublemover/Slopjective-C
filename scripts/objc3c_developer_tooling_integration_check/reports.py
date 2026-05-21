"""Developer-tooling report loading and report-path inventory."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .assertions import expect
from .constants import (
    DIAGNOSTIC_QUALITY_SUMMARY_PATH,
    EDITOR_SURFACE_PATH,
    FORMATTER_DEBUG_SUMMARY_PATH,
    FORMATTER_REWRITE_SUMMARY_PATH,
    PUBLIC_WORKFLOW_REPORT_ROOT,
    ROOT,
    WORKSPACE_INTEGRATION_SUMMARY_PATH,
)

REPORT_PATHS = {
    "compile_observability": PUBLIC_WORKFLOW_REPORT_ROOT / "compile-observability.json",
    "runtime_inspector": PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-inspector.json",
    "capability_explorer": PUBLIC_WORKFLOW_REPORT_ROOT / "capability-explorer.json",
    "runtime_inspector_benchmark": PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-inspector-benchmark.json",
    "compile_stage_trace": PUBLIC_WORKFLOW_REPORT_ROOT / "compile-stage-trace.json",
    "runtime_debug_trace": PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-debug-trace.json",
    "editor_surface": EDITOR_SURFACE_PATH,
    "formatter_debug_summary": FORMATTER_DEBUG_SUMMARY_PATH,
    "formatter_rewrite_summary": FORMATTER_REWRITE_SUMMARY_PATH,
    "diagnostic_quality_summary": DIAGNOSTIC_QUALITY_SUMMARY_PATH,
    "workspace_integration_summary": WORKSPACE_INTEGRATION_SUMMARY_PATH,
}


def read_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def assert_report_paths_exist(failures: list[str]) -> None:
    for path in REPORT_PATHS.values():
        expect(path.is_file(), f"missing expected report: {path.relative_to(ROOT).as_posix()}", failures)


def load_reports() -> dict[str, Any]:
    return {
        name: read_json(path) if path.is_file() else {}
        for name, path in REPORT_PATHS.items()
    }


def report_path_payload() -> dict[str, str]:
    return {
        name: path.relative_to(ROOT).as_posix()
        for name, path in REPORT_PATHS.items()
    }
