"""Public conformance reporting release-governance actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .schema_surfaces import PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY

PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_public_conformance_reporting_source_surface.py"
)
PUBLIC_CONFORMANCE_SCORECARD_PY = (
    ROOT / "scripts" / "build_objc3c_public_conformance_scorecard.py"
)
PUBLIC_CONFORMANCE_REPORT_PY = (
    ROOT / "scripts" / "publish_objc3c_public_conformance_report.py"
)
PUBLIC_CONFORMANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_public_conformance_reporting_integration.py"
)
PUBLIC_CONFORMANCE_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_public_conformance_reporting_end_to_end.py"
)


def action_check_public_conformance_reporting_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY)])


def action_build_public_conformance_scorecard(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SCORECARD_PY)])


def action_publish_public_conformance_report(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_REPORT_PY)])


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
