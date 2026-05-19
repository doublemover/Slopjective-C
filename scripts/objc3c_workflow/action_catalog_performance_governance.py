"""Performance governance reporting action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PERFORMANCE_GOVERNANCE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-performance-governance-surface": ActionSpec(
        "check-performance-governance-surface",
        "validate the checked-in performance governance source surface",
        "python:scripts/check_performance_governance_source_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "performance governance only publishes from the checked-in benchmark, "
            "policy, and reporting sources"
        ),
    ),
    "check-performance-governance-schema-surface": ActionSpec(
        "check-performance-governance-schema-surface",
        "validate the checked-in performance governance schema surface",
        "python:scripts/check_performance_governance_schema_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "performance dashboard and public report artifacts stay on checked-in "
            "schema contracts"
        ),
    ),
    "build-performance-dashboard": ActionSpec(
        "build-performance-dashboard",
        "derive the live performance governance dashboard from checked-in budgets and live benchmark summaries",
        "python:scripts/build_objc3c_performance_dashboard.py",
        validation_tier="repo",
        guarantee_owner=(
            "performance governance dashboard derivation stays tied to the live benchmark, "
            "throughput, and runtime summaries"
        ),
    ),
    "publish-performance-report": ActionSpec(
        "publish-performance-report",
        "publish the live performance governance report artifacts and summary",
        "python:scripts/publish_objc3c_performance_report.py",
        validation_tier="repo",
        guarantee_owner=(
            "performance governance publication stays traceable to the checked-in "
            "policy contracts and live dashboard summary"
        ),
    ),
    "validate-performance-governance": ActionSpec(
        "validate-performance-governance",
        "run the integrated performance governance workflow",
        "runner-internal performance-governance child actions",
        validation_tier="repo",
        guarantee_owner=(
            "performance governance budgets, drift diagnostics, dashboard derivation, "
            "and report publication stay executable on the live performance surfaces"
        ),
    ),
    "validate-performance-governance-integration": ActionSpec(
        "validate-performance-governance-integration",
        "validate the integrated performance governance workflow report and child artifacts",
        "python:scripts/check_objc3c_performance_governance_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "integrated performance governance artifacts stay coherent across source, "
            "schema, dashboard, and publication outputs"
        ),
    ),
    "validate-performance-governance-end-to-end": ActionSpec(
        "validate-performance-governance-end-to-end",
        "validate performance governance entrypoints, command-surface sync, and ci/nightly wiring",
        "python:scripts/check_objc3c_performance_governance_end_to_end.py",
        validation_tier="repo",
        guarantee_owner=(
            "performance governance entrypoints and ci/nightly wiring stay coherent "
            "with the integrated reporting artifacts"
        ),
    ),
}
