#!/usr/bin/env python3
"""Validate M260-D001 runtime memory-management API freeze."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-d001-runtime-memory-management-api-freeze-v1"
CONTRACT_ID = "objc3c-runtime-memory-management-api-freeze/m260-d001-v1"
REFERENCE_MODEL = (
    "public-runtime-abi-stays-register-lookup-dispatch-while-reference-counting-helpers-remain-private-runtime-entrypoints"
)
WEAK_MODEL = (
    "weak-storage-remains-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables"
)
AUTORELEASEPOOL_MODEL = (
    "no-public-autoreleasepool-push-pop-api-yet-autorelease-helper-drains-only-on-dispatch-frame-return"
)
FAIL_CLOSED_MODEL = (
    "no-public-memory-management-header-widening-no-user-facing-arc-entrypoints-yet"
)
BOUNDARY_PREFIX = "; runtime_memory_management_api = "
NAMED_METADATA_PREFIX = "!objc3.objc_runtime_memory_management_api = !{!70}"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-D001" / "runtime_memory_management_api_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_runtime_memory_management_api_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_d001_runtime_memory_management_api_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_ownership_runtime_hook_emission_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m260_d001_runtime_memory_management_api_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "d001"
CLANGXX = "clang++"
HELPER_SYMBOLS = (
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_read_current_property_i32",
    "objc3_runtime_write_current_property_i32",
    "objc3_runtime_exchange_current_property_i32",
    "objc3_runtime_load_weak_current_property_i32",
    "objc3_runtime_store_weak_current_property_i32",
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


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M260-D001-DOC-01", "# M260 Runtime Memory Management API Contract And Architecture Freeze Expectations (D001)"),
    SnippetCheck("M260-D001-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M260-D001-DOC-03", "`tests/tooling/runtime/m260_d001_runtime_memory_management_api_probe.cpp`"),
    SnippetCheck("M260-D001-DOC-04", "`tmp/reports/m260/M260-D001/runtime_memory_management_api_contract_summary.json`"),
    SnippetCheck("M260-D001-DOC-05", "no public autoreleasepool push/pop runtime API lands here"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M260-D001-PKT-01", "# M260-D001 Runtime Memory Management API Contract And Architecture Freeze Packet"),
    SnippetCheck("M260-D001-PKT-02", "Issue: `#7175`"),
    SnippetCheck("M260-D001-PKT-03", "Packet: `M260-D001`"),
    SnippetCheck("M260-D001-PKT-04", "`tests/tooling/runtime/m260_d001_runtime_memory_management_api_probe.cpp`"),
    SnippetCheck("M260-D001-PKT-05", "`M260-D002`"),
)
DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M260-D001-SRC-01", "## M260 runtime memory-management API freeze (D001)"),
    SnippetCheck("M260-D001-SRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D001-SRC-03", "`objc3_runtime_bootstrap_internal.h`"),
    SnippetCheck("M260-D001-SRC-04", "`check:objc3c:m260-d001-lane-d-readiness`"),
    SnippetCheck("M260-D001-SRC-05", "`M260-D002` is the next issue"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M260-D001-NDOC-01", "## M260 runtime memory-management API freeze (D001)"),
    SnippetCheck("M260-D001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D001-NDOC-03", "`objc3_runtime_autorelease_i32` remains a private helper"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M260-D001-ARCH-01", "## M260 Runtime Memory-Management API Freeze (D001)"),
    SnippetCheck("M260-D001-ARCH-02", "retain/release/autorelease/current-property/weak helper entrypoints remain"),
    SnippetCheck("M260-D001-ARCH-03", "the next issue is `M260-D002`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M260-D001-SPC-01", "## M260 runtime memory-management API freeze (D001)"),
    SnippetCheck("M260-D001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D001-SPC-03", f"`{REFERENCE_MODEL}`"),
    SnippetCheck("M260-D001-SPC-04", f"`{AUTORELEASEPOOL_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M260-D001-META-01", "## M260 runtime memory-management API metadata anchors (D001)"),
    SnippetCheck("M260-D001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D001-META-03", "`!objc3.objc_runtime_memory_management_api`"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M260-D001-SEMA-01", "M260-D001 runtime memory-management API freeze anchor"),
)
IR_SNIPPETS = (
    SnippetCheck("M260-D001-IR-01", 'out << "; runtime_memory_management_api = "'),
    SnippetCheck("M260-D001-IR-02", '!objc3.objc_runtime_memory_management_api = !{!70}'),
    SnippetCheck("M260-D001-IR-03", "Objc3RuntimeMemoryManagementApiSummary()"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M260-D001-LHDR-01", "kObjc3RuntimeMemoryManagementApiContractId"),
    SnippetCheck("M260-D001-LHDR-02", "kObjc3RuntimeMemoryManagementApiReferenceModel"),
    SnippetCheck("M260-D001-LHDR-03", "kObjc3RuntimeMemoryManagementApiFailClosedModel"),
    SnippetCheck("M260-D001-LHDR-04", "Objc3RuntimeMemoryManagementApiSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M260-D001-LCPP-01", "Objc3RuntimeMemoryManagementApiSummary()"),
    SnippetCheck("M260-D001-LCPP-02", "M260-D001 runtime memory-management API freeze anchor"),
    SnippetCheck("M260-D001-LCPP-03", "reference_model="),
    SnippetCheck("M260-D001-LCPP-04", "autoreleasepool_model="),
)
RUNTIME_HEADER_SNIPPETS = (
    SnippetCheck("M260-D001-RTH-01", "M260-D001 runtime-memory-management-api anchor"),
    SnippetCheck("M260-D001-RTH-02", "int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,"),
)
RUNTIME_INTERNAL_HEADER_SNIPPETS = (
    SnippetCheck("M260-D001-RIH-01", "M260-D001 runtime-memory-management-api anchor"),
    SnippetCheck("M260-D001-RIH-02", "int objc3_runtime_retain_i32(int value);"),
    SnippetCheck("M260-D001-RIH-03", "int objc3_runtime_autorelease_i32(int value);"),
)
RUNTIME_CPP_SNIPPETS = (
    SnippetCheck("M260-D001-RT-01", "M260-D001 runtime memory-management API freeze anchor"),
    SnippetCheck("M260-D001-RT-02", "objc3_runtime_autorelease_i32"),
    SnippetCheck("M260-D001-RT-03", "objc3_runtime_store_weak_current_property_i32"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M260-D001-PKG-01", '"check:objc3c:m260-d001-runtime-memory-management-api-contract": "python scripts/check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py"'),
    SnippetCheck("M260-D001-PKG-02", '"test:tooling:m260-d001-runtime-memory-management-api-contract": "python -m pytest tests/tooling/test_check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M260-D001-PKG-03", '"check:objc3c:m260-d001-lane-d-readiness": "python scripts/run_m260_d001_lane_d_readiness.py"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


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


def extract_count(boundary_line: str, token: str) -> int:
    import re
    match = re.search(rf"(?:^|;){token}=(\d+)", boundary_line)
    return int(match.group(1)) if match else -1


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=NATIVE_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--sema-cpp", type=Path, default=SEMA_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=IR_EMITTER)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=LOWERING_CPP)
    parser.add_argument("--runtime-header", type=Path, default=RUNTIME_HEADER)
    parser.add_argument("--runtime-internal-header", type=Path, default=RUNTIME_INTERNAL_HEADER)
    parser.add_argument("--runtime-cpp", type=Path, default=RUNTIME_CPP)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--fixture", type=Path, default=FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=RUNTIME_PROBE)
    parser.add_argument("--probe-root", type=Path, default=PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--clangxx", default=CLANGXX)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def validate_static_public_private_boundary(
    runtime_header_text: str,
    runtime_internal_text: str,
    failures: list[Finding],
) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    for symbol in HELPER_SYMBOLS:
        checks_total += 1
        checks_passed += require(
            symbol not in runtime_header_text,
            display_path(RUNTIME_HEADER),
            f"M260-D001-PUB-{checks_total:02d}",
            f"public runtime header must not expose helper symbol {symbol}",
            failures,
        )
    for symbol in HELPER_SYMBOLS:
        checks_total += 1
        checks_passed += require(
            symbol in runtime_internal_text,
            display_path(RUNTIME_INTERNAL_HEADER),
            f"M260-D001-PRI-{checks_total:02d}",
            f"private runtime header must expose helper symbol {symbol}",
            failures,
        )
    return checks_passed, checks_total


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "probe_payload", check_id, detail, failures)

    parent = int(payload.get("parent", 0))
    child = int(payload.get("child", 0))
    check(parent > 0, "M260-D001-PROBE-01", "parent allocation must produce a positive runtime object identity")
    check(child > 0, "M260-D001-PROBE-02", "child allocation must produce a positive runtime object identity")
    check(payload.get("strong_set_result") == 0, "M260-D001-PROBE-03", "strong setter must return zero")
    check(payload.get("weak_set_result") == 0, "M260-D001-PROBE-04", "weak setter must return zero")
    check(payload.get("retain_result") == child, "M260-D001-PROBE-05", "retain helper must preserve the child identity")
    check(payload.get("autorelease_result") == child, "M260-D001-PROBE-06", "autorelease helper must preserve the child identity")
    check(payload.get("release_after_helper_result") == child, "M260-D001-PROBE-07", "release helper must preserve the child identity while the strong edge remains alive")
    check(payload.get("release_local_result") == child, "M260-D001-PROBE-08", "dropping the local child reference must leave the strong edge alive")
    check(payload.get("strong_before_clear") == child, "M260-D001-PROBE-09", "strong getter must observe the child before clear")
    check(payload.get("weak_before_clear") == child, "M260-D001-PROBE-10", "weak getter must observe the child before clear")
    check(payload.get("clear_strong_result") == 0, "M260-D001-PROBE-11", "clearing the strong property must return zero")
    check(payload.get("strong_after_clear") == 0, "M260-D001-PROBE-12", "strong getter must observe zero after clear")
    check(payload.get("weak_after_clear") == 0, "M260-D001-PROBE-13", "weak getter must zero after the child is destroyed")
    check(payload.get("parent_release_result") == parent, "M260-D001-PROBE-14", "parent release must preserve the released parent identity")

    registration_state = payload.get("registration_state", {})
    check(registration_state.get("registered_image_count", 0) >= 1, "M260-D001-PROBE-15", "runtime must report at least one registered image")
    check(registration_state.get("registered_descriptor_total", 0) >= 1, "M260-D001-PROBE-16", "runtime must report a non-zero descriptor total")

    graph_after_alloc = payload.get("graph_after_alloc", {})
    graph_after_helper_release = payload.get("graph_after_helper_release", {})
    graph_after_clear = payload.get("graph_after_clear", {})
    graph_after_parent_release = payload.get("graph_after_parent_release", {})
    check(graph_after_alloc.get("live_instance_count") == 2, "M260-D001-PROBE-17", "alloc phase must show two live instances")
    check(graph_after_helper_release.get("live_instance_count") == 2, "M260-D001-PROBE-18", "helper activity must keep the child alive while the strong edge exists")
    check(graph_after_clear.get("live_instance_count") == 1, "M260-D001-PROBE-19", "clearing the strong edge must destroy the child")
    check(graph_after_parent_release.get("live_instance_count") == 0, "M260-D001-PROBE-20", "releasing the parent must leave zero live instances")

    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M260-D001-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M260-D001-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M260-D001-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M260-D001-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M260-D001-DYN-05", f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    positive_dir = args.probe_root / "positive"
    positive_dir.mkdir(parents=True, exist_ok=True)
    compile_command = [
        str(args.native_exe),
        str(args.fixture),
        "--out-dir",
        str(positive_dir),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_command, ROOT)
    module_ir = positive_dir / "module.ll"
    module_obj = positive_dir / "module.obj"
    check(compile_result.returncode == 0, "M260-D001-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    check(module_ir.exists(), "M260-D001-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M260-D001-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {"skipped": False, "compile_stdout": compile_result.stdout, "compile_stderr": compile_result.stderr}

    ir_text = read_text(module_ir)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_PREFIX + "contract=")), "")
    check(bool(boundary_line), "M260-D001-DYN-09", "IR must publish the runtime memory-management API boundary line")
    check(NAMED_METADATA_PREFIX in ir_text, "M260-D001-DYN-10", "IR must publish !objc3.objc_runtime_memory_management_api")
    check(f"contract={CONTRACT_ID}" in boundary_line, "M260-D001-DYN-11", "boundary line must carry the runtime memory-management contract id")
    check(f"reference_model={REFERENCE_MODEL}" in boundary_line, "M260-D001-DYN-12", "boundary line must carry the frozen reference model")
    check(f"weak_model={WEAK_MODEL}" in boundary_line, "M260-D001-DYN-13", "boundary line must carry the frozen weak model")
    check(f"autoreleasepool_model={AUTORELEASEPOOL_MODEL}" in boundary_line, "M260-D001-DYN-14", "boundary line must carry the frozen autoreleasepool model")
    check(f"fail_closed_model={FAIL_CLOSED_MODEL}" in boundary_line, "M260-D001-DYN-15", "boundary line must carry the fail-closed model")
    check(extract_count(boundary_line, "synthesized_accessor_entries") == 4, "M260-D001-DYN-16", "positive fixture must still emit exactly four synthesized accessors")
    check(extract_count(boundary_line, "synthesized_storage_globals") == 2, "M260-D001-DYN-17", "positive fixture must still emit exactly two legacy synthesized storage globals")
    for symbol in HELPER_SYMBOLS:
        checks_total += 1
        checks_passed += require(symbol in ir_text, display_path(module_ir), f"M260-D001-DYN-SYM-{checks_total:02d}", f"IR must retain helper symbol {symbol}", failures)

    probe_exe = positive_dir / "m260_d001_runtime_memory_management_api_probe.exe"
    probe_compile_command = [
        str(clangxx),
        "-std=c++20",
        "-I",
        str(args.runtime_include_root),
        str(args.runtime_probe),
        str(module_obj),
        str(args.runtime_library),
        "-o",
        str(probe_exe),
    ]
    probe_compile = run_command(probe_compile_command, ROOT)
    check(probe_compile.returncode == 0, "M260-D001-DYN-18", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_compile_stdout": probe_compile.stdout, "probe_compile_stderr": probe_compile.stderr}

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M260-D001-DYN-19", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_stdout": probe_run.stdout, "probe_stderr": probe_run.stderr}

    try:
        payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M260-D001-DYN-20", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = validate_probe_payload(payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "compile_command": compile_command,
        "probe_compile_command": probe_compile_command,
        "positive_dir": display_path(positive_dir),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "boundary_line": boundary_line,
        "probe_payload": payload,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.doc_source, DOC_SOURCE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.runtime_header, RUNTIME_HEADER_SNIPPETS),
        (args.runtime_internal_header, RUNTIME_INTERNAL_HEADER_SNIPPETS),
        (args.runtime_cpp, RUNTIME_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    runtime_header_text = read_text(args.runtime_header)
    runtime_internal_text = read_text(args.runtime_internal_header)
    passed, total = validate_static_public_private_boundary(runtime_header_text, runtime_internal_text, failures)
    checks_passed += passed
    checks_total += total

    if args.skip_dynamic_probes:
        dynamic_case: dict[str, Any] = {"skipped": True}
    else:
        passed, total, dynamic_case = run_dynamic_case(args, failures)
        checks_passed += passed
        checks_total += total

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_case,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}", file=sys.stderr)
        print(f"[info] summary: {display_path(summary_path)}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
