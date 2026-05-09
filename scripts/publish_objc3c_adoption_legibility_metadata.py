#!/usr/bin/env python3
"""Publish evaluator-facing adoption and migration metadata."""

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
INTEGRATION_CHECK = ROOT / "scripts" / "check_objc3c_adoption_legibility_integration.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "adoption-legibility-evidence.json"
PUBLICATION_ARTIFACT = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "evaluator-publication.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "publication-summary.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "integration-summary.json"

EXPECTED_PUBLIC_ACTIONS = [
    "validate-adoption-legibility",
    "publish-adoption-legibility",
]
PACKAGE_BRIDGE = "objc3c"
PUBLICATION_ID = "objc3c.adoption_legibility.evaluator_publication.v1"




def ensure_integration() -> None:
    if INTEGRATION_SUMMARY.is_file():
        summary = load_json(INTEGRATION_SUMMARY)
        if summary.get("status") == "PASS" and EVIDENCE_ARTIFACT.is_file():
            return
    result = subprocess.run(
        python_script_command(INTEGRATION_CHECK),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        raise RuntimeError("adoption-legibility integration failed during publication")


def main() -> int:
    ensure_integration()
    evidence = load_json(EVIDENCE_ARTIFACT)
    claim_audit = evidence.get("claim_audit", {})
    if not isinstance(claim_audit, dict):
        raise RuntimeError("evidence claim_audit must be an object")
    release_blockers = claim_audit.get("release_blockers", [])
    if release_blockers:
        raise RuntimeError(f"cannot publish adoption metadata with release blockers: {release_blockers}")

    evaluator_path = evidence.get("evaluator_path", {}) if isinstance(evidence.get("evaluator_path"), dict) else {}
    boundary_inventory = evidence.get("boundary_inventory", {}) if isinstance(evidence.get("boundary_inventory"), dict) else {}
    artifact_contract = evidence.get("artifact_contract", {}) if isinstance(evidence.get("artifact_contract"), dict) else {}
    public_workflow = evidence.get("public_workflow", {}) if isinstance(evidence.get("public_workflow"), dict) else {}
    migration = evidence.get("migration_playbook", {}) if isinstance(evidence.get("migration_playbook"), dict) else {}
    comparison = evidence.get("comparison_matrix", {}) if isinstance(evidence.get("comparison_matrix"), dict) else {}
    onboarding = evidence.get("onboarding", {}) if isinstance(evidence.get("onboarding"), dict) else {}
    candidate_claims = evidence.get("candidate_claims", {}) if isinstance(evidence.get("candidate_claims"), dict) else {}

    publication = dict(evidence)
    publication["published_at_utc"] = datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")
    publication["evaluator_publication"] = {
        "publication_id": PUBLICATION_ID,
        "support_state": claim_audit.get("support_state"),
        "public_actions": public_workflow.get("public_actions", EXPECTED_PUBLIC_ACTIONS),
        "package_bridge": public_workflow.get("package_bridge", PACKAGE_BRIDGE),
        "operator_runbook": "docs/runbooks/objc3c_adoption_legibility.md",
        "entrypoints": evaluator_path.get("entrypoints", []),
        "required_actions": evaluator_path.get("required_actions", []),
        "boundary_inventory": boundary_inventory,
        "artifact_contract": artifact_contract,
        "migration_phases": migration.get("phases", []),
        "interop_axes": migration.get("interop_axes", []),
        "comparison_axes": comparison.get("axes", []),
        "onboarding_tutorials": onboarding.get("tutorials", []),
        "showcase_workspaces": onboarding.get("showcase_workspaces", []),
        "candidate_claim_classes": candidate_claims.get("claim_classes", []),
        "forbidden_claims": candidate_claims.get("forbidden_claims", []),
        "demoted_or_out_of_scope_claims": claim_audit.get("demoted_or_out_of_scope_claims", []),
    }

    PUBLICATION_ARTIFACT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PUBLICATION_ARTIFACT, publication)

    summary = {
        "contract_id": "objc3c.adoption_legibility.publication.summary.v1",
        "status": "PASS",
        "runner_path": "scripts/publish_objc3c_adoption_legibility_metadata.py",
        "evidence_artifact": repo_rel(EVIDENCE_ARTIFACT),
        "publication_artifact": repo_rel(PUBLICATION_ARTIFACT),
        "integration_summary": repo_rel(INTEGRATION_SUMMARY),
        "publication_id": PUBLICATION_ID,
        "support_state": claim_audit.get("support_state"),
        "public_actions": EXPECTED_PUBLIC_ACTIONS,
        "package_bridge": PACKAGE_BRIDGE,
        "entrypoint_count": len(publication["evaluator_publication"]["entrypoints"]),
        "boundary_surface_count": len(boundary_inventory.get("primary_evaluator_surfaces", [])),
        "migration_phase_count": len(publication["evaluator_publication"]["migration_phases"]),
        "comparison_axis_count": len(publication["evaluator_publication"]["comparison_axes"]),
        "release_blocker_count": len(release_blockers) if isinstance(release_blockers, list) else 0,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"publication_artifact: {repo_rel(PUBLICATION_ARTIFACT)}")
    print("objc3c-adoption-legibility-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
