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


ROOT = Path(__file__).resolve().parents[1]
EVIDENCE_BUILDER = ROOT / "scripts" / "build_objc3c_adoption_legibility_evidence.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "adoption-legibility-evidence.json"
PUBLICATION_ARTIFACT = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "evaluator-publication.json"
EVIDENCE_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "evidence-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "integration-summary.json"
PACKAGE_JSON = ROOT / "package.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    result = subprocess.run(
        [sys.executable, str(EVIDENCE_BUILDER)],
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

    for section in ("evaluator_path", "migration_playbook", "comparison_matrix", "onboarding", "candidate_claims", "claim_audit"):
        expect(isinstance(artifact.get(section), dict), f"evidence artifact missing section {section}", failures)

    evaluator_path = artifact.get("evaluator_path", {}) if isinstance(artifact.get("evaluator_path"), dict) else {}
    migration = artifact.get("migration_playbook", {}) if isinstance(artifact.get("migration_playbook"), dict) else {}
    comparison = artifact.get("comparison_matrix", {}) if isinstance(artifact.get("comparison_matrix"), dict) else {}
    onboarding = artifact.get("onboarding", {}) if isinstance(artifact.get("onboarding"), dict) else {}
    claim_audit = artifact.get("claim_audit", {}) if isinstance(artifact.get("claim_audit"), dict) else {}

    expect(len(evaluator_path.get("entrypoints", [])) >= 8, "evaluator entrypoint surface is too narrow", failures)
    expect(len(evaluator_path.get("public_commands", [])) >= 10, "public command surface is too narrow", failures)
    expect(len(migration.get("phases", [])) >= 4, "migration playbook phase coverage is too narrow", failures)
    expect(len(migration.get("interop_axes", [])) >= 3, "interop axis coverage is too narrow", failures)
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
        "evaluator_entrypoint_count": len(evaluator_path.get("entrypoints", [])),
        "public_command_count": len(evaluator_path.get("public_commands", [])),
        "migration_phase_count": len(migration.get("phases", [])),
        "interop_axis_count": len(migration.get("interop_axes", [])),
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
