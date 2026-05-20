#!/usr/bin/env python3
"""Validate the integrated release-foundation workflow and publication outputs."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_release_manifest.hashing import sha256_file
from objc3c_workflow.action_catalog_release_foundation import (
    RELEASE_FOUNDATION_VALIDATE_CHILD_ACTIONS,
)

ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_REPORT = ROOT / 'tmp' / 'reports' / 'objc3c-public-workflow' / 'validate-release-foundation.json'
WORKFLOW_SURFACE = ROOT / 'tests' / 'tooling' / 'fixtures' / 'release_foundation' / 'workflow_surface.json'
MANIFEST_SUMMARY = ROOT / 'tmp' / 'reports' / 'release-foundation' / 'release-manifest-summary.json'
ABI_API_DRIFT_SUMMARY = ROOT / 'tmp' / 'reports' / 'release-foundation' / 'abi-api-drift-summary.json'
PUBLICATION_SUMMARY = ROOT / 'tmp' / 'reports' / 'release-foundation' / 'publication-summary.json'
SUMMARY_PATH = ROOT / 'tmp' / 'reports' / 'release-foundation' / 'integration-summary.json'
MANIFEST_PATH = ROOT / 'tmp' / 'artifacts' / 'release-foundation' / 'manifest' / 'objc3c-release-manifest.json'
SBOM_PATH = ROOT / 'tmp' / 'artifacts' / 'release-foundation' / 'sbom' / 'objc3c-release-sbom.json'
ATTESTATION_PATH = ROOT / 'tmp' / 'artifacts' / 'release-foundation' / 'attestation' / 'objc3c-release-attestation.json'
INTEGRATION_SUMMARY_CONTRACT_ID = 'objc3c.release.foundation.integration.summary.v1'

REQUIRED_STEPS = list(RELEASE_FOUNDATION_VALIDATE_CHILD_ACTIONS)

REQUIRED_REPORT_CONTRACTS = {
    'abi_api_drift_summary': 'objc3c.release.foundation.abi_api_drift.summary.v1',
    'manifest_summary': 'objc3c.release.foundation.manifest.summary.v1',
    'publication_summary': 'objc3c.release.foundation.publication.summary.v1',
}


def fail(message: str) -> int:
    print(f"objc3c-release-foundation-integration: {message}", file=sys.stderr)
    return 1


def require_contract(payload: dict[str, Any], label: str) -> int | None:
    expected_contract = REQUIRED_REPORT_CONTRACTS[label]
    if payload.get('contract_id') != expected_contract:
        fail(f"{label} contract drifted from {expected_contract}")
        return 1
    return None


def require_artifact_digest(
    *,
    summary: dict[str, Any],
    path_field: str,
    digest_field: str,
    expected_path: Path,
) -> int | None:
    if summary.get(path_field) != repo_rel(expected_path):
        fail(f"{path_field} drifted from {repo_rel(expected_path)}")
        return 1
    if not expected_path.is_file():
        fail(f"missing published artifact {repo_rel(expected_path)}")
        return 1
    if summary.get(digest_field) != sha256_file(expected_path):
        fail(f"{digest_field} drifted from {repo_rel(expected_path)}")
        return 1
    return None


def main() -> int:
    for path in (WORKFLOW_REPORT, WORKFLOW_SURFACE, ABI_API_DRIFT_SUMMARY, MANIFEST_SUMMARY, PUBLICATION_SUMMARY):
        if not path.is_file():
            return fail(f"missing required artifact {repo_rel(path)}")

    workflow_report = load_json(WORKFLOW_REPORT)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    abi_api_drift_summary = load_json(ABI_API_DRIFT_SUMMARY)
    manifest_summary = load_json(MANIFEST_SUMMARY)
    publication_summary = load_json(PUBLICATION_SUMMARY)

    if workflow_report.get('status') != 'PASS':
        return fail('workflow report did not pass')
    steps = workflow_report.get('steps')
    if not isinstance(steps, list):
        return fail('workflow report was missing steps')
    step_actions = [step.get('action') for step in steps if isinstance(step, dict)]
    if step_actions != REQUIRED_STEPS:
        return fail(f"workflow steps drifted: {step_actions}")
    if workflow_surface.get('validate_action') != 'validate-release-foundation':
        return fail('workflow surface drifted from validate-release-foundation')
    if workflow_surface.get('ordered_child_actions') != REQUIRED_STEPS:
        return fail('workflow surface ordered_child_actions drifted')
    if workflow_surface.get('report_contracts') != REQUIRED_REPORT_CONTRACTS:
        return fail('workflow surface report_contracts drifted')
    if abi_api_drift_summary.get('status') != 'PASS':
        return fail('ABI/API drift summary did not pass')
    if manifest_summary.get('status') != 'PASS':
        return fail('release manifest summary did not pass')
    if publication_summary.get('status') != 'PASS':
        return fail('release publication summary did not pass')
    if require_contract(abi_api_drift_summary, 'abi_api_drift_summary') is not None:
        return 1
    if require_contract(manifest_summary, 'manifest_summary') is not None:
        return 1
    if require_contract(publication_summary, 'publication_summary') is not None:
        return 1
    current_abi_api_drift_summary_sha256 = sha256_file(ABI_API_DRIFT_SUMMARY)
    if manifest_summary.get('abi_api_drift_summary_sha256') != current_abi_api_drift_summary_sha256:
        return fail('abi_api_drift_summary_sha256 drifted from current ABI/API drift summary')
    for path_field, digest_field, expected_path in (
        ('release_manifest_path', 'release_manifest_sha256', MANIFEST_PATH),
        ('sbom_path', 'sbom_sha256', SBOM_PATH),
        ('attestation_path', 'attestation_sha256', ATTESTATION_PATH),
    ):
        if require_artifact_digest(
            summary=publication_summary,
            path_field=path_field,
            digest_field=digest_field,
            expected_path=expected_path,
        ) is not None:
            return 1

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        'contract_id': INTEGRATION_SUMMARY_CONTRACT_ID,
        'status': 'PASS',
        'workflow_report': repo_rel(WORKFLOW_REPORT),
        'validated_steps': REQUIRED_STEPS,
        'abi_api_drift_summary_path': repo_rel(ABI_API_DRIFT_SUMMARY),
        'abi_api_drift_summary_sha256': current_abi_api_drift_summary_sha256,
        'release_manifest_path': manifest_summary.get('release_manifest_path'),
        'release_manifest_sha256': publication_summary.get('release_manifest_sha256'),
        'published_sbom': publication_summary.get('sbom_path'),
        'published_sbom_sha256': publication_summary.get('sbom_sha256'),
        'published_attestation': publication_summary.get('attestation_path'),
        'published_attestation_sha256': publication_summary.get('attestation_sha256'),
        'report_contracts': REQUIRED_REPORT_CONTRACTS,
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print('objc3c-release-foundation-integration: PASS')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
