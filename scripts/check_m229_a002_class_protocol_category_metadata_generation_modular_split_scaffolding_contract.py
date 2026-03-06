#!/usr/bin/env python3
"""Fail-closed checker for M229-A002 class/protocol/category metadata modular split/scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m229-a002-class-protocol-category-metadata-generation-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m229_class_protocol_category_metadata_generation_modular_split_scaffolding_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m229"
    / "m229_a002_class_protocol_category_metadata_generation_modular_split_scaffolding_packet.md"
)
DEFAULT_A001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m229_class_protocol_category_metadata_generation_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_A001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m229"
    / "m229_a001_class_protocol_category_metadata_generation_contract_and_architecture_freeze_packet.md"
)
DEFAULT_A001_CHECKER = ROOT / "scripts" / "check_m229_a001_class_protocol_category_metadata_generation_contract.py"
DEFAULT_A001_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m229_a001_class_protocol_category_metadata_generation_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m229/M229-A002/class_protocol_category_metadata_generation_modular_split_scaffolding_summary.json"
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
        "M229-A002-DEP-A001-01",
        Path("docs/contracts/m229_class_protocol_category_metadata_generation_contract_and_architecture_freeze_a001_expectations.md"),
    ),
    AssetCheck(
        "M229-A002-DEP-A001-02",
        Path("spec/planning/compiler/m229/m229_a001_class_protocol_category_metadata_generation_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M229-A002-DEP-A001-03",
        Path("scripts/check_m229_a001_class_protocol_category_metadata_generation_contract.py"),
    ),
    AssetCheck(
        "M229-A002-DEP-A001-04",
        Path("tests/tooling/test_check_m229_a001_class_protocol_category_metadata_generation_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-DOC-EXP-01",
        "# M229 Class/Protocol/Category Metadata Generation Modular Split/Scaffolding Expectations (A002)",
    ),
    SnippetCheck(
        "M229-A002-DOC-EXP-02",
        "Contract ID: `objc3c-class-protocol-category-metadata-generation-modular-split-scaffolding/m229-a002-v1`",
    ),
    SnippetCheck("M229-A002-DOC-EXP-03", "Dependencies: `M229-A001`"),
    SnippetCheck(
        "M229-A002-DOC-EXP-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M229-A002-DOC-EXP-05",
        "docs/contracts/m229_class_protocol_category_metadata_generation_contract_and_architecture_freeze_a001_expectations.md",
    ),
    SnippetCheck(
        "M229-A002-DOC-EXP-06",
        "scripts/check_m229_a001_class_protocol_category_metadata_generation_contract.py",
    ),
    SnippetCheck(
        "M229-A002-DOC-EXP-07",
        "tests/tooling/test_check_m229_a001_class_protocol_category_metadata_generation_contract.py",
    ),
    SnippetCheck(
        "M229-A002-DOC-EXP-08",
        "`check:objc3c:m229-a002-class-protocol-category-metadata-generation-modular-split-scaffolding-contract`",
    ),
    SnippetCheck("M229-A002-DOC-EXP-09", "`test:objc3c:execution-smoke`"),
    SnippetCheck(
        "M229-A002-DOC-EXP-10",
        "`tmp/reports/m229/M229-A002/class_protocol_category_metadata_generation_modular_split_scaffolding_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-DOC-PKT-01",
        "# M229-A002 Class/Protocol/Category Metadata Generation Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M229-A002-DOC-PKT-02", "Packet: `M229-A002`"),
    SnippetCheck("M229-A002-DOC-PKT-03", "Dependencies: `M229-A001`"),
    SnippetCheck(
        "M229-A002-DOC-PKT-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M229-A002-DOC-PKT-05",
        "docs/contracts/m229_class_protocol_category_metadata_generation_modular_split_scaffolding_a002_expectations.md",
    ),
    SnippetCheck(
        "M229-A002-DOC-PKT-06",
        "scripts/check_m229_a002_class_protocol_category_metadata_generation_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M229-A002-DOC-PKT-07",
        "tests/tooling/test_check_m229_a002_class_protocol_category_metadata_generation_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck("M229-A002-DOC-PKT-08", "`npm run check:objc3c:m229-a002-lane-a-readiness`"),
    SnippetCheck(
        "M229-A002-DOC-PKT-09",
        "`tmp/reports/m229/M229-A002/class_protocol_category_metadata_generation_modular_split_scaffolding_summary.json`",
    ),
    SnippetCheck(
        "M229-A002-DOC-PKT-10",
        "spec/planning/compiler/m229/m229_a001_class_protocol_category_metadata_generation_contract_and_architecture_freeze_packet.md",
    ),
)

A001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-A001-DOC-01",
        "# M229 Class/Protocol/Category Metadata Generation Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M229-A002-A001-DOC-02",
        "Contract ID: `objc3c-class-protocol-category-metadata-generation/m229-a001-v1`",
    ),
    SnippetCheck(
        "M229-A002-A001-DOC-03",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
)

A001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M229-A002-A001-PKT-01", "Packet: `M229-A001`"),
    SnippetCheck("M229-A002-A001-PKT-02", "Dependencies: none"),
    SnippetCheck(
        "M229-A002-A001-PKT-03",
        "`tmp/reports/m229/M229-A001/class_protocol_category_metadata_generation_contract_summary.json`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-ARCH-01",
        "M229 lane-A A002 class/protocol/category metadata generation modular split/scaffolding anchors",
    ),
    SnippetCheck(
        "M229-A002-ARCH-02",
        "docs/contracts/m229_class_protocol_category_metadata_generation_modular_split_scaffolding_a002_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-SPC-01",
        "class/protocol/category metadata generation modular split/scaffolding governance shall preserve explicit",
    ),
    SnippetCheck(
        "M229-A002-SPC-02",
        "lane-A dependency anchors (`M229-A001`) and fail closed on scaffolding evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-META-01",
        "deterministic lane-A class/protocol/category metadata generation modular split anchors for `M229-A002`",
    ),
    SnippetCheck(
        "M229-A002-META-02",
        "explicit `M229-A001` dependency continuity so class/protocol/category metadata scaffolding drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A002-PKG-01",
        '"check:objc3c:m229-a002-class-protocol-category-metadata-generation-modular-split-scaffolding-contract": '
        '"python scripts/check_m229_a002_class_protocol_category_metadata_generation_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M229-A002-PKG-02",
        '"test:tooling:m229-a002-class-protocol-category-metadata-generation-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m229_a002_class_protocol_category_metadata_generation_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M229-A002-PKG-03",
        '"check:objc3c:m229-a002-lane-a-readiness": '
        '"npm run check:objc3c:m229-a001-lane-a-readiness '
        '&& npm run check:objc3c:m229-a002-class-protocol-category-metadata-generation-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m229-a002-class-protocol-category-metadata-generation-modular-split-scaffolding-contract"',
    ),
    SnippetCheck(
        "M229-A002-PKG-04",
        '"check:objc3c:m229-a001-lane-a-readiness": '
        '"npm run check:objc3c:m229-a001-class-protocol-category-metadata-generation-contract '
        '&& npm run test:tooling:m229-a001-class-protocol-category-metadata-generation-contract"',
    ),
    SnippetCheck("M229-A002-PKG-05", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M229-A002-PKG-06", '"test:objc3c:parser-ast-extraction": '),
    SnippetCheck("M229-A002-PKG-07", '"test:objc3c:execution-smoke": '),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_file(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required file is missing: {display_path(path)}"))
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


def check_prerequisites() -> tuple[int, list[Finding]]:
    checks_total = len(PREREQUISITE_ASSETS)
    failures: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        resolved = ROOT / asset.relative_path
        if not resolved.exists() or not resolved.is_file():
            failures.append(
                Finding(
                    asset.relative_path.as_posix(),
                    asset.check_id,
                    f"missing required dependency artifact: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, failures


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    prereq_checks, prereq_failures = check_prerequisites()
    checks_total += prereq_checks
    failures.extend(prereq_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M229-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M229-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a001_expectations_doc, "M229-A002-A001-DOC-EXISTS", A001_EXPECTATIONS_SNIPPETS),
        (args.a001_packet_doc, "M229-A002-A001-PKT-EXISTS", A001_PACKET_SNIPPETS),
        (args.architecture_doc, "M229-A002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M229-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M229-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M229-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, file_failures = check_file(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(file_failures)

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
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
