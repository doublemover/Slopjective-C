#!/usr/bin/env python3
"""Fail-closed contract checker for M251-C003 runtime metadata object inspection harness."""

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
MODE = "m251-c003-runtime-metadata-object-inspection-harness-v1"
CONTRACT_ID = "objc3c-runtime-metadata-object-inspection-harness/m251-c003-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_metadata_object_inspection_harness_conformance_matrix_implementation_c003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
)
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_PROBE_ROOT = (
    ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-metadata-object-inspection"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-C003/runtime_metadata_object_inspection_harness_summary.json"
)
EXPECTED_SECTIONS = (
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
)
EXPECTED_SYMBOLS = (
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
)
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


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-DOC-EXP-01",
        "# M251 Metadata Object Inspection Harness Conformance Matrix Implementation Expectations (C003)",
    ),
    SnippetCheck(
        "M251-C003-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-metadata-object-inspection-harness/m251-c003-v1`",
    ),
    SnippetCheck(
        "M251-C003-DOC-EXP-03",
        "`Objc3RuntimeMetadataObjectInspectionHarnessSummary` becomes the canonical lane-C inspection matrix packet.",
    ),
    SnippetCheck(
        "M251-C003-DOC-EXP-04",
        "`llvm-readobj --sections module.obj`",
    ),
    SnippetCheck(
        "M251-C003-DOC-EXP-05",
        "`llvm-objdump --syms module.obj`",
    ),
    SnippetCheck(
        "M251-C003-DOC-EXP-06",
        "`tmp/reports/m251/M251-C003/runtime_metadata_object_inspection_harness_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-DOC-PKT-01",
        "# M251-C003 Metadata Object Inspection Harness Conformance Matrix Implementation Packet",
    ),
    SnippetCheck("M251-C003-DOC-PKT-02", "Packet: `M251-C003`"),
    SnippetCheck("M251-C003-DOC-PKT-03", "- `M251-C002`"),
    SnippetCheck(
        "M251-C003-DOC-PKT-04",
        "Publish matrix row `zero-descriptor-section-inventory` mapped to `llvm-readobj --sections module.obj`.",
    ),
    SnippetCheck(
        "M251-C003-DOC-PKT-05",
        "Publish matrix row `zero-descriptor-symbol-inventory` mapped to `llvm-objdump --syms module.obj`.",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-ARC-01",
        "M251 lane-C C003 metadata object inspection harness conformance matrix implementation",
    ),
    SnippetCheck(
        "M251-C003-ARC-02",
        "m251_metadata_object_inspection_harness_conformance_matrix_implementation_c003_expectations.md",
    ),
    SnippetCheck(
        "M251-C003-ARC-03",
        "canonical zero-descriptor object inspection matrix stays published through manifest/IR surfaces and backed by real llvm-readobj/llvm-objdump evidence on emitted objects",
    ),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-NDOC-01",
        "## Metadata object inspection harness (M251-C003)",
    ),
    SnippetCheck(
        "M251-C003-NDOC-02",
        "`Objc3RuntimeMetadataObjectInspectionHarnessSummary`",
    ),
    SnippetCheck(
        "M251-C003-NDOC-03",
        "`!objc3.objc_runtime_metadata_object_inspection`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-SPC-01",
        "## M251 runtime metadata object inspection harness (C003)",
    ),
    SnippetCheck(
        "M251-C003-SPC-02",
        "`Objc3RuntimeMetadataObjectInspectionHarnessSummary` to remain the single lane-C inspection matrix packet",
    ),
    SnippetCheck(
        "M251-C003-SPC-03",
        "`!objc3.objc_runtime_metadata_object_inspection`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-META-01",
        "## M251 runtime metadata object inspection harness metadata anchors (C003)",
    ),
    SnippetCheck(
        "M251-C003-META-02",
        "contract id `objc3c-runtime-metadata-object-inspection-harness/m251-c003-v1`",
    ),
    SnippetCheck(
        "M251-C003-META-03",
        "`zero-descriptor-section-inventory` and `zero-descriptor-symbol-inventory`",
    ),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C003-AST-01", "kObjc3RuntimeMetadataObjectInspectionContractId"),
    SnippetCheck("M251-C003-AST-02", "kObjc3RuntimeMetadataObjectInspectionFixturePath"),
    SnippetCheck("M251-C003-AST-03", "kObjc3RuntimeMetadataObjectInspectionSectionCommand"),
    SnippetCheck("M251-C003-AST-04", "kObjc3RuntimeMetadataObjectInspectionSymbolCommand"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-TYP-01",
        "struct Objc3RuntimeMetadataObjectInspectionHarnessSummary {",
    ),
    SnippetCheck("M251-C003-TYP-02", "bool matrix_published = false;"),
    SnippetCheck("M251-C003-TYP-03", "std::string section_inventory_command ="),
    SnippetCheck("M251-C003-TYP-04", "summary.matrix_row_count == 2u"),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-ART-01",
        "BuildRuntimeMetadataObjectInspectionHarnessSummary(",
    ),
    SnippetCheck(
        "M251-C003-ART-02",
        "runtime metadata object inspection harness prerequisites are not ready",
    ),
    SnippetCheck(
        "M251-C003-ART-03",
        "runtime_metadata_object_inspection_contract_id",
    ),
    SnippetCheck(
        "M251-C003-ART-04",
        "runtime_metadata_object_inspection_matrix_row_count",
    ),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-IRH-01",
        "std::string runtime_metadata_object_inspection_contract_id;",
    ),
    SnippetCheck(
        "M251-C003-IRH-02",
        "bool runtime_metadata_object_inspection_matrix_published = false;",
    ),
    SnippetCheck(
        "M251-C003-IRH-03",
        "std::string runtime_metadata_object_inspection_symbol_inventory_command;",
    ),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-IRC-01",
        "; runtime_metadata_object_inspection = ",
    ),
    SnippetCheck(
        "M251-C003-IRC-02",
        "!objc3.objc_runtime_metadata_object_inspection = !{!50}",
    ),
    SnippetCheck(
        "M251-C003-IRC-03",
        "runtime_metadata_object_inspection_matrix_published",
    ),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-DRV-01",
        "llvm-readobj/llvm-objdump matrix so object inspection remains tied",
    ),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-SHIM-01",
        "M251-C003 object inspection harness: emitted objects are now inspected with",
    ),
    SnippetCheck(
        "M251-C003-SHIM-02",
        "validates compiler layout and retention rather than pretending the shim is a native runtime.",
    ),
)

FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-FIX-01",
        "module runtimeMetadataObjectInspectionZeroDescriptor;",
    ),
    SnippetCheck("M251-C003-FIX-02", "fn main() {"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-C003-PKG-01",
        '"check:objc3c:m251-c003-metadata-object-inspection-harness-conformance-matrix-implementation-contract": "python scripts/check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract.py"',
    ),
    SnippetCheck(
        "M251-C003-PKG-02",
        '"test:tooling:m251-c003-metadata-object-inspection-harness-conformance-matrix-implementation-contract": "python -m pytest tests/tooling/test_check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract.py -q"',
    ),
    SnippetCheck(
        "M251-C003-PKG-03",
        '"check:objc3c:m251-c003-lane-c-readiness": "npm run check:objc3c:m251-c002-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m251-c003-metadata-object-inspection-harness-conformance-matrix-implementation-contract && npm run test:tooling:m251-c003-metadata-object-inspection-harness-conformance-matrix-implementation-contract"',
    ),
)


ARTIFACT_SNIPPETS: dict[str, tuple[SnippetCheck, ...]] = {
    "expectations_doc": EXPECTATIONS_SNIPPETS,
    "packet_doc": PACKET_SNIPPETS,
    "architecture_doc": ARCHITECTURE_SNIPPETS,
    "native_doc": NATIVE_DOC_SNIPPETS,
    "lowering_spec": LOWERING_SPEC_SNIPPETS,
    "metadata_spec": METADATA_SPEC_SNIPPETS,
    "ast_header": AST_SNIPPETS,
    "frontend_types": FRONTEND_TYPES_SNIPPETS,
    "frontend_artifacts": FRONTEND_ARTIFACTS_SNIPPETS,
    "ir_emitter_h": IR_EMITTER_H_SNIPPETS,
    "ir_emitter_cpp": IR_EMITTER_CPP_SNIPPETS,
    "driver_cpp": DRIVER_SNIPPETS,
    "runtime_shim": RUNTIME_SHIM_SNIPPETS,
    "fixture": FIXTURE_SNIPPETS,
    "package_json": PACKAGE_SNIPPETS,
}


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
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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
            failures.append(
                Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}")
            )
    return passed


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


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


