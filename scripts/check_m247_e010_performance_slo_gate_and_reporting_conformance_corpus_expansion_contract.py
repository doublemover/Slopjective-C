#!/usr/bin/env python3
"""Fail-closed checker for M247-E010 performance SLO conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e010-performance-slo-gate-reporting-conformance-corpus-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_conformance_corpus_expansion_e010_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e010_performance_slo_gate_and_reporting_conformance_corpus_expansion_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e010_lane_e_readiness.py"

DEFAULT_E009_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_conformance_matrix_implementation_e009_expectations.md"
)
DEFAULT_E009_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e009_performance_slo_gate_and_reporting_conformance_matrix_implementation_packet.md"
)
DEFAULT_E009_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_e009_performance_slo_gate_and_reporting_conformance_matrix_implementation_contract.py"
)
DEFAULT_E009_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_e009_performance_slo_gate_and_reporting_conformance_matrix_implementation_contract.py"
)
DEFAULT_E009_RUNNER = ROOT / "scripts" / "run_m247_e009_lane_e_readiness.py"

DEFAULT_D010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_conformance_corpus_expansion_d010_expectations.md"
)
DEFAULT_D010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_packet.md"
)
DEFAULT_D010_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py"
)
DEFAULT_D010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py"
)
DEFAULT_D010_RUNNER = ROOT / "scripts" / "run_m247_d010_lane_d_readiness.py"

DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E010/performance_slo_gate_and_reporting_conformance_corpus_expansion_contract_summary.json"
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
        "M247-E010-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Conformance Corpus Expansion Expectations (E010)",
    ),
    SnippetCheck(
        "M247-E010-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-conformance-corpus-expansion-contract/m247-e010-v1`",
    ),
    SnippetCheck(
        "M247-E010-DOC-EXP-03",
        "Dependencies: `M247-E009`, `M247-A010`, `M247-B010`, `M247-C010`, `M247-D010`",
    ),
    SnippetCheck("M247-E010-DOC-EXP-04", "Issue `#6781` defines canonical lane-E"),
    SnippetCheck("M247-E010-DOC-EXP-05", "`python scripts/run_m247_e009_lane_e_readiness.py`"),
    SnippetCheck("M247-E010-DOC-EXP-06", "`check:objc3c:m247-d010-lane-d-readiness`"),
    SnippetCheck(
        "M247-E010-DOC-EXP-07",
        "`tmp/reports/m247/M247-E010/performance_slo_gate_and_reporting_conformance_corpus_expansion_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E010-DOC-PKT-01",
        "# M247-E010 Performance SLO Gate and Reporting Conformance Corpus Expansion Packet",
    ),
    SnippetCheck("M247-E010-DOC-PKT-02", "Packet: `M247-E010`"),
    SnippetCheck("M247-E010-DOC-PKT-03", "Issue: `#6781`"),
    SnippetCheck(
        "M247-E010-DOC-PKT-04",
        "Dependencies: `M247-E009`, `M247-A010`, `M247-B010`, `M247-C010`, `M247-D010`",
    ),
    SnippetCheck(
        "M247-E010-DOC-PKT-05",
        "scripts/check_m247_e010_performance_slo_gate_and_reporting_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M247-E010-DOC-PKT-06",
        "scripts/run_m247_e010_lane_e_readiness.py",
    ),
)

E009_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E010-E009-DOC-01",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-conformance-matrix-implementation-contract/m247-e009-v1`",
    ),
)

E009_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E010-E009-PKT-01", "Packet: `M247-E009`"),
    SnippetCheck("M247-E010-E009-PKT-02", "Issue: `#6780`"),
)

D010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E010-D010-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-conformance-corpus-expansion/m247-d010-v1`",
    ),
)

D010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E010-D010-PKT-01", "Packet: `M247-D010`"),
    SnippetCheck("M247-E010-D010-PKT-02", "Issue: `#6768`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E010-RUN-01",
        '"""Run M247-E010 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-E010-RUN-02", 'BASELINE_DEPENDENCIES = ("M247-E009",)'),
    SnippetCheck("M247-E010-RUN-03", 'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A010", "M247-B010", "M247-C010", "M247-D010")'),
    SnippetCheck("M247-E010-RUN-04", "scripts/run_m247_e009_lane_e_readiness.py"),
    SnippetCheck("M247-E010-RUN-05", "check:objc3c:m247-a010-lane-a-readiness"),
    SnippetCheck("M247-E010-RUN-06", "check:objc3c:m247-b010-lane-b-readiness"),
    SnippetCheck("M247-E010-RUN-07", "check:objc3c:m247-c010-lane-c-readiness"),
    SnippetCheck("M247-E010-RUN-08", "check:objc3c:m247-d010-lane-d-readiness"),
    SnippetCheck("M247-E010-RUN-09", "[ok] M247-E010 lane-E readiness chain completed"),
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
    parser.add_argument("--e009-expectations-doc", type=Path, default=DEFAULT_E009_EXPECTATIONS_DOC)
    parser.add_argument("--e009-packet-doc", type=Path, default=DEFAULT_E009_PACKET_DOC)
    parser.add_argument("--d010-expectations-doc", type=Path, default=DEFAULT_D010_EXPECTATIONS_DOC)
    parser.add_argument("--d010-packet-doc", type=Path, default=DEFAULT_D010_PACKET_DOC)
    parser.add_argument("--e009-checker", type=Path, default=DEFAULT_E009_CHECKER)
    parser.add_argument("--e009-test", type=Path, default=DEFAULT_E009_TEST)
    parser.add_argument("--e009-runner", type=Path, default=DEFAULT_E009_RUNNER)
    parser.add_argument("--d010-checker", type=Path, default=DEFAULT_D010_CHECKER)
    parser.add_argument("--d010-test", type=Path, default=DEFAULT_D010_TEST)
    parser.add_argument("--d010-runner", type=Path, default=DEFAULT_D010_RUNNER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
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
        (args.expectations_doc, "M247-E010-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E010-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e009_expectations_doc, "M247-E010-E009-DOC-EXISTS", E009_EXPECTATIONS_SNIPPETS),
        (args.e009_packet_doc, "M247-E010-E009-PKT-EXISTS", E009_PACKET_SNIPPETS),
        (args.d010_expectations_doc, "M247-E010-D010-DOC-EXISTS", D010_EXPECTATIONS_SNIPPETS),
        (args.d010_packet_doc, "M247-E010-D010-PKT-EXISTS", D010_PACKET_SNIPPETS),
        (args.readiness_script, "M247-E010-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e009_checker, "M247-E010-DEP-E009-ARG-01"),
        (args.e009_test, "M247-E010-DEP-E009-ARG-02"),
        (args.e009_runner, "M247-E010-DEP-E009-ARG-03"),
        (args.d010_checker, "M247-E010-DEP-D010-ARG-01"),
        (args.d010_test, "M247-E010-DEP-D010-ARG-02"),
        (args.d010_runner, "M247-E010-DEP-D010-ARG-03"),
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

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if args.emit_json:
        return 0

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))




