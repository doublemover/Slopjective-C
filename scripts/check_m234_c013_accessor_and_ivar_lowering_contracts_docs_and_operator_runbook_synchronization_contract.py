#!/usr/bin/env python3
"""Fail-closed checker for M234-C013 accessor and ivar lowering docs and operator runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-c013-accessor-and-ivar-lowering-contracts-docs-and-operator-runbook-synchronization-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_c013_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_C012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_accessor_and_ivar_lowering_contracts_cross_lane_integration_sync_c012_expectations.md"
)
DEFAULT_C012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_c012_accessor_and_ivar_lowering_contracts_cross_lane_integration_sync_packet.md"
)
DEFAULT_C012_CHECKER = (
    ROOT / "scripts" / "check_m234_c012_accessor_and_ivar_lowering_contracts_cross_lane_integration_sync_contract.py"
)
DEFAULT_C012_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m234_c012_accessor_and_ivar_lowering_contracts_cross_lane_integration_sync_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-C013/accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_summary.json"
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
        "M234-C013-DOC-EXP-01",
        "# M234 Accessor and Ivar Lowering Contracts Docs and Operator Runbook Synchronization Expectations (C013)",
    ),
    SnippetCheck(
        "M234-C013-DOC-EXP-02",
        "Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-docs-and-operator-runbook-synchronization/m234-c013-v1`",
    ),
    SnippetCheck(
        "M234-C013-DOC-EXP-03",
        "Issue `#5731` defines canonical lane-C docs and operator runbook synchronization scope.",
    ),
    SnippetCheck("M234-C013-DOC-EXP-04", "Dependencies: `M234-C012`"),
    SnippetCheck(
        "M234-C013-DOC-EXP-05",
        "docs/contracts/m234_accessor_and_ivar_lowering_contracts_cross_lane_integration_sync_c012_expectations.md",
    ),
    SnippetCheck(
        "M234-C013-DOC-EXP-06",
        "scripts/check_m234_c013_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M234-C013-DOC-EXP-07",
        "`check:objc3c:m234-c013-lane-c-readiness`",
    ),
    SnippetCheck(
        "M234-C013-DOC-EXP-08",
        "`tmp/reports/m234/M234-C013/accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C013-DOC-PKT-01",
        "# M234-C013 Accessor and Ivar Lowering Contracts Docs and Operator Runbook Synchronization Packet",
    ),
    SnippetCheck("M234-C013-DOC-PKT-02", "Packet: `M234-C013`"),
    SnippetCheck("M234-C013-DOC-PKT-03", "Issue: `#5731`"),
    SnippetCheck("M234-C013-DOC-PKT-04", "Dependencies: `M234-C012`"),
    SnippetCheck(
        "M234-C013-DOC-PKT-05",
        "scripts/check_m234_c013_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M234-C013-DOC-PKT-06",
        "tests/tooling/test_check_m234_c013_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck("M234-C013-DOC-PKT-07", "`npm run check:objc3c:m234-c013-lane-c-readiness`"),
)

C012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C013-C012-DOC-01",
        "# M234 Accessor and Ivar Lowering Contracts Cross-Lane Integration Sync Expectations (C012)",
    ),
    SnippetCheck(
        "M234-C013-C012-DOC-02",
        "Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-cross-lane-integration-sync/m234-c012-v1`",
    ),
)

C012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-C013-C012-PKT-01", "Packet: `M234-C012`"),
    SnippetCheck("M234-C013-C012-PKT-02", "Dependencies: `M234-C011`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C013-ARCH-01",
        "M234 lane-C C013 accessor and ivar lowering contracts docs and operator runbook synchronization anchors",
    ),
    SnippetCheck(
        "M234-C013-ARCH-02",
        "dependency token (`M234-C012`) in",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C013-SPC-01",
        "accessor and ivar lowering contracts docs and operator runbook synchronization governance shall preserve explicit",
    ),
    SnippetCheck(
        "M234-C013-SPC-02",
        "lane-C dependency anchors (`M234-C012`) and fail closed on docs and operator runbook synchronization evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C013-META-01",
        "deterministic lane-C accessor and ivar lowering docs and operator runbook synchronization metadata anchors for `M234-C013`",
    ),
    SnippetCheck(
        "M234-C013-META-02",
        "explicit `M234-C012` dependency continuity so docs and operator runbook synchronization drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C013-PKG-01",
        '"check:objc3c:m234-c013-accessor-and-ivar-lowering-contracts-docs-and-operator-runbook-synchronization-contract": '
        '"python scripts/check_m234_c013_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_contract.py"',
    ),
    SnippetCheck(
        "M234-C013-PKG-02",
        '"test:tooling:m234-c013-accessor-and-ivar-lowering-contracts-docs-and-operator-runbook-synchronization-contract": '
        '"python -m pytest tests/tooling/test_check_m234_c013_accessor_and_ivar_lowering_contracts_docs_and_operator_runbook_synchronization_contract.py -q"',
    ),
    SnippetCheck(
        "M234-C013-PKG-03",
        '"check:objc3c:m234-c013-lane-c-readiness": '
        '"npm run check:objc3c:m234-c012-lane-c-readiness '
        '&& npm run check:objc3c:m234-c013-accessor-and-ivar-lowering-contracts-docs-and-operator-runbook-synchronization-contract '
        '&& npm run test:tooling:m234-c013-accessor-and-ivar-lowering-contracts-docs-and-operator-runbook-synchronization-contract"',
    ),
    SnippetCheck("M234-C013-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M234-C013-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M234-C013-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M234-C013-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--c012-expectations-doc", type=Path, default=DEFAULT_C012_EXPECTATIONS_DOC)
    parser.add_argument("--c012-packet-doc", type=Path, default=DEFAULT_C012_PACKET_DOC)
    parser.add_argument("--c012-checker", type=Path, default=DEFAULT_C012_CHECKER)
    parser.add_argument("--c012-test", type=Path, default=DEFAULT_C012_TEST)
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
        (args.expectations_doc, "M234-C013-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-C013-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c012_expectations_doc, "M234-C013-C012-DOC-EXISTS", C012_EXPECTATIONS_SNIPPETS),
        (args.c012_packet_doc, "M234-C013-C012-PKT-EXISTS", C012_PACKET_SNIPPETS),
        (args.architecture_doc, "M234-C013-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-C013-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-C013-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-C013-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        c_total, c_findings = check_doc_contract(path=path, exists_check_id=exists_id, snippets=snippets)
        checks_total += c_total
        findings.extend(c_findings)

    for path, check_id in (
        (args.c012_checker, "M234-C013-C012-CHECKER-EXISTS"),
        (args.c012_test, "M234-C013-C012-TEST-EXISTS"),
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




