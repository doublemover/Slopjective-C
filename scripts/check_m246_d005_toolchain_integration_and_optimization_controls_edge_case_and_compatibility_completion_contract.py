#!/usr/bin/env python3
"""Fail-closed checker for M246-D005 toolchain integration/optimization controls edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-d005-toolchain-integration-optimization-controls-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_d005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d005_lane_d_readiness.py"
DEFAULT_D004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_core_feature_expansion_d004_expectations.md"
)
DEFAULT_D004_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_packet.md"
)
DEFAULT_D004_CHECKER = (
    ROOT / "scripts" / "check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py"
)
DEFAULT_D004_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py"
)
DEFAULT_D004_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d004_lane_d_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-D005/toolchain_integration_optimization_controls_edge_case_and_compatibility_completion_contract_summary.json"
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
        "M246-D005-DOC-EXP-01",
        "# M246 Toolchain Integration and Optimization Controls Edge-Case and Compatibility Completion Expectations (D005)",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-edge-case-and-compatibility-completion/m246-d005-v1`",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-03",
        "## Issue Anchor",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-04",
        "Canonical issue: `#5110`",
    ),
    SnippetCheck("M246-D005-DOC-EXP-05", "Dependencies: `M246-D004`"),
    SnippetCheck(
        "M246-D005-DOC-EXP-06",
        "Dependency chain integrity is mandatory: all `M246-D004` anchors must remain",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-07",
        "scripts/check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-08",
        "scripts/run_m246_d005_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-09",
        "code/spec anchors, and milestone optimization improvements as mandatory scope",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-10",
        "python scripts/check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py --emit-json --summary-out tmp/reports/m246/M246-D005/toolchain_integration_optimization_controls_edge_case_and_compatibility_completion_contract_summary.json",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-11",
        "`python scripts/run_m246_d005_lane_d_readiness.py`",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-12",
        "tests/tooling/test_check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-13",
        "tmp/reports/m246/M246-D005/toolchain_integration_optimization_controls_edge_case_and_compatibility_completion_contract_summary.json",
    ),
    SnippetCheck(
        "M246-D005-DOC-EXP-14",
        "Checker outputs must remain deterministically sorted, fail closed on document",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-D005-DOC-PKT-01",
        "# M246-D005 Toolchain Integration and Optimization Controls Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M246-D005-DOC-PKT-02", "Packet: `M246-D005`"),
    SnippetCheck("M246-D005-DOC-PKT-03", "Issue: `#5110`"),
    SnippetCheck("M246-D005-DOC-PKT-04", "Dependencies: `M246-D004`"),
    SnippetCheck("M246-D005-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck("M246-D005-DOC-PKT-06", "## Issue Anchor"),
    SnippetCheck(
        "M246-D005-DOC-PKT-07",
        "Canonical issue: `#5110`",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-08",
        "Dependency anchors from `M246-D004`:",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-09",
        "scripts/run_m246_d005_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-10",
        "python scripts/check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py --emit-json --summary-out tmp/reports/m246/M246-D005/toolchain_integration_optimization_controls_edge_case_and_compatibility_completion_contract_summary.json",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-11",
        "python -m pytest tests/tooling/test_check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py -q",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-12",
        "including dependency chain",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-13",
        "tmp/reports/m246/M246-D005/toolchain_integration_optimization_controls_edge_case_and_compatibility_completion_contract_summary.json",
    ),
    SnippetCheck(
        "M246-D005-DOC-PKT-14",
        "Contract checker failure reporting must remain deterministic/sorted, fail",
    ),
)

D004_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-D005-D004-DOC-01",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-core-feature-expansion/m246-d004-v1`",
    ),
    SnippetCheck(
        "M246-D005-D004-DOC-02",
        "Dependencies: `M246-D003`",
    ),
    SnippetCheck(
        "M246-D005-D004-DOC-03",
        "Issue `#5109` defines canonical lane-D core feature expansion scope.",
    ),
)

D004_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D005-D004-PKT-01", "Packet: `M246-D004`"),
    SnippetCheck("M246-D005-D004-PKT-02", "Issue: `#5109`"),
    SnippetCheck("M246-D005-D004-PKT-03", "Dependencies: `M246-D003`"),
    SnippetCheck("M246-D005-D004-PKT-04", "Freeze date: `2026-03-04`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D005-RDY-01", 'DEPENDENCY_TOKEN = "M246-D004"'),
    SnippetCheck(
        "M246-D005-RDY-02",
        "scripts/run_m246_d004_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D005-RDY-03",
        "scripts/check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M246-D005-RDY-04",
        "tests/tooling/test_check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck("M246-D005-RDY-05", "[ok] M246-D005 lane-D readiness chain completed"),
)

D004_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D005-D004-RDY-01", 'DEPENDENCY_TOKEN = "M246-D003"'),
    SnippetCheck(
        "M246-D005-D004-RDY-02",
        "scripts/run_m246_d003_lane_d_readiness.py",
    ),
    SnippetCheck("M246-D005-D004-RDY-03", "[ok] M246-D004 lane-D readiness chain completed"),
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
    parser.add_argument("--d004-expectations-doc", type=Path, default=DEFAULT_D004_EXPECTATIONS_DOC)
    parser.add_argument("--d004-packet-doc", type=Path, default=DEFAULT_D004_PACKET_DOC)
    parser.add_argument("--d004-checker", type=Path, default=DEFAULT_D004_CHECKER)
    parser.add_argument("--d004-test", type=Path, default=DEFAULT_D004_TEST)
    parser.add_argument("--d004-readiness-script", type=Path, default=DEFAULT_D004_READINESS_SCRIPT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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
    except (OSError, UnicodeError) as exc:
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


def finding_sort_key(failure: Finding) -> tuple[str, str, str]:
    return (failure.artifact, failure.check_id, failure.detail)


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
        (args.expectations_doc, "M246-D005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-D005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d004_expectations_doc, "M246-D005-D004-DOC-EXISTS", D004_EXPECTATIONS_SNIPPETS),
        (args.d004_packet_doc, "M246-D005-D004-PKT-EXISTS", D004_PACKET_SNIPPETS),
        (args.readiness_script, "M246-D005-RDY-EXISTS", READINESS_SNIPPETS),
        (args.d004_readiness_script, "M246-D005-D004-RDY-EXISTS", D004_READINESS_SNIPPETS),
    ):
        count, findings = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d004_checker, "M246-D005-DEP-D004-ARG-01"),
        (args.d004_test, "M246-D005-DEP-D004-ARG-02"),
    ):
        count, findings = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_ok, summary_error = write_summary(summary_path, summary_payload)
    if not summary_ok:
        print(
            f"[M246-D005-SUMMARY-WRITE-01] {display_path(summary_path)}: {summary_error}",
            file=sys.stderr,
        )
        return 1

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        if not args.emit_json:
            print(f"{MODE}: contract drift detected ({len(failures)} failed check(s)).", file=sys.stderr)
            for finding in failures:
                print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
