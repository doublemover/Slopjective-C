#!/usr/bin/env python3
"""Fail-closed contract checker for M247-B012 semantic hot-path cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-b012-semantic-hot-path-analysis-budgeting-cross-lane-integration-sync-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_b012_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_packet.md"
)
DEFAULT_B011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_b011_expectations.md"
)
DEFAULT_B011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b011_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_packet.md"
)
DEFAULT_B011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b011_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_contract.py"
)
DEFAULT_B011_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b011_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_contract.py"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b012_lane_b_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-B012/semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-DOC-EXP-01",
        "# M247 Semantic Hot-Path Analysis and Budgeting Cross-Lane Integration Sync Expectations (B012)",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-cross-lane-integration-sync/m247-b012-v1`",
    ),
    SnippetCheck("M247-B012-DOC-EXP-03", "Dependencies: `M247-B011`"),
    SnippetCheck(
        "M247-B012-DOC-EXP-04",
        "Issue `#6735` defines canonical lane-B cross-lane integration synchronization scope.",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-05",
        "cross-lane integration synchronization governance",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-06",
        "cross-lane integration synchronization continuity",
    ),
    SnippetCheck("M247-B012-DOC-EXP-07", "Performance profiling and compile-time budgets."),
    SnippetCheck(
        "M247-B012-DOC-EXP-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-09",
        "`scripts/check_m247_b011_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-10",
        "`spec/planning/compiler/m247/m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_packet.md`",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-11",
        "`check:objc3c:m247-b012-lane-b-readiness`",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-12",
        "`scripts/run_m247_b012_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M247-B012-DOC-EXP-13",
        "`tmp/reports/m247/M247-B012/semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_summary.json`",
    ),
    SnippetCheck("M247-B012-DOC-EXP-14", "`compile:objc3c`"),
    SnippetCheck("M247-B012-DOC-EXP-15", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-DOC-PKT-01",
        "# M247-B012 Semantic Hot-Path Analysis and Budgeting Cross-Lane Integration Sync Packet",
    ),
    SnippetCheck("M247-B012-DOC-PKT-02", "Packet: `M247-B012`"),
    SnippetCheck("M247-B012-DOC-PKT-03", "Issue: `#6735`"),
    SnippetCheck("M247-B012-DOC-PKT-04", "Dependencies: `M247-B011`"),
    SnippetCheck(
        "M247-B012-DOC-PKT-05",
        "`scripts/check_m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M247-B012-DOC-PKT-06",
        "`tests/tooling/test_check_m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M247-B012-DOC-PKT-07",
        "`scripts/run_m247_b012_lane_b_readiness.py`",
    ),
    SnippetCheck("M247-B012-DOC-PKT-08", "`check:objc3c:m247-b012-lane-b-readiness`"),
    SnippetCheck(
        "M247-B012-DOC-PKT-09",
        "cross-lane integration synchronization continuity",
    ),
    SnippetCheck(
        "M247-B012-DOC-PKT-10",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-B012-DOC-PKT-11",
        "tmp/reports/m247/M247-B012/semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_summary.json",
    ),
)

B011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-B011-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-performance-and-quality-guardrails/m247-b011-v1`",
    ),
    SnippetCheck("M247-B012-B011-DOC-02", "Dependencies: `M247-B010`"),
    SnippetCheck(
        "M247-B012-B011-DOC-03",
        "scripts/check_m247_b011_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_contract.py",
    ),
)

B011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B012-B011-PKT-01", "Packet: `M247-B011`"),
    SnippetCheck("M247-B012-B011-PKT-02", "Issue: `#6734`"),
    SnippetCheck("M247-B012-B011-PKT-03", "Dependencies: `M247-B010`"),
    SnippetCheck(
        "M247-B012-B011-PKT-04",
        "scripts/check_m247_b011_semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M247-B012-B011-PKT-05",
        "tmp/reports/m247/M247-B011/semantic_hot_path_analysis_and_budgeting_performance_and_quality_guardrails_summary.json",
    ),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-RUN-01",
        '"""Run M247-B012 lane-B readiness checks without deep npm nesting."""',
    ),
    SnippetCheck(
        "M247-B012-RUN-02",
        "scripts/run_m247_b011_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M247-B012-RUN-03",
        "scripts/check_m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck(
        "M247-B012-RUN-04",
        "tests/tooling/test_check_m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck("M247-B012-RUN-05", "[ok] M247-B012 lane-B readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-ARCH-01",
        "M247 lane-B B012 semantic hot-path analysis/budgeting cross-lane integration sync anchors",
    ),
    SnippetCheck(
        "M247-B012-ARCH-02",
        "m247_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_b012_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-SPC-01",
        "semantic hot-path analysis/budgeting cross-lane integration sync wiring",
    ),
    SnippetCheck(
        "M247-B012-SPC-02",
        "lane-B dependency anchor (`M247-B011`) and fail closed",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-META-01",
        "deterministic lane-B semantic hot-path analysis/budgeting cross-lane integration metadata anchors for `M247-B012`",
    ),
    SnippetCheck("M247-B012-META-02", "`M247-B011` dependency continuity"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B012-PKG-01",
        '"check:objc3c:m247-b012-semantic-hot-path-analysis-and-budgeting-cross-lane-integration-sync-contract": '
        '"python scripts/check_m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_contract.py"',
    ),
    SnippetCheck(
        "M247-B012-PKG-02",
        '"test:tooling:m247-b012-semantic-hot-path-analysis-and-budgeting-cross-lane-integration-sync-contract": '
        '"python -m pytest tests/tooling/test_check_m247_b012_semantic_hot_path_analysis_and_budgeting_cross_lane_integration_sync_contract.py -q"',
    ),
    SnippetCheck(
        "M247-B012-PKG-03",
        '"check:objc3c:m247-b012-lane-b-readiness": "python scripts/run_m247_b012_lane_b_readiness.py"',
    ),
    SnippetCheck("M247-B012-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M247-B012-PKG-05", '"compile:objc3c": '),
    SnippetCheck("M247-B012-PKG-06", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--b011-expectations-doc", type=Path, default=DEFAULT_B011_EXPECTATIONS_DOC)
    parser.add_argument("--b011-packet-doc", type=Path, default=DEFAULT_B011_PACKET_DOC)
    parser.add_argument("--b011-checker", type=Path, default=DEFAULT_B011_CHECKER)
    parser.add_argument("--b011-test", type=Path, default=DEFAULT_B011_TEST)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-B012-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-B012-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b011_expectations_doc, "M247-B012-B011-DOC-EXISTS", B011_EXPECTATIONS_SNIPPETS),
        (args.b011_packet_doc, "M247-B012-B011-PKT-EXISTS", B011_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-B012-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M247-B012-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-B012-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-B012-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-B012-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b011_checker, "M247-B012-DEP-B011-ARG-01"),
        (args.b011_test, "M247-B012-DEP-B011-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
