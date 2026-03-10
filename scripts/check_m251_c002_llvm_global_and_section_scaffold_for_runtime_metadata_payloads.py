#!/usr/bin/env python3
"""Fail-closed contract checker for M251-C002 runtime metadata section scaffold emission."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-c002-runtime-metadata-section-scaffold-v1"
CONTRACT_ID = "objc3c-runtime-metadata-section-scaffold/m251-c002-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_llvm_global_and_section_scaffold_for_runtime_metadata_payloads_c002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads_packet.md"
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
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_CATEGORY_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m251_runtime_metadata_section_scaffold_all_sections.objc3"
)
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-metadata-section-scaffold"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-C002/runtime_metadata_section_scaffold_summary.json"
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
    SnippetCheck("M251-C002-DOC-EXP-01", "# M251 LLVM Global and Section Scaffold for Runtime Metadata Payloads Expectations (C002)"),
    SnippetCheck("M251-C002-DOC-EXP-02", "Contract ID: `objc3c-runtime-metadata-section-scaffold/m251-c002-v1`"),
    SnippetCheck("M251-C002-DOC-EXP-03", "`Objc3RuntimeMetadataSectionScaffoldSummary` becomes the canonical lane-C scaffold packet"),
    SnippetCheck("M251-C002-DOC-EXP-04", "The scaffold is retained through `@llvm.used`"),
    SnippetCheck("M251-C002-DOC-EXP-05", "`tmp/reports/m251/M251-C002/runtime_metadata_section_scaffold_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-DOC-PKT-01", "# M251-C002 LLVM Global and Section Scaffold for Runtime Metadata Payloads Packet"),
    SnippetCheck("M251-C002-DOC-PKT-02", "Packet: `M251-C002`"),
    SnippetCheck("M251-C002-DOC-PKT-03", "- `M251-C001`"),
    SnippetCheck("M251-C002-DOC-PKT-04", "Emit retained aggregate globals at:"),
    SnippetCheck("M251-C002-DOC-PKT-05", "Retain the emitted globals through `@llvm.used`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-ARCH-01", "M251 lane-C C002 LLVM global and section scaffold for runtime metadata payloads"),
    SnippetCheck("M251-C002-ARCH-02", "m251_llvm_global_and_section_scaffold_for_runtime_metadata_payloads_c002_expectations.md"),
    SnippetCheck("M251-C002-ARCH-03", "per-record descriptor placeholders, and `@llvm.used` retention"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-NDOC-01", "## LLVM global and section scaffold for runtime metadata payloads (M251-C002)"),
    SnippetCheck("M251-C002-NDOC-02", "`Objc3RuntimeMetadataSectionScaffoldSummary`"),
    SnippetCheck("M251-C002-NDOC-03", "`!objc3.objc_runtime_metadata_section_scaffold`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-SPC-01", "## M251 runtime metadata section scaffold emission (C002)"),
    SnippetCheck("M251-C002-SPC-02", "`Objc3RuntimeMetadataSectionScaffoldSummary` to remain the single lane-C"),
    SnippetCheck("M251-C002-SPC-03", "`@llvm.used` to retain the scaffolded metadata globals"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-META-01", "## M251 runtime metadata section scaffold metadata anchors (C002)"),
    SnippetCheck("M251-C002-META-02", "contract id `objc3c-runtime-metadata-section-scaffold/m251-c002-v1`"),
    SnippetCheck("M251-C002-META-03", "`!objc3.objc_runtime_metadata_section_scaffold`"),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-AST-01", "kObjc3RuntimeMetadataSectionScaffoldContractId"),
    SnippetCheck("M251-C002-AST-02", "kObjc3RuntimeMetadataClassDescriptorAggregateSymbol"),
    SnippetCheck("M251-C002-AST-03", "kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-TYP-01", "struct Objc3RuntimeMetadataSectionScaffoldSummary {"),
    SnippetCheck("M251-C002-TYP-02", "bool uses_llvm_used = false;"),
    SnippetCheck("M251-C002-TYP-03", "std::string class_aggregate_symbol ="),
    SnippetCheck("M251-C002-TYP-04", "summary.total_retained_global_count =="),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-ART-01", "BuildRuntimeMetadataSectionScaffoldSummary("),
    SnippetCheck("M251-C002-ART-02", "runtime_metadata_section_scaffold_contract_id"),
    SnippetCheck("M251-C002-ART-03", "runtime_metadata_section_scaffold_total_retained_global_count"),
    SnippetCheck("M251-C002-ART-04", "ir_frontend_metadata.runtime_metadata_section_scaffold_contract_id ="),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-IRH-01", "std::string runtime_metadata_section_scaffold_contract_id;"),
    SnippetCheck("M251-C002-IRH-02", "bool runtime_metadata_section_scaffold_emitted = false;"),
    SnippetCheck("M251-C002-IRH-03", "std::string runtime_metadata_section_scaffold_class_aggregate_symbol;"),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-IRC-01", "void EmitRuntimeMetadataSectionScaffold(std::ostringstream &out) const {"),
    SnippetCheck("M251-C002-IRC-02", "@llvm.used = appending global ["),
    SnippetCheck("M251-C002-IRC-03", "!objc3.objc_runtime_metadata_section_scaffold = !{!49}"),
    SnippetCheck("M251-C002-IRC-04", "runtime_metadata_section_scaffold_total_retained_global_count"),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-DRV-01", "manifest emission mirrors the live runtime-metadata"),
    SnippetCheck("M251-C002-DRV-02", "LLVM IR/object evidence without treating the manifest as the only source."),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-SHIM-01", "M251-C002 scaffold: the native driver now emits retained metadata placeholder"),
    SnippetCheck("M251-C002-SHIM-02", "runtime registration, lookup, or executable object-model implementation."),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C002-PKG-01", '"check:objc3c:m251-c002-llvm-global-and-section-scaffold-for-runtime-metadata-payloads": "python scripts/check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads.py"'),
    SnippetCheck("M251-C002-PKG-02", '"test:tooling:m251-c002-llvm-global-and-section-scaffold-for-runtime-metadata-payloads": "python -m pytest tests/tooling/test_check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads.py -q"'),
    SnippetCheck("M251-C002-PKG-03", '"check:objc3c:m251-c002-lane-c-readiness": "npm run check:objc3c:m251-c001-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m251-c002-llvm-global-and-section-scaffold-for-runtime-metadata-payloads && npm run test:tooling:m251-c002-llvm-global-and-section-scaffold-for-runtime-metadata-payloads"'),
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
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--hello-fixture", type=Path, default=DEFAULT_HELLO_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
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


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[list[dict[str, object]], int, int]:
    cases: list[dict[str, object]] = []
    checks_total = 0
    checks_passed = 0
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)

    manifest_out = probe_root / "manifest_only"
    manifest_out.mkdir(parents=True, exist_ok=True)
    manifest_cmd = [
        str(args.runner_exe.resolve()),
        str(args.class_fixture.resolve()),
        "--out-dir",
        str(manifest_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    manifest_result = run_command(manifest_cmd, ROOT)
    manifest_summary_path = manifest_out / "module.c_api_summary.json"
    manifest_path = manifest_out / "module.manifest.json"
    manifest_case: dict[str, object] = {
        "case_id": "M251-C002-CASE-MANIFEST-CLASS",
        "command": manifest_cmd,
        "process_exit_code": manifest_result.returncode,
        "summary_path": display_path(manifest_summary_path),
        "manifest_path": display_path(manifest_path),
    }
    checks_total += 10
    if manifest_result.returncode == 0 and manifest_summary_path.exists() and manifest_path.exists():
        manifest_summary = load_json(manifest_summary_path)
        manifest = load_json(manifest_path)
        sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
        manifest_case["status"] = manifest_summary["status"]
        manifest_case["success"] = manifest_summary["success"]
        manifest_case["runtime_metadata_section_scaffold_contract_id"] = sema["runtime_metadata_section_scaffold_contract_id"]
        manifest_case["runtime_metadata_section_scaffold_emitted"] = sema["runtime_metadata_section_scaffold_emitted"]
        manifest_case["runtime_metadata_section_scaffold_uses_llvm_used"] = sema["runtime_metadata_section_scaffold_uses_llvm_used"]
        manifest_case["runtime_metadata_section_scaffold_class_descriptor_count"] = sema["runtime_metadata_section_scaffold_class_descriptor_count"]
        manifest_case["runtime_metadata_section_scaffold_protocol_descriptor_count"] = sema["runtime_metadata_section_scaffold_protocol_descriptor_count"]
        manifest_case["runtime_metadata_section_scaffold_category_descriptor_count"] = sema["runtime_metadata_section_scaffold_category_descriptor_count"]
        manifest_case["runtime_metadata_section_scaffold_property_descriptor_count"] = sema["runtime_metadata_section_scaffold_property_descriptor_count"]
        manifest_case["runtime_metadata_section_scaffold_ivar_descriptor_count"] = sema["runtime_metadata_section_scaffold_ivar_descriptor_count"]
        manifest_case["runtime_metadata_section_scaffold_total_descriptor_count"] = sema["runtime_metadata_section_scaffold_total_descriptor_count"]
        manifest_case["runtime_metadata_section_scaffold_total_retained_global_count"] = sema["runtime_metadata_section_scaffold_total_retained_global_count"]
        manifest_case["runtime_metadata_section_scaffold_image_info_symbol"] = sema["runtime_metadata_section_scaffold_image_info_symbol"]
        manifest_case["runtime_metadata_section_scaffold_class_aggregate_symbol"] = sema["runtime_metadata_section_scaffold_class_aggregate_symbol"]
        if manifest_summary["status"] == 0 and manifest_summary["success"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-STATUS", "manifest-only probe did not succeed"))
        if sema["runtime_metadata_section_scaffold_contract_id"] == CONTRACT_ID:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-CONTRACT", "manifest-only probe contract id mismatch"))
        if sema["runtime_metadata_section_scaffold_emitted"] is True:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-EMITTED", "manifest-only probe did not report scaffold emission"))
        if sema["runtime_metadata_section_scaffold_uses_llvm_used"] is True:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-LLVM-USED", "manifest-only probe did not report llvm.used retention"))
        if sema["runtime_metadata_section_scaffold_class_descriptor_count"] > 0:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-COUNT", "manifest-only probe reported zero class descriptors"))
        if sema["runtime_metadata_section_scaffold_protocol_descriptor_count"] > 0:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-PROTOCOL", "manifest-only probe reported zero protocol descriptors"))
        if sema["runtime_metadata_section_scaffold_property_descriptor_count"] > 0 and sema["runtime_metadata_section_scaffold_ivar_descriptor_count"] > 0:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-PROPERTY-IVAR", "manifest-only probe reported zero property or ivar descriptors"))
        if sema["runtime_metadata_section_scaffold_total_retained_global_count"] == sema["runtime_metadata_section_scaffold_total_descriptor_count"] + 6:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-RETENTION-COUNT", "manifest-only probe retained-global count drifted"))
        if sema["runtime_metadata_section_scaffold_image_info_symbol"] == "__objc3_image_info":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-IMAGE-SYMBOL", "manifest-only probe image-info symbol drifted"))
        if sema["runtime_metadata_section_scaffold_class_aggregate_symbol"] == "__objc3_sec_class_descriptors":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-AGGREGATE", "manifest-only probe class aggregate symbol drifted"))
    else:
        failures.append(Finding("dynamic", "M251-C002-CASE-MANIFEST-CLASS-EXIT", f"runner exited {manifest_result.returncode} or artifacts were missing"))
    cases.append(manifest_case)

    compile_out = probe_root / "native_hello"
    compile_out.mkdir(parents=True, exist_ok=True)
    compile_cmd = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(compile_out),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_cmd, ROOT)
    ir_path = compile_out / "module.ll"
    object_path = compile_out / "module.obj"
    compile_case: dict[str, object] = {
        "case_id": "M251-C002-CASE-NATIVE-HELLO",
        "command": compile_cmd,
        "process_exit_code": compile_result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(object_path),
    }
    checks_total += 11
    if compile_result.returncode == 0 and ir_path.exists() and object_path.exists():
        ir_text = read_text(ir_path)
        compile_case["ir_contains_contract_comment"] = f"; runtime_metadata_section_scaffold = {CONTRACT_ID}" in ir_text
        compile_case["ir_contains_image_info_global"] = '@__objc3_image_info = internal global { i32, i32 } zeroinitializer, section "objc3.runtime.image_info", align 4' in ir_text
        compile_case["ir_contains_zero_count_class_aggregate"] = ('@__objc3_sec_class_descriptors = internal global { i64 } { i64 0 }, section "objc3.runtime.class_descriptors", align 8' in ir_text or '@__objc3_sec_class_descriptors = internal constant { i64 } { i64 0 }, section "objc3.runtime.class_descriptors", align 8' in ir_text)
        compile_case["ir_contains_zero_count_protocol_aggregate"] = ('@__objc3_sec_protocol_descriptors = internal global { i64 } { i64 0 }, section "objc3.runtime.protocol_descriptors", align 8' in ir_text or '@__objc3_sec_protocol_descriptors = internal constant { i64 } { i64 0 }, section "objc3.runtime.protocol_descriptors", align 8' in ir_text)
        compile_case["ir_contains_zero_count_category_aggregate"] = ('@__objc3_sec_category_descriptors = internal global { i64 } { i64 0 }, section "objc3.runtime.category_descriptors", align 8' in ir_text or '@__objc3_sec_category_descriptors = internal constant { i64 } { i64 0 }, section "objc3.runtime.category_descriptors", align 8' in ir_text)
        compile_case["ir_contains_zero_count_property_aggregate"] = ('@__objc3_sec_property_descriptors = internal global { i64 } { i64 0 }, section "objc3.runtime.property_descriptors", align 8' in ir_text or '@__objc3_sec_property_descriptors = internal constant { i64 } { i64 0 }, section "objc3.runtime.property_descriptors", align 8' in ir_text)
        compile_case["ir_contains_zero_count_ivar_aggregate"] = ('@__objc3_sec_ivar_descriptors = internal global { i64 } { i64 0 }, section "objc3.runtime.ivar_descriptors", align 8' in ir_text or '@__objc3_sec_ivar_descriptors = internal constant { i64 } { i64 0 }, section "objc3.runtime.ivar_descriptors", align 8' in ir_text)
        compile_case["ir_contains_llvm_used"] = '@llvm.used = appending global [' in ir_text
        compile_case["ir_contains_named_metadata"] = '!objc3.objc_runtime_metadata_section_scaffold = !{!49}' in ir_text
        compile_case["object_exists"] = object_path.exists()
        if compile_result.returncode == 0:
            checks_passed += 1
        if compile_case["ir_contains_contract_comment"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-COMMENT", "IR did not preserve the scaffold contract comment"))
        if compile_case["ir_contains_image_info_global"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-IMAGE", "IR did not emit the image-info scaffold global"))
        if compile_case["ir_contains_zero_count_class_aggregate"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-CLASS-AGGREGATE", "IR did not emit the zero-count class aggregate symbol"))
        if compile_case["ir_contains_zero_count_protocol_aggregate"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-PROTOCOL-AGGREGATE", "IR did not emit the zero-count protocol aggregate symbol"))
        if compile_case["ir_contains_zero_count_category_aggregate"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-CATEGORY-AGGREGATE", "IR did not emit the zero-count category aggregate symbol"))
        if compile_case["ir_contains_zero_count_property_aggregate"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-PROPERTY-AGGREGATE", "IR did not emit the zero-count property aggregate symbol"))
        if compile_case["ir_contains_zero_count_ivar_aggregate"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-IVAR-AGGREGATE", "IR did not emit the zero-count ivar aggregate symbol"))
        if compile_case["ir_contains_llvm_used"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-LLVM-USED", "IR did not retain scaffold globals with llvm.used"))
        if compile_case["ir_contains_named_metadata"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-METADATA", "IR did not emit the scaffold named metadata node"))
        if compile_case["object_exists"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-OBJECT", "native compile probe did not emit module.obj"))
    else:
        failures.append(Finding("dynamic", "M251-C002-CASE-NATIVE-HELLO-EXIT", f"native driver exited {compile_result.returncode} or artifacts were missing"))
    cases.append(compile_case)

    return cases, checks_total, checks_passed


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_h, IR_EMITTER_H_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        dynamic_cases, dynamic_total, dynamic_passed = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [failure.__dict__ for failure in failures],
    }
    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
        return 0

    for failure in failures:
        print(f"[fail] {failure.check_id} ({failure.artifact}): {failure.detail}")
    return 1


if __name__ == "__main__":
    raise SystemExit(run())
