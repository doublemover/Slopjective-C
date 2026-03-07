#!/usr/bin/env python3
"""Fail-closed contract checker for M251-A001 runtime metadata source ownership."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-a001-runtime-metadata-source-ownership-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_runtime_metadata_source_ownership_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_a001_runtime_metadata_source_ownership_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-A001/runtime_metadata_source_ownership_contract_summary.json"
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
        "M251-A001-DOC-EXP-01",
        "# M251 Runtime Metadata Source Ownership Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M251-A001-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-metadata-source-ownership-freeze/m251-a001-v1`",
    ),
    SnippetCheck(
        "M251-A001-DOC-EXP-03",
        "`Objc3RuntimeMetadataSourceOwnershipBoundary` as the canonical lane-A",
    ),
    SnippetCheck(
        "M251-A001-DOC-EXP-04",
        "`check:objc3c:m251-a001-runtime-metadata-source-ownership-contract`",
    ),
    SnippetCheck(
        "M251-A001-DOC-EXP-05",
        "`tmp/reports/m251/M251-A001/runtime_metadata_source_ownership_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-DOC-PKT-01",
        "# M251-A001 Runtime Metadata Source Ownership Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M251-A001-DOC-PKT-02", "Packet: `M251-A001`"),
    SnippetCheck("M251-A001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck(
        "M251-A001-DOC-PKT-04",
        "Canonical source schema: `objc3-runtime-metadata-source-boundary-v1`",
    ),
    SnippetCheck(
        "M251-A001-DOC-PKT-05",
        "`property-synthesis-ivar-binding-symbols`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-ARCH-01",
        "M251 lane-A A001 runtime metadata source ownership freeze anchors explicit",
    ),
    SnippetCheck(
        "M251-A001-ARCH-02",
        "docs/contracts/m251_runtime_metadata_source_ownership_contract_and_architecture_freeze_a001_expectations.md",
    ),
    SnippetCheck(
        "M251-A001-ARCH-03",
        "fail-closed before `M251-A002` extraction work begins.",
    ),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-NDOC-01",
        "## Runtime metadata source ownership freeze (M251-A001)",
    ),
    SnippetCheck(
        "M251-A001-NDOC-02",
        "`objc3-runtime-metadata-source-boundary-v1`",
    ),
    SnippetCheck(
        "M251-A001-NDOC-03",
        "the deterministic `objc3_msgsend_i32` shim remains test-only evidence.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-SPC-01",
        "## M251 runtime metadata source ownership freeze (A001)",
    ),
    SnippetCheck(
        "M251-A001-SPC-02",
        "lane-A boundary anchors and fail closed on runtime metadata source drift",
    ),
    SnippetCheck(
        "M251-A001-SPC-03",
        "property-synthesis ivar binding",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-META-01",
        "## M251 runtime metadata source ownership metadata anchors (A001)",
    ),
    SnippetCheck(
        "M251-A001-META-02",
        "record-count evidence for classes, protocols, category interfaces,",
    ),
    SnippetCheck(
        "M251-A001-META-03",
        "test-shim-only topology",
    ),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-A001-AST-01", "kObjc3RuntimeMetadataSourceOwnershipContractId"),
    SnippetCheck("M251-A001-AST-02", '"objc3-runtime-metadata-source-boundary-v1"'),
    SnippetCheck("M251-A001-AST-03", "kObjc3RuntimeMetadataClassAstAnchor"),
    SnippetCheck("M251-A001-AST-04", "kObjc3RuntimeMetadataIvarSourceModel"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-TYP-01",
        "struct Objc3RuntimeMetadataSourceOwnershipBoundary {",
    ),
    SnippetCheck(
        "M251-A001-TYP-02",
        "bool frontend_owns_runtime_metadata_source_records = false;",
    ),
    SnippetCheck(
        "M251-A001-TYP-03",
        "bool runtime_metadata_source_records_ready_for_lowering = false;",
    ),
    SnippetCheck(
        "M251-A001-TYP-04",
        "inline bool IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(",
    ),
    SnippetCheck(
        "M251-A001-TYP-05",
        "Objc3RuntimeMetadataSourceOwnershipBoundary runtime_metadata_source_ownership_boundary;",
    ),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-PIP-01",
        "Objc3RuntimeMetadataSourceOwnershipBoundary BuildRuntimeMetadataSourceOwnershipBoundary(",
    ),
    SnippetCheck(
        "M251-A001-PIP-02",
        "boundary.runtime_metadata_source_records_ready_for_lowering = false;",
    ),
    SnippetCheck(
        "M251-A001-PIP-03",
        "boundary.native_runtime_library_present = false;",
    ),
    SnippetCheck(
        "M251-A001-PIP-04",
        "boundary.runtime_shim_test_only = true;",
    ),
    SnippetCheck(
        "M251-A001-PIP-05",
        "BuildRuntimeMetadataSourceOwnershipBoundary(Objc3ParsedProgramAst(result.program),",
    ),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-ART-01",
        "const Objc3RuntimeMetadataSourceOwnershipBoundary &runtime_metadata_source_ownership =",
    ),
    SnippetCheck(
        "M251-A001-ART-02",
        "runtime_metadata_source_ownership_contract_id",
    ),
    SnippetCheck(
        "M251-A001-ART-03",
        "runtime_metadata_source_boundary_ready",
    ),
    SnippetCheck(
        "M251-A001-ART-04",
        "runtime_metadata_source_boundary_failure_reason",
    ),
)

IR_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-IRH-01",
        "std::string runtime_metadata_source_ownership_contract_id;",
    ),
    SnippetCheck(
        "M251-A001-IRH-02",
        "std::size_t runtime_metadata_class_record_count = 0;",
    ),
    SnippetCheck(
        "M251-A001-IRH-03",
        "bool runtime_metadata_source_boundary_fail_closed = false;",
    ),
)

IR_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-IRC-01",
        "; runtime_metadata_source_ownership = ",
    ),
    SnippetCheck(
        "M251-A001-IRC-02",
        "!objc3.objc_runtime_metadata_source_ownership = !{!45}",
    ),
    SnippetCheck(
        "M251-A001-IRC-03",
        "frontend_metadata_.runtime_metadata_source_ownership_contract_id",
    ),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-DRV-01",
        "M251-A001 freeze: the frontend emits runtime metadata ownership evidence",
    ),
    SnippetCheck("M251-A001-DRV-02", "WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);"),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-SHIM-01",
        "M251-A001 freeze: this shim remains test-only evidence and is not the native",
    ),
    SnippetCheck("M251-A001-SHIM-02", "int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3) {"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A001-PKG-01",
        '"check:objc3c:m251-a001-runtime-metadata-source-ownership-contract": "python scripts/check_m251_a001_runtime_metadata_source_ownership_contract.py"',
    ),
    SnippetCheck(
        "M251-A001-PKG-02",
        '"test:tooling:m251-a001-runtime-metadata-source-ownership-contract": "python -m pytest tests/tooling/test_check_m251_a001_runtime_metadata_source_ownership_contract.py -q"',
    ),
    SnippetCheck(
        "M251-A001-PKG-03",
        '"check:objc3c:m251-a001-lane-a-readiness": "npm run check:objc3c:m251-a001-runtime-metadata-source-ownership-contract && npm run test:tooling:m251-a001-runtime-metadata-source-ownership-contract"',
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
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-header", type=Path, default=DEFAULT_IR_EMITTER_HEADER)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
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
        (args.expectations_doc, "M251-A001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M251-A001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M251-A001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M251-A001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M251-A001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M251-A001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M251-A001-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M251-A001-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M251-A001-PIP-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M251-A001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_header, "M251-A001-IRH-EXISTS", IR_HEADER_SNIPPETS),
        (args.ir_emitter_cpp, "M251-A001-IRC-EXISTS", IR_CPP_SNIPPETS),
        (args.driver_cpp, "M251-A001-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.runtime_shim, "M251-A001-SHIM-EXISTS", RUNTIME_SHIM_SNIPPETS),
        (args.package_json, "M251-A001-PKG-EXISTS", PACKAGE_SNIPPETS),
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
