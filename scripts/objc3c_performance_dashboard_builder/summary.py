from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_performance_dashboard.contracts import EXPECTED_CLAIM_STATUS_REQUIREMENTS
from objc3c_performance_dashboard.contracts import EXPECTED_POLICY_CONTRACTS
from objc3c_performance_dashboard.contracts import EXPECTED_UPSTREAM_REPORT_CONTRACTS
from objc3c_performance_dashboard.contracts import SUMMARY_CONTRACT_ID
from objc3c_performance_dashboard.input_loading import validate_contracts
from objc3c_performance_dashboard.metrics import build_breach_lookup
from objc3c_performance_dashboard.metrics import build_taxonomy_lookup
from objc3c_performance_dashboard.paths import BUDGET_MODEL_PATH
from objc3c_performance_dashboard.paths import CLAIM_POLICY_PATH
from objc3c_performance_dashboard.paths import COMPARATIVE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import COMPILER_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import COMPILER_SUMMARY_PATH
from objc3c_performance_dashboard.paths import LAB_POLICY_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import RUNTIME_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import RUNTIME_SUMMARY_PATH
from objc3c_performance_dashboard.paths import SOURCE_SURFACE_PATH
from objc3c_performance_dashboard.paths import TRIAGE_POLICY_PATH
from objc3c_performance_dashboard.paths import WAIVERS_PATH
from objc3c_performance_dashboard.paths import WORKFLOW_SURFACE_PATH

from objc3c_performance_dashboard_builder.input_loading import DashboardInputs
from objc3c_performance_dashboard_builder.input_loading import build_policy_contracts
from objc3c_performance_dashboard_builder.input_loading import build_upstream_report_contracts
from objc3c_performance_dashboard_builder.series import append_environment_breach
from objc3c_performance_dashboard_builder.series import build_dashboard_series
from objc3c_performance_dashboard_builder.series import build_summary_lookup
from objc3c_performance_dashboard_builder.series import evaluate_budget_families
from objc3c_performance_dashboard_builder.series import evaluate_waivers


