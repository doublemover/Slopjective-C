#!/usr/bin/env python3
"""Publish operator-facing long-horizon support-window metadata."""

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
INTEGRATION_CHECK = ROOT / "scripts" / "check_objc3c_long_horizon_operations_integration.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "long-horizon-operations" / "long-horizon-operations-evidence.json"
PUBLICATION_ARTIFACT = ROOT / "tmp" / "artifacts" / "long-horizon-operations" / "support-window-publication.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "publication-summary.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "long-horizon-operations" / "integration-summary.json"

EXPECTED_PUBLIC_ACTIONS = [
    "validate-long-horizon-operations",
    "publish-long-horizon-operations",
]
PACKAGE_BRIDGE = "objc3c"




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
        raise RuntimeError("long-horizon integration failed during publication")


def main() -> int:
    ensure_integration()
    evidence = load_json(EVIDENCE_ARTIFACT)
    claim_audit = evidence.get("claim_audit", {})
    if not isinstance(claim_audit, dict):
        raise RuntimeError("evidence claim_audit must be an object")
    release_blockers = claim_audit.get("release_blockers", [])
    if release_blockers:
        raise RuntimeError(f"cannot publish long-horizon metadata with release blockers: {release_blockers}")

    owner_contracts = evidence.get("owner_contracts", {}) if isinstance(evidence.get("owner_contracts"), dict) else {}
    publication = dict(evidence)
    publication["published_at_utc"] = datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")
    publication["operator_publication"] = {
        "publication_id": "objc3c.long_horizon_operations.support_window_publication.v1",
        "support_state": claim_audit.get("support_state"),
        "public_actions": EXPECTED_PUBLIC_ACTIONS,
        "package_bridge": PACKAGE_BRIDGE,
        "operator_runbook": "docs/runbooks/objc3c_long_horizon_operations.md",
        "owner_contracts": owner_contracts,
        "blocker_metadata": claim_audit.get("blocker_metadata", {}),
        "support_window_summary": evidence.get("support_window", {}),
        "revert_channels": evidence.get("revert_readiness", {}).get("channels", []) if isinstance(evidence.get("revert_readiness"), dict) else [],
        "demoted_or_out_of_scope_claims": claim_audit.get("demoted_or_out_of_scope_claims", []),
    }

    PUBLICATION_ARTIFACT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PUBLICATION_ARTIFACT, publication)

    summary = {
        "contract_id": "objc3c.long_horizon_operations.publication.summary.v1",
        "status": "PASS",
        "runner_path": "scripts/publish_objc3c_long_horizon_operations_metadata.py",
        "evidence_artifact": repo_rel(EVIDENCE_ARTIFACT),
        "publication_artifact": repo_rel(PUBLICATION_ARTIFACT),
        "integration_summary": repo_rel(INTEGRATION_SUMMARY),
        "support_state": claim_audit.get("support_state"),
        "public_actions": EXPECTED_PUBLIC_ACTIONS,
        "package_bridge": PACKAGE_BRIDGE,
        "revert_channel_count": len(publication["operator_publication"]["revert_channels"]),
        "owner_contract_count": len(owner_contracts),
        "blocker_metadata_count": len(claim_audit.get("blocker_metadata", {})) if isinstance(claim_audit.get("blocker_metadata"), dict) else 0,
        "release_blocker_count": len(release_blockers) if isinstance(release_blockers, list) else 0,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"publication_artifact: {repo_rel(PUBLICATION_ARTIFACT)}")
    print("objc3c-long-horizon-operations-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
