from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_adoption_legibility_evidence.evidence_loading import AdoptionLegibilityEvidenceInputs
from objc3c_adoption_legibility_evidence.normalization import (
    adoption_replay_actions,
    adoption_replay_phases,
    comparison_axes,
    comparison_evidence_paths,
    interop_axes,
    package_actions,
)
from objc3c_adoption_legibility_evidence.paths import (
    BLOCKER_METADATA,
    CONTRACT_ID,
    OWNER_CONTRACTS,
    OWNER_SPLIT,
    PACKAGE_BRIDGE,
    PUBLIC_ACTIONS,
    SUMMARY_CONTRACT_ID,
    SUPPORT_STATE,
    AdoptionLegibilityEvidencePaths,
)
from objc3c_adoption_legibility_evidence.validation import status_passes


@dataclass(frozen=True)
class AdoptionLegibilityEvidenceModel:
    artifact: dict[str, Any]
    publication: dict[str, Any]
    summary: dict[str, Any]

    @property
    def passed(self) -> bool:
        return self.summary["status"] == "PASS"


def rel(paths: AdoptionLegibilityEvidencePaths, path: Path) -> str:
    return repo_rel(path, root=paths.root)


def generated_from_commands(paths: AdoptionLegibilityEvidencePaths) -> list[str]:
    return [" ".join(step.command) for step in paths.steps()]


