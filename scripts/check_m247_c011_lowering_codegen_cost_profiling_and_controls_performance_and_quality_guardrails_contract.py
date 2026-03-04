#!/usr/bin/env python3
"""Fail-closed checker for M247-C011 lowering/codegen performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-c011-lowering-codegen-cost-profiling-controls-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_performance_and_quality_guardrails_c011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c011_lowering_codegen_cost_profiling_and_controls_performance_and_quality_guardrails_packet.md"
)
DEFAULT_C010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_conformance_corpus_expansion_c010_expectations.md"
)
DEFAULT_C010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c010_lowering_codegen_cost_profiling_and_controls_conformance_corpus_expansion_packet.md"
)
DEFAULT_C010_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_c010_lowering_codegen_cost_profiling_and_controls_conformance_corpus_expansion_contract.py"
)
DEFAULT_C010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_c010_lowering_codegen_cost_profiling_and_controls_conformance_corpus_expansion_contract.py"
)
DEFAULT_C010_READINESS_RUNNER = ROOT / "scripts" / "run_m247_c010_lane_c_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_c011_lane_c_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-C011/lowering_codegen_cost_profiling_and_controls_performance_and_quality_guardrails_contract_summary.json"
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
        "M247-C011-DOC-EXP-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-performance-and-quality-guardrails-contract/m247-c011-v1`",
    ),
    SnippetCheck("M247-C011-DOC-EXP-02", "Dependencies: `M247-C010`"),
    SnippetCheck(
        "M247-C011-DOC-EXP-03",
        "Issue `#6752` defines canonical lane-C performance and quality guardrails scope.",
    ),
    SnippetCheck(
        "M247-C011-DOC-EXP-04",
        "scripts/check_m247_c010_lowering_codegen_cost_profiling_and_controls_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M247-C011-DOC-EXP-05",
        "`C010 readiness -> C011 checker -> C011 pytest`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C011-DOC-PKT-01", "Packet: `M247-C011`"),
    SnippetCheck("M247-C011-DOC-PKT-02", "Issue: `#6752`"),
    SnippetCheck("M247-C011-DOC-PKT-03", "Dependencies: `M247-C010`"),
    SnippetCheck(
        "M247-C011-DOC-PKT-04",
        "scripts/check_m247_c011_lowering_codegen_cost_profiling_and_controls_performance_and_quality_guardrails_contract.py",
    ),
)

C010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C011-C010-DOC-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-conformance-corpus-expansion-contract/m247-c010-v1`",
    ),
    SnippetCheck("M247-C011-C010-DOC-02", "Dependencies: `M247-C009`"),
)

C010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C011-C010-PKT-01", "Packet: `M247-C010`"),
    SnippetCheck("M247-C011-C010-PKT-02", "Issue: `#6751`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C011-RUN-01",
        '"""Run M247-C011 lane-C readiness checks without deep npm nesting."""',
    ),
    SnippetCheck(
        "M247-C011-RUN-02",
        "scripts/run_m247_c010_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C011-RUN-03",
        "scripts/check_m247_c011_lowering_codegen_cost_profiling_and_controls_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M247-C011-RUN-04",
        "tests/tooling/test_check_m247_c011_lowering_codegen_cost_profiling_and_controls_performance_and_quality_guardrails_contract.py",
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
    parser.add_argument("--c010-expectations-doc", type=Path, default=DEFAULT_C010_EXPECTATIONS_DOC)
    parser.add_argument("--c010-packet-doc", type=Path, default=DEFAULT_C010_PACKET_DOC)
    parser.add_argument("--c010-checker", type=Path, default=DEFAULT_C010_CHECKER)
    parser.add_argument("--c010-test", type=Path, default=DEFAULT_C010_TEST)
    parser.add_argument("--c010-readiness-runner", type=Path, default=DEFAULT_C010_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]
) -> tuple[int, list[Finding]]:
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
        (args.expectations_doc, "M247-C011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-C011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c010_expectations_doc, "M247-C011-C010-DOC-EXISTS", C010_EXPECTATIONS_SNIPPETS),
        (args.c010_packet_doc, "M247-C011-C010-PKT-EXISTS", C010_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-C011-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.c010_checker, "M247-C011-DEP-C010-ARG-01"),
        (args.c010_test, "M247-C011-DEP-C010-ARG-02"),
        (args.c010_readiness_runner, "M247-C011-DEP-C010-ARG-03"),
    ):
        checks_total += 1
        if not path.exists() or not path.is_file():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is missing: {display_path(path)}",
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
