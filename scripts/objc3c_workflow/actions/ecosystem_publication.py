"""Package ecosystem and planning publication workflow actions."""

from __future__ import annotations

from ..commands import run
from .ecosystem_publication_contracts import (
    ADOPTION_LEGIBILITY_INTEGRATION_PY,
    ADOPTION_LEGIBILITY_PUBLICATION_PY,
    GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY,
    GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY,
    LONG_HORIZON_OPERATIONS_INTEGRATION_PY,
    LONG_HORIZON_OPERATIONS_PUBLICATION_PY,
    PACKAGE_AUTHORING_WORKFLOW_PY,
    PACKAGE_ECOSYSTEM_INTEGRATION_PY,
    PACKAGE_LOCK_PY,
    PACKAGE_MIRROR_REPRODUCIBILITY_PY,
    PLANNING_ISSUE_PUBLISHER_PY,
    PLANNING_PUBLICATION_AUDIT_PY,
    PUBLICATION_ARTIFACT_CONTRACTS,
    RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY,
    PublicationArtifactContract,
)
from .ecosystem_publication_metadata import (
    ACTION_FEEDS,
    PACKAGE_FEED_METADATA,
    PublicationFeedMetadata,
)


def _run_publication_action(action_name: str, rest: list[str] | None = None) -> int:
    contract = PUBLICATION_ARTIFACT_CONTRACTS[action_name]
    return run(contract.command(rest))


def action_build_package_lock(_: list[str]) -> int:
    return _run_publication_action("build-package-lock")


def action_validate_package_authoring(_: list[str]) -> int:
    return _run_publication_action("validate-package-authoring")


def action_validate_package_mirror(_: list[str]) -> int:
    return _run_publication_action("validate-package-mirror")


def action_validate_package_ecosystem(_: list[str]) -> int:
    return _run_publication_action("validate-package-ecosystem")


def action_validate_runnable_package_ecosystem(_: list[str]) -> int:
    return _run_publication_action("validate-runnable-package-ecosystem")


def action_validate_long_horizon_operations(_: list[str]) -> int:
    return _run_publication_action("validate-long-horizon-operations")


def action_publish_long_horizon_operations(_: list[str]) -> int:
    return _run_publication_action("publish-long-horizon-operations")


def action_validate_adoption_legibility(_: list[str]) -> int:
    return _run_publication_action("validate-adoption-legibility")


def action_publish_adoption_legibility(_: list[str]) -> int:
    return _run_publication_action("publish-adoption-legibility")


def action_validate_governance_sustainability(_: list[str]) -> int:
    return _run_publication_action("validate-governance-sustainability")


def action_publish_governance_sustainability(_: list[str]) -> int:
    return _run_publication_action("publish-governance-sustainability")


def action_publish_planning_issues(rest: list[str]) -> int:
    return _run_publication_action("publish-planning-issues", rest)


def action_check_planning_publication_drift(rest: list[str]) -> int:
    return _run_publication_action("check-planning-publication-drift", rest)


__all__ = [
    "ACTION_FEEDS",
    "ADOPTION_LEGIBILITY_INTEGRATION_PY",
    "ADOPTION_LEGIBILITY_PUBLICATION_PY",
    "GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY",
    "GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY",
    "LONG_HORIZON_OPERATIONS_INTEGRATION_PY",
    "LONG_HORIZON_OPERATIONS_PUBLICATION_PY",
    "PACKAGE_AUTHORING_WORKFLOW_PY",
    "PACKAGE_ECOSYSTEM_INTEGRATION_PY",
    "PACKAGE_FEED_METADATA",
    "PACKAGE_LOCK_PY",
    "PACKAGE_MIRROR_REPRODUCIBILITY_PY",
    "PLANNING_ISSUE_PUBLISHER_PY",
    "PLANNING_PUBLICATION_AUDIT_PY",
    "PUBLICATION_ARTIFACT_CONTRACTS",
    "PublicationArtifactContract",
    "PublicationFeedMetadata",
    "RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY",
    "action_build_package_lock",
    "action_check_planning_publication_drift",
    "action_publish_adoption_legibility",
    "action_publish_governance_sustainability",
    "action_publish_long_horizon_operations",
    "action_publish_planning_issues",
    "action_validate_adoption_legibility",
    "action_validate_governance_sustainability",
    "action_validate_long_horizon_operations",
    "action_validate_package_authoring",
    "action_validate_package_ecosystem",
    "action_validate_package_mirror",
    "action_validate_runnable_package_ecosystem",
]
