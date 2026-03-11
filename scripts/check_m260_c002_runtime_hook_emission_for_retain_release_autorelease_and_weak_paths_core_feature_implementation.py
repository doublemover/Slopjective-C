#!/usr/bin/env python3
"""Fail-closed contract checker for M260-C002 ownership runtime hook emission."""

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
MODE = "m260-c002-runtime-hook-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-ownership-runtime-hook-emission/m260-c002-v1"
BOUNDARY_COMMENT_PREFIX = "; ownership_runtime_hook_emission = contract=objc3c-ownership-runtime-hook-emission/m260-c002-v1"
NAMED_METADATA_PREFIX = "!objc3.objc_runtime_ownership_hook_emission = !{"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation_packet.md"
DEFAULT_DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_ownership_runtime_hook_emission_positive.objc3"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m260_c002_ownership_runtime_hook_probe.cpp"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "c002"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json")
DEFAULT_CLANGXX = "clang++"
STORAGE_GLOBAL_RE = re.compile(r"^@objc3_property_storage_[A-Za-z0-9_]+ = private global i32 0, align 4$", re.MULTILINE)


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
    SnippetCheck("M260-C002-DOC-EXP-01", "# M260 Runtime Hook Emission For Retain, Release, Autorelease, And Weak Paths Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M260-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M260-C002-DOC-EXP-03", "`tests/tooling/fixtures/native/m260_ownership_runtime_hook_emission_positive.objc3`"),
    SnippetCheck("M260-C002-DOC-EXP-04", "`tests/tooling/runtime/m260_c002_ownership_runtime_hook_probe.cpp`"),
    SnippetCheck("M260-C002-DOC-EXP-05", "`tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-DOC-PKT-01", "# M260-C002 Runtime Hook Emission For Retain, Release, Autorelease, And Weak Paths Core Feature Implementation Packet"),
    SnippetCheck("M260-C002-DOC-PKT-02", "Issue: `#7174`"),
    SnippetCheck("M260-C002-DOC-PKT-03", "Packet: `M260-C002`"),
    SnippetCheck("M260-C002-DOC-PKT-04", "`tests/tooling/runtime/m260_c002_ownership_runtime_hook_probe.cpp`"),
    SnippetCheck("M260-C002-DOC-PKT-05", "`M260-D001`"),
)

DOC_SOURCE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-SRC-01", "## M260 ownership runtime hook emission (C002)"),
    SnippetCheck("M260-C002-SRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-C002-SRC-03", "`objc3_runtime_read_current_property_i32`"),
    SnippetCheck("M260-C002-SRC-04", "`check:objc3c:m260-c002-lane-c-readiness`"),
    SnippetCheck("M260-C002-SRC-05", "`M260-D001` is the next issue"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-NDOC-01", "## M260 ownership runtime hook emission (C002)"),
    SnippetCheck("M260-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-C002-NDOC-03", "`objc3_runtime_exchange_current_property_i32`"),
    SnippetCheck("M260-C002-NDOC-04", "`check:objc3c:m260-c002-lane-c-readiness`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-ARCH-01", "## M260 Ownership Runtime Hook Emission (C002)"),
    SnippetCheck("M260-C002-ARCH-02", "!objc3.objc_runtime_ownership_hook_emission"),
    SnippetCheck("M260-C002-ARCH-03", "runtime dispatch frames now carry the current receiver/property accessor"),
    SnippetCheck("M260-C002-ARCH-04", "the next issue is `M260-D001`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-SPC-01", "## M260 ownership runtime hook emission (C002)"),
    SnippetCheck("M260-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-C002-SPC-03", "`synthesized-accessors-call-runtime-owned-current-property-and-ownership-hook-entrypoints`"),
    SnippetCheck("M260-C002-SPC-04", "`runtime-dispatch-frame-selects-current-receiver-property-accessor-and-autorelease-queue`"),
    SnippetCheck("M260-C002-SPC-05", "`autorelease-values-drain-at-runtime-dispatch-return`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-META-01", "## M260 ownership runtime hook emission metadata anchors (C002)"),
    SnippetCheck("M260-C002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M260-C002-META-03", "`!objc3.objc_runtime_ownership_hook_emission`"),
    SnippetCheck("M260-C002-META-04", "`tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json`"),
)

SEMA_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-SEMA-01", "M260-C002 ownership runtime hook emission anchor:"),
)

