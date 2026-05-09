from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions import validation_timing
from scripts.objc3c_workflow.actions import validation_timing_reports
from scripts.objc3c_workflow.actions.validation_timing_numbers import safe_float
from scripts.objc3c_workflow.actions.validation_timing_report_summaries import (
    summarize_runtime_acceptance_report,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "validation_timing_numbers",
    "validation_timing_report_io",
    "validation_timing_child_report_loading",
    "validation_timing_report_summaries",
    "validation_timing_dashboard_sections",
)


def test_validation_timing_reports_is_public_import_surface_only() -> None:
    facade_text = (ACTION_ROOT / "validation_timing_reports.py").read_text(
        encoding="utf-8"
    )

    for owner in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.actions.{owner}")
        assert f"from .{owner} import" in facade_text
    assert "def " not in facade_text
    assert "import json" not in facade_text
    assert "from ..environment import ROOT" not in facade_text


def test_validation_timing_public_facades_export_owner_functions() -> None:
    assert validation_timing.safe_float is safe_float
    assert validation_timing_reports.safe_float is safe_float
    assert validation_timing.summarize_runtime_acceptance_report is (
        summarize_runtime_acceptance_report
    )
    assert validation_timing_reports.summarize_runtime_acceptance_report is (
        summarize_runtime_acceptance_report
    )


def test_validation_timing_summary_contract_uses_strict_command_groups() -> None:
    payload = {
        "case_count": 2,
        "default_compile_backend": "native",
        "timing": {
            "elapsed_seconds": "3.25",
            "completed_case_count": 2,
            "command_timings": [
                {"command": "objc3c-native sample.objc3", "duration_seconds": "1.5"},
                {
                    "command": "objc3c_native_compile.ps1 sample.objc3",
                    "duration_seconds": 2,
                },
            ],
        },
    }

    summary = summarize_runtime_acceptance_report(
        "tmp/report.json",
        payload,
        report_reused=True,
    )

    assert summary["elapsed_seconds"] == 3.25
    assert summary["case_count"] == 2
    assert summary["completed_case_count"] == 2
    assert summary["report_reused"] is True
    assert summary["command_groups"]["native"] == {
        "count": 1,
        "duration_seconds": 1.5,
    }
    assert summary["command_groups"]["wrapper"] == {
        "count": 1,
        "duration_seconds": 2.0,
    }