def build_artifact(paths: AdoptionLegibilityEvidencePaths, inputs: AdoptionLegibilityEvidenceInputs) -> dict[str, Any]:
    reports = inputs.reports
    release_blockers = list(inputs.failures)
    boundary_summary = reports.get("boundary", {})
    public_claim_summary = reports.get("public_claim", {})
    support_classes = [str(value) for value in public_claim_summary.get("support_classes", [])]
    comparison_axis_ids = comparison_axes(inputs.comparison_semantics)
    comparison_paths = comparison_evidence_paths(inputs.comparison_semantics)
    replay_phases = adoption_replay_phases(inputs.adoption_replay_semantics)
    replay_actions = adoption_replay_actions(inputs.adoption_replay_semantics)
    replay_interop_axes = interop_axes(inputs.adoption_replay_semantics)

    return {
        "contract_id": CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "generated_from": {
            "owner_split": OWNER_SPLIT,
            "owner_contracts": OWNER_CONTRACTS,
            "contracts": [rel(paths, path) for path in paths.contracts()],
            "commands": generated_from_commands(paths),
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
            "working_scope": inputs.boundary.get("working_scope", []),
            "non_goals": inputs.boundary.get("non_goals", []),
            "primary_evaluator_surfaces": inputs.boundary.get("primary_evaluator_surfaces", []),
            "substrate_runbooks": inputs.boundary.get("substrate_runbooks", []),
            "machine_owned_output_roots": inputs.boundary.get("machine_owned_output_roots", []),
            "successor_surfaces": inputs.boundary.get("successor_surfaces", []),
        },
        "artifact_contract": {
            "status": "PASS" if status_passes(reports.get("artifact_contract", {})) and not release_blockers else "FAIL",
            "schema": inputs.artifact_contract.get("schema"),
            "artifact_root": inputs.artifact_contract.get("artifact_root"),
            "report_root": inputs.artifact_contract.get("report_root"),
            "generated_artifacts": inputs.artifact_contract.get("generated_artifacts", []),
            "generated_reports": inputs.artifact_contract.get("generated_reports", []),
            "claim_rules": inputs.artifact_contract.get("claim_rules", []),
        },
        "evaluator_path": {
            "status": "PASS" if not release_blockers else "FAIL",
            "entrypoints": inputs.boundary.get("primary_evaluator_surfaces", []),
            "package_bridge": inputs.boundary.get("package_bridge"),
            "required_actions": inputs.boundary.get("required_actions", []),
            "tutorial_doc_count": boundary_summary.get("tutorial_doc_count"),
            "showcase_source_count": boundary_summary.get("showcase_source_count"),
        },
        "adoption_replay": {
            "status": "PASS" if not release_blockers else "FAIL",
            "phases": replay_phases,
            "interop_axes": replay_interop_axes,
            "replay_fields": inputs.adoption_replay_semantics.get("required_adoption_replay_fields", []),
            "package_bridge": inputs.adoption_replay_semantics.get("package_bridge"),
            "required_actions": replay_actions,
        },
        "comparison_matrix": {
            "status": "PASS" if not release_blockers else "FAIL",
            "axes": comparison_axis_ids,
            "adjacent_ecosystems": reports.get("comparison", {}).get("adjacent_ecosystems", []),
            "evidence_paths": comparison_paths,
        },
        "onboarding": {
            "status": "PASS" if not release_blockers else "FAIL",
            "tutorials": boundary_summary.get("tutorial_docs", []),
            "showcase_workspaces": boundary_summary.get("showcase_workspaces", []),
            "package_actions": package_actions(replay_actions),
        },
        "candidate_claims": {
            "status": "PASS" if not release_blockers else "FAIL",
            "claim_classes": support_classes,
            "publication_fields": inputs.public_claim_policy.get("required_publication_fields", []),
            "forbidden_claims": inputs.public_claim_policy.get("forbidden_claims", []),
            "deferred_behavior_policy": inputs.public_claim_policy.get("deferred_behavior_policy", {}),
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


def build_publication(paths: AdoptionLegibilityEvidencePaths, artifact: dict[str, Any]) -> dict[str, Any]:
    publication = dict(artifact)
    publication["publication_view"] = {
        "entrypoints": artifact["evaluator_path"]["entrypoints"],
        "adoption_replay_phases": artifact["adoption_replay"]["phases"],
        "comparison_axes": artifact["comparison_matrix"]["axes"],
        "support_state": SUPPORT_STATE,
        "artifact_contract": rel(paths, paths.artifact_contract),
        "artifact_root": artifact["artifact_contract"]["artifact_root"],
        "report_root": artifact["artifact_contract"]["report_root"],
        "public_actions": PUBLIC_ACTIONS,
    }
    return publication


def build_summary(
    paths: AdoptionLegibilityEvidencePaths,
    inputs: AdoptionLegibilityEvidenceInputs,
    artifact: dict[str, Any],
) -> dict[str, Any]:
    release_blockers = artifact["claim_audit"]["release_blockers"]
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not release_blockers else "FAIL",
        "runner_path": "scripts/build_objc3c_adoption_legibility_evidence.py",
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "artifact_path": rel(paths, paths.artifact_path),
        "publication_path": rel(paths, paths.publication_path),
        "owner_contract_count": len(OWNER_CONTRACTS),
        "blocker_metadata_count": len(BLOCKER_METADATA),
        "step_count": len(inputs.steps),
        "steps": inputs.steps,
        "report_count": len(paths.required_reports()),
        "evaluator_entrypoint_count": len(artifact["evaluator_path"]["entrypoints"]),
        "required_action_count": len(artifact["evaluator_path"]["required_actions"]),
        "adoption_replay_phase_count": len(artifact["adoption_replay"]["phases"]),
        "interop_axis_count": len(artifact["adoption_replay"]["interop_axes"]),
        "comparison_axis_count": len(artifact["comparison_matrix"]["axes"]),
        "onboarding_tutorial_count": len(artifact["onboarding"]["tutorials"]),
        "showcase_workspace_count": len(artifact["onboarding"]["showcase_workspaces"]),
        "support_state": artifact["claim_audit"]["support_state"],
        "failures": release_blockers,
    }


def build_adoption_legibility_model(
    paths: AdoptionLegibilityEvidencePaths,
    inputs: AdoptionLegibilityEvidenceInputs,
) -> AdoptionLegibilityEvidenceModel:
    artifact = build_artifact(paths, inputs)
    publication = build_publication(paths, artifact)
    summary = build_summary(paths, inputs, artifact)
    return AdoptionLegibilityEvidenceModel(artifact=artifact, publication=publication, summary=summary)
