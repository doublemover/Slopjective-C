#!/usr/bin/env python3
from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

try:
    from objc3c_tooling.paths import repo_rel, resolve_repo_path
except ModuleNotFoundError:
    from scripts.objc3c_tooling.paths import repo_rel, resolve_repo_path

ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests/tooling/fixtures/full_envelope_claimability"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"

SUPPORT_MATRIX_SUMMARY = "tmp/reports/full-envelope-claimability/support-matrix/support_matrix_summary.json"
CLAIM_POLICY_SUMMARY = "tmp/reports/full-envelope-claimability/claim-policy/claim_policy_summary.json"
RELEASE_BLOCKER_SUMMARY = "tmp/reports/full-envelope-claimability/release-blockers/release_blocker_summary.json"
ROLLOUT_READINESS_SUMMARY = "tmp/reports/full-envelope-claimability/rollout-readiness/rollout_readiness_summary.json"
DASHBOARD_SUMMARY = "tmp/reports/full-envelope-claimability/dashboard-summary.json"
PUBLIC_SUMMARY = "tmp/reports/full-envelope-claimability/public-summary.json"
CLAIMABILITY_REPORT_MD = "tmp/artifacts/full-envelope-claimability/report/full-envelope-claimability-report.md"

CONFORMANCE_CORPUS_SUMMARY = "tmp/reports/conformance/corpus-integration-summary.json"
STRESS_INTEGRATION_SUMMARY = "tmp/reports/stress/integration-summary.json"
EXTERNAL_VALIDATION_SUMMARY = "tmp/reports/external-validation/integration-summary.json"
PUBLIC_CONFORMANCE_SUMMARY = "tmp/reports/public-conformance/integration-summary.json"
PERFORMANCE_GOVERNANCE_SUMMARY = "tmp/reports/performance-governance/integration-summary.json"
RELEASE_FOUNDATION_SUMMARY = "tmp/reports/release-foundation/integration-summary.json"
RELEASE_OPERATIONS_SUMMARY = "tmp/reports/release-operations/integration-summary.json"
DISTRIBUTION_CREDIBILITY_SUMMARY = "tmp/reports/distribution-credibility/integration-summary.json"

ROLLOUT_CLASS_STABLE = "stable"
ROLLOUT_CLASS_CANDIDATE = "candidate"
ROLLOUT_CLASS_PREVIEW = "preview"
ROLLOUT_CLASSES = (ROLLOUT_CLASS_STABLE, ROLLOUT_CLASS_CANDIDATE, ROLLOUT_CLASS_PREVIEW)

PUBLIC_CLAIM_PRODUCTION_STRENGTH = "production-strength"
PUBLIC_CLAIM_CANDIDATE_SCOPED = "candidate-scoped"
PUBLIC_CLAIM_PREVIEW_ONLY = "preview-only"
PUBLIC_CLAIM_CLASSES = (
    PUBLIC_CLAIM_PRODUCTION_STRENGTH,
    PUBLIC_CLAIM_CANDIDATE_SCOPED,
    PUBLIC_CLAIM_PREVIEW_ONLY,
)
NON_PRODUCTION_PUBLIC_CLAIM_CLASSES = (
    PUBLIC_CLAIM_CANDIDATE_SCOPED,
    PUBLIC_CLAIM_PREVIEW_ONLY,
)

RELEASE_ARTIFACT_FIELDS = (
    "release_manifest_path",
    "published_sbom",
    "published_attestation",
    "update_manifest_path",
    "compatibility_report",
    "channel_catalog",
    "trust_report_json",
)

DASHBOARD_DECISION_FIELDS = (
    "current_rollout_class",
    "public_claim_class",
    "production_strength_claimable",
    "triggered_release_blockers",
    "dashboard_release_blocker_projection",
    "acceptance_matrix",
    "release_artifacts",
)

PUBLIC_SUMMARY_DECISION_FIELDS = (
    "current_rollout_class",
    "public_claim_class",
    "production_strength_claimable",
    "dashboard_release_blocker",
    "dashboard_blocks_production_strength_claim",
    "report_markdown_path",
)


@dataclass(frozen=True)
class AcceptanceFamily:
    family: str
    report_path: str
    claim_signal: str


ACCEPTANCE_FAMILIES = (
    AcceptanceFamily("conformance-corpus", CONFORMANCE_CORPUS_SUMMARY, "passing-corpus-gate"),
    AcceptanceFamily("stress-integration", STRESS_INTEGRATION_SUMMARY, "passing-stress-gate"),
    AcceptanceFamily("external-validation", EXTERNAL_VALIDATION_SUMMARY, "passing-external-validation-gate"),
    AcceptanceFamily("public-conformance", PUBLIC_CONFORMANCE_SUMMARY, "public_status"),
    AcceptanceFamily("performance-governance", PERFORMANCE_GOVERNANCE_SUMMARY, "release_status"),
    AcceptanceFamily("release-foundation", RELEASE_FOUNDATION_SUMMARY, "release_artifacts"),
    AcceptanceFamily("release-operations", RELEASE_OPERATIONS_SUMMARY, "release_artifacts"),
    AcceptanceFamily("distribution-credibility", DISTRIBUTION_CREDIBILITY_SUMMARY, "trust_state"),
)


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def require_string(payload: dict[str, Any], key: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"`{key}` must be a non-empty string")
    return value


def require_string_list(payload: dict[str, Any], key: str) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list) or not value:
        raise RuntimeError(f"`{key}` must be a non-empty list")
    strings: list[str] = []
    for index, item in enumerate(value):
        if not isinstance(item, str) or not item:
            raise RuntimeError(f"`{key}[{index}]` must be a non-empty string")
        strings.append(item)
    return strings


def public_claim_class_for_rollout(
    current_rollout_class: str,
    production_strength_claimable: bool,
) -> str:
    if current_rollout_class == ROLLOUT_CLASS_STABLE and production_strength_claimable:
        return PUBLIC_CLAIM_PRODUCTION_STRENGTH
    if current_rollout_class == ROLLOUT_CLASS_CANDIDATE:
        return PUBLIC_CLAIM_CANDIDATE_SCOPED
    return PUBLIC_CLAIM_PREVIEW_ONLY


def resolve_report_map(paths: list[str]) -> dict[str, dict[str, Any]]:
    return {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in paths
    }


def report_status(payload: dict[str, Any]) -> str | None:
    status = payload.get("status")
    return status if isinstance(status, str) else None
