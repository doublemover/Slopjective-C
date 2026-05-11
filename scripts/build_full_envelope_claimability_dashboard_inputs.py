from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel, resolve_repo_path

from build_full_envelope_claimability_dashboard_io import read_json

try:
    from build_full_envelope_claimability_contracts import (
        CLAIM_POLICY_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        RELEASE_BLOCKER_SUMMARY,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_READINESS_SUMMARY,
        SUPPORT_MATRIX_SUMMARY,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        CLAIM_POLICY_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        RELEASE_BLOCKER_SUMMARY,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_READINESS_SUMMARY,
        SUPPORT_MATRIX_SUMMARY,
    )


@dataclass(frozen=True)
class ClaimabilityDashboardInputs:
    contract: dict[str, Any]
    schema: dict[str, Any]
    source_summaries: dict[str, dict[str, Any]]
    integration_reports: dict[str, dict[str, Any]]
    support_matrix: dict[str, Any]
    claim_policy: dict[str, Any]
    release_blockers: dict[str, Any]
    rollout: dict[str, Any]
    release_foundation: dict[str, Any]
    release_operations: dict[str, Any]
    distribution: dict[str, Any]


def load_claimability_inputs(*, root: Path) -> ClaimabilityDashboardInputs:
    contract_path = (
        root / "tests/tooling/fixtures/full_envelope_claimability/dashboard_reporting_contract.json"
    )
    schema_path = root / "schemas/objc3c-full-envelope-dashboard-summary-v1.schema.json"
    contract = read_json(contract_path)
    schema = read_json(schema_path)

    source_summaries = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_source_summaries"]
    }
    integration_reports = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_integration_reports"]
    }

    return ClaimabilityDashboardInputs(
        contract=contract,
        schema=schema,
        source_summaries=source_summaries,
        integration_reports=integration_reports,
        support_matrix=source_summaries[SUPPORT_MATRIX_SUMMARY],
        claim_policy=source_summaries[CLAIM_POLICY_SUMMARY],
        release_blockers=source_summaries[RELEASE_BLOCKER_SUMMARY],
        rollout=source_summaries[ROLLOUT_READINESS_SUMMARY],
        release_foundation=integration_reports[RELEASE_FOUNDATION_SUMMARY],
        release_operations=integration_reports[RELEASE_OPERATIONS_SUMMARY],
        distribution=integration_reports[DISTRIBUTION_CREDIBILITY_SUMMARY],
    )


__all__ = (
    "ClaimabilityDashboardInputs",
    "load_claimability_inputs",
)
