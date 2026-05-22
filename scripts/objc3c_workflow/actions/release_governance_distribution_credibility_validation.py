"""Distribution credibility validation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from .release_governance_distribution_credibility_paths import (
    DISTRIBUTION_CREDIBILITY_DASHBOARD_PY,
    DISTRIBUTION_CREDIBILITY_END_TO_END_PY,
    DISTRIBUTION_CREDIBILITY_PUBLICATION_PY,
    DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY,
    RELEASE_OPERATIONS_INTEGRATION_PY,
)
from .release_governance_distribution_credibility_owner_contracts import (
    require_distribution_credibility_owner_contract,
)
from .schema_surfaces import DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY


def action_validate_distribution_credibility(rest: list[str]) -> int:
    reuse_upstream_report = False
    if rest == ["--reuse-upstream-report"]:
        reuse_upstream_report = True
    elif rest:
        raise RuntimeError(f"unexpected validate-distribution-credibility arguments: {rest}")

    require_distribution_credibility_owner_contract("validate-distribution-credibility")
    release_operations_command = (
        [sys.executable, str(RELEASE_OPERATIONS_INTEGRATION_PY)]
        if reuse_upstream_report
        else workflow_command("validate-release-operations")
    )
    return run_composite_validation(
        "validate-distribution-credibility",
        [
            ("validate-release-operations", release_operations_command),
            (
                "validate-packaging-channels-end-to-end",
                workflow_command(
                    "validate-packaging-channels-end-to-end",
                    "--use-existing-build-report",
                ),
            ),
            ("validate-package-install-distribution", workflow_command("validate-package-install-distribution")),
            (
                "check-distribution-credibility-surface",
                [sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)],
            ),
            (
                "check-distribution-credibility-schema-surface",
                [sys.executable, str(DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY)],
            ),
            (
                "build-distribution-credibility-dashboard",
                [sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)],
            ),
            (
                "publish-distribution-credibility",
                [sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)],
            ),
        ],
    )


def action_validate_distribution_credibility_end_to_end(_: list[str]) -> int:
    require_distribution_credibility_owner_contract("validate-distribution-credibility-end-to-end")
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_END_TO_END_PY)])


__all__ = [
    "action_validate_distribution_credibility",
    "action_validate_distribution_credibility_end_to_end",
]