def build_dashboard_summary(
    inputs: DashboardInputs,
    *,
    now: datetime | None = None,
    generated_at_utc: datetime | None = None,
) -> dict[str, Any]:
    observed_at = now or datetime.now(timezone.utc)
    failures: list[str] = []
    policy_contracts = build_policy_contracts(inputs)
    upstream_report_contracts = build_upstream_report_contracts(inputs)
    validate_contracts(
        policy_contracts,
        EXPECTED_POLICY_CONTRACTS,
        failures,
        label="performance governance policy",
    )
    validate_contracts(
        upstream_report_contracts,
        EXPECTED_UPSTREAM_REPORT_CONTRACTS,
        failures,
        label="upstream performance report",
    )

    claim_status_requirements = {
        str(entry.get("status", "")): entry.get("requires", [])
        for entry in inputs.claim_policy.get("claim_statuses", [])
        if isinstance(entry, dict)
    }
    if claim_status_requirements != EXPECTED_CLAIM_STATUS_REQUIREMENTS:
        failures.append("claim policy status requirements drifted from hard-cutover release gates")
    for requirements in claim_status_requirements.values():
        if isinstance(requirements, list) and any("retired-route" in str(item) for item in requirements):
            failures.append("claim policy reintroduced retired route wording")

    waivers = inputs.waivers_payload.get("waivers", [])
    if not isinstance(waivers, list):
        failures.append("waiver registry waivers field drifted")
        waivers = []

    taxonomy_lookup = build_taxonomy_lookup(inputs.budget_model)
    breach_lookup = build_breach_lookup(inputs.triage_policy)
    series = build_dashboard_series(inputs, now=observed_at)
    budget_evaluation = evaluate_budget_families(
        budget_model=inputs.budget_model,
        summary_lookup=build_summary_lookup(inputs),
        derived=series.derived,
        taxonomy_lookup=taxonomy_lookup,
        breach_lookup=breach_lookup,
        waivers=waivers,
        failures=failures,
    )
    breaches = budget_evaluation.breaches

    if series.environment_issues:
        append_environment_breach(
            breaches=breaches,
            taxonomy_lookup=taxonomy_lookup,
            breach_lookup=breach_lookup,
        )

    expired_waivers = evaluate_waivers(
        waivers=waivers,
        failures=failures,
        now=observed_at,
    )

    blocking_breach_count = sum(1 for breach in breaches if breach.get("severity") == "blocking")
    warning_breach_count = sum(1 for breach in breaches if breach.get("severity") in {"warning", "caution"})
    if blocking_breach_count or expired_waivers:
        release_status = "blocked"
    elif warning_breach_count:
        release_status = "caution"
    else:
        release_status = "release-ready"

    allowed_statuses = [
        entry.get("status")
        for entry in inputs.claim_policy.get("claim_statuses", [])
        if isinstance(entry, dict)
    ]
    if release_status not in allowed_statuses:
        failures.append("claim policy no longer admits the derived release_status")

    regression_gate = {
        "gate_id": "objc3c.performance.release.regression-gate.v1",
        "passed": release_status == "release-ready",
        "release_status": release_status,
        "blocking_breach_count": blocking_breach_count,
        "warning_breach_count": warning_breach_count,
        "budget_ids": [
            str(summary.get("budget_id", ""))
            for summary in budget_evaluation.budget_family_summaries
            if isinstance(summary.get("budget_id"), str)
        ],
        "blocking_breach_ids": [
            str(breach.get("breach_id", ""))
            for breach in breaches
            if breach.get("severity") == "blocking"
        ],
        "child_report_paths": [
            str(summary.get("child_report_path", ""))
            for summary in budget_evaluation.budget_family_summaries
            if isinstance(summary.get("child_report_path"), str)
        ],
    }

    generated_at = generated_at_utc or datetime.now(timezone.utc)
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": generated_at.isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "budget_model_path": repo_rel(BUDGET_MODEL_PATH),
        "claim_policy_path": repo_rel(CLAIM_POLICY_PATH),
        "breach_triage_policy_path": repo_rel(TRIAGE_POLICY_PATH),
        "lab_policy_path": repo_rel(LAB_POLICY_PATH),
        "source_surface_path": repo_rel(SOURCE_SURFACE_PATH),
        "workflow_surface_path": repo_rel(WORKFLOW_SURFACE_PATH),
        "policy_contracts": policy_contracts,
        "upstream_report_contracts": upstream_report_contracts,
        "upstream_reports": {
            "performance_summary": repo_rel(PERFORMANCE_SUMMARY_PATH),
            "performance_integration": repo_rel(PERFORMANCE_INTEGRATION_PATH),
            "comparative_summary": repo_rel(COMPARATIVE_SUMMARY_PATH),
            "compiler_summary": repo_rel(COMPILER_SUMMARY_PATH),
            "compiler_integration": repo_rel(COMPILER_INTEGRATION_PATH),
            "runtime_summary": repo_rel(RUNTIME_SUMMARY_PATH),
            "runtime_integration": repo_rel(RUNTIME_INTEGRATION_PATH),
        },
        "release_status": release_status,
        "claim_ready": release_status == "release-ready",
        "regression_gate": regression_gate,
        "owner_split": inputs.source_surface.get("owner_split", {}),
        "workflow_actions": {
            "validate_action": inputs.workflow_surface.get("validate_action"),
            "integration_action": inputs.workflow_surface.get("integration_action"),
            "end_to_end_action": inputs.workflow_surface.get("end_to_end_action"),
            "required_actions": inputs.workflow_surface.get("required_actions", []),
            "upstream_validate_actions": inputs.workflow_surface.get("upstream_validate_actions", []),
        },
        "budget_family_summaries": budget_evaluation.budget_family_summaries,
        "breaches": breaches,
        "waivers": waivers,
        "environment_drift": {
            "machine_profiles_consistent": series.machine_profiles_consistent,
            "stale_report_paths": series.stale_report_paths,
            "issues": series.environment_issues,
        },
        "blocking_breach_count": blocking_breach_count,
        "warning_breach_count": warning_breach_count,
        "failures": failures,
    }
