"""Distribution credibility workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .schema_surfaces import DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY

DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_distribution_credibility_source_surface.py"
)
DISTRIBUTION_CREDIBILITY_DASHBOARD_PY = (
    ROOT / "scripts" / "build_objc3c_distribution_credibility_dashboard.py"
)
DISTRIBUTION_CREDIBILITY_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_distribution_trust_report.py"
)
DISTRIBUTION_CREDIBILITY_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_distribution_credibility_end_to_end.py"
)


def action_check_distribution_credibility_surface(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)])


def action_build_distribution_credibility_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)])


def action_publish_distribution_credibility(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)])


def action_validate_distribution_credibility(_: list[str]) -> int:
    return run_composite_validation(
        "validate-distribution-credibility",
        [
            ("validate-release-operations", workflow_command("validate-release-operations")),
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
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_END_TO_END_PY)])
