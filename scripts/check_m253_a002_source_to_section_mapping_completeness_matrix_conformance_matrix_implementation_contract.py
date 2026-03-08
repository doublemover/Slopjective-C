#!/usr/bin/env python3
"""Fail-closed checker for M253-A002 source-to-section completeness matrix."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-a002-source-to-section-mapping-completeness-matrix-v1"
CONTRACT_ID = "objc3c-runtime-metadata-source-to-section-matrix/m253-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix"
ROW_ORDERING_MODEL = "source-graph-node-kind-order-v1"
NO_STANDALONE = "(none)"
NO_STANDALONE_EMISSION = "no-standalone-emission-yet"
STANDALONE_EMISSION = "standalone-descriptor-section"
AGGREGATE_RELOCATION = "zero-sentinel-or-count-plus-pointer-vector"
NO_STANDALONE_RELOCATION = "none"
EXPECTED_SECTIONS = (
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
)
EXPECTED_SUPPLEMENTAL_SECTIONS = (
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
)
EXPECTED_INSPECTION_SECTIONS = EXPECTED_SECTIONS + EXPECTED_SUPPLEMENTAL_SECTIONS
EXPECTED_SYMBOLS = (
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
)
EXPECTED_ROWS = {
    "interface-node-to-emission": {
        "graph_node_kind": "interface",
        "emission_mode": NO_STANDALONE_EMISSION,
        "logical_section": NO_STANDALONE,
        "payload_role": "no standalone emitted interface payload yet",
        "descriptor_symbol_family": NO_STANDALONE,
        "aggregate_symbol": NO_STANDALONE,
        "relocation_behavior": NO_STANDALONE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "source-graph-fixture",
        "section_inventory_command": NO_STANDALONE,
        "symbol_inventory_command": NO_STANDALONE,
    },
    "implementation-node-to-emission": {
        "graph_node_kind": "implementation",
        "emission_mode": NO_STANDALONE_EMISSION,
        "logical_section": NO_STANDALONE,
        "payload_role": "no standalone emitted implementation payload yet",
        "descriptor_symbol_family": NO_STANDALONE,
        "aggregate_symbol": NO_STANDALONE,
        "relocation_behavior": NO_STANDALONE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "source-graph-fixture",
        "section_inventory_command": NO_STANDALONE,
        "symbol_inventory_command": NO_STANDALONE,
    },
    "class-node-to-emission": {
        "graph_node_kind": "class",
        "emission_mode": STANDALONE_EMISSION,
        "logical_section": "objc3.runtime.class_descriptors",
        "payload_role": "standalone class descriptor payload",
        "descriptor_symbol_family": "__objc3_meta_class_####",
        "aggregate_symbol": "__objc3_sec_class_descriptors",
        "relocation_behavior": AGGREGATE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "fixture-plus-object-inspection",
        "section_inventory_command": "llvm-readobj --sections module.obj",
        "symbol_inventory_command": "llvm-objdump --syms module.obj",
    },
    "metaclass-node-to-emission": {
        "graph_node_kind": "metaclass",
        "emission_mode": NO_STANDALONE_EMISSION,
        "logical_section": NO_STANDALONE,
        "payload_role": "no standalone emitted metaclass payload yet",
        "descriptor_symbol_family": NO_STANDALONE,
        "aggregate_symbol": NO_STANDALONE,
        "relocation_behavior": NO_STANDALONE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "source-graph-fixture",
        "section_inventory_command": NO_STANDALONE,
        "symbol_inventory_command": NO_STANDALONE,
    },
    "protocol-node-to-emission": {
        "graph_node_kind": "protocol",
        "emission_mode": STANDALONE_EMISSION,
        "logical_section": "objc3.runtime.protocol_descriptors",
        "payload_role": "standalone protocol descriptor payload",
        "descriptor_symbol_family": "__objc3_meta_protocol_####",
        "aggregate_symbol": "__objc3_sec_protocol_descriptors",
        "relocation_behavior": AGGREGATE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "fixture-plus-object-inspection",
        "section_inventory_command": "llvm-readobj --sections module.obj",
        "symbol_inventory_command": "llvm-objdump --syms module.obj",
    },
    "category-node-to-emission": {
        "graph_node_kind": "category",
        "emission_mode": STANDALONE_EMISSION,
        "logical_section": "objc3.runtime.category_descriptors",
        "payload_role": "standalone category descriptor payload",
        "descriptor_symbol_family": "__objc3_meta_category_####",
        "aggregate_symbol": "__objc3_sec_category_descriptors",
        "relocation_behavior": AGGREGATE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3",
        "proof_mode": "fixture-plus-object-inspection",
        "section_inventory_command": "llvm-readobj --sections module.obj",
        "symbol_inventory_command": "llvm-objdump --syms module.obj",
    },
    "property-node-to-emission": {
        "graph_node_kind": "property",
        "emission_mode": STANDALONE_EMISSION,
        "logical_section": "objc3.runtime.property_descriptors",
        "payload_role": "standalone property descriptor payload",
        "descriptor_symbol_family": "__objc3_meta_property_####",
        "aggregate_symbol": "__objc3_sec_property_descriptors",
        "relocation_behavior": AGGREGATE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "fixture-plus-object-inspection",
        "section_inventory_command": "llvm-readobj --sections module.obj",
        "symbol_inventory_command": "llvm-objdump --syms module.obj",
    },
    "method-node-to-emission": {
        "graph_node_kind": "method",
        "emission_mode": NO_STANDALONE_EMISSION,
        "logical_section": NO_STANDALONE,
        "payload_role": "no standalone emitted method payload yet",
        "descriptor_symbol_family": NO_STANDALONE,
        "aggregate_symbol": NO_STANDALONE,
        "relocation_behavior": NO_STANDALONE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "source-graph-fixture",
        "section_inventory_command": NO_STANDALONE,
        "symbol_inventory_command": NO_STANDALONE,
    },
    "ivar-node-to-emission": {
        "graph_node_kind": "ivar",
        "emission_mode": STANDALONE_EMISSION,
        "logical_section": "objc3.runtime.ivar_descriptors",
        "payload_role": "standalone ivar descriptor payload",
        "descriptor_symbol_family": "__objc3_meta_ivar_####",
        "aggregate_symbol": "__objc3_sec_ivar_descriptors",
        "relocation_behavior": AGGREGATE_RELOCATION,
        "proof_fixture_path": "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3",
        "proof_mode": "fixture-plus-object-inspection",
        "section_inventory_command": "llvm-readobj --sections module.obj",
        "symbol_inventory_command": "llvm-objdump --syms module.obj",
    },
}

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_a002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_a002_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_INSPECTION_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "source-to-section-matrix"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json")
MISSING = object()


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
    SnippetCheck("M253-A002-DOC-EXP-01", "# M253 Source-to-Section Mapping Completeness Matrix Conformance Matrix Implementation Expectations (A002)"),
    SnippetCheck("M253-A002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-A002-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M253-A002-DOC-EXP-04", "`interface`"),
    SnippetCheck("M253-A002-DOC-EXP-05", "`__objc3_meta_class_####`"),
    SnippetCheck("M253-A002-DOC-EXP-06", "`zero-sentinel-or-count-plus-pointer-vector`"),
    SnippetCheck("M253-A002-DOC-EXP-07", "`tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M253-A002-DOC-PKT-01", "# M253-A002 Source-to-Section Mapping Completeness Matrix Conformance Matrix Implementation Packet"),
    SnippetCheck("M253-A002-DOC-PKT-02", "Packet: `M253-A002`"),
    SnippetCheck("M253-A002-DOC-PKT-03", "Dependencies: `M253-A001`"),
    SnippetCheck("M253-A002-DOC-PKT-04", "`llvm-readobj --sections module.obj`"),
    SnippetCheck("M253-A002-DOC-PKT-05", "`llvm-objdump --syms module.obj`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M253-A002-ARCH-01", "M253 lane-A A002 source-to-section completeness matrix anchors explicit"),
    SnippetCheck("M253-A002-ARCH-02", "m253_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_a002_expectations.md"),
    SnippetCheck("M253-A002-ARCH-03", "no-standalone-emission entries until later payload work lands"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M253-A002-NDOC-01", "## Source-to-section mapping completeness matrix (M253-A002)"),
    SnippetCheck("M253-A002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-A002-NDOC-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M253-A002-NDOC-04", "Current non-standalone rows remain explicit and fail closed"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M253-A002-SPC-01", "## M253 source-to-section mapping completeness matrix (A002)"),
    SnippetCheck("M253-A002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-A002-SPC-03", "`interface`, `implementation`, `class`, `metaclass`, `protocol`, `category`,"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M253-A002-META-01", "## M253 source-to-section completeness matrix metadata anchors (A002)"),
    SnippetCheck("M253-A002-META-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M253-A002-META-03", "m251_runtime_metadata_source_records_category_protocol_property.objc3"),
)
AST_SNIPPETS = (
    SnippetCheck("M253-A002-AST-01", "kObjc3RuntimeMetadataSourceToSectionMatrixContractId"),
    SnippetCheck("M253-A002-AST-02", "kObjc3RuntimeMetadataSourceToSectionMatrixSurfacePath"),
    SnippetCheck("M253-A002-AST-03", "kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey"),
    SnippetCheck("M253-A002-AST-04", "kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M253-A002-LHDR-01", "M253-A002 source-to-section matrix anchor: interface/implementation/"),
    SnippetCheck("M253-A002-LHDR-02", "source-to-section"),
    SnippetCheck("M253-A002-LHDR-03", "metaclass/method rows stay explicit no-standalone-emission entries until"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M253-A002-LCPP-01", "M253-A002 source-to-section matrix anchor"),
    SnippetCheck("M253-A002-LCPP-02", "node-to-section matrix"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M253-A002-IR-01", "M253-A002 source-to-section matrix anchor"),
    SnippetCheck("M253-A002-IR-02", "Interface/implementation/metaclass/method nodes"),
)
PROCESS_SNIPPETS = (
    SnippetCheck("M253-A002-PROC-01", "M253-A002 source-to-section matrix anchor"),
    SnippetCheck("M253-A002-PROC-02", "unsupported standalone rows stay explicit in the published"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M253-A002-TYP-01", "struct Objc3RuntimeMetadataSourceToSectionMatrixRow {"),
    SnippetCheck("M253-A002-TYP-02", "struct Objc3RuntimeMetadataSourceToSectionMatrixSummary {"),
    SnippetCheck("M253-A002-TYP-03", "std::array<Objc3RuntimeMetadataSourceToSectionMatrixRow, 9u> rows = {};"),
    SnippetCheck("M253-A002-TYP-04", "summary.matrix_row_count != summary.rows.size()"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M253-A002-ART-01", "BuildRuntimeMetadataSourceToSectionMatrixSummary("),
    SnippetCheck("M253-A002-ART-02", "BuildRuntimeMetadataSourceToSectionMatrixSummaryJson("),
    SnippetCheck("M253-A002-ART-03", "objc_runtime_metadata_source_to_section_matrix"),
    SnippetCheck("M253-A002-ART-04", "kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M253-A002-PKG-01", '"check:objc3c:m253-a002-source-to-section-mapping-completeness-matrix-contract": "python scripts/check_m253_a002_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_contract.py"'),
    SnippetCheck("M253-A002-PKG-02", '"test:tooling:m253-a002-source-to-section-mapping-completeness-matrix-contract": "python -m pytest tests/tooling/test_check_m253_a002_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_contract.py -q"'),
    SnippetCheck("M253-A002-PKG-03", '"check:objc3c:m253-a002-lane-a-readiness": "npm run check:objc3c:m253-a001-lane-a-readiness && npm run check:objc3c:m253-a002-source-to-section-mapping-completeness-matrix-contract && npm run test:tooling:m253-a002-source-to-section-mapping-completeness-matrix-contract"'),
)

ARTIFACT_SNIPPETS = {
    "expectations_doc": EXPECTATIONS_SNIPPETS,
    "packet_doc": PACKET_SNIPPETS,
    "architecture_doc": ARCHITECTURE_SNIPPETS,
    "native_doc": NATIVE_DOC_SNIPPETS,
    "lowering_spec": LOWERING_SPEC_SNIPPETS,
    "metadata_spec": METADATA_SPEC_SNIPPETS,
    "ast_header": AST_SNIPPETS,
    "lowering_header": LOWERING_HEADER_SNIPPETS,
    "lowering_cpp": LOWERING_CPP_SNIPPETS,
    "ir_emitter_cpp": IR_EMITTER_SNIPPETS,
    "process_cpp": PROCESS_SNIPPETS,
    "frontend_types": FRONTEND_TYPES_SNIPPETS,
    "frontend_artifacts": FRONTEND_ARTIFACTS_SNIPPETS,
    "package_json": PACKAGE_SNIPPETS,
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--inspection-fixture", type=Path, default=DEFAULT_INSPECTION_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-readobj", type=Path, default=None)
    parser.add_argument("--llvm-objdump", type=Path, default=None)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def find_first_key(payload: object, key: str) -> object:
    if isinstance(payload, dict):
        if key in payload:
            return payload[key]
        for value in payload.values():
            found = find_first_key(value, key)
            if found is not MISSING:
                return found
    elif isinstance(payload, list):
        for value in payload:
            found = find_first_key(value, key)
            if found is not MISSING:
                return found
    return MISSING


def resolve_tool(tool_name: str, explicit_path: Path | None) -> Path | None:
    if explicit_path is not None:
        candidate = explicit_path.resolve()
        return candidate if candidate.exists() else None
    env_bin = os.environ.get("LLVM_BIN_DIR")
    if env_bin:
        candidate = Path(env_bin) / tool_name
        if candidate.exists():
            return candidate.resolve()
    program_files = os.environ.get("ProgramFiles")
    if program_files:
        candidate = Path(program_files) / "LLVM" / "bin" / tool_name
        if candidate.exists():
            return candidate.resolve()
    resolved = shutil.which(tool_name)
    if resolved:
        return Path(resolved).resolve()
    return None


def extract_metadata_sections(readobj_stdout: str) -> list[dict[str, object]]:
    sections: list[dict[str, object]] = []
    for block in readobj_stdout.split("Section {"):
        name_match = re.search(r"Name: ([^\s(]+)", block)
        if name_match is None:
            continue
        name = name_match.group(1)
        if not name.startswith("objc3.runtime."):
            continue
        raw_match = re.search(r"RawDataSize: (0x[0-9A-Fa-f]+|\d+)", block)
        raw_size = int(raw_match.group(1), 0) if raw_match else None
        sections.append({"name": name, "raw_data_size": raw_size})
    return sections


def extract_retained_symbols(objdump_stdout: str) -> list[str]:
    return [symbol for symbol in EXPECTED_SYMBOLS if symbol in objdump_stdout]


def locate_graph(payload: dict[str, object]) -> dict[str, object] | None:
    current: object = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_executable_metadata_source_graph"):
        if not isinstance(current, dict):
            return None
        next_value = current.get(key)
        if not isinstance(next_value, dict):
            return None
        current = next_value
    return current if isinstance(current, dict) else None


def locate_matrix(payload: dict[str, object]) -> dict[str, object] | None:
    current: object = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_runtime_metadata_source_to_section_matrix"):
        if not isinstance(current, dict):
            return None
        next_value = current.get(key)
        if not isinstance(next_value, dict):
            return None
        current = next_value
    return current if isinstance(current, dict) else None


def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path, failures: list[Finding], issue_prefix: str) -> tuple[int, dict[str, object] | None]:
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), f"{issue_prefix}-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures and any(f.check_id == f"{issue_prefix}-FIXTURE-EXISTS" for f in failures):
        return checks_total, None
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(runner_exe), str(fixture_path), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    completed = run_command(command, ROOT)
    summary_path = out_dir / "module.c_api_summary.json"
    checks_total += require(summary_path.exists(), display_path(summary_path), f"{issue_prefix}-SUMMARY", "runner did not write module.c_api_summary.json", failures)
    if not summary_path.exists():
        return checks_total, None
    try:
        summary_payload = json.loads(summary_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(summary_path), f"{issue_prefix}-SUMMARY-JSON", f"failed to parse summary JSON: {exc}"))
        return checks_total + 1, None
    for stage_name in ("lex", "parse", "sema", "lower"):
        stage = summary_payload.get("stages", {}).get(stage_name)
        checks_total += require(isinstance(stage, dict) and stage.get("diagnostics_errors") == 0, display_path(summary_path), f"{issue_prefix}-STAGE-{stage_name.upper()}", f"expected {stage_name} diagnostics_errors == 0", failures)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), f"{issue_prefix}-SUCCESS", "runner success must be true", failures)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), f"{issue_prefix}-STATUS", "runner status must be 0", failures)
    manifest_rel = summary_payload.get("paths", {}).get("manifest")
    manifest_path = ROOT / manifest_rel if isinstance(manifest_rel, str) and not Path(manifest_rel).is_absolute() else Path(str(manifest_rel))
    checks_total += require(manifest_path.exists(), display_path(manifest_path), f"{issue_prefix}-MANIFEST-EXISTS", "runner manifest path is missing", failures)
    if not manifest_path.exists():
        return checks_total, None
    manifest_payload = load_json(manifest_path)
    graph = locate_graph(manifest_payload if isinstance(manifest_payload, dict) else {})
    checks_total += require(graph is not None, display_path(manifest_path), f"{issue_prefix}-GRAPH-PATH", "missing executable metadata source graph", failures)
    if graph is None:
        return checks_total, None
    return checks_total, {
        "fixture": display_path(fixture_path),
        "out_dir": display_path(out_dir),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "graph": graph,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def validate_row_matrix(matrix: dict[str, object], failures: list[Finding], artifact: str) -> int:
    checks_total = 0
    checks_total += require(matrix.get("contract_id") == CONTRACT_ID, artifact, "M253-A002-MATRIX-CONTRACT-ID", "matrix contract id mismatch", failures)
    checks_total += require(matrix.get("manifest_surface_path") == SURFACE_PATH, artifact, "M253-A002-MATRIX-SURFACE", "matrix surface path mismatch", failures)
    checks_total += require(matrix.get("row_ordering_model") == ROW_ORDERING_MODEL, artifact, "M253-A002-MATRIX-ORDERING", "row ordering model mismatch", failures)
    checks_total += require(matrix.get("ready") is True, artifact, "M253-A002-MATRIX-READY", "matrix ready must be true", failures)
    checks_total += require(matrix.get("matrix_published") is True, artifact, "M253-A002-MATRIX-PUBLISHED", "matrix_published must be true", failures)
    checks_total += require(matrix.get("fail_closed") is True, artifact, "M253-A002-MATRIX-FAIL-CLOSED", "fail_closed must be true", failures)
    checks_total += require(matrix.get("source_graph_ready") is True, artifact, "M253-A002-MATRIX-SOURCE-GRAPH", "source_graph_ready must be true", failures)
    checks_total += require(matrix.get("section_abi_ready") is True, artifact, "M253-A002-MATRIX-SECTION-ABI", "section_abi_ready must be true", failures)
    checks_total += require(matrix.get("section_scaffold_ready") is True, artifact, "M253-A002-MATRIX-SCAFFOLD", "section_scaffold_ready must be true", failures)
    checks_total += require(matrix.get("object_inspection_ready") is True, artifact, "M253-A002-MATRIX-OBJECT-INSPECTION", "object_inspection_ready must be true", failures)
    checks_total += require(matrix.get("supported_node_coverage_complete") is True, artifact, "M253-A002-MATRIX-COVERAGE", "supported_node_coverage_complete must be true", failures)
    checks_total += require(matrix.get("explicit_non_goals_published") is True, artifact, "M253-A002-MATRIX-NON-GOALS", "explicit_non_goals_published must be true", failures)
    checks_total += require(matrix.get("row_ordering_frozen") is True, artifact, "M253-A002-MATRIX-ORDERING-FROZEN", "row_ordering_frozen must be true", failures)
    checks_total += require(matrix.get("matrix_row_count") == 9, artifact, "M253-A002-MATRIX-ROW-COUNT", "matrix_row_count must be 9", failures)
    rows = matrix.get("rows")
    checks_total += require(isinstance(rows, list) and len(rows) == 9, artifact, "M253-A002-MATRIX-ROWS", "rows must be a list of length 9", failures)
    if not isinstance(rows, list):
        return checks_total
    by_key = {row.get("row_key"): row for row in rows if isinstance(row, dict)}
    for row_key, expected in EXPECTED_ROWS.items():
        row = by_key.get(row_key)
        checks_total += require(isinstance(row, dict), artifact, f"M253-A002-ROW-{row_key}-EXISTS", f"missing row {row_key}", failures)
        if not isinstance(row, dict):
            continue
        for field_name, expected_value in expected.items():
            checks_total += require(row.get(field_name) == expected_value, artifact, f"M253-A002-ROW-{row_key}-{field_name}", f"expected {row_key}.{field_name} == {expected_value!r}", failures)
    return checks_total


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object], dict[str, str]]:
    checks_total = 0
    checks_passed = 0
    cases: dict[str, object] = {}
    tool_paths: dict[str, str] = {}
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)

    llvm_readobj = resolve_tool("llvm-readobj.exe", args.llvm_readobj)
    llvm_objdump = resolve_tool("llvm-objdump.exe", args.llvm_objdump)
    checks_total += 1
    if llvm_readobj is None:
        failures.append(Finding("dynamic", "M253-A002-TOOL-READOBJ", "llvm-readobj.exe not found"))
    else:
        checks_passed += 1
        tool_paths["llvm_readobj"] = display_path(llvm_readobj)
    checks_total += 1
    if llvm_objdump is None:
        failures.append(Finding("dynamic", "M253-A002-TOOL-OBJDUMP", "llvm-objdump.exe not found"))
    else:
        checks_passed += 1
        tool_paths["llvm_objdump"] = display_path(llvm_objdump)

    checks_total += 1
    if not args.runner_exe.exists():
        failures.append(Finding(display_path(args.runner_exe), "M253-A002-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
    else:
        checks_passed += 1
        class_probe_checks, class_case = run_runner_probe(runner_exe=args.runner_exe, fixture_path=args.class_fixture, out_dir=probe_root / "class-probe", failures=failures, issue_prefix="M253-A002-CLASS")
        checks_total += class_probe_checks
        if class_case is not None:
            cases["class_fixture"] = class_case
            graph = class_case["graph"]
            assert isinstance(graph, dict)
            artifact = str(class_case["manifest_path"])
            for key, minimum in (("interface_nodes", 1), ("implementation_nodes", 1), ("class_nodes", 1), ("metaclass_nodes", 1), ("protocol_nodes", 1), ("property_nodes", 1), ("method_nodes", 1), ("ivar_nodes", 1)):
                checks_total += require(isinstance(graph.get(key), int) and int(graph.get(key)) >= minimum, artifact, f"M253-A002-CLASS-{key}", f"expected {key} >= {minimum}", failures)
        category_probe_checks, category_case = run_runner_probe(runner_exe=args.runner_exe, fixture_path=args.category_fixture, out_dir=probe_root / "category-probe", failures=failures, issue_prefix="M253-A002-CATEGORY")
        checks_total += category_probe_checks
        if category_case is not None:
            cases["category_fixture"] = category_case
            graph = category_case["graph"]
            assert isinstance(graph, dict)
            artifact = str(category_case["manifest_path"])
            for key, minimum in (("protocol_nodes", 1), ("category_nodes", 1), ("property_nodes", 1), ("method_nodes", 1)):
                checks_total += require(isinstance(graph.get(key), int) and int(graph.get(key)) >= minimum, artifact, f"M253-A002-CATEGORY-{key}", f"expected {key} >= {minimum}", failures)

    checks_total += 1
    if not args.native_exe.exists():
        failures.append(Finding(display_path(args.native_exe), "M253-A002-NATIVE-EXISTS", "objc3c-native.exe is missing; run npm run build:objc3c-native"))
    else:
        checks_passed += 1
        out_dir = probe_root / "inspection-probe"
        out_dir.mkdir(parents=True, exist_ok=True)
        compile_result = run_command([str(args.native_exe.resolve()), str(args.inspection_fixture.resolve()), "--out-dir", str(out_dir), "--emit-prefix", "module"], ROOT)
        manifest_path = out_dir / "module.manifest.json"
        object_path = out_dir / "module.obj"
        backend_path = out_dir / "module.object-backend.txt"
        backend_marker = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
        manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
        matrix = locate_matrix(manifest_payload if isinstance(manifest_payload, dict) else {})
        native_case = {
            "fixture": display_path(args.inspection_fixture),
            "out_dir": display_path(out_dir),
            "compile_exit_code": compile_result.returncode,
            "manifest_exists": manifest_path.exists(),
            "object_exists": object_path.exists(),
            "object_backend": backend_marker,
            "matrix": matrix,
            "readobj_exit_code": None,
            "objdump_exit_code": None,
            "metadata_sections": [],
            "retained_symbols": [],
        }
        cases["inspection_fixture"] = native_case
        checks_total += require(compile_result.returncode == 0, display_path(manifest_path), "M253-A002-NATIVE-STATUS", "native probe must exit 0", failures)
        checks_total += require(manifest_path.exists(), display_path(manifest_path), "M253-A002-NATIVE-MANIFEST", "native probe must emit module.manifest.json", failures)
        checks_total += require(object_path.exists(), display_path(object_path), "M253-A002-NATIVE-OBJECT", "native probe must emit module.obj", failures)
        checks_total += require(matrix is not None, display_path(manifest_path), "M253-A002-NATIVE-MATRIX", "matrix semantic surface missing from manifest", failures)
        if isinstance(matrix, dict):
            checks_total += validate_row_matrix(matrix, failures, display_path(manifest_path))
        if compile_result.returncode == 0 and object_path.exists() and llvm_readobj is not None and llvm_objdump is not None:
            readobj_result = run_command([str(llvm_readobj), "--sections", str(object_path)], ROOT)
            objdump_result = run_command([str(llvm_objdump), "--syms", str(object_path)], ROOT)
            metadata_sections = extract_metadata_sections(readobj_result.stdout)
            retained_symbols = extract_retained_symbols(objdump_result.stdout)
            native_case["readobj_exit_code"] = readobj_result.returncode
            native_case["objdump_exit_code"] = objdump_result.returncode
            native_case["metadata_sections"] = metadata_sections
            native_case["retained_symbols"] = retained_symbols
            checks_total += require(readobj_result.returncode == 0, display_path(object_path), "M253-A002-READOBJ-STATUS", "llvm-readobj must exit 0", failures)
            checks_total += require(objdump_result.returncode == 0, display_path(object_path), "M253-A002-OBJDUMP-STATUS", "llvm-objdump must exit 0", failures)
            section_names = sorted(section["name"] for section in metadata_sections)
            section_sizes = {section["name"]: section["raw_data_size"] for section in metadata_sections}
            checks_total += require(section_names == sorted(EXPECTED_INSPECTION_SECTIONS), display_path(object_path), "M253-A002-OBJECT-SECTIONS", "metadata sections mismatch", failures)
            checks_total += require(all(section_sizes.get(name) == 8 for name in EXPECTED_SECTIONS), display_path(object_path), "M253-A002-OBJECT-BASE-SIZES", "expected base matrix section sizes of 8", failures)
            checks_total += require(int(section_sizes.get("objc3.runtime.discovery_root", 0) or 0) > 8, display_path(object_path), "M253-A002-OBJECT-DISCOVERY-SIZE", "expected non-trivial discovery_root packaging payload", failures)
            checks_total += require(section_sizes.get("objc3.runtime.linker_anchor") == 8, display_path(object_path), "M253-A002-OBJECT-LINKER-ANCHOR-SIZE", "expected linker_anchor section size of 8", failures)
            checks_total += require(sorted(retained_symbols) == sorted(EXPECTED_SYMBOLS), display_path(object_path), "M253-A002-OBJECT-SYMBOLS", "retained symbol inventory mismatch", failures)
            checks_total += require(backend_marker in {"llvm-direct", "llc"}, display_path(object_path), "M253-A002-OBJECT-BACKEND", "unexpected object backend marker", failures)

    checks_passed = checks_total - len(failures)
    return checks_total, checks_passed, cases, tool_paths


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    artifact_paths = {
        "expectations_doc": args.expectations_doc,
        "packet_doc": args.packet_doc,
        "architecture_doc": args.architecture_doc,
        "native_doc": args.native_doc,
        "lowering_spec": args.lowering_spec,
        "metadata_spec": args.metadata_spec,
        "ast_header": args.ast_header,
        "lowering_header": args.lowering_header,
        "lowering_cpp": args.lowering_cpp,
        "ir_emitter_cpp": args.ir_emitter_cpp,
        "process_cpp": args.process_cpp,
        "frontend_types": args.frontend_types,
        "frontend_artifacts": args.frontend_artifacts,
        "package_json": args.package_json,
    }
    for artifact_name, path in artifact_paths.items():
        checks_total += len(ARTIFACT_SNIPPETS[artifact_name])
        checks_passed += ensure_snippets(path, ARTIFACT_SNIPPETS[artifact_name], failures)

    dynamic_cases: dict[str, object] = {}
    tool_paths: dict[str, str] = {}
    if not args.skip_dynamic_probes:
        dynamic_total, dynamic_passed, dynamic_cases, tool_paths = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed = checks_total - len(failures)

    summary = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "tool_paths": tool_paths,
        "dynamic_cases": dynamic_cases,
        "expected_row_count": 9,
        "expected_rows": EXPECTED_ROWS,
        "failures": [{"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail} for finding in failures],
    }
    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
