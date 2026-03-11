"""Validate M260-D002 reference counting, weak table, and autoreleasepool implementation."""

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
MODE = "m260-d002-runtime-memory-management-implementation-v1"
CONTRACT_ID = "objc3c-runtime-memory-management-implementation/m260-d002-v1"
REFCOUNT_MODEL = (
    "runtime-managed-instance-retain-counts-destroy-strong-owned-storage-on-final-release"
)
WEAK_MODEL = (
    "weak-side-table-tracks-runtime-storage-observers-and-zeroes-them-on-final-release"
)
AUTORELEASEPOOL_MODEL = (
    "private-autoreleasepool-push-pop-scopes-retain-autoreleased-runtime-values-until-lifo-drain"
)
FAIL_CLOSED_MODEL = (
    "memory-management-runtime-support-remains-private-lowered-and-runtime-probe-driven"
)
BOUNDARY_PREFIX = "; runtime_memory_management_implementation = "
NAMED_METADATA_PREFIX = "!objc3.objc_runtime_memory_management_implementation = !{!71}"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-D002" / "reference_counting_weak_autoreleasepool_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation_packet.md"
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
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_d002_reference_counting_weak_autoreleasepool_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m260_d002_reference_counting_weak_autoreleasepool_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "d002"
CLANGXX = "clang++"
PRIVATE_SYMBOLS = (
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
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
    SnippetCheck("M260-D002-DOC-01", "# M260 Reference Counting Weak Table And Autoreleasepool Implementation Core Feature Implementation Expectations (D002)"),
    SnippetCheck("M260-D002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M260-D002-DOC-03", "`tests/tooling/runtime/m260_d002_reference_counting_weak_autoreleasepool_probe.cpp`"),
    SnippetCheck("M260-D002-DOC-04", "`@autoreleasepool`") ,
    SnippetCheck("M260-D002-DOC-05", "`tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M260-D002-PKT-01", "# M260-D002 Reference Counting Weak Table And Autoreleasepool Implementation Core Feature Implementation Packet"),
    SnippetCheck("M260-D002-PKT-02", "Issue: `#7176`"),
    SnippetCheck("M260-D002-PKT-03", "Packet: `M260-D002`"),
    SnippetCheck("M260-D002-PKT-04", "`tests/tooling/fixtures/native/m260_d002_reference_counting_weak_autoreleasepool_positive.objc3`"),
    SnippetCheck("M260-D002-PKT-05", "`M260-E001`"),
)
DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M260-D002-SRC-01", "## M260 reference counting, weak table, and autoreleasepool implementation (D002)"),
    SnippetCheck("M260-D002-SRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D002-SRC-03", "`objc3_runtime_push_autoreleasepool_scope`") ,
    SnippetCheck("M260-D002-SRC-04", "`check:objc3c:m260-d002-lane-d-readiness`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M260-D002-NDOC-01", "## M260 reference counting, weak table, and autoreleasepool implementation (D002)"),
    SnippetCheck("M260-D002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D002-NDOC-03", "`objc3_runtime_pop_autoreleasepool_scope`") ,
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M260-D002-ARCH-01", "## M260 Reference Counting Weak Table And Autoreleasepool Implementation (D002)"),
    SnippetCheck("M260-D002-ARCH-02", "autoreleased runtime values now drain on pool pop"),
    SnippetCheck("M260-D002-ARCH-03", "the next issue is `M260-E001`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M260-D002-SPC-01", "## M260 reference counting, weak table, and autoreleasepool implementation (D002)"),
    SnippetCheck("M260-D002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D002-SPC-03", f"`{REFCOUNT_MODEL}`"),
    SnippetCheck("M260-D002-SPC-04", f"`{AUTORELEASEPOOL_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M260-D002-META-01", "## M260 reference counting, weak table, and autoreleasepool metadata anchors (D002)"),
    SnippetCheck("M260-D002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-D002-META-03", "`!objc3.objc_runtime_memory_management_implementation`"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M260-D002-SEMA-01", "M260-D002 runtime-memory-management implementation anchor:"),
    SnippetCheck("M260-D002-SEMA-02", "now accepts `@autoreleasepool` blocks"),
)
IR_SNIPPETS = (
    SnippetCheck("M260-D002-IR-01", "M260-D002 runtime memory-management implementation anchor:"),
    SnippetCheck("M260-D002-IR-02", "runtime_memory_management_implementation"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M260-D002-LOWH-01", "kObjc3RuntimeMemoryManagementImplementationContractId"),
    SnippetCheck("M260-D002-LOWH-02", "kObjc3RuntimePushAutoreleasepoolScopeSymbol"),
    SnippetCheck("M260-D002-LOWH-03", "kObjc3RuntimePopAutoreleasepoolScopeSymbol"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M260-D002-LOWC-01", "Objc3RuntimeMemoryManagementImplementationSummary"),
    SnippetCheck("M260-D002-LOWC-02", "M260-D002 runtime memory-management implementation anchor:"),
)
RUNTIME_INTERNAL_SNIPPETS = (
    SnippetCheck("M260-D002-RIN-01", "objc3_runtime_push_autoreleasepool_scope"),
    SnippetCheck("M260-D002-RIN-02", "objc3_runtime_copy_memory_management_state_for_testing"),
)
RUNTIME_CPP_SNIPPETS = (
    SnippetCheck("M260-D002-RCP-01", "M260-D002 runtime-memory-management implementation anchor:"),
    SnippetCheck("M260-D002-RCP-02", "g_runtime_autoreleasepool_frames"),
    SnippetCheck("M260-D002-RCP-03", "objc3_runtime_pop_autoreleasepool_scope"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M260-D002-PKG-01", '"check:objc3c:m260-d002-runtime-memory-management-implementation":'),
    SnippetCheck("M260-D002-PKG-02", '"test:tooling:m260-d002-runtime-memory-management-implementation":'),
    SnippetCheck("M260-D002-PKG-03", '"check:objc3c:m260-d002-lane-d-readiness":'),
)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def display_path(path: Path) -> str:
    return path.resolve().relative_to(ROOT).as_posix()


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if not condition:
        failures.append(Finding(artifact, check_id, detail))
        return 0
    return 1


def resolve_tool(name: str) -> str | None:
    return shutil.which(name) or shutil.which(name + ".exe")


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


def boundary_line(ir_text: str, prefix: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(prefix + "contract="):
            return line
    return ""


def check_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> tuple[int, int]:
    text = read_text(path)
    total = 0
    passed = 0
    artifact = display_path(path)
    for snippet in snippets:
        total += 1
        passed += require(snippet.snippet in text, artifact, snippet.check_id, f"missing snippet: {snippet.snippet}", failures)
    return passed, total


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


def validate_public_private_boundary(runtime_header_text: str, runtime_internal_text: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for symbol in PRIVATE_SYMBOLS:
        total += 1
        passed += require(
            symbol not in runtime_header_text,
            display_path(RUNTIME_HEADER),
            f"M260-D002-PUB-{total:02d}",
            f"public runtime header must not expose private helper symbol {symbol}",
            failures,
        )
    for symbol in PRIVATE_SYMBOLS:
        total += 1
        passed += require(
            symbol in runtime_internal_text,
            display_path(RUNTIME_INTERNAL_HEADER),
            f"M260-D002-PRI-{total:02d}",
            f"private runtime header must expose helper symbol {symbol}",
            failures,
        )
    return passed, total


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal total, passed
        total += 1
        passed += require(condition, "probe_payload", check_id, detail, failures)

    parent = int(payload.get("parent", 0))
    child = int(payload.get("child", 0))
    check(parent > 0, "M260-D002-PROBE-01", "parent allocation must produce a positive runtime identity")
    check(child > 0, "M260-D002-PROBE-02", "child allocation must produce a positive runtime identity")
    check(payload.get("strong_set_result") == 0, "M260-D002-PROBE-03", "strong setter must return zero")
    check(payload.get("release_local_result") == child, "M260-D002-PROBE-04", "releasing the local child must preserve the identity while the strong edge exists")
    check(payload.get("getter_value") == child, "M260-D002-PROBE-05", "getter must return the child identity inside the pool")
    check(payload.get("weak_set_result") == 0, "M260-D002-PROBE-06", "weak setter must return zero")
    check(payload.get("clear_strong_result") == 0, "M260-D002-PROBE-07", "clearing the strong property must return zero")
    check(payload.get("weak_inside_pool") == child, "M260-D002-PROBE-08", "weak slot must still observe the child while the pool retains it")
    check(payload.get("weak_after_pool") == 0, "M260-D002-PROBE-09", "weak slot must zero after the pool drains the last retain")
    check(payload.get("parent_release_result") == parent, "M260-D002-PROBE-10", "parent release must preserve the released parent identity")

    graph_after_setup = payload.get("graph_after_setup", {})
    graph_inside_pool = payload.get("graph_inside_pool", {})
    graph_after_pool = payload.get("graph_after_pool", {})
    graph_after_parent_release = payload.get("graph_after_parent_release", {})
    check(graph_after_setup.get("live_instance_count") == 2, "M260-D002-PROBE-11", "setup must leave parent and child alive")
    check(graph_inside_pool.get("live_instance_count") == 2, "M260-D002-PROBE-12", "pool scope must keep the child alive after clearing the strong edge")
    check(graph_after_pool.get("live_instance_count") == 1, "M260-D002-PROBE-13", "pool drain must destroy the child")
    check(graph_after_parent_release.get("live_instance_count") == 0, "M260-D002-PROBE-14", "releasing the parent must clear all live instances")

    memory_inside_pool = payload.get("memory_inside_pool", {})
    memory_after_pool = payload.get("memory_after_pool", {})
    memory_after_parent_release = payload.get("memory_after_parent_release", {})
    check(memory_inside_pool.get("autoreleasepool_depth") == 1, "M260-D002-PROBE-15", "active pool depth must be one inside the explicit scope")
    check(memory_inside_pool.get("autoreleasepool_max_depth", 0) >= 1, "M260-D002-PROBE-16", "max pool depth must record at least one pushed scope")
    check(memory_inside_pool.get("queued_autorelease_value_count") == 1, "M260-D002-PROBE-17", "pool must hold one queued autoreleased child value")
    check(memory_inside_pool.get("weak_target_count") == 1, "M260-D002-PROBE-18", "weak table must track the child while it is still alive")
    check(memory_inside_pool.get("weak_slot_ref_count") == 1, "M260-D002-PROBE-19", "weak table must hold one tracked slot for the child")
    check(memory_inside_pool.get("last_autoreleased_value") == child, "M260-D002-PROBE-20", "pool state must publish the queued child identity")
    check(memory_after_pool.get("autoreleasepool_depth") == 0, "M260-D002-PROBE-21", "pool depth must return to zero after pop")
    check(memory_after_pool.get("queued_autorelease_value_count") == 0, "M260-D002-PROBE-22", "no queued autorelease values may remain after pop")
    check(memory_after_pool.get("drained_autorelease_value_count") == 1, "M260-D002-PROBE-23", "one autoreleased value must drain on pop")
    check(memory_after_pool.get("last_drained_autorelease_value") == child, "M260-D002-PROBE-24", "drain state must publish the child identity")
    check(memory_after_pool.get("weak_target_count") == 0, "M260-D002-PROBE-25", "weak table must be empty after the child is destroyed")
    check(memory_after_parent_release.get("live_runtime_instance_count") == 0, "M260-D002-PROBE-26", "memory state must report zero live instances after parent release")

    weak_value_entry = payload.get("weak_value_entry", {})
    check(weak_value_entry.get("found") == 1, "M260-D002-PROBE-27", "weak property entry must remain queryable")
    check(weak_value_entry.get("has_runtime_getter") == 1, "M260-D002-PROBE-28", "weak property must preserve its runtime getter")
    check(weak_value_entry.get("has_runtime_setter") == 1, "M260-D002-PROBE-29", "weak property must preserve its runtime setter")
    check(weak_value_entry.get("ownership_runtime_hook_profile") is not None, "M260-D002-PROBE-30", "weak property entry must publish an ownership runtime hook profile")

    return passed, total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal total, passed
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M260-D002-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M260-D002-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M260-D002-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M260-D002-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M260-D002-DYN-05", f"unable to resolve {args.clangxx}")
    if failures:
        return passed, total, {"skipped": False}

    positive_dir = args.probe_root / "positive"
    positive_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command(
        [
            str(args.native_exe),
            str(args.fixture),
            "--out-dir",
            str(positive_dir),
            "--emit-prefix",
            "module",
        ],
        ROOT,
    )
    module_ir = positive_dir / "module.ll"
    module_obj = positive_dir / "module.obj"
    check(compile_result.returncode == 0, "M260-D002-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    check(module_ir.exists(), "M260-D002-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M260-D002-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return passed, total, {"skipped": False, "compile_stdout": compile_result.stdout, "compile_stderr": compile_result.stderr}

    ir_text = read_text(module_ir)
    boundary = boundary_line(ir_text, BOUNDARY_PREFIX)
    check(bool(boundary), "M260-D002-DYN-09", "IR must publish the D002 runtime memory-management implementation boundary line")
    check(NAMED_METADATA_PREFIX in ir_text, "M260-D002-DYN-10", "IR must publish !objc3.objc_runtime_memory_management_implementation")
    check(f"contract={CONTRACT_ID}" in boundary, "M260-D002-DYN-11", "boundary line must publish the contract id")
    check(f"refcount_model={REFCOUNT_MODEL}" in boundary, "M260-D002-DYN-12", "boundary line must publish the refcount model")
    check(f"weak_model={WEAK_MODEL}" in boundary, "M260-D002-DYN-13", "boundary line must publish the weak model")
    check(f"autoreleasepool_model={AUTORELEASEPOOL_MODEL}" in boundary, "M260-D002-DYN-14", "boundary line must publish the autoreleasepool model")
    check(f"fail_closed_model={FAIL_CLOSED_MODEL}" in boundary, "M260-D002-DYN-15", "boundary line must publish the fail-closed model")
    push_calls = len(re.findall(r"call void @objc3_runtime_push_autoreleasepool_scope\(\)", ir_text))
    pop_calls = len(re.findall(r"call void @objc3_runtime_pop_autoreleasepool_scope\(\)", ir_text))
    check(push_calls == 2, "M260-D002-DYN-16", "fixture must lower two autoreleasepool push calls")
    check(pop_calls == 2, "M260-D002-DYN-17", "fixture must lower two autoreleasepool pop calls")
    check("unsupported feature claim: '@autoreleasepool'" not in compile_result.stdout + compile_result.stderr, "M260-D002-DYN-18", "native compile must no longer fail-close @autoreleasepool")

    probe_exe = positive_dir / "m260_d002_reference_counting_weak_autoreleasepool_probe.exe"
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
    check(probe_compile.returncode == 0, "M260-D002-DYN-19", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return passed, total, {"skipped": False, "probe_compile_stdout": probe_compile.stdout, "probe_compile_stderr": probe_compile.stderr}

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M260-D002-DYN-20", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return passed, total, {"skipped": False, "probe_stdout": probe_run.stdout, "probe_stderr": probe_run.stderr}

    try:
        payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M260-D002-DYN-21", f"probe output is not valid JSON: {exc}"))
        return passed, total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = validate_probe_payload(payload, failures)
    passed += payload_passed
    total += payload_total
    return passed, total, {
        "skipped": False,
        "positive_dir": display_path(positive_dir),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "boundary": boundary,
        "push_calls": push_calls,
        "pop_calls": pop_calls,
        "probe_payload": payload,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0

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
        (args.runtime_internal_header, RUNTIME_INTERNAL_SNIPPETS),
        (args.runtime_cpp, RUNTIME_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        chunk_passed, chunk_total = check_snippets(path, snippets, failures)
        passed += chunk_passed
        total += chunk_total

    runtime_header_text = read_text(args.runtime_header)
    runtime_internal_text = read_text(args.runtime_internal_header)
    boundary_passed, boundary_total = validate_public_private_boundary(runtime_header_text, runtime_internal_text, failures)
    passed += boundary_passed
    total += boundary_total

    dynamic_result: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_passed, dynamic_total, dynamic_result = run_dynamic_case(args, failures)
        passed += dynamic_passed
        total += dynamic_total

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_passed": passed,
        "checks_total": total,
        "dynamic": dynamic_result,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}")
        print(f"[fail] {MODE} ({passed}/{total} checks passed)")
        return 1

    print(f"[ok] {MODE} ({passed}/{total} checks passed)")
    print(f"[ok] summary={display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
