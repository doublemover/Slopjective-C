#!/usr/bin/env python3
"""Fail-closed contract checker for the M227-B001 ObjC3 type-system freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-b001-type-system-objc3-forms-contract-and-architecture-freeze-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_contract_and_architecture_freeze_b001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b001_type_system_objc3_forms_contract_and_architecture_freeze_packet.md"
)
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m227/M227-B001/type_system_objc3_forms_contract_and_architecture_freeze_summary.json"
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
        "M227-B001-DOC-EXP-01",
        "# M227 Type-System Completeness for ObjC3 Forms Contract and Architecture Freeze Expectations (B001)",
    ),
    SnippetCheck(
        "M227-B001-DOC-EXP-02",
        "Contract ID: `objc3c-type-system-objc3-forms-contract-and-architecture-freeze/m227-b001-v1`",
    ),
    SnippetCheck(
        "M227-B001-DOC-EXP-03",
        "Issue `#4842` defines canonical lane-B contract freeze scope.",
    ),
    SnippetCheck("M227-B001-DOC-EXP-04", "Dependencies: none"),
    SnippetCheck(
        "M227-B001-DOC-EXP-05",
        "m227_b001_type_system_objc3_forms_contract_and_architecture_freeze_packet.md",
    ),
    SnippetCheck(
        "M227-B001-DOC-EXP-06",
        "scripts/check_m227_b001_type_system_objc3_forms_contract.py",
    ),
    SnippetCheck(
        "M227-B001-DOC-EXP-07",
        "tests/tooling/test_check_m227_b001_type_system_objc3_forms_contract.py",
    ),
    SnippetCheck(
        "M227-B001-DOC-EXP-08",
        "`check:objc3c:m227-b001-lane-b-readiness`",
    ),
    SnippetCheck(
        "M227-B001-DOC-EXP-09",
        "`tmp/reports/m227/M227-B001/type_system_objc3_forms_contract_and_architecture_freeze_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-B001-DOC-PKT-01",
        "# M227-B001 Type-System Completeness for ObjC3 Forms Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M227-B001-DOC-PKT-02", "Packet: `M227-B001`"),
    SnippetCheck("M227-B001-DOC-PKT-03", "Issue: `#4842`"),
    SnippetCheck("M227-B001-DOC-PKT-04", "Freeze date: `2026-03-03`"),
    SnippetCheck("M227-B001-DOC-PKT-05", "Dependencies: none"),
    SnippetCheck(
        "M227-B001-DOC-PKT-06",
        "`check:objc3c:m227-b001-type-system-objc3-forms-contract`",
    ),
    SnippetCheck(
        "M227-B001-DOC-PKT-07",
        "`check:objc3c:m227-b001-lane-b-readiness`",
    ),
    SnippetCheck("M227-B001-DOC-PKT-08", "`compile:objc3c`"),
)

SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-B001-SEM-01", "kObjc3CanonicalReferenceTypeForms"),
    SnippetCheck("M227-B001-SEM-02", "kObjc3CanonicalScalarMessageSendTypeForms"),
    SnippetCheck("M227-B001-SEM-03", "kObjc3CanonicalBridgeTopReferenceTypeForms"),
    SnippetCheck("M227-B001-SEM-04", "inline bool IsObjc3CanonicalReferenceTypeForm(ValueType type) {"),
    SnippetCheck("M227-B001-SEM-05", "inline bool IsObjc3CanonicalMessageSendTypeForm(ValueType type) {"),
    SnippetCheck("M227-B001-SEM-06", "inline bool IsObjc3CanonicalBridgeTopReferenceTypeForm(ValueType type) {"),
)

SEMANTIC_PASSES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-B001-PSS-01", "return IsObjc3CanonicalReferenceTypeForm(type);"),
    SnippetCheck("M227-B001-PSS-02", "return !info.is_vector && IsObjc3CanonicalMessageSendTypeForm(info.type);"),
    SnippetCheck("M227-B001-PSS-03", "IsObjc3CanonicalBridgeTopReferenceTypeForm(target.type) ||"),
    SnippetCheck("M227-B001-PSS-04", "IsObjc3CanonicalBridgeTopReferenceTypeForm(value.type)) {"),
    SnippetCheck("M227-B001-PSS-05", "if (!IsCanonicalObjc3TypeFormScaffoldReady()) {"),
)

SEMANTIC_PASSES_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-B001-PSS-FORB-01", "target.type == ValueType::ObjCId || value.type == ValueType::ObjCId"),
    SnippetCheck(
        "M227-B001-PSS-FORB-02",
        "target.type == ValueType::ObjCInstancetype || value.type == ValueType::ObjCInstancetype",
    ),
    SnippetCheck(
        "M227-B001-PSS-FORB-03",
        "target.type == ValueType::ObjCObjectPtr || value.type == ValueType::ObjCObjectPtr",
    ),
    SnippetCheck("M227-B001-PSS-FORB-04", "target.type == ValueType::ObjCClass || value.type == ValueType::ObjCClass"),
    SnippetCheck(
        "M227-B001-PSS-FORB-05",
        "target.type == ValueType::ObjCProtocol || value.type == ValueType::ObjCProtocol",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-B001-ARCH-01",
        "M227 lane-B B001 type-system completeness for ObjC3 forms contract and architecture freeze anchors",
    ),
    SnippetCheck(
        "M227-B001-ARCH-02",
        "docs/contracts/m227_type_system_objc3_forms_contract_and_architecture_freeze_b001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-B001-SPC-01",
        "type-system completeness for ObjC3 forms governance shall preserve explicit",
    ),
    SnippetCheck(
        "M227-B001-SPC-02",
        "lane-B dependency anchors (`M227-B001`) and fail closed on canonical ObjC",
    ),
    SnippetCheck(
        "M227-B001-SPC-03",
        "type-form contract drift before semantic compatibility and migration",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-B001-META-01",
        "deterministic lane-B type-system completeness for ObjC3 forms metadata anchors for `M227-B001`",
    ),
    SnippetCheck(
        "M227-B001-META-02",
        "canonical reference/message/bridge-top form evidence and semantic-pass fail-closed continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-B001-PKG-01",
        '"check:objc3c:m227-b001-type-system-objc3-forms-contract": "python scripts/check_m227_b001_type_system_objc3_forms_contract.py"',
    ),
    SnippetCheck(
        "M227-B001-PKG-02",
        '"test:tooling:m227-b001-type-system-objc3-forms-contract": "python -m pytest tests/tooling/test_check_m227_b001_type_system_objc3_forms_contract.py -q"',
    ),
    SnippetCheck(
        "M227-B001-PKG-03",
        '"check:objc3c:m227-b001-lane-b-readiness": "npm run check:objc3c:m227-b001-type-system-objc3-forms-contract && npm run test:tooling:m227-b001-type-system-objc3-forms-contract"',
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
    parser.add_argument("--sema-contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--semantic-passes", type=Path, default=DEFAULT_SEMANTIC_PASSES)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    required_snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
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
    for snippet in required_snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"forbidden snippet present: {snippet.snippet}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, required_snippets, forbidden_snippets in (
        (args.expectations_doc, "M227-B001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M227-B001-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.sema_contract, "M227-B001-SEM-EXISTS", SEMA_CONTRACT_SNIPPETS, ()),
        (
            args.semantic_passes,
            "M227-B001-PSS-EXISTS",
            SEMANTIC_PASSES_SNIPPETS,
            SEMANTIC_PASSES_FORBIDDEN_SNIPPETS,
        ),
        (args.architecture_doc, "M227-B001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS, ()),
        (args.lowering_spec, "M227-B001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS, ()),
        (args.metadata_spec, "M227-B001-META-EXISTS", METADATA_SPEC_SNIPPETS, ()),
        (args.package_json, "M227-B001-PKG-EXISTS", PACKAGE_SNIPPETS, ()),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += count
        failures.extend(findings)

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
