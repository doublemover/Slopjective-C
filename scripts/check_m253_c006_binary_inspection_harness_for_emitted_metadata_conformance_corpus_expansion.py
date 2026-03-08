#!/usr/bin/env python3
"""Fail-closed contract checker for M253-C006 emitted metadata binary inspection."""

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
MODE = "m253-c006-binary-inspection-harness-for-emitted-metadata-conformance-corpus-expansion-v1"
CONTRACT_ID = "objc3c-runtime-binary-inspection-harness/m253-c006-v1"
BOUNDARY_COMMENT_PREFIX = (
    "; runtime_metadata_binary_inspection_harness = contract="
    "objc3c-runtime-binary-inspection-harness/m253-c006-v1"
)
NAMED_METADATA_LINE = '!objc3.objc_runtime_binary_inspection_harness = !{!60}'
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion_c006_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_ZERO_DESCRIPTOR_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_MESSAGE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive" / "message_send_runtime_shim.objc3"
DEFAULT_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_missing_interface_property.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "binary-inspection-harness"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json")
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
class SectionExpectation:
    name: str
    min_raw_size: int
    min_relocations: int
    max_raw_size: int | None = None
    max_relocations: int | None = None


@dataclass(frozen=True)
class PositiveProbeSpec:
    case_id: str
    fixture: Path
    probe_subdir: str
    expected_sections: tuple[SectionExpectation, ...]
    expected_absent_sections: tuple[str, ...]
    expected_present_symbols: tuple[str, ...]
    expected_absent_symbols: tuple[str, ...]
    expected_zero_offset_symbols: tuple[str, ...]
    expected_nonzero_offset_symbols: tuple[str, ...]


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-DOC-EXP-01", "# M253 Binary Inspection Harness For Emitted Metadata Conformance Corpus Expansion Expectations (C006)"),
    SnippetCheck("M253-C006-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-C006-DOC-EXP-03", "`llvm-readobj --sections module.obj`"),
    SnippetCheck("M253-C006-DOC-EXP-04", "`tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3`"),
    SnippetCheck("M253-C006-DOC-EXP-05", "`tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-DOC-PKT-01", "# M253-C006 Binary Inspection Harness For Emitted Metadata Conformance Corpus Expansion Packet"),
    SnippetCheck("M253-C006-DOC-PKT-02", "Packet: `M253-C006`"),
    SnippetCheck("M253-C006-DOC-PKT-03", "- `M253-C005`"),
    SnippetCheck("M253-C006-DOC-PKT-04", "`tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`"),
    SnippetCheck("M253-C006-DOC-PKT-05", "`llvm-objdump --syms module.obj`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-ARCH-01", "M253 lane-C C006 binary inspection harness expansion anchors explicit"),
    SnippetCheck("M253-C006-ARCH-02", "llvm-readobj/llvm-objdump corpus structurally proves every currently emitted"),
    SnippetCheck("M253-C006-ARCH-03", "fail-closed negative compile case"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-NDOC-01", "## Binary inspection harness for emitted metadata (M253-C006)"),
    SnippetCheck("M253-C006-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C006-NDOC-03", "`!objc3.objc_runtime_binary_inspection_harness`"),
    SnippetCheck("M253-C006-NDOC-04", "`tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-SPC-01", "## M253 binary inspection harness for emitted metadata (C006)"),
    SnippetCheck("M253-C006-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C006-SPC-03", "positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-META-01", "## M253 binary inspection harness metadata anchors (C006)"),
    SnippetCheck("M253-C006-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C006-META-03", "`__objc3_sec_selector_pool`"),
    SnippetCheck("M253-C006-META-04", "`__objc3_sec_string_pool`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-LHDR-01", "kObjc3RuntimeBinaryInspectionHarnessContractId"),
    SnippetCheck("M253-C006-LHDR-02", "kObjc3RuntimeBinaryInspectionPositiveCorpusModel"),
    SnippetCheck("M253-C006-LHDR-03", "kObjc3RuntimeBinaryInspectionSectionCommand"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-LCPP-01", "Objc3RuntimeMetadataBinaryInspectionHarnessSummary()"),
    SnippetCheck("M253-C006-LCPP-02", "M253-C006 binary inspection harness expansion anchor"),
    SnippetCheck("M253-C006-LCPP-03", "symbol_inventory_command="),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-IR-01", 'out << "!objc3.objc_runtime_binary_inspection_harness = !{!60}\\n";'),
    SnippetCheck("M253-C006-IR-02", 'out << "; runtime_metadata_binary_inspection_harness = "'),
    SnippetCheck("M253-C006-IR-03", "kObjc3RuntimeBinaryInspectionSectionCommand"),
    SnippetCheck("M253-C006-IR-04", "kObjc3RuntimeBinaryInspectionSymbolCommand"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C006-PROC-01", "M253-C006 binary inspection harness expansion anchor"),
    SnippetCheck("M253-C006-PROC-02", "produce no synthesized object-inspection artifacts"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-C006-PKG-01",
        '"check:objc3c:m253-c006-binary-inspection-harness-for-emitted-metadata-conformance-corpus-expansion": "python scripts/check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion.py"',
    ),
    SnippetCheck(
        "M253-C006-PKG-02",
        '"test:tooling:m253-c006-binary-inspection-harness-for-emitted-metadata-conformance-corpus-expansion": "python -m pytest tests/tooling/test_check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion.py -q"',
    ),
    SnippetCheck(
        "M253-C006-PKG-03",
        '"check:objc3c:m253-c006-lane-c-readiness": "npm run check:objc3c:m253-c005-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-c006-binary-inspection-harness-for-emitted-metadata-conformance-corpus-expansion && npm run test:tooling:m253-c006-binary-inspection-harness-for-emitted-metadata-conformance-corpus-expansion"',
    ),
)

ARTIFACT_SNIPPETS: dict[str, tuple[SnippetCheck, ...]] = {
    "expectations_doc": EXPECTATIONS_SNIPPETS,
    "packet_doc": PACKET_SNIPPETS,
    "architecture_doc": ARCHITECTURE_SNIPPETS,
    "native_doc": NATIVE_DOC_SNIPPETS,
    "lowering_spec": LOWERING_SPEC_SNIPPETS,
    "metadata_spec": METADATA_SPEC_SNIPPETS,
    "lowering_header": LOWERING_HEADER_SNIPPETS,
    "lowering_cpp": LOWERING_CPP_SNIPPETS,
    "ir_emitter": IR_EMITTER_SNIPPETS,
    "process_cpp": PROCESS_SNIPPETS,
    "package_json": PACKAGE_SNIPPETS,
}

COMMON_BOUNDARY_TOKENS = (
    "positive_case_count=4",
    "negative_case_count=1",
    "section_inventory_command=llvm-readobj --sections module.obj",
    "symbol_inventory_command=llvm-objdump --syms module.obj",
)

SYMBOL_RE = re.compile(r"0x(?P<value>[0-9A-Fa-f]+)\s+(?P<name>__objc3_[A-Za-z0-9_]+)$")


def exact_section(name: str, raw_size: int, relocations: int) -> SectionExpectation:
    return SectionExpectation(
        name=name,
        min_raw_size=raw_size,
        max_raw_size=raw_size,
        min_relocations=relocations,
        max_relocations=relocations,
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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--zero-descriptor-fixture", type=Path, default=DEFAULT_ZERO_DESCRIPTOR_FIXTURE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--message-fixture", type=Path, default=DEFAULT_MESSAGE_FIXTURE)
    parser.add_argument("--negative-fixture", type=Path, default=DEFAULT_NEGATIVE_FIXTURE)
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


def diagnostics_count(manifest_payload: object) -> int:
    diagnostics = manifest_value(manifest_payload, "diagnostics", [])
    if isinstance(diagnostics, list):
        return len(diagnostics)
    return 0


def resolve_command(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


def extract_metadata_sections(readobj_stdout: str) -> dict[str, dict[str, int | None]]:
    sections: dict[str, dict[str, int | None]] = {}
    for block in readobj_stdout.split("Section {"):
        name_match = re.search(r"Name: ([^\s(]+)", block)
        if name_match is None:
            continue
        name = name_match.group(1)
        if not name.startswith("objc3.runtime."):
            continue
        raw_match = re.search(r"RawDataSize:\s*(\d+)", block)
        reloc_match = re.search(r"RelocationCount:\s*(\d+)", block)
        sections[name] = {
            "raw_data_size": int(raw_match.group(1)) if raw_match else None,
            "relocation_count": int(reloc_match.group(1)) if reloc_match else None,
        }
    return sections


def extract_symbol_offsets(objdump_stdout: str) -> dict[str, int]:
    symbols: dict[str, int] = {}
    for line in objdump_stdout.splitlines():
        match = SYMBOL_RE.search(line.strip())
        if match is None:
            continue
        symbols[match.group("name")] = int(match.group("value"), 16)
    return symbols


def make_positive_probe_specs(args: argparse.Namespace) -> tuple[PositiveProbeSpec, ...]:
    return (
        PositiveProbeSpec(
            case_id="M253-C006-CASE-ZERO-DESCRIPTOR",
            fixture=args.zero_descriptor_fixture,
            probe_subdir="zero-descriptor",
            expected_sections=(
                exact_section("objc3.runtime.image_info", 8, 0),
                exact_section("objc3.runtime.class_descriptors", 8, 0),
                exact_section("objc3.runtime.protocol_descriptors", 8, 0),
                exact_section("objc3.runtime.category_descriptors", 8, 0),
                exact_section("objc3.runtime.property_descriptors", 8, 0),
                exact_section("objc3.runtime.ivar_descriptors", 8, 0),
            ),
            expected_absent_sections=(
                "objc3.runtime.selector_pool",
                "objc3.runtime.string_pool",
            ),
            expected_present_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
            ),
            expected_absent_symbols=(
                "__objc3_sec_selector_pool",
                "__objc3_sec_string_pool",
            ),
            expected_zero_offset_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
            ),
            expected_nonzero_offset_symbols=(),
        ),
        PositiveProbeSpec(
            case_id="M253-C006-CASE-CLASS-PROTOCOL-PROPERTY-IVAR",
            fixture=args.class_fixture,
            probe_subdir="class-protocol-property-ivar",
            expected_sections=(
                exact_section("objc3.runtime.image_info", 8, 0),
                SectionExpectation("objc3.runtime.class_descriptors", 800, 30),
                SectionExpectation("objc3.runtime.protocol_descriptors", 300, 10),
                exact_section("objc3.runtime.category_descriptors", 8, 0),
                SectionExpectation("objc3.runtime.property_descriptors", 500, 20),
                SectionExpectation("objc3.runtime.ivar_descriptors", 200, 5),
                SectionExpectation("objc3.runtime.selector_pool", 64, 4),
                SectionExpectation("objc3.runtime.string_pool", 512, 16),
            ),
            expected_absent_sections=(),
            expected_present_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
                "__objc3_sec_selector_pool",
                "__objc3_sec_string_pool",
            ),
            expected_absent_symbols=(),
            expected_zero_offset_symbols=(
                "__objc3_image_info",
                "__objc3_sec_category_descriptors",
            ),
            expected_nonzero_offset_symbols=(
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
                "__objc3_sec_selector_pool",
                "__objc3_sec_string_pool",
            ),
        ),
        PositiveProbeSpec(
            case_id="M253-C006-CASE-CATEGORY-PROTOCOL-PROPERTY",
            fixture=args.category_fixture,
            probe_subdir="category-protocol-property",
            expected_sections=(
                exact_section("objc3.runtime.image_info", 8, 0),
                exact_section("objc3.runtime.class_descriptors", 8, 0),
                SectionExpectation("objc3.runtime.protocol_descriptors", 300, 10),
                SectionExpectation("objc3.runtime.category_descriptors", 700, 25),
                SectionExpectation("objc3.runtime.property_descriptors", 500, 18),
                SectionExpectation("objc3.runtime.ivar_descriptors", 200, 5),
                SectionExpectation("objc3.runtime.selector_pool", 32, 2),
                SectionExpectation("objc3.runtime.string_pool", 512, 16),
            ),
            expected_absent_sections=(),
            expected_present_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
                "__objc3_sec_selector_pool",
                "__objc3_sec_string_pool",
            ),
            expected_absent_symbols=(),
            expected_zero_offset_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
            ),
            expected_nonzero_offset_symbols=(
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
                "__objc3_sec_selector_pool",
                "__objc3_sec_string_pool",
            ),
        ),
        PositiveProbeSpec(
            case_id="M253-C006-CASE-MESSAGE-SEND",
            fixture=args.message_fixture,
            probe_subdir="message-send",
            expected_sections=(
                exact_section("objc3.runtime.image_info", 8, 0),
                exact_section("objc3.runtime.class_descriptors", 8, 0),
                exact_section("objc3.runtime.protocol_descriptors", 8, 0),
                exact_section("objc3.runtime.category_descriptors", 8, 0),
                exact_section("objc3.runtime.property_descriptors", 8, 0),
                exact_section("objc3.runtime.ivar_descriptors", 8, 0),
                SectionExpectation("objc3.runtime.selector_pool", 24, 1),
                exact_section("objc3.runtime.string_pool", 8, 0),
            ),
            expected_absent_sections=(),
            expected_present_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
                "__objc3_sec_selector_pool",
                "__objc3_sec_string_pool",
            ),
            expected_absent_symbols=(),
            expected_zero_offset_symbols=(
                "__objc3_image_info",
                "__objc3_sec_class_descriptors",
                "__objc3_sec_protocol_descriptors",
                "__objc3_sec_category_descriptors",
                "__objc3_sec_property_descriptors",
                "__objc3_sec_ivar_descriptors",
                "__objc3_sec_string_pool",
            ),
            expected_nonzero_offset_symbols=(
                "__objc3_sec_selector_pool",
            ),
        ),
    )


