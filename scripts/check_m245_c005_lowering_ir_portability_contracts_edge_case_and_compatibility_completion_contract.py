#!/usr/bin/env python3
"""Fail-closed contract checker for M245-C005 lowering/IR portability edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-c005-lowering-ir-portability-contracts-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_c005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_C004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lowering_ir_portability_contracts_core_feature_expansion_c004_expectations.md"
)
DEFAULT_C004_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_c004_lowering_ir_portability_contracts_core_feature_expansion_packet.md"
)
DEFAULT_C004_CHECKER = (
    ROOT
    / "scripts"
    / "check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py"
)
DEFAULT_C004_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-C005/lowering_ir_portability_contracts_edge_case_and_compatibility_completion_summary.json"
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
        "M245-C005-DOC-EXP-01",
        "# M245 Lowering/IR Portability Contracts Edge-Case and Compatibility Completion Expectations (C005)",
    ),
    SnippetCheck(
        "M245-C005-DOC-EXP-02",
        "Contract ID: `objc3c-lowering-ir-portability-contracts-edge-case-and-compatibility-completion/m245-c005-v1`",
    ),
    SnippetCheck("M245-C005-DOC-EXP-03", "Dependencies: `M245-C004`"),
    SnippetCheck(
        "M245-C005-DOC-EXP-04",
        "Issue `#6640` defines canonical lane-C edge-case and compatibility completion scope.",
    ),
    SnippetCheck("M245-C005-DOC-EXP-05", "Dependency token: `M245-C004`."),
    SnippetCheck(
        "M245-C005-DOC-EXP-06",
        "`scripts/check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M245-C005-DOC-EXP-07",
        "`tests/tooling/test_check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M245-C005-DOC-EXP-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-C005-DOC-EXP-09",
        "fail-closed snippet checks on owned lane-C",
    ),
    SnippetCheck(
        "M245-C005-DOC-EXP-10",
        "`tmp/reports/m245/M245-C005/lowering_ir_portability_contracts_edge_case_and_compatibility_completion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C005-DOC-PKT-01",
        "# M245-C005 Lowering/IR Portability Contracts Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M245-C005-DOC-PKT-02", "Packet: `M245-C005`"),
    SnippetCheck("M245-C005-DOC-PKT-03", "Issue: `#6640`"),
    SnippetCheck("M245-C005-DOC-PKT-04", "Dependencies: `M245-C004`"),
    SnippetCheck("M245-C005-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M245-C005-DOC-PKT-06",
        "m245_c004_lowering_ir_portability_contracts_core_feature_expansion_packet.md",
    ),
    SnippetCheck(
        "M245-C005-DOC-PKT-07",
        "package.json` lane-C readiness chain (shared-owner follow-up)",
    ),
    SnippetCheck(
        "M245-C005-DOC-PKT-08",
        "python scripts/check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py --emit-json",
    ),
    SnippetCheck(
        "M245-C005-DOC-PKT-09",
        "improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-C005-DOC-PKT-10",
        "tmp/reports/m245/M245-C005/lowering_ir_portability_contracts_edge_case_and_compatibility_completion_summary.json",
    ),
)

C004_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C005-C004-01",
        "Contract ID: `objc3c-lowering-ir-portability-contracts-core-feature-expansion/m245-c004-v1`",
    ),
    SnippetCheck("M245-C005-C004-02", "Dependencies: `M245-C003`"),
)

C004_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-C005-C004-PKT-01", "Packet: `M245-C004`"),
    SnippetCheck("M245-C005-C004-PKT-02", "Dependencies: `M245-C003`"),
)

C004_CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C005-C004-CHK-01",
        'MODE = "m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract-v1"',
    ),
    SnippetCheck(
        "M245-C005-C004-CHK-02",
        "DEFAULT_SUMMARY_OUT = Path(",
    ),
)

C004_TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-C005-C004-TST-01", "def test_contract_passes_on_repository_sources"),
    SnippetCheck(
        "M245-C005-C004-TST-02",
        "def test_contract_fails_closed_when_package_chain_drops_c003_readiness",
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
    parser.add_argument("--c004-expectations-doc", type=Path, default=DEFAULT_C004_EXPECTATIONS_DOC)
    parser.add_argument("--c004-packet-doc", type=Path, default=DEFAULT_C004_PACKET_DOC)
    parser.add_argument("--c004-checker", type=Path, default=DEFAULT_C004_CHECKER)
    parser.add_argument("--c004-test", type=Path, default=DEFAULT_C004_TEST)
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
        (args.expectations_doc, "M245-C005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-C005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c004_expectations_doc, "M245-C005-C004-EXISTS", C004_EXPECTATIONS_SNIPPETS),
        (args.c004_packet_doc, "M245-C005-C004-PKT-EXISTS", C004_PACKET_SNIPPETS),
        (args.c004_checker, "M245-C005-C004-CHK-EXISTS", C004_CHECKER_SNIPPETS),
        (args.c004_test, "M245-C005-C004-TST-EXISTS", C004_TEST_SNIPPETS),
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
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
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

