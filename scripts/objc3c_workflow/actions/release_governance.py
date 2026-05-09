"""Release, packaging, distribution, and security workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .release_governance_foundation import (
    action_build_release_manifest,
    action_check_release_foundation_surface,
    action_publish_release_provenance,
    action_validate_release_foundation,
)
from .release_governance_operations import (
    action_build_update_manifest,
    action_check_release_operations_surface,
    action_publish_release_operations,
    action_validate_release_operations,
    action_validate_release_operations_end_to_end,
)
from .release_governance_packaging_channels import (
    action_build_package_channels,
    action_build_platform_support_matrix,
    action_check_packaging_channels_surface,
    action_validate_packaging_channels,
    action_validate_packaging_channels_end_to_end,
    action_validate_platform_hardening,
    action_validate_platform_hardening_end_to_end,
)
from .release_governance_public_conformance import (
    action_build_public_conformance_scorecard,
    action_check_public_conformance_reporting_surface,
    action_publish_public_conformance_report,
    action_validate_public_conformance_reporting,
    action_validate_public_conformance_reporting_end_to_end,
    action_validate_public_conformance_reporting_integration,
)
from .schema_surfaces import (
    DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY,
    SECURITY_HARDENING_SCHEMA_SURFACE_PY,
)

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
SECURITY_HARDENING_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_security_hardening_source_surface.py"
)
SECURITY_HARDENING_RESPONSE_DRILL_PY = (
    ROOT / "scripts" / "check_security_hardening_response_drill.py"
)
SECURITY_HARDENING_RUNTIME_HARDENING_PY = (
    ROOT / "scripts" / "check_security_hardening_runtime_hardening.py"
)
SECURITY_HARDENING_POSTURE_PY = (
    ROOT / "scripts" / "build_objc3c_security_posture.py"
)
SECURITY_HARDENING_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_security_advisories.py"
)
SECURITY_HARDENING_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_security_hardening_end_to_end.py"
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


def action_check_security_hardening_surface(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)])


def action_build_security_posture(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_POSTURE_PY)])


def action_publish_security_advisories(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)])


def action_validate_security_hardening(_: list[str]) -> int:
    return run_composite_validation(
        "validate-security-hardening",
        [
            (
                "check-security-response-drill",
                [sys.executable, str(SECURITY_HARDENING_RESPONSE_DRILL_PY)],
            ),
            (
                "check-security-runtime-hardening",
                [sys.executable, str(SECURITY_HARDENING_RUNTIME_HARDENING_PY)],
            ),
            (
                "check-security-hardening-surface",
                [sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)],
            ),
            (
                "check-security-hardening-schema-surface",
                [sys.executable, str(SECURITY_HARDENING_SCHEMA_SURFACE_PY)],
            ),
            ("build-security-posture", [sys.executable, str(SECURITY_HARDENING_POSTURE_PY)]),
            (
                "publish-security-advisories",
                [sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)],
            ),
        ],
    )


def action_validate_security_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_END_TO_END_PY)])
