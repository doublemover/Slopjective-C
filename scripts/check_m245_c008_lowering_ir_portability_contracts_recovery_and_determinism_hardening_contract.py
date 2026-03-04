#!/usr/bin/env python3
"""Fail-closed contract checker for M245-C008 lowering/IR portability recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-c008-lowering-ir-portability-contracts-recovery-and-determinism-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lowering_ir_portability_contracts_recovery_and_determinism_hardening_c008_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_C007_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lowering_ir_portability_contracts_diagnostics_hardening_c007_expectations.md"
)
DEFAULT_C007_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_packet.md"
)
DEFAULT_C007_CHECKER = (
    ROOT
    / "scripts"
    / "check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py"
)
DEFAULT_C007_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-C008/lowering_ir_portability_contracts_recovery_and_determinism_hardening_summary.json"
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
        "M245-C008-DOC-EXP-01",
        "# M245 Lowering/IR Portability Contracts Recovery and Determinism Hardening Expectations (C008)",
    ),
    SnippetCheck(
        "M245-C008-DOC-EXP-02",
        "Contract ID: `objc3c-lowering-ir-portability-contracts-recovery-and-determinism-hardening/m245-c008-v1`",
    ),
    SnippetCheck("M245-C008-DOC-EXP-03", "Dependencies: `M245-C007`"),
    SnippetCheck(
        "M245-C008-DOC-EXP-04",
        "Issue `#6643` defines canonical lane-C recovery and determinism hardening scope.",
    ),
    SnippetCheck("M245-C008-DOC-EXP-05", "Dependency token: `M245-C007`."),
    SnippetCheck(
        "M245-C008-DOC-EXP-06",
        "`scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`",
    ),
    SnippetCheck(
        "M245-C008-DOC-EXP-07",
        "`tests/tooling/test_check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`",
    ),
    SnippetCheck(
        "M245-C008-DOC-EXP-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-C008-DOC-EXP-09",
        "fail-closed snippet checks on owned lane-C",
    ),
    SnippetCheck(
        "M245-C008-DOC-EXP-10",
        "`tmp/reports/m245/M245-C008/lowering_ir_portability_contracts_recovery_and_determinism_hardening_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C008-DOC-PKT-01",
        "# M245-C008 Lowering/IR Portability Contracts Recovery and Determinism Hardening Packet",
    ),
    SnippetCheck("M245-C008-DOC-PKT-02", "Packet: `M245-C008`"),
    SnippetCheck("M245-C008-DOC-PKT-03", "Issue: `#6643`"),
    SnippetCheck("M245-C008-DOC-PKT-04", "Dependencies: `M245-C007`"),
    SnippetCheck("M245-C008-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M245-C008-DOC-PKT-06",
        "m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_packet.md",
    ),
    SnippetCheck(
        "M245-C008-DOC-PKT-07",
        "package.json` lane-C readiness chain (shared-owner follow-up)",
    ),
    SnippetCheck(
        "M245-C008-DOC-PKT-08",
        "python scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py --emit-json",
    ),
    SnippetCheck(
        "M245-C008-DOC-PKT-09",
        "improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-C008-DOC-PKT-10",
        "tmp/reports/m245/M245-C008/lowering_ir_portability_contracts_recovery_and_determinism_hardening_summary.json",
    ),
)

C007_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C008-C007-01",
        "Contract ID: `objc3c-lowering-ir-portability-contracts-diagnostics-hardening/m245-c007-v1`",
    ),
    SnippetCheck("M245-C008-C007-02", "Dependencies: `M245-C006`"),
)

C007_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-C008-C007-PKT-01", "Packet: `M245-C007`"),
    SnippetCheck("M245-C008-C007-PKT-02", "Dependencies: `M245-C006`"),
)

C007_CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C008-C007-CHK-01",
        'MODE = "m245-c007-lowering-ir-portability-contracts-diagnostics-hardening-contract-v1"',
    ),
    SnippetCheck(
        "M245-C008-C007-CHK-02",
        "DEFAULT_SUMMARY_OUT = Path(",
    ),
)

C007_TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-C008-C007-TST-01", "def test_contract_passes_on_repository_sources"),
    SnippetCheck(
        "M245-C008-C007-TST-02",
        "def test_contract_fails_closed_when_packet_dependency_token_drifts",
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
    parser.add_argument("--c007-expectations-doc", type=Path, default=DEFAULT_C007_EXPECTATIONS_DOC)
    parser.add_argument("--c007-packet-doc", type=Path, default=DEFAULT_C007_PACKET_DOC)
    parser.add_argument("--c007-checker", type=Path, default=DEFAULT_C007_CHECKER)
    parser.add_argument("--c007-test", type=Path, default=DEFAULT_C007_TEST)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
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
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M245-C008-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-C008-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c007_expectations_doc, "M245-C008-C007-EXISTS", C007_EXPECTATIONS_SNIPPETS),
        (args.c007_packet_doc, "M245-C008-C007-PKT-EXISTS", C007_PACKET_SNIPPETS),
        (args.c007_checker, "M245-C008-C007-CHK-EXISTS", C007_CHECKER_SNIPPETS),
        (args.c007_test, "M245-C008-C007-TST-EXISTS", C007_TEST_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

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
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

