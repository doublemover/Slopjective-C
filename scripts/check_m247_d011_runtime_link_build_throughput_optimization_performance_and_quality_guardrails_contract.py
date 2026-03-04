#!/usr/bin/env python3
"""Fail-closed checker for M247-D011 runtime/link/build throughput optimization performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-d011-runtime-link-build-throughput-optimization-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_d011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_d011_lane_d_readiness.py"
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
DEFAULT_D010_READINESS = ROOT / "scripts" / "run_m247_d010_lane_d_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-D011/runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract_summary.json"
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
        "M247-D011-D010-01",
        "M247-D010",
        Path("docs/contracts/m247_runtime_link_build_throughput_optimization_conformance_corpus_expansion_d010_expectations.md"),
    ),
    AssetCheck(
        "M247-D011-D010-02",
        "M247-D010",
        Path("spec/planning/compiler/m247/m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M247-D011-D010-03",
        "M247-D010",
        Path("scripts/check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M247-D011-D010-04",
        "M247-D010",
        Path("tests/tooling/test_check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M247-D011-D010-05",
        "M247-D010",
        Path("scripts/run_m247_d010_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D011-DOC-EXP-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-performance-and-quality-guardrails/m247-d011-v1`",
    ),
    SnippetCheck("M247-D011-DOC-EXP-02", "Dependencies: `M247-D010`"),
    SnippetCheck("M247-D011-DOC-EXP-03", "Issue `#6769` defines canonical lane-D performance and quality guardrails scope."),
    SnippetCheck(
        "M247-D011-DOC-EXP-04",
        "scripts/check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck("M247-D011-DOC-EXP-05", "scripts/run_m247_d011_lane_d_readiness.py"),
    SnippetCheck("M247-D011-DOC-EXP-06", "`D010 readiness -> D011 checker -> D011 pytest`."),
    SnippetCheck(
        "M247-D011-DOC-EXP-07",
        "tmp/reports/m247/M247-D011/runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D011-DOC-PKT-01", "Packet: `M247-D011`"),
    SnippetCheck("M247-D011-DOC-PKT-02", "Issue: `#6769`"),
    SnippetCheck("M247-D011-DOC-PKT-03", "Dependencies: `M247-D010`"),
    SnippetCheck("M247-D011-DOC-PKT-04", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M247-D011-DOC-PKT-05",
        "`scripts/check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M247-D011-DOC-PKT-06",
        "`tests/tooling/test_check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py`",
    ),
)

D010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D011-D010-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-conformance-corpus-expansion/m247-d010-v1`",
    ),
    SnippetCheck("M247-D011-D010-DOC-02", "Dependencies: `M247-D009`"),
)

D010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D011-D010-PKT-01", "Packet: `M247-D010`"),
    SnippetCheck("M247-D011-D010-PKT-02", "Issue: `#6768`"),
    SnippetCheck("M247-D011-D010-PKT-03", "Dependencies: `M247-D009`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D011-RUN-01", 'DEPENDENCY_TOKEN = "M247-D010"'),
    SnippetCheck("M247-D011-RUN-02", "scripts/run_m247_d010_lane_d_readiness.py"),
    SnippetCheck(
        "M247-D011-RUN-03",
        "scripts/check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M247-D011-RUN-04",
        "tests/tooling/test_check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck("M247-D011-RUN-05", "[ok] M247-D011 lane-D readiness chain completed"),
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
    parser.add_argument("--d010-expectations-doc", type=Path, default=DEFAULT_D010_EXPECTATIONS_DOC)
    parser.add_argument("--d010-packet-doc", type=Path, default=DEFAULT_D010_PACKET_DOC)
    parser.add_argument("--d010-checker", type=Path, default=DEFAULT_D010_CHECKER)
    parser.add_argument("--d010-test", type=Path, default=DEFAULT_D010_TEST)
    parser.add_argument("--d010-readiness-script", type=Path, default=DEFAULT_D010_READINESS)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(*, artifact_name: str, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
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

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact_name,
                    check_id=snippet_check.check_id,
                    detail=f"missing required snippet: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    failures: list[Finding] = []

    count, findings = check_prerequisite_assets()
    checks_total += count
    failures.extend(findings)

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M247-D011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M247-D011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        ("d010_expectations_doc", args.d010_expectations_doc, "M247-D011-D010-DOC-EXISTS", D010_EXPECTATIONS_SNIPPETS),
        ("d010_packet_doc", args.d010_packet_doc, "M247-D011-D010-PKT-EXISTS", D010_PACKET_SNIPPETS),
        ("readiness_script", args.readiness_script, "M247-D011-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d010_checker, "M247-D011-D010-DEP-ARG-01"),
        (args.d010_test, "M247-D011-D010-DEP-ARG-02"),
        (args.d010_readiness_script, "M247-D011-D010-DEP-ARG-03"),
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
    summary = {
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
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