def manifest_value(payload: object, key: str, default: object) -> object:
    found = find_first_key(payload, key)
    return default if found is MISSING else found


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
    symbols: list[str] = []
    for symbol in EXPECTED_SYMBOLS:
        if symbol in objdump_stdout:
            symbols.append(symbol)
    return symbols


def run_dynamic_probes(
    args: argparse.Namespace, failures: list[Finding]
) -> tuple[list[dict[str, object]], int, int, dict[str, str]]:
    cases: list[dict[str, object]] = []
    checks_total = 0
    checks_passed = 0
    tool_paths: dict[str, str] = {}
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)

    llvm_readobj = resolve_tool("llvm-readobj.exe", args.llvm_readobj)
    llvm_objdump = resolve_tool("llvm-objdump.exe", args.llvm_objdump)
    if llvm_readobj is None:
        failures.append(Finding("dynamic", "M251-C003-TOOL-READOBJ", "llvm-readobj.exe not found"))
    else:
        tool_paths["llvm_readobj"] = display_path(llvm_readobj)
    if llvm_objdump is None:
        failures.append(Finding("dynamic", "M251-C003-TOOL-OBJDUMP", "llvm-objdump.exe not found"))
    else:
        tool_paths["llvm_objdump"] = display_path(llvm_objdump)

    out_dir = probe_root / "zero_descriptor"
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_cmd = [
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_cmd, ROOT)

    manifest_path = out_dir / "module.manifest.json"
    object_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    backend_marker = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}

    manifest_case = {
        "case_id": "M251-C003-CASE-MANIFEST-ZERO-DESCRIPTOR",
        "fixture": display_path(args.fixture),
        "out_dir": display_path(out_dir),
        "process_exit_code": compile_result.returncode,
        "manifest_exists": manifest_path.exists(),
        "runtime_metadata_object_inspection_contract_id": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_contract_id", ""
        ),
        "runtime_metadata_object_inspection_scaffold_contract_id": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_scaffold_contract_id", ""
        ),
        "runtime_metadata_object_inspection_matrix_published": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_matrix_published", False
        ),
        "runtime_metadata_object_inspection_fail_closed": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_fail_closed", False
        ),
        "runtime_metadata_object_inspection_uses_llvm_readobj": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_uses_llvm_readobj", False
        ),
        "runtime_metadata_object_inspection_uses_llvm_objdump": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_uses_llvm_objdump", False
        ),
        "runtime_metadata_object_inspection_matrix_row_count": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_matrix_row_count", 0
        ),
        "runtime_metadata_object_inspection_fixture_path": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_fixture_path", ""
        ),
        "runtime_metadata_object_inspection_emit_prefix": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_emit_prefix", ""
        ),
        "runtime_metadata_object_inspection_object_relative_path": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_object_relative_path", ""
        ),
        "runtime_metadata_object_inspection_section_inventory_row_key": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_section_inventory_row_key", ""
        ),
        "runtime_metadata_object_inspection_section_inventory_command": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_section_inventory_command", ""
        ),
        "runtime_metadata_object_inspection_symbol_inventory_row_key": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_symbol_inventory_row_key", ""
        ),
        "runtime_metadata_object_inspection_symbol_inventory_command": manifest_value(
            manifest_payload,
            "runtime_metadata_object_inspection_symbol_inventory_command", ""
        ),
        "object_backend": backend_marker,
    }

    manifest_success = (
        compile_result.returncode == 0
        and manifest_path.exists()
        and manifest_case["runtime_metadata_object_inspection_contract_id"] == CONTRACT_ID
        and manifest_case["runtime_metadata_object_inspection_scaffold_contract_id"]
        == "objc3c-runtime-metadata-section-scaffold/m251-c002-v1"
        and manifest_case["runtime_metadata_object_inspection_matrix_published"] is True
        and manifest_case["runtime_metadata_object_inspection_fail_closed"] is True
        and manifest_case["runtime_metadata_object_inspection_uses_llvm_readobj"] is True
        and manifest_case["runtime_metadata_object_inspection_uses_llvm_objdump"] is True
        and manifest_case["runtime_metadata_object_inspection_matrix_row_count"] == 2
        and manifest_case["runtime_metadata_object_inspection_fixture_path"]
        == "tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
        and manifest_case["runtime_metadata_object_inspection_emit_prefix"] == "module"
        and manifest_case["runtime_metadata_object_inspection_object_relative_path"] == "module.obj"
        and manifest_case["runtime_metadata_object_inspection_section_inventory_row_key"]
        == "zero-descriptor-section-inventory"
        and manifest_case["runtime_metadata_object_inspection_section_inventory_command"]
        == "llvm-readobj --sections module.obj"
        and manifest_case["runtime_metadata_object_inspection_symbol_inventory_row_key"]
        == "zero-descriptor-symbol-inventory"
        and manifest_case["runtime_metadata_object_inspection_symbol_inventory_command"]
        == "llvm-objdump --syms module.obj"
        and backend_marker in {"llvm-direct", "llc"}
    )
    manifest_case["status"] = 0 if manifest_success else 1
    manifest_case["success"] = manifest_success
    cases.append(manifest_case)
    checks_total += 1
    if manifest_success:
        checks_passed += 1
    else:
        failures.append(
            Finding("dynamic", "M251-C003-CASE-MANIFEST", "manifest inspection matrix probe failed")
        )

    object_case = {
        "case_id": "M251-C003-CASE-OBJECT-ZERO-DESCRIPTOR",
        "fixture": display_path(args.fixture),
        "out_dir": display_path(out_dir),
        "compile_exit_code": compile_result.returncode,
        "object_exists": object_path.exists(),
        "object_backend": backend_marker,
        "readobj_exit_code": None,
        "objdump_exit_code": None,
        "metadata_sections": [],
        "retained_symbols": [],
    }

    readobj_result = None
    objdump_result = None
    if compile_result.returncode == 0 and object_path.exists() and llvm_readobj is not None and llvm_objdump is not None:
        readobj_result = run_command([str(llvm_readobj), "--sections", str(object_path)], ROOT)
        objdump_result = run_command([str(llvm_objdump), "--syms", str(object_path)], ROOT)
        metadata_sections = extract_metadata_sections(readobj_result.stdout)
        retained_symbols = extract_retained_symbols(objdump_result.stdout)
        object_case["readobj_exit_code"] = readobj_result.returncode
        object_case["objdump_exit_code"] = objdump_result.returncode
        object_case["metadata_sections"] = metadata_sections
        object_case["retained_symbols"] = retained_symbols
        object_success = (
            readobj_result.returncode == 0
            and objdump_result.returncode == 0
            and sorted(section["name"] for section in metadata_sections) == sorted(EXPECTED_SECTIONS)
            and all(section["raw_data_size"] == 8 for section in metadata_sections)
            and sorted(retained_symbols) == sorted(EXPECTED_SYMBOLS)
            and backend_marker in {"llvm-direct", "llc"}
        )
    else:
        object_success = False

    object_case["status"] = 0 if object_success else 1
    object_case["success"] = object_success
    cases.append(object_case)
    checks_total += 1
    if object_success:
        checks_passed += 1
    else:
        failures.append(
            Finding("dynamic", "M251-C003-CASE-OBJECT", "object inspection probe failed")
        )

    return cases, checks_total, checks_passed, tool_paths


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifact_paths: dict[str, Path] = {
        "expectations_doc": args.expectations_doc,
        "packet_doc": args.packet_doc,
        "architecture_doc": args.architecture_doc,
        "native_doc": args.native_doc,
        "lowering_spec": args.lowering_spec,
        "metadata_spec": args.metadata_spec,
        "ast_header": args.ast_header,
        "frontend_types": args.frontend_types,
        "frontend_artifacts": args.frontend_artifacts,
        "ir_emitter_h": args.ir_emitter_h,
        "ir_emitter_cpp": args.ir_emitter_cpp,
        "driver_cpp": args.driver_cpp,
        "runtime_shim": args.runtime_shim,
        "fixture": args.fixture,
        "package_json": args.package_json,
    }

    for artifact_name, path in artifact_paths.items():
        checks_total += len(ARTIFACT_SNIPPETS[artifact_name])
        checks_passed += ensure_snippets(path, ARTIFACT_SNIPPETS[artifact_name], failures)

    dynamic_cases: list[dict[str, object]] = []
    dynamic_checks_total = 0
    dynamic_checks_passed = 0
    tool_paths: dict[str, str] = {}
    if not args.skip_dynamic_probes:
        dynamic_cases, dynamic_checks_total, dynamic_checks_passed, tool_paths = run_dynamic_probes(
            args, failures
        )
        checks_total += dynamic_checks_total
        checks_passed += dynamic_checks_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "tool_paths": tool_paths,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
