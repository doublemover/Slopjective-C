"""Static contract values for the performance-governance source surface."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE_SURFACE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "performance_governance"
    / "source_surface.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "performance-governance"
    / "source-surface-summary.json"
)
SOURCE_SURFACE_CONTRACT_ID = "objc3c.performance.governance.source.surface.v1"
SOURCE_SURFACE_KIND = "publishable-performance-report-source-surface"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.source.surface.summary.v1"
EXPECTED_RUNBOOK = "docs/runbooks/objc3c_performance_governance.md"

EXPECTED_REQUIRED_PATHS = {
    "runbook": EXPECTED_RUNBOOK,
    "budget_model": "tests/tooling/fixtures/performance_governance/budget_model.json",
    "claim_policy": "tests/tooling/fixtures/performance_governance/claim_policy.json",
    "breach_triage_policy": (
        "tests/tooling/fixtures/performance_governance/breach_triage_policy.json"
    ),
    "lab_policy": "tests/tooling/fixtures/performance_governance/lab_policy.json",
    "waiver_registry": "tests/tooling/fixtures/performance_governance/waivers.json",
    "workflow_surface": "tests/tooling/fixtures/performance_governance/workflow_surface.json",
    "schema_surface": "tests/tooling/fixtures/performance_governance/schema_surface.json",
}

EXPECTED_UPSTREAM_REPORTS = (
    "tmp/reports/performance/benchmark-summary.json",
    "tmp/reports/performance/comparative-baselines-summary.json",
    "tmp/reports/compiler-throughput/benchmark-summary.json",
    "tmp/reports/compiler-throughput/integration-summary.json",
    "tmp/reports/runtime-performance/benchmark-summary.json",
    "tmp/reports/runtime-performance/integration-summary.json",
)

EXPECTED_CHECKED_IN_SOURCES = (
    "docs/runbooks/objc3c_performance.md",
    "docs/runbooks/objc3c_compiler_throughput.md",
    "docs/runbooks/objc3c_runtime_performance.md",
    EXPECTED_RUNBOOK,
    "tests/tooling/fixtures/performance_governance/budget_model.json",
    "tests/tooling/fixtures/performance_governance/claim_policy.json",
    "tests/tooling/fixtures/performance_governance/breach_triage_policy.json",
    "tests/tooling/fixtures/performance_governance/lab_policy.json",
    "tests/tooling/fixtures/performance_governance/schema_surface.json",
    "tests/tooling/fixtures/performance_governance/workflow_surface.json",
    "tests/tooling/fixtures/performance_governance/waivers.json",
    "tests/tooling/fixtures/performance/benchmark_portfolio.json",
    "tests/tooling/fixtures/performance/comparative_baseline_manifest.json",
    "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
    "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
)

EXPECTED_BUILD_SCRIPTS = (
    "scripts/build_objc3c_performance_dashboard.py",
    "scripts/publish_objc3c_performance_report.py",
    "scripts/check_objc3c_performance_governance_integration.py",
    "scripts/check_objc3c_performance_governance_end_to_end.py",
)

EXPECTED_OWNER_SPLIT = {
    "performance_governance": [
        "tests/tooling/fixtures/performance_governance/budget_model.json",
        "tests/tooling/fixtures/performance_governance/claim_policy.json",
        "tests/tooling/fixtures/performance_governance/breach_triage_policy.json",
        "tests/tooling/fixtures/performance_governance/lab_policy.json",
        "tests/tooling/fixtures/performance_governance/waivers.json",
        "tests/tooling/fixtures/performance_governance/workflow_surface.json",
    ],
    "compiler_throughput": [
        "tests/tooling/fixtures/compiler_throughput/source_surface.json",
        "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
        "tests/tooling/fixtures/compiler_throughput/validation_tier_map.json",
        "tests/tooling/fixtures/compiler_throughput/artifact_surface.json",
        "schemas/objc3c-compiler-throughput-summary-v1.schema.json",
    ],
    "runtime_performance": [
        "tests/tooling/fixtures/runtime_performance/source_surface.json",
        "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
        "tests/tooling/fixtures/runtime_performance/artifact_surface.json",
        "schemas/objc3c-runtime-performance-telemetry-v1.schema.json",
    ],
    "public_performance_report": [
        "scripts/objc3c_performance_report/model.py",
        "scripts/objc3c_performance_report/publication.py",
        "scripts/objc3c_performance_report/rendering.py",
        "schemas/objc3c-performance-dashboard-summary-v1.schema.json",
        "schemas/objc3c-performance-public-report-v1.schema.json",
    ],
}

EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS = (
    "tmp/reports/performance-governance",
    "tmp/artifacts/performance-governance",
)

EXPECTED_EXPLICIT_NON_GOALS = (
    "no spreadsheet-only publication path",
    "no hand-edited performance badge or dashboard status",
    (
        "no benchmark source outside the existing performance, "
        "compiler-throughput, and runtime-performance surfaces"
    ),
)

EXPECTED_CHECKED_IN_ROOTS = (
    "docs/runbooks",
    "schemas",
    "scripts",
    "tests/tooling/fixtures/performance_governance",
    "tests/tooling/fixtures/performance",
    "tests/tooling/fixtures/compiler_throughput",
    "tests/tooling/fixtures/runtime_performance",
)
