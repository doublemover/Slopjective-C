#!/usr/bin/env python3
"""Fail-closed checker for M235-C011 qualified type performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m235-c011-qualified-type-lowering-and-abi-representation-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_c011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_c011_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_packet.md"
)
DEFAULT_C010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_c010_expectations.md"
)
DEFAULT_C010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_packet.md"
)
DEFAULT_C010_CHECKER = (
    ROOT / "scripts" / "check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py"
)
DEFAULT_C010_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m235/M235-C011/qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M235-C011-DEP-C010-01",
        Path(
            "docs/contracts/m235_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_c010_expectations.md"
        ),
    ),
    AssetCheck(
        "M235-C011-DEP-C010-02",
        Path("spec/planning/compiler/m235/m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M235-C011-DEP-C010-03",
        Path("scripts/check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M235-C011-DEP-C010-04",
        Path("tests/tooling/test_check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-DOC-EXP-01",
        "# M235 Qualified Type Lowering and ABI Representation Performance and Quality Guardrails Expectations (C011)",
    ),
    SnippetCheck(
        "M235-C011-DOC-EXP-02",
        "Contract ID: `objc3c-qualified-type-lowering-and-abi-representation-performance-and-quality-guardrails/m235-c011-v1`",
    ),
    SnippetCheck("M235-C011-DOC-EXP-03", "Dependencies: `M235-C010`"),
    SnippetCheck(
        "M235-C011-DOC-EXP-04",
        "Issue `#5821` defines canonical lane-C performance and quality guardrails scope.",
    ),
    SnippetCheck(
        "M235-C011-DOC-EXP-05",
        "optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M235-C011-DOC-EXP-06",
        "docs/contracts/m235_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_c010_expectations.md",
    ),
    SnippetCheck(
        "M235-C011-DOC-EXP-07",
        "scripts/check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M235-C011-DOC-EXP-08",
        "tests/tooling/test_check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck("M235-C011-DOC-EXP-09", "`C010 readiness -> C011 checker -> C011 pytest`."),
    SnippetCheck(
        "M235-C011-DOC-EXP-10",
        "`tmp/reports/m235/M235-C011/qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-DOC-PKT-01",
        "# M235-C011 Qualified Type Lowering and ABI Representation Performance and Quality Guardrails Packet",
    ),
    SnippetCheck("M235-C011-DOC-PKT-02", "Packet: `M235-C011`"),
    SnippetCheck("M235-C011-DOC-PKT-03", "Issue: `#5821`"),
    SnippetCheck("M235-C011-DOC-PKT-04", "Dependencies: `M235-C010`"),
    SnippetCheck(
        "M235-C011-DOC-PKT-05",
        "including code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M235-C011-DOC-PKT-06",
        "scripts/check_m235_c011_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M235-C011-DOC-PKT-07",
        "tests/tooling/test_check_m235_c011_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck("M235-C011-DOC-PKT-08", "`npm run check:objc3c:m235-c010-lane-c-readiness`"),
    SnippetCheck(
        "M235-C011-DOC-PKT-09",
        "`C010 readiness -> C011 checker -> C011 pytest`",
    ),
    SnippetCheck(
        "M235-C011-DOC-PKT-10",
        "spec/planning/compiler/m235/m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_packet.md",
    ),
)

C010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-C010-DOC-01",
        "# M235 Qualified Type Lowering and ABI Representation Conformance Corpus Expansion Expectations (C010)",
    ),
    SnippetCheck(
        "M235-C011-C010-DOC-02",
        "Contract ID: `objc3c-qualified-type-lowering-and-abi-representation-conformance-corpus-expansion/m235-c010-v1`",
    ),
    SnippetCheck("M235-C011-C010-DOC-03", "Dependencies: `M235-C009`"),
    SnippetCheck(
        "M235-C011-C010-DOC-04",
        "Issue `#5820` defines canonical lane-C conformance corpus expansion scope.",
    ),
)

C010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M235-C011-C010-PKT-01", "Packet: `M235-C010`"),
    SnippetCheck("M235-C011-C010-PKT-02", "Issue: `#5820`"),
    SnippetCheck("M235-C011-C010-PKT-03", "Dependencies: `M235-C009`"),
    SnippetCheck("M235-C011-C010-PKT-04", "Freeze date: `2026-03-05`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-ARCH-01",
        "M235 lane-C C011 qualified type lowering and ABI representation performance and quality guardrails anchors",
    ),
    SnippetCheck(
        "M235-C011-ARCH-02",
        "docs/contracts/m235_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_c011_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-SPC-01",
        "qualified type lowering and ABI representation performance and quality guardrails governance shall preserve explicit",
    ),
    SnippetCheck(
        "M235-C011-SPC-02",
        "lane-C dependency anchors (`M235-C010`) and fail closed on performance and quality guardrails evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-META-01",
        "deterministic lane-C qualified type lowering and ABI representation performance and quality guardrails metadata anchors for `M235-C011`",
    ),
    SnippetCheck(
        "M235-C011-META-02",
        "with explicit `M235-C010` dependency continuity so performance and quality guardrails lowering and ABI drift fails closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-C011-PKG-01",
        '"check:objc3c:m235-c010-qualified-type-lowering-and-abi-representation-conformance-corpus-expansion-contract": '
        '"python scripts/check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py"',
    ),
    SnippetCheck(
        "M235-C011-PKG-02",
        '"test:tooling:m235-c010-qualified-type-lowering-and-abi-representation-conformance-corpus-expansion-contract": '
        '"python -m pytest tests/tooling/test_check_m235_c010_qualified_type_lowering_and_abi_representation_conformance_corpus_expansion_contract.py -q"',
    ),
    SnippetCheck(
        "M235-C011-PKG-03",
        '"check:objc3c:m235-c010-lane-c-readiness": '
        '"npm run check:objc3c:m235-c009-lane-c-readiness '
        '&& npm run check:objc3c:m235-c010-qualified-type-lowering-and-abi-representation-conformance-corpus-expansion-contract '
        '&& npm run test:tooling:m235-c010-qualified-type-lowering-and-abi-representation-conformance-corpus-expansion-contract"',
    ),
    SnippetCheck(
        "M235-C011-PKG-04",
        '"check:objc3c:m235-c011-lane-c-readiness": '
        '"npm run check:objc3c:m235-c010-lane-c-readiness '
        '&& npm run check:objc3c:m235-c011-qualified-type-lowering-and-abi-representation-performance-and-quality-guardrails-contract '
        '&& npm run test:tooling:m235-c011-qualified-type-lowering-and-abi-representation-performance-and-quality-guardrails-contract"',
    ),
    SnippetCheck(
        "M235-C011-PKG-05",
        '"check:objc3c:m235-c011-qualified-type-lowering-and-abi-representation-performance-and-quality-guardrails-contract": '
        '"python scripts/check_m235_c011_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_contract.py"',
    ),
    SnippetCheck(
        "M235-C011-PKG-06",
        '"test:tooling:m235-c011-qualified-type-lowering-and-abi-representation-performance-and-quality-guardrails-contract": '
        '"python -m pytest tests/tooling/test_check_m235_c011_qualified_type_lowering_and_abi_representation_performance_and_quality_guardrails_contract.py -q"',
    ),
    SnippetCheck("M235-C011-PKG-07", '"compile:objc3c": '),
    SnippetCheck("M235-C011-PKG-08", '"proof:objc3c": '),
    SnippetCheck("M235-C011-PKG-09", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M235-C011-PKG-10", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--c010-expectations-doc", type=Path, default=DEFAULT_C010_EXPECTATIONS_DOC)
    parser.add_argument("--c010-packet-doc", type=Path, default=DEFAULT_C010_PACKET_DOC)
    parser.add_argument("--c010-checker", type=Path, default=DEFAULT_C010_CHECKER)
    parser.add_argument("--c010-test", type=Path, default=DEFAULT_C010_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
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
                display_path(path),
                exists_check_id,
                f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M235-C011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M235-C011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c010_expectations_doc, "M235-C011-C010-DOC-EXISTS", C010_EXPECTATIONS_SNIPPETS),
        (args.c010_packet_doc, "M235-C011-C010-PKT-EXISTS", C010_PACKET_SNIPPETS),
        (args.architecture_doc, "M235-C011-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M235-C011-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M235-C011-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M235-C011-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.c010_checker, "M235-C011-DEP-C010-ARG-01"),
        (args.c010_test, "M235-C011-DEP-C010-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))







