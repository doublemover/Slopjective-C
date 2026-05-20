"""Nested stress source-surface contract validation."""

from __future__ import annotations

from typing import Any

from .constants import EXPECTED_FAMILIES
from .failures import require
from .paths import require_path


def validate_safety_policy(safety_policy: dict[str, Any]) -> None:
    require(safety_policy.get("policy_id") == "objc3c.stress.validation.safety-policy.v1", "stress safety policy_id drifted")
    require(safety_policy.get("schema_version") == 1, "stress safety policy schema_version drifted")
    for key in (
        "determinism_rule",
        "allowed_input_classes",
        "allowed_execution_modes",
        "required_guards",
        "differential_rules",
        "forbidden_patterns",
        "claim_boundary",
    ):
        require(key in safety_policy, f"stress safety policy missing {key}")


def validate_artifact_surface(artifact_surface: dict[str, Any]) -> None:
    require(artifact_surface.get("contract_id") == "objc3c.stress.artifact.surface.v1", "stress artifact surface contract_id drifted")
    require(artifact_surface.get("schema_version") == 1, "stress artifact surface schema_version drifted")
    for key in (
        "machine_owned_artifact_roots",
        "machine_owned_report_roots",
        "summary_reports",
        "failure_capsule_required_artifacts",
        "reducer_session_required_artifacts",
        "triage_required_artifacts",
        "artifact_rules",
    ):
        require(key in artifact_surface, f"stress artifact surface missing {key}")

    for root_key in ("machine_owned_artifact_roots", "machine_owned_report_roots"):
        roots = artifact_surface.get(root_key)
        require(isinstance(roots, list) and bool(roots), f"stress artifact surface {root_key} drifted")
        for relative_path in roots:
            require(
                isinstance(relative_path, str) and relative_path.startswith("tmp/"),
                f"stress artifact surface {root_key} contains an invalid path",
            )

    summary_reports = artifact_surface.get("summary_reports")
    require(isinstance(summary_reports, dict) and bool(summary_reports), "stress artifact surface summary_reports drifted")
    for relative_path in summary_reports.values():
        require(
            isinstance(relative_path, str) and relative_path.startswith("tmp/reports/stress/"),
            "stress artifact surface summary_reports contains an invalid path",
        )


def validate_workflow_surface(workflow_surface: dict[str, Any], source_surface: dict[str, Any]) -> None:
    require(workflow_surface.get("contract_id") == "objc3c.stress.workflow.surface.v1", "stress workflow surface contract_id drifted")
    require(workflow_surface.get("schema_version") == 1, "stress workflow surface schema_version drifted")
    require(workflow_surface.get("source_check_action") == source_surface.get("source_check_action"), "stress workflow source_check_action drifted from source surface")
    require(workflow_surface.get("validate_action") == "validate-stress", "stress workflow validate_action drifted")
    require(workflow_surface.get("package_bridge") == source_surface.get("package_bridge"), "stress workflow package_bridge drifted from source surface")
    require(workflow_surface.get("performance_dashboard_action") == "build-performance-dashboard", "stress workflow performance_dashboard_action drifted")
    require(
        workflow_surface.get("performance_dashboard_report") == "tmp/reports/performance-governance/dashboard-summary.json",
        "stress workflow performance_dashboard_report drifted",
    )
    require(
        workflow_surface.get("performance_regression_gate_id") == "objc3c.performance.release.regression-gate.v1",
        "stress workflow performance_regression_gate_id drifted",
    )

    required_actions = workflow_surface.get("required_actions")
    validate_child_actions = workflow_surface.get("validate_child_actions")
    require(isinstance(required_actions, list) and bool(required_actions), "stress workflow required_actions drifted")
    require(isinstance(validate_child_actions, list) and bool(validate_child_actions), "stress workflow validate_child_actions drifted")
    require(
        validate_child_actions == required_actions[: len(validate_child_actions)],
        "stress workflow validate_child_actions drifted from required action order",
    )

    required_child_reports = workflow_surface.get("required_child_reports")
    require(isinstance(required_child_reports, dict) and bool(required_child_reports), "stress workflow required_child_reports drifted")
    for relative_path in required_child_reports:
        require(
            isinstance(relative_path, str) and relative_path.startswith("tmp/reports/stress/"),
            "stress workflow required_child_reports contains an invalid path",
        )


