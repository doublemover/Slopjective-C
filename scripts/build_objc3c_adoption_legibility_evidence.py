#!/usr/bin/env python3
"""Generate adoption, migration, comparison, and onboarding evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
ARTIFACT_PATH = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "adoption-legibility-evidence.json"
PUBLICATION_PATH = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "evaluator-publication.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "evidence-summary.json"

BOUNDARY_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "boundary_inventory.json"
PUBLIC_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "public_claim_policy.json"
COMPARISON_SEMANTICS = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "capability_comparison_semantics.json"
MIGRATION_SEMANTICS = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "migration_playbook_semantics.json"
ARTIFACT_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "artifact_contract.json"

BOUNDARY_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "boundary-inventory-summary.json"
PUBLIC_CLAIM_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "public-claim-policy-summary.json"
COMPARISON_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "capability-comparison-summary.json"
MIGRATION_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "migration-playbook-summary.json"
ARTIFACT_CONTRACT_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "artifact-contract-summary.json"

STEPS = [
    ("boundary-inventory", [sys.executable, "scripts/build_adoption_legibility_boundary_inventory_summary.py"]),
    ("public-claim-policy", [sys.executable, "scripts/build_adoption_legibility_public_claim_policy_summary.py"]),
    ("capability-comparison", [sys.executable, "scripts/build_adoption_legibility_capability_comparison_summary.py"]),
    ("migration-playbook", [sys.executable, "scripts/build_adoption_legibility_migration_playbook_summary.py"]),
    ("artifact-contract", [sys.executable, "scripts/build_adoption_legibility_artifact_contract_summary.py"]),
]


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def run_step(name: str, command: list[str]) -> dict[str, object]:
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def status_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    failures: list[str] = []
    steps: list[dict[str, object]] = []
    for name, command in STEPS:
        step = run_step(name, command)
        steps.append(step)
        expect(step["exit_code"] == 0, f"{name} failed", failures)
        if step["exit_code"] != 0:
            break

    required_reports = {
        "boundary": BOUNDARY_SUMMARY,
        "public_claim": PUBLIC_CLAIM_SUMMARY,
        "comparison": COMPARISON_SUMMARY,
        "migration": MIGRATION_SUMMARY,
        "artifact_contract": ARTIFACT_CONTRACT_SUMMARY,
    }
    for name, path in required_reports.items():
        expect(path.is_file(), f"missing {name} report {repo_rel(path)}", failures)

    reports = {name: load_json(path) for name, path in required_reports.items() if path.is_file()}
    for name, payload in reports.items():
        expect(status_passes(payload), f"{name} report did not pass", failures)

    boundary = load_json(BOUNDARY_CONTRACT)
    public_claim_policy = load_json(PUBLIC_CLAIM_POLICY)
    comparison_semantics = load_json(COMPARISON_SEMANTICS)
    migration_semantics = load_json(MIGRATION_SEMANTICS)
    artifact_contract = load_json(ARTIFACT_CONTRACT)

    comparison_axes = [
        str(axis.get("axis_id"))
        for axis in comparison_semantics.get("comparison_axes", [])
        if isinstance(axis, dict)
    ]
    comparison_evidence_paths = sorted(
        {
            str(path)
            for axis in comparison_semantics.get("comparison_axes", [])
            if isinstance(axis, dict)
            for path in axis.get("required_evidence", [])
        }
    )
    migration_phases = [
        str(phase.get("phase_id"))
        for phase in migration_semantics.get("playbook_phases", [])
        if isinstance(phase, dict)
    ]
    migration_public_scripts = sorted(
        {
            str(script)
            for phase in migration_semantics.get("playbook_phases", [])
            if isinstance(phase, dict)
            for script in phase.get("required_public_scripts", [])
        }
    )
    interop_axes = [
        str(axis.get("axis_id"))
        for axis in migration_semantics.get("interop_guidance_axes", [])
        if isinstance(axis, dict)
    ]
    package_commands = [
        script
        for script in migration_public_scripts
        if "package" in script or "application" in script or "release" in script or "long-horizon" in script
    ]

    boundary_summary = reports.get("boundary", {})
    public_claim_summary = reports.get("public_claim", {})
    support_classes = [str(value) for value in public_claim_summary.get("support_classes", [])]
    release_blockers = list(failures)

    artifact = {
        "contract_id": "objc3c.adoption_legibility.evidence.v1",
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "generated_from": {
            "contracts": [
                repo_rel(BOUNDARY_CONTRACT),
                repo_rel(PUBLIC_CLAIM_POLICY),
                repo_rel(COMPARISON_SEMANTICS),
                repo_rel(MIGRATION_SEMANTICS),
                repo_rel(ARTIFACT_CONTRACT),
            ],
            "commands": [" ".join(command) for _, command in STEPS],
        },
        "evaluator_path": {
            "status": "PASS" if not release_blockers else "FAIL",
            "entrypoints": boundary.get("primary_evaluator_surfaces", []),
            "public_commands": boundary.get("required_existing_public_scripts", []),
            "tutorial_doc_count": boundary_summary.get("tutorial_doc_count"),
            "showcase_source_count": boundary_summary.get("showcase_source_count"),
        },
        "migration_playbook": {
            "status": "PASS" if not release_blockers else "FAIL",
            "phases": migration_phases,
            "interop_axes": interop_axes,
            "replay_fields": migration_semantics.get("required_migration_replay_fields", []),
            "public_commands": migration_public_scripts,
        },
        "comparison_matrix": {
            "status": "PASS" if not release_blockers else "FAIL",
            "axes": comparison_axes,
            "adjacent_ecosystems": reports.get("comparison", {}).get("adjacent_ecosystems", []),
            "evidence_paths": comparison_evidence_paths,
        },
        "onboarding": {
            "status": "PASS" if not release_blockers else "FAIL",
            "tutorials": boundary_summary.get("tutorial_docs", []),
            "showcase_workspaces": boundary_summary.get("showcase_workspaces", []),
            "package_commands": package_commands,
        },
        "candidate_claims": {
            "status": "PASS" if not release_blockers else "FAIL",
            "claim_classes": support_classes,
            "publication_fields": public_claim_policy.get("required_publication_fields", []),
            "forbidden_claims": public_claim_policy.get("forbidden_claims", []),
        },
        "claim_audit": {
            "support_state": "evaluator-ready-with-same-major-migration-and-evidence-linked-comparison",
            "earned_claims": [
                "external evaluator path links README, site, tutorials, showcase, commands, package workflows, and support evidence",
                "Objective-C 2 migration guidance is same-major scoped and package/support-window aware",
                "Swift and C++ comparison guidance is evidence-linked and parity-claim guarded",
                "onboarding uses checked-in tutorials, showcase workspaces, and public package scripts",
            ],
            "demoted_or_out_of_scope_claims": [
                "drop-in Objective-C 2 replacement",
                "zero-risk migration",
                "performance leadership",
                "hosted registry or IDE marketplace parity",
                "private maintainer context as an evaluator prerequisite",
            ],
            "release_blockers": release_blockers,
        },
    }
    ARTIFACT_PATH.parent.mkdir(parents=True, exist_ok=True)
    ARTIFACT_PATH.write_text(json.dumps(artifact, indent=2) + "\n", encoding="utf-8")

    publication = dict(artifact)
    publication["publication_view"] = {
        "entrypoints": artifact["evaluator_path"]["entrypoints"],
        "migration_phases": migration_phases,
        "comparison_axes": comparison_axes,
        "support_state": artifact["claim_audit"]["support_state"],
        "artifact_contract": repo_rel(ARTIFACT_CONTRACT),
        "artifact_root": artifact_contract.get("artifact_root"),
        "report_root": artifact_contract.get("report_root"),
    }
    PUBLICATION_PATH.parent.mkdir(parents=True, exist_ok=True)
    PUBLICATION_PATH.write_text(json.dumps(publication, indent=2) + "\n", encoding="utf-8")

    summary = {
        "contract_id": "objc3c.adoption_legibility.evidence.summary.v1",
        "status": "PASS" if not release_blockers else "FAIL",
        "runner_path": "scripts/build_objc3c_adoption_legibility_evidence.py",
        "artifact_path": repo_rel(ARTIFACT_PATH),
        "publication_path": repo_rel(PUBLICATION_PATH),
        "step_count": len(steps),
        "steps": steps,
        "report_count": len(required_reports),
        "evaluator_entrypoint_count": len(artifact["evaluator_path"]["entrypoints"]),
        "public_command_count": len(artifact["evaluator_path"]["public_commands"]),
        "migration_phase_count": len(migration_phases),
        "interop_axis_count": len(interop_axes),
        "comparison_axis_count": len(comparison_axes),
        "onboarding_tutorial_count": len(artifact["onboarding"]["tutorials"]),
        "showcase_workspace_count": len(artifact["onboarding"]["showcase_workspaces"]),
        "support_state": artifact["claim_audit"]["support_state"],
        "failures": release_blockers,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"artifact_path: {repo_rel(ARTIFACT_PATH)}")
    print(f"publication_path: {repo_rel(PUBLICATION_PATH)}")
    print("objc3c-adoption-legibility-evidence: PASS" if not release_blockers else "objc3c-adoption-legibility-evidence: FAIL")
    return 0 if not release_blockers else 1


if __name__ == "__main__":
    raise SystemExit(main())
