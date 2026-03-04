#!/usr/bin/env python3
"""Fail-closed checker for M247-E008 performance SLO recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e008-performance-slo-gate-reporting-recovery-and-determinism-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_recovery_and_determinism_hardening_e008_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e008_performance_slo_gate_and_reporting_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e008_lane_e_readiness.py"

DEFAULT_E007_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_diagnostics_hardening_e007_expectations.md"
)
DEFAULT_E007_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_packet.md"
)
DEFAULT_E007_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py"
)
DEFAULT_E007_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py"
)
DEFAULT_E007_RUNNER = ROOT / "scripts" / "run_m247_e007_lane_e_readiness.py"

DEFAULT_D008_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_recovery_and_determinism_hardening_d008_expectations.md"
)
DEFAULT_D008_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d008_runtime_link_build_throughput_optimization_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_D008_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d008_runtime_link_build_throughput_optimization_recovery_and_determinism_hardening_contract.py"
)
DEFAULT_D008_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d008_runtime_link_build_throughput_optimization_recovery_and_determinism_hardening_contract.py"
)
DEFAULT_D008_RUNNER = ROOT / "scripts" / "run_m247_d008_lane_d_readiness.py"

DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E008/performance_slo_gate_and_reporting_recovery_and_determinism_hardening_contract_summary.json"
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
        "M247-E008-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Recovery and Determinism Hardening Expectations (E008)",
    ),
    SnippetCheck(
        "M247-E008-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-recovery-and-determinism-hardening-contract/m247-e008-v1`",
    ),
    SnippetCheck(
        "M247-E008-DOC-EXP-03",
        "Dependencies: `M247-E007`, `M247-A008`, `M247-B008`, `M247-C008`, `M247-D008`",
    ),
    SnippetCheck("M247-E008-DOC-EXP-04", "Issue `#6779` defines canonical lane-E"),
    SnippetCheck("M247-E008-DOC-EXP-05", "`check:objc3c:m247-e007-lane-e-readiness`"),
    SnippetCheck("M247-E008-DOC-EXP-06", "`check:objc3c:m247-d008-lane-d-readiness`"),
    SnippetCheck(
        "M247-E008-DOC-EXP-07",
        "`tmp/reports/m247/M247-E008/performance_slo_gate_and_reporting_recovery_and_determinism_hardening_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E008-DOC-PKT-01",
        "# M247-E008 Performance SLO Gate and Reporting Recovery and Determinism Hardening Packet",
    ),
    SnippetCheck("M247-E008-DOC-PKT-02", "Packet: `M247-E008`"),
    SnippetCheck("M247-E008-DOC-PKT-03", "Issue: `#6779`"),
    SnippetCheck(
        "M247-E008-DOC-PKT-04",
        "Dependencies: `M247-E007`, `M247-A008`, `M247-B008`, `M247-C008`, `M247-D008`",
    ),
    SnippetCheck(
        "M247-E008-DOC-PKT-05",
        "scripts/check_m247_e008_performance_slo_gate_and_reporting_recovery_and_determinism_hardening_contract.py",
    ),
    SnippetCheck(
        "M247-E008-DOC-PKT-06",
        "scripts/run_m247_e008_lane_e_readiness.py",
    ),
)

E007_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E008-E007-DOC-01",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-diagnostics-hardening-contract/m247-e007-v1`",
    ),
)

E007_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E008-E007-PKT-01", "Packet: `M247-E007`"),
    SnippetCheck("M247-E008-E007-PKT-02", "Issue: `#6778`"),
)

D008_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E008-D008-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-recovery-and-determinism-hardening/m247-d008-v1`",
    ),
)

D008_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E008-D008-PKT-01", "Packet: `M247-D008`"),
    SnippetCheck("M247-E008-D008-PKT-02", "Issue: `#6766`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E008-RUN-01",
        '"""Run M247-E008 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-E008-RUN-02", 'BASELINE_DEPENDENCIES = ("M247-E007",)'),
    SnippetCheck("M247-E008-RUN-03", 'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A008", "M247-B008", "M247-C008", "M247-D008")'),
    SnippetCheck("M247-E008-RUN-04", "check:objc3c:m247-e007-lane-e-readiness"),
    SnippetCheck("M247-E008-RUN-05", "check:objc3c:m247-a008-lane-a-readiness"),
    SnippetCheck("M247-E008-RUN-06", "check:objc3c:m247-b008-lane-b-readiness"),
    SnippetCheck("M247-E008-RUN-07", "check:objc3c:m247-c008-lane-c-readiness"),
    SnippetCheck("M247-E008-RUN-08", "check:objc3c:m247-d008-lane-d-readiness"),
    SnippetCheck("M247-E008-RUN-09", "[ok] M247-E008 lane-E readiness chain completed"),
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
    parser.add_argument("--e007-expectations-doc", type=Path, default=DEFAULT_E007_EXPECTATIONS_DOC)
    parser.add_argument("--e007-packet-doc", type=Path, default=DEFAULT_E007_PACKET_DOC)
    parser.add_argument("--d008-expectations-doc", type=Path, default=DEFAULT_D008_EXPECTATIONS_DOC)
    parser.add_argument("--d008-packet-doc", type=Path, default=DEFAULT_D008_PACKET_DOC)
    parser.add_argument("--e007-checker", type=Path, default=DEFAULT_E007_CHECKER)
    parser.add_argument("--e007-test", type=Path, default=DEFAULT_E007_TEST)
    parser.add_argument("--e007-runner", type=Path, default=DEFAULT_E007_RUNNER)
    parser.add_argument("--d008-checker", type=Path, default=DEFAULT_D008_CHECKER)
    parser.add_argument("--d008-test", type=Path, default=DEFAULT_D008_TEST)
    parser.add_argument("--d008-runner", type=Path, default=DEFAULT_D008_RUNNER)
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
        (args.expectations_doc, "M247-E008-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E008-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e007_expectations_doc, "M247-E008-E007-DOC-EXISTS", E007_EXPECTATIONS_SNIPPETS),
        (args.e007_packet_doc, "M247-E008-E007-PKT-EXISTS", E007_PACKET_SNIPPETS),
        (args.d008_expectations_doc, "M247-E008-D008-DOC-EXISTS", D008_EXPECTATIONS_SNIPPETS),
        (args.d008_packet_doc, "M247-E008-D008-PKT-EXISTS", D008_PACKET_SNIPPETS),
        (args.readiness_script, "M247-E008-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e007_checker, "M247-E008-DEP-E007-ARG-01"),
        (args.e007_test, "M247-E008-DEP-E007-ARG-02"),
        (args.e007_runner, "M247-E008-DEP-E007-ARG-03"),
        (args.d008_checker, "M247-E008-DEP-D008-ARG-01"),
        (args.d008_test, "M247-E008-DEP-D008-ARG-02"),
        (args.d008_runner, "M247-E008-DEP-D008-ARG-03"),
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
