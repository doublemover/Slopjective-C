#!/usr/bin/env python3
"""Fail-closed checker for M247-E013 lane-E performance SLO docs/runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e013-lane-e-performance-slo-gate-reporting-docs-operator-runbook-synchronization-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_e013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e013_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E013/lane_e_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M247-E013-A013-01",
        "M247-A013",
        Path("docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_a013_expectations.md"),
    ),
    AssetCheck(
        "M247-E013-A013-02",
        "M247-A013",
        Path("spec/planning/compiler/m247/m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M247-E013-A013-03",
        "M247-A013",
        Path("scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M247-E013-A013-04",
        "M247-A013",
        Path("tests/tooling/test_check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M247-E013-A013-05",
        "M247-A013",
        Path("scripts/run_m247_a013_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M247-E013-B015-01",
        "M247-B015",
        Path("docs/contracts/m247_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_b015_expectations.md"),
    ),
    AssetCheck(
        "M247-E013-B015-02",
        "M247-B015",
        Path("spec/planning/compiler/m247/m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_packet.md"),
    ),
    AssetCheck(
        "M247-E013-B015-03",
        "M247-B015",
        Path("scripts/check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py"),
    ),
    AssetCheck(
        "M247-E013-B015-04",
        "M247-B015",
        Path("tests/tooling/test_check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py"),
    ),
    AssetCheck(
        "M247-E013-B015-05",
        "M247-B015",
        Path("scripts/run_m247_b015_lane_b_readiness.py"),
    ),
    AssetCheck(
        "M247-E013-C014-01",
        "M247-C014",
        Path("docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_c014_expectations.md"),
    ),
    AssetCheck(
        "M247-E013-C014-02",
        "M247-C014",
        Path("spec/planning/compiler/m247/m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_packet.md"),
    ),
    AssetCheck(
        "M247-E013-C014-03",
        "M247-C014",
        Path("scripts/check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py"),
    ),
    AssetCheck(
        "M247-E013-C014-04",
        "M247-C014",
        Path("tests/tooling/test_check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py"),
    ),
    AssetCheck(
        "M247-E013-C014-05",
        "M247-C014",
        Path("scripts/run_m247_c014_lane_c_readiness.py"),
    ),
    AssetCheck(
        "M247-E013-D011-01",
        "M247-D011",
        Path("docs/contracts/m247_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_d011_expectations.md"),
    ),
    AssetCheck(
        "M247-E013-D011-02",
        "M247-D011",
        Path("spec/planning/compiler/m247/m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_packet.md"),
    ),
    AssetCheck(
        "M247-E013-D011-03",
        "M247-D011",
        Path("scripts/check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M247-E013-D011-04",
        "M247-D011",
        Path("tests/tooling/test_check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M247-E013-D011-05",
        "M247-D011",
        Path("scripts/run_m247_d011_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E013-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Docs and Operator Runbook Synchronization Expectations (E013)",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-docs-operator-runbook-synchronization-contract/m247-e013-v1`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-03",
        "Dependencies: `M247-E012`, `M247-A013`, `M247-B015`, `M247-C014`, `M247-D011`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-04",
        "Issue `#6784` defines canonical lane-E docs and operator runbook",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-05",
        "`check:objc3c:m247-e012-lane-e-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-06",
        "`check:objc3c:m247-a013-lane-a-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-07",
        "`check:objc3c:m247-b015-lane-b-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-08",
        "`check:objc3c:m247-c014-lane-c-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-09",
        "`check:objc3c:m247-d011-lane-d-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-10",
        "`check:objc3c:m247-e013-performance-slo-gate-reporting-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-11",
        "`test:tooling:m247-e013-performance-slo-gate-reporting-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M247-E013-DOC-EXP-12",
        "`tmp/reports/m247/M247-E013/lane_e_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E013-DOC-PKT-01", "Packet: `M247-E013`"),
    SnippetCheck("M247-E013-DOC-PKT-02", "Issue: `#6784`"),
    SnippetCheck(
        "M247-E013-DOC-PKT-03",
        "Dependencies: `M247-E012`, `M247-A013`, `M247-B015`, `M247-C014`, `M247-D011`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-04",
        "`scripts/check_m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-05",
        "`tests/tooling/test_check_m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-06",
        "`check:objc3c:m247-e012-lane-e-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-07",
        "`check:objc3c:m247-a013-lane-a-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-08",
        "`check:objc3c:m247-b015-lane-b-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-09",
        "`check:objc3c:m247-c014-lane-c-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-10",
        "`check:objc3c:m247-d011-lane-d-readiness`",
    ),
    SnippetCheck(
        "M247-E013-DOC-PKT-11",
        "`tmp/reports/m247/M247-E013/lane_e_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_summary.json`",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E013-RUN-01",
        '"""Run M247-E013 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-E013-RUN-02", 'BASELINE_DEPENDENCIES = ("M247-E012",)'),
    SnippetCheck(
        "M247-E013-RUN-03",
        'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A013", "M247-B015", "M247-C014", "M247-D011")',
    ),
    SnippetCheck("M247-E013-RUN-04", "check:objc3c:m247-e012-lane-e-readiness"),
    SnippetCheck("M247-E013-RUN-05", "check:objc3c:m247-a013-lane-a-readiness"),
    SnippetCheck("M247-E013-RUN-06", "check:objc3c:m247-b015-lane-b-readiness"),
    SnippetCheck("M247-E013-RUN-07", "check:objc3c:m247-c014-lane-c-readiness"),
    SnippetCheck("M247-E013-RUN-08", "check:objc3c:m247-d011-lane-d-readiness"),
    SnippetCheck(
        "M247-E013-RUN-09",
        "scripts/check_m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M247-E013-RUN-10",
        "tests/tooling/test_check_m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck("M247-E013-RUN-11", "[ok] M247-E013 lane-E readiness chain completed"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E013-PKG-01",
        '"check:objc3c:m247-e013-performance-slo-gate-reporting-docs-operator-runbook-synchronization-contract": '
        '"python scripts/check_m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_contract.py"',
    ),
    SnippetCheck(
        "M247-E013-PKG-02",
        '"test:tooling:m247-e013-performance-slo-gate-reporting-docs-operator-runbook-synchronization-contract": '
        '"python -m pytest tests/tooling/test_check_m247_e013_performance_slo_gate_and_reporting_docs_and_operator_runbook_synchronization_contract.py -q"',
    ),
    SnippetCheck(
        "M247-E013-PKG-03",
        '"check:objc3c:m247-e013-lane-e-readiness": "python scripts/run_m247_e013_lane_e_readiness.py"',
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_text_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required text artifact is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required text artifact: {exc}",
            )
        )
        return checks_total, findings

    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-E013-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E013-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M247-E013-RUN-EXISTS", READINESS_SNIPPETS),
        (args.package_json, "M247-E013-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
