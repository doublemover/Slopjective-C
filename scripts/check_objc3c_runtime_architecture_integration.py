#!/usr/bin/env python3
"""Validate integrated runtime architecture over the live full public workflow."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
HARNESS_SCRIPT = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
PROOF_PACKET_SCRIPT = ROOT / "scripts" / "check_objc3c_runtime_architecture_proof_packet.py"
FULL_HARNESS_SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "runtime"
    / "shared-executable-acceptance-harness"
    / "public-test-full"
    / "summary.json"
)
PROOF_PACKET_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "architecture-proof" / "summary.json"
)
INTEGRATION_SUMMARY_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
)
INTEGRATION_CONTRACT_ID = "objc3c.runtime.architecture.integration.summary.v1"
INTEGRATION_SURFACE_CONTRACT_ID = "objc3c.runtime.architecture.integration.surface.v1"
PROOF_PACKET_CONTRACT_ID = "objc3c.runtime.architecture.proof.packet.v1"
HARNESS_SUMMARY_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.summary.v1"
CLAIM_BOUNDARY_CONTRACT_ID = "objc3c.runtime.execution.claim.boundary.v1"
SURFACE_KEYS = (
    "runtime_state_publication_surface",
    "acceptance_suite_surface",
    "runtime_object_model_realization_source_surface",
    "runtime_property_ivar_storage_accessor_source_surface",
    "runtime_realization_lowering_reflection_artifact_surface",
    "runtime_dispatch_table_reflection_record_lowering_surface",
    "runtime_cross_module_realized_metadata_replay_preservation_surface",
    "runtime_object_model_abi_query_surface",
    "runtime_realization_lookup_reflection_implementation_surface",
    "runtime_reflection_query_surface",
    "runtime_realization_lookup_semantics_surface",
    "runtime_class_metaclass_protocol_realization_surface",
    "runtime_category_attachment_merged_dispatch_surface",
    "runtime_reflection_visibility_coherence_diagnostics_surface",
    "runtime_installation_abi_surface",
    "runtime_loader_lifecycle_surface",
)
REQUIRED_STEP_ACTIONS = (
    "test-execution-smoke",
    "test-runtime-acceptance",
    "test-execution-replay",
)


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid JSON artifact at {repo_rel(path)}: {exc}") from exc
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


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
    missing_actions = [action for action in REQUIRED_STEP_ACTIONS if action not in observed_actions]
    expect(
        not missing_actions,
        "public workflow report did not carry the required integrated step actions: "
        + ", ".join(missing_actions),
    )
    return observed_actions, child_report_paths


def main() -> int:
    result = run_capture(
        [sys.executable, str(HARNESS_SCRIPT), "--run-suite", "public-test-full"]
    )
    if result.returncode != 0:
        raise RuntimeError("shared executable acceptance harness failed for public-test-full")

    result = run_capture([sys.executable, str(PROOF_PACKET_SCRIPT)])
    if result.returncode != 0:
        raise RuntimeError("runtime architecture proof packet validation failed")

    harness_summary = load_json(FULL_HARNESS_SUMMARY_PATH)
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

    public_workflow_report_path = ROOT / str(suite_summary.get("report_path", ""))
    public_workflow_report = load_json(public_workflow_report_path)
    expect(public_workflow_report.get("status") == "PASS", "public workflow report did not pass")

    acceptance_suite_surface = suite_summary.get("acceptance_suite_surface")
    expect(
        isinstance(acceptance_suite_surface, dict),
        "shared harness suite summary did not publish acceptance_suite_surface",
    )
    runtime_acceptance_report_path = ROOT / str(acceptance_suite_surface.get("report_path", ""))
    runtime_acceptance_report = load_json(runtime_acceptance_report_path)
    expect(runtime_acceptance_report.get("status") == "PASS", "runtime acceptance report did not pass")

    proof_packet = load_json(PROOF_PACKET_PATH)
    expect(
        proof_packet.get("contract_id") == PROOF_PACKET_CONTRACT_ID,
        "unexpected runtime architecture proof packet contract id",
    )
    expect(proof_packet.get("status") == "PASS", "runtime architecture proof packet did not pass")
    proof_packet_surface = proof_packet.get("proof_packet_surface")
    expect(isinstance(proof_packet_surface, dict), "proof packet did not publish proof_packet_surface")
    expect(
        proof_packet_surface.get("requires_compile_coupled_child_reports") is True,
        "proof packet no longer requires compile-coupled child reports",
    )

    suite_claim_boundary = suite_summary.get("claim_boundary")
    public_claim_boundary = public_workflow_report.get("claim_boundary")
    runtime_claim_boundary = runtime_acceptance_report.get("claim_boundary")
    proof_claim_boundary = proof_packet.get("composite_claim_boundary")
    proof_runtime_claim_boundary = proof_packet.get("runtime_acceptance_claim_boundary")
    expect(isinstance(suite_claim_boundary, dict), "shared harness suite summary did not publish claim_boundary")
    expect(isinstance(public_claim_boundary, dict), "public workflow report did not publish claim_boundary")
    expect(isinstance(runtime_claim_boundary, dict), "runtime acceptance report did not publish claim_boundary")
    expect(isinstance(proof_claim_boundary, dict), "proof packet did not publish composite_claim_boundary")
    expect(
        isinstance(proof_runtime_claim_boundary, dict),
        "proof packet did not publish runtime_acceptance_claim_boundary",
    )
    expect(
        suite_claim_boundary == public_claim_boundary == proof_claim_boundary,
        "public full workflow claim boundary drifted from the integrated architecture proof packet",
    )
    expect(
        runtime_claim_boundary == proof_runtime_claim_boundary,
        "runtime acceptance claim boundary drifted from the integrated architecture proof packet",
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
        expect(isinstance(proof_surface, dict), f"proof packet did not publish {surface_key}")
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
        "proof packet did not publish child_step_report_paths",
    )

    payload = {
        "contract_id": INTEGRATION_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "integration_surface": {
            "contract_id": INTEGRATION_SURFACE_CONTRACT_ID,
            "runner_path": "scripts/check_objc3c_runtime_architecture_integration.py",
            "summary_path": repo_rel(INTEGRATION_SUMMARY_PATH),
            "harness_summary_path": repo_rel(FULL_HARNESS_SUMMARY_PATH),
            "public_workflow_report_path": repo_rel(public_workflow_report_path),
            "runtime_acceptance_report_path": repo_rel(runtime_acceptance_report_path),
            "proof_packet_path": repo_rel(PROOF_PACKET_PATH),
            "required_surface_keys": ["claim_boundary", *SURFACE_KEYS],
            "required_step_actions": list(REQUIRED_STEP_ACTIONS),
            "requires_compile_coupled_full_workflow": True,
            "proof_packet_must_match_full_workflow": True,
        },
        "claim_boundary": public_claim_boundary,
        "runtime_acceptance_claim_boundary": runtime_claim_boundary,
        "runtime_state_publication_surface": public_workflow_report["runtime_state_publication_surface"],
        "acceptance_suite_surface": public_workflow_report["acceptance_suite_surface"],
        "runtime_object_model_realization_source_surface": public_workflow_report[
            "runtime_object_model_realization_source_surface"
        ],
        "runtime_property_ivar_storage_accessor_source_surface": public_workflow_report[
            "runtime_property_ivar_storage_accessor_source_surface"
        ],
        "runtime_realization_lowering_reflection_artifact_surface": public_workflow_report[
            "runtime_realization_lowering_reflection_artifact_surface"
        ],
        "runtime_dispatch_table_reflection_record_lowering_surface": public_workflow_report[
            "runtime_dispatch_table_reflection_record_lowering_surface"
        ],
        "runtime_cross_module_realized_metadata_replay_preservation_surface": public_workflow_report[
            "runtime_cross_module_realized_metadata_replay_preservation_surface"
        ],
        "runtime_object_model_abi_query_surface": public_workflow_report[
            "runtime_object_model_abi_query_surface"
        ],
        "runtime_realization_lookup_reflection_implementation_surface": public_workflow_report[
            "runtime_realization_lookup_reflection_implementation_surface"
        ],
        "runtime_reflection_query_surface": public_workflow_report[
            "runtime_reflection_query_surface"
        ],
        "runtime_realization_lookup_semantics_surface": public_workflow_report[
            "runtime_realization_lookup_semantics_surface"
        ],
        "runtime_class_metaclass_protocol_realization_surface": public_workflow_report[
            "runtime_class_metaclass_protocol_realization_surface"
        ],
        "runtime_category_attachment_merged_dispatch_surface": public_workflow_report[
            "runtime_category_attachment_merged_dispatch_surface"
        ],
        "runtime_reflection_visibility_coherence_diagnostics_surface": public_workflow_report[
            "runtime_reflection_visibility_coherence_diagnostics_surface"
        ],
        "runtime_installation_abi_surface": public_workflow_report["runtime_installation_abi_surface"],
        "runtime_loader_lifecycle_surface": public_workflow_report[
            "runtime_loader_lifecycle_surface"
        ],
        "full_workflow_step_actions": observed_actions,
        "full_workflow_child_report_paths": child_report_paths,
        "proof_packet_child_report_paths": proof_child_report_paths,
        "proof_chain": [
            {
                "kind": "public-workflow-report",
                "report_path": repo_rel(public_workflow_report_path),
                "action": public_workflow_report.get("action"),
                "step_count": len(public_workflow_report.get("steps", [])),
            },
            {
                "kind": "shared-harness-summary",
                "report_path": repo_rel(FULL_HARNESS_SUMMARY_PATH),
                "suite_id": suite_summary.get("suite_id"),
                "suite_count": harness_summary.get("suite_count"),
            },
            {
                "kind": "runtime-architecture-proof-packet",
                "report_path": repo_rel(PROOF_PACKET_PATH),
                "proof_chain_length": len(proof_packet.get("proof_chain", [])),
            },
            {
                "kind": "runtime-acceptance-report",
                "report_path": repo_rel(runtime_acceptance_report_path),
                "case_count": runtime_acceptance_report.get("case_count"),
            },
        ],
    }
    INTEGRATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    INTEGRATION_SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(INTEGRATION_SUMMARY_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
