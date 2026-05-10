"""Validation checks for integrated runtime architecture reports."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from .contracts import (
    CLAIM_BOUNDARY_CONTRACT_ID,
    HARNESS_SUMMARY_CONTRACT_ID,
    PROOF_PACKET_CONTRACT_ID,
    REQUIRED_STEP_ACTION_GROUPS,
    ROOT,
    SURFACE_KEYS,
)
from .fixtures import (
    load_harness_summary,
    load_proof_packet,
    load_public_workflow_report,
    load_runtime_acceptance_report,
)


@dataclass(frozen=True)
class ValidatedRuntimeArchitecture:
    harness_summary: dict[str, Any]
    suite_summary: dict[str, Any]
    public_workflow_report_path: Path
    public_workflow_report: dict[str, Any]
    runtime_acceptance_report_path: Path
    runtime_acceptance_report: dict[str, Any]
    proof_packet: dict[str, Any]
    observed_actions: list[str]
    child_report_paths: list[str]
    proof_child_report_paths: list[Any]
    public_claim_boundary: dict[str, Any]
    runtime_claim_boundary: dict[str, Any]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def collect_step_details(public_workflow_report: dict[str, Any]) -> tuple[list[str], list[str]]:
    steps = public_workflow_report.get("steps")
    expect(isinstance(steps, list) and steps, "public workflow report did not publish child steps")
    observed_actions: list[str] = []
    child_report_paths: list[str] = []
    for step in steps:
        expect(isinstance(step, dict), "public workflow report contained a non-object step")
        action = step.get("action")
        expect(isinstance(action, str) and action, "public workflow report step did not publish action")
        observed_actions.append(action)
        expect(step.get("exit_code") == 0, f"public workflow step failed during integration: {action}")
        report_paths = step.get("report_paths", [])
        expect(isinstance(report_paths, list), f"public workflow step {action} did not publish report_paths")
        for raw_path in report_paths:
            expect(isinstance(raw_path, str), f"public workflow step {action} had a non-string report path")
            candidate = ROOT / raw_path
            expect(candidate.is_file(), f"public workflow step {action} referenced a missing child report: {raw_path}")
            if raw_path not in child_report_paths:
                child_report_paths.append(raw_path)
    missing_actions = [
        group_name
        for group_name, accepted_actions in REQUIRED_STEP_ACTION_GROUPS
        if not any(action in observed_actions for action in accepted_actions)
    ]
    expect(
        not missing_actions,
        "public workflow report did not carry the required integrated step action groups: "
        + ", ".join(missing_actions),
    )
    return observed_actions, child_report_paths


def validate_runtime_architecture_reports() -> ValidatedRuntimeArchitecture:
    harness_summary = load_harness_summary()
    expect(
        harness_summary.get("contract_id") == HARNESS_SUMMARY_CONTRACT_ID,
        "unexpected shared harness summary contract id",
    )
    expect(harness_summary.get("status") == "PASS", "shared harness summary did not pass")
    suites = harness_summary.get("suites")
    expect(
        isinstance(suites, list) and len(suites) == 1,
        "shared harness summary did not publish exactly one suite result",
    )
    suite_summary = suites[0]
    expect(isinstance(suite_summary, dict), "shared harness suite summary was not a JSON object")
    expect(
        suite_summary.get("suite_id") == "public-test-full",
        "shared harness summary drifted from public-test-full",
    )

    public_workflow_report_path, public_workflow_report = load_public_workflow_report(suite_summary)
    expect(public_workflow_report.get("status") == "PASS", "public workflow report did not pass")

    acceptance_suite_surface = suite_summary.get("acceptance_suite_surface")
    expect(
        isinstance(acceptance_suite_surface, dict),
        "shared harness suite summary did not publish acceptance_suite_surface",
    )
    runtime_acceptance_report_path, runtime_acceptance_report = load_runtime_acceptance_report(
        acceptance_suite_surface
    )
    expect(runtime_acceptance_report.get("status") == "PASS", "runtime acceptance report did not pass")

    proof_packet = load_proof_packet()
    expect(
        proof_packet.get("contract_id") == PROOF_PACKET_CONTRACT_ID,
        "unexpected runtime architecture evidence bundle contract id",
    )
    expect(proof_packet.get("status") == "PASS", "runtime architecture evidence bundle did not pass")
    proof_packet_surface = proof_packet.get("proof_packet_surface")
    expect(isinstance(proof_packet_surface, dict), "evidence bundle did not publish proof_packet_surface")
    expect(
        proof_packet_surface.get("requires_compile_coupled_child_reports") is True,
        "evidence bundle no longer requires compile-coupled child reports",
    )

    suite_claim_boundary = suite_summary.get("claim_boundary")
    public_claim_boundary = public_workflow_report.get("claim_boundary")
    runtime_claim_boundary = runtime_acceptance_report.get("claim_boundary")
    proof_claim_boundary = proof_packet.get("composite_claim_boundary")
    proof_runtime_claim_boundary = proof_packet.get("runtime_acceptance_claim_boundary")
    expect(isinstance(suite_claim_boundary, dict), "shared harness suite summary did not publish claim_boundary")
    expect(isinstance(public_claim_boundary, dict), "public workflow report did not publish claim_boundary")
    expect(isinstance(runtime_claim_boundary, dict), "runtime acceptance report did not publish claim_boundary")
    expect(isinstance(proof_claim_boundary, dict), "evidence bundle did not publish composite_claim_boundary")
    expect(
        isinstance(proof_runtime_claim_boundary, dict),
        "evidence bundle did not publish runtime_acceptance_claim_boundary",
    )
    expect(
        suite_claim_boundary == public_claim_boundary == proof_claim_boundary,
        "public full workflow claim boundary drifted from the integrated architecture evidence bundle",
    )
    expect(
        runtime_claim_boundary == proof_runtime_claim_boundary,
        "runtime acceptance claim boundary drifted from the integrated architecture evidence bundle",
    )
    expect(
        public_claim_boundary.get("contract_id") == CLAIM_BOUNDARY_CONTRACT_ID,
        "public workflow claim boundary drifted from the expected contract",
    )
    expect(
        runtime_claim_boundary.get("contract_id") == CLAIM_BOUNDARY_CONTRACT_ID,
        "runtime acceptance claim boundary drifted from the expected contract",
    )
    authoritative_child_surfaces = public_claim_boundary.get("authoritative_child_surfaces", [])
    expect(
        isinstance(authoritative_child_surfaces, list)
        and "scripts/check_objc3c_runtime_acceptance.py" in authoritative_child_surfaces,
        "public workflow claim boundary did not carry the runtime acceptance suite as an authoritative child surface",
    )

    for surface_key in SURFACE_KEYS:
        suite_surface = suite_summary.get(surface_key)
        public_surface = public_workflow_report.get(surface_key)
        runtime_surface = runtime_acceptance_report.get(surface_key)
        proof_surface = proof_packet.get(surface_key)
        expect(isinstance(suite_surface, dict), f"shared harness suite summary did not publish {surface_key}")
        expect(isinstance(public_surface, dict), f"public workflow report did not publish {surface_key}")
        expect(isinstance(runtime_surface, dict), f"runtime acceptance report did not publish {surface_key}")
        expect(isinstance(proof_surface, dict), f"evidence bundle did not publish {surface_key}")
        expect(
            suite_surface == public_surface == runtime_surface == proof_surface,
            f"runtime architecture surface drift detected for {surface_key}",
        )

    observed_actions, child_report_paths = collect_step_details(public_workflow_report)
    expect(
        suite_summary.get("step_count") == len(public_workflow_report.get("steps", [])),
        "shared harness suite summary step_count drifted from the public workflow report",
    )
    proof_child_report_paths = proof_packet_surface.get("child_step_report_paths", [])
    expect(
        isinstance(proof_child_report_paths, list),
        "evidence bundle did not publish child_step_report_paths",
    )

    return ValidatedRuntimeArchitecture(
        harness_summary=harness_summary,
        suite_summary=suite_summary,
        public_workflow_report_path=public_workflow_report_path,
        public_workflow_report=public_workflow_report,
        runtime_acceptance_report_path=runtime_acceptance_report_path,
        runtime_acceptance_report=runtime_acceptance_report,
        proof_packet=proof_packet,
        observed_actions=observed_actions,
        child_report_paths=child_report_paths,
        proof_child_report_paths=proof_child_report_paths,
        public_claim_boundary=public_claim_boundary,
        runtime_claim_boundary=runtime_claim_boundary,
    )
