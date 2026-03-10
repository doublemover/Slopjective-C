#!/usr/bin/env python3
"""Fail-closed contract checker for M257-C002 ivar offset/layout emission."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-c002-ivar-offset-and-layout-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-executable-ivar-layout-emission/m257-c002-v1"
BOUNDARY_COMMENT_PREFIX = "; executable_ivar_layout_emission = contract=objc3c-executable-ivar-layout-emission/m257-c002-v1"
NAMED_METADATA_PREFIX = "!objc3.objc_executable_ivar_layout_emission = !{"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_ivar_offset_and_layout_emission_core_feature_implementation_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_c002_ivar_offset_and_layout_emission_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_ivar_layout_offset_emission_positive.objc3"
DEFAULT_MIXED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "ivar-layout-emission"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m257/M257-C002/ivar_layout_offset_emission_summary.json")
DEFAULT_LLVM_READOBJ = "llvm-readobj"

OFFSET_GLOBAL_RE = re.compile(r"^@__objc3_meta_ivar_offset_\d{4} = private global i64 \d+, section \"objc3.runtime.ivar_descriptors\", align 8$", re.MULTILINE)
LAYOUT_RECORD_RE = re.compile(r"^@__objc3_meta_ivar_layout_record_\d{4} = private global \{ ptr, i64, i64, i64, i64 \} ", re.MULTILINE)
LAYOUT_TABLE_RE = re.compile(r"^@__objc3_meta_ivar_layout_table_\d{4} = private global \{ ptr, i64, \[\d+ x ptr\], i64 \} ", re.MULTILINE)


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
    SnippetCheck("M257-C002-DOC-EXP-01", "# M257 Ivar Offset And Layout Emission Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M257-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M257-C002-DOC-EXP-03", "`one-retained-i64-offset-global-per-emitted-ivar-binding`"),
    SnippetCheck("M257-C002-DOC-EXP-04", "`declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size`"),
    SnippetCheck("M257-C002-DOC-EXP-05", "`tmp/reports/m257/M257-C002/ivar_layout_offset_emission_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-DOC-PKT-01", "# M257-C002 Ivar Offset And Layout Emission Core Feature Implementation Packet"),
    SnippetCheck("M257-C002-DOC-PKT-02", "Packet: `M257-C002`"),
    SnippetCheck("M257-C002-DOC-PKT-03", "Issue: `#7151`"),
    SnippetCheck("M257-C002-DOC-PKT-04", "`tests/tooling/fixtures/native/m257_ivar_layout_offset_emission_positive.objc3`"),
    SnippetCheck("M257-C002-DOC-PKT-05", "`m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-ARCH-01", "## M257 ivar offset and layout emission (C002)"),
    SnippetCheck("M257-C002-ARCH-02", "!objc3.objc_executable_ivar_layout_emission"),
    SnippetCheck("M257-C002-ARCH-03", "check:objc3c:m257-c002-lane-c-readiness"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-NDOC-01", "## Ivar offset and layout emission (M257-C002)"),
    SnippetCheck("M257-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-C002-NDOC-03", "`@__objc3_meta_ivar_offset_####`"),
    SnippetCheck("M257-C002-NDOC-04", "`!objc3.objc_executable_ivar_layout_emission`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-SPC-01", "## M257 ivar offset and layout emission (C002)"),
    SnippetCheck("M257-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-C002-SPC-03", "`declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-META-01", "## M257 ivar offset and layout emission metadata anchors (C002)"),
    SnippetCheck("M257-C002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-C002-META-03", "`@__objc3_meta_ivar_layout_table_####`"),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-AST-01", "M257-C002 ivar offset/layout emission anchor: AST-owned"),
)

SEMA_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-SEMA-01", "M257-C002 ivar offset/layout emission anchor: sema owns the canonical slot"),
)

IR_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-IHDR-01", "executable_ivar_layout_emission_contract_id"),
    SnippetCheck("M257-C002-IHDR-02", "executable_ivar_offset_global_entries"),
    SnippetCheck("M257-C002-IHDR-03", "executable_ivar_layout_table_entries"),
    SnippetCheck("M257-C002-IHDR-04", "executable_ivar_layout_owner_entries"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-IR-01", 'out << "; executable_ivar_layout_emission = "'),
    SnippetCheck("M257-C002-IR-02", 'out << "!objc3.objc_executable_ivar_layout_emission = !{!67}\\n";'),
    SnippetCheck("M257-C002-IR-03", "M257-C002 ivar offset/layout emission anchor:"),
    SnippetCheck("M257-C002-IR-04", '"layout_table", layout_table_ordinal'),
    SnippetCheck("M257-C002-IR-05", '"offset", i);'),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-ART-01", "executable_ivar_layout_emission_contract_id ="),
    SnippetCheck("M257-C002-ART-02", "executable_ivar_offset_global_entries ="),
    SnippetCheck("M257-C002-ART-03", "executable_ivar_layout_table_entries ="),
    SnippetCheck("M257-C002-ART-04", "executable_ivar_layout_emission_replay_key ="),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-LHDR-01", "kObjc3ExecutableIvarLayoutEmissionContractId"),
    SnippetCheck("M257-C002-LHDR-02", "kObjc3ExecutableIvarOffsetGlobalModel"),
    SnippetCheck("M257-C002-LHDR-03", "kObjc3ExecutableIvarLayoutTableModel"),
    SnippetCheck("M257-C002-LHDR-04", "Objc3ExecutableIvarLayoutEmissionSummary()"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C002-LCPP-01", "Objc3ExecutableIvarLayoutEmissionSummary()"),
    SnippetCheck("M257-C002-LCPP-02", "M257-C002 ivar offset/layout emission anchor"),
    SnippetCheck("M257-C002-LCPP-03", "non_goals=no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M257-C002-PKG-01",
        '"check:objc3c:m257-c002-ivar-offset-and-layout-emission": "python scripts/check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M257-C002-PKG-02",
        '"test:tooling:m257-c002-ivar-offset-and-layout-emission": "python -m pytest tests/tooling/test_check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M257-C002-PKG-03",
        '"check:objc3c:m257-c002-lane-c-readiness": "python scripts/run_m257_c002_lane_c_readiness.py"',
    ),
)


@dataclass(frozen=True)
class ProbeSpec:
    case_id: str
    fixture: Path
    probe_subdir: str
    require_exact_layout: bool


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
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-header", type=Path, default=DEFAULT_IR_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--positive-fixture", type=Path, default=DEFAULT_POSITIVE_FIXTURE)
    parser.add_argument("--mixed-fixture", type=Path, default=DEFAULT_MIXED_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-readobj", default=DEFAULT_LLVM_READOBJ)
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def resolve_command(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


def parse_readobj_section(text: str, section_name: str) -> tuple[int | None, int | None]:
    marker = f"Name: {section_name}"
    idx = text.find(marker)
    if idx < 0:
        return None, None
    tail = text[idx:]
    block = tail.split("  Section {", 1)[0]
    raw_match = re.search(r"RawDataSize:\s*(\d+)", block)
    reloc_match = re.search(r"RelocationCount:\s*(\d+)", block)
    raw_size = int(raw_match.group(1)) if raw_match else None
    relocations = int(reloc_match.group(1)) if reloc_match else None
    return raw_size, relocations


def extract_boundary_count(boundary_line: str, token: str) -> int:
    match = re.search(rf"(?:^|;){re.escape(token)}=(\d+)", boundary_line)
    return int(match.group(1)) if match else -1


def exact_layout_expectations(ir_text: str, artifact: str, case_id: str, failures: list[Finding]) -> int:
    checks_total = 0
    checks_passed = 0
    exact_snippets = (
        ("POS-NAME-COUNT", '@__objc3_meta_ivar_property_name_0000 = private constant [6 x i8] c"count\\00"'),
        ("POS-OFFSET-COUNT", '@__objc3_meta_ivar_offset_0000 = private global i64 4'),
        ("POS-RECORD-COUNT", '@__objc3_meta_ivar_layout_record_0000 = private global { ptr, i64, i64, i64, i64 } { ptr @__objc3_meta_ivar_layout_symbol_0000, i64 1, i64 4, i64 4, i64 4 }'),
        ("POS-DESC-COUNT", '@__objc3_meta_ivar_0000 = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64 } { ptr @__objc3_meta_ivar_owner_identity_0000, ptr @__objc3_meta_ivar_declaration_owner_identity_0000, ptr @__objc3_meta_ivar_export_owner_identity_0000, ptr @__objc3_meta_ivar_property_owner_identity_0000, ptr @__objc3_meta_ivar_property_name_0000, ptr @__objc3_meta_ivar_binding_0000, ptr @__objc3_meta_ivar_layout_record_0000, ptr @__objc3_meta_ivar_offset_0000, i64 1, i64 4, i64 4, i64 4 }'),
        ("POS-NAME-ENABLED", '@__objc3_meta_ivar_property_name_0001 = private constant [8 x i8] c"enabled\\00"'),
        ("POS-OFFSET-ENABLED", '@__objc3_meta_ivar_offset_0001 = private global i64 0'),
        ("POS-RECORD-ENABLED", '@__objc3_meta_ivar_layout_record_0001 = private global { ptr, i64, i64, i64, i64 } { ptr @__objc3_meta_ivar_layout_symbol_0001, i64 0, i64 0, i64 1, i64 1 }'),
        ("POS-DESC-ENABLED", '@__objc3_meta_ivar_0001 = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64 } { ptr @__objc3_meta_ivar_owner_identity_0001, ptr @__objc3_meta_ivar_declaration_owner_identity_0001, ptr @__objc3_meta_ivar_export_owner_identity_0001, ptr @__objc3_meta_ivar_property_owner_identity_0001, ptr @__objc3_meta_ivar_property_name_0001, ptr @__objc3_meta_ivar_binding_0001, ptr @__objc3_meta_ivar_layout_record_0001, ptr @__objc3_meta_ivar_offset_0001, i64 0, i64 0, i64 1, i64 1 }'),
        ("POS-NAME-TOKEN", '@__objc3_meta_ivar_property_name_0002 = private constant [6 x i8] c"token\\00"'),
        ("POS-OFFSET-TOKEN", '@__objc3_meta_ivar_offset_0002 = private global i64 8'),
        ("POS-RECORD-TOKEN", '@__objc3_meta_ivar_layout_record_0002 = private global { ptr, i64, i64, i64, i64 } { ptr @__objc3_meta_ivar_layout_symbol_0002, i64 2, i64 8, i64 8, i64 8 }'),
        ("POS-DESC-TOKEN", '@__objc3_meta_ivar_0002 = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64 } { ptr @__objc3_meta_ivar_owner_identity_0002, ptr @__objc3_meta_ivar_declaration_owner_identity_0002, ptr @__objc3_meta_ivar_export_owner_identity_0002, ptr @__objc3_meta_ivar_property_owner_identity_0002, ptr @__objc3_meta_ivar_property_name_0002, ptr @__objc3_meta_ivar_binding_0002, ptr @__objc3_meta_ivar_layout_record_0002, ptr @__objc3_meta_ivar_offset_0002, i64 2, i64 8, i64 8, i64 8 }'),
        ("POS-TABLE", '@__objc3_meta_ivar_layout_table_0000 = private global { ptr, i64, [3 x ptr], i64 } { ptr @__objc3_meta_ivar_layout_owner_0000, i64 3, [3 x ptr] [ptr @__objc3_meta_ivar_0000, ptr @__objc3_meta_ivar_0001, ptr @__objc3_meta_ivar_0002], i64 16 }'),
    )
    for suffix, snippet in exact_snippets:
        checks_total += 1
        checks_passed += require(snippet in ir_text, artifact, f"{case_id}-{suffix}", f"IR must contain exact layout proof snippet: {snippet}", failures)
    return checks_total, checks_passed


def run_native_probe(
    *,
    args: argparse.Namespace,
    spec: ProbeSpec,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / spec.probe_subdir
    command = [
        str(args.native_exe.resolve()),
        str(spec.fixture.resolve()),
        "--out-dir",
        str(probe_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)

    ir_path = probe_dir / "module.ll"
    obj_path = probe_dir / "module.obj"
    case: dict[str, Any] = {
        "case_id": spec.case_id,
        "fixture": display_path(spec.fixture),
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), f"{spec.case_id}-NATIVE-EXISTS", "objc3c-native.exe is missing", failures)
    checks_total += 1
    checks_passed += require(spec.fixture.exists(), display_path(spec.fixture), f"{spec.case_id}-FIXTURE", "fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(ir_path), f"{spec.case_id}-STATUS", "native probe must exit 0", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), f"{spec.case_id}-IR", "native probe must emit module.ll", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), f"{spec.case_id}-OBJ", "native probe must emit module.obj", failures)

    if not ir_path.exists() or not obj_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    offset_global_entries = extract_boundary_count(boundary_line, "offset_global_entries")
    layout_table_entries = extract_boundary_count(boundary_line, "layout_table_entries")
    layout_owner_entries = extract_boundary_count(boundary_line, "layout_owner_entries")
    case["boundary_line"] = boundary_line
    case["offset_global_entries"] = offset_global_entries
    case["layout_table_entries"] = layout_table_entries
    case["layout_owner_entries"] = layout_owner_entries
    case["offset_global_symbol_count"] = len(OFFSET_GLOBAL_RE.findall(ir_text))
    case["layout_record_symbol_count"] = len(LAYOUT_RECORD_RE.findall(ir_text))
    case["layout_table_symbol_count"] = len(LAYOUT_TABLE_RE.findall(ir_text))

    common_checks = (
        (bool(boundary_line), f"{spec.case_id}-BOUNDARY", "IR must publish the ivar layout emission boundary line"),
        ("descriptor_model=ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment" in boundary_line, f"{spec.case_id}-DESCRIPTOR-MODEL", "boundary line must carry the descriptor model"),
        ("offset_global_model=one-retained-i64-offset-global-per-emitted-ivar-binding" in boundary_line, f"{spec.case_id}-OFFSET-MODEL", "boundary line must carry the offset-global model"),
        ("layout_table_model=declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size" in boundary_line, f"{spec.case_id}-TABLE-MODEL", "boundary line must carry the layout-table model"),
        ("scope_model=sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation" in boundary_line, f"{spec.case_id}-SCOPE-MODEL", "boundary line must carry the scope model"),
        ("fail_closed_model=no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis" in boundary_line, f"{spec.case_id}-FAIL-CLOSED", "boundary line must carry the fail-closed model"),
        (NAMED_METADATA_PREFIX in ir_text, f"{spec.case_id}-NAMED-METADATA", "IR must publish !objc3.objc_executable_ivar_layout_emission"),
        (case["offset_global_symbol_count"] >= 1, f"{spec.case_id}-OFFSET-GLOBALS", "IR must emit at least one ivar offset global"),
        (case["layout_record_symbol_count"] >= 1, f"{spec.case_id}-LAYOUT-RECORDS", "IR must emit at least one ivar layout record"),
        (case["layout_table_symbol_count"] >= 1, f"{spec.case_id}-LAYOUT-TABLES", "IR must emit at least one ivar layout table"),
        ("@llvm.used = appending global" in ir_text, f"{spec.case_id}-LLVM-USED", "IR must retain emitted layout payloads via @llvm.used"),
        (obj_path.stat().st_size > 0, f"{spec.case_id}-OBJ-SIZE", "module.obj must be non-empty"),
    )
    for condition, check_id, detail in common_checks:
        checks_total += 1
        checks_passed += require(condition, display_path(ir_path if "OBJ" not in check_id else obj_path), check_id, detail, failures)

    if spec.require_exact_layout:
        exact_total, exact_passed = exact_layout_expectations(ir_text, display_path(ir_path), spec.case_id, failures)
        checks_total += exact_total
        checks_passed += exact_passed
        exact_counts = (
            (offset_global_entries == 3, f"{spec.case_id}-OFFSET-ENTRY-COUNT", "positive proof fixture must advertise exactly three offset globals"),
            (layout_table_entries == 1, f"{spec.case_id}-TABLE-ENTRY-COUNT", "positive proof fixture must advertise exactly one layout table"),
            (layout_owner_entries == 1, f"{spec.case_id}-OWNER-ENTRY-COUNT", "positive proof fixture must advertise exactly one layout owner entry"),
            ("ptr @__objc3_meta_ivar_offset_0000" in ir_text and "ptr @__objc3_meta_ivar_layout_record_0000" in ir_text and "ptr @__objc3_meta_ivar_layout_table_0000" in ir_text, f"{spec.case_id}-LLVM-USED-TOKENS", "@llvm.used must retain emitted ivar offset globals, layout records, and layout tables"),
        )
        for condition, check_id, detail in exact_counts:
            checks_total += 1
            checks_passed += require(condition, display_path(ir_path), check_id, detail, failures)
    else:
        looser_counts = (
            (offset_global_entries >= 1, f"{spec.case_id}-OFFSET-ENTRY-COUNT", "mixed metadata fixture must advertise at least one offset global"),
            (layout_table_entries >= 1, f"{spec.case_id}-TABLE-ENTRY-COUNT", "mixed metadata fixture must advertise at least one layout table"),
            (layout_owner_entries >= 1, f"{spec.case_id}-OWNER-ENTRY-COUNT", "mixed metadata fixture must advertise at least one layout owner entry"),
        )
        for condition, check_id, detail in looser_counts:
            checks_total += 1
            checks_passed += require(condition, display_path(ir_path), check_id, detail, failures)

    llvm_readobj = resolve_command(args.llvm_readobj)
    checks_total += 1
    checks_passed += require(llvm_readobj is not None, args.llvm_readobj, f"{spec.case_id}-LLVM-READOBJ", "llvm-readobj must be available", failures)
    if llvm_readobj is not None:
        readobj_result = run_command((llvm_readobj, "--sections", "--relocations", str(obj_path)), ROOT)
        case["llvm_readobj_command"] = [llvm_readobj, "--sections", "--relocations", str(obj_path)]
        case["llvm_readobj_exit_code"] = readobj_result.returncode
        case["llvm_readobj_stdout"] = readobj_result.stdout
        checks_total += 1
        checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), f"{spec.case_id}-READOBJ-STATUS", f"llvm-readobj failed: {readobj_result.stderr.strip()}", failures)
        raw_size, relocations = parse_readobj_section(readobj_result.stdout, "objc3.runtime.ivar_descriptors")
        case["ivar_section_raw_size"] = raw_size
        case["ivar_section_relocations"] = relocations
        checks_total += 1
        checks_passed += require(raw_size is not None and raw_size > 0, display_path(obj_path), f"{spec.case_id}-IVAR-SECTION-SIZE", "objc3.runtime.ivar_descriptors must be present and non-empty", failures)
        checks_total += 1
        checks_passed += require(relocations is not None and relocations > 0, display_path(obj_path), f"{spec.case_id}-IVAR-SECTION-RELOCS", "objc3.runtime.ivar_descriptors must publish relocations", failures)

    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_header, IR_HEADER_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        for spec in (
            ProbeSpec(
                case_id="M257-C002-CASE-POSITIVE-LAYOUT",
                fixture=args.positive_fixture,
                probe_subdir="positive-layout",
                require_exact_layout=True,
            ),
            ProbeSpec(
                case_id="M257-C002-CASE-MIXED-METADATA",
                fixture=args.mixed_fixture,
                probe_subdir="mixed-metadata",
                require_exact_layout=False,
            ),
        ):
            case_total, case_passed, case = run_native_probe(args=args, spec=spec, failures=failures)
            checks_total += case_total
            checks_passed += case_passed
            dynamic_cases.append(case)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "evidence_path": str(args.summary_out).replace("\\", "/"),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
