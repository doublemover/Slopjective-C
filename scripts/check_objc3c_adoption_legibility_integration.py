#!/usr/bin/env python3
"""Validate the integrated adoption and evaluator-legibility workflow."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command


ROOT = Path(__file__).resolve().parents[1]
EVIDENCE_BUILDER = ROOT / "scripts" / "build_objc3c_adoption_legibility_evidence.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "adoption-legibility-evidence.json"
PUBLICATION_ARTIFACT = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "evaluator-publication.json"
EVIDENCE_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "evidence-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "integration-summary.json"
PACKAGE_JSON = ROOT / "package.json"

EXPECTED_PUBLIC_ACTIONS = [
    "validate-adoption-legibility",
    "publish-adoption-legibility",
]
EXPECTED_OWNER_SECTIONS = [
    "owner_contracts",
    "public_workflow",
    "boundary_inventory",
    "artifact_contract",
    "evaluator_path",
    "adoption_replay",
    "comparison_matrix",
    "onboarding",
    "candidate_claims",
    "claim_audit",
]



def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    result = subprocess.run(
        python_script_command(EVIDENCE_BUILDER),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
    expect(result.returncode == 0, "adoption evidence builder failed", failures)
    expect(EVIDENCE_ARTIFACT.is_file(), f"missing evidence artifact {repo_rel(EVIDENCE_ARTIFACT)}", failures)
    expect(PUBLICATION_ARTIFACT.is_file(), f"missing publication artifact {repo_rel(PUBLICATION_ARTIFACT)}", failures)
    expect(EVIDENCE_SUMMARY.is_file(), f"missing evidence summary {repo_rel(EVIDENCE_SUMMARY)}", failures)

    artifact = load_json(EVIDENCE_ARTIFACT) if EVIDENCE_ARTIFACT.is_file() else {}
    publication = load_json(PUBLICATION_ARTIFACT) if PUBLICATION_ARTIFACT.is_file() else {}
    evidence_summary = load_json(EVIDENCE_SUMMARY) if EVIDENCE_SUMMARY.is_file() else {}
    package = load_json(PACKAGE_JSON)
    scripts = package.get("scripts", {})
    expect(isinstance(scripts, dict), "package.json scripts field drifted", failures)

    expect(artifact.get("contract_id") == "objc3c.adoption_legibility.evidence.v1", "evidence artifact contract_id drifted", failures)
    expect(artifact.get("schema_version") == 1, "evidence artifact schema_version drifted", failures)
    expect(publication.get("contract_id") == "objc3c.adoption_legibility.evidence.v1", "publication artifact contract_id drifted", failures)
    expect(evidence_summary.get("status") == "PASS", "evidence summary did not report PASS", failures)
    expect("objc3c" in scripts, "public objc3c package bridge missing", failures)

    for section in EXPECTED_OWNER_SECTIONS:
        expect(isinstance(artifact.get(section), dict), f"evidence artifact missing section {section}", failures)

    public_workflow = artifact.get("public_workflow", {}) if isinstance(artifact.get("public_workflow"), dict) else {}
    boundary_inventory = artifact.get("boundary_inventory", {}) if isinstance(artifact.get("boundary_inventory"), dict) else {}
    artifact_contract = artifact.get("artifact_contract", {}) if isinstance(artifact.get("artifact_contract"), dict) else {}
    evaluator_path = artifact.get("evaluator_path", {}) if isinstance(artifact.get("evaluator_path"), dict) else {}
    adoption_replay = artifact.get("adoption_replay", {}) if isinstance(artifact.get("adoption_replay"), dict) else {}
    comparison = artifact.get("comparison_matrix", {}) if isinstance(artifact.get("comparison_matrix"), dict) else {}
    onboarding = artifact.get("onboarding", {}) if isinstance(artifact.get("onboarding"), dict) else {}
    claim_audit = artifact.get("claim_audit", {}) if isinstance(artifact.get("claim_audit"), dict) else {}
    owner_contracts = artifact.get("owner_contracts", {}) if isinstance(artifact.get("owner_contracts"), dict) else {}
    blocker_metadata = claim_audit.get("blocker_metadata", {}) if isinstance(claim_audit.get("blocker_metadata"), dict) else {}

    expect(
        {"public_claim_owner", "adoption_comparison_owner", "adoption_replay_owner", "publication_owner"}.issubset(owner_contracts),
        "adoption owner contracts are incomplete",
        failures,
    )
    expect(
        {"public_claim_policy", "capability_comparison", "adoption_replay", "metadata_publication"}.issubset(blocker_metadata),
        "adoption blocker metadata is incomplete",
        failures,
    )
    expect(public_workflow.get("public_actions") == EXPECTED_PUBLIC_ACTIONS, "adoption public action names drifted", failures)
    expect(public_workflow.get("package_bridge") == "objc3c", "adoption public workflow package bridge drifted", failures)
    expect(len(boundary_inventory.get("primary_evaluator_surfaces", [])) >= 8, "boundary inventory evaluator surface is too narrow", failures)
    expect(artifact_contract.get("schema") == "schemas/objc3c-adoption-legibility-evidence-v1.schema.json", "adoption artifact schema drifted", failures)
    expect(len(artifact_contract.get("generated_artifacts", [])) >= 2, "adoption generated artifact contract is too narrow", failures)
    expect(len(evaluator_path.get("entrypoints", [])) >= 8, "evaluator entrypoint surface is too narrow", failures)
    expect(evaluator_path.get("package_bridge") == "objc3c", "evaluator path package bridge drifted", failures)
    expect(len(evaluator_path.get("required_actions", [])) >= 10, "workflow action surface is too narrow", failures)
    expect(len(adoption_replay.get("phases", [])) >= 4, "adoption replay phase coverage is too narrow", failures)
    expect(len(adoption_replay.get("interop_axes", [])) >= 3, "interop axis coverage is too narrow", failures)
    expect(len(comparison.get("axes", [])) >= 3, "comparison axis coverage is too narrow", failures)
    expect(len(comparison.get("evidence_paths", [])) >= 10, "comparison evidence path coverage is too narrow", failures)
    expect(len(onboarding.get("tutorials", [])) >= 4, "onboarding tutorial coverage is too narrow", failures)
    expect(len(onboarding.get("showcase_workspaces", [])) >= 3, "onboarding workspace coverage is too narrow", failures)
    expect("same-major" in str(claim_audit.get("support_state", "")), "support_state is not same-major scoped", failures)
    expect(claim_audit.get("release_blockers") == [], "claim audit reported release blockers", failures)

    payload = {
        "contract_id": "objc3c.adoption_legibility.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_adoption_legibility_integration.py",
        "evidence_builder": repo_rel(EVIDENCE_BUILDER),
        "evidence_artifact": repo_rel(EVIDENCE_ARTIFACT),
        "publication_artifact": repo_rel(PUBLICATION_ARTIFACT),
        "evidence_summary": repo_rel(EVIDENCE_SUMMARY),
        "support_state": claim_audit.get("support_state"),
        "public_actions": public_workflow.get("public_actions", []),
        "owner_section_count": len(EXPECTED_OWNER_SECTIONS),
        "owner_contract_count": len(owner_contracts),
        "blocker_metadata_count": len(blocker_metadata),
        "boundary_surface_count": len(boundary_inventory.get("primary_evaluator_surfaces", [])),
        "artifact_generated_artifact_count": len(artifact_contract.get("generated_artifacts", [])),
        "evaluator_entrypoint_count": len(evaluator_path.get("entrypoints", [])),
        "required_action_count": len(evaluator_path.get("required_actions", [])),
        "adoption_replay_phase_count": len(adoption_replay.get("phases", [])),
        "interop_axis_count": len(adoption_replay.get("interop_axes", [])),
        "comparison_axis_count": len(comparison.get("axes", [])),
        "onboarding_tutorial_count": len(onboarding.get("tutorials", [])),
        "showcase_workspace_count": len(onboarding.get("showcase_workspaces", [])),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-adoption-legibility-integration: PASS" if not failures else "objc3c-adoption-legibility-integration: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
