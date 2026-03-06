#!/usr/bin/env python3
"""Fail-closed contract checker for the M229-A001 class/protocol/category metadata generation freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m229-a001-class-protocol-category-metadata-generation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m229_class_protocol_category_metadata_generation_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m229"
    / "m229_a001_class_protocol_category_metadata_generation_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_PARSE_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
DEFAULT_SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
DEFAULT_TYPED_SURFACE_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h"
)
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m229/M229-A001/class_protocol_category_metadata_generation_contract_summary.json"
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
        "M229-A001-DOC-EXP-01",
        "# M229 Class/Protocol/Category Metadata Generation Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M229-A001-DOC-EXP-02",
        "Contract ID: `objc3c-class-protocol-category-metadata-generation/m229-a001-v1`",
    ),
    SnippetCheck(
        "M229-A001-DOC-EXP-03",
        "Issue `#5301` defines canonical lane-A contract-freeze scope.",
    ),
    SnippetCheck(
        "M229-A001-DOC-EXP-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M229-A001-DOC-EXP-05",
        "`check:objc3c:m229-a001-class-protocol-category-metadata-generation-contract`",
    ),
    SnippetCheck(
        "M229-A001-DOC-EXP-06",
        "`check:objc3c:m229-a001-lane-a-readiness`",
    ),
    SnippetCheck(
        "M229-A001-DOC-EXP-07",
        "`tmp/reports/m229/M229-A001/class_protocol_category_metadata_generation_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-DOC-PKT-01",
        "# M229-A001 Class/Protocol/Category Metadata Generation Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M229-A001-DOC-PKT-02", "Packet: `M229-A001`"),
    SnippetCheck("M229-A001-DOC-PKT-03", "Issue: `#5301`"),
    SnippetCheck("M229-A001-DOC-PKT-04", "Dependencies: none"),
    SnippetCheck(
        "M229-A001-DOC-PKT-05",
        "`check:objc3c:m229-a001-class-protocol-category-metadata-generation-contract`",
    ),
    SnippetCheck(
        "M229-A001-DOC-PKT-06",
        "`test:objc3c:execution-smoke`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-ARCH-01",
        "M229 lane-A A001 class/protocol/category metadata generation anchors explicit",
    ),
    SnippetCheck(
        "M229-A001-ARCH-02",
        "docs/contracts/m229_class_protocol_category_metadata_generation_contract_and_architecture_freeze_a001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-SPC-01",
        "class/protocol/category metadata generation governance shall preserve explicit",
    ),
    SnippetCheck(
        "M229-A001-SPC-02",
        "deterministic lane-A boundary anchors and fail closed on class/protocol/category metadata linkage drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-META-01",
        "deterministic lane-A class/protocol/category metadata generation anchors for `M229-A001`",
    ),
    SnippetCheck(
        "M229-A001-META-02",
        "class/protocol/category metadata evidence and parser replay-budget continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-PKG-01",
        '"check:objc3c:m229-a001-class-protocol-category-metadata-generation-contract": '
        '"python scripts/check_m229_a001_class_protocol_category_metadata_generation_contract.py"',
    ),
    SnippetCheck(
        "M229-A001-PKG-02",
        '"test:tooling:m229-a001-class-protocol-category-metadata-generation-contract": '
        '"python -m pytest tests/tooling/test_check_m229_a001_class_protocol_category_metadata_generation_contract.py -q"',
    ),
    SnippetCheck(
        "M229-A001-PKG-03",
        '"check:objc3c:m229-a001-lane-a-readiness": '
        '"npm run check:objc3c:m229-a001-class-protocol-category-metadata-generation-contract '
        '&& npm run test:tooling:m229-a001-class-protocol-category-metadata-generation-contract"',
    ),
    SnippetCheck("M229-A001-PKG-04", '"test:objc3c:parser-ast-extraction": '),
    SnippetCheck("M229-A001-PKG-05", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M229-A001-PKG-06", '"test:objc3c:execution-smoke": '),
)

PARSE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M229-A001-PARSE-01", "static std::string BuildObjcCategorySemanticLinkSymbol("),
    SnippetCheck("M229-A001-PARSE-02", "std::unique_ptr<Objc3ProtocolDecl> ParseObjcProtocolDecl()"),
    SnippetCheck("M229-A001-PARSE-03", "std::unique_ptr<Objc3InterfaceDecl> ParseObjcInterfaceDecl()"),
    SnippetCheck("M229-A001-PARSE-04", "std::unique_ptr<Objc3ImplementationDecl> ParseObjcImplementationDecl()"),
    SnippetCheck(
        "M229-A001-PARSE-05",
        "decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);",
    ),
)

SEMA_MANAGER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-SEMA-01",
        "result.class_protocol_category_linking_summary = result.integration_surface.class_protocol_category_linking_summary;",
    ),
    SnippetCheck("M229-A001-SEMA-02", "result.deterministic_class_protocol_category_linking_handoff ="),
    SnippetCheck("M229-A001-SEMA-03", "result.parity_surface.class_protocol_category_linking_summary ="),
    SnippetCheck(
        "M229-A001-SEMA-04",
        "result.parity_surface.deterministic_class_protocol_category_linking_handoff =",
    ),
    SnippetCheck(
        "M229-A001-SEMA-05",
        "result.parity_surface.class_protocol_category_linking_summary.total_composition_sites() &&",
    ),
)

SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-SEMA-CON-01",
        "bool deterministic_class_protocol_category_linking_handoff = false;",
    ),
    SnippetCheck(
        "M229-A001-SEMA-CON-02",
        "Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;",
    ),
    SnippetCheck(
        "M229-A001-SEMA-CON-03",
        "surface.deterministic_class_protocol_category_linking_handoff &&",
    ),
)

TYPED_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-TYPED-01",
        "surface.class_protocol_category_linking_handoff_deterministic =",
    ),
    SnippetCheck(
        "M229-A001-TYPED-02",
        "pipeline_result.class_protocol_category_linking_summary",
    ),
    SnippetCheck(
        "M229-A001-TYPED-03",
        ".deterministic_class_protocol_category_linking_handoff;",
    ),
    SnippetCheck(
        "M229-A001-TYPED-04",
        "} else if (!surface.class_protocol_category_linking_handoff_deterministic) {",
    ),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-ART-01",
        "deterministic_class_protocol_category_linking_handoff",
    ),
    SnippetCheck(
        "M229-A001-ART-02",
        "ir_frontend_metadata.declared_class_interfaces = class_protocol_category_linking_summary.declared_class_interfaces;",
    ),
    SnippetCheck(
        "M229-A001-ART-03",
        "ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =",
    ),
    SnippetCheck(
        "M229-A001-ART-04",
        "class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff;",
    ),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A001-IR-01",
        "; frontend_objc_class_protocol_category_linking_profile = declared_class_interfaces=",
    ),
    SnippetCheck(
        "M229-A001-IR-02",
        ", deterministic_class_protocol_category_linking_handoff=",
    ),
    SnippetCheck(
        "M229-A001-IR-03",
        "frontend_metadata_.deterministic_class_protocol_category_linking_handoff ? \"true\" : \"false\"",
    ),
    SnippetCheck(
        "M229-A001-IR-04",
        "!objc3.objc_class_protocol_category_linking = !{!7}",
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--parse-source", type=Path, default=DEFAULT_PARSE_SOURCE)
    parser.add_argument("--sema-pass-manager", type=Path, default=DEFAULT_SEMA_PASS_MANAGER)
    parser.add_argument("--sema-contract-header", type=Path, default=DEFAULT_SEMA_CONTRACT_HEADER)
    parser.add_argument("--typed-surface-header", type=Path, default=DEFAULT_TYPED_SURFACE_HEADER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_file_contract(
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M229-A001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M229-A001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M229-A001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M229-A001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M229-A001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M229-A001-PKG-EXISTS", PACKAGE_SNIPPETS),
        (args.parse_source, "M229-A001-PARSE-EXISTS", PARSE_SNIPPETS),
        (args.sema_pass_manager, "M229-A001-SEMA-EXISTS", SEMA_MANAGER_SNIPPETS),
        (args.sema_contract_header, "M229-A001-SEMA-CON-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.typed_surface_header, "M229-A001-TYPED-EXISTS", TYPED_SURFACE_SNIPPETS),
        (args.frontend_artifacts, "M229-A001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter, "M229-A001-IR-EXISTS", IR_EMITTER_SNIPPETS),
    ):
        count, findings = check_file_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
