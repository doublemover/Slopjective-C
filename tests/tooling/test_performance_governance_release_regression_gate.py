from __future__ import annotations

import sys
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_performance_dashboard_builder.input_loading import DashboardInputs
from objc3c_performance_dashboard_builder.summary import build_dashboard_summary


GENERATED_AT = "2026-05-20T12:00:00+00:00"


def _status_payload(contract_id: str) -> dict[str, object]:
    return {
        "contract_id": contract_id,
        "generated_at_utc": GENERATED_AT,
        "status": "PASS",
        "telemetry_packets": [],
    }


def _dashboard_inputs(runtime_median_ms: float) -> DashboardInputs:
    return DashboardInputs(
        source_surface={
            "contract_id": "objc3c.performance.governance.source.surface.v1",
            "owner_split": {"performance_governance": ["tests/tooling/fixtures/performance_governance/budget_model.json"]},
        },
        budget_model={
            "contract_id": "objc3c.performance.governance.budget.model.v1",
            "budget_families": [
                {
                    "budget_id": "runtime-hot-path",
                    "summary_path": "tmp/reports/runtime-performance/benchmark-summary.json",
                    "required_status": "PASS",
                    "metric_definitions": [
                        {
                            "metric_id": "dispatch_wall_clock_ms",
                            "source_field": "workloads[dispatch-cache].summary.median_duration_ms",
                            "comparison": "max",
                            "warning_value": 100.0,
                            "blocking_value": 200.0,
                        }
                    ],
                }
            ],
            "breach_taxonomy": [
                {
                    "breach_id": "hard-regression",
                    "severity": "blocking",
                    "operator_action": "block-release-and-refresh-evidence",
                },
                {
                    "breach_id": "soft-regression",
                    "severity": "warning",
                    "operator_action": "publish-with-caution-or-fix-before-release",
                },
                {
                    "breach_id": "coverage-gap",
                    "severity": "blocking",
                    "operator_action": "rebuild-missing-upstream-evidence",
                },
                {
                    "breach_id": "environment-drift",
                    "severity": "blocking",
                    "operator_action": "re-run-on-approved-lab-profile",
                },
                {
                    "breach_id": "waived-regression",
                    "severity": "caution",
                    "operator_action": "publish-with-explicit-waiver",
                },
            ],
        },
        claim_policy={
            "contract_id": "objc3c.performance.governance.claim.policy.v1",
            "claim_statuses": [
                {
                    "status": "release-ready",
                    "requires": [
                        "all_required_budget_families_pass",
                        "no_blocking_breach",
                        "no_expired_waiver",
                    ],
                },
                {
                    "status": "caution",
                    "requires": [
                        "all_required_budget_families_present",
                        "no_blocking_environment_drift",
                    ],
                },
                {
                    "status": "blocked",
                    "requires": ["blocked-when-any-release-blocking-condition-is-met"],
                },
            ],
        },
        triage_policy={
            "contract_id": "objc3c.performance.governance.breach.triage.policy.v1",
            "classifications": [
                {
                    "classification": "code-regression",
                    "allowed_breach_ids": [
                        "hard-regression",
                        "soft-regression",
                        "waived-regression",
                    ],
                },
                {
                    "classification": "coverage-gap",
                    "allowed_breach_ids": ["coverage-gap"],
                },
                {
                    "classification": "environment-drift",
                    "allowed_breach_ids": ["environment-drift"],
                },
            ],
        },
        lab_policy={"contract_id": "objc3c.performance.governance.lab.policy.v1"},
        waivers_payload={
            "contract_id": "objc3c.performance.governance.waiver.registry.v1",
            "waivers": [],
        },
        workflow_surface={
            "contract_id": "objc3c.performance.governance.workflow.surface.v1",
            "validate_action": "validate-performance-governance",
            "integration_action": "validate-performance-governance-integration",
            "end_to_end_action": "validate-performance-governance-end-to-end",
            "required_actions": ["validate-performance-governance"],
            "upstream_validate_actions": ["validate-runtime-performance"],
        },
        performance_summary=_status_payload("objc3c.performance.benchmark.summary.v1"),
        performance_integration=_status_payload("objc3c.performance.integration.summary.v1"),
        comparative_summary=_status_payload("objc3c.performance.comparative.baselines.summary.v1"),
        compiler_summary=_status_payload("objc3c.compiler.throughput.summary.v1"),
        compiler_integration=_status_payload("objc3c.compiler.throughput.integration.summary.v1"),
        runtime_summary={
            "contract_id": "objc3c.runtime.performance.summary.v1",
            "generated_at_utc": GENERATED_AT,
            "status": "PASS",
            "packet_paths": [],
            "workloads": [
                {
                    "workload_id": "dispatch-cache",
                    "summary": {"median_duration_ms": runtime_median_ms},
                }
            ],
        },
        runtime_integration=_status_payload("objc3c.runtime.performance.integration.summary.v1"),
    )


def test_release_regression_gate_blocks_hard_runtime_regression() -> None:
    payload = build_dashboard_summary(
        _dashboard_inputs(runtime_median_ms=250.0),
        now=datetime(2026, 5, 20, 12, 0, tzinfo=timezone.utc),
        generated_at_utc=datetime(2026, 5, 20, 12, 0, tzinfo=timezone.utc),
    )

    assert payload["release_status"] == "blocked"
    assert payload["claim_ready"] is False
    assert payload["regression_gate"] == {
        "gate_id": "objc3c.performance.release.regression-gate.v1",
        "passed": False,
        "release_status": "blocked",
        "blocking_breach_count": 1,
        "warning_breach_count": 0,
        "budget_ids": ["runtime-hot-path"],
        "blocking_breach_ids": ["hard-regression"],
        "child_report_paths": ["tmp/reports/runtime-performance/benchmark-summary.json"],
    }
