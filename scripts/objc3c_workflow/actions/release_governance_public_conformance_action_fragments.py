"""Public-conformance reporting action payload fragments."""

from __future__ import annotations

from ..action_spec import ActionSpec
from .release_governance_public_conformance_contracts import (
    PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS,
    PUBLIC_CONFORMANCE_END_TO_END_SCRIPT,
    PUBLIC_CONFORMANCE_END_TO_END_SUMMARY,
    PUBLIC_CONFORMANCE_INTEGRATION_SCRIPT,
    PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY,
    PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,
    PUBLIC_CONFORMANCE_PUBLIC_SUITE_SUMMARY,
    PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS,
    PUBLIC_CONFORMANCE_REPORT_SCRIPT,
    PUBLIC_CONFORMANCE_SCHEMA_CHECK_SCRIPT,
    PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
    PUBLIC_CONFORMANCE_SCORECARD_SCRIPT,
    PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
    PUBLIC_CONFORMANCE_SOURCE_CHECK_SCRIPT,
    PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
)
from .release_governance_public_conformance_models import (
    PublicConformanceActionFragment,
)


def _python_backend(script_path: str) -> str:
    return f"python:{script_path}"


PUBLIC_CONFORMANCE_ACTION_FRAGMENTS: dict[str, PublicConformanceActionFragment] = {
    "check-public-conformance-reporting-surface": PublicConformanceActionFragment(
        action="check-public-conformance-reporting-surface",
        summary="validate checked-in public-conformance source, owner, and policy contracts",
        backend=_python_backend(PUBLIC_CONFORMANCE_SOURCE_CHECK_SCRIPT),
        guarantee_owner=(
            "public conformance source truth stays rooted in checked-in corpus, "
            "external-validation, schema, and stability-policy owner models"
        ),
        report_paths=(PUBLIC_CONFORMANCE_SOURCE_SUMMARY,),
        upstream_truth=(
            "tests/conformance/corpus_surface.json",
            "tests/tooling/fixtures/external_validation/source_surface.json",
            "tests/tooling/fixtures/public_conformance_reporting/stability_policy.json",
        ),
    ),
    "check-public-conformance-schema-surface": PublicConformanceActionFragment(
        action="check-public-conformance-schema-surface",
        summary="validate checked-in public-conformance scorecard and summary schemas",
        backend=_python_backend(PUBLIC_CONFORMANCE_SCHEMA_CHECK_SCRIPT),
        guarantee_owner=(
            "public conformance payloads stay bound to registered checked-in "
            "schema identities before they can be published"
        ),
        report_paths=(PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,),
        upstream_truth=(
            "schemas/objc3-conformance-dashboard-status-v1.schema.json",
            "schemas/objc3c-public-conformance-scorecard-v1.schema.json",
            "schemas/objc3c-public-conformance-summary-v1.schema.json",
        ),
    ),
    "build-public-conformance-scorecard": PublicConformanceActionFragment(
        action="build-public-conformance-scorecard",
        summary="derive the public-conformance badge and stability score from live upstream reports",
        backend=_python_backend(PUBLIC_CONFORMANCE_SCORECARD_SCRIPT),
        guarantee_owner=(
            "public stability language reflects live corpus, external-validation, "
            "schema-anchor, and fail-closed policy truth"
        ),
        report_paths=(PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,),
        upstream_truth=(
            "tmp/reports/conformance/corpus-integration-summary.json",
            PUBLIC_CONFORMANCE_PUBLIC_SUITE_SUMMARY,
            "tmp/reports/external-validation/integration-summary.json",
            "tmp/reports/external-validation/publication-summary.json",
        ),
    ),
    "publish-public-conformance-report": PublicConformanceActionFragment(
        action="publish-public-conformance-report",
        summary="publish public-conformance summary, badge, scorecard, and Markdown artifacts",
        backend=_python_backend(PUBLIC_CONFORMANCE_REPORT_SCRIPT),
        guarantee_owner=(
            "public artifacts preserve the exact scorecard truth without stale "
            "promotion claims"
        ),
        report_paths=(PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,),
        artifact_paths=PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS,
        upstream_truth=(
            PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
            PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
            PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
        ),
    ),
    "validate-public-conformance-reporting": PublicConformanceActionFragment(
        action="validate-public-conformance-reporting",
        summary="run the integrated public-conformance reporting workflow",
        backend="runner-internal public-reporting child actions",
        guarantee_owner=(
            "source, schema, scorecard, and publication owners run as one "
            "strict public-conformance workflow"
        ),
        report_paths=(
            PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
            PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
            PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
            PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,
        ),
        artifact_paths=PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS,
        upstream_truth=tuple(path for path, _ in PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS),
    ),
    "validate-public-conformance-reporting-integration": PublicConformanceActionFragment(
        action="validate-public-conformance-reporting-integration",
        summary="validate public-conformance workflow report and child artifact contracts",
        backend=_python_backend(PUBLIC_CONFORMANCE_INTEGRATION_SCRIPT),
        guarantee_owner=(
            "integrated public-conformance reporting stays coherent across child "
            "report contracts, published artifacts, and stability truth"
        ),
        report_paths=(PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY,),
        upstream_truth=tuple(path for path, _ in PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS),
    ),
    "validate-public-conformance-reporting-end-to-end": PublicConformanceActionFragment(
        action="validate-public-conformance-reporting-end-to-end",
        summary="validate public-conformance entrypoints, command-surface sync, and nightly wiring",
        backend=_python_backend(PUBLIC_CONFORMANCE_END_TO_END_SCRIPT),
        guarantee_owner=(
            "public-conformance reporting is reachable through the public workflow "
            "surface with the same strict action inventory"
        ),
        report_paths=(PUBLIC_CONFORMANCE_END_TO_END_SUMMARY,),
        upstream_truth=(PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY,),
    ),
}


def public_conformance_action_specs() -> dict[str, ActionSpec]:
    return {
        action: fragment.to_action_spec()
        for action, fragment in PUBLIC_CONFORMANCE_ACTION_FRAGMENTS.items()
    }
