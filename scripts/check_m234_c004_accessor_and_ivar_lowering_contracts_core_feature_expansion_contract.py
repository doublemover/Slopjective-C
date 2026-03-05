#!/usr/bin/env python3
"""Fail-closed contract checker for M234-C004 accessor/ivar lowering core feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_accessor_and_ivar_lowering_contracts_core_feature_expansion_c004_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_packet.md"
)
DEFAULT_C003_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_accessor_and_ivar_lowering_contracts_core_feature_implementation_c003_expectations.md"
)
DEFAULT_C003_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-C004/accessor_and_ivar_lowering_contracts_core_feature_expansion_summary.json"
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
        "M234-C004-DOC-EXP-01",
        "# M234 Accessor and Ivar Lowering Contracts Core Feature Expansion Expectations (C004)",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-02",
        "Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-core-feature-expansion/m234-c004-v1`",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-03",
        "Dependencies: `M234-C003`",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-04",
        "Issue `#5722` defines canonical lane-C core-feature expansion scope.",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-05",
        "Dependency token: `M234-C003`.",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-06",
        "`scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-07",
        "`check:objc3c:m234-c004-lane-c-readiness`",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-08",
        "`check:objc3c:m234-c003-lane-c-readiness`",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-09",
        "Code/spec anchors and milestone optimization improvements",
    ),
    SnippetCheck(
        "M234-C004-DOC-EXP-10",
        "`tmp/reports/m234/M234-C004/accessor_and_ivar_lowering_contracts_core_feature_expansion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C004-DOC-PKT-01",
        "# M234-C004 Accessor and Ivar Lowering Contracts Core Feature Expansion Packet",
    ),
    SnippetCheck("M234-C004-DOC-PKT-02", "Packet: `M234-C004`"),
    SnippetCheck("M234-C004-DOC-PKT-03", "Issue: `#5722`"),
    SnippetCheck("M234-C004-DOC-PKT-04", "Dependencies: `M234-C003`"),
    SnippetCheck("M234-C004-DOC-PKT-05", "Freeze date: `2026-03-05`"),
    SnippetCheck(
        "M234-C004-DOC-PKT-06",
        "m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_packet.md",
    ),
    SnippetCheck(
        "M234-C004-DOC-PKT-07",
        "`check:objc3c:m234-c004-lane-c-readiness`",
    ),
    SnippetCheck(
        "M234-C004-DOC-PKT-08",
        "python scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py --emit-json",
    ),
    SnippetCheck(
        "M234-C004-DOC-PKT-09",
        "improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-C004-DOC-PKT-10",
        "tmp/reports/m234/M234-C004/accessor_and_ivar_lowering_contracts_core_feature_expansion_summary.json",
    ),
)

C003_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C004-C003-01",
        "Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-core-feature-implementation/m234-c003-v1`",
    ),
    SnippetCheck(
        "M234-C004-C003-02",
        "- Dependencies: `M234-C001`, `M234-C002`",
    ),
)

C003_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-C004-C003-PKT-01", "Packet: `M234-C003`"),
    SnippetCheck("M234-C004-C003-PKT-02", "Dependencies: `M234-C001`, `M234-C002`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C004-ARCH-01",
        "M234 lane-C C004 accessor and ivar lowering contracts core feature expansion anchors",
    ),
    SnippetCheck(
        "M234-C004-ARCH-02",
        "dependency token (`M234-C003`) in",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C004-SPC-01",
        "accessor and ivar lowering contracts core feature expansion shall preserve explicit lane-C dependency token (`M234-C003`)",
    ),
    SnippetCheck(
        "M234-C004-SPC-02",
        "core-feature expansion evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C004-META-01",
        "deterministic lane-C accessor and ivar lowering core-feature expansion metadata anchors for `M234-C004`",
    ),
    SnippetCheck(
        "M234-C004-META-02",
        "with explicit `M234-C003` dependency continuity and fail-closed core-feature expansion evidence continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-C004-PKG-01",
        '"check:objc3c:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract": '
        '"python scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py"',
    ),
    SnippetCheck(
        "M234-C004-PKG-02",
        '"test:tooling:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract": '
        '"python -m pytest tests/tooling/test_check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py -q"',
    ),
    SnippetCheck(
        "M234-C004-PKG-03",
        '"check:objc3c:m234-c004-lane-c-readiness": '
        '"npm run check:objc3c:m234-c003-lane-c-readiness '
        '&& npm run check:objc3c:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract '
        '&& npm run test:tooling:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract"',
    ),
    SnippetCheck("M234-C004-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M234-C004-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M234-C004-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M234-C004-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--c003-expectations-doc", type=Path, default=DEFAULT_C003_EXPECTATIONS_DOC)
    parser.add_argument("--c003-packet-doc", type=Path, default=DEFAULT_C003_PACKET_DOC)
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M234-C004-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-C004-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c003_expectations_doc, "M234-C004-C003-EXISTS", C003_EXPECTATIONS_SNIPPETS),
        (args.c003_packet_doc, "M234-C004-C003-PKT-EXISTS", C003_PACKET_SNIPPETS),
        (args.architecture_doc, "M234-C004-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-C004-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-C004-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-C004-PKG-EXISTS", PACKAGE_SNIPPETS),
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
