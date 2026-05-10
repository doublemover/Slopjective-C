"""Validation checks for runtime architecture proof-packet inputs."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from .contracts import (
    CLAIM_BOUNDARY_CONTRACT_ID,
    HARNESS_SUMMARY_CONTRACT_ID,
    PUBLIC_SMOKE_SUITE_ID,
    ROOT,
    SURFACE_KEYS,
)
from .fixtures import (
    load_harness_summary,
    load_public_workflow_report,
    load_runtime_acceptance_report,
)


@dataclass(frozen=True)
class ValidatedRuntimeArchitectureProofPacket:
    harness_summary: dict[str, Any]
    suite_summary: dict[str, Any]
    public_workflow_report_path: Path
    public_workflow_report: dict[str, Any]
    runtime_acceptance_report_path: Path
    runtime_acceptance_report: dict[str, Any]
    child_report_paths: list[str]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def collect_child_report_paths(public_workflow_report: dict[str, Any]) -> list[str]:
    steps = public_workflow_report.get("steps")
    expect(isinstance(steps, list) and steps, "public workflow report did not publish child steps")
    ordered_paths: list[str] = []
    for step in steps:
        expect(isinstance(step, dict), "public workflow report contained a non-object step")
        report_paths = step.get("report_paths", [])
        expect(isinstance(report_paths, list), "public workflow report step did not publish report_paths")
        for raw_path in report_paths:
            expect(isinstance(raw_path, str), "public workflow report step report path was not a string")
            candidate = ROOT / raw_path
            expect(candidate.is_file(), f"public workflow report referenced a missing child report: {raw_path}")
            if raw_path not in ordered_paths:
                ordered_paths.append(raw_path)
    return ordered_paths


def validate_runtime_architecture_proof_packet() -> ValidatedRuntimeArchitectureProofPacket:
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
        suite_summary.get("suite_id") == PUBLIC_SMOKE_SUITE_ID,
        "shared harness summary drifted from public-test-smoke",
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

    suite_claim_boundary = suite_summary.get("claim_boundary")
    public_claim_boundary = public_workflow_report.get("claim_boundary")
    runtime_claim_boundary = runtime_acceptance_report.get("claim_boundary")
    expect(isinstance(suite_claim_boundary, dict), "shared harness suite summary did not publish claim_boundary")
    expect(isinstance(public_claim_boundary, dict), "public workflow report did not publish claim_boundary")
    expect(isinstance(runtime_claim_boundary, dict), "runtime acceptance report did not publish claim_boundary")
    expect(
        suite_claim_boundary == public_claim_boundary,
        "shared harness and public workflow claim boundaries drifted",
    )
    expect(
        suite_claim_boundary.get("contract_id") == CLAIM_BOUNDARY_CONTRACT_ID,
        "shared harness/public workflow claim boundary drifted from the expected contract",
    )
    expect(
        runtime_claim_boundary.get("contract_id") == CLAIM_BOUNDARY_CONTRACT_ID,
        "runtime acceptance claim boundary drifted from the expected contract",
    )
    authoritative_child_surfaces = suite_claim_boundary.get("authoritative_child_surfaces", [])
    expect(
        isinstance(authoritative_child_surfaces, list)
        and "scripts/check_objc3c_runtime_acceptance.py" in authoritative_child_surfaces,
        "composite claim boundary did not carry the runtime acceptance suite as an authoritative child surface",
    )

    for surface_key in SURFACE_KEYS:
        suite_surface = suite_summary.get(surface_key)
        public_surface = public_workflow_report.get(surface_key)
        runtime_surface = runtime_acceptance_report.get(surface_key)
        expect(isinstance(suite_surface, dict), f"shared harness suite summary did not publish {surface_key}")
        expect(isinstance(public_surface, dict), f"public workflow report did not publish {surface_key}")
        expect(isinstance(runtime_surface, dict), f"runtime acceptance report did not publish {surface_key}")
        expect(
            suite_surface == public_surface == runtime_surface,
            f"runtime architecture surface drift detected for {surface_key}",
        )

    child_report_paths = collect_child_report_paths(public_workflow_report)
    return ValidatedRuntimeArchitectureProofPacket(
        harness_summary=harness_summary,
        suite_summary=suite_summary,
        public_workflow_report_path=public_workflow_report_path,
        public_workflow_report=public_workflow_report,
        runtime_acceptance_report_path=runtime_acceptance_report_path,
        runtime_acceptance_report=runtime_acceptance_report,
        child_report_paths=child_report_paths,
    )
