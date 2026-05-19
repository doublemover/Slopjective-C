from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_capture

from objc3c_performance_report.paths import PerformanceReportPaths, RequiredReportPath
from objc3c_performance_report.validation import require_pass_status


@dataclass(frozen=True)
class PerformanceReportInputs:
    source_summary: dict[str, Any]
    schema_summary: dict[str, Any]
    dashboard_summary: dict[str, Any]


def ensure_success(required: RequiredReportPath) -> dict[str, Any]:
    if not required.summary_path.is_file():
        result = run_capture(python_script_command(required.builder_script))
        if result.returncode != 0:
            raise RuntimeError(f"failed to build required report via {repo_rel(required.builder_script)}")
    payload = load_json(required.summary_path)
    require_pass_status(payload, required.summary_path)
    return payload


def load_performance_report_inputs(paths: PerformanceReportPaths) -> PerformanceReportInputs:
    source_report, schema_report, dashboard_report = paths.required_reports()
    return PerformanceReportInputs(
        source_summary=ensure_success(source_report),
        schema_summary=ensure_success(schema_report),
        dashboard_summary=ensure_success(dashboard_report),
    )
