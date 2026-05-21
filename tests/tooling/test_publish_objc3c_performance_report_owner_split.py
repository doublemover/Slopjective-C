from __future__ import annotations

import importlib
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_performance_report.model import (  # noqa: E402
    build_performance_report_model,
    public_summary_payload,
)
from objc3c_performance_report.paths import PerformanceReportPaths, SUMMARY_CONTRACT_ID  # noqa: E402
from objc3c_performance_report.rendering import render_markdown_report  # noqa: E402


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
        "contract_id": "objc3c.performance.governance.dashboard.summary.v1",
        "budget_model_path": "tests/tooling/fixtures/performance_governance/budget_model.json",
        "claim_policy_path": "tests/tooling/fixtures/performance_governance/claim_policy.json",
        "breach_triage_policy_path": "tests/tooling/fixtures/performance_governance/breach_triage_policy.json",
        "lab_policy_path": "tests/tooling/fixtures/performance_governance/lab_policy.json",
        "source_surface_path": "tests/tooling/fixtures/performance_governance/source_surface.json",
        "workflow_surface_path": "tests/tooling/fixtures/performance_governance/workflow_surface.json",
        "environment_drift": {"issues": ["cpu profile drift", "toolchain drift"]},
        "policy_contracts": {
            "budget_model": "objc3c.performance.governance.budget.model.v1",
        },
        "upstream_report_contracts": {
            "performance_summary": "objc3c.performance.benchmark.summary.v1",
        },
        "owner_split": {
            "runtime_performance": [
                "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
            ],
        },
        "upstream_reports": {
            "benchmark": "tmp/reports/performance/benchmark-summary.json",
            "runtime": "tmp/reports/performance/runtime-summary.json",
        },
        "runtime_contract_evidence": {
            "evidence_id": "objc3c.performance.governance.runtime-contract-evidence.v1",
            "status": "PASS",
            "support_authority": False,
            "contract_paths": [
                "tests/tooling/fixtures/runtime_performance/workload_replay_contract.json",
                "tests/tooling/fixtures/runtime_performance/metadata_resilience_contract.json",
                "tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json",
            ],
            "budget_metric_ids": ["dispatch_wall_clock_ms"],
            "workload_ids": ["dispatch-cache"],
            "summary_counts": {"replay.row_count": 12},
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
    assert payload["policy_paths"]["budget_model"] == (
        "tests/tooling/fixtures/performance_governance/budget_model.json"
    )
    assert payload["upstream_reports"]["benchmark"] == (
        "tmp/reports/performance/benchmark-summary.json"
    )
    assert payload["runtime_contract_evidence"]["status"] == "PASS"
    assert payload["runtime_contract_evidence"]["support_authority"] is False
    assert payload["runtime_contract_evidence"]["contract_paths"] == [
        "tests/tooling/fixtures/runtime_performance/workload_replay_contract.json",
        "tests/tooling/fixtures/runtime_performance/metadata_resilience_contract.json",
        "tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json",
    ]
    assert payload["owner_split"]["runtime_performance"] == [
        "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
    ]
    assert payload["publication_contracts"]["dashboard_summary"] == (
        "objc3c.performance.governance.dashboard.summary.v1"
    )
    assert payload["publication_contracts"]["policy.budget_model"] == (
        "objc3c.performance.governance.budget.model.v1"
    )
    assert payload["publication_contracts"]["upstream.performance_summary"] == (
        "objc3c.performance.benchmark.summary.v1"
    )
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
            "contract_id": "objc3c.performance.governance.dashboard.summary.v1",
            "environment_drift": {"issues": []},
            "upstream_reports": {},
        },
    )

    markdown = render_markdown_report(model)

    assert markdown.startswith("# Objective-C 3 Performance Report\n")
    assert "- Release status: `release-ready`" in markdown
    assert "Environment drift issues: none." in markdown
    assert "## Evidence" in markdown
    assert "## Runtime Contract Evidence" in markdown
    assert "## Policy Contracts" in markdown
    assert "## Owner Split" in markdown
