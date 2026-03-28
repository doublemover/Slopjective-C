#!/usr/bin/env python3
"""Emit an integrated proof packet for the live objc3 runtime architecture."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
HARNESS_SCRIPT = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
HARNESS_SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "runtime"
    / "shared-executable-acceptance-harness"
    / "public-test-fast"
    / "summary.json"
)
PROOF_PACKET_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "architecture-proof" / "summary.json"
)
PROOF_PACKET_CONTRACT_ID = "objc3c.runtime.architecture.proof.packet.v1"
PROOF_PACKET_SURFACE_CONTRACT_ID = "objc3c.runtime.architecture.proof.packet.surface.v1"
SURFACE_KEYS = (
    "runtime_state_publication_surface",
    "acceptance_suite_surface",
    "runtime_object_model_realization_source_surface",
    "runtime_realization_lowering_reflection_artifact_surface",
    "runtime_reflection_query_surface",
    "runtime_realization_lookup_semantics_surface",
    "runtime_class_metaclass_protocol_realization_surface",
    "runtime_category_attachment_merged_dispatch_surface",
    "runtime_reflection_visibility_coherence_diagnostics_surface",
    "runtime_installation_abi_surface",
    "runtime_loader_lifecycle_surface",
)
CLAIM_BOUNDARY_CONTRACT_ID = "objc3c.runtime.execution.claim.boundary.v1"


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


def main() -> int:
    result = run_capture(
        [sys.executable, str(HARNESS_SCRIPT), "--run-suite", "public-test-fast"]
    )
    if result.returncode != 0:
        raise RuntimeError("shared executable acceptance harness failed for public-test-fast")

    harness_summary = load_json(HARNESS_SUMMARY_PATH)
    expect(
        harness_summary.get("contract_id") == "objc3c.shared.executable.acceptance.harness.summary.v1",
        "unexpected shared harness summary contract id",
    )
    expect(harness_summary.get("status") == "PASS", "shared harness summary did not pass")
    suites = harness_summary.get("suites")
    expect(isinstance(suites, list) and len(suites) == 1, "shared harness summary did not publish exactly one suite result")
    suite_summary = suites[0]
    expect(isinstance(suite_summary, dict), "shared harness suite summary was not a JSON object")
    expect(suite_summary.get("suite_id") == "public-test-fast", "shared harness summary drifted from public-test-fast")

    public_workflow_report_path = ROOT / str(suite_summary.get("report_path", ""))
    public_workflow_report = load_json(public_workflow_report_path)
    expect(public_workflow_report.get("status") == "PASS", "public workflow report did not pass")

    acceptance_suite_surface = suite_summary.get("acceptance_suite_surface")
    expect(isinstance(acceptance_suite_surface, dict), "shared harness suite summary did not publish acceptance_suite_surface")
    runtime_acceptance_report_path = ROOT / str(acceptance_suite_surface.get("report_path", ""))
    runtime_acceptance_report = load_json(runtime_acceptance_report_path)
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
    packet = {
        "contract_id": PROOF_PACKET_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "proof_packet_surface": {
            "contract_id": PROOF_PACKET_SURFACE_CONTRACT_ID,
            "runner_path": "scripts/check_objc3c_runtime_architecture_proof_packet.py",
            "packet_path": repo_rel(PROOF_PACKET_PATH),
            "harness_summary_path": repo_rel(HARNESS_SUMMARY_PATH),
            "public_workflow_report_path": repo_rel(public_workflow_report_path),
            "runtime_acceptance_report_path": repo_rel(runtime_acceptance_report_path),
            "required_surface_keys": ["claim_boundary", *SURFACE_KEYS],
            "claim_boundary_alignment_rule": "composite-claim-boundary-must-match-between-shared-harness-and-public-workflow-and-must-carry-the-runtime-acceptance-suite-as-an-authoritative-child-surface",
            "child_step_report_paths": child_report_paths,
            "requires_compile_coupled_child_reports": True,
        },
        "composite_claim_boundary": public_workflow_report["claim_boundary"],
        "runtime_acceptance_claim_boundary": runtime_acceptance_report["claim_boundary"],
        "runtime_state_publication_surface": runtime_acceptance_report["runtime_state_publication_surface"],
        "acceptance_suite_surface": runtime_acceptance_report["acceptance_suite_surface"],
        "runtime_object_model_realization_source_surface": runtime_acceptance_report[
            "runtime_object_model_realization_source_surface"
        ],
        "runtime_realization_lowering_reflection_artifact_surface": runtime_acceptance_report[
            "runtime_realization_lowering_reflection_artifact_surface"
        ],
        "runtime_reflection_query_surface": runtime_acceptance_report[
            "runtime_reflection_query_surface"
        ],
        "runtime_realization_lookup_semantics_surface": runtime_acceptance_report[
            "runtime_realization_lookup_semantics_surface"
        ],
        "runtime_class_metaclass_protocol_realization_surface": runtime_acceptance_report[
            "runtime_class_metaclass_protocol_realization_surface"
        ],
        "runtime_category_attachment_merged_dispatch_surface": runtime_acceptance_report[
            "runtime_category_attachment_merged_dispatch_surface"
        ],
        "runtime_reflection_visibility_coherence_diagnostics_surface": runtime_acceptance_report[
            "runtime_reflection_visibility_coherence_diagnostics_surface"
        ],
        "runtime_installation_abi_surface": runtime_acceptance_report["runtime_installation_abi_surface"],
        "runtime_loader_lifecycle_surface": runtime_acceptance_report[
            "runtime_loader_lifecycle_surface"
        ],
        "proof_chain": [
            {
                "kind": "runtime-acceptance-report",
                "report_path": repo_rel(runtime_acceptance_report_path),
                "case_count": runtime_acceptance_report.get("case_count"),
            },
            {
                "kind": "public-workflow-report",
                "report_path": repo_rel(public_workflow_report_path),
                "action": public_workflow_report.get("action"),
                "step_count": len(public_workflow_report.get("steps", [])),
            },
            {
                "kind": "shared-harness-summary",
                "report_path": repo_rel(HARNESS_SUMMARY_PATH),
                "suite_id": suite_summary.get("suite_id"),
                "suite_count": harness_summary.get("suite_count"),
            },
        ],
    }
    PROOF_PACKET_PATH.parent.mkdir(parents=True, exist_ok=True)
    PROOF_PACKET_PATH.write_text(json.dumps(packet, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(PROOF_PACKET_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
