from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_long_horizon_operations_evidence.evidence_loading import LongHorizonEvidenceInputs
from objc3c_long_horizon_operations_evidence.paths import (
    ARTIFACT_CONTRACT_ID,
    BLOCKER_METADATA,
    SUMMARY_CONTRACT_ID,
    LongHorizonEvidencePaths,
    OWNER_CONTRACTS,
    OWNER_SPLIT,
)


@dataclass(frozen=True)
class LongHorizonEvidenceModel:
    artifact: dict[str, Any]
    summary: dict[str, Any]

    @property
    def passed(self) -> bool:
        return self.summary["status"] == "PASS"


def present_existing_paths(paths: tuple[Path, ...]) -> list[Path]:
    return [path for path in paths if path.is_file()]


def revert_channels(upgrade_support_report: dict[str, Any]) -> list[str]:
    revert_guidance = upgrade_support_report.get("revert_guidance", [])
    return [
        str(entry.get("channel_id"))
        for entry in revert_guidance
        if isinstance(entry, dict)
    ]


def rel(paths: LongHorizonEvidencePaths, path: Path) -> str:
    return repo_rel(path, root=paths.root)


def generated_from_commands(paths: LongHorizonEvidencePaths) -> list[str]:
    return [" ".join(step.command) for step in paths.steps()]


def candidate_target_version(update_manifest: dict[str, Any]) -> str:
    channels = update_manifest.get("channels", [])
    for channel in channels:
        if isinstance(channel, dict) and channel.get("channel_id") == "candidate":
            return str(channel.get("version"))
    return str(update_manifest.get("current_version", ""))


def build_artifact(paths: LongHorizonEvidencePaths, inputs: LongHorizonEvidenceInputs) -> dict[str, Any]:
    reports = inputs.reports
    failures = inputs.failures
    update_manifest = inputs.update_manifest
    upgrade_support_report = inputs.upgrade_support_report
    revert_channel_ids = revert_channels(upgrade_support_report)
    conversion_summary = reports.get("conversion", {})
    aging_summary = reports.get("aging", {})
    soak_reports = paths.soak_report_paths()
    evidence_paths = present_existing_paths(
        paths.package_and_app_evidence_paths() + soak_reports + (paths.upgrade_support_report,)
    )

    return {
        "contract_id": ARTIFACT_CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "generated_from": {
            "owner_split": OWNER_SPLIT,
            "owner_contracts": OWNER_CONTRACTS,
            "contracts": [
                "tests/tooling/fixtures/long_horizon_operations/boundary_inventory.json",
                "tests/tooling/fixtures/long_horizon_operations/deprecation_support_policy.json",
                "tests/tooling/fixtures/long_horizon_operations/conversion_replay_revert_support_window_semantics.json",
                "tests/tooling/fixtures/long_horizon_operations/aging_regression_release_cadence_criteria.json",
                "tests/tooling/fixtures/long_horizon_operations/artifact_contract.json",
            ],
            "commands": generated_from_commands(paths),
        },
        "support_window": {
            "current_version": str(update_manifest.get("current_version", "")),
            "supported_major_line": int(update_manifest.get("supported_major_line", 0)),
            "default_channel": update_manifest.get("default_channel"),
            "supported_platform_ids": update_manifest.get("supported_platform_ids", []),
            "channels": update_manifest.get("channels", []),
        },
        "upgrade_replay": {
            "status": "PASS" if not failures else "FAIL",
            "source_version": str(update_manifest.get("current_version", "")),
            "target_version": candidate_target_version(update_manifest),
            "source_channel": "stable",
            "target_channel": "candidate",
            "requirements": conversion_summary.get("conversion_replay_requirements", []),
            "evidence_paths": [rel(paths, path) for path in evidence_paths],
        },
        "revert_readiness": {
            "status": "PASS" if revert_channel_ids and not failures else "FAIL",
            "channels": revert_channel_ids,
            "evidence_paths": [rel(paths, paths.upgrade_support_report), rel(paths, paths.conversion_summary)],
        },
        "soak": {
            "status": "PASS" if all(path.is_file() for path in soak_reports) and not failures else "FAIL",
            "required_families": [
                "conformance-corpus",
                "stress-integration",
                "external-validation",
                "public-conformance",
            ],
            "evidence_paths": [rel(paths, path) for path in soak_reports if path.is_file()],
        },
        "aging_regression": {
            "status": "PASS" if not failures else "FAIL",
            "cadence_classes": [
                "patch-repair",
                "minor-additive",
                "preview-aging",
            ],
            "freshness_budget": {
                "publication_freshness_metric_count": aging_summary.get("publication_freshness_metric_count"),
                "performance_governance_integration": rel(paths, paths.performance_governance_integration),
            },
        },
        "claim_audit": {
            "support_state": "supported-for-same-major-maintenance-with-generated-conversion-revert-soak-and-aging-evidence",
            "earned_claims": [
                "same-major support windows are generated from release operations metadata",
                "conversion replay evidence composes release, package, and canonical application reports",
                "revert guidance is read from the generated upgrade support report",
                "soak and aging evidence consume conformance, stress, external-validation, public-conformance, and performance governance reports",
            ],
            "demoted_or_out_of_scope_claims": [
                "cross-major support without generated conversion replay",
                "hosted registry availability",
                "background auto-update behavior",
                "manual support waivers without generated evidence",
            ],
            "blocker_metadata": BLOCKER_METADATA,
            "release_blockers": failures,
        },
    }


def build_summary(
    paths: LongHorizonEvidencePaths,
    inputs: LongHorizonEvidenceInputs,
    artifact: dict[str, Any],
) -> dict[str, Any]:
    failures = inputs.failures
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/build_objc3c_long_horizon_operations_evidence.py",
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "artifact_path": rel(paths, paths.artifact_path),
        "owner_contract_count": len(OWNER_CONTRACTS),
        "blocker_metadata_count": len(BLOCKER_METADATA),
        "step_count": len(inputs.steps),
        "steps": inputs.steps,
        "report_count": len(paths.required_reports()),
        "conversion_evidence_path_count": len(artifact["upgrade_replay"]["evidence_paths"]),
        "soak_evidence_path_count": len(artifact["soak"]["evidence_paths"]),
        "revert_channel_count": len(artifact["revert_readiness"]["channels"]),
        "support_state": artifact["claim_audit"]["support_state"],
        "failures": failures,
    }


def build_long_horizon_model(
    paths: LongHorizonEvidencePaths,
    inputs: LongHorizonEvidenceInputs,
) -> LongHorizonEvidenceModel:
    artifact = build_artifact(paths, inputs)
    summary = build_summary(paths, inputs, artifact)
    return LongHorizonEvidenceModel(artifact=artifact, summary=summary)