IR_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-IR-01", 'out << "; ownership_runtime_hook_emission = "'),
    SnippetCheck("M260-C002-IR-02", '!objc3.objc_runtime_ownership_hook_emission = !{!69}'),
    SnippetCheck("M260-C002-IR-03", "kObjc3RuntimeReadCurrentPropertyI32Symbol"),
    SnippetCheck("M260-C002-IR-04", "kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol"),
    SnippetCheck("M260-C002-IR-05", "synthesized_ownership_runtime_hook_profile"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-LHDR-01", "kObjc3OwnershipRuntimeHookEmissionContractId"),
    SnippetCheck("M260-C002-LHDR-02", "kObjc3OwnershipRuntimeHookEmissionAccessorModel"),
    SnippetCheck("M260-C002-LHDR-03", "kObjc3RuntimeExchangeCurrentPropertyI32Symbol"),
    SnippetCheck("M260-C002-LHDR-04", "Objc3OwnershipRuntimeHookEmissionSummary()"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-LCPP-01", "Objc3OwnershipRuntimeHookEmissionSummary()"),
    SnippetCheck("M260-C002-LCPP-02", "M260-C002 runtime hook emission anchor"),
    SnippetCheck("M260-C002-LCPP-03", "read_property_symbol="),
    SnippetCheck("M260-C002-LCPP-04", "weak_store_symbol="),
)

RUNTIME_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-RT-01", "RuntimeDispatchFrame"),
    SnippetCheck("M260-C002-RT-02", "PushRuntimeDispatchFrame"),
    SnippetCheck("M260-C002-RT-03", "objc3_runtime_read_current_property_i32"),
    SnippetCheck("M260-C002-RT-04", "objc3_runtime_store_weak_current_property_i32"),
    SnippetCheck("M260-C002-RT-05", "objc3_runtime_autorelease_i32"),
)

RUNTIME_INTERNAL_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-RTH-01", "int objc3_runtime_read_current_property_i32(void);"),
    SnippetCheck("M260-C002-RTH-02", "void objc3_runtime_store_weak_current_property_i32(int value);"),
    SnippetCheck("M260-C002-RTH-03", "int objc3_runtime_autorelease_i32(int value);"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M260-C002-PKG-01", '"check:objc3c:m260-c002-runtime-hook-emission": "python scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py"'),
    SnippetCheck("M260-C002-PKG-02", '"test:tooling:m260-c002-runtime-hook-emission": "python -m pytest tests/tooling/test_check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py -q"'),
    SnippetCheck("M260-C002-PKG-03", '"check:objc3c:m260-c002-lane-c-readiness": "python scripts/run_m260_c002_lane_c_readiness.py"'),
)


HELPER_DECLARATIONS: tuple[str, ...] = (
    "declare i32 @objc3_runtime_read_current_property_i32()",
    "declare void @objc3_runtime_write_current_property_i32(i32)",
    "declare i32 @objc3_runtime_exchange_current_property_i32(i32)",
    "declare i32 @objc3_runtime_load_weak_current_property_i32()",
    "declare void @objc3_runtime_store_weak_current_property_i32(i32)",
    "declare i32 @objc3_runtime_retain_i32(i32)",
    "declare i32 @objc3_runtime_release_i32(i32)",
    "declare i32 @objc3_runtime_autorelease_i32(i32)",
)

