#!/usr/bin/env python3
"""Fail-closed contract checker for M253-A001 emitted metadata inventory freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-a001-emitted-metadata-inventory-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m253_emitted_metadata_inventory_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m253"
    / "m253_a001_emitted_metadata_inventory_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-A001/emitted_metadata_inventory_contract_summary.json")

CONTRACT_ID = "objc3c-emitted-metadata-inventory-freeze/m253-a001-v1"
CANONICAL_SECTIONS = [
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
]
SYMBOL_POLICIES = {
    "descriptor_symbol_prefix": "__objc3_meta_",
    "aggregate_symbol_prefix": "__objc3_sec_",
    "image_info_symbol": "__objc3_image_info",
    "descriptor_linkage": "private",
    "aggregate_linkage": "internal",
    "metadata_visibility": "hidden",
    "retention_root": "llvm.used",
}


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
        "M253-A001-DOC-EXP-01",
        "# M253 Emitted Metadata Inventory Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck("M253-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-A001-DOC-EXP-03", "`objc3.runtime.image_info`"),
    SnippetCheck("M253-A001-DOC-EXP-04", "`objc3.runtime.class_descriptors`"),
    SnippetCheck("M253-A001-DOC-EXP-05", "`objc3.runtime.protocol_descriptors`"),
    SnippetCheck("M253-A001-DOC-EXP-06", "`objc3.runtime.category_descriptors`"),
    SnippetCheck("M253-A001-DOC-EXP-07", "`objc3.runtime.property_descriptors`"),
    SnippetCheck("M253-A001-DOC-EXP-08", "`objc3.runtime.ivar_descriptors`"),
    SnippetCheck("M253-A001-DOC-EXP-09", "`__objc3_meta_`"),
    SnippetCheck("M253-A001-DOC-EXP-10", "`__objc3_sec_`"),
    SnippetCheck("M253-A001-DOC-EXP-11", "`__objc3_image_info`"),
    SnippetCheck("M253-A001-DOC-EXP-12", "`llvm.used`"),
    SnippetCheck("M253-A001-DOC-EXP-13", "`M253-A002`"),
    SnippetCheck(
        "M253-A001-DOC-EXP-14",
        "`tmp/reports/m253/M253-A001/emitted_metadata_inventory_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-DOC-PKT-01",
        "# M253-A001 Emitted Metadata Inventory Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M253-A001-DOC-PKT-02", "Packet: `M253-A001`"),
    SnippetCheck("M253-A001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M253-A001-DOC-PKT-04", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-A001-DOC-PKT-05", "`llvm-readobj --sections module.obj`"),
    SnippetCheck("M253-A001-DOC-PKT-06", "`llvm-objdump --syms module.obj`"),
    SnippetCheck("M253-A001-DOC-PKT-07", "`M253-A002`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-ARCH-01",
        "M253 lane-A A001 emitted metadata inventory freeze anchors explicit contract",
    ),
    SnippetCheck(
        "M253-A001-ARCH-02",
        "docs/contracts/m253_emitted_metadata_inventory_contract_and_architecture_freeze_a001_expectations.md",
    ),
    SnippetCheck("M253-A001-ARCH-03", "`M253-A002`"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-A001-NDOC-01", "## Emitted metadata inventory freeze (M253-A001)"),
    SnippetCheck("M253-A001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-A001-NDOC-03", "`__objc3_meta_`"),
    SnippetCheck("M253-A001-NDOC-04", "`llvm.used`"),
    SnippetCheck("M253-A001-NDOC-05", "no standalone emitted method/selector/string-pool sections yet"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-A001-SPC-01", "## M253 emitted metadata inventory freeze (A001)"),
    SnippetCheck("M253-A001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-A001-SPC-03", "`__objc3_image_info`"),
    SnippetCheck("M253-A001-SPC-04", "method/selector/string-pool emitted sections"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-A001-META-01", "## M253 emitted metadata inventory metadata anchors (A001)"),
    SnippetCheck("M253-A001-META-02", "`objc3.runtime.class_descriptors`"),
    SnippetCheck("M253-A001-META-03", "`llvm-readobj --sections module.obj`"),
    SnippetCheck(
        "M253-A001-META-04",
        "`tmp/reports/m253/M253-A001/emitted_metadata_inventory_contract_summary.json`",
    ),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-AST-01",
        "M253-A001 emitted metadata inventory freeze anchor: these constants define",
    ),
    SnippetCheck("M253-A001-AST-02", 'inline constexpr const char *kObjc3RuntimeMetadataLogicalImageInfoSection ='),
    SnippetCheck("M253-A001-AST-03", '"objc3.runtime.class_descriptors"'),
    SnippetCheck("M253-A001-AST-04", '"__objc3_meta_"'),
    SnippetCheck("M253-A001-AST-05", '"__objc3_sec_"'),
    SnippetCheck("M253-A001-AST-06", '"__objc3_image_info"'),
    SnippetCheck("M253-A001-AST-07", '"private"'),
    SnippetCheck("M253-A001-AST-08", '"internal"'),
    SnippetCheck("M253-A001-AST-09", '"hidden"'),
    SnippetCheck("M253-A001-AST-10", '"llvm.used"'),
    SnippetCheck("M253-A001-AST-11", '"llvm-readobj --sections module.obj"'),
    SnippetCheck("M253-A001-AST-12", '"llvm-objdump --syms module.obj"'),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-LHDR-01",
        "M253-A001 emitted metadata inventory freeze anchor: lowering contracts do",
    ),
    SnippetCheck("M253-A001-LHDR-02", "frontend ABI/scaffold/object-inspection boundary"),
    SnippetCheck("M253-A001-LHDR-03", "plus class/protocol/category/property/ivar descriptor sections"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-LCPP-01",
        "M253-A001 emitted metadata inventory freeze anchor: replay keys here",
    ),
    SnippetCheck("M253-A001-LCPP-02", "Runtime metadata section inventory"),
    SnippetCheck("M253-A001-LCPP-03", "frontend ABI/scaffold summaries"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-IR-01",
        "M253-A001 emitted metadata inventory freeze anchor: the currently",
    ),
    SnippetCheck("M253-A001-IR-02", "image-info plus class/protocol/category/"),
    SnippetCheck("M253-A001-IR-03", "property/ivar descriptor sections retained via llvm.used"),
    SnippetCheck("M253-A001-IR-04", "section families remain explicit non-goals"),
    SnippetCheck("M253-A001-IR-05", '!objc3.objc_runtime_metadata_section_abi = !{!48}'),
    SnippetCheck("M253-A001-IR-06", '!objc3.objc_runtime_metadata_section_scaffold = !{!49}'),
    SnippetCheck("M253-A001-IR-07", '!objc3.objc_runtime_metadata_object_inspection = !{!50}'),
    SnippetCheck("M253-A001-IR-08", '@llvm.used = appending global ['),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-PROC-01",
        "M253-A001 emitted metadata inventory freeze anchor: the llvm-direct path",
    ),
    SnippetCheck("M253-A001-PROC-02", "preserve the IR-emitted runtime metadata section inventory exactly"),
    SnippetCheck("M253-A001-PROC-03", "may not rewrite, rename, or silently substitute"),
    SnippetCheck("M253-A001-PROC-04", "RunIRCompileLLVMDirect("),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-A001-FT-01", "struct Objc3RuntimeMetadataSectionAbiFreezeSummary {"),
    SnippetCheck("M253-A001-FT-02", "struct Objc3RuntimeMetadataSectionScaffoldSummary {"),
    SnippetCheck("M253-A001-FT-03", "struct Objc3RuntimeMetadataObjectInspectionHarnessSummary {"),
    SnippetCheck("M253-A001-FT-04", "std::string logical_class_descriptor_section ="),
    SnippetCheck("M253-A001-FT-05", "std::string descriptor_linkage ="),
    SnippetCheck("M253-A001-FT-06", "std::string retention_root = kObjc3RuntimeMetadataRetentionPolicyRoot;"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-A001-PKG-01",
        '"check:objc3c:m253-a001-emitted-metadata-inventory-contract": "python scripts/check_m253_a001_emitted_metadata_inventory_contract.py"',
    ),
    SnippetCheck(
        "M253-A001-PKG-02",
        '"test:tooling:m253-a001-emitted-metadata-inventory-contract": "python -m pytest tests/tooling/test_check_m253_a001_emitted_metadata_inventory_contract.py -q"',
    ),
    SnippetCheck(
        "M253-A001-PKG-03",
        '"check:objc3c:m253-a001-lane-a-readiness": "npm run build:objc3c-native && npm run check:objc3c:m253-a001-emitted-metadata-inventory-contract && npm run test:tooling:m253-a001-emitted-metadata-inventory-contract"',
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    return str(path.resolve())


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def apply_snippet_checks(
    artifact_label: str, text: str, checks: Sequence[SnippetCheck], failures: list[Finding]
) -> int:
    passed = 0
    for check in checks:
        if check.snippet in text:
            passed += 1
        else:
            failures.append(
                Finding(
                    artifact=artifact_label,
                    check_id=check.check_id,
                    detail=f"Missing snippet: {check.snippet}",
                )
            )
    return passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifacts = (
        ("expectations_doc", args.expectations_doc, EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, PACKET_SNIPPETS),
        ("architecture_doc", args.architecture_doc, ARCHITECTURE_SNIPPETS),
        ("native_doc", args.native_doc, NATIVE_DOC_SNIPPETS),
        ("lowering_spec", args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, METADATA_SPEC_SNIPPETS),
        ("ast_header", args.ast_header, AST_SNIPPETS),
        ("lowering_header", args.lowering_header, LOWERING_HEADER_SNIPPETS),
        ("lowering_cpp", args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        ("ir_emitter", args.ir_emitter, IR_EMITTER_SNIPPETS),
        ("process_cpp", args.process_cpp, PROCESS_SNIPPETS),
        ("frontend_types", args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        ("package_json", args.package_json, PACKAGE_SNIPPETS),
    )

    for label, path, snippet_checks in artifacts:
        text = read_text(path)
        checks_total += len(snippet_checks)
        checks_passed += apply_snippet_checks(label, text, snippet_checks, failures)

    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "canonical_sections": CANONICAL_SECTIONS,
        "symbol_policies": SYMBOL_POLICIES,
        "non_goals": [
            "no completeness matrix yet",
            "no concrete descriptor payload layout beyond scaffold",
            "no startup registration",
            "no standalone emitted method/selector/string-pool sections",
        ],
        "next_implementation_issue": "M253-A002",
        "evidence_path": str(args.summary_out).replace('\\', '/'),
        "artifacts": {label: display_path(path) for label, path, _ in artifacts},
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
