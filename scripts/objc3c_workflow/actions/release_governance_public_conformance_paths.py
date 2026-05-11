"""Public-conformance paths, scripts, actions, and report contracts."""

from __future__ import annotations


PUBLIC_CONFORMANCE_FIXTURE_ROOT = (
    "tests/tooling/fixtures/public_conformance_reporting"
)
PUBLIC_CONFORMANCE_RUNBOOK = "docs/runbooks/objc3c_public_conformance_reporting.md"
PUBLIC_CONFORMANCE_SOURCE_SURFACE = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/source_surface.json"
)
PUBLIC_CONFORMANCE_SOURCE_README = f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/README.md"
PUBLIC_CONFORMANCE_STABILITY_POLICY_PATH = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/stability_policy.json"
)
PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PATH = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/schema_surface.json"
)
PUBLIC_CONFORMANCE_WORKFLOW_SURFACE_PATH = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/workflow_surface.json"
)

PUBLIC_CONFORMANCE_REPORT_ROOT = "tmp/reports/public-conformance"
PUBLIC_CONFORMANCE_ARTIFACT_ROOT = "tmp/artifacts/public-conformance"
PUBLIC_CONFORMANCE_SOURCE_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/source-surface-summary.json"
)
PUBLIC_CONFORMANCE_SCHEMA_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/schema-surface-summary.json"
)
PUBLIC_CONFORMANCE_SCORECARD_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/scorecard-summary.json"
)
PUBLIC_CONFORMANCE_PUBLIC_SUMMARY = f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/public-summary.json"
PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/integration-summary.json"
)
PUBLIC_CONFORMANCE_END_TO_END_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/end-to-end-summary.json"
)
PUBLIC_CONFORMANCE_PUBLISHED_SCORECARD = (
    f"{PUBLIC_CONFORMANCE_ARTIFACT_ROOT}/scorecard/public-conformance-scorecard.json"
)
PUBLIC_CONFORMANCE_PUBLISHED_BADGE = (
    f"{PUBLIC_CONFORMANCE_ARTIFACT_ROOT}/badge/public-conformance-badge.json"
)
PUBLIC_CONFORMANCE_PUBLISHED_REPORT = (
    f"{PUBLIC_CONFORMANCE_ARTIFACT_ROOT}/report/public-conformance-report.md"
)

PUBLIC_CONFORMANCE_SOURCE_CHECK_SCRIPT = (
    "scripts/check_public_conformance_reporting_source_surface.py"
)
PUBLIC_CONFORMANCE_SCHEMA_CHECK_SCRIPT = (
    "scripts/check_public_conformance_schema_surface.py"
)
PUBLIC_CONFORMANCE_SCORECARD_SCRIPT = (
    "scripts/build_objc3c_public_conformance_scorecard.py"
)
PUBLIC_CONFORMANCE_REPORT_SCRIPT = (
    "scripts/publish_objc3c_public_conformance_report.py"
)
PUBLIC_CONFORMANCE_INTEGRATION_SCRIPT = (
    "scripts/check_objc3c_public_conformance_reporting_integration.py"
)
PUBLIC_CONFORMANCE_END_TO_END_SCRIPT = (
    "scripts/check_objc3c_public_conformance_reporting_end_to_end.py"
)

PUBLIC_CONFORMANCE_REQUIRED_ACTIONS = (
    "check-public-conformance-reporting-surface",
    "check-public-conformance-schema-surface",
    "build-public-conformance-scorecard",
    "publish-public-conformance-report",
    "validate-public-conformance-reporting",
    "validate-public-conformance-reporting-integration",
    "validate-public-conformance-reporting-end-to-end",
)
PUBLIC_CONFORMANCE_COMPOSITE_CHILD_ACTIONS = PUBLIC_CONFORMANCE_REQUIRED_ACTIONS[:4]

PUBLIC_CONFORMANCE_REPORT_PATHS = (
    PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
    PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
    PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
    PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,
    PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY,
    PUBLIC_CONFORMANCE_END_TO_END_SUMMARY,
)
PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS = (
    PUBLIC_CONFORMANCE_PUBLISHED_SCORECARD,
    PUBLIC_CONFORMANCE_PUBLISHED_BADGE,
    PUBLIC_CONFORMANCE_PUBLISHED_REPORT,
)
PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS = (
    (
        PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
        "objc3c.public_conformance_reporting.source.surface.summary.v1",
    ),
    (
        PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
        "objc3c.public_conformance_reporting.schema.surface.summary.v1",
    ),
    (
        PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
        "objc3c.public_conformance_reporting.scorecard.summary.v1",
    ),
    (
        PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,
        "objc3c.public_conformance_reporting.summary.v1",
    ),
)


__all__ = tuple(name for name in globals() if name.startswith("PUBLIC_CONFORMANCE_"))
