#!/usr/bin/env python3
"""Fail-closed contract checker for M252-A001 executable metadata source graph freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-a001-executable-metadata-source-graph-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m252_executable_metadata_source_graph_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m252"
    / "m252_a001_executable_metadata_source_graph_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-A001/executable_metadata_source_graph_contract_summary.json")


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
        "M252-A001-DOC-EXP-01",
        "# M252 Executable Metadata Source Graph Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M252-A001-DOC-EXP-02",
        "Contract ID: `objc3c-executable-metadata-source-graph-freeze/m252-a001-v1`",
    ),
    SnippetCheck(
        "M252-A001-DOC-EXP-03",
        "`semantic-link-symbol-lexicographic-owner-identity`",
    ),
    SnippetCheck(
        "M252-A001-DOC-EXP-04",
        "`metaclass-nodes-derived-from-resolved-interface-symbols`",
    ),
    SnippetCheck(
        "M252-A001-DOC-EXP-05",
        "`objc_executable_metadata_source_graph`",
    ),
    SnippetCheck(
        "M252-A001-DOC-EXP-06",
        "`tmp/reports/m252/M252-A001/executable_metadata_source_graph_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-DOC-PKT-01",
        "# M252-A001 Executable Metadata Source Graph Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M252-A001-DOC-PKT-02", "Packet: `M252-A001`"),
    SnippetCheck("M252-A001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck(
        "M252-A001-DOC-PKT-04",
        "`semantic-link-symbol-lexicographic-owner-identity`",
    ),
    SnippetCheck(
        "M252-A001-DOC-PKT-05",
        "`objc_executable_metadata_source_graph`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-ARCH-01",
        "M252 lane-A A001 executable metadata source graph freeze anchors explicit",
    ),
    SnippetCheck(
        "M252-A001-ARCH-02",
        "docs/contracts/m252_executable_metadata_source_graph_contract_and_architecture_freeze_a001_expectations.md",
    ),
    SnippetCheck(
        "M252-A001-ARCH-03",
        "fail-closed before",
    ),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-NDOC-01",
        "## Executable metadata source graph freeze (M252-A001)",
    ),
    SnippetCheck(
        "M252-A001-NDOC-02",
        "`semantic-link-symbol-lexicographic-owner-identity`",
    ),
    SnippetCheck(
        "M252-A001-NDOC-03",
        "`metaclass-nodes-derived-from-resolved-interface-symbols`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-SPC-01",
        "## M252 executable metadata source graph freeze (A001)",
    ),
    SnippetCheck(
        "M252-A001-SPC-02",
        "lexicographic owner/edge ordering",
    ),
    SnippetCheck(
        "M252-A001-SPC-03",
        "`metaclass-nodes-derived-from-resolved-interface-symbols`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-META-01",
        "## M252 executable metadata source graph metadata anchors (A001)",
    ),
    SnippetCheck(
        "M252-A001-META-02",
        "`objc_executable_metadata_source_graph`",
    ),
    SnippetCheck(
        "M252-A001-META-03",
        "`tmp/reports/m252/M252-A001/executable_metadata_source_graph_contract_summary.json`",
    ),
)

PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-PARSE-01",
        "M252-A001 freeze: semantic-link symbols remain the canonical owner identities",
    ),
    SnippetCheck(
        "M252-A001-PARSE-02",
        "static std::string BuildObjcContainerScopeOwner(",
    ),
    SnippetCheck(
        "M252-A001-PARSE-03",
        'return "category:" + owner_name + "(" + category_name + ")";',
    ),
    SnippetCheck(
        "M252-A001-PARSE-04",
        "AssignObjcPropertySynthesisIvarBindingSymbols(property, decl->semantic_link_symbol);",
    ),
)

SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-SEMA-01",
        "struct Objc3InterfaceImplementationSummary {",
    ),
    SnippetCheck(
        "M252-A001-SEMA-02",
        "struct Objc3ProtocolCategoryCompositionSummary {",
    ),
    SnippetCheck(
        "M252-A001-SEMA-03",
        "struct Objc3ClassProtocolCategoryLinkingSummary {",
    ),
    SnippetCheck(
        "M252-A001-SEMA-04",
        "struct Objc3PropertySynthesisIvarBindingSummary {",
    ),
    SnippetCheck(
        "M252-A001-SEMA-05",
        "Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;",
    ),
)

SEMA_PASSES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-PASS-01",
        "surface.protocol_category_composition_summary = BuildProtocolCategoryCompositionSummaryFromSurface(surface);",
    ),
    SnippetCheck(
        "M252-A001-PASS-02",
        "surface.class_protocol_category_linking_summary =",
    ),
    SnippetCheck(
        "M252-A001-PASS-03",
        "handoff.class_protocol_category_linking_summary.deterministic &&",
    ),
    SnippetCheck(
        "M252-A001-PASS-04",
        "handoff.protocol_category_composition_summary.deterministic &&",
    ),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-ART-01",
        "BuildExecutableMetadataSourceGraphJson(",
    ),
    SnippetCheck(
        "M252-A001-ART-02",
        'const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph =',
    ),
    SnippetCheck(
        "M252-A001-ART-03",
        'graph.owner_identity_model',
    ),
    SnippetCheck(
        "M252-A001-ART-04",
        'graph.ready_for_lowering',
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-A001-PKG-01",
        '"check:objc3c:m252-a001-executable-metadata-source-graph-contract": "python scripts/check_m252_a001_executable_metadata_source_graph_contract.py"',
    ),
    SnippetCheck(
        "M252-A001-PKG-02",
        '"test:tooling:m252-a001-executable-metadata-source-graph-contract": "python -m pytest tests/tooling/test_check_m252_a001_executable_metadata_source_graph_contract.py -q"',
    ),
    SnippetCheck(
        "M252-A001-PKG-03",
        '"check:objc3c:m252-a001-lane-a-readiness": "npm run check:objc3c:m252-a001-executable-metadata-source-graph-contract && npm run test:tooling:m252-a001-executable-metadata-source-graph-contract"',
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
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
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

    checks = (
        (args.expectations_doc, "M252-A001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-A001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-A001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-A001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-A001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-A001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-A001-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-A001-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-A001-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_artifacts, "M252-A001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, "M252-A001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )

    for path, exists_check_id, snippets in checks:
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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
