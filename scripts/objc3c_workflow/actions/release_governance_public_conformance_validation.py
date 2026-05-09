"""Public-conformance reporting validation actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from .release_governance_public_conformance_paths import (
    PUBLIC_CONFORMANCE_END_TO_END_PY,
    PUBLIC_CONFORMANCE_INTEGRATION_PY,
    PUBLIC_CONFORMANCE_REPORT_PY,
    PUBLIC_CONFORMANCE_SCORECARD_PY,
    PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY,
)
from .schema_surfaces import PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY


def action_validate_public_conformance_reporting(_: list[str]) -> int:
    return run_composite_validation(
        "validate-public-conformance-reporting",
        [
            (
                "check-public-conformance-reporting-surface",
                [sys.executable, str(PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY)],
            ),
            (
                "check-public-conformance-schema-surface",
                [sys.executable, str(PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY)],
            ),
            (
                "build-public-conformance-scorecard",
                [sys.executable, str(PUBLIC_CONFORMANCE_SCORECARD_PY)],
            ),
            (
                "publish-public-conformance-report",
                [sys.executable, str(PUBLIC_CONFORMANCE_REPORT_PY)],
            ),
        ],
    )


def action_validate_public_conformance_reporting_integration(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_INTEGRATION_PY)])


def action_validate_public_conformance_reporting_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_END_TO_END_PY)])
