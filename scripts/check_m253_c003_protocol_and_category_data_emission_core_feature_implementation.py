#!/usr/bin/env python3
"""Fail-closed contract checker for M253-C003 protocol/category data emission."""

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
MODE = "m253-c003-protocol-and-category-data-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-protocol-category-data-emission/m253-c003-v1"
BOUNDARY_COMMENT_PREFIX = (
    "; runtime_metadata_protocol_category_emission = "
    "contract=objc3c-runtime-protocol-category-data-emission/m253-c003-v1"
)
NAMED_METADATA_LINE = "!objc3.objc_runtime_protocol_category_emission = !{!57}"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_protocol_and_category_data_emission_core_feature_implementation_c003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_c003_protocol_and_category_data_emission_core_feature_implementation_packet.md"
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
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "protocol-category-data-emission"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-C003/protocol_and_category_data_emission_summary.json")
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
    SnippetCheck("M253-C003-DOC-EXP-01", "# M253 Protocol And Category Data Emission Core Feature Implementation Expectations (C003)"),
    SnippetCheck("M253-C003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-C003-DOC-EXP-03", "`protocol-descriptor-bundles-with-inherited-protocol-ref-lists`"),
    SnippetCheck("M253-C003-DOC-EXP-04", "`category-descriptor-bundles-with-attachment-and-protocol-ref-lists`"),
    SnippetCheck("M253-C003-DOC-EXP-05", "`@__objc3_meta_category_adopted_protocol_refs_0000`"),
    SnippetCheck("M253-C003-DOC-EXP-06", "`tmp/reports/m253/M253-C003/protocol_and_category_data_emission_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-DOC-PKT-01", "# M253-C003 Protocol And Category Data Emission Core Feature Implementation Packet"),
    SnippetCheck("M253-C003-DOC-PKT-02", "Packet: `M253-C003`"),
    SnippetCheck("M253-C003-DOC-PKT-03", "- `M253-C002`"),
    SnippetCheck("M253-C003-DOC-PKT-04", "- `M252-C002`"),
    SnippetCheck("M253-C003-DOC-PKT-05", "- `M253-B002`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-ARCH-01", "M253 lane-C C003 protocol and category data emission anchors explicit"),
    SnippetCheck("M253-C003-ARCH-02", "runtime_metadata_protocol_bundles_lexicographic"),
    SnippetCheck("M253-C003-ARCH-03", "runtime_metadata_category_bundles_lexicographic"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-NDOC-01", "## Protocol and category data emission (M253-C003)"),
    SnippetCheck("M253-C003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C003-NDOC-03", "`@__objc3_meta_protocol_inherited_protocol_refs_0001`"),
    SnippetCheck("M253-C003-NDOC-04", "`@__objc3_meta_category_attachments_0001`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-SPC-01", "## M253 protocol and category data emission (C003)"),
    SnippetCheck("M253-C003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C003-SPC-03", "protocol-descriptor-bundles-with-inherited-protocol-ref-lists"),
    SnippetCheck("M253-C003-SPC-04", "category-descriptor-bundles-with-attachment-and-protocol-ref-lists"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-META-01", "## M253 protocol and category data emission metadata anchors (C003)"),
    SnippetCheck("M253-C003-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C003-META-03", "`__objc3_meta_protocol_inherited_protocol_refs_0001`"),
    SnippetCheck("M253-C003-META-04", "`__objc3_meta_category_attachments_0001`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-LHDR-01", "kObjc3RuntimeProtocolCategoryEmissionContractId"),
    SnippetCheck("M253-C003-LHDR-02", "kObjc3RuntimeProtocolEmissionPayloadModel"),
    SnippetCheck("M253-C003-LHDR-03", "kObjc3RuntimeCategoryEmissionPayloadModel"),
    SnippetCheck("M253-C003-LHDR-04", "kObjc3RuntimeProtocolReferenceModel"),
    SnippetCheck("M253-C003-LHDR-05", "kObjc3RuntimeCategoryAttachmentModel"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-LCPP-01", "Objc3RuntimeMetadataProtocolCategoryEmissionSummary()"),
    SnippetCheck("M253-C003-LCPP-02", "M253-C003 protocol/category data emission anchor"),
    SnippetCheck("M253-C003-LCPP-03", "non_goals=no-selector-string-pool-or-standalone-property-ivar-payloads"),
)

IR_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-IHDR-01", "struct Objc3IRRuntimeMetadataProtocolBundle {"),
    SnippetCheck("M253-C003-IHDR-02", "struct Objc3IRRuntimeMetadataCategoryBundle {"),
    SnippetCheck("M253-C003-IHDR-03", "runtime_metadata_protocol_bundles_lexicographic;"),
    SnippetCheck("M253-C003-IHDR-04", "runtime_metadata_category_bundles_lexicographic;"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-IR-01", 'out << "!objc3.objc_runtime_protocol_category_emission = !{!57}\\n";'),
    SnippetCheck("M253-C003-IR-02", 'out << "; runtime_metadata_protocol_category_emission = "'),
    SnippetCheck("M253-C003-IR-03", '"inherited_protocol_refs", i);'),
    SnippetCheck("M253-C003-IR-04", '"adopted_protocol_refs", i);'),
    SnippetCheck("M253-C003-IR-05", '"attachments", i);'),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-ART-01", "M253-C003 protocol/category data emission anchor"),
    SnippetCheck("M253-C003-ART-02", 'append_category_bundle('),
    SnippetCheck("M253-C003-ART-03", "runtime_metadata_protocol_bundles_lexicographic"),
    SnippetCheck("M253-C003-ART-04", "runtime_metadata_category_bundles_lexicographic"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C003-PROC-01", "M253-C003 protocol/category data emission anchor"),
    SnippetCheck("M253-C003-PROC-02", "inherited/adopted protocol-ref lists"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-C003-PKG-01",
        '"check:objc3c:m253-c003-protocol-and-category-data-emission-core-feature-implementation": "python scripts/check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M253-C003-PKG-02",
        '"test:tooling:m253-c003-protocol-and-category-data-emission-core-feature-implementation": "python -m pytest tests/tooling/test_check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M253-C003-PKG-03",
        '"check:objc3c:m253-c003-lane-c-readiness": "npm run check:objc3c:m253-c002-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-c003-protocol-and-category-data-emission-core-feature-implementation && npm run test:tooling:m253-c003-protocol-and-category-data-emission-core-feature-implementation"',
    ),
)

PROTOCOL_PLACEHOLDER_RE = re.compile(
    r'^@__objc3_meta_protocol_\d{4} = private global \[1 x i8\] zeroinitializer',
    re.MULTILINE,
)
CATEGORY_PLACEHOLDER_RE = re.compile(
    r'^@__objc3_meta_category_\d{4} = private global \[1 x i8\] zeroinitializer',
    re.MULTILINE,
)
PROTOCOL_BUNDLE_RE = re.compile(
    r'^@__objc3_meta_protocol_\d{4} = private global \{ ptr, ptr, ptr(?:, ptr, ptr)?, i64, i64(?:, i64, i64)?, i1 \}',
    re.MULTILINE,
)
CATEGORY_BUNDLE_RE = re.compile(
    r'^@__objc3_meta_category_\d{4} = private global \{ ptr, ptr, ptr, ptr, ptr, ptr(?:, ptr, ptr, ptr, ptr)?, i64, i64, i64 \}',
    re.MULTILINE,
)
INHERITED_REFS_RE = re.compile(
    r'^@__objc3_meta_protocol_inherited_protocol_refs_\d{4} = private global ',
    re.MULTILINE,
)
ADOPTED_REFS_RE = re.compile(
    r'^@__objc3_meta_category_adopted_protocol_refs_\d{4} = private global ',
    re.MULTILINE,
)
ATTACHMENTS_RE = re.compile(
    r'^@__objc3_meta_category_attachments_\d{4} = private global ',
    re.MULTILINE,
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def summary_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


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


def run_native_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / "native-protocol-category"
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

    checks_total = 0
    checks_passed = 0
    case: dict[str, Any] = {
        "probe_dir": display_path(probe_dir),
        "command": command,
        "process_exit_code": result.returncode,
    }

    for artifact_path, check_id in (
        (ir_path, "M253-C003-DYN-01"),
        (obj_path, "M253-C003-DYN-02"),
        (manifest_path, "M253-C003-DYN-03"),
        (backend_path, "M253-C003-DYN-04"),
        (runtime_metadata_bin_path, "M253-C003-DYN-05"),
        (diagnostics_path, "M253-C003-DYN-06"),
    ):
        checks_total += 1
        checks_passed += require(
            artifact_path.exists(),
            display_path(artifact_path),
            check_id,
            "expected probe artifact to exist",
            failures,
        )

    checks_total += 1
    checks_passed += require(
        result.returncode == 0,
        display_path(args.fixture),
        "M253-C003-DYN-07",
        f"native probe exited with {result.returncode}: {result.stderr.strip()}",
        failures,
    )

    ir_text = read_text(ir_path) if ir_path.exists() else ""
    backend_text = read_text(backend_path).strip() if backend_path.exists() else ""
    diagnostics_text = read_text(diagnostics_path) if diagnostics_path.exists() else ""
    case["backend"] = backend_text
    case["diagnostics_is_empty"] = diagnostics_text == ""

    checks_total += 1
    checks_passed += require(
        backend_text == "llvm-direct",
        display_path(backend_path),
        "M253-C003-DYN-08",
        f"expected llvm-direct backend, saw {backend_text!r}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        diagnostics_text == "",
        display_path(diagnostics_path),
        "M253-C003-DYN-09",
        f"expected empty diagnostics, saw {diagnostics_text!r}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        NAMED_METADATA_LINE in ir_text,
        display_path(ir_path),
        "M253-C003-DYN-10",
        "missing protocol/category named metadata line",
        failures,
    )

    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    case["boundary_line"] = boundary_line
    checks_total += 1
    checks_passed += require(
        boundary_line.startswith(BOUNDARY_COMMENT_PREFIX),
        display_path(ir_path),
        "M253-C003-DYN-11",
        "missing protocol/category boundary comment",
        failures,
    )

    protocol_bundle_count = len(PROTOCOL_BUNDLE_RE.findall(ir_text))
    category_bundle_count = len(CATEGORY_BUNDLE_RE.findall(ir_text))
    inherited_ref_count = len(INHERITED_REFS_RE.findall(ir_text))
    adopted_ref_count = len(ADOPTED_REFS_RE.findall(ir_text))
    attachment_list_count = len(ATTACHMENTS_RE.findall(ir_text))
    case["protocol_bundle_count"] = protocol_bundle_count
    case["category_bundle_count"] = category_bundle_count
    case["inherited_protocol_ref_list_count"] = inherited_ref_count
    case["adopted_protocol_ref_list_count"] = adopted_ref_count
    case["category_attachment_list_count"] = attachment_list_count

    for value, expected, check_id, detail in (
        (protocol_bundle_count, 2, "M253-C003-DYN-12", "expected two protocol descriptor bundles"),
        (category_bundle_count, 2, "M253-C003-DYN-13", "expected two category descriptor bundles"),
        (inherited_ref_count, 2, "M253-C003-DYN-14", "expected two inherited-protocol ref list globals"),
        (adopted_ref_count, 2, "M253-C003-DYN-15", "expected two adopted-protocol ref list globals"),
        (attachment_list_count, 2, "M253-C003-DYN-16", "expected two category attachment list globals"),
    ):
        checks_total += 1
        checks_passed += require(value == expected, display_path(ir_path), check_id, detail, failures)

    checks_total += 1
    checks_passed += require(
        PROTOCOL_PLACEHOLDER_RE.search(ir_text) is None,
        display_path(ir_path),
        "M253-C003-DYN-17",
        "protocol placeholder globals remain on the happy path",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        CATEGORY_PLACEHOLDER_RE.search(ir_text) is None,
        display_path(ir_path),
        "M253-C003-DYN-18",
        "category placeholder globals remain on the happy path",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        "ptr @__objc3_meta_protocol_0000" in ir_text,
        display_path(ir_path),
        "M253-C003-DYN-19",
        "expected inherited/adopted protocol references to point at protocol descriptors",
        failures,
    )

    readobj_path = resolve_command(args.llvm_readobj)
    objdump_path = resolve_command(args.llvm_objdump)
    checks_total += 1
    checks_passed += require(
        readobj_path is not None,
        args.llvm_readobj,
        "M253-C003-DYN-20",
        f"unable to resolve {args.llvm_readobj}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        objdump_path is not None,
        args.llvm_objdump,
        "M253-C003-DYN-21",
        f"unable to resolve {args.llvm_objdump}",
        failures,
    )

    readobj_text = ""
    objdump_text = ""
    if readobj_path and obj_path.exists():
        readobj_result = run_command([readobj_path, "--sections", str(obj_path)], ROOT)
        case["readobj_exit_code"] = readobj_result.returncode
        readobj_text = readobj_result.stdout
        checks_total += 1
        checks_passed += require(
            readobj_result.returncode == 0,
            display_path(obj_path),
            "M253-C003-DYN-22",
            f"llvm-readobj failed: {readobj_result.stderr.strip()}",
            failures,
        )
    if objdump_path and obj_path.exists():
        objdump_result = run_command([objdump_path, "--syms", str(obj_path)], ROOT)
        case["objdump_exit_code"] = objdump_result.returncode
        objdump_text = objdump_result.stdout
        checks_total += 1
        checks_passed += require(
            objdump_result.returncode == 0,
            display_path(obj_path),
            "M253-C003-DYN-23",
            f"llvm-objdump failed: {objdump_result.stderr.strip()}",
            failures,
        )

    protocol_raw_size, protocol_relocations = parse_readobj_section(
        readobj_text, "objc3.runtime.protocol_descriptors"
    )
    category_raw_size, category_relocations = parse_readobj_section(
        readobj_text, "objc3.runtime.category_descriptors"
    )
    case["protocol_section_raw_size"] = protocol_raw_size
    case["protocol_section_relocations"] = protocol_relocations
    case["category_section_raw_size"] = category_raw_size
    case["category_section_relocations"] = category_relocations

    checks_total += 1
    checks_passed += require(
        protocol_raw_size is not None and protocol_raw_size > 64,
        display_path(obj_path),
        "M253-C003-DYN-24",
        f"expected nontrivial protocol section raw size, saw {protocol_raw_size}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        protocol_relocations is not None and protocol_relocations >= 4,
        display_path(obj_path),
        "M253-C003-DYN-25",
        f"expected protocol section relocations, saw {protocol_relocations}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        category_raw_size is not None and category_raw_size > 128,
        display_path(obj_path),
        "M253-C003-DYN-26",
        f"expected nontrivial category section raw size, saw {category_raw_size}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        category_relocations is not None and category_relocations >= 8,
        display_path(obj_path),
        "M253-C003-DYN-27",
        f"expected category section relocations, saw {category_relocations}",
        failures,
    )

    checks_total += 1
    checks_passed += require(
        "__objc3_sec_protocol_descriptors" in objdump_text,
        display_path(obj_path),
        "M253-C003-DYN-28",
        "missing protocol aggregate symbol in objdump output",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        "__objc3_sec_category_descriptors" in objdump_text,
        display_path(obj_path),
        "M253-C003-DYN-29",
        "missing category aggregate symbol in objdump output",
        failures,
    )

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
        dynamic_total, dynamic_passed, case = run_native_probe(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed
        dynamic_cases.append(case)

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    out_path = summary_path(args.summary_out)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