def validate_claim_gate(
    claim_gate: dict[str, Any],
    *,
    surface: dict[str, Any],
    artifact_surface: dict[str, Any],
    workflow_surface: dict[str, Any],
) -> list[dict[str, Any]]:
    require(claim_gate.get("contract_id") == "objc3c.stress.claim.gate.v1", "stress claim gate contract_id drifted")
    require(claim_gate.get("schema_version") == 1, "stress claim gate schema_version drifted")
    require(claim_gate.get("source_surface") == "tests/tooling/fixtures/stress/source_surface.json", "stress claim gate source_surface drifted")
    require(claim_gate.get("workflow_surface") == surface.get("workflow_surface"), "stress claim gate workflow_surface drifted")
    require(claim_gate.get("artifact_surface") == surface.get("artifact_surface"), "stress claim gate artifact_surface drifted")

    summary_reports = artifact_surface.get("summary_reports")
    require(isinstance(summary_reports, dict) and bool(summary_reports), "stress claim gate missing artifact summary reports")
    known_report_contracts = workflow_surface.get("required_child_reports")
    require(isinstance(known_report_contracts, dict) and bool(known_report_contracts), "stress claim gate missing workflow child reports")

    claims = claim_gate.get("claims")
    require(isinstance(claims, list) and bool(claims), "stress claim gate missing claims")
    claim_summaries: list[dict[str, Any]] = []
    seen_claim_ids: set[str] = set()
    for claim in claims:
        require(isinstance(claim, dict), "stress claim gate contains a non-object claim")
        claim_id = claim.get("claim_id")
        status = claim.get("status")
        evidence_reports = claim.get("evidence_reports")
        durable_inputs = claim.get("durable_inputs")
        unsupported_surfaces = claim.get("unsupported_surfaces")
        fail_closed_by = claim.get("fail_closed_by")
        require(isinstance(claim_id, str) and bool(claim_id), "stress claim gate claim missing claim_id")
        require(claim_id not in seen_claim_ids, f"stress claim gate duplicate claim_id {claim_id}")
        seen_claim_ids.add(claim_id)
        require(status in {"supported", "partial"}, f"stress claim gate claim {claim_id} has invalid status")
        require(isinstance(evidence_reports, list) and bool(evidence_reports), f"stress claim gate claim {claim_id} missing evidence_reports")
        require(isinstance(durable_inputs, list) and bool(durable_inputs), f"stress claim gate claim {claim_id} missing durable_inputs")
        require(isinstance(unsupported_surfaces, list), f"stress claim gate claim {claim_id} missing unsupported_surfaces")
        require(isinstance(fail_closed_by, list) and bool(fail_closed_by), f"stress claim gate claim {claim_id} missing fail_closed_by")

        for report_path in evidence_reports:
            require(isinstance(report_path, str) and report_path.startswith("tmp/reports/stress/"), f"stress claim gate claim {claim_id} has invalid evidence report")
            require(report_path in known_report_contracts, f"stress claim gate claim {claim_id} evidence report is not workflow-required")
        for durable_input in durable_inputs:
            require(isinstance(durable_input, str) and bool(durable_input), f"stress claim gate claim {claim_id} has invalid durable input")
            require(not durable_input.startswith("tmp/"), f"stress claim gate claim {claim_id} uses tmp as source of truth")
            require_path(durable_input, kind=f"{claim_id} durable input")
        for guard_path in fail_closed_by:
            require(isinstance(guard_path, str) and bool(guard_path), f"stress claim gate claim {claim_id} has invalid fail-closed guard")
            require(not guard_path.startswith("tmp/"), f"stress claim gate claim {claim_id} uses tmp as fail-closed guard")
            require_path(guard_path, kind=f"{claim_id} fail-closed guard")

        claim_summaries.append(
            {
                "claim_id": claim_id,
                "status": status,
                "evidence_report_count": len(evidence_reports),
                "durable_input_count": len(durable_inputs),
                "unsupported_surface_count": len(unsupported_surfaces),
            }
        )

    return claim_summaries


def validate_checked_in_roots(source_surface: dict[str, Any]) -> list[str]:
    checked_in_roots = source_surface.get("checked_in_roots")
    require(isinstance(checked_in_roots, list) and bool(checked_in_roots), "checked_in_roots missing")
    for relative_path in checked_in_roots:
        require(isinstance(relative_path, str) and bool(relative_path), "checked_in_roots contains a non-string entry")
        require_path(relative_path, kind="checked-in stress root")
    return checked_in_roots


def collect_family_summaries(source_surface: dict[str, Any]) -> list[dict[str, Any]]:
    source_families = source_surface.get("source_families")
    require(isinstance(source_families, list) and bool(source_families), "source_families missing")

    family_ids: list[str] = []
    family_summaries: list[dict[str, Any]] = []
    for family in source_families:
        require(isinstance(family, dict), "source_families contains a non-object entry")
        family_id = family.get("family_id")
        coverage_goal = family.get("coverage_goal")
        source_paths = family.get("source_paths")
        require(isinstance(family_id, str) and bool(family_id), "source_families entry missing family_id")
        require(isinstance(coverage_goal, str) and bool(coverage_goal), f"{family_id} missing coverage_goal")
        require(isinstance(source_paths, list) and bool(source_paths), f"{family_id} missing source_paths")
        for source_path in source_paths:
            require(isinstance(source_path, str) and bool(source_path), f"{family_id} contains a non-string source path")
            if not source_path.startswith("npm run "):
                require_path(source_path, kind=f"{family_id} source path")
        family_ids.append(family_id)
        family_summaries.append(
            {
                "family_id": family_id,
                "source_path_count": len(source_paths),
                "coverage_goal": coverage_goal,
            }
        )

    require(family_ids == EXPECTED_FAMILIES, "source family inventory drifted")
    return family_summaries
