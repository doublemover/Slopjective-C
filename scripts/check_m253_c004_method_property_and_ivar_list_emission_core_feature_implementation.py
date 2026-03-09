#!/usr/bin/env python3
"""Fail-closed contract checker for M253-C004 method/property/ivar payload emission."""

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
MODE = "m253-c004-method-property-and-ivar-list-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-member-table-emission/m253-c004-v1"
BOUNDARY_COMMENT_PREFIX = (
    "; runtime_metadata_member_table_emission = contract="
    "objc3c-runtime-member-table-emission/m253-c004-v1"
)
NAMED_METADATA_LINE = '!objc3.objc_runtime_member_table_emission = !{!58}'
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_method_property_and_ivar_list_emission_core_feature_implementation_c004_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_c004_method_property_and_ivar_list_emission_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "member-table-emission"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-C004/method_property_and_ivar_list_emission_summary.json")
DEFAULT_LLVM_READOBJ = "llvm-readobj"
DEFAULT_LLVM_OBJDUMP = "llvm-objdump"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class ProbeSpec:
    case_id: str
    fixture: Path
    probe_subdir: str
    expected_method_list_bundle_count: int
    expected_method_entry_count: int
    expected_property_descriptor_count: int
    expected_ivar_descriptor_count: int
    expected_graph_method_nodes: int
    expected_graph_property_nodes: int
    expected_graph_ivar_nodes: int
    expected_class_instance_method_lists: int
    expected_class_class_method_lists: int
    expected_protocol_instance_method_lists: int
    expected_category_instance_method_lists: int
    expected_category_class_method_lists: int
    symbol_tokens: tuple[str, ...]
    section_expectations: tuple[tuple[str, int, int], ...]


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-DOC-EXP-01", "# M253 Method Property And Ivar List Emission Core Feature Implementation Expectations (C004)"),
    SnippetCheck("M253-C004-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-C004-DOC-EXP-03", "`owner-scoped-method-table-globals-with-inline-entry-records`"),
    SnippetCheck("M253-C004-DOC-EXP-04", "`@__objc3_meta_class_instance_methods_0001`"),
    SnippetCheck("M253-C004-DOC-EXP-05", "`@__objc3_meta_property_0001`"),
    SnippetCheck("M253-C004-DOC-EXP-06", "`@__objc3_meta_ivar_0000`"),
    SnippetCheck("M253-C004-DOC-EXP-07", "`tmp/reports/m253/M253-C004/method_property_and_ivar_list_emission_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-DOC-PKT-01", "# M253-C004 Method Property And Ivar List Emission Core Feature Implementation Packet"),
    SnippetCheck("M253-C004-DOC-PKT-02", "Packet: `M253-C004`"),
    SnippetCheck("M253-C004-DOC-PKT-03", "- `M253-C003`"),
    SnippetCheck("M253-C004-DOC-PKT-04", "owner-scoped-method-table-globals-with-inline-entry-records"),
    SnippetCheck("M253-C004-DOC-PKT-05", "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-ARCH-01", "M253 lane-C C004 method/property/ivar list emission anchors explicit"),
    SnippetCheck("M253-C004-ARCH-02", "runtime_metadata_method_list_bundles_lexicographic"),
    SnippetCheck("M253-C004-ARCH-03", "real property/ivar descriptor bytes"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-NDOC-01", "## Method, property, and ivar payload emission (M253-C004)"),
    SnippetCheck("M253-C004-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C004-NDOC-03", "`!objc3.objc_runtime_member_table_emission`"),
    SnippetCheck("M253-C004-NDOC-04", "`@__objc3_meta_property_0001`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-SPC-01", "## M253 method/property/ivar payload emission (C004)"),
    SnippetCheck("M253-C004-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C004-SPC-03", "owner-scoped-method-table-globals-with-inline-entry-records"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-META-01", "## M253 method/property/ivar payload metadata anchors (C004)"),
    SnippetCheck("M253-C004-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C004-META-03", "`__objc3_meta_property_0001`"),
    SnippetCheck("M253-C004-META-04", "`__objc3_meta_ivar_0000`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-LHDR-01", "kObjc3RuntimeMemberTableEmissionContractId"),
    SnippetCheck("M253-C004-LHDR-02", "kObjc3RuntimeMethodListEmissionPayloadModel"),
    SnippetCheck("M253-C004-LHDR-03", "kObjc3RuntimePropertyDescriptorEmissionPayloadModel"),
    SnippetCheck("M253-C004-LHDR-04", "kObjc3RuntimeIvarDescriptorEmissionPayloadModel"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-LCPP-01", "Objc3RuntimeMetadataMemberTableEmissionSummary()"),
    SnippetCheck("M253-C004-LCPP-02", "M253-C004 member-table data emission anchor"),
    SnippetCheck("M253-C004-LCPP-03", "non_goals=no-selector-string-pool-or-runtime-registration"),
)

IR_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-IHDR-01", "struct Objc3IRRuntimeMetadataMethodListBundle {"),
    SnippetCheck("M253-C004-IHDR-02", "struct Objc3IRRuntimeMetadataPropertyBundle {"),
    SnippetCheck("M253-C004-IHDR-03", "struct Objc3IRRuntimeMetadataIvarBundle {"),
    SnippetCheck("M253-C004-IHDR-04", "runtime_metadata_method_list_bundles_lexicographic;"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-IR-01", 'out << "!objc3.objc_runtime_member_table_emission = !{!58}\\n";'),
    SnippetCheck("M253-C004-IR-02", 'out << "; runtime_metadata_member_table_emission = "'),
    SnippetCheck("M253-C004-IR-03", "emit_method_list_bundles_for_family"),
    SnippetCheck("M253-C004-IR-04", "emit_property_descriptor_section"),
    SnippetCheck("M253-C004-IR-05", "emit_ivar_descriptor_section"),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-ART-01", "runtime_metadata_member_table_emission_contract_id"),
    SnippetCheck("M253-C004-ART-02", "runtime_metadata_method_list_bundles_lexicographic"),
    SnippetCheck("M253-C004-ART-03", "runtime_metadata_property_bundles_lexicographic"),
    SnippetCheck("M253-C004-ART-04", "runtime_metadata_ivar_bundles_lexicographic"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C004-PROC-01", "M253-C004 member-table data emission anchor"),
    SnippetCheck("M253-C004-PROC-02", "method/property/ivar payloads verbatim"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-C004-PKG-01",
        '"check:objc3c:m253-c004-method-property-and-ivar-list-emission-core-feature-implementation": "python scripts/check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M253-C004-PKG-02",
        '"test:tooling:m253-c004-method-property-and-ivar-list-emission-core-feature-implementation": "python -m pytest tests/tooling/test_check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M253-C004-PKG-03",
        '"check:objc3c:m253-c004-lane-c-readiness": "npm run check:objc3c:m253-c003-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-c004-method-property-and-ivar-list-emission-core-feature-implementation && npm run test:tooling:m253-c004-method-property-and-ivar-list-emission-core-feature-implementation"',
    ),
)

CLASS_INSTANCE_METHOD_LIST_RE = re.compile(r"^@__objc3_meta_class_instance_methods_\d{4} = private global ", re.MULTILINE)
CLASS_CLASS_METHOD_LIST_RE = re.compile(r"^@__objc3_meta_class_class_methods_\d{4} = private global ", re.MULTILINE)
PROTOCOL_INSTANCE_METHOD_LIST_RE = re.compile(r"^@__objc3_meta_protocol_instance_methods_\d{4} = private global ", re.MULTILINE)
CATEGORY_INSTANCE_METHOD_LIST_RE = re.compile(r"^@__objc3_meta_category_instance_methods_\d{4} = private global ", re.MULTILINE)
CATEGORY_CLASS_METHOD_LIST_RE = re.compile(r"^@__objc3_meta_category_class_methods_\d{4} = private global ", re.MULTILINE)
PROPERTY_DESCRIPTOR_RE = re.compile(
    r"^@__objc3_meta_property_\d{4} = private global \{ ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 \}",
    re.MULTILINE,
)
IVAR_DESCRIPTOR_RE = re.compile(
    r"^@__objc3_meta_ivar_\d{4} = private global \{ ptr, ptr, ptr, ptr, ptr, ptr \}",
    re.MULTILINE,
)
PROPERTY_PLACEHOLDER_RE = re.compile(r"^@__objc3_meta_property_\d{4} = private global \[1 x i8\] zeroinitializer", re.MULTILINE)
IVAR_PLACEHOLDER_RE = re.compile(r"^@__objc3_meta_ivar_\d{4} = private global \[1 x i8\] zeroinitializer", re.MULTILINE)
LLVM_USED_RE = re.compile(r"^@llvm\.used = appending global \[(?P<count>\d+) x ptr\]", re.MULTILINE)


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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-header", type=Path, default=DEFAULT_IR_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-readobj", default=DEFAULT_LLVM_READOBJ)
    parser.add_argument("--llvm-objdump", default=DEFAULT_LLVM_OBJDUMP)
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


def make_probe_specs(args: argparse.Namespace) -> tuple[ProbeSpec, ...]:
    return (
        ProbeSpec(
            case_id="M253-C004-CASE-CLASS-PROTOCOL-PROPERTY-IVAR",
            fixture=args.class_fixture,
            probe_subdir="class-protocol-property-ivar",
            expected_method_list_bundle_count=5,
            expected_method_entry_count=7,
            expected_property_descriptor_count=5,
            expected_ivar_descriptor_count=2,
            expected_graph_method_nodes=7,
            expected_graph_property_nodes=5,
            expected_graph_ivar_nodes=2,
            expected_class_instance_method_lists=2,
            expected_class_class_method_lists=2,
            expected_protocol_instance_method_lists=1,
            expected_category_instance_method_lists=0,
            expected_category_class_method_lists=0,
            symbol_tokens=(
                "__objc3_meta_class_instance_methods_0001",
                "__objc3_meta_class_class_methods_0000",
                "__objc3_meta_protocol_instance_methods_0004",
                "__objc3_meta_property_0001",
                "__objc3_meta_ivar_0000",
            ),
            section_expectations=(
                ("objc3.runtime.class_descriptors", 256, 8),
                ("objc3.runtime.protocol_descriptors", 128, 4),
                ("objc3.runtime.property_descriptors", 256, 8),
                ("objc3.runtime.ivar_descriptors", 128, 4),
            ),
        ),
        ProbeSpec(
            case_id="M253-C004-CASE-CATEGORY-PROTOCOL-PROPERTY",
            fixture=args.category_fixture,
            probe_subdir="category-protocol-property",
            expected_method_list_bundle_count=3,
            expected_method_entry_count=5,
            expected_property_descriptor_count=5,
            expected_ivar_descriptor_count=2,
            expected_graph_method_nodes=5,
            expected_graph_property_nodes=5,
            expected_graph_ivar_nodes=2,
            expected_class_instance_method_lists=0,
            expected_class_class_method_lists=0,
            expected_protocol_instance_method_lists=1,
            expected_category_instance_method_lists=2,
            expected_category_class_method_lists=0,
            symbol_tokens=(
                "__objc3_meta_category_instance_methods_0000",
                "__objc3_meta_protocol_instance_methods_0002",
                "__objc3_meta_property_0001",
                "__objc3_meta_ivar_0000",
            ),
            section_expectations=(
                ("objc3.runtime.protocol_descriptors", 128, 4),
                ("objc3.runtime.category_descriptors", 256, 8),
                ("objc3.runtime.property_descriptors", 256, 8),
                ("objc3.runtime.ivar_descriptors", 128, 4),
            ),
        ),
    )


def run_native_probe(
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
    manifest_path = probe_dir / "module.manifest.json"
    backend_path = probe_dir / "module.object-backend.txt"
    runtime_metadata_bin_path = probe_dir / "module.runtime-metadata.bin"
    diagnostics_path = probe_dir / "module.diagnostics.txt"

    case: dict[str, Any] = {
        "case_id": spec.case_id,
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
        "manifest_path": display_path(manifest_path),
        "object_backend_path": display_path(backend_path),
        "runtime_metadata_bin_path": display_path(runtime_metadata_bin_path),
        "diagnostics_path": display_path(diagnostics_path),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), f"{spec.case_id}-EXE", "objc3c-native.exe is missing", failures)
    checks_total += 1
    checks_passed += require(spec.fixture.exists(), display_path(spec.fixture), f"{spec.case_id}-FIXTURE", "fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(spec.fixture), f"{spec.case_id}-STATUS", f"native probe exited with {result.returncode}: {result.stderr.strip()}", failures)

    for path, check_id, detail in (
        (ir_path, f"{spec.case_id}-IR", "native probe must emit module.ll"),
        (obj_path, f"{spec.case_id}-OBJ", "native probe must emit module.obj"),
        (manifest_path, f"{spec.case_id}-MANIFEST", "native probe must emit module.manifest.json"),
        (backend_path, f"{spec.case_id}-BACKEND", "native probe must emit module.object-backend.txt"),
        (runtime_metadata_bin_path, f"{spec.case_id}-BIN", "native probe must emit module.runtime-metadata.bin"),
        (diagnostics_path, f"{spec.case_id}-DIAGNOSTICS", "native probe must emit module.diagnostics.txt"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)

    if not all(path.exists() for path in (ir_path, obj_path, manifest_path, backend_path, runtime_metadata_bin_path, diagnostics_path)):
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    manifest = json.loads(read_text(manifest_path))
    backend_text = read_text(backend_path).strip()
    diagnostics_text = read_text(diagnostics_path)
    runtime_metadata_bin_size = runtime_metadata_bin_path.stat().st_size
    case["backend"] = backend_text
    case["diagnostics_is_empty"] = diagnostics_text.strip() == ""
    case["runtime_metadata_bin_size"] = runtime_metadata_bin_size

    semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
    typed = semantic_surface["objc_executable_metadata_typed_lowering_handoff"]
    graph = typed["source_graph"]
    debug_projection = semantic_surface["objc_executable_metadata_debug_projection"]
    binary_boundary = semantic_surface["objc_executable_metadata_runtime_ingest_binary_boundary"]

    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    llvm_used_line = next((line for line in ir_text.splitlines() if line.startswith("@llvm.used = appending global [")), "")
    case["boundary_line"] = boundary_line
    case["llvm_used_line"] = llvm_used_line

    class_instance_method_lists = len(CLASS_INSTANCE_METHOD_LIST_RE.findall(ir_text))
    class_class_method_lists = len(CLASS_CLASS_METHOD_LIST_RE.findall(ir_text))
    protocol_instance_method_lists = len(PROTOCOL_INSTANCE_METHOD_LIST_RE.findall(ir_text))
    category_instance_method_lists = len(CATEGORY_INSTANCE_METHOD_LIST_RE.findall(ir_text))
    category_class_method_lists = len(CATEGORY_CLASS_METHOD_LIST_RE.findall(ir_text))
    property_descriptors = len(PROPERTY_DESCRIPTOR_RE.findall(ir_text))
    ivar_descriptors = len(IVAR_DESCRIPTOR_RE.findall(ir_text))
    method_list_bundle_count = (
        class_instance_method_lists
        + class_class_method_lists
        + protocol_instance_method_lists
        + category_instance_method_lists
        + category_class_method_lists
    )
    method_entry_count = len(re.findall(r"_method_selector_\d{4}_\d{4} = private constant ", ir_text, re.MULTILINE))

    case.update(
        {
            "class_instance_method_lists": class_instance_method_lists,
            "class_class_method_lists": class_class_method_lists,
            "protocol_instance_method_lists": protocol_instance_method_lists,
            "category_instance_method_lists": category_instance_method_lists,
            "category_class_method_lists": category_class_method_lists,
            "method_list_bundle_count": method_list_bundle_count,
            "method_entry_count": method_entry_count,
            "property_descriptor_count": property_descriptors,
            "ivar_descriptor_count": ivar_descriptors,
            "typed_replay_key": typed.get("replay_key", ""),
        }
    )

    for condition, artifact, check_id, detail in (
        (backend_text == "llvm-direct", display_path(backend_path), f"{spec.case_id}-BACKEND-TEXT", f"expected llvm-direct backend, saw {backend_text!r}"),
        (diagnostics_text.strip() == "", display_path(diagnostics_path), f"{spec.case_id}-DIAGNOSTICS-EMPTY", "module.diagnostics.txt must be empty"),
        (runtime_metadata_bin_size > 0, display_path(runtime_metadata_bin_path), f"{spec.case_id}-BIN-SIZE", "module.runtime-metadata.bin must be non-empty"),
        (typed.get("ready_for_lowering") is True, display_path(manifest_path), f"{spec.case_id}-TYPED-READY", "typed metadata handoff must remain lowering-ready"),
        (debug_projection.get("ready") is True, display_path(manifest_path), f"{spec.case_id}-DEBUG-PROJECTION", "debug projection must remain ready"),
        (binary_boundary.get("ready") is True, display_path(manifest_path), f"{spec.case_id}-BINARY-BOUNDARY", "runtime ingest binary boundary must remain ready"),
        (len(graph.get("method_node_entries", [])) == spec.expected_graph_method_nodes, display_path(manifest_path), f"{spec.case_id}-GRAPH-METHODS", f"expected {spec.expected_graph_method_nodes} method nodes"),
        (len(graph.get("property_node_entries", [])) == spec.expected_graph_property_nodes, display_path(manifest_path), f"{spec.case_id}-GRAPH-PROPERTIES", f"expected {spec.expected_graph_property_nodes} property nodes"),
        (len(graph.get("ivar_node_entries", [])) == spec.expected_graph_ivar_nodes, display_path(manifest_path), f"{spec.case_id}-GRAPH-IVARS", f"expected {spec.expected_graph_ivar_nodes} ivar nodes"),
        (bool(boundary_line), display_path(ir_path), f"{spec.case_id}-BOUNDARY", "IR must publish the member-table emission summary line"),
        (NAMED_METADATA_LINE in ir_text, display_path(ir_path), f"{spec.case_id}-NAMED-METADATA", "IR must publish !objc3.objc_runtime_member_table_emission"),
        (PROPERTY_PLACEHOLDER_RE.search(ir_text) is None, display_path(ir_path), f"{spec.case_id}-NO-PROPERTY-PLACEHOLDERS", "property family must not emit placeholder globals on the happy path"),
        (IVAR_PLACEHOLDER_RE.search(ir_text) is None, display_path(ir_path), f"{spec.case_id}-NO-IVAR-PLACEHOLDERS", "ivar family must not emit placeholder globals on the happy path"),
        (bool(llvm_used_line), display_path(ir_path), f"{spec.case_id}-LLVM-USED", "IR must retain emitted metadata through @llvm.used"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    for check_id, token in (
        (f"{spec.case_id}-BOUNDARY-METHOD-PAYLOAD", "method_list_payload_model=owner-scoped-method-table-globals-with-inline-entry-records"),
        (f"{spec.case_id}-BOUNDARY-METHOD-GROUPING", "method_list_grouping_model=declaration-owner-plus-class-kind-lexicographic"),
        (f"{spec.case_id}-BOUNDARY-PROPERTY-PAYLOAD", "property_payload_model=property-descriptor-records-with-accessor-and-binding-strings"),
        (f"{spec.case_id}-BOUNDARY-IVAR-PAYLOAD", "ivar_payload_model=ivar-descriptor-records-with-property-binding-strings"),
        (f"{spec.case_id}-BOUNDARY-METHOD-BUNDLES", f"method_list_bundle_count={spec.expected_method_list_bundle_count}"),
        (f"{spec.case_id}-BOUNDARY-METHOD-ENTRIES", f"method_entry_count={spec.expected_method_entry_count}"),
        (f"{spec.case_id}-BOUNDARY-PROPERTY-COUNT", f"property_descriptor_count={spec.expected_property_descriptor_count}"),
        (f"{spec.case_id}-BOUNDARY-IVAR-COUNT", f"ivar_descriptor_count={spec.expected_ivar_descriptor_count}"),
    ):
        checks_total += 1
        checks_passed += require(token in boundary_line, display_path(ir_path), check_id, f"boundary line must contain {token}", failures)

    for value, expected, check_id, detail in (
        (class_instance_method_lists, spec.expected_class_instance_method_lists, f"{spec.case_id}-CLASS-INSTANCE-LISTS", "unexpected class instance-method list count"),
        (class_class_method_lists, spec.expected_class_class_method_lists, f"{spec.case_id}-CLASS-CLASS-LISTS", "unexpected class class-method list count"),
        (protocol_instance_method_lists, spec.expected_protocol_instance_method_lists, f"{spec.case_id}-PROTOCOL-LISTS", "unexpected protocol method-list count"),
        (category_instance_method_lists, spec.expected_category_instance_method_lists, f"{spec.case_id}-CATEGORY-INSTANCE-LISTS", "unexpected category instance-method list count"),
        (category_class_method_lists, spec.expected_category_class_method_lists, f"{spec.case_id}-CATEGORY-CLASS-LISTS", "unexpected category class-method list count"),
        (method_list_bundle_count, spec.expected_method_list_bundle_count, f"{spec.case_id}-METHOD-BUNDLE-COUNT", "unexpected total method-list bundle count"),
        (method_entry_count, spec.expected_method_entry_count, f"{spec.case_id}-METHOD-ENTRY-COUNT", "unexpected total method-entry count"),
        (property_descriptors, spec.expected_property_descriptor_count, f"{spec.case_id}-PROPERTY-DESCRIPTOR-COUNT", "unexpected property descriptor count"),
        (ivar_descriptors, spec.expected_ivar_descriptor_count, f"{spec.case_id}-IVAR-DESCRIPTOR-COUNT", "unexpected ivar descriptor count"),
    ):
        checks_total += 1
        checks_passed += require(value == expected, display_path(ir_path), check_id, f"{detail}: expected {expected}, saw {value}", failures)

    for token in spec.symbol_tokens:
        checks_total += 1
        checks_passed += require(token in ir_text, display_path(ir_path), f"{spec.case_id}-IR-SYMBOL-{token}", f"IR must mention {token}", failures)
        checks_total += 1
        checks_passed += require(token in llvm_used_line or token in ir_text, display_path(ir_path), f"{spec.case_id}-USED-{token}", f"retained metadata must keep {token} reachable", failures)

    llvm_readobj = resolve_command(args.llvm_readobj)
    llvm_objdump = resolve_command(args.llvm_objdump)
    checks_total += 1
    checks_passed += require(llvm_readobj is not None, args.llvm_readobj, f"{spec.case_id}-LLVM-READOBJ", f"unable to resolve {args.llvm_readobj}", failures)
    checks_total += 1
    checks_passed += require(llvm_objdump is not None, args.llvm_objdump, f"{spec.case_id}-LLVM-OBJDUMP", f"unable to resolve {args.llvm_objdump}", failures)

    readobj_text = ""
    objdump_text = ""
    if llvm_readobj is not None:
        readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)], ROOT)
        case["llvm_readobj_exit_code"] = readobj_result.returncode
        readobj_text = readobj_result.stdout
        checks_total += 1
        checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), f"{spec.case_id}-READOBJ-STATUS", f"llvm-readobj failed: {readobj_result.stderr.strip()}", failures)
    if llvm_objdump is not None:
        objdump_result = run_command([llvm_objdump, "--syms", str(obj_path)], ROOT)
        case["llvm_objdump_exit_code"] = objdump_result.returncode
        objdump_text = objdump_result.stdout
        checks_total += 1
        checks_passed += require(objdump_result.returncode == 0, display_path(obj_path), f"{spec.case_id}-OBJDUMP-STATUS", f"llvm-objdump failed: {objdump_result.stderr.strip()}", failures)

    for section_name, min_raw_size, min_relocations in spec.section_expectations:
        raw_size, relocations = parse_readobj_section(readobj_text, section_name)
        case[f"{section_name}_raw_size"] = raw_size
        case[f"{section_name}_relocations"] = relocations
        checks_total += 1
        checks_passed += require(section_name in readobj_text, display_path(obj_path), f"{spec.case_id}-SECTION-{section_name}", f"object must expose {section_name}", failures)
        checks_total += 1
        checks_passed += require(raw_size is not None and raw_size >= min_raw_size, display_path(obj_path), f"{spec.case_id}-SECTION-SIZE-{section_name}", f"{section_name} must have at least {min_raw_size} bytes, saw {raw_size}", failures)
        checks_total += 1
        checks_passed += require(relocations is not None and relocations >= min_relocations, display_path(obj_path), f"{spec.case_id}-SECTION-RELOCS-{section_name}", f"{section_name} must have at least {min_relocations} relocations, saw {relocations}", failures)

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
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ir_header, IR_HEADER_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        for spec in make_probe_specs(args):
            probe_total, probe_passed, case = run_native_probe(args, spec, failures)
            checks_total += probe_total
            checks_passed += probe_passed
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