MISSING = object()


def run_positive_probe(
    spec: PositiveProbeSpec,
    args: argparse.Namespace,
    llvm_readobj: str,
    llvm_objdump: str,
    failures: list[Finding],
) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root.resolve() / spec.probe_subdir
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_cmd = [
        str(args.native_exe.resolve()),
        str(spec.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_cmd, ROOT)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
    backend = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
    ir_text = read_text(ir_path) if ir_path.exists() else ""
    diagnostics_seen = diagnostics_count(manifest_payload)
    boundary_line = next(
        (line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)),
        "",
    )

    readobj_stdout = ""
    objdump_stdout = ""
    if obj_path.exists():
        readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)], ROOT)
        checks_total += 1
        checks_passed += require(
            readobj_result.returncode == 0,
            display_path(obj_path),
            f"{spec.case_id}-READOBJ-STATUS",
            f"llvm-readobj failed: {readobj_result.stderr.strip()}",
            failures,
        )
        if readobj_result.returncode == 0:
            readobj_stdout = readobj_result.stdout
        objdump_result = run_command([llvm_objdump, "--syms", str(obj_path)], ROOT)
        checks_total += 1
        checks_passed += require(
            objdump_result.returncode == 0,
            display_path(obj_path),
            f"{spec.case_id}-OBJDUMP-STATUS",
            f"llvm-objdump failed: {objdump_result.stderr.strip()}",
            failures,
        )
        if objdump_result.returncode == 0:
            objdump_stdout = objdump_result.stdout

    sections = extract_metadata_sections(readobj_stdout)
    symbols = extract_symbol_offsets(objdump_stdout)
    section_inventory = [
        {
            "name": name,
            "raw_data_size": payload["raw_data_size"],
            "relocation_count": payload["relocation_count"],
        }
        for name, payload in sorted(sections.items())
    ]
    tracked_symbols = {
        name: symbols[name]
        for name in sorted(set(spec.expected_present_symbols) | set(spec.expected_zero_offset_symbols) | set(spec.expected_nonzero_offset_symbols))
        if name in symbols
    }

    case_summary: dict[str, Any] = {
        "case_id": spec.case_id,
        "fixture": display_path(spec.fixture),
        "out_dir": display_path(out_dir),
        "process_exit_code": compile_result.returncode,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "object_exists": obj_path.exists(),
        "backend": backend,
        "diagnostics_count": diagnostics_seen,
        "named_metadata_present": NAMED_METADATA_LINE in ir_text,
        "boundary_line": boundary_line,
        "sections": section_inventory,
        "tracked_symbol_offsets": tracked_symbols,
    }

    checks_total += 1
    checks_passed += require(
        compile_result.returncode == 0,
        display_path(spec.fixture),
        f"{spec.case_id}-COMPILE-STATUS",
        "native compile must succeed",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        manifest_path.exists(),
        display_path(out_dir),
        f"{spec.case_id}-MANIFEST",
        "module.manifest.json must exist",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        ir_path.exists(),
        display_path(out_dir),
        f"{spec.case_id}-IR",
        "module.ll must exist",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        obj_path.exists(),
        display_path(out_dir),
        f"{spec.case_id}-OBJECT",
        "module.obj must exist",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        backend == "llvm-direct",
        display_path(backend_path),
        f"{spec.case_id}-BACKEND",
        "object backend must be llvm-direct",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        diagnostics_seen == 0,
        display_path(manifest_path if manifest_path.exists() else out_dir),
        f"{spec.case_id}-DIAGNOSTICS",
        "manifest diagnostics must be empty",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        NAMED_METADATA_LINE in ir_text,
        display_path(ir_path if ir_path.exists() else out_dir),
        f"{spec.case_id}-NAMED-METADATA",
        "IR must publish !objc3.objc_runtime_binary_inspection_harness",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        bool(boundary_line),
        display_path(ir_path if ir_path.exists() else out_dir),
        f"{spec.case_id}-BOUNDARY-LINE",
        "IR must publish the binary inspection harness boundary line",
        failures,
    )
    for index, token in enumerate(COMMON_BOUNDARY_TOKENS, start=1):
        checks_total += 1
        checks_passed += require(
            token in boundary_line,
            display_path(ir_path if ir_path.exists() else out_dir),
            f"{spec.case_id}-BOUNDARY-TOKEN-{index:02d}",
            f"boundary line missing token: {token}",
            failures,
        )

    expected_section_names = sorted(expectation.name for expectation in spec.expected_sections)
    actual_section_names = sorted(sections.keys())
    checks_total += 1
    checks_passed += require(
        actual_section_names == expected_section_names,
        display_path(obj_path if obj_path.exists() else out_dir),
        f"{spec.case_id}-SECTION-INVENTORY",
        f"unexpected metadata section inventory: {actual_section_names}",
        failures,
    )
    for section_name in spec.expected_absent_sections:
        checks_total += 1
        checks_passed += require(
            section_name not in sections,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-ABSENT-SECTION-{section_name}",
            f"section must be absent: {section_name}",
            failures,
        )
    for expectation in spec.expected_sections:
        payload = sections.get(expectation.name, {})
        raw_size = payload.get("raw_data_size") if isinstance(payload, dict) else None
        relocations = payload.get("relocation_count") if isinstance(payload, dict) else None
        checks_total += 1
        checks_passed += require(
            raw_size is not None and raw_size >= expectation.min_raw_size,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-SECTION-RAW-MIN-{expectation.name}",
            f"section {expectation.name} raw size below minimum",
            failures,
        )
        if expectation.max_raw_size is not None:
            checks_total += 1
            checks_passed += require(
                raw_size is not None and raw_size <= expectation.max_raw_size,
                display_path(obj_path if obj_path.exists() else out_dir),
                f"{spec.case_id}-SECTION-RAW-MAX-{expectation.name}",
                f"section {expectation.name} raw size above maximum",
                failures,
            )
        checks_total += 1
        checks_passed += require(
            relocations is not None and relocations >= expectation.min_relocations,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-SECTION-RELOC-MIN-{expectation.name}",
            f"section {expectation.name} relocation count below minimum",
            failures,
        )
        if expectation.max_relocations is not None:
            checks_total += 1
            checks_passed += require(
                relocations is not None and relocations <= expectation.max_relocations,
                display_path(obj_path if obj_path.exists() else out_dir),
                f"{spec.case_id}-SECTION-RELOC-MAX-{expectation.name}",
                f"section {expectation.name} relocation count above maximum",
                failures,
            )

    for symbol in spec.expected_present_symbols:
        checks_total += 1
        checks_passed += require(
            symbol in symbols,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-SYMBOL-PRESENT-{symbol}",
            f"symbol must be present: {symbol}",
            failures,
        )
    for symbol in spec.expected_absent_symbols:
        checks_total += 1
        checks_passed += require(
            symbol not in symbols,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-SYMBOL-ABSENT-{symbol}",
            f"symbol must be absent: {symbol}",
            failures,
        )
    for symbol in spec.expected_zero_offset_symbols:
        checks_total += 1
        checks_passed += require(
            symbols.get(symbol) == 0,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-SYMBOL-ZERO-{symbol}",
            f"symbol must have zero offset: {symbol}",
            failures,
        )
    for symbol in spec.expected_nonzero_offset_symbols:
        checks_total += 1
        checks_passed += require(
            symbols.get(symbol, 0) > 0,
            display_path(obj_path if obj_path.exists() else out_dir),
            f"{spec.case_id}-SYMBOL-NONZERO-{symbol}",
            f"symbol must have nonzero offset: {symbol}",
            failures,
        )

    return case_summary, checks_total, checks_passed


