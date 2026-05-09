from __future__ import annotations

import importlib
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_performance_report.model import (
    build_performance_report_model,
    public_summary_payload,
)
from objc3c_performance_report.paths import PerformanceReportPaths, SUMMARY_CONTRACT_ID
from objc3c_performance_report.rendering import render_markdown_report


OWNER_MODULES = (
    "objc3c_performance_report.paths",
    "objc3c_performance_report.input_loading",
    "objc3c_performance_report.validation",
    "objc3c_performance_report.model",
    "objc3c_performance_report.rendering",
    "objc3c_performance_report.publication",
)


def test_performance_report_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_performance_report_entrypoint_delegates_to_owner_modules() -> None:
    script_text = (ROOT / "scripts" / "publish_objc3c_performance_report.py").read_text(encoding="utf-8")
    assert "objc3c_performance_report.cli" in script_text
    assert "json.dumps(" not in script_text
    assert "shutil." not in script_text
    assert "subprocess" not in script_text


def test_performance_report_model_preserves_public_contract(tmp_path: Path) -> None:
    paths = PerformanceReportPaths.for_root(tmp_path)
    dashboard = {
        "release_status": "caution",
        "claim_ready": True,
        "blocking_breach_count": 0,
        "warning_breach_count": 2,
        "environment_drift": {"issues": ["cpu profile drift", "toolchain drift"]},
        "upstream_reports": {
            "benchmark": "tmp/reports/performance/benchmark-summary.json",
            "runtime": "tmp/reports/performance/runtime-summary.json",
        },
    }

    model = build_performance_report_model(paths, dashboard)
    payload = public_summary_payload(
        paths=paths,
        model=model,
        generated_at_utc=datetime(2026, 5, 9, tzinfo=timezone.utc),
    )

    assert payload["contract_id"] == SUMMARY_CONTRACT_ID
    assert payload["release_status"] == "caution"
    assert payload["claim_ready"] is True
    assert payload["evidence_paths"] == [
        "tmp/reports/performance-governance/source-surface-summary.json",
        "tmp/reports/performance-governance/schema-surface-summary.json",
        "tmp/reports/performance-governance/dashboard-summary.json",
        "tmp/reports/performance/benchmark-summary.json",
        "tmp/reports/performance/runtime-summary.json",
    ]
    assert "warning-or-caution breaches" in payload["summary_lines"][0]
    assert "cpu profile drift; toolchain drift" in payload["summary_lines"][2]


def test_performance_report_markdown_is_rendering_owned(tmp_path: Path) -> None:
    paths = PerformanceReportPaths.for_root(tmp_path)
    model = build_performance_report_model(
        paths,
        {
            "release_status": "release-ready",
            "claim_ready": True,
            "blocking_breach_count": 0,
            "warning_breach_count": 0,
            "environment_drift": {"issues": []},
            "upstream_reports": {},
        },
    )

    markdown = render_markdown_report(model)

    assert markdown.startswith("# Objective-C 3 Performance Report\n")
    assert "- Release status: `release-ready`" in markdown
    assert "Environment drift issues: none." in markdown
    assert "## Evidence" in markdown
