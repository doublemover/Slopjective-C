#!/usr/bin/env python3
"""Validate M257-D002 instance allocation/layout runtime support."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "M257-D002"
CONTRACT_ID = "objc3c-runtime-instance-allocation-layout-support/m257-d002-v1"
DESCRIPTOR_MODEL = (
    "runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery"
)
ALLOCATOR_MODEL = (
    "alloc-new-materialize-distinct-runtime-instance-identities-backed-by-realized-class-layout"
)
STORAGE_MODEL = (
    "synthesized-accessor-execution-reads-and-writes-per-instance-slot-storage-using-emitted-ivar-offset-layout-records"
)
FAIL_CLOSED_MODEL = (
    "no-layout-rederivation-no-shared-global-property-storage-no-reflective-property-registration-yet"
)
SUMMARY_RELATIVE_PATH = "tmp/reports/m257/M257-D002/instance_allocation_layout_runtime_summary.json"
FIXTURE_SOURCE = "tests/tooling/fixtures/native/m257_d002_instance_allocation_runtime_positive.objc3"
RUNTIME_PROBE_SOURCE = "tests/tooling/runtime/m257_d002_instance_allocation_runtime_probe.cpp"
BOUNDARY_PREFIX = "; runtime_instance_allocation_layout_support = "
SYNTHESIZED_PREFIX = "; executable_synthesized_accessor_property_lowering = "
EXPECTED_BASE_IDENTITY = 1024
EXPECTED_INSTANCE_SIZE = 16
EXPECTED_COUNT_OWNER = "implementation:Widget::instance_method:count"
EXPECTED_SET_COUNT_OWNER = "implementation:Widget::instance_method:setCount:"

EXPECTATIONS_SNIPPETS = (
    "# M257 Instance Allocation Layout And Ivar Offset Runtime Support Core Feature Implementation Expectations (D002)",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{DESCRIPTOR_MODEL}`",
    f"`{ALLOCATOR_MODEL}`",
    f"`{STORAGE_MODEL}`",
    f"`{FAIL_CLOSED_MODEL}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
PACKET_SNIPPETS = (
    "# M257-D002 Instance Allocation Layout And Ivar Offset Runtime Support Core Feature Implementation Packet",
    "Packet: `M257-D002`",
    "Issue: `#7154`",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
NATIVE_DOC_SNIPPETS = (
    "## Instance allocation, layout, and ivar-offset runtime support (M257-D002)",
    CONTRACT_ID,
    ALLOCATOR_MODEL,
    STORAGE_MODEL,
    f"`{RUNTIME_PROBE_SOURCE}`",
)
LOWERING_SPEC_SNIPPETS = (
    "## M257 instance allocation, layout, and ivar-offset runtime support (D002)",
    CONTRACT_ID,
    DESCRIPTOR_MODEL,
    ALLOCATOR_MODEL,
    STORAGE_MODEL,
)
METADATA_SPEC_SNIPPETS = (
    "## M257 runtime instance-allocation and layout metadata anchors (D002)",
    CONTRACT_ID,
    "`; runtime_instance_allocation_layout_support = contract=objc3c-runtime-instance-allocation-layout-support/m257-d002-v1`",
    f"`{RUNTIME_PROBE_SOURCE}`",
)
ARCHITECTURE_SNIPPETS = (
    "## M257 instance allocation, layout, and ivar-offset runtime support (D002)",
    "runtime consumes emitted accessor implementation pointers and property/layout attachment identities without source rediscovery",
    "`M257-D002` upgrades the runtime surface immediately above `M257-D001`.",
    "synthesized property execution now uses realized per-instance slot storage",
)
RUNTIME_README_SNIPPETS = (
    "`M257-D002` upgrades the property/layout runtime from the D001 freeze into true",
    "per-instance allocation:",
    CONTRACT_ID,
    ALLOCATOR_MODEL,
    STORAGE_MODEL,
)
TOOLING_RUNTIME_README_SNIPPETS = (
    "`M257-D002` upgrades the same lookup/dispatch surface to true per-instance",
    "allocation:",
    CONTRACT_ID,
    "`tests/tooling/runtime/m257_d002_instance_allocation_runtime_probe.cpp`",
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3RuntimeInstanceAllocationLayoutSupportContractId",
    "kObjc3RuntimeInstanceAllocationLayoutSupportDescriptorModel",
    "kObjc3RuntimeInstanceAllocationLayoutSupportAllocatorModel",
    "kObjc3RuntimeInstanceAllocationLayoutSupportStorageModel",
    "kObjc3RuntimeInstanceAllocationLayoutSupportFailClosedModel",
    "Objc3RuntimeInstanceAllocationLayoutSupportSummary()",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary()",
    'out << "contract=" << kObjc3RuntimeInstanceAllocationLayoutSupportContractId',
    '<< ";descriptor_model="',
    '<< ";allocator_model="',
    '<< ";storage_model="',
    '<< ";fail_closed_model="',
)
AST_SNIPPETS = (
    "M257-D002 instance-allocation-layout-runtime anchor",
    "per-instance allocation and slot storage without rederiving layout from",
)
SEMA_SNIPPETS = (
    "M257-D002 instance-allocation-layout-runtime anchor",
    "without recovering property storage or allocator behavior from source.",
)
IR_SNIPPETS = (
    "M257-D002 instance-allocation-layout-runtime anchor",
    "; runtime_instance_allocation_layout_support = ",
    '<< ";property_descriptor_entries="',
    '<< ";ivar_layout_owner_entries="',
    '<< ";synthesized_accessor_entries="',
)
RUNTIME_HEADER_SNIPPETS = (
    "M257-D002 instance-allocation-layout-runtime anchor",
    "carries true per-instance allocation and slot-backed synthesized accessors",
)
BOOTSTRAP_HEADER_SNIPPETS = (
    "M257-D002 instance-allocation-layout-runtime anchor",
    "runtime layout-consumption evidence for synthesized property access without",
)
RUNTIME_CPP_SNIPPETS = (
    "M257-D002 instance-allocation-layout-runtime anchor",
    "per-instance storage instead of lane-C globals",
    "builtin alloc/new now",
)
PROBE_SNIPPETS = (
    '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    'objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(second_alloc, "setCount:", 9, 0, 0, 0)',
    'objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state)',
    'objc3_runtime_copy_realized_class_entry_for_testing("Widget",',
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m257-d002-instance-allocation-layout-runtime-support": "python scripts/check_m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation.py"',
    '"test:tooling:m257-d002-instance-allocation-layout-runtime-support": "python -m pytest tests/tooling/test_check_m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation.py -q"',
    '"check:objc3c:m257-d002-lane-d-readiness": "python scripts/run_m257_d002_lane_d_readiness.py"',
)


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path).replace("\\", "/")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str,
            failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[str], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for index, snippet in enumerate(snippets, start=1):
        passed += require(
            snippet in text,
            display_path(path),
            f"{MODE}-DOC-{index:02d}",
            f"missing snippet: {snippet}",
            failures,
        )
    return passed


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    if sys.platform == "win32":
        candidate = Path("C:/Program Files/LLVM/bin") / name
        if candidate.exists():
            return str(candidate)
        candidate = Path("C:/Program Files/LLVM/bin") / f"{name}.exe"
        if candidate.exists():
            return str(candidate)
    return None


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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=ROOT / "docs/contracts/m257_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation_d002_expectations.md")
    parser.add_argument("--packet-doc", type=Path, default=ROOT / "spec/planning/compiler/m257/m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation_packet.md")
    parser.add_argument("--native-doc", type=Path, default=ROOT / "docs/objc3c-native.md")
    parser.add_argument("--lowering-spec", type=Path, default=ROOT / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md")
    parser.add_argument("--metadata-spec", type=Path, default=ROOT / "spec/MODULE_METADATA_AND_ABI_TABLES.md")
    parser.add_argument("--architecture-doc", type=Path, default=ROOT / "native/objc3c/src/ARCHITECTURE.md")
    parser.add_argument("--runtime-readme", type=Path, default=ROOT / "native/objc3c/src/runtime/README.md")
    parser.add_argument("--tooling-runtime-readme", type=Path, default=ROOT / "tests/tooling/runtime/README.md")
    parser.add_argument("--lowering-header", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h")
    parser.add_argument("--lowering-cpp", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.cpp")
    parser.add_argument("--ast-header", type=Path, default=ROOT / "native/objc3c/src/ast/objc3_ast.h")
    parser.add_argument("--sema-cpp", type=Path, default=ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp")
    parser.add_argument("--ir-emitter", type=Path, default=ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp")
    parser.add_argument("--runtime-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.h")
    parser.add_argument("--bootstrap-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    parser.add_argument("--runtime-source", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp")
    parser.add_argument("--runtime-probe", type=Path, default=ROOT / RUNTIME_PROBE_SOURCE)
    parser.add_argument("--fixture", type=Path, default=ROOT / FIXTURE_SOURCE)
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument("--native-exe", type=Path, default=ROOT / "artifacts/bin/objc3c-native.exe")
    parser.add_argument("--runtime-library", type=Path, default=ROOT / "artifacts/lib/objc3_runtime.lib")
    parser.add_argument("--runtime-include-root", type=Path, default=ROOT / "native/objc3c/src")
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--probe-root", type=Path, default=ROOT / "tmp/artifacts/compilation/objc3c-native/m257/d002-instance-allocation-runtime")
    parser.add_argument("--summary-out", type=Path, default=ROOT / SUMMARY_RELATIVE_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(list(argv))


def boundary_line(ir_text: str, prefix: str) -> str:
    return next((line for line in ir_text.splitlines() if line.startswith(prefix)), "")


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "runtime_probe", check_id, detail, failures)

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))
    check(first_alloc > 0,
          "M257-D002-DYN-PAY-01", "first alloc must materialize a positive runtime instance identity")
    check(second_alloc > 0,
          "M257-D002-DYN-PAY-02", "second alloc must materialize a positive runtime instance identity")
    check(first_alloc != second_alloc,
          "M257-D002-DYN-PAY-03", "repeated alloc must produce distinct runtime instance identities")
    check(payload.get("set_count_first") == 0,
          "M257-D002-DYN-PAY-04", "first count setter dispatch must return zero")
    check(payload.get("count_value_first") == 37,
          "M257-D002-DYN-PAY-05", "first instance count getter must observe the written value")
    check(payload.get("count_value_second_before") == 0,
          "M257-D002-DYN-PAY-06", "second instance count getter must stay zero before a write")
    check(payload.get("set_enabled_first") == 0,
          "M257-D002-DYN-PAY-07", "first enabled setter dispatch must return zero")
    check(payload.get("enabled_value_first") == 1,
          "M257-D002-DYN-PAY-08", "first instance enabled getter must observe the written bool")
    check(payload.get("enabled_value_second") == 0,
          "M257-D002-DYN-PAY-09", "second instance enabled getter must remain at the default value")
    check(payload.get("set_value_first") == 0,
          "M257-D002-DYN-PAY-10", "first value setter dispatch must return zero")
    check(payload.get("value_result_first") == 55,
          "M257-D002-DYN-PAY-11", "first instance value getter must observe the written scalar alias")
    check(payload.get("value_result_second_before") == 0,
          "M257-D002-DYN-PAY-12", "second instance value getter must remain zero before a write")
    check(payload.get("set_count_second") == 0,
          "M257-D002-DYN-PAY-13", "second count setter dispatch must return zero")
    check(payload.get("count_value_first_after_second") == 37,
          "M257-D002-DYN-PAY-14", "first instance count must remain stable after writing the second instance")
    check(payload.get("count_value_second_after") == 9,
          "M257-D002-DYN-PAY-15", "second instance count must observe its own write")
    check(payload.get("set_value_second") == 0,
          "M257-D002-DYN-PAY-16", "second value setter dispatch must return zero")
    check(payload.get("value_result_first_after_second") == 55,
          "M257-D002-DYN-PAY-17", "first instance value must remain stable after writing the second instance")
    check(payload.get("value_result_second_after") == 91,
          "M257-D002-DYN-PAY-18", "second instance value must observe its own write")

    registration_state = payload.get("registration_state", {})
    check(registration_state.get("registered_image_count", 0) >= 1,
          "M257-D002-DYN-PAY-19", "runtime must report at least one registered image")
    check(registration_state.get("registered_descriptor_total", 0) >= 1,
          "M257-D002-DYN-PAY-20", "runtime must report a non-zero descriptor total")

    selector_state = payload.get("selector_table_state", {})
    check(selector_state.get("selector_table_entry_count", 0) >= 6,
          "M257-D002-DYN-PAY-21", "selector table must materialize the synthesized accessor surface")

    graph_state = payload.get("graph_state", {})
    check(graph_state.get("live_instance_count") == 2,
          "M257-D002-DYN-PAY-22", "runtime graph state must publish two live instances after two alloc calls")
    check(graph_state.get("last_allocated_receiver_identity") == second_alloc,
          "M257-D002-DYN-PAY-23", "runtime graph state must publish the second allocation as the latest receiver")
    check(graph_state.get("last_allocated_base_identity") == EXPECTED_BASE_IDENTITY,
          "M257-D002-DYN-PAY-24", "runtime graph state must preserve the Widget base receiver identity")
    check(graph_state.get("last_allocated_instance_size_bytes") == EXPECTED_INSTANCE_SIZE,
          "M257-D002-DYN-PAY-25", "runtime graph state must publish the realized Widget instance size")
    check(graph_state.get("last_allocated_class_name") == "Widget",
          "M257-D002-DYN-PAY-26", "runtime graph state must publish the last allocated class name")

    widget_entry = payload.get("widget_entry", {})
    check(widget_entry.get("found") == 1,
          "M257-D002-DYN-PAY-27", "realized Widget entry must be queryable")
    check(widget_entry.get("base_identity") == EXPECTED_BASE_IDENTITY,
          "M257-D002-DYN-PAY-28", "realized Widget entry must preserve the base receiver identity")
    check(widget_entry.get("runtime_property_accessor_count") == 3,
          "M257-D002-DYN-PAY-29", "realized Widget entry must publish all synthesized runtime property accessors")
    check(widget_entry.get("runtime_instance_size_bytes") == EXPECTED_INSTANCE_SIZE,
          "M257-D002-DYN-PAY-30", "realized Widget entry must publish the realized instance size")

    count_entry = payload.get("count_entry", {})
    check(count_entry.get("found") == 1 and count_entry.get("resolved") == 1,
          "M257-D002-DYN-PAY-31", "count getter cache entry must resolve")
    check(count_entry.get("parameter_count") == 0,
          "M257-D002-DYN-PAY-32", "count getter cache entry must preserve zero parameters")
    check(count_entry.get("normalized_receiver_identity") == EXPECTED_BASE_IDENTITY + 1,
          "M257-D002-DYN-PAY-33", "count getter cache entry must normalize dynamic instance receivers onto the realized dispatch slot")
    check(str(count_entry.get("resolved_owner_identity", "")).endswith(EXPECTED_COUNT_OWNER),
          "M257-D002-DYN-PAY-34", "count getter cache entry must preserve the synthesized owner identity")

    set_count_entry = payload.get("set_count_entry", {})
    check(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1,
          "M257-D002-DYN-PAY-35", "setCount setter cache entry must resolve")
    check(set_count_entry.get("parameter_count") == 1,
          "M257-D002-DYN-PAY-36", "setCount setter cache entry must preserve one parameter")
    check(str(set_count_entry.get("resolved_owner_identity", "")).endswith(EXPECTED_SET_COUNT_OWNER),
          "M257-D002-DYN-PAY-37", "setCount setter cache entry must preserve the synthesized owner identity")

    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M257-D002-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M257-D002-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M257-D002-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M257-D002-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M257-D002-DYN-05", f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    probe_dir = args.probe_root.resolve() / f"probe-{uuid.uuid4().hex}"
    probe_dir.mkdir(parents=True, exist_ok=True)

    compile_result = run_command(
        [
            str(args.native_exe),
            str(args.fixture),
            "--out-dir",
            str(probe_dir),
            "--emit-prefix",
            "module",
        ],
        ROOT,
    )
    check(compile_result.returncode == 0, "M257-D002-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    module_manifest = probe_dir / "module.manifest.json"
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    module_backend = probe_dir / "module.object-backend.txt"
    check(module_manifest.exists(), "M257-D002-DYN-07", f"missing manifest: {display_path(module_manifest)}")
    check(module_ir.exists(), "M257-D002-DYN-08", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M257-D002-DYN-09", f"missing emitted object: {display_path(module_obj)}")
    check(module_backend.exists(), "M257-D002-DYN-10", f"missing backend marker: {display_path(module_backend)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    ir_text = read_text(module_ir)
    synthesized_line = boundary_line(ir_text, SYNTHESIZED_PREFIX)
    boundary = boundary_line(ir_text, BOUNDARY_PREFIX)
    check(bool(synthesized_line), "M257-D002-DYN-11", "IR must publish the C003 synthesized accessor summary")
    check(bool(boundary), "M257-D002-DYN-12", "IR must publish the D002 runtime instance-allocation/layout summary")
    check("synthesized_accessor_entries=6" in synthesized_line, "M257-D002-DYN-13", "C003 summary must retain the expected synthesized accessor count")
    check("property_descriptor_entries=" in boundary, "M257-D002-DYN-14", "D002 summary must publish property descriptor inventory")
    check("ivar_layout_owner_entries=" in boundary, "M257-D002-DYN-15", "D002 summary must publish ivar layout owner inventory")
    check("synthesized_accessor_entries=6" in boundary, "M257-D002-DYN-16", "D002 summary must preserve the expected synthesized accessor count")

    probe_exe = probe_dir / "m257_d002_instance_allocation_runtime_probe.exe"
    probe_compile = run_command(
        [
            str(clangxx),
            "-std=c++20",
            "-I",
            str(args.runtime_include_root),
            str(args.runtime_probe),
            str(module_obj),
            str(args.runtime_library),
            "-o",
            str(probe_exe),
        ],
        ROOT,
    )
    check(probe_compile.returncode == 0, "M257-D002-DYN-17", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M257-D002-DYN-18", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M257-D002-DYN-19", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = validate_probe_payload(payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_manifest": display_path(module_manifest),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "boundary": boundary,
        "synthesized_line": synthesized_line,
        "payload": payload,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_SNIPPETS),
        (args.runtime_header, RUNTIME_HEADER_SNIPPETS),
        (args.bootstrap_header, BOOTSTRAP_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_CPP_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        passed, total, dynamic_payload = run_dynamic_case(args, failures)
        checks_passed += passed
        checks_total += total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "descriptor_model": DESCRIPTOR_MODEL,
        "allocator_model": ALLOCATOR_MODEL,
        "storage_model": STORAGE_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "dynamic_probe": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)")
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}")
        print(f"[info] summary: {display_path(args.summary_out)}")
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())







