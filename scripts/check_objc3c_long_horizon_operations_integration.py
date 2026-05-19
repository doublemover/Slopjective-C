#!/usr/bin/env python3
"""Validate the integrated long-horizon operations workflow."""

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
EVIDENCE_BUILDER = ROOT / "scripts" / "build_objc3c_long_horizon_operations_evidence.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "long-horizon-operations" / "long-horizon-operations-evidence.json"
EVIDENCE_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "evidence-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "integration-summary.json"




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
    expect(result.returncode == 0, "long-horizon evidence builder failed", failures)
    expect(EVIDENCE_ARTIFACT.is_file(), f"missing evidence artifact {repo_rel(EVIDENCE_ARTIFACT)}", failures)
    expect(EVIDENCE_SUMMARY.is_file(), f"missing evidence summary {repo_rel(EVIDENCE_SUMMARY)}", failures)

    artifact = load_json(EVIDENCE_ARTIFACT) if EVIDENCE_ARTIFACT.is_file() else {}
    evidence_summary = load_json(EVIDENCE_SUMMARY) if EVIDENCE_SUMMARY.is_file() else {}
    expect(artifact.get("contract_id") == "objc3c.long_horizon_operations.evidence.v1", "evidence artifact contract_id drifted", failures)
    expect(artifact.get("schema_version") == 1, "evidence artifact schema_version drifted", failures)
    expect(evidence_summary.get("status") == "PASS", "evidence summary did not report PASS", failures)

    for section in ("owner_contracts", "support_window", "upgrade_replay", "revert_readiness", "soak", "aging_regression", "claim_audit"):
        expect(isinstance(artifact.get(section), dict), f"evidence artifact missing section {section}", failures)

    claim_audit = artifact.get("claim_audit", {}) if isinstance(artifact.get("claim_audit"), dict) else {}
    owner_contracts = artifact.get("owner_contracts", {}) if isinstance(artifact.get("owner_contracts"), dict) else {}
    blocker_metadata = claim_audit.get("blocker_metadata", {}) if isinstance(claim_audit.get("blocker_metadata"), dict) else {}
    expect(
        {"deprecation_owner", "revert_owner", "cadence_owner", "publication_owner"}.issubset(owner_contracts),
        "long-horizon owner contracts are incomplete",
        failures,
    )
    expect(
        {"deprecation_support_policy", "revert_readiness", "aging_release_cadence", "metadata_publication"}.issubset(blocker_metadata),
        "long-horizon blocker metadata is incomplete",
        failures,
    )
    expect("same-major" in str(claim_audit.get("support_state", "")), "support_state is not same-major scoped", failures)
    expect(claim_audit.get("release_blockers") == [], "claim audit reported release blockers", failures)
    expect(len(artifact.get("upgrade_replay", {}).get("evidence_paths", [])) >= 4, "upgrade replay evidence is too narrow", failures)
    expect(len(artifact.get("soak", {}).get("evidence_paths", [])) >= 4, "soak evidence is too narrow", failures)
    expect(len(artifact.get("revert_readiness", {}).get("channels", [])) >= 3, "revert channels are too narrow", failures)

    payload = {
        "contract_id": "objc3c.long_horizon_operations.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_long_horizon_operations_integration.py",
        "evidence_builder": repo_rel(EVIDENCE_BUILDER),
        "evidence_artifact": repo_rel(EVIDENCE_ARTIFACT),
        "evidence_summary": repo_rel(EVIDENCE_SUMMARY),
        "support_state": claim_audit.get("support_state"),
        "owner_contract_count": len(owner_contracts),
        "blocker_metadata_count": len(blocker_metadata),
        "upgrade_evidence_path_count": len(artifact.get("upgrade_replay", {}).get("evidence_paths", [])) if isinstance(artifact.get("upgrade_replay"), dict) else 0,
        "soak_evidence_path_count": len(artifact.get("soak", {}).get("evidence_paths", [])) if isinstance(artifact.get("soak"), dict) else 0,
        "revert_channel_count": len(artifact.get("revert_readiness", {}).get("channels", [])) if isinstance(artifact.get("revert_readiness"), dict) else 0,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-long-horizon-operations-integration: PASS" if not failures else "objc3c-long-horizon-operations-integration: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
