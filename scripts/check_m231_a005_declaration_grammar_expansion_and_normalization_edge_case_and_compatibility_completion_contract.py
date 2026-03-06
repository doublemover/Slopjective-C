#!/usr/bin/env python3
"""Fail-closed checker for M231-A005 declaration grammar expansion/normalization edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m231_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_a005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m231"
    / "m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_A004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m231_declaration_grammar_expansion_and_normalization_core_feature_expansion_a004_expectations.md"
)
DEFAULT_A004_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m231"
    / "m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_packet.md"
)
DEFAULT_A004_CHECKER = (
    ROOT / "scripts" / "check_m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_contract.py"
)
DEFAULT_A004_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m231/M231-A005/declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_summary.json"
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


PREREQUISITE_ASSETS: tuple[tuple[str, Path], ...] = (
    ("M231-A005-DEP-A004-01", Path("docs/contracts/m231_declaration_grammar_expansion_and_normalization_core_feature_expansion_a004_expectations.md")),
    ("M231-A005-DEP-A004-02", Path("spec/planning/compiler/m231/m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_packet.md")),
    ("M231-A005-DEP-A004-03", Path("scripts/check_m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_contract.py")),
    ("M231-A005-DEP-A004-04", Path("tests/tooling/test_check_m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_contract.py")),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-DOC-EXP-01", "# M231 Declaration Grammar Expansion and Normalization Edge-case and Compatibility Completion Expectations (A005)"),
    SnippetCheck("M231-A005-DOC-EXP-02", "Contract ID: `objc3c-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion/m231-a005-v1`"),
    SnippetCheck("M231-A005-DOC-EXP-03", "Dependencies: `M231-A004`"),
    SnippetCheck("M231-A005-DOC-EXP-04", "code/spec anchors and milestone optimization improvements as mandatory scope inputs."),
    SnippetCheck("M231-A005-DOC-EXP-05", "check:objc3c:m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract"),
    SnippetCheck("M231-A005-DOC-EXP-06", "check:objc3c:m231-a005-lane-a-readiness"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-DOC-PKT-01", "# M231-A005 Declaration Grammar Expansion and Normalization Edge-case and Compatibility Completion Packet"),
    SnippetCheck("M231-A005-DOC-PKT-02", "Packet: `M231-A005`"),
    SnippetCheck("M231-A005-DOC-PKT-03", "Dependencies: `M231-A004`"),
    SnippetCheck("M231-A005-DOC-PKT-04", "code/spec anchors and milestone optimization improvements as mandatory scope inputs."),
    SnippetCheck("M231-A005-DOC-PKT-05", "spec/planning/compiler/m231/m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_packet.md"),
)

A004_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-A004-EXP-01", "# M231 Declaration Grammar Expansion and Normalization Core Feature Expansion Expectations (A004)"),
    SnippetCheck("M231-A005-A004-EXP-02", "Contract ID: `objc3c-declaration-grammar-expansion-and-normalization-core-feature-expansion/m231-a004-v1`"),
)

A004_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-A004-PKT-01", "Packet: `M231-A004`"),
    SnippetCheck("M231-A005-A004-PKT-02", "Dependencies: `M231-A003`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-ARCH-01", "M231 lane-A A005 declaration grammar expansion and normalization edge-case and compatibility completion anchors"),
    SnippetCheck("M231-A005-ARCH-02", "m231_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_a005_expectations.md"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-SPC-01", "declaration grammar expansion and normalization edge-case and compatibility completion governance shall preserve explicit"),
    SnippetCheck("M231-A005-SPC-02", "lane-A dependency anchors (`M231-A004`) and fail closed on core-feature evidence drift"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M231-A005-META-01", "deterministic lane-A declaration grammar expansion and normalization core feature anchors for `M231-A005`"),
    SnippetCheck(
        "M231-A005-META-02",
        "explicit `M231-A004` dependency continuity so declaration grammar expansion/normalization core-feature drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M231-A005-PKG-01",
        '"check:objc3c:m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract": '
        '"python scripts/check_m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_contract.py"',
    ),
    SnippetCheck(
        "M231-A005-PKG-02",
        '"test:tooling:m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract": '
        '"python -m pytest tests/tooling/test_check_m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_contract.py -q"',
    ),
    SnippetCheck(
        "M231-A005-PKG-03",
        '"check:objc3c:m231-a005-lane-a-readiness": '
        '"npm run check:objc3c:m231-a004-lane-a-readiness '
        '&& npm run check:objc3c:m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract '
        '&& npm run test:tooling:m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract"',
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
    parser.add_argument("--a004-expectations-doc", type=Path, default=DEFAULT_A004_EXPECTATIONS_DOC)
    parser.add_argument("--a004-packet-doc", type=Path, default=DEFAULT_A004_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_file(path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required file is missing: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def check_prerequisites() -> tuple[int, list[Finding]]:
    checks_total = len(PREREQUISITE_ASSETS)
    findings: list[Finding] = []
    for check_id, rel in PREREQUISITE_ASSETS:
        target = ROOT / rel
        if not target.exists() or not target.is_file():
            findings.append(Finding(rel.as_posix(), check_id, f"missing required dependency artifact: {rel.as_posix()}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    count, findings = check_prerequisites()
    checks_total += count
    failures.extend(findings)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M231-A005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M231-A005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a004_expectations_doc, "M231-A005-A004-EXP-EXISTS", A004_EXPECTATIONS_SNIPPETS),
        (args.a004_packet_doc, "M231-A005-A004-PKT-EXISTS", A004_PACKET_SNIPPETS),
        (args.architecture_doc, "M231-A005-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M231-A005-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M231-A005-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M231-A005-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_file(path, exists_check_id, snippets)
        checks_total += count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    payload = {
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
    summary_path.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        for f in failures:
            print(f"[{f.check_id}] {f.artifact}: {f.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))




