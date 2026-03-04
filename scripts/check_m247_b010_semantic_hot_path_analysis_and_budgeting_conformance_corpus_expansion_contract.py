#!/usr/bin/env python3
"""Fail-closed contract checker for M247-B010 semantic hot-path conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-b010-semantic-hot-path-analysis-budgeting-conformance-corpus-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_b010_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_packet.md"
)
DEFAULT_B009_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_b009_expectations.md"
)
DEFAULT_B009_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b009_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_packet.md"
)
DEFAULT_B009_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b009_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_contract.py"
)
DEFAULT_B009_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b009_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_contract.py"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b010_lane_b_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-B010/semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_summary.json"
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
        "M247-B010-DOC-EXP-01",
        "# M247 Semantic Hot-Path Analysis and Budgeting Conformance Corpus Expansion Expectations (B010)",
    ),
    SnippetCheck(
        "M247-B010-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-conformance-corpus-expansion/m247-b010-v1`",
    ),
    SnippetCheck("M247-B010-DOC-EXP-03", "Dependencies: `M247-B009`"),
    SnippetCheck(
        "M247-B010-DOC-EXP-04",
        "Issue `#6733` defines canonical lane-B conformance corpus expansion scope.",
    ),
    SnippetCheck("M247-B010-DOC-EXP-05", "conformance corpus expansion governance"),
    SnippetCheck(
        "M247-B010-DOC-EXP-06",
        "conformance corpus case-accounting continuity",
    ),
    SnippetCheck("M247-B010-DOC-EXP-07", "Performance profiling and compile-time budgets."),
    SnippetCheck(
        "M247-B010-DOC-EXP-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-B010-DOC-EXP-09",
        "`scripts/check_m247_b009_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_contract.py`",
    ),
    SnippetCheck(
        "M247-B010-DOC-EXP-10",
        "`spec/planning/compiler/m247/m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_packet.md`",
    ),
    SnippetCheck(
        "M247-B010-DOC-EXP-11",
        "`check:objc3c:m247-b010-lane-b-readiness`",
    ),
    SnippetCheck(
        "M247-B010-DOC-EXP-12",
        "`scripts/run_m247_b010_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M247-B010-DOC-EXP-13",
        "`tmp/reports/m247/M247-B010/semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_summary.json`",
    ),
    SnippetCheck("M247-B010-DOC-EXP-14", "`compile:objc3c`"),
    SnippetCheck("M247-B010-DOC-EXP-15", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B010-DOC-PKT-01",
        "# M247-B010 Semantic Hot-Path Analysis and Budgeting Conformance Corpus Expansion Packet",
    ),
    SnippetCheck("M247-B010-DOC-PKT-02", "Packet: `M247-B010`"),
    SnippetCheck("M247-B010-DOC-PKT-03", "Issue: `#6733`"),
    SnippetCheck("M247-B010-DOC-PKT-04", "Dependencies: `M247-B009`"),
    SnippetCheck(
        "M247-B010-DOC-PKT-05",
        "`scripts/check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py`",
    ),
    SnippetCheck(
        "M247-B010-DOC-PKT-06",
        "`tests/tooling/test_check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py`",
    ),
    SnippetCheck(
        "M247-B010-DOC-PKT-07",
        "`scripts/run_m247_b010_lane_b_readiness.py`",
    ),
    SnippetCheck("M247-B010-DOC-PKT-08", "`check:objc3c:m247-b010-lane-b-readiness`"),
    SnippetCheck("M247-B010-DOC-PKT-09", "conformance corpus case-accounting continuity"),
    SnippetCheck(
        "M247-B010-DOC-PKT-10",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-B010-DOC-PKT-11",
        "tmp/reports/m247/M247-B010/semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_summary.json",
    ),
)

B009_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B010-B009-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-conformance-matrix-implementation/m247-b009-v1`",
    ),
    SnippetCheck("M247-B010-B009-DOC-02", "Dependencies: `M247-B008`"),
    SnippetCheck(
        "M247-B010-B009-DOC-03",
        "scripts/check_m247_b009_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_contract.py",
    ),
)

B009_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B010-B009-PKT-01", "Packet: `M247-B009`"),
    SnippetCheck("M247-B010-B009-PKT-02", "Issue: `#6732`"),
    SnippetCheck("M247-B010-B009-PKT-03", "Dependencies: `M247-B008`"),
    SnippetCheck(
        "M247-B010-B009-PKT-04",
        "scripts/check_m247_b009_semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_contract.py",
    ),
    SnippetCheck(
        "M247-B010-B009-PKT-05",
        "tmp/reports/m247/M247-B009/semantic_hot_path_analysis_and_budgeting_conformance_matrix_implementation_summary.json",
    ),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B010-RUN-01",
        '"""Run M247-B010 lane-B readiness checks without deep npm nesting."""',
    ),
    SnippetCheck(
        "M247-B010-RUN-02",
        "scripts/run_m247_b009_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M247-B010-RUN-03",
        "scripts/check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M247-B010-RUN-04",
        "tests/tooling/test_check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck("M247-B010-RUN-05", "[ok] M247-B010 lane-B readiness chain completed"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B010-PKG-01",
        '"check:objc3c:m247-b010-semantic-hot-path-analysis-and-budgeting-conformance-corpus-expansion-contract": '
        '"python scripts/check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py"',
    ),
    SnippetCheck(
        "M247-B010-PKG-02",
        '"test:tooling:m247-b010-semantic-hot-path-analysis-and-budgeting-conformance-corpus-expansion-contract": '
        '"python -m pytest tests/tooling/test_check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py -q"',
    ),
    SnippetCheck(
        "M247-B010-PKG-03",
        '"check:objc3c:m247-b010-lane-b-readiness": "python scripts/run_m247_b010_lane_b_readiness.py"',
    ),
    SnippetCheck("M247-B010-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M247-B010-PKG-05", '"compile:objc3c": '),
    SnippetCheck("M247-B010-PKG-06", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--b009-expectations-doc", type=Path, default=DEFAULT_B009_EXPECTATIONS_DOC)
    parser.add_argument("--b009-packet-doc", type=Path, default=DEFAULT_B009_PACKET_DOC)
    parser.add_argument("--b009-checker", type=Path, default=DEFAULT_B009_CHECKER)
    parser.add_argument("--b009-test", type=Path, default=DEFAULT_B009_TEST)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
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
        (args.expectations_doc, "M247-B010-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-B010-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b009_expectations_doc, "M247-B010-B009-DOC-EXISTS", B009_EXPECTATIONS_SNIPPETS),
        (args.b009_packet_doc, "M247-B010-B009-PKT-EXISTS", B009_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-B010-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.package_json, "M247-B010-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b009_checker, "M247-B010-DEP-B009-ARG-01"),
        (args.b009_test, "M247-B010-DEP-B009-ARG-02"),
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
