#!/usr/bin/env python3
"""Generate long-horizon migration, rollback, soak, and aging evidence."""

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
ARTIFACT_PATH = ROOT / "tmp" / "artifacts" / "long-horizon-operations" / "long-horizon-operations-evidence.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "evidence-summary.json"

BOUNDARY_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "boundary-inventory-summary.json"
DEPRECATION_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "deprecation-compatibility-policy-summary.json"
MIGRATION_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "migration-rollback-support-window-summary.json"
AGING_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "aging-regression-release-cadence-summary.json"
ARTIFACT_CONTRACT_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "artifact-contract-summary.json"

UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
COMPATIBILITY_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
PACKAGE_INTEGRATION = ROOT / "tmp" / "reports" / "package-ecosystem" / "integration-summary.json"
PACKAGE_LOCK_SUMMARY = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-lock-summary.json"
APPLICATION_ARCHITECTURE_INTEGRATION = ROOT / "tmp" / "reports" / "application-architecture-testing" / "runnable-template-canonical-app-summary.json"
PERFORMANCE_GOVERNANCE_INTEGRATION = ROOT / "tmp" / "reports" / "performance-governance" / "integration-summary.json"
CONFORMANCE_CORPUS_INTEGRATION = ROOT / "tmp" / "reports" / "conformance" / "corpus-integration-summary.json"
STRESS_INTEGRATION = ROOT / "tmp" / "reports" / "stress" / "integration-summary.json"
EXTERNAL_VALIDATION_INTEGRATION = ROOT / "tmp" / "reports" / "external-validation" / "integration-summary.json"
PUBLIC_CONFORMANCE_INTEGRATION = ROOT / "tmp" / "reports" / "public-conformance" / "integration-summary.json"

