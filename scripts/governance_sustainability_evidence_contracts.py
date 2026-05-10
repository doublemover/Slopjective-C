"""Governance-sustainability evidence payload contracts."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any, Mapping


JsonObject = dict[str, Any]

TEMP_REPORT_ROOT = ("tmp", "reports")
GOVERNANCE_REPORT_FAMILY = (*TEMP_REPORT_ROOT, "governance-sustainability")


def evidence_path(*parts: str) -> str:
    return "/".join(parts)


SELF_GENERATED_REPORTS = {
    evidence_path(*GOVERNANCE_REPORT_FAMILY, "evidence-summary.json"),
    evidence_path(*GOVERNANCE_REPORT_FAMILY, "publication-summary.json"),
    evidence_path(
        *GOVERNANCE_REPORT_FAMILY,
        "closeout-gate",
        "governance_sustainability_closeout_gate.json",
    ),
}

CONTRACT_ID = "objc3c.governance.sustainability.evidence.v1"
SUPPORT_STATE = "stable-governance-process"
PACKAGE_BRIDGE = "objc3c"
PUBLIC_ACTIONS = [
    "validate-governance-sustainability",
    "publish-governance-sustainability",
]
OWNER_SPLIT = {
    "budget_inventory": (
        "tests/tooling/fixtures/governance_sustainability/budget_inventory.json"
    ),
    "budget_policy": (
        "tests/tooling/fixtures/governance_sustainability/"
        "sustainable_progress_policy.json"
    ),
    "waiver_registry": (
        "tests/tooling/fixtures/governance_sustainability/waiver_registry.json"
    ),
    "stewardship": (
        "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json"
    ),
    "extension_review": (
        "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json"
    ),
    "anti_regression": (
        "tests/tooling/fixtures/governance_sustainability/"
        "anti_regression_reporting_contract.json"
    ),
    "metadata_publication": (
        "scripts/publish_objc3c_governance_sustainability_metadata.py"
    ),
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
        "blocker_projection": (
            "claim_audit.blocker_metadata.sustainable_progress_policy"
        ),
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
        "blocked_when": (
            "expired, unowned, duplicate active, or evidence-free waiver is present"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "budget_inventory": {
        "owner": "budget_owner",
        "blocked_when": (
            "public surface growth is not measured or exceeds a release-blocking "
            "budget"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "stewardship": {
        "owner": "stewardship_owner",
        "blocked_when": (
            "review roles, entry surfaces, or required checks lose checked-in "
            "ownership"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "extension_review": {
        "owner": "extension_review_owner",
        "blocked_when": (
            "new work proposal lacks review class, evidence surfaces, owner, "
            "revert path, or publication plan"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "sustainable_progress_policy": {
        "owner": "sustainable_progress_owner",
        "blocked_when": (
            "new work lacks measured budget delta, owner, exception status, or "
            "follow-on ratchet"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "metadata_publication": {
        "owner": "publication_owner",
        "blocked_when": (
            "publication presents governance stability wider than generated evidence"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
}


@dataclass(frozen=True)
class GovernanceSustainabilityEvidenceInputs:
    contract: Mapping[str, Any]
    waiver_registry: Mapping[str, Any]
    generated_reports: list[str]
    report_payloads: Mapping[str, JsonObject]
    artifact_contract_path: str
    evidence_artifact_path: str


@dataclass(frozen=True)
class GovernanceSustainabilityEvidencePayloads:
    evidence: JsonObject
    summary: JsonObject
    failures: list[str]


def _generated_at_utc() -> str:
    return datetime.now(timezone.utc).isoformat()


def _report_payload(
    report_payloads: Mapping[str, JsonObject],
    *path_parts: str,
) -> JsonObject:
    return report_payloads.get(evidence_path(*GOVERNANCE_REPORT_FAMILY, *path_parts), {})


def _release_blockers(
    report_payloads: Mapping[str, JsonObject],
    budget_enforcement: Mapping[str, Any],
    integration: Mapping[str, Any],
) -> list[str]:
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
    return failures


def _claim_audit(
    failures: list[str],
    budget_inventory: Mapping[str, Any],
) -> JsonObject:
    measured = (
        budget_inventory.get("measured", {})
        if isinstance(budget_inventory.get("measured"), dict)
        else {}
    )
    return {
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


def build_governance_sustainability_evidence_payloads(
    inputs: GovernanceSustainabilityEvidenceInputs,
) -> GovernanceSustainabilityEvidencePayloads:
    budget_inventory = _report_payload(
        inputs.report_payloads,
        "budget-inventory",
        "governance_budget_inventory_summary.json",
    )
    policy = _report_payload(
        inputs.report_payloads,
        "sustainable-progress-policy",
        "governance_policy_summary.json",
    )
    maintainer_review = _report_payload(
        inputs.report_payloads,
        "maintainer-review-regression",
        "governance_maintainer_review_summary.json",
    )
    extension_policy = _report_payload(
        inputs.report_payloads,
        "extension-review-policy",
        "governance_extension_review_policy_summary.json",
    )
    extension_workflow = _report_payload(
        inputs.report_payloads,
        "extension-review-workflow",
        "governance_extension_review_workflow_summary.json",
    )
    stewardship = _report_payload(
        inputs.report_payloads,
        "stewardship-semantics",
        "governance_stewardship_semantics_summary.json",
    )
    budget_enforcement = _report_payload(
        inputs.report_payloads,
        "budget-enforcement",
        "governance_budget_enforcement_summary.json",
    )
    anti_regression = _report_payload(
        inputs.report_payloads,
        "anti-regression",
        "governance_anti_regression_summary.json",
    )
    integration = _report_payload(
        inputs.report_payloads,
        "integration",
        "governance_sustainability_integration_summary.json",
    )

    failures = _release_blockers(
        report_payloads=inputs.report_payloads,
        budget_enforcement=budget_enforcement,
        integration=integration,
    )
    claim_audit = _claim_audit(
        failures=failures,
        budget_inventory=budget_inventory,
    )
    status = "PASS" if not failures else "FAIL"

    evidence = {
        "contract_id": CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": _generated_at_utc(),
        "status": status,
        "artifact_contract": inputs.artifact_contract_path,
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "source_contracts": inputs.contract.get("source_contracts", []),
        "generated_reports": inputs.generated_reports,
        "publication_artifacts": inputs.contract.get("publication_artifacts", []),
        "metadata_publication": {
            "status": status,
            "public_actions": PUBLIC_ACTIONS,
            "package_bridge": PACKAGE_BRIDGE,
            "publisher": "scripts/publish_objc3c_governance_sustainability_metadata.py",
            "publication_artifacts": inputs.contract.get("publication_artifacts", []),
        },
        "budget": {
            "inventory_summary": budget_inventory,
            "enforcement_summary": budget_enforcement,
            "anti_regression_summary": anti_regression,
            "waiver_registry": inputs.waiver_registry,
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

    summary = {
        "contract_id": "objc3c.governance.sustainability.evidence.summary.v1",
        "status": status,
        "runner_path": "scripts/build_objc3c_governance_sustainability_evidence.py",
        "evidence_artifact": inputs.evidence_artifact_path,
        "artifact_contract": inputs.artifact_contract_path,
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "owner_contract_count": len(OWNER_CONTRACTS),
        "blocker_metadata_count": len(BLOCKER_METADATA),
        "source_contract_count": len(evidence["source_contracts"]),
        "generated_report_count": len(inputs.generated_reports),
        "publication_artifact_count": len(evidence["publication_artifacts"]),
        "release_blocker_count": len(failures),
        "failures": failures,
    }
    return GovernanceSustainabilityEvidencePayloads(
        evidence=evidence,
        summary=summary,
        failures=failures,
    )


__all__ = [
    "GovernanceSustainabilityEvidenceInputs",
    "GovernanceSustainabilityEvidencePayloads",
    "build_governance_sustainability_evidence_payloads",
]
