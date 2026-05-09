#!/usr/bin/env python3
"""Build the machine-owned governance sustainability evidence artifact."""

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
ARTIFACT_CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "artifact_contract.json"
WAIVER_REGISTRY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "waiver_registry.json"
INTEGRATION_CHECK = ROOT / "scripts" / "check_objc3c_governance_sustainability_integration.py"
EVIDENCE_ARTIFACT = ROOT / "tmp" / "artifacts" / "governance-sustainability" / "governance-sustainability-evidence.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "evidence-summary.json"
SELF_GENERATED_REPORTS = {
    "tmp/reports/governance-sustainability/evidence-summary.json",
    "tmp/reports/governance-sustainability/publication-summary.json",
    "tmp/reports/governance-sustainability/closeout-gate/governance_sustainability_closeout_gate.json",
}

CONTRACT_ID = "objc3c.governance.sustainability.evidence.v1"
SUPPORT_STATE = "stable-governance-process"
PACKAGE_BRIDGE = "objc3c"
PUBLIC_ACTIONS = [
    "validate-governance-sustainability",
    "publish-governance-sustainability",
]
OWNER_SPLIT = {
    "budget_inventory": "tests/tooling/fixtures/governance_sustainability/budget_inventory.json",
    "budget_policy": "tests/tooling/fixtures/governance_sustainability/sustainable_progress_policy.json",
    "waiver_registry": "tests/tooling/fixtures/governance_sustainability/waiver_registry.json",
    "stewardship": "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json",
    "extension_review": "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json",
    "anti_regression": "tests/tooling/fixtures/governance_sustainability/anti_regression_reporting_contract.json",
    "metadata_publication": "scripts/publish_objc3c_governance_sustainability_metadata.py",
}
OWNER_CONTRACTS = {
    "waiver_owner": {
        "source_contract": OWNER_SPLIT["waiver_registry"],
        "artifact_section": "budget.waiver_registry",
        "publication_projection": "stewardship_publication.budget.waiver_registry",
        "blocker_projection": "claim_audit.blocker_metadata.waiver_registry",
    },
    "budget_owner": {
        "source_contract": OWNER_SPLIT["budget_inventory"],
        "artifact_section": "budget.inventory_summary",
        "publication_projection": "stewardship_publication.budget.inventory_summary",
        "blocker_projection": "claim_audit.blocker_metadata.budget_inventory",
    },
    "stewardship_owner": {
        "source_contract": OWNER_SPLIT["stewardship"],
        "artifact_section": "stewardship",
        "publication_projection": "stewardship_publication.stewardship",
        "blocker_projection": "claim_audit.blocker_metadata.stewardship",
    },
    "extension_review_owner": {
        "source_contract": OWNER_SPLIT["extension_review"],
        "artifact_section": "extension_review",
        "publication_projection": "extension_review_publication.extension_review",
        "blocker_projection": "claim_audit.blocker_metadata.extension_review",
    },
    "sustainable_progress_owner": {
        "source_contract": OWNER_SPLIT["budget_policy"],
        "artifact_section": "stewardship.policy_summary",
        "publication_projection": "stewardship_publication.stewardship.policy_summary",
        "blocker_projection": "claim_audit.blocker_metadata.sustainable_progress_policy",
    },
    "publication_owner": {
        "source_contract": OWNER_SPLIT["metadata_publication"],
        "artifact_section": "metadata_publication",
        "publication_projection": "stewardship_publication.metadata_publication",
        "blocker_projection": "claim_audit.blocker_metadata.metadata_publication",
    },
}
BLOCKER_METADATA = {
    "waiver_registry": {
        "owner": "waiver_owner",
        "blocked_when": "expired, unowned, duplicate active, or evidence-free waiver is present",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "budget_inventory": {
        "owner": "budget_owner",
        "blocked_when": "public surface growth is not measured or exceeds a release-blocking budget",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "stewardship": {
        "owner": "stewardship_owner",
        "blocked_when": "review roles, entry surfaces, or required checks lose checked-in ownership",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "extension_review": {
        "owner": "extension_review_owner",
        "blocked_when": "new work proposal lacks review class, evidence surfaces, owner, revert path, or publication plan",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "sustainable_progress_policy": {
        "owner": "sustainable_progress_owner",
        "blocked_when": "new work lacks measured budget delta, owner, exception status, or follow-on ratchet",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "metadata_publication": {
        "owner": "publication_owner",
        "blocked_when": "publication presents governance stability wider than generated evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
}



def ensure_integration() -> None:
    result = subprocess.run(
        python_script_command(INTEGRATION_CHECK),
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
        raise RuntimeError("governance sustainability integration failed during evidence generation")


def load_optional_summary(relative_path: str) -> dict[str, Any]:
    path = ROOT / relative_path
    return load_json(path) if path.is_file() else {}


def main() -> int:
    ensure_integration()
    contract = load_json(ARTIFACT_CONTRACT_PATH)
    waiver_registry = load_json(WAIVER_REGISTRY_PATH)
    generated_reports = [str(path) for path in contract.get("generated_reports", [])]
    report_payloads = {path: load_optional_summary(path) for path in generated_reports}

    budget_inventory = report_payloads.get("tmp/reports/governance-sustainability/budget-inventory/governance_budget_inventory_summary.json", {})
    policy = report_payloads.get("tmp/reports/governance-sustainability/sustainable-progress-policy/governance_policy_summary.json", {})
    maintainer_review = report_payloads.get("tmp/reports/governance-sustainability/maintainer-review-regression/governance_maintainer_review_summary.json", {})
    extension_policy = report_payloads.get("tmp/reports/governance-sustainability/extension-review-policy/governance_extension_review_policy_summary.json", {})
    extension_workflow = report_payloads.get("tmp/reports/governance-sustainability/extension-review-workflow/governance_extension_review_workflow_summary.json", {})
    stewardship = report_payloads.get("tmp/reports/governance-sustainability/stewardship-semantics/governance_stewardship_semantics_summary.json", {})
    budget_enforcement = report_payloads.get("tmp/reports/governance-sustainability/budget-enforcement/governance_budget_enforcement_summary.json", {})
    anti_regression = report_payloads.get("tmp/reports/governance-sustainability/anti-regression/governance_anti_regression_summary.json", {})
    integration = report_payloads.get("tmp/reports/governance-sustainability/integration/governance_sustainability_integration_summary.json", {})

    failures: list[str] = []
    for path, payload in report_payloads.items():
        if path in SELF_GENERATED_REPORTS:
            continue
        if not payload:
            failures.append(f"missing generated report {path}")
        elif payload.get("status") not in (None, "PASS") or payload.get("ok") is False:
            failures.append(f"generated report not passing: {path}")
    if budget_enforcement.get("status") != "PASS":
        failures.append("budget enforcement is not PASS")
    if integration.get("status") != "PASS":
        failures.append("integration summary is not PASS")

    measured = budget_inventory.get("measured", {}) if isinstance(budget_inventory.get("measured"), dict) else {}
    claim_audit = {
        "support_state": SUPPORT_STATE,
        "release_blockers": failures,
        "demoted_or_out_of_scope_claims": [
            "hosted community infrastructure",
            "hosted registry moderation",
            "prose-only governance approval",
        ],
        "blocker_metadata": BLOCKER_METADATA,
        "measured_budget_state": {
            "package_bridge_count": measured.get("package_bridge_count"),
            "package_bridge_budget": measured.get("package_bridge_budget"),
            "public_workflow_action_count": measured.get("public_workflow_action_count"),
            "live_check_script_count": measured.get("live_check_script_count"),
        },
    }

    evidence = {
        "contract_id": CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "artifact_contract": repo_rel(ARTIFACT_CONTRACT_PATH),
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "source_contracts": contract.get("source_contracts", []),
        "generated_reports": generated_reports,
        "publication_artifacts": contract.get("publication_artifacts", []),
        "metadata_publication": {
            "status": "PASS" if not failures else "FAIL",
            "public_actions": PUBLIC_ACTIONS,
            "package_bridge": PACKAGE_BRIDGE,
            "publisher": "scripts/publish_objc3c_governance_sustainability_metadata.py",
            "publication_artifacts": contract.get("publication_artifacts", []),
        },
        "budget": {
            "inventory_summary": budget_inventory,
            "enforcement_summary": budget_enforcement,
            "anti_regression_summary": anti_regression,
            "waiver_registry": waiver_registry,
        },
        "extension_review": {
            "policy_summary": extension_policy,
            "workflow_summary": extension_workflow,
        },
        "stewardship": {
            "semantics_summary": stewardship,
            "maintainer_review_summary": maintainer_review,
            "policy_summary": policy,
        },
        "public_workflow": {
            "integration_summary": integration,
            "public_actions": PUBLIC_ACTIONS,
            "package_bridge": PACKAGE_BRIDGE,
        },
        "claim_audit": claim_audit,
        "failures": failures,
    }

    EVIDENCE_ARTIFACT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(EVIDENCE_ARTIFACT, evidence)

    summary = {
        "contract_id": "objc3c.governance.sustainability.evidence.summary.v1",
        "status": evidence["status"],
        "runner_path": "scripts/build_objc3c_governance_sustainability_evidence.py",
        "evidence_artifact": repo_rel(EVIDENCE_ARTIFACT),
        "artifact_contract": repo_rel(ARTIFACT_CONTRACT_PATH),
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "owner_contract_count": len(OWNER_CONTRACTS),
        "blocker_metadata_count": len(BLOCKER_METADATA),
        "source_contract_count": len(evidence["source_contracts"]),
        "generated_report_count": len(generated_reports),
        "publication_artifact_count": len(evidence["publication_artifacts"]),
        "release_blocker_count": len(failures),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"evidence_artifact: {repo_rel(EVIDENCE_ARTIFACT)}")
    print("objc3c-governance-sustainability-evidence: PASS" if not failures else "objc3c-governance-sustainability-evidence: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
