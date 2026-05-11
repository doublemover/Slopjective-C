"""Payload construction and writing for the security posture builder."""

from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .constants import (
    ARTIFACT_CONTRACT_SUMMARY,
    DISTRIBUTION_TRUST_REPORT,
    MACRO_SUMMARY,
    POSTURE_PATH,
    RELEASE_KEY_SUMMARY,
    RESPONSE_SUMMARY,
    RUNTIME_HARDENING_SUMMARY,
    SCHEMA_SUMMARY,
    SOURCE_SUMMARY,
    SUMMARY_PATH,
    SUPPLY_CHAIN_SUMMARY,
)


def build_evidence_paths(trust_report: dict[str, Any]) -> list[str]:
    return [
        repo_rel(SOURCE_SUMMARY),
        repo_rel(SCHEMA_SUMMARY),
        repo_rel(RESPONSE_SUMMARY),
        repo_rel(MACRO_SUMMARY),
        repo_rel(RELEASE_KEY_SUMMARY),
        repo_rel(ARTIFACT_CONTRACT_SUMMARY),
        repo_rel(SUPPLY_CHAIN_SUMMARY),
        repo_rel(RUNTIME_HARDENING_SUMMARY),
        repo_rel(DISTRIBUTION_TRUST_REPORT),
        *[str(path) for path in trust_report.get("evidence_paths", []) if isinstance(path, str)],
    ]


def build_posture_payload(security_state: str, headline: str, trust_boundaries: list[dict[str, Any]], evidence_paths: list[str]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.security.hardening.posture.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "security_state": security_state,
        "headline": headline,
        "trust_boundaries": trust_boundaries,
        "evidence_paths": evidence_paths,
        "publication_surface": {
            "package_bridge": "objc3c",
            "inspect_security_posture_command": "build-security-posture",
            "publish_security_advisories_command": "publish-security-advisories",
            "validate_security_hardening_command": "validate-security-hardening",
            "validate_security_hardening_end_to_end_command": "validate-security-hardening-end-to-end",
        },
    }


def build_summary_payload(security_state: str, headline: str, trust_boundaries: list[dict[str, Any]], evidence_paths: list[str]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.security.hardening.posture.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "security_state": security_state,
        "headline": headline,
        "trust_boundary_count": len(trust_boundaries),
        "posture_path": repo_rel(POSTURE_PATH),
        "evidence_paths": evidence_paths,
    }


def write_posture_outputs(posture_payload: dict[str, Any], summary_payload: dict[str, Any]) -> None:
    POSTURE_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(POSTURE_PATH, posture_payload)
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary_payload)
