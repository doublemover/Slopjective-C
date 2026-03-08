#!/usr/bin/env python3
"""Fail-closed contract checker for M253-C002 class/metaclass data emission."""

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
MODE = "m253-c002-class-and-metaclass-data-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-class-metaclass-data-emission/m253-c002-v1"
BOUNDARY_COMMENT_PREFIX = "; runtime_metadata_class_metaclass_emission = contract=objc3c-runtime-class-metaclass-data-emission/m253-c002-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_class_and_metaclass_data_emission_core_feature_implementation_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_c002_class_and_metaclass_data_emission_core_feature_implementation_packet.md"
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
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_executable_metadata_graph_class_metaclass.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "class-metaclass-data-emission"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-C002/class_and_metaclass_data_emission_summary.json")
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


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-DOC-EXP-01", "# M253 Class And Metaclass Data Emission Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M253-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-C002-DOC-EXP-03", "`class-source-record-descriptor-bundles-with-inline-metaclass-records`"),
    SnippetCheck("M253-C002-DOC-EXP-04", "`@__objc3_meta_class_owner_identity_0000`"),
    SnippetCheck("M253-C002-DOC-EXP-05", "`tmp/reports/m253/M253-C002/class_and_metaclass_data_emission_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-DOC-PKT-01", "# M253-C002 Class And Metaclass Data Emission Core Feature Implementation Packet"),
    SnippetCheck("M253-C002-DOC-PKT-02", "Packet: `M253-C002`"),
    SnippetCheck("M253-C002-DOC-PKT-03", "- `M253-C001`"),
    SnippetCheck("M253-C002-DOC-PKT-04", "- `M252-C002`"),
    SnippetCheck("M253-C002-DOC-PKT-05", "- `M253-B002`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-ARCH-01", "M253 lane-C C002 class and metaclass data emission anchors explicit"),
    SnippetCheck("M253-C002-ARCH-02", "runtime_metadata_class_metaclass_bundles_lexicographic"),
    SnippetCheck("M253-C002-ARCH-03", "llvm-direct class bundle payloads"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-NDOC-01", "## Class and metaclass data emission (M253-C002)"),
    SnippetCheck("M253-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C002-NDOC-03", "`@__objc3_meta_class_owner_identity_0000`"),
    SnippetCheck("M253-C002-NDOC-04", "`tmp/reports/m253/M253-C002/class_and_metaclass_data_emission_summary.json`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-SPC-01", "## M253 class and metaclass data emission (C002)"),
    SnippetCheck("M253-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C002-SPC-03", "class-source-record-descriptor-bundles-with-inline-metaclass-records"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-META-01", "## M253 class and metaclass data emission metadata anchors (C002)"),
    SnippetCheck("M253-C002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C002-META-03", "`__objc3_meta_class_owner_identity_0000`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-LHDR-01", "kObjc3RuntimeClassMetaclassEmissionContractId"),
    SnippetCheck("M253-C002-LHDR-02", "kObjc3RuntimeClassMetaclassEmissionPayloadModel"),
    SnippetCheck("M253-C002-LHDR-03", "kObjc3RuntimeClassMetaclassEmissionSuperLinkModel"),
    SnippetCheck("M253-C002-LHDR-04", "kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-LCPP-01", "Objc3RuntimeMetadataClassMetaclassEmissionSummary()"),
    SnippetCheck("M253-C002-LCPP-02", "M253-C002 class/metaclass data emission anchor"),
    SnippetCheck("M253-C002-LCPP-03", "non_goals=no-standalone-metaclass-section-or-selector-string-pool"),
)

IR_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-IHDR-01", "struct Objc3IRRuntimeMetadataClassMetaclassBundle {"),
    SnippetCheck("M253-C002-IHDR-02", "std::string owner_identity;"),
    SnippetCheck("M253-C002-IHDR-03", "std::string super_bundle_owner_identity;"),
    SnippetCheck("M253-C002-IHDR-04", "runtime_metadata_class_metaclass_bundles_lexicographic;"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-IR-01", 'out << "; runtime_metadata_class_metaclass_emission = "'),
    SnippetCheck("M253-C002-IR-02", "!objc3.objc_runtime_class_metaclass_emission = !{!56}"),
    SnippetCheck("M253-C002-IR-03", "owner_identity_symbol ="),
    SnippetCheck("M253-C002-IR-04", "descriptor_symbols_by_owner_identity"),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-ART-01", "metadata_only_ir_emission_mode"),
    SnippetCheck("M253-C002-ART-02", "runtime_metadata_class_metaclass_bundles_lexicographic"),
    SnippetCheck("M253-C002-ART-03", "bundle.super_bundle_owner_identity"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C002-PROC-01", "M253-C002 class/metaclass data emission anchor"),
    SnippetCheck("M253-C002-PROC-02", "inline class/metaclass/name/method-ref payloads"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-C002-PKG-01",
        '"check:objc3c:m253-c002-class-and-metaclass-data-emission-core-feature-implementation": "python scripts/check_m253_c002_class_and_metaclass_data_emission_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M253-C002-PKG-02",
        '"test:tooling:m253-c002-class-and-metaclass-data-emission-core-feature-implementation": "python -m pytest tests/tooling/test_check_m253_c002_class_and_metaclass_data_emission_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M253-C002-PKG-03",
        '"check:objc3c:m253-c002-lane-c-readiness": "npm run check:objc3c:m253-c001-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-c002-class-and-metaclass-data-emission-core-feature-implementation && npm run test:tooling:m253-c002-class-and-metaclass-data-emission-core-feature-implementation"',
    ),
)


CLASS_SECTION_PLACEHOLDER_RE = re.compile(
    r'@__objc3_meta_[^\n]+zeroinitializer, section "objc3\.runtime\.class_descriptors"'
)
CLASS_BUNDLE_RE = re.compile(r"^@__objc3_meta_class_\d{4} = private global ", re.MULTILINE)
OWNER_ID_RE = re.compile(r"^@__objc3_meta_class_owner_identity_\d{4} = private constant ", re.MULTILINE)
INSTANCE_METHOD_REF_RE = re.compile(r"^@__objc3_meta_class_instance_method_list_ref_\d{4} = private global ", re.MULTILINE)
METACLASS_METHOD_REF_RE = re.compile(r"^@__objc3_meta_metaclass_method_list_ref_\d{4} = private global ", re.MULTILINE)


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
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
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
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def resolve_command(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    candidate = name + ".exe"
    return shutil.which(candidate)


def parse_readobj_class_section(text: str) -> tuple[int | None, int | None]:
    marker = "Name: objc3.runtime.class_descriptors"
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


def run_native_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / "native-class-metaclass"
    command = [
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
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
        "case_id": "M253-C002-CASE-NATIVE",
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
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M253-C002-NATIVE-EXE", "objc3c-native.exe is missing", failures)
    checks_total += 1
    checks_passed += require(args.fixture.exists(), display_path(args.fixture), "M253-C002-FIXTURE", "class/metaclass fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(ir_path), "M253-C002-NATIVE-STATUS", "native probe must exit 0", failures)
    for path, check_id, detail in (
        (ir_path, "M253-C002-IR", "native probe must emit module.ll"),
        (obj_path, "M253-C002-OBJ", "native probe must emit module.obj"),
        (manifest_path, "M253-C002-MANIFEST", "native probe must emit module.manifest.json"),
        (backend_path, "M253-C002-BACKEND", "native probe must emit module.object-backend.txt"),
        (runtime_metadata_bin_path, "M253-C002-BIN", "native probe must emit module.runtime-metadata.bin"),
        (diagnostics_path, "M253-C002-DIAGNOSTICS", "native probe must emit module.diagnostics.txt"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)

    if not all(path.exists() for path in (ir_path, obj_path, manifest_path, backend_path, runtime_metadata_bin_path, diagnostics_path)):
        return checks_total, checks_passed, case

    diagnostics_text = read_text(diagnostics_path)
    backend_text = read_text(backend_path).strip()
    manifest = json.loads(read_text(manifest_path))
    ir_text = read_text(ir_path)
    runtime_metadata_bin_size = runtime_metadata_bin_path.stat().st_size

    semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
    typed = semantic_surface["objc_executable_metadata_typed_lowering_handoff"]
    graph = typed["source_graph"]
    debug_projection = semantic_surface["objc_executable_metadata_debug_projection"]
    binary_boundary = semantic_surface["objc_executable_metadata_runtime_ingest_binary_boundary"]
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]

    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    class_bundle_count = len(CLASS_BUNDLE_RE.findall(ir_text))
    owner_identity_count = len(OWNER_ID_RE.findall(ir_text))
    instance_method_ref_count = len(INSTANCE_METHOD_REF_RE.findall(ir_text))
    metaclass_method_ref_count = len(METACLASS_METHOD_REF_RE.findall(ir_text))
    class_placeholders = CLASS_SECTION_PLACEHOLDER_RE.findall(ir_text)

    case.update(
        {
            "backend": backend_text,
            "diagnostics_is_empty": diagnostics_text.strip() == "",
            "runtime_metadata_bin_size": runtime_metadata_bin_size,
            "boundary_line": boundary_line,
            "class_bundle_count": class_bundle_count,
            "owner_identity_count": owner_identity_count,
            "instance_method_ref_count": instance_method_ref_count,
            "metaclass_method_ref_count": metaclass_method_ref_count,
            "typed_replay_key": typed.get("replay_key", ""),
        }
    )

    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M253-C002-BACKEND-TEXT", f"expected llvm-direct backend, saw {backend_text!r}", failures)
    checks_total += 1
    checks_passed += require(diagnostics_text.strip() == "", display_path(diagnostics_path), "M253-C002-DIAGNOSTICS-EMPTY", "module.diagnostics.txt must be empty", failures)
    checks_total += 1
    checks_passed += require(runtime_metadata_bin_size > 0, display_path(runtime_metadata_bin_path), "M253-C002-BIN-SIZE", "module.runtime-metadata.bin must be non-empty", failures)

    checks_total += 1
    checks_passed += require(typed.get("ready_for_lowering") is True, display_path(manifest_path), "M253-C002-TYPED-READY", "typed metadata handoff must remain lowering-ready", failures)
    checks_total += 1
    checks_passed += require(len(graph.get("interface_node_entries", [])) == 2, display_path(manifest_path), "M253-C002-GRAPH-INTERFACES", "fixture must expose exactly 2 interface nodes", failures)
    checks_total += 1
    checks_passed += require(len(graph.get("implementation_node_entries", [])) == 2, display_path(manifest_path), "M253-C002-GRAPH-IMPLEMENTATIONS", "fixture must expose exactly 2 implementation nodes", failures)
    checks_total += 1
    checks_passed += require(len(graph.get("class_node_entries", [])) == 2, display_path(manifest_path), "M253-C002-GRAPH-CLASSES", "fixture must expose exactly 2 class nodes", failures)
    checks_total += 1
    checks_passed += require(len(graph.get("metaclass_node_entries", [])) == 2, display_path(manifest_path), "M253-C002-GRAPH-METACLASSES", "fixture must expose exactly 2 metaclass nodes", failures)
    checks_total += 1
    checks_passed += require(sema.get("runtime_export_class_record_count") == 4, display_path(manifest_path), "M253-C002-EXPORT-CLASS-COUNT", "runtime export class record count must stay 4", failures)
    checks_total += 1
    checks_passed += require(sema.get("runtime_metadata_section_scaffold_class_descriptor_count") == 4, display_path(manifest_path), "M253-C002-SCAFFOLD-CLASS-COUNT", "class descriptor scaffold count must stay 4", failures)
    checks_total += 1
    checks_passed += require(debug_projection.get("ready") is True, display_path(manifest_path), "M253-C002-DEBUG-PROJECTION", "debug projection must remain ready", failures)
    checks_total += 1
    checks_passed += require(binary_boundary.get("ready") is True, display_path(manifest_path), "M253-C002-BINARY-BOUNDARY", "runtime ingest binary boundary must remain ready", failures)

    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), "M253-C002-BOUNDARY", "IR must publish the class/metaclass emission summary line", failures)
    for check_id, token in (
        ("M253-C002-BOUNDARY-PAYLOAD", "payload_model=class-source-record-descriptor-bundles-with-inline-metaclass-records"),
        ("M253-C002-BOUNDARY-NAME", "name_model=shared-class-name-cstring-per-bundle"),
        ("M253-C002-BOUNDARY-SUPER", "super_link_model=nullable-super-source-record-bundle-pointer"),
        ("M253-C002-BOUNDARY-METHOD-REF", "method_list_reference_model=count-plus-owner-identity-pointer-method-list-ref"),
        ("M253-C002-BOUNDARY-BUNDLE-COUNT", "bundle_count=4"),
        ("M253-C002-BOUNDARY-INSTANCE-METHOD-REFS", "instance_method_refs=4"),
        ("M253-C002-BOUNDARY-CLASS-METHOD-REFS", "class_method_refs=4"),
    ):
        checks_total += 1
        checks_passed += require(token in boundary_line, display_path(ir_path), check_id, f"boundary line must contain {token}", failures)

    checks_total += 1
    checks_passed += require("!objc3.objc_runtime_class_metaclass_emission = !{!56}" in ir_text, display_path(ir_path), "M253-C002-IR-NAMED-METADATA", "IR must publish !objc3.objc_runtime_class_metaclass_emission", failures)
    checks_total += 1
    checks_passed += require(class_bundle_count == 4, display_path(ir_path), "M253-C002-IR-CLASS-BUNDLES", f"expected 4 class bundles, saw {class_bundle_count}", failures)
    checks_total += 1
    checks_passed += require(owner_identity_count == 4, display_path(ir_path), "M253-C002-IR-OWNER-IDS", f"expected 4 owner-identity strings, saw {owner_identity_count}", failures)
    checks_total += 1
    checks_passed += require(instance_method_ref_count == 4, display_path(ir_path), "M253-C002-IR-INSTANCE-METHOD-REFS", f"expected 4 instance-method refs, saw {instance_method_ref_count}", failures)
    checks_total += 1
    checks_passed += require(metaclass_method_ref_count == 4, display_path(ir_path), "M253-C002-IR-METACLASS-METHOD-REFS", f"expected 4 metaclass-method refs, saw {metaclass_method_ref_count}", failures)
    checks_total += 1
    checks_passed += require('@__objc3_sec_class_descriptors = internal global { i64, [4 x ptr] }' in ir_text, display_path(ir_path), "M253-C002-IR-CLASS-AGGREGATE", "IR must publish a 4-entry class aggregate", failures)
    checks_total += 1
    checks_passed += require(not class_placeholders, display_path(ir_path), "M253-C002-IR-NO-CLASS-PLACEHOLDERS", "class family must not emit placeholder [1 x i8] zeroinitializer records", failures)

    llvm_readobj = resolve_command(args.llvm_readobj)
    llvm_objdump = resolve_command(args.llvm_objdump)
    checks_total += 1
    checks_passed += require(llvm_readobj is not None, args.llvm_readobj, "M253-C002-LLVM-READOBJ", "llvm-readobj must be available", failures)
    checks_total += 1
    checks_passed += require(llvm_objdump is not None, args.llvm_objdump, "M253-C002-LLVM-OBJDUMP", "llvm-objdump must be available", failures)

    if llvm_readobj is not None:
        readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)], ROOT)
        case["llvm_readobj_exit_code"] = readobj_result.returncode
        case["llvm_readobj_stdout"] = readobj_result.stdout
        checks_total += 1
        checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), "M253-C002-READOBJ-STATUS", "llvm-readobj --sections must exit 0", failures)
        raw_size, relocations = parse_readobj_class_section(readobj_result.stdout)
        case["class_section_raw_size"] = raw_size
        case["class_section_relocations"] = relocations
        checks_total += 1
        checks_passed += require("Name: objc3.runtime.class_descriptors" in readobj_result.stdout, display_path(obj_path), "M253-C002-READOBJ-CLASS-SECTION", "object must expose objc3.runtime.class_descriptors", failures)
        checks_total += 1
        checks_passed += require(raw_size is not None and raw_size > 64, display_path(obj_path), "M253-C002-READOBJ-CLASS-SIZE", f"class descriptor section must have nontrivial bytes, saw {raw_size}", failures)
        checks_total += 1
        checks_passed += require(relocations is not None and relocations >= 8, display_path(obj_path), "M253-C002-READOBJ-CLASS-RELOCS", f"class descriptor section must have relocations, saw {relocations}", failures)

    if llvm_objdump is not None:
        objdump_result = run_command([llvm_objdump, "--syms", str(obj_path)], ROOT)
        case["llvm_objdump_exit_code"] = objdump_result.returncode
        case["llvm_objdump_stdout"] = objdump_result.stdout
        checks_total += 1
        checks_passed += require(objdump_result.returncode == 0, display_path(obj_path), "M253-C002-OBJDUMP-STATUS", "llvm-objdump --syms must exit 0", failures)
        checks_total += 1
        checks_passed += require("objc3.runtime.class_descriptors" in objdump_result.stdout, display_path(obj_path), "M253-C002-OBJDUMP-SECTION", "symbol table must mention objc3.runtime.class_descriptors", failures)
        checks_total += 1
        checks_passed += require("__objc3_sec_class_descriptors" in objdump_result.stdout, display_path(obj_path), "M253-C002-OBJDUMP-AGGREGATE", "symbol table must mention __objc3_sec_class_descriptors", failures)

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
        probe_total, probe_passed, case = run_native_probe(args, failures)
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