def run_negative_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root.resolve() / "negative-missing-interface-property"
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_cmd = [
        str(args.native_exe.resolve()),
        str(args.negative_fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_cmd, ROOT)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    diagnostics_txt_path = out_dir / "module.diagnostics.txt"
    diagnostics_preview = ""
    if diagnostics_txt_path.exists():
        diagnostics_preview = diagnostics_txt_path.read_text(encoding="utf-8").strip()

    case_summary: dict[str, Any] = {
        "case_id": "M253-C006-CASE-NEGATIVE-MISSING-INTERFACE-PROPERTY",
        "fixture": display_path(args.negative_fixture),
        "out_dir": display_path(out_dir),
        "process_exit_code": compile_result.returncode,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "object_exists": obj_path.exists(),
        "backend_exists": backend_path.exists(),
        "diagnostics_json_exists": diagnostics_json_path.exists(),
        "diagnostics_txt_exists": diagnostics_txt_path.exists(),
        "diagnostics_preview": diagnostics_preview,
        "inspection_blocked": True,
        "inspection_block_reason": "compile-failed-before-object-emission",
    }

    checks_total += 1
    checks_passed += require(
        compile_result.returncode != 0,
        display_path(args.negative_fixture),
        "M253-C006-NEGATIVE-COMPILE-STATUS",
        "negative metadata fixture must fail compilation",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        not manifest_path.exists(),
        display_path(out_dir),
        "M253-C006-NEGATIVE-NO-MANIFEST",
        "negative compile must not emit module.manifest.json",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        not obj_path.exists(),
        display_path(out_dir),
        "M253-C006-NEGATIVE-NO-OBJECT",
        "negative compile must not emit module.obj",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        not backend_path.exists(),
        display_path(out_dir),
        "M253-C006-NEGATIVE-NO-BACKEND",
        "negative compile must not emit backend marker",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        diagnostics_json_path.exists(),
        display_path(out_dir),
        "M253-C006-NEGATIVE-DIAGNOSTICS-JSON",
        "negative compile must emit diagnostics json",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        diagnostics_txt_path.exists(),
        display_path(out_dir),
        "M253-C006-NEGATIVE-DIAGNOSTICS-TXT",
        "negative compile must emit diagnostics text",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        "property" in diagnostics_preview.lower(),
        display_path(diagnostics_txt_path if diagnostics_txt_path.exists() else out_dir),
        "M253-C006-NEGATIVE-DIAGNOSTIC-CONTENT",
        "negative diagnostics preview must mention property failure",
        failures,
    )

    return case_summary, checks_total, checks_passed


def run_dynamic_probes(
    args: argparse.Namespace,
    failures: list[Finding],
) -> tuple[list[dict[str, Any]], int, int, dict[str, str]]:
    checks_total = 0
    checks_passed = 0
    dynamic_cases: list[dict[str, Any]] = []
    tool_paths: dict[str, str] = {}

    llvm_readobj = resolve_command(args.llvm_readobj)
    llvm_objdump = resolve_command(args.llvm_objdump)
    if llvm_readobj is None:
        failures.append(Finding("dynamic", "M253-C006-TOOL-READOBJ", "llvm-readobj not found"))
    else:
        tool_paths["llvm_readobj"] = llvm_readobj
    if llvm_objdump is None:
        failures.append(Finding("dynamic", "M253-C006-TOOL-OBJDUMP", "llvm-objdump not found"))
    else:
        tool_paths["llvm_objdump"] = llvm_objdump

    if llvm_readobj is None or llvm_objdump is None:
        return dynamic_cases, checks_total, checks_passed, tool_paths

    for spec in make_positive_probe_specs(args):
        case_summary, case_total, case_passed = run_positive_probe(
            spec, args, llvm_readobj, llvm_objdump, failures
        )
        dynamic_cases.append(case_summary)
        checks_total += case_total
        checks_passed += case_passed

    negative_summary, negative_total, negative_passed = run_negative_probe(args, failures)
    dynamic_cases.append(negative_summary)
    checks_total += negative_total
    checks_passed += negative_passed
    return dynamic_cases, checks_total, checks_passed, tool_paths


def build_static_checks(args: argparse.Namespace) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_passed = 0
    for attribute, snippets in ARTIFACT_SNIPPETS.items():
        path = getattr(args, attribute)
        checks_passed += ensure_snippets(path, snippets, failures)
    return checks_passed, failures


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    checks_passed, failures = build_static_checks(args)
    checks_total = sum(len(snippets) for snippets in ARTIFACT_SNIPPETS.values())

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_tools: dict[str, str] = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        cases, dynamic_total, dynamic_passed, dynamic_tools = run_dynamic_probes(args, failures)
        dynamic_cases = cases
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "dynamic_tools": dynamic_tools,
        "dynamic_cases": dynamic_cases,
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks)")
    print(f"[summary] {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
