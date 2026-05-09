"""Owner contracts for distribution credibility workflow actions."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class DistributionCredibilityOwnerContract:
    action_name: str
    owner_role: str
    source_contracts: tuple[str, ...]
    required_artifacts: tuple[str, ...]
    claim_boundary: str
    blocking_conditions: tuple[str, ...]
    report_only_allowed: bool = False
    wrapper_only_allowed: bool = False


DISTRIBUTION_SOURCE_CONTRACTS = (
    "tests/tooling/fixtures/distribution_credibility/source_surface.json",
    "tests/tooling/fixtures/distribution_credibility/schema_surface.json",
    "tests/tooling/fixtures/distribution_credibility/workflow_surface.json",
    "tests/tooling/fixtures/distribution_credibility/trust_signal_architecture.json",
    "tests/tooling/fixtures/distribution_credibility/release_drill_policy.json",
    "tests/tooling/fixtures/distribution_credibility/operator_release_policy.json",
    "tests/tooling/fixtures/distribution_credibility/artifact_surface.json",
)
DISTRIBUTION_BLOCKING_CONDITIONS = (
    "trust report generated without live release drill evidence",
    "distribution action exposes a report-only credibility claim",
    "publication detaches from machine-owned release manifest lineage",
    "operator-authored trust badge replaces checked-in source contract evidence",
    "wrapper-only distribution publication action",
)


def _contract(
    action_name: str,
    owner_role: str,
    required_artifacts: tuple[str, ...],
    claim_boundary: str,
) -> DistributionCredibilityOwnerContract:
    return DistributionCredibilityOwnerContract(
        action_name=action_name,
        owner_role=owner_role,
        source_contracts=DISTRIBUTION_SOURCE_CONTRACTS,
        required_artifacts=required_artifacts,
        claim_boundary=claim_boundary,
        blocking_conditions=DISTRIBUTION_BLOCKING_CONDITIONS,
    )


DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS: dict[str, DistributionCredibilityOwnerContract] = {
    "check-distribution-credibility-surface": _contract(
        "check-distribution-credibility-surface",
        "distribution-credibility-source-owner",
        ("tmp/reports/distribution-credibility/source-surface-summary.json",),
        "source-surface checks are credible only when every trust input has checked-in owner data",
    ),
    "check-distribution-credibility-schema-surface": _contract(
        "check-distribution-credibility-schema-surface",
        "distribution-credibility-schema-owner",
        ("tmp/reports/distribution-credibility/schema-surface-summary.json",),
        "schema checks must bind dashboard and trust-report artifacts to registered schemas",
    ),
    "build-distribution-credibility-dashboard": _contract(
        "build-distribution-credibility-dashboard",
        "distribution-credibility-dashboard-owner",
        (
            "tmp/reports/distribution-credibility/dashboard-summary.json",
            "tmp/artifacts/distribution-credibility/dashboard/distribution-credibility-dashboard.json",
        ),
        "dashboard readiness is a projection of live release evidence, not a hand-authored status page",
    ),
    "publish-distribution-credibility": _contract(
        "publish-distribution-credibility",
        "distribution-credibility-trust-owner",
        (
            "tmp/reports/distribution-credibility/publication-summary.json",
            "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.json",
        ),
        "trust-report publication must preserve release drill lineage and may not stand alone as report-only evidence",
    ),
    "validate-distribution-credibility": _contract(
        "validate-distribution-credibility",
        "distribution-credibility-gate-owner",
        ("tmp/reports/distribution-credibility/integration-summary.json",),
        "integrated distribution credibility passes only when source, schema, dashboard, and trust publication owners agree",
    ),
    "validate-distribution-credibility-end-to-end": _contract(
        "validate-distribution-credibility-end-to-end",
        "distribution-credibility-release-drill-owner",
        ("tmp/reports/distribution-credibility/end-to-end-summary.json",),
        "end-to-end readiness requires release drill evidence from package channels, release operations, and release evidence",
    ),
}


def distribution_credibility_owner_contract(action_name: str) -> DistributionCredibilityOwnerContract:
    return DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS[action_name]


def require_distribution_credibility_owner_contract(action_name: str) -> DistributionCredibilityOwnerContract:
    contract = distribution_credibility_owner_contract(action_name)
    if not contract.source_contracts:
        raise RuntimeError(f"{action_name} has no source owner contract")
    if not contract.required_artifacts:
        raise RuntimeError(f"{action_name} has no required artifact contract")
    if contract.report_only_allowed:
        raise RuntimeError(f"{action_name} is report-only")
    if contract.wrapper_only_allowed:
        raise RuntimeError(f"{action_name} is wrapper-only")
    return contract


__all__ = [
    "DISTRIBUTION_BLOCKING_CONDITIONS",
    "DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS",
    "DISTRIBUTION_SOURCE_CONTRACTS",
    "DistributionCredibilityOwnerContract",
    "distribution_credibility_owner_contract",
    "require_distribution_credibility_owner_contract",
]
