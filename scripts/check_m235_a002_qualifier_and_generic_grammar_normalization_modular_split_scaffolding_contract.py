#!/usr/bin/env python3
"""Fail-closed checker for M235-A002 qualifier/generic grammar normalization modular split/scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_packet.md"
)
DEFAULT_A001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_A001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_a001_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_packet.md"
)
DEFAULT_A001_CHECKER = (
    ROOT / "scripts" / "check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py"
)
DEFAULT_A001_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m235/M235-A002/qualifier_and_generic_grammar_normalization_modular_split_scaffolding_summary.json"
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
        "M235-A002-DEP-A001-01",
        Path("docs/contracts/m235_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_a001_expectations.md"),
    ),
    AssetCheck(
        "M235-A002-DEP-A001-02",
        Path("spec/planning/compiler/m235/m235_a001_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M235-A002-DEP-A001-03",
        Path("scripts/check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py"),
    ),
    AssetCheck(
        "M235-A002-DEP-A001-04",
        Path("tests/tooling/test_check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-DOC-EXP-01",
        "# M235 Qualifier/Generic Grammar Normalization Modular Split/Scaffolding Expectations (A002)",
    ),
    SnippetCheck(
        "M235-A002-DOC-EXP-02",
        "Contract ID: `objc3c-qualifier-and-generic-grammar-normalization-modular-split-scaffolding/m235-a002-v1`",
    ),
    SnippetCheck(
        "M235-A002-DOC-EXP-03",
        "Issue `#5765` defines canonical lane-A modular split/scaffolding scope.",
    ),
    SnippetCheck("M235-A002-DOC-EXP-04", "Dependencies: `M235-A001`"),
    SnippetCheck(
        "M235-A002-DOC-EXP-05",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M235-A002-DOC-EXP-06",
        "docs/contracts/m235_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_a001_expectations.md",
    ),
    SnippetCheck(
        "M235-A002-DOC-EXP-07",
        "scripts/check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py",
    ),
    SnippetCheck(
        "M235-A002-DOC-EXP-08",
        "tests/tooling/test_check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py",
    ),
    SnippetCheck(
        "M235-A002-DOC-EXP-09",
        "`check:objc3c:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract`",
    ),
    SnippetCheck("M235-A002-DOC-EXP-10", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck(
        "M235-A002-DOC-EXP-11",
        "`tmp/reports/m235/M235-A002/qualifier_and_generic_grammar_normalization_modular_split_scaffolding_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-DOC-PKT-01",
        "# M235-A002 Qualifier/Generic Grammar Normalization Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M235-A002-DOC-PKT-02", "Packet: `M235-A002`"),
    SnippetCheck("M235-A002-DOC-PKT-03", "Issue: `#5765`"),
    SnippetCheck("M235-A002-DOC-PKT-04", "Dependencies: `M235-A001`"),
    SnippetCheck(
        "M235-A002-DOC-PKT-05",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M235-A002-DOC-PKT-06",
        "docs/contracts/m235_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_a002_expectations.md",
    ),
    SnippetCheck(
        "M235-A002-DOC-PKT-07",
        "scripts/check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M235-A002-DOC-PKT-08",
        "tests/tooling/test_check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck("M235-A002-DOC-PKT-09", "`npm run check:objc3c:m235-a002-lane-a-readiness`"),
    SnippetCheck(
        "M235-A002-DOC-PKT-10",
        "`tmp/reports/m235/M235-A002/qualifier_and_generic_grammar_normalization_modular_split_scaffolding_summary.json`",
    ),
    SnippetCheck(
        "M235-A002-DOC-PKT-11",
        "spec/planning/compiler/m235/m235_a001_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_packet.md",
    ),
)

A001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-A001-DOC-01",
        "# M235 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M235-A002-A001-DOC-02",
        "Contract ID: `objc3c-qualifier-and-generic-grammar-normalization/m235-a001-v1`",
    ),
    SnippetCheck(
        "M235-A002-A001-DOC-03",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
)

A001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M235-A002-A001-PKT-01", "Packet: `M235-A001`"),
    SnippetCheck("M235-A002-A001-PKT-02", "Dependencies: none"),
    SnippetCheck(
        "M235-A002-A001-PKT-03",
        "`tmp/reports/m235/M235-A001/qualifier_and_generic_grammar_normalization_contract_summary.json`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-ARCH-01",
        "M235 lane-A A002 qualifier/generic grammar normalization modular split/scaffolding anchors",
    ),
    SnippetCheck(
        "M235-A002-ARCH-02",
        "docs/contracts/m235_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_a002_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-SPC-01",
        "qualifier/generic grammar normalization modular split/scaffolding governance shall preserve explicit",
    ),
    SnippetCheck(
        "M235-A002-SPC-02",
        "lane-A dependency anchors (`M235-A001`) and fail closed on scaffolding evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-META-01",
        "deterministic lane-A qualifier/generic grammar normalization modular split metadata anchors for `M235-A002`",
    ),
    SnippetCheck(
        "M235-A002-META-02",
        "explicit `M235-A001` dependency continuity so nullability/generics/qualifier scaffolding drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-A002-PKG-01",
        '"check:objc3c:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract": '
        '"python scripts/check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M235-A002-PKG-02",
        '"test:tooling:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M235-A002-PKG-03",
        '"check:objc3c:m235-a002-lane-a-readiness": '
        '"npm run check:objc3c:m235-a001-lane-a-readiness '
        '&& npm run check:objc3c:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract"',
    ),
    SnippetCheck("M235-A002-PKG-04", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M235-A002-PKG-05", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--a001-expectations-doc", type=Path, default=DEFAULT_A001_EXPECTATIONS_DOC)
    parser.add_argument("--a001-packet-doc", type=Path, default=DEFAULT_A001_PACKET_DOC)
    parser.add_argument("--a001-checker", type=Path, default=DEFAULT_A001_CHECKER)
    parser.add_argument("--a001-test", type=Path, default=DEFAULT_A001_TEST)
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
        (args.expectations_doc, "M235-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M235-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a001_expectations_doc, "M235-A002-A001-DOC-EXISTS", A001_EXPECTATIONS_SNIPPETS),
        (args.a001_packet_doc, "M235-A002-A001-PKT-EXISTS", A001_PACKET_SNIPPETS),
        (args.architecture_doc, "M235-A002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M235-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M235-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M235-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(finding_batch)

    for path, check_id in (
        (args.a001_checker, "M235-A002-DEP-A001-ARG-01"),
        (args.a001_test, "M235-A002-DEP-A001-ARG-02"),
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
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
        ],
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
