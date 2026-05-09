"""Public conformance reporting action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-public-conformance-reporting-surface": ActionSpec(
        "check-public-conformance-reporting-surface",
        "validate the checked-in public-conformance reporting source and policy contracts",
        "python:scripts/check_public_conformance_reporting_source_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance reporting stays rooted in checked-in source, policy, "
            "and upstream evidence boundaries"
        ),
    ),
    "check-public-conformance-schema-surface": ActionSpec(
        "check-public-conformance-schema-surface",
        "validate the checked-in public-conformance schema surface",
        "python:scripts/check_public_conformance_schema_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance scorecard and summary outputs stay tied to checked-in "
            "schema contracts"
        ),
    ),
    "build-public-conformance-scorecard": ActionSpec(
        "build-public-conformance-scorecard",
        "derive the live public-conformance badge and stability score from upstream reports",
        "python:scripts/build_objc3c_public_conformance_scorecard.py",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance scoring stays derived from live conformance and "
            "external-validation reports"
        ),
    ),
    "publish-public-conformance-report": ActionSpec(
        "publish-public-conformance-report",
        "materialize the machine-owned public-conformance summary and publication artifacts",
        "python:scripts/publish_objc3c_public_conformance_report.py",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance summary and publication artifacts stay derived from "
            "the live scorecard and checked-in schema surface"
        ),
    ),
    "validate-public-conformance-reporting": ActionSpec(
        "validate-public-conformance-reporting",
        "run the integrated public-conformance reporting workflow",
        "runner-internal public-reporting child actions",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance reporting source policy schema scorecard and publication "
            "flows stay executable on the live workflow"
        ),
    ),
    "validate-public-conformance-reporting-integration": ActionSpec(
        "validate-public-conformance-reporting-integration",
        "validate the integrated public-conformance reporting workflow report and child artifacts",
        "python:scripts/check_objc3c_public_conformance_reporting_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "integrated public conformance reporting artifacts stay coherent across "
            "source, schema, scorecard, and publication outputs"
        ),
    ),
    "validate-public-conformance-reporting-end-to-end": ActionSpec(
        "validate-public-conformance-reporting-end-to-end",
        "validate public-conformance reporting entrypoints, command-surface sync, and nightly wiring",
        "python:scripts/check_objc3c_public_conformance_reporting_end_to_end.py",
        validation_tier="repo",
        guarantee_owner=(
            "public conformance reporting entrypoints and nightly wiring stay coherent "
            "with the integrated reporting artifacts"
        ),
    ),
}
