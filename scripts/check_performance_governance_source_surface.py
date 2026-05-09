#!/usr/bin/env python3
"""Validate the checked-in performance-governance source surface."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance-governance" / "source-surface-summary.json"
SOURCE_SURFACE_CONTRACT_ID = "objc3c.performance.governance.source.surface.v1"
SOURCE_SURFACE_KIND = "publishable-performance-report-source-surface"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.source.surface.summary.v1"
EXPECTED_RUNBOOK = "docs/runbooks/objc3c_performance_governance.md"

EXPECTED_REQUIRED_PATHS = {
    "runbook": EXPECTED_RUNBOOK,
    "budget_model": "tests/tooling/fixtures/performance_governance/budget_model.json",
    "claim_policy": "tests/tooling/fixtures/performance_governance/claim_policy.json",
    "breach_triage_policy": "tests/tooling/fixtures/performance_governance/breach_triage_policy.json",
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
    "no benchmark source outside the existing performance, compiler-throughput, and runtime-performance surfaces",
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


def fail(message: str) -> int:
    print(f"performance-governance-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def require_exact_path(source_surface: dict[str, object], field_name: str) -> str | None:
    expected_path = EXPECTED_REQUIRED_PATHS[field_name]
    if source_surface.get(field_name) != expected_path:
        fail(f"{field_name} drifted from required path {expected_path}")
        return None
    return expected_path


def require_path(relative_path: str, *, kind: str) -> bool:
    path = ROOT / relative_path
    if not path.exists():
        fail(f"missing {kind}: {relative_path}")
        return False
    return True


def require_exact_list(
    source_surface: dict[str, object],
    field_name: str,
    expected_items: tuple[str, ...],
) -> tuple[str, ...] | None:
    if source_surface.get(field_name) != list(expected_items):
        fail(f"{field_name} drifted from required entries")
        return None
    return expected_items


def require_exact_owner_split(source_surface: dict[str, object]) -> dict[str, list[str]] | None:
    if source_surface.get("owner_split") != EXPECTED_OWNER_SPLIT:
        fail("owner_split drifted from required performance ownership boundaries")
        return None
    return EXPECTED_OWNER_SPLIT


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface contract: {repo_rel(SOURCE_SURFACE)}")

    surface = load_json(SOURCE_SURFACE)
    if surface.get("contract_id") != SOURCE_SURFACE_CONTRACT_ID:
        return fail("contract_id drifted")
    if surface.get("surface_kind") != SOURCE_SURFACE_KIND:
        return fail("surface_kind drifted")

    checked_paths = [repo_rel(SOURCE_SURFACE)]
    for field_name in EXPECTED_REQUIRED_PATHS:
        relative_path = require_exact_path(surface, field_name)
        if relative_path is None:
            return 1
        if not require_path(relative_path, kind=field_name):
            return 1
        checked_paths.append(relative_path)

    checked_in_sources = require_exact_list(
        surface,
        "checked_in_sources",
        EXPECTED_CHECKED_IN_SOURCES,
    )
    build_scripts = require_exact_list(surface, "build_scripts", EXPECTED_BUILD_SCRIPTS)
    upstream_reports = require_exact_list(surface, "upstream_reports", EXPECTED_UPSTREAM_REPORTS)
    machine_owned_output_roots = require_exact_list(
        surface,
        "machine_owned_output_roots",
        EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS,
    )
    owner_split = require_exact_owner_split(surface)
    explicit_non_goals = require_exact_list(
        surface,
        "explicit_non_goals",
        EXPECTED_EXPLICIT_NON_GOALS,
    )
    if (
        checked_in_sources is None
        or build_scripts is None
        or upstream_reports is None
        or machine_owned_output_roots is None
        or owner_split is None
        or explicit_non_goals is None
    ):
        return 1

    for list_name, items in (
        ("checked_in_sources", checked_in_sources),
        ("build_scripts", build_scripts),
        ("checked_in_roots", EXPECTED_CHECKED_IN_ROOTS),
    ):
        for relative_path in items:
            if not require_path(relative_path, kind=list_name):
                return 1
            checked_paths.append(relative_path)

    for owner_name, owner_paths in owner_split.items():
        for relative_path in owner_paths:
            if not require_path(relative_path, kind=f"{owner_name} owner_split"):
                return 1
            checked_paths.append(relative_path)

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "runbook": EXPECTED_RUNBOOK,
        "budget_model": EXPECTED_REQUIRED_PATHS["budget_model"],
        "claim_policy": EXPECTED_REQUIRED_PATHS["claim_policy"],
        "breach_triage_policy": EXPECTED_REQUIRED_PATHS["breach_triage_policy"],
        "lab_policy": EXPECTED_REQUIRED_PATHS["lab_policy"],
        "waiver_registry": EXPECTED_REQUIRED_PATHS["waiver_registry"],
        "workflow_surface": EXPECTED_REQUIRED_PATHS["workflow_surface"],
        "schema_surface": EXPECTED_REQUIRED_PATHS["schema_surface"],
        "checked_in_sources": list(checked_in_sources),
        "checked_in_roots": list(EXPECTED_CHECKED_IN_ROOTS),
        "owner_split": owner_split,
        "build_scripts": list(build_scripts),
        "upstream_reports": list(upstream_reports),
        "machine_owned_output_roots": list(machine_owned_output_roots),
        "explicit_non_goals": list(explicit_non_goals),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("performance-governance-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
