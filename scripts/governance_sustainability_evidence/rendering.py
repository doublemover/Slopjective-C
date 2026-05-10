"""Payload rendering for governance sustainability evidence reports."""

from __future__ import annotations

from datetime import datetime, timezone
from typing import Any, Mapping

from .contracts import (
    BLOCKER_METADATA,
    CONTRACT_ID,
    OWNER_CONTRACTS,
    OWNER_SPLIT,
    PACKAGE_BRIDGE,
    PUBLIC_ACTIONS,
)
from .model import JsonObject


def generated_at_utc() -> str:
    return datetime.now(timezone.utc).isoformat()


def render_evidence_payload(
    *,
    contract: Mapping[str, Any],
    waiver_registry: Mapping[str, Any],
    generated_reports: list[str],
    artifact_contract_path: str,
    budget_inventory: Mapping[str, Any],
    budget_enforcement: Mapping[str, Any],
    anti_regression: Mapping[str, Any],
    policy: Mapping[str, Any],
    maintainer_review: Mapping[str, Any],
    extension_policy: Mapping[str, Any],
    extension_workflow: Mapping[str, Any],
    stewardship: Mapping[str, Any],
    integration: Mapping[str, Any],
    claim_audit: JsonObject,
    failures: list[str],
) -> JsonObject:
    status = "PASS" if not failures else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": generated_at_utc(),
        "status": status,
        "artifact_contract": artifact_contract_path,
        "owner_split": OWNER_SPLIT,
        "owner_contracts": OWNER_CONTRACTS,
        "source_contracts": contract.get("source_contracts", []),
        "generated_reports": generated_reports,
        "publication_artifacts": contract.get("publication_artifacts", []),
        "metadata_publication": {
            "status": status,
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


def render_summary_payload(
    *,
    evidence: Mapping[str, Any],
    generated_reports: list[str],
    artifact_contract_path: str,
    evidence_artifact_path: str,
    failures: list[str],
) -> JsonObject:
    status = "PASS" if not failures else "FAIL"
    return {
        "contract_id": "objc3c.governance.sustainability.evidence.summary.v1",
        "status": status,
        "runner_path": "scripts/build_objc3c_governance_sustainability_evidence.py",
        "evidence_artifact": evidence_artifact_path,
        "artifact_contract": artifact_contract_path,
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


def render_console_lines(
    *,
    summary_path: str,
    evidence_artifact: str,
    failures: list[str],
) -> list[str]:
    return [
        f"summary_path: {summary_path}",
        f"evidence_artifact: {evidence_artifact}",
        (
            "objc3c-governance-sustainability-evidence: PASS"
            if not failures
            else "objc3c-governance-sustainability-evidence: FAIL"
        ),
    ]


__all__ = [
    "generated_at_utc",
    "render_console_lines",
    "render_evidence_payload",
    "render_summary_payload",
]
