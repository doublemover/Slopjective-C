"""Release, packaging, distribution, and security workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..environment import ROOT
from .schema_surfaces import (
    DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY,
    PACKAGING_CHANNELS_SCHEMA_SURFACE_PY,
    PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY,
    RELEASE_FOUNDATION_SCHEMA_SURFACE_PY,
    RELEASE_OPERATIONS_SCHEMA_SURFACE_PY,
    SECURITY_HARDENING_SCHEMA_SURFACE_PY,
)

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
RELEASE_FOUNDATION_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_foundation_source_surface.py"
)
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
PACKAGING_CHANNELS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_packaging_channels_source_surface.py"
)
PACKAGE_CHANNELS_BUILD_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
PACKAGING_CHANNELS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"
)
PLATFORM_SUPPORT_MATRIX_PY = (
    ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
)
PLATFORM_HARDENING_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_platform_hardening_integration.py"
)
RUNNABLE_PLATFORM_HARDENING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_platform_hardening_end_to_end.py"
)
RELEASE_OPERATIONS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_operations_source_surface.py"
)
UPDATE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_update_manifest.py"
RELEASE_OPERATIONS_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_release_operations_metadata.py"
)
RELEASE_OPERATIONS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_release_operations_end_to_end.py"
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


def action_check_public_conformance_reporting_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY)])


def action_build_public_conformance_scorecard(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SCORECARD_PY)])


def action_publish_public_conformance_report(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_REPORT_PY)])


def action_validate_public_conformance_reporting(_: list[str]) -> int:
    from scripts.objc3c_workflow.runner import run_composite_validation

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


def action_check_release_foundation_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)])


def action_build_release_manifest(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_MANIFEST_PY)])


def action_publish_release_provenance(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_PROVENANCE_PY)])


def action_validate_release_foundation(_: list[str]) -> int:
    from scripts.objc3c_workflow.runner import run_composite_validation

    return run_composite_validation(
        "validate-release-foundation",
        [
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("validate-runnable-release-candidate", workflow_command("validate-runnable-release-candidate")),
            (
                "check-release-evidence",
                [sys.executable, str(RELEASE_EVIDENCE_PY)],
            ),
            (
                "check-release-foundation-surface",
                [sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)],
            ),
            (
                "check-release-foundation-schema-surface",
                [sys.executable, str(RELEASE_FOUNDATION_SCHEMA_SURFACE_PY)],
            ),
            ("build-release-manifest", [sys.executable, str(RELEASE_MANIFEST_PY)]),
            (
                "publish-release-provenance",
                [sys.executable, str(RELEASE_PROVENANCE_PY)],
            ),
        ],
    )


def action_check_packaging_channels_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)])


def action_build_package_channels(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)])


def action_build_platform_support_matrix(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_SUPPORT_MATRIX_PY)])


def action_validate_packaging_channels(_: list[str]) -> int:
    from scripts.objc3c_workflow.runner import run_composite_validation

    return run_composite_validation(
        "validate-packaging-channels",
        [
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            (
                "check-packaging-channels-surface",
                [sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)],
            ),
            (
                "check-packaging-channels-schema-surface",
                [sys.executable, str(PACKAGING_CHANNELS_SCHEMA_SURFACE_PY)],
            ),
            ("build-package-channels", [sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)]),
        ],
    )


def action_validate_packaging_channels_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_END_TO_END_PY)])


def action_validate_platform_hardening(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_HARDENING_INTEGRATION_PY)])


def action_validate_platform_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PLATFORM_HARDENING_E2E_PY)])


def action_check_release_operations_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)])


def action_build_update_manifest(_: list[str]) -> int:
    return run([sys.executable, str(UPDATE_MANIFEST_PY)])


def action_publish_release_operations(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)])


def action_validate_release_operations(_: list[str]) -> int:
    from scripts.objc3c_workflow.runner import run_composite_validation

    rc = run_composite_validation(
        "validate-release-operations",
        [
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            (
                "check-release-operations-surface",
                [sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)],
            ),
            (
                "check-release-operations-schema-surface",
                [sys.executable, str(RELEASE_OPERATIONS_SCHEMA_SURFACE_PY)],
            ),
            ("build-update-manifest", [sys.executable, str(UPDATE_MANIFEST_PY)]),
            (
                "publish-release-operations",
                [sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)],
            ),
        ],
    )
    if rc != 0:
        return rc
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY), "--skip-upstream"])


def action_validate_release_operations_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY)])


def action_check_distribution_credibility_surface(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)])


def action_build_distribution_credibility_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)])


def action_publish_distribution_credibility(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)])


def action_validate_distribution_credibility(_: list[str]) -> int:
    from scripts.objc3c_workflow.runner import run_composite_validation

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
    from scripts.objc3c_workflow.runner import run_composite_validation

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
