#!/usr/bin/env python3
"""Fail-closed checker for M234-C005 accessor and ivar lowering edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-c005-accessor-and-ivar-lowering-contracts-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_c005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_c005_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_C004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_accessor_and_ivar_lowering_contracts_core_feature_expansion_c004_expectations.md"
)
DEFAULT_C004_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_packet.md"
)
DEFAULT_C004_CHECKER = (
    ROOT / "scripts" / "check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py"
)
DEFAULT_C004_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-C005/accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_summary.json"
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
        "M234-C005-DOC-EXP-01",
        "# M234 Accessor and Ivar Lowering Contracts Edge-Case and Compatibility Completion Expectations (C005)",
    ),
    SnippetCheck(
        "M234-C005-DOC-EXP-02",
        "Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-edge-case-and-compatibility-completion/m234-c005-v1`",
    ),
    SnippetCheck("M234-C005-DOC-EXP-03", "Issue `#5723` defines canonical lane-C edge-case and compatibility completion scope."),
    SnippetCheck("M234-C005-DOC-EXP-04", "Dependencies: `M234-C004`"),
    SnippetCheck(
        "M234-C005-DOC-EXP-05",
        "docs/contracts/m234_accessor_and_ivar_lowering_contracts_core_feature_expansion_c004_expectations.md",
    ),
    SnippetCheck(
        "M234-C005-DOC-EXP-06",
        "scripts/check_m234_c005_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M234-C005-DOC-EXP-07",
        "`check:objc3c:m234-c005-lane-c-readiness`",
    ),
    SnippetCheck(
        "M234-C005-DOC-EXP-08",
        "`tmp/reports/m234/M234-C005/accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C005-DOC-PKT-01",
        "# M234-C005 Accessor and Ivar Lowering Contracts Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M234-C005-DOC-PKT-02", "Packet: `M234-C005`"),
    SnippetCheck("M234-C005-DOC-PKT-03", "Issue: `#5723`"),
    SnippetCheck("M234-C005-DOC-PKT-04", "Dependencies: `M234-C004`"),
    SnippetCheck(
        "M234-C005-DOC-PKT-05",
        "scripts/check_m234_c005_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M234-C005-DOC-PKT-06",
        "tests/tooling/test_check_m234_c005_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck("M234-C005-DOC-PKT-07", "`npm run check:objc3c:m234-c005-lane-c-readiness`"),
)

C004_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C005-C004-DOC-01",
        "# M234 Accessor and Ivar Lowering Contracts Core Feature Expansion Expectations (C004)",
    ),
    SnippetCheck(
        "M234-C005-C004-DOC-02",
        "Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-core-feature-expansion/m234-c004-v1`",
    ),
)

C004_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-C005-C004-PKT-01", "Packet: `M234-C004`"),
    SnippetCheck("M234-C005-C004-PKT-02", "Dependencies: `M234-C003`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C005-ARCH-01",
        "M234 lane-C C005 accessor and ivar lowering contracts edge-case and compatibility completion anchors",
    ),
    SnippetCheck(
        "M234-C005-ARCH-02",
        "dependency token (`M234-C004`) in",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C005-SPC-01",
        "accessor and ivar lowering contracts edge-case and compatibility completion governance shall preserve explicit",
    ),
    SnippetCheck(
        "M234-C005-SPC-02",
        "lane-C dependency anchors (`M234-C004`) and fail closed on edge-case and compatibility completion evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C005-META-01",
        "deterministic lane-C accessor and ivar lowering edge-case and compatibility completion metadata anchors for `M234-C005`",
    ),
    SnippetCheck(
        "M234-C005-META-02",
        "explicit `M234-C004` dependency continuity so edge-case and compatibility completion drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C005-PKG-01",
        '"check:objc3c:m234-c005-accessor-and-ivar-lowering-contracts-edge-case-and-compatibility-completion-contract": '
        '"python scripts/check_m234_c005_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_contract.py"',
    ),
    SnippetCheck(
        "M234-C005-PKG-02",
        '"test:tooling:m234-c005-accessor-and-ivar-lowering-contracts-edge-case-and-compatibility-completion-contract": '
        '"python -m pytest tests/tooling/test_check_m234_c005_accessor_and_ivar_lowering_contracts_edge_case_and_compatibility_completion_contract.py -q"',
    ),
    SnippetCheck(
        "M234-C005-PKG-03",
        '"check:objc3c:m234-c005-lane-c-readiness": '
        '"npm run check:objc3c:m234-c004-lane-c-readiness '
        '&& npm run check:objc3c:m234-c005-accessor-and-ivar-lowering-contracts-edge-case-and-compatibility-completion-contract '
        '&& npm run test:tooling:m234-c005-accessor-and-ivar-lowering-contracts-edge-case-and-compatibility-completion-contract"',
    ),
    SnippetCheck("M234-C005-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M234-C005-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M234-C005-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M234-C005-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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


def check_file_exists(path: Path, check_id: str) -> tuple[int, list[Finding]]:
    if path.exists() and path.is_file():
        return 1, []
    if path.exists() and not path.is_file():
        return 1, [Finding(display_path(path), check_id, f"required path is not a file: {display_path(path)}")]
    return 1, [Finding(display_path(path), check_id, f"required file is missing: {display_path(path)}")]


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])

    checks_total = 0
    findings: list[Finding] = []

    for path, exists_id, snippets in (
        (args.expectations_doc, "M234-C005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-C005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c004_expectations_doc, "M234-C005-C004-DOC-EXISTS", C004_EXPECTATIONS_SNIPPETS),
        (args.c004_packet_doc, "M234-C005-C004-PKT-EXISTS", C004_PACKET_SNIPPETS),
        (args.architecture_doc, "M234-C005-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-C005-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-C005-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-C005-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        c_total, c_findings = check_doc_contract(path=path, exists_check_id=exists_id, snippets=snippets)
        checks_total += c_total
        findings.extend(c_findings)

    for path, check_id in (
        (args.c004_checker, "M234-C005-C004-CHECKER-EXISTS"),
        (args.c004_test, "M234-C005-C004-TEST-EXISTS"),
    ):
        c_total, c_findings = check_file_exists(path=path, check_id=check_id)
        checks_total += c_total
        findings.extend(c_findings)

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [finding.__dict__ for finding in findings],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary))

    if not ok:
        for finding in findings:
            sys.stderr.write(f"[{finding.check_id}] {finding.artifact}: {finding.detail}\n")
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {summary['checks_passed']}/{summary['checks_total']} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
