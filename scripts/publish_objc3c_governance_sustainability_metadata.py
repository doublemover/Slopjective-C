#!/usr/bin/env python3
"""Publish governance stewardship and extension-review metadata."""

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
EVIDENCE_BUILDER = ROOT / "scripts" / "build_objc3c_governance_sustainability_evidence.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "governance-sustainability" / "governance-sustainability-evidence.json"
STEWARDSHIP_PUBLICATION = ROOT / "tmp" / "artifacts" / "governance-sustainability" / "stewardship-publication.json"
EXTENSION_PUBLICATION = ROOT / "tmp" / "artifacts" / "governance-sustainability" / "extension-review-publication.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "publication-summary.json"

EXPECTED_PUBLIC_ACTIONS = [
    "validate-governance-sustainability",
    "publish-governance-sustainability",
]
PACKAGE_BRIDGE = "objc3c"
STEWARDSHIP_PUBLICATION_ID = "objc3c.governance.sustainability.stewardship_publication.v1"
EXTENSION_PUBLICATION_ID = "objc3c.governance.sustainability.extension_review_publication.v1"




def ensure_evidence() -> None:
    result = subprocess.run(
        python_script_command(EVIDENCE_BUILDER),
        cwd=ROOT,
        text=True,
        capture_output=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        raise RuntimeError("governance sustainability evidence generation failed during publication")


def main() -> int:
    ensure_evidence()
    evidence = load_json(EVIDENCE_ARTIFACT)
    claim_audit = evidence.get("claim_audit", {}) if isinstance(evidence.get("claim_audit"), dict) else {}
    release_blockers = claim_audit.get("release_blockers", [])
    if release_blockers:
        raise RuntimeError(f"cannot publish governance metadata with release blockers: {release_blockers}")

    published_at = datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")
    stewardship = evidence.get("stewardship", {}) if isinstance(evidence.get("stewardship"), dict) else {}
    extension_review = evidence.get("extension_review", {}) if isinstance(evidence.get("extension_review"), dict) else {}
    public_workflow = evidence.get("public_workflow", {}) if isinstance(evidence.get("public_workflow"), dict) else {}
    metadata_publication = evidence.get("metadata_publication", {}) if isinstance(evidence.get("metadata_publication"), dict) else {}
    owner_contracts = evidence.get("owner_contracts", {}) if isinstance(evidence.get("owner_contracts"), dict) else {}

    stewardship_publication = {
        "contract_id": STEWARDSHIP_PUBLICATION_ID,
        "published_at_utc": published_at,
        "source_evidence": repo_rel(EVIDENCE_ARTIFACT),
        "operator_runbook": "docs/runbooks/objc3c_governance_sustainability.md",
        "maintainer_runbook": "docs/runbooks/objc3c_maintainer_workflows.md",
        "contributor_surface": "CONTRIBUTING.md",
        "public_actions": public_workflow.get("public_actions", EXPECTED_PUBLIC_ACTIONS),
        "package_bridge": public_workflow.get("package_bridge", PACKAGE_BRIDGE),
        "owner_contracts": owner_contracts,
        "blocker_metadata": claim_audit.get("blocker_metadata", {}),
        "metadata_publication": metadata_publication,
        "stewardship": stewardship,
        "budget": evidence.get("budget", {}),
        "claim_audit": claim_audit,
    }
    extension_publication = {
        "contract_id": EXTENSION_PUBLICATION_ID,
        "published_at_utc": published_at,
        "source_evidence": repo_rel(EVIDENCE_ARTIFACT),
        "operator_runbook": "docs/runbooks/objc3c_governance_sustainability.md",
        "author_guide": "docs/governance/extension_author_guide_v1.md",
        "proposal_template": "tests/tooling/fixtures/governance_sustainability/new_work_proposal_template.json",
        "public_actions": public_workflow.get("public_actions", EXPECTED_PUBLIC_ACTIONS),
        "package_bridge": public_workflow.get("package_bridge", PACKAGE_BRIDGE),
        "owner_contracts": owner_contracts,
        "blocker_metadata": claim_audit.get("blocker_metadata", {}),
        "metadata_publication": metadata_publication,
        "extension_review": extension_review,
        "claim_audit": claim_audit,
    }

    STEWARDSHIP_PUBLICATION.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(STEWARDSHIP_PUBLICATION, stewardship_publication)
    write_json_file(EXTENSION_PUBLICATION, extension_publication)

    summary = {
        "contract_id": "objc3c.governance.sustainability.publication.summary.v1",
        "status": "PASS",
        "runner_path": "scripts/publish_objc3c_governance_sustainability_metadata.py",
        "evidence_artifact": repo_rel(EVIDENCE_ARTIFACT),
        "stewardship_publication": repo_rel(STEWARDSHIP_PUBLICATION),
        "extension_review_publication": repo_rel(EXTENSION_PUBLICATION),
        "publication_ids": [
            STEWARDSHIP_PUBLICATION_ID,
            EXTENSION_PUBLICATION_ID,
        ],
        "public_actions": EXPECTED_PUBLIC_ACTIONS,
        "package_bridge": PACKAGE_BRIDGE,
        "owner_contract_count": len(owner_contracts),
        "blocker_metadata_count": len(claim_audit.get("blocker_metadata", {})) if isinstance(claim_audit.get("blocker_metadata"), dict) else 0,
        "release_blocker_count": len(release_blockers) if isinstance(release_blockers, list) else 0,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"stewardship_publication: {repo_rel(STEWARDSHIP_PUBLICATION)}")
    print(f"extension_review_publication: {repo_rel(EXTENSION_PUBLICATION)}")
    print("objc3c-governance-sustainability-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
