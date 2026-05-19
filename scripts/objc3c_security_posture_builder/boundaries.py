"""Trust-boundary assembly for the security posture builder."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import repo_rel

from .constants import (
    ARTIFACT_CONTRACT_SUMMARY,
    DISTRIBUTION_TRUST_REPORT,
    MACRO_SUMMARY,
    RELEASE_KEY_SUMMARY,
    RESPONSE_SUMMARY,
    RUNTIME_HARDENING_SUMMARY,
    SCHEMA_SUMMARY,
    SOURCE_SUMMARY,
    SUPPLY_CHAIN_SUMMARY,
)
from .state import boundary_status_from_summary


def build_trust_boundaries(reports: dict[str, dict[str, Any]], trust_report: dict[str, Any], trust_state: str) -> list[dict[str, Any]]:
    return [
        {
            "boundary_id": "checked-in-security-source-surface",
            "status": boundary_status_from_summary(reports["source"]),
            "source_path": repo_rel(SOURCE_SUMMARY),
            "detail": "checked-in runbook, policy, and workflow surfaces remain complete",
        },
        {
            "boundary_id": "checked-in-security-schema-surface",
            "status": boundary_status_from_summary(reports["schema"]),
            "source_path": repo_rel(SCHEMA_SUMMARY),
            "detail": "security posture and advisory outputs stay on checked-in schema contracts",
        },
        {
            "boundary_id": "response-disclosure-policy",
            "status": boundary_status_from_summary(reports["response"], warn_on_degraded=True, trust_state=trust_state),
            "source_path": repo_rel(RESPONSE_SUMMARY),
            "detail": "response classes and disclosure narrowing rules remain replayable",
        },
        {
            "boundary_id": "macro-package-provenance-trust",
            "status": boundary_status_from_summary(reports["macro"]),
            "source_path": repo_rel(MACRO_SUMMARY),
            "detail": "macro package and provenance enforcement stays fail-closed on checked-in surfaces",
        },
        {
            "boundary_id": "release-update-lineage-hardening",
            "status": boundary_status_from_summary(reports["release_key"], warn_on_degraded=True, trust_state=trust_state),
            "source_path": repo_rel(RELEASE_KEY_SUMMARY),
            "detail": "release manifest, SBOM, attestation, update, and channel lineage remain coherent",
        },
        {
            "boundary_id": "security-artifact-publication-contract",
            "status": boundary_status_from_summary(reports["artifact_contract"]),
            "source_path": repo_rel(ARTIFACT_CONTRACT_SUMMARY),
            "detail": "security posture and advisory publication artifacts stay canonical and replayable",
        },
        {
            "boundary_id": "supply-chain-audit",
            "status": boundary_status_from_summary(reports["supply_chain"], warn_on_degraded=True, trust_state=trust_state),
            "source_path": repo_rel(SUPPLY_CHAIN_SUMMARY),
            "detail": "release evidence, update metadata, and trust-report lineage remain coherent",
        },
        {
            "boundary_id": "runtime-hardening-memory-safety-regression",
            "status": boundary_status_from_summary(reports["runtime_hardening"]),
            "source_path": repo_rel(RUNTIME_HARDENING_SUMMARY),
            "detail": "runtime hardening claims stay bounded to passing runtime acceptance and packaged runnable release-candidate evidence",
        },
        {
            "boundary_id": "distribution-trust-signal",
            "status": "PASS" if trust_state == "ready" else "WARN" if trust_state == "degraded" else "FAIL",
            "source_path": repo_rel(DISTRIBUTION_TRUST_REPORT),
            "detail": str(trust_report.get("headline", "")),
        },
    ]