STEPS = [
    ("boundary-inventory", python_script_command("scripts/build_long_horizon_operations_boundary_inventory_summary.py")),
    ("deprecation-policy", python_script_command("scripts/build_long_horizon_operations_deprecation_policy_summary.py")),
    ("migration-rollback-support-window", python_script_command("scripts/build_long_horizon_operations_migration_rollback_summary.py")),
    ("aging-cadence", python_script_command("scripts/build_long_horizon_operations_aging_cadence_summary.py")),
    ("artifact-contract", python_script_command("scripts/build_long_horizon_operations_artifact_contract_summary.py")),
    ("package-ecosystem-integration", python_script_command("scripts/check_objc3c_package_ecosystem_integration.py")),
    ("application-architecture-integration", python_script_command("scripts/check_objc3c_application_architecture_integration.py")),
    ("performance-governance-integration", python_script_command("scripts/check_objc3c_performance_governance_integration.py")),
    ("conformance-corpus-integration", python_script_command("scripts/check_objc3c_conformance_corpus_integration.py")),
    ("stress-integration", python_script_command("scripts/check_objc3c_stress_integration.py")),
    ("external-validation-integration", python_script_command("scripts/check_objc3c_external_validation_integration.py")),
    ("public-conformance-integration", python_script_command("scripts/check_objc3c_public_conformance_reporting_integration.py")),
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
        "deprecation": DEPRECATION_SUMMARY,
        "migration": MIGRATION_SUMMARY,
        "aging": AGING_SUMMARY,
        "artifact_contract": ARTIFACT_CONTRACT_SUMMARY,
        "package": PACKAGE_INTEGRATION,
        "application_architecture": APPLICATION_ARCHITECTURE_INTEGRATION,
        "performance_governance": PERFORMANCE_GOVERNANCE_INTEGRATION,
        "conformance_corpus": CONFORMANCE_CORPUS_INTEGRATION,
        "stress": STRESS_INTEGRATION,
        "external_validation": EXTERNAL_VALIDATION_INTEGRATION,
        "public_conformance": PUBLIC_CONFORMANCE_INTEGRATION,
    }
    for name, path in required_reports.items():
        expect(path.is_file(), f"missing {name} report {repo_rel(path)}", failures)

    reports = {name: load_json(path) for name, path in required_reports.items() if path.is_file()}
    for name, payload in reports.items():
        expect(status_passes(payload), f"{name} report did not pass", failures)

    update_manifest = load_json(UPDATE_MANIFEST) if UPDATE_MANIFEST.is_file() else {}
    compatibility_report = load_json(COMPATIBILITY_REPORT) if COMPATIBILITY_REPORT.is_file() else {}
    expect(bool(update_manifest), f"missing update manifest {repo_rel(UPDATE_MANIFEST)}", failures)
    expect(bool(compatibility_report), f"missing compatibility report {repo_rel(COMPATIBILITY_REPORT)}", failures)

    rollback_guidance = compatibility_report.get("rollback_guidance", [])
    rollback_channels = [str(entry.get("channel_id")) for entry in rollback_guidance if isinstance(entry, dict)]
    migration_summary = reports.get("migration", {})
    aging_summary = reports.get("aging", {})

    soak_reports = [
        CONFORMANCE_CORPUS_INTEGRATION,
        STRESS_INTEGRATION,
        EXTERNAL_VALIDATION_INTEGRATION,
        PUBLIC_CONFORMANCE_INTEGRATION,
    ]
    package_and_app_evidence = [PACKAGE_INTEGRATION, PACKAGE_LOCK_SUMMARY, APPLICATION_ARCHITECTURE_INTEGRATION]
    evidence_paths = [path for path in package_and_app_evidence + soak_reports + [COMPATIBILITY_REPORT] if path.is_file()]

    artifact = {
        "contract_id": "objc3c.long_horizon_operations.evidence.v1",
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "generated_from": {
            "contracts": [
                "tests/tooling/fixtures/long_horizon_operations/boundary_inventory.json",
                "tests/tooling/fixtures/long_horizon_operations/deprecation_compatibility_policy.json",
                "tests/tooling/fixtures/long_horizon_operations/migration_rollback_support_window_semantics.json",
                "tests/tooling/fixtures/long_horizon_operations/aging_regression_release_cadence_criteria.json",
                "tests/tooling/fixtures/long_horizon_operations/artifact_contract.json",
            ],
            "commands": [" ".join(command) for _, command in STEPS],
        },
        "support_window": {
            "current_version": str(update_manifest.get("current_version", "")),
            "supported_major_line": int(update_manifest.get("supported_major_line", 0)),
            "default_channel": update_manifest.get("default_channel"),
            "supported_platform_ids": update_manifest.get("supported_platform_ids", []),
            "channels": update_manifest.get("channels", []),
        },
        "migration_replay": {
            "status": "PASS" if not failures else "FAIL",
            "source_version": str(update_manifest.get("current_version", "")),
            "target_version": next((str(channel.get("version")) for channel in update_manifest.get("channels", []) if isinstance(channel, dict) and channel.get("channel_id") == "candidate"), str(update_manifest.get("current_version", ""))),
            "source_channel": "stable",
            "target_channel": "candidate",
            "requirements": migration_summary.get("migration_replay_requirements", []),
            "evidence_paths": [repo_rel(path) for path in evidence_paths],
        },
        "rollback": {
            "status": "PASS" if rollback_channels and not failures else "FAIL",
            "channels": rollback_channels,
            "evidence_paths": [repo_rel(COMPATIBILITY_REPORT), repo_rel(MIGRATION_SUMMARY)],
        },
        "soak": {
            "status": "PASS" if all(path.is_file() for path in soak_reports) and not failures else "FAIL",
            "required_families": ["conformance-corpus", "stress-integration", "external-validation", "public-conformance"],
            "evidence_paths": [repo_rel(path) for path in soak_reports if path.is_file()],
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
                "performance_governance_integration": repo_rel(PERFORMANCE_GOVERNANCE_INTEGRATION),
            },
        },
        "claim_audit": {
            "support_state": "supported-for-same-major-maintenance-with-generated-migration-rollback-soak-and-aging-evidence",
            "earned_claims": [
                "same-major support windows are generated from release operations metadata",
                "migration replay evidence composes release, package, and canonical application reports",
                "rollback guidance is read from the generated compatibility report",
                "soak and aging evidence consume conformance, stress, external-validation, public-conformance, and performance governance reports",
            ],
            "demoted_or_out_of_scope_claims": [
                "cross-major compatibility without generated migration replay",
                "hosted registry availability",
                "background auto-update behavior",
                "manual compatibility waivers without generated evidence",
            ],
            "release_blockers": failures,
        },
    }

    ARTIFACT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(ARTIFACT_PATH, artifact)

    summary = {
        "contract_id": "objc3c.long_horizon_operations.evidence.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/build_objc3c_long_horizon_operations_evidence.py",
        "artifact_path": repo_rel(ARTIFACT_PATH),
        "step_count": len(steps),
        "steps": steps,
        "report_count": len(required_reports),
        "migration_evidence_path_count": len(artifact["migration_replay"]["evidence_paths"]),
        "soak_evidence_path_count": len(artifact["soak"]["evidence_paths"]),
        "rollback_channel_count": len(rollback_channels),
        "support_state": artifact["claim_audit"]["support_state"],
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"artifact_path: {repo_rel(ARTIFACT_PATH)}")
    print("objc3c-long-horizon-evidence: PASS" if not failures else "objc3c-long-horizon-evidence: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
