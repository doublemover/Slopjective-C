#!/usr/bin/env python3
"""Fail-closed checker for M256-B001 object-model semantic rule freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-b001-object-model-semantic-rules-contract-v1"
CONTRACT_ID = "objc3c-object-model-semantic-rules/m256-b001-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_object_model_semantic_rules_contract_and_architecture_freeze_b001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_b001_object_model_semantic_rules_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-B001" / "object_model_semantic_rules_contract_summary.json"

REALIZATION_MODEL = "interface-plus-implementation-pair-required-before-runtime-realization"
INHERITANCE_MODEL = "single-superclass-no-cycles-rooted-in-source-closure-parent-identities"
OVERRIDE_MODEL = "selector-kind-and-instance-class-ownership-must-remain-compatible-before-runtime-binding"
CONFORMANCE_MODEL = "declared-adoption-requires-required-member-coverage-optional-members-are-non-blocking"
CATEGORY_MERGE_MODEL = "deterministic-declaration-order-with-fail-closed-conflict-detection-before-runtime-installation"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M256-B001-DOC-EXP-01", "# M256 Object Model Semantic Rules Contract and Architecture Freeze Expectations (B001)"),
    SnippetCheck("M256-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-B001-DOC-EXP-03", f"`{REALIZATION_MODEL}`"),
    SnippetCheck("M256-B001-DOC-EXP-04", f"`{CATEGORY_MERGE_MODEL}`"),
    SnippetCheck("M256-B001-DOC-EXP-05", "`M256-B002`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M256-B001-DOC-PKT-01", "# M256-B001 Object Model Semantic Rules Contract and Architecture Freeze Packet"),
    SnippetCheck("M256-B001-DOC-PKT-02", "Packet: `M256-B001`"),
    SnippetCheck("M256-B001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M256-B001-DOC-PKT-04", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-B001-DOC-PKT-05", f"`{OVERRIDE_MODEL}`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M256-B001-NDOC-01", "## Object-model semantic rules (M256-B001)"),
    SnippetCheck("M256-B001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-B001-NDOC-03", f"`{CONFORMANCE_MODEL}`"),
    SnippetCheck("M256-B001-NDOC-04", "`M256-B002`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M256-B001-SPC-01", "## M256 object-model semantic rules (B001)"),
    SnippetCheck("M256-B001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-B001-SPC-03", f"`{INHERITANCE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M256-B001-META-01", "## M256 object-model semantic-rule metadata anchors (B001)"),
    SnippetCheck("M256-B001-META-02", f"`{REALIZATION_MODEL}`"),
    SnippetCheck("M256-B001-META-03", "IR anchor remains proof-only commentary"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M256-B001-ARCH-01", "## M256 object-model semantic rules (B001)"),
    SnippetCheck("M256-B001-ARCH-02", f"`{OVERRIDE_MODEL}`"),
    SnippetCheck("M256-B001-ARCH-03", "check:objc3c:m256-b001-lane-b-readiness"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M256-B001-PARSE-01", "M256-B001 object-model semantic-rule freeze anchor"),
    SnippetCheck("M256-B001-PARSE-02", "legality for realization, overrides, conformance, and category merge"),
    SnippetCheck("M256-B001-PARSE-03", "realization legality and merge behavior remain sema-owned decisions"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M256-B001-SEMA-01", "M256-B001 object-model semantic-rule freeze anchor"),
    SnippetCheck("M256-B001-SEMA-02", "realization legality, inheritance legality, override"),
    SnippetCheck("M256-B001-SEMA-03", "declared protocol conformance, and deterministic category"),
)
IR_SNIPPETS = (
    SnippetCheck("M256-B001-IR-01", "M256-B001 object-model semantic-rule freeze anchor"),
    SnippetCheck("M256-B001-IR-02", "IR stays proof-only"),
    SnippetCheck("M256-B001-IR-03", "executable enforcement begins in"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M256-B001-PKG-01", "\"check:objc3c:m256-b001-object-model-semantic-rules-contract-and-architecture-freeze\": \"python scripts/check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py\""),
    SnippetCheck("M256-B001-PKG-02", "\"test:tooling:m256-b001-object-model-semantic-rules-contract-and-architecture-freeze\": \"python -m pytest tests/tooling/test_check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py -q\""),
    SnippetCheck("M256-B001-PKG-03", "\"check:objc3c:m256-b001-lane-b-readiness\": \"npm run check:objc3c:m256-a003-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m256-b001-object-model-semantic-rules-contract-and-architecture-freeze && npm run test:tooling:m256-b001-object-model-semantic-rules-contract-and-architecture-freeze\""),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> tuple[int, int]:
    checks_total = len(snippets)
    if not path.exists():
        failures.append(Finding(display_path(path), "M256-B001-MISSING", f"required artifact is missing: {display_path(path)}"))
        return checks_total, 0
    text = path.read_text(encoding="utf-8")
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return checks_total, passed


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        total, passed = ensure_snippets(path, snippets, failures)
        checks_total += total
        checks_passed += passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[FAIL] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        print(f"Wrote fail-closed summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"M256-B001 contract verified. Summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
