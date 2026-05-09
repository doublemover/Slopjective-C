#!/usr/bin/env python3
"""Generate adoption, replay, comparison, and onboarding evidence."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_timed


ROOT = Path(__file__).resolve().parents[1]
ARTIFACT_PATH = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "adoption-legibility-evidence.json"
PUBLICATION_PATH = ROOT / "tmp" / "artifacts" / "adoption-legibility" / "evaluator-publication.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "evidence-summary.json"

BOUNDARY_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "boundary_inventory.json"
PUBLIC_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "public_claim_policy.json"
COMPARISON_SEMANTICS = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "capability_comparison_semantics.json"
ADOPTION_REPLAY_SEMANTICS = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "adoption_replay_semantics.json"
ARTIFACT_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "artifact_contract.json"

BOUNDARY_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "boundary-inventory-summary.json"
PUBLIC_CLAIM_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "public-claim-policy-summary.json"
COMPARISON_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "capability-comparison-summary.json"
ADOPTION_REPLAY_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "adoption-replay-summary.json"
ARTIFACT_CONTRACT_SUMMARY = ROOT / "tmp" / "reports" / "adoption-legibility" / "artifact-contract-summary.json"

CONTRACT_ID = "objc3c.adoption_legibility.evidence.v1"
SUPPORT_STATE = "evaluator-ready-with-same-major-adoption-replay-and-evidence-linked-comparison"
PACKAGE_BRIDGE = "objc3c"
PUBLIC_ACTIONS = [
    "validate-adoption-legibility",
    "publish-adoption-legibility",
]
OWNER_SPLIT = {
    "boundary_inventory": "tests/tooling/fixtures/adoption_legibility/boundary_inventory.json",
    "artifact_contract": "tests/tooling/fixtures/adoption_legibility/artifact_contract.json",
    "capability_comparison": "tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json",
    "adoption_replay": "tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json",
    "public_claim_policy": "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json",
    "metadata_publication": "scripts/publish_objc3c_adoption_legibility_metadata.py",
}
OWNER_CONTRACTS = {
    "public_claim_owner": {
        "source_contract": OWNER_SPLIT["public_claim_policy"],
        "artifact_section": "candidate_claims",
        "publication_projection": "evaluator_publication.candidate_claim_classes",
        "blocker_projection": "claim_audit.blocker_metadata.public_claim_policy",
    },
    "adoption_comparison_owner": {
        "source_contract": OWNER_SPLIT["capability_comparison"],
        "artifact_section": "comparison_matrix",
        "publication_projection": "evaluator_publication.comparison_axes",
        "blocker_projection": "claim_audit.blocker_metadata.capability_comparison",
    },
    "adoption_replay_owner": {
        "source_contract": OWNER_SPLIT["adoption_replay"],
        "artifact_section": "adoption_replay",
        "publication_projection": "evaluator_publication.adoption_replay_phases",
        "blocker_projection": "claim_audit.blocker_metadata.adoption_replay",
    },
    "publication_owner": {
        "source_contract": OWNER_SPLIT["metadata_publication"],
        "artifact_section": "public_workflow",
        "publication_projection": "evaluator_publication",
        "blocker_projection": "claim_audit.blocker_metadata.metadata_publication",
    },
}
BLOCKER_METADATA = {
    "public_claim_policy": {
        "owner": "public_claim_owner",
        "blocked_when": "candidate claim omits support class, evidence paths, or forbidden-claim demotion",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "capability_comparison": {
        "owner": "adoption_comparison_owner",
        "blocked_when": "comparison wording widens beyond conformance, performance, package, support, and checked-in example evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "adoption_replay": {
        "owner": "adoption_replay_owner",
        "blocked_when": "same-major adoption replay lacks replay fields, package bridge, rollback target, or public workflow action",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "metadata_publication": {
        "owner": "publication_owner",
        "blocked_when": "publication widens claims beyond generated adoption evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
}

STEPS = [
    ("boundary-inventory", python_script_command("scripts/build_adoption_legibility_boundary_inventory_summary.py")),
    ("public-claim-policy", python_script_command("scripts/build_adoption_legibility_public_claim_policy_summary.py")),
    ("capability-comparison", python_script_command("scripts/build_adoption_legibility_capability_comparison_summary.py")),
    ("adoption-replay", python_script_command("scripts/build_adoption_legibility_adoption_replay_summary.py")),
    ("artifact-contract", python_script_command("scripts/build_adoption_legibility_artifact_contract_summary.py")),
]




def run_step(name: str, command: list[str]) -> dict[str, object]:
    result = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "duration_ms": result.duration_ms,
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
        "adoption_replay": ADOPTION_REPLAY_SUMMARY,
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
    adoption_replay_semantics = load_json(ADOPTION_REPLAY_SEMANTICS)
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
    adoption_replay_phases = [
        str(phase.get("phase_id"))
        for phase in adoption_replay_semantics.get("adoption_replay_phases", [])
        if isinstance(phase, dict)
    ]
    adoption_replay_actions = sorted(
        {
            str(action)
            for phase in adoption_replay_semantics.get("adoption_replay_phases", [])
            if isinstance(phase, dict)
            for action in phase.get("required_actions", [])
        }
    )
    interop_axes = [
        str(axis.get("axis_id"))
        for axis in adoption_replay_semantics.get("interop_guidance_axes", [])
        if isinstance(axis, dict)
    ]
    package_actions = [
        action
        for action in adoption_replay_actions
        if "package" in action or "application" in action or "release" in action or "long-horizon" in action
    ]

    boundary_summary = reports.get("boundary", {})
    public_claim_summary = reports.get("public_claim", {})
    support_classes = [str(value) for value in public_claim_summary.get("support_classes", [])]
    release_blockers = list(failures)

    artifact = {
        "contract_id": CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "generated_from": {
            "owner_split": OWNER_SPLIT,
            "owner_contracts": OWNER_CONTRACTS,
            "contracts": [
                repo_rel(BOUNDARY_CONTRACT),
                repo_rel(PUBLIC_CLAIM_POLICY),
                repo_rel(COMPARISON_SEMANTICS),
                repo_rel(ADOPTION_REPLAY_SEMANTICS),
                repo_rel(ARTIFACT_CONTRACT),
            ],
            "commands": [" ".join(command) for _, command in STEPS],
        },
        "owner_contracts": OWNER_CONTRACTS,
        "public_workflow": {
            "status": "PASS" if not release_blockers else "FAIL",
            "public_actions": PUBLIC_ACTIONS,
            "package_bridge": PACKAGE_BRIDGE,
            "metadata_publisher": "scripts/publish_objc3c_adoption_legibility_metadata.py",
            "integration_checker": "scripts/check_objc3c_adoption_legibility_integration.py",
        },
        "boundary_inventory": {
            "status": "PASS" if status_passes(boundary_summary) and not release_blockers else "FAIL",
            "working_scope": boundary.get("working_scope", []),
            "non_goals": boundary.get("non_goals", []),
            "primary_evaluator_surfaces": boundary.get("primary_evaluator_surfaces", []),
            "substrate_runbooks": boundary.get("substrate_runbooks", []),
            "machine_owned_output_roots": boundary.get("machine_owned_output_roots", []),
            "successor_surfaces": boundary.get("successor_surfaces", []),
        },
        "artifact_contract": {
            "status": "PASS" if status_passes(reports.get("artifact_contract", {})) and not release_blockers else "FAIL",
            "schema": artifact_contract.get("schema"),
            "artifact_root": artifact_contract.get("artifact_root"),
            "report_root": artifact_contract.get("report_root"),
            "generated_artifacts": artifact_contract.get("generated_artifacts", []),
            "generated_reports": artifact_contract.get("generated_reports", []),
            "claim_rules": artifact_contract.get("claim_rules", []),
        },
        "evaluator_path": {
            "status": "PASS" if not release_blockers else "FAIL",
            "entrypoints": boundary.get("primary_evaluator_surfaces", []),
            "package_bridge": boundary.get("package_bridge"),
            "required_actions": boundary.get("required_actions", []),
            "tutorial_doc_count": boundary_summary.get("tutorial_doc_count"),
            "showcase_source_count": boundary_summary.get("showcase_source_count"),
        },
        "adoption_replay": {
            "status": "PASS" if not release_blockers else "FAIL",
            "phases": adoption_replay_phases,
            "interop_axes": interop_axes,
            "replay_fields": adoption_replay_semantics.get("required_adoption_replay_fields", []),
            "package_bridge": adoption_replay_semantics.get("package_bridge"),
            "required_actions": adoption_replay_actions,
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
            "package_actions": package_actions,
        },
        "candidate_claims": {
            "status": "PASS" if not release_blockers else "FAIL",
            "claim_classes": support_classes,
            "publication_fields": public_claim_policy.get("required_publication_fields", []),
            "forbidden_claims": public_claim_policy.get("forbidden_claims", []),
            "deferred_behavior_policy": public_claim_policy.get("deferred_behavior_policy", {}),
        },
        "claim_audit": {
            "support_state": SUPPORT_STATE,
            "earned_claims": [
                "external evaluator path links README, site, tutorials, showcase, workflow actions, package workflows, and support evidence",
                "Objective-C 2 conversion guidance is same-major scoped and package/support-window aware",
                "Swift and C++ comparison guidance is evidence-linked and parity-claim guarded",
                "onboarding uses checked-in tutorials, showcase workspaces, and objc3c workflow actions",
            ],
            "demoted_or_out_of_scope_claims": [
                "drop-in Objective-C 2 replacement",
                "zero-risk conversion",
                "performance leadership",
                "hosted registry or IDE marketplace parity",
                "private maintainer context as an evaluator prerequisite",
            ],
            "blocker_metadata": BLOCKER_METADATA,
            "release_blockers": release_blockers,
        },
    }
    ARTIFACT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(ARTIFACT_PATH, artifact)

    publication = dict(artifact)
    publication["publication_view"] = {
        "entrypoints": artifact["evaluator_path"]["entrypoints"],
        "adoption_replay_phases": adoption_replay_phases,
        "comparison_axes": comparison_axes,
        "support_state": SUPPORT_STATE,
        "artifact_contract": repo_rel(ARTIFACT_CONTRACT),
        "artifact_root": artifact_contract.get("artifact_root"),
        "report_root": artifact_contract.get("report_root"),
        "public_actions": PUBLIC_ACTIONS,
    }
    PUBLICATION_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PUBLICATION_PATH, publication)

    summary = {
        "contract_id": "objc3c.adoption_legibility.evidence.summary.v1",
        "status": "PASS" if not release_blockers else "FAIL",
        "runner_path": "scripts/build_objc3c_adoption_legibility_evidence.py",
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "artifact_path": repo_rel(ARTIFACT_PATH),
        "publication_path": repo_rel(PUBLICATION_PATH),
        "owner_contract_count": len(OWNER_CONTRACTS),
        "blocker_metadata_count": len(BLOCKER_METADATA),
        "step_count": len(steps),
        "steps": steps,
        "report_count": len(required_reports),
        "evaluator_entrypoint_count": len(artifact["evaluator_path"]["entrypoints"]),
        "required_action_count": len(artifact["evaluator_path"]["required_actions"]),
        "adoption_replay_phase_count": len(adoption_replay_phases),
        "interop_axis_count": len(interop_axes),
        "comparison_axis_count": len(comparison_axes),
        "onboarding_tutorial_count": len(artifact["onboarding"]["tutorials"]),
        "showcase_workspace_count": len(artifact["onboarding"]["showcase_workspaces"]),
        "support_state": artifact["claim_audit"]["support_state"],
        "failures": release_blockers,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"artifact_path: {repo_rel(ARTIFACT_PATH)}")
    print(f"publication_path: {repo_rel(PUBLICATION_PATH)}")
    print("objc3c-adoption-legibility-evidence: PASS" if not release_blockers else "objc3c-adoption-legibility-evidence: FAIL")
    return 0 if not release_blockers else 1


if __name__ == "__main__":
    raise SystemExit(main())