HELPER_CALLS: tuple[str, ...] = (
    "call i32 @objc3_runtime_read_current_property_i32()",
    "call i32 @objc3_runtime_retain_i32(",
    "call i32 @objc3_runtime_release_i32(",
    "call i32 @objc3_runtime_autorelease_i32(",
    "call i32 @objc3_runtime_exchange_current_property_i32(",
    "call i32 @objc3_runtime_load_weak_current_property_i32()",
    "call void @objc3_runtime_store_weak_current_property_i32(i32 ",
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


@dataclass(frozen=True)
class CheckSummary:
    passed: int
    total: int


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DEFAULT_DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--runtime-cpp", type=Path, default=DEFAULT_RUNTIME_CPP)
    parser.add_argument("--runtime-internal-header", type=Path, default=DEFAULT_RUNTIME_INTERNAL_HEADER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default=DEFAULT_CLANGXX)
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


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


def extract_count(boundary_line: str, token: str) -> int:
    match = re.search(rf"(?:^|;){re.escape(token)}=(\d+)", boundary_line)
    return int(match.group(1)) if match else -1


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "probe_payload", check_id, detail, failures)

    parent = int(payload.get("parent", 0))
    child = int(payload.get("child", 0))
    check(parent > 0, "M260-C002-PROBE-01", "parent allocation must produce a positive runtime object identity")
    check(child > 0, "M260-C002-PROBE-02", "child allocation must produce a positive runtime object identity")
    check(payload.get("strong_set_result") == 0, "M260-C002-PROBE-03", "strong setter dispatch must return zero")
    check(payload.get("weak_set_result") == 0, "M260-C002-PROBE-04", "weak setter dispatch must return zero")
    check(payload.get("weak_before_clear") == child, "M260-C002-PROBE-05", "weak getter must observe the live child before the strong edge is cleared")
    check(payload.get("retain_result") == child, "M260-C002-PROBE-06", "retain helper must preserve the child identity")
    check(payload.get("release_after_retain_result") == child, "M260-C002-PROBE-07", "release after retain must preserve the child identity")
    check(payload.get("release_local_result") == child, "M260-C002-PROBE-08", "dropping the local child reference must leave the strong property owner alive")
    check(payload.get("strong_before_clear") == child, "M260-C002-PROBE-09", "strong getter must observe the child before clear")
    check(payload.get("clear_strong_result") == 0, "M260-C002-PROBE-10", "clearing the strong property must return zero")
    check(payload.get("strong_after_clear") == 0, "M260-C002-PROBE-11", "strong getter must observe zero after clear")
    check(payload.get("weak_after_clear") == 0, "M260-C002-PROBE-12", "weak getter must zero after the child is destroyed")
    check(payload.get("parent_release_result") == parent, "M260-C002-PROBE-13", "releasing the parent must return the released parent identity")

    graph_after_alloc = payload.get("graph_after_alloc", {})
    graph_after_drop_local = payload.get("graph_after_drop_local", {})
    graph_after_clear = payload.get("graph_after_clear", {})
    graph_after_parent_release = payload.get("graph_after_parent_release", {})
    check(graph_after_alloc.get("live_instance_count") == 2, "M260-C002-PROBE-14", "alloc phase must show two live instances")
    check(graph_after_drop_local.get("live_instance_count") == 2, "M260-C002-PROBE-15", "dropping the local child reference must keep two live instances")
    check(graph_after_clear.get("live_instance_count") == 1, "M260-C002-PROBE-16", "clearing the strong property must destroy the child and leave one live instance")
    check(graph_after_parent_release.get("live_instance_count") == 0, "M260-C002-PROBE-17", "releasing the parent must leave zero live instances")

    current_value_entry = payload.get("current_value_entry", {})
    weak_value_entry = payload.get("weak_value_entry", {})
    check(current_value_entry.get("found") == 1, "M260-C002-PROBE-18", "currentValue property entry must exist")
    check(current_value_entry.get("has_runtime_getter") == 1, "M260-C002-PROBE-19", "currentValue getter must be runtime backed")
    check(current_value_entry.get("has_runtime_setter") == 1, "M260-C002-PROBE-20", "currentValue setter must be runtime backed")
    check(current_value_entry.get("ownership_lifetime_profile") == "strong-owned", "M260-C002-PROBE-21", "currentValue lifetime must remain strong-owned")
    check(current_value_entry.get("ownership_runtime_hook_profile") is None, "M260-C002-PROBE-22", "currentValue runtime hook profile must stay null because the strong path is implicit in the synthesized accessor profile")
    check("ownership_lifetime=strong-owned" in str(current_value_entry.get("accessor_ownership_profile", "")), "M260-C002-PROBE-23", "currentValue accessor profile must preserve the strong-owned lowering claim")

    check(weak_value_entry.get("found") == 1, "M260-C002-PROBE-24", "weakValue property entry must exist")
    check(weak_value_entry.get("has_runtime_getter") == 1, "M260-C002-PROBE-25", "weakValue getter must be runtime backed")
    check(weak_value_entry.get("has_runtime_setter") == 1, "M260-C002-PROBE-26", "weakValue setter must be runtime backed")
    check(weak_value_entry.get("ownership_lifetime_profile") == "weak", "M260-C002-PROBE-27", "weakValue lifetime must remain weak")
    check(weak_value_entry.get("ownership_runtime_hook_profile") == "objc-weak-side-table", "M260-C002-PROBE-28", "weakValue must advertise the weak side-table runtime hook profile")
    check("runtime_hook=objc-weak-side-table" in str(weak_value_entry.get("accessor_ownership_profile", "")), "M260-C002-PROBE-29", "weakValue accessor profile must preserve the weak side-table hook claim")

    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M260-C002-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M260-C002-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M260-C002-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M260-C002-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")

    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M260-C002-DYN-05", f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    positive_dir = args.probe_root.resolve() / "positive"
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
    check(compile_result.returncode == 0, "M260-C002-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    check(module_ir.exists(), "M260-C002-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M260-C002-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_command": compile_command,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    ir_text = read_text(module_ir)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    accessor_count = extract_count(boundary_line, "synthesized_accessor_entries")
    storage_count = extract_count(boundary_line, "synthesized_storage_globals")
    storage_globals = len(STORAGE_GLOBAL_RE.findall(ir_text))
    check(bool(boundary_line), "M260-C002-DYN-09", "IR must publish the ownership runtime hook emission boundary line")
    check(NAMED_METADATA_PREFIX in ir_text, "M260-C002-DYN-10", "IR must publish !objc3.objc_runtime_ownership_hook_emission")
    for token, check_id in (
        ("accessor_model=synthesized-accessors-call-runtime-owned-current-property-and-ownership-hook-entrypoints", "M260-C002-DYN-11"),
        ("property_context_model=runtime-dispatch-frame-selects-current-receiver-property-accessor-and-autorelease-queue", "M260-C002-DYN-12"),
        ("autorelease_model=autorelease-values-drain-at-runtime-dispatch-return", "M260-C002-DYN-13"),
        ("fail_closed_model=owned-and-weak-runtime-backed-accessors-may-not-fall-back-to-summary-only-lowering", "M260-C002-DYN-14"),
        ("retain_symbol=objc3_runtime_retain_i32", "M260-C002-DYN-15"),
        ("release_symbol=objc3_runtime_release_i32", "M260-C002-DYN-16"),
        ("autorelease_symbol=objc3_runtime_autorelease_i32", "M260-C002-DYN-17"),
        ("read_property_symbol=objc3_runtime_read_current_property_i32", "M260-C002-DYN-18"),
        ("write_property_symbol=objc3_runtime_write_current_property_i32", "M260-C002-DYN-19"),
        ("exchange_property_symbol=objc3_runtime_exchange_current_property_i32", "M260-C002-DYN-20"),
        ("weak_load_symbol=objc3_runtime_load_weak_current_property_i32", "M260-C002-DYN-21"),
        ("weak_store_symbol=objc3_runtime_store_weak_current_property_i32", "M260-C002-DYN-22"),
    ):
        check(token in boundary_line, check_id, f"boundary line must contain {token}")
    check(accessor_count == 4, "M260-C002-DYN-23", "positive fixture must emit exactly four synthesized property accessors")
    check(storage_count == 2, "M260-C002-DYN-24", "positive fixture must preserve exactly two legacy synthesized storage globals")
    check(storage_globals == 2, "M260-C002-DYN-25", "IR must retain exactly two legacy synthesized storage globals for the positive fixture")

    for index, declaration in enumerate(HELPER_DECLARATIONS, start=26):
        check(declaration in ir_text, f"M260-C002-DYN-{index:02d}", f"IR must declare {declaration}")
    for index, call_site in enumerate(HELPER_CALLS, start=34):
        check(call_site in ir_text, f"M260-C002-DYN-{index:02d}", f"IR must execute {call_site}")

    check(module_obj.stat().st_size > 0, "M260-C002-DYN-41", "emitted object must be non-empty")

    probe_exe = positive_dir / "m260_c002_ownership_runtime_hook_probe.exe"
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
    check(probe_compile.returncode == 0, "M260-C002-DYN-42", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_command": compile_command,
            "probe_compile_command": probe_compile_command,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M260-C002-DYN-43", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_command": compile_command,
            "probe_compile_command": probe_compile_command,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M260-C002-DYN-44", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {
            "skipped": False,
            "compile_command": compile_command,
            "probe_compile_command": probe_compile_command,
            "probe_stdout": probe_run.stdout,
        }

    payload_passed, payload_total = validate_probe_payload(probe_payload, failures)
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
        "synthesized_accessor_entries": accessor_count,
        "synthesized_storage_globals": storage_count,
        "legacy_storage_global_count": storage_globals,
        "probe_payload": probe_payload,
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
        (args.runtime_cpp, RUNTIME_CPP_SNIPPETS),
        (args.runtime_internal_header, RUNTIME_INTERNAL_HEADER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    if args.skip_dynamic_probes:
        dynamic_case: dict[str, Any] = {"skipped": True}
    else:
        passed, total, dynamic_case = run_dynamic_case(args, failures)
        checks_passed += passed
        checks_total += total

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_case,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

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
