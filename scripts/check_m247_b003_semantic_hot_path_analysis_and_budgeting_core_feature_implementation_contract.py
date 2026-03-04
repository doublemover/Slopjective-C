#!/usr/bin/env python3
"""Fail-closed checker for M247-B003 semantic hot-path core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-b003-semantic-hot-path-analysis-and-budgeting-core-feature-implementation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_b003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b003_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_packet.md"
)
DEFAULT_B002_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_modular_split_scaffolding_b002_expectations.md"
)
DEFAULT_B002_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b002_semantic_hot_path_analysis_and_budgeting_modular_split_scaffolding_packet.md"
)
DEFAULT_B002_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b002_semantic_hot_path_analysis_and_budgeting_modular_split_scaffolding_contract.py"
)
DEFAULT_B002_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b002_semantic_hot_path_analysis_and_budgeting_modular_split_scaffolding_contract.py"
)
DEFAULT_B002_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b002_lane_b_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b003_lane_b_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-B003/semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract_summary.json"
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
        "M247-B003-DOC-EXP-01",
        "# M247 Semantic Hot-Path Analysis and Budgeting Core Feature Implementation Expectations (B003)",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-core-feature-implementation/m247-b003-v1`",
    ),
    SnippetCheck("M247-B003-DOC-EXP-03", "Dependencies: `M247-B002`"),
    SnippetCheck(
        "M247-B003-DOC-EXP-04",
        "Issue `#6726` defines canonical lane-B core feature implementation scope.",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-05",
        "scripts/check_m247_b002_semantic_hot_path_analysis_and_budgeting_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-06",
        "scripts/run_m247_b003_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-07",
        "Readiness chain order: `B002 readiness -> B003 checker -> B003 pytest`.",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-08",
        "Code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-09",
        "tests/tooling/test_check_m247_b003_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M247-B003-DOC-EXP-10",
        "tmp/reports/m247/M247-B003/semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B003-DOC-PKT-01",
        "# M247-B003 Semantic Hot-Path Analysis and Budgeting Core Feature Implementation Packet",
    ),
    SnippetCheck("M247-B003-DOC-PKT-02", "Packet: `M247-B003`"),
    SnippetCheck("M247-B003-DOC-PKT-03", "Issue: `#6726`"),
    SnippetCheck("M247-B003-DOC-PKT-04", "Dependencies: `M247-B002`"),
    SnippetCheck("M247-B003-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M247-B003-DOC-PKT-06",
        "`scripts/check_m247_b003_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract.py`",
    ),
    SnippetCheck(
        "M247-B003-DOC-PKT-07",
        "`tests/tooling/test_check_m247_b003_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract.py`",
    ),
    SnippetCheck(
        "M247-B003-DOC-PKT-08",
        "`scripts/run_m247_b003_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M247-B003-DOC-PKT-09",
        "`B002 readiness -> B003 checker -> B003 pytest`",
    ),
    SnippetCheck(
        "M247-B003-DOC-PKT-10",
        "tmp/reports/m247/M247-B003/semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract_summary.json",
    ),
)

B002_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B003-B002-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-modular-split-scaffolding/m247-b002-v1`",
    ),
    SnippetCheck("M247-B003-B002-DOC-02", "Dependencies: `M247-B001`"),
    SnippetCheck(
        "M247-B003-B002-DOC-03",
        "Issue `#6725` defines canonical lane-B modular split and scaffolding scope.",
    ),
)

B002_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B003-B002-PKT-01", "Packet: `M247-B002`"),
    SnippetCheck("M247-B003-B002-PKT-02", "Issue: `#6725`"),
    SnippetCheck("M247-B003-B002-PKT-03", "Dependencies: `M247-B001`"),
    SnippetCheck("M247-B003-B002-PKT-04", "Freeze date: `2026-03-04`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B003-RUN-01", 'DEPENDENCY_TOKEN = "M247-B002"'),
    SnippetCheck("M247-B003-RUN-02", "scripts/run_m247_b002_lane_b_readiness.py"),
    SnippetCheck(
        "M247-B003-RUN-03",
        "scripts/check_m247_b003_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M247-B003-RUN-04",
        "tests/tooling/test_check_m247_b003_semantic_hot_path_analysis_and_budgeting_core_feature_implementation_contract.py",
    ),
    SnippetCheck("M247-B003-RUN-05", "[ok] M247-B003 lane-B readiness chain completed"),
)

B002_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B003-B002-RUN-01", 'DEPENDENCY_TOKEN = "M247-B001"'),
    SnippetCheck(
        "M247-B003-B002-RUN-02",
        "scripts/check_m247_b002_semantic_hot_path_analysis_and_budgeting_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck("M247-B003-B002-RUN-03", "[ok] M247-B002 lane-B readiness chain completed"),
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
    parser.add_argument("--b002-expectations-doc", type=Path, default=DEFAULT_B002_EXPECTATIONS_DOC)
    parser.add_argument("--b002-packet-doc", type=Path, default=DEFAULT_B002_PACKET_DOC)
    parser.add_argument("--b002-checker", type=Path, default=DEFAULT_B002_CHECKER)
    parser.add_argument("--b002-test", type=Path, default=DEFAULT_B002_TEST)
    parser.add_argument("--b002-readiness-runner", type=Path, default=DEFAULT_B002_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_text_artifact(
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


def check_dependency_path(path: Path, check_id: str) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required dependency path is missing: {display_path(path)}",
            )
        )
    elif not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required dependency path is not a file: {display_path(path)}",
            )
        )
    return 1, findings


def write_summary(summary_path: Path, payload: object) -> tuple[bool, str]:
    try:
        summary_path.parent.mkdir(parents=True, exist_ok=True)
        summary_path.write_text(canonical_json(payload), encoding="utf-8")
    except OSError as exc:
        return False, f"unable to write summary file: {exc}"
    return True, ""


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-B003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-B003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b002_expectations_doc, "M247-B003-B002-DOC-EXISTS", B002_EXPECTATIONS_SNIPPETS),
        (args.b002_packet_doc, "M247-B003-B002-PKT-EXISTS", B002_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-B003-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.b002_readiness_runner, "M247-B003-B002-RUN-EXISTS", B002_READINESS_SNIPPETS),
    ):
        count, found = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(found)

    for path, check_id in (
        (args.b002_checker, "M247-B003-DEP-B002-ARG-01"),
        (args.b002_test, "M247-B003-DEP-B002-ARG-02"),
    ):
        count, found = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(found)

    failures = sorted(failures, key=lambda finding: (finding.artifact, finding.check_id, finding.detail))
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

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    wrote_summary, error_message = write_summary(summary_path, summary_payload)
    if not wrote_summary:
        print(
            f"[M247-B003-SUMMARY-WRITE-01] {display_path(summary_path)}: {error_message}",
            file=sys.stderr,
        )
        return 1

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))


