#!/usr/bin/env python3
"""Fail-closed checker for M256-A001 executable class/protocol/category source closure."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-a001-executable-class-protocol-category-source-closure-contract-v1"
CONTRACT_ID = "objc3c-executable-class-protocol-category-source-closure/m256-a001-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_executable_class_protocol_category_source_closure_contract_and_architecture_freeze_a001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_a001_executable_class_protocol_category_source_closure_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-A001" / "executable_class_protocol_category_source_closure_contract_summary.json"


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
    SnippetCheck("M256-A001-DOC-EXP-01", "# M256 Executable Class Protocol Category Source Closure Contract and Architecture Freeze Expectations (A001)"),
    SnippetCheck("M256-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-A001-DOC-EXP-03", "`interface_implementation_summary`"),
    SnippetCheck("M256-A001-DOC-EXP-04", "`protocol_category_composition_summary`"),
    SnippetCheck("M256-A001-DOC-EXP-05", "`class_protocol_category_linking_summary`"),
    SnippetCheck("M256-A001-DOC-EXP-06", "`tmp/reports/m256/M256-A001/executable_class_protocol_category_source_closure_contract_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M256-A001-DOC-PKT-01", "# M256-A001 Executable Class Protocol Category Source Closure Contract and Architecture Freeze Packet"),
    SnippetCheck("M256-A001-DOC-PKT-02", "Packet: `M256-A001`"),
    SnippetCheck("M256-A001-DOC-PKT-03", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-A001-DOC-PKT-04", "`!objc3.objc_class_protocol_category_linking`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M256-A001-NDOC-01", "## Executable class/protocol/category source closure (M256-A001)"),
    SnippetCheck("M256-A001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-A001-NDOC-03", "`interface_implementation_summary`"),
    SnippetCheck("M256-A001-NDOC-04", "`M256-A002`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M256-A001-SPC-01", "## M256 executable class/protocol/category source closure (A001)"),
    SnippetCheck("M256-A001-SPC-02", "`!objc3.objc_interface_implementation`"),
    SnippetCheck("M256-A001-SPC-03", "category attachment ownership"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M256-A001-META-01", "## M256 executable class/protocol/category source-closure metadata anchors (A001)"),
    SnippetCheck("M256-A001-META-02", "`!objc3.objc_protocol_category`"),
    SnippetCheck("M256-A001-META-03", "`tmp/reports/m256/M256-A001/executable_class_protocol_category_source_closure_contract_summary.json`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M256-A001-ARCH-01", "## M256 executable class/protocol/category source closure (A001)"),
    SnippetCheck("M256-A001-ARCH-02", "docs/contracts/m256_executable_class_protocol_category_source_closure_contract_and_architecture_freeze_a001_expectations.md"),
    SnippetCheck("M256-A001-ARCH-03", "later `M256` implementation issues must preserve these identities"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M256-A001-PARSE-01", "M256-A001 executable source-closure freeze anchor"),
    SnippetCheck("M256-A001-PARSE-02", 'BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name)'),
    SnippetCheck("M256-A001-PARSE-03", "decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(decl->adopted_protocols);"),
    SnippetCheck("M256-A001-PARSE-04", 'BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name)'),
)
SEMA_SNIPPETS = (
    SnippetCheck("M256-A001-SEMA-01", "M256-A001 executable source-closure freeze anchor"),
    SnippetCheck("M256-A001-SEMA-02", "surface.interface_implementation_summary = interface_implementation_summary;"),
    SnippetCheck("M256-A001-SEMA-03", "surface.protocol_category_composition_summary = BuildProtocolCategoryCompositionSummaryFromSurface(surface);"),
    SnippetCheck("M256-A001-SEMA-04", "surface.class_protocol_category_linking_summary ="),
)
IR_SNIPPETS = (
    SnippetCheck("M256-A001-IR-01", "M256-A001 executable source-closure freeze anchor"),
    SnippetCheck("M256-A001-IR-02", 'out << "!objc3.objc_interface_implementation = !{!1}\\n";'),
    SnippetCheck("M256-A001-IR-03", 'out << "!objc3.objc_protocol_category = !{!2}\\n";'),
    SnippetCheck("M256-A001-IR-04", 'out << "!objc3.objc_class_protocol_category_linking = !{!7}\\n";'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M256-A001-PKG-01", '"check:objc3c:m256-a001-executable-class-protocol-category-source-closure-contract": "python scripts/check_m256_a001_executable_class_protocol_category_source_closure_contract.py"'),
    SnippetCheck("M256-A001-PKG-02", '"test:tooling:m256-a001-executable-class-protocol-category-source-closure-contract": "python -m pytest tests/tooling/test_check_m256_a001_executable_class_protocol_category_source_closure_contract.py -q"'),
    SnippetCheck("M256-A001-PKG-03", '"check:objc3c:m256-a001-lane-a-readiness": "npm run check:objc3c:m256-a001-executable-class-protocol-category-source-closure-contract && npm run test:tooling:m256-a001-executable-class-protocol-category-source-closure-contract"'),
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


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M256-A001-MISSING", f"required artifact is missing: {display_path(path)}"))
        return 0
    text = path.read_text(encoding="utf-8")
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks = (
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
    )
    for path, snippets in checks:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

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
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"summary_path: {display_path(args.summary_out)}")
    print("status: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
