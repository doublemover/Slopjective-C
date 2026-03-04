#!/usr/bin/env python3
"""Fail-closed checker for M247-B006 semantic hot-path edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-b006-semantic-hot-path-analysis-and-budgeting-edge-case-expansion-and-robustness-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_b006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b006_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_B005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_edge_case_and_compatibility_completion_b005_expectations.md"
)
DEFAULT_B005_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b005_semantic_hot_path_analysis_and_budgeting_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_B005_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b005_semantic_hot_path_analysis_and_budgeting_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_B005_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b005_semantic_hot_path_analysis_and_budgeting_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_B005_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b005_lane_b_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b006_lane_b_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-B006/semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract_summary.json"
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
        "M247-B006-DOC-EXP-01",
        "# M247 Semantic Hot-Path Analysis and Budgeting Edge-Case Expansion and Robustness Expectations (B006)",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-edge-case-expansion-and-robustness/m247-b006-v1`",
    ),
    SnippetCheck("M247-B006-DOC-EXP-03", "Dependencies: `M247-B005`"),
    SnippetCheck(
        "M247-B006-DOC-EXP-04",
        "Issue `#6729` defines canonical lane-B edge-case expansion and robustness scope.",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-05",
        "scripts/check_m247_b005_semantic_hot_path_analysis_and_budgeting_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-06",
        "scripts/run_m247_b006_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-07",
        "Readiness chain order: `B005 readiness -> B006 checker -> B006 pytest`.",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-08",
        "Code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-09",
        "tests/tooling/test_check_m247_b006_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M247-B006-DOC-EXP-10",
        "tmp/reports/m247/M247-B006/semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B006-DOC-PKT-01",
        "# M247-B006 Semantic Hot-Path Analysis and Budgeting Edge-Case Expansion and Robustness Packet",
    ),
    SnippetCheck("M247-B006-DOC-PKT-02", "Packet: `M247-B006`"),
    SnippetCheck("M247-B006-DOC-PKT-03", "Issue: `#6729`"),
    SnippetCheck("M247-B006-DOC-PKT-04", "Dependencies: `M247-B005`"),
    SnippetCheck("M247-B006-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M247-B006-DOC-PKT-06",
        "`scripts/check_m247_b006_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M247-B006-DOC-PKT-07",
        "`tests/tooling/test_check_m247_b006_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M247-B006-DOC-PKT-08",
        "`scripts/run_m247_b006_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M247-B006-DOC-PKT-09",
        "`B005 readiness -> B006 checker -> B006 pytest`",
    ),
    SnippetCheck(
        "M247-B006-DOC-PKT-10",
        "tmp/reports/m247/M247-B006/semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract_summary.json",
    ),
)

B005_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B006-B005-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-edge-case-and-compatibility-completion/m247-b005-v1`",
    ),
    SnippetCheck("M247-B006-B005-DOC-02", "Dependencies: `M247-B004`"),
    SnippetCheck(
        "M247-B006-B005-DOC-03",
        "Issue `#6728` defines canonical lane-B edge-case and compatibility completion scope.",
    ),
)

B005_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B006-B005-PKT-01", "Packet: `M247-B005`"),
    SnippetCheck("M247-B006-B005-PKT-02", "Issue: `#6728`"),
    SnippetCheck("M247-B006-B005-PKT-03", "Dependencies: `M247-B004`"),
    SnippetCheck("M247-B006-B005-PKT-04", "Freeze date: `2026-03-04`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B006-RUN-01", 'DEPENDENCY_TOKEN = "M247-B005"'),
    SnippetCheck("M247-B006-RUN-02", "scripts/run_m247_b005_lane_b_readiness.py"),
    SnippetCheck("M247-B006-RUN-03", "[ok] M247-B006 lane-B readiness chain completed"),
)

B005_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B006-B005-RUN-01", 'DEPENDENCY_TOKEN = "M247-B004"'),
    SnippetCheck(
        "M247-B006-B005-RUN-02",
        "scripts/check_m247_b005_semantic_hot_path_analysis_and_budgeting_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck("M247-B006-B005-RUN-03", "[ok] M247-B005 lane-B readiness chain completed"),
)

PACKAGE_JSON_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B006-PKG-01",
        '"check:objc3c:m247-b006-semantic-hot-path-analysis-and-budgeting-edge-case-expansion-and-robustness-contract": '
        '"python scripts/check_m247_b006_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract.py"',
    ),
    SnippetCheck(
        "M247-B006-PKG-02",
        '"test:tooling:m247-b006-semantic-hot-path-analysis-and-budgeting-edge-case-expansion-and-robustness-contract": '
        '"python -m pytest tests/tooling/test_check_m247_b006_semantic_hot_path_analysis_and_budgeting_edge_case_expansion_and_robustness_contract.py -q"',
    ),
    SnippetCheck(
        "M247-B006-PKG-03",
        '"check:objc3c:m247-b006-lane-b-readiness": "python scripts/run_m247_b006_lane_b_readiness.py"',
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
    parser.add_argument("--b005-expectations-doc", type=Path, default=DEFAULT_B005_EXPECTATIONS_DOC)
    parser.add_argument("--b005-packet-doc", type=Path, default=DEFAULT_B005_PACKET_DOC)
    parser.add_argument("--b005-checker", type=Path, default=DEFAULT_B005_CHECKER)
    parser.add_argument("--b005-test", type=Path, default=DEFAULT_B005_TEST)
    parser.add_argument("--b005-readiness-runner", type=Path, default=DEFAULT_B005_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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
        (args.expectations_doc, "M247-B006-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-B006-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b005_expectations_doc, "M247-B006-B005-DOC-EXISTS", B005_EXPECTATIONS_SNIPPETS),
        (args.b005_packet_doc, "M247-B006-B005-PKT-EXISTS", B005_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-B006-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.b005_readiness_runner, "M247-B006-B005-RUN-EXISTS", B005_READINESS_SNIPPETS),
        (args.package_json, "M247-B006-PKG-EXISTS", PACKAGE_JSON_SNIPPETS),
    ):
        count, found = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(found)

    for path, check_id in (
        (args.b005_checker, "M247-B006-DEP-B005-ARG-01"),
        (args.b005_test, "M247-B006-DEP-B005-ARG-02"),
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
            f"[M247-B006-SUMMARY-WRITE-01] {display_path(summary_path)}: {error_message}",
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



