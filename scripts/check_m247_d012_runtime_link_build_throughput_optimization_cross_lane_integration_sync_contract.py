#!/usr/bin/env python3
"""Fail-closed checker for M247-D012 runtime/link/build throughput optimization cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-d012-runtime-link-build-throughput-optimization-cross-lane-integration-sync-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_cross_lane_integration_sync_d012_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_d012_lane_d_readiness.py"
DEFAULT_D011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_d011_expectations.md"
)
DEFAULT_D011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_packet.md"
)
DEFAULT_D011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"
)
DEFAULT_D011_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"
)
DEFAULT_D011_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_d011_lane_d_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-D012/runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract_summary.json"
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
        "M247-D012-DOC-EXP-01",
        "# M247 Runtime/Link/Build Throughput Optimization Cross-Lane Integration Sync Expectations (D012)",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-cross-lane-integration-sync/m247-d012-v1`",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-03",
        "Issue `#6770` defines canonical lane-D cross-lane integration sync scope.",
    ),
    SnippetCheck("M247-D012-DOC-EXP-04", "Dependencies: `M247-D011`"),
    SnippetCheck(
        "M247-D012-DOC-EXP-05",
        "scripts/check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-06",
        "scripts/run_m247_d012_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-07",
        "code/spec anchors and milestone\noptimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-08",
        "Readiness chain order: `D011 readiness -> D012 checker -> D012 pytest`.",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-09",
        "tests/tooling/test_check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck(
        "M247-D012-DOC-EXP-10",
        "tmp/reports/m247/M247-D012/runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D012-DOC-PKT-01",
        "# M247-D012 Runtime/Link/Build Throughput Optimization Cross-Lane Integration Sync Packet",
    ),
    SnippetCheck("M247-D012-DOC-PKT-02", "Packet: `M247-D012`"),
    SnippetCheck("M247-D012-DOC-PKT-03", "Issue: `#6770`"),
    SnippetCheck("M247-D012-DOC-PKT-04", "Dependencies: `M247-D011`"),
    SnippetCheck("M247-D012-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck("M247-D012-DOC-PKT-06", "Dependency anchors from `M247-D011`:"),
    SnippetCheck(
        "M247-D012-DOC-PKT-07",
        "scripts/run_m247_d012_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M247-D012-DOC-PKT-08",
        "python -m pytest tests/tooling/test_check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py -q",
    ),
    SnippetCheck(
        "M247-D012-DOC-PKT-09",
        "`D011 readiness -> D012 checker -> D012 pytest`",
    ),
    SnippetCheck(
        "M247-D012-DOC-PKT-10",
        "tmp/reports/m247/M247-D012/runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract_summary.json",
    ),
)

D011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D012-D011-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-performance-and-quality-guardrails/m247-d011-v1`",
    ),
    SnippetCheck(
        "M247-D012-D011-DOC-02",
        "Dependencies: `M247-D010`",
    ),
    SnippetCheck(
        "M247-D012-D011-DOC-03",
        "Issue `#6769` defines canonical lane-D performance and quality guardrails scope.",
    ),
)

D011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D012-D011-PKT-01", "Packet: `M247-D011`"),
    SnippetCheck("M247-D012-D011-PKT-02", "Issue: `#6769`"),
    SnippetCheck("M247-D012-D011-PKT-03", "Dependencies: `M247-D010`"),
    SnippetCheck("M247-D012-D011-PKT-04", "Freeze date: `2026-03-04`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D012-RDY-01", 'DEPENDENCY_TOKEN = "M247-D011"'),
    SnippetCheck(
        "M247-D012-RDY-02",
        "scripts/run_m247_d011_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M247-D012-RDY-03",
        "scripts/check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck(
        "M247-D012-RDY-04",
        "tests/tooling/test_check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck("M247-D012-RDY-05", "[ok] M247-D012 lane-D readiness chain completed"),
)

D011_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D012-D011-RDY-01", 'DEPENDENCY_TOKEN = "M247-D010"'),
    SnippetCheck(
        "M247-D012-D011-RDY-02",
        "scripts/run_m247_d010_lane_d_readiness.py",
    ),
    SnippetCheck("M247-D012-D011-RDY-03", "[ok] M247-D011 lane-D readiness chain completed"),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
    parser.add_argument("--d011-expectations-doc", type=Path, default=DEFAULT_D011_EXPECTATIONS_DOC)
    parser.add_argument("--d011-packet-doc", type=Path, default=DEFAULT_D011_PACKET_DOC)
    parser.add_argument("--d011-checker", type=Path, default=DEFAULT_D011_CHECKER)
    parser.add_argument("--d011-test", type=Path, default=DEFAULT_D011_TEST)
    parser.add_argument("--d011-readiness-script", type=Path, default=DEFAULT_D011_READINESS_SCRIPT)
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
        (args.expectations_doc, "M247-D012-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-D012-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d011_expectations_doc, "M247-D012-D011-DOC-EXISTS", D011_EXPECTATIONS_SNIPPETS),
        (args.d011_packet_doc, "M247-D012-D011-PKT-EXISTS", D011_PACKET_SNIPPETS),
        (args.readiness_script, "M247-D012-RDY-EXISTS", READINESS_SNIPPETS),
        (args.d011_readiness_script, "M247-D012-D011-RDY-EXISTS", D011_READINESS_SNIPPETS),
    ):
        count, findings = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d011_checker, "M247-D012-DEP-D011-ARG-01"),
        (args.d011_test, "M247-D012-DEP-D011-ARG-02"),
    ):
        count, findings = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(findings)

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
            f"[M247-D012-SUMMARY-WRITE-01] {display_path(summary_path)}: {summary_error}",
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

