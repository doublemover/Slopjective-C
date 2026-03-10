#!/usr/bin/env python3
"""Fail-closed checker for M256-C002 executable method-body binding."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-c002-bind-method-bodies-to-runtime-metadata-entries-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-executable-method-body-binding/m256-c002-v1"
BOUNDARY_COMMENT_PREFIX = (
    "; executable_method_body_binding = contract="
    "objc3c-executable-method-body-binding/m256-c002-v1"
)
OBJECT_BOUNDARY_COMMENT_PREFIX = (
    "; executable_object_artifact_lowering = contract="
    "objc3c-executable-object-artifact-lowering/m256-c001-v1"
)
DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m256_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation_c002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m256"
    / "m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
)
DEFAULT_LOWERING_CPP = (
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
)
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_c002_method_body_binding.objc3"
)
DEFAULT_RUNTIME_PROBE = (
    ROOT / "tests" / "tooling" / "runtime" / "m256_c002_method_binding_probe.cpp"
)
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PROBE_ROOT = (
    ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "c002"
)
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m256/M256-C002/method_body_binding_summary.json")

EXPECTED_INSTANCE_RESULT = 20
EXPECTED_CLASS_RESULT = 44
EXPECTED_CATEGORY_RESULT = 33
EXPECTED_INSTANCE_OWNER = "implementation:Widget::instance_method:value:extra:"
EXPECTED_CLASS_OWNER = "implementation:Widget::class_method:classValue"
EXPECTED_CATEGORY_OWNER = "implementation:Widget(Tracing)::instance_method:tracedValue"


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
    SnippetCheck("M256-C002-DOC-EXP-01", "# M256 Bind Method Bodies to Runtime Metadata Entries Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M256-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-C002-DOC-EXP-03", "`error-on-missing-or-duplicate-implementation-binding`"),
    SnippetCheck("M256-C002-DOC-EXP-04", "`tests/tooling/runtime/m256_c002_method_binding_probe.cpp`"),
    SnippetCheck("M256-C002-DOC-EXP-05", "`tmp/reports/m256/M256-C002/method_body_binding_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-PKT-01", "# M256-C002 Bind Method Bodies to Runtime Metadata Entries Core Feature Implementation Packet"),
    SnippetCheck("M256-C002-PKT-02", "Packet: `M256-C002`"),
    SnippetCheck("M256-C002-PKT-03", "Issue: `#7137`"),
    SnippetCheck("M256-C002-PKT-04", "`tests/tooling/fixtures/native/m256_c002_method_body_binding.objc3`"),
    SnippetCheck("M256-C002-PKT-05", "`tests/tooling/runtime/m256_c002_method_binding_probe.cpp`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-ARCH-01", "## M256 executable method-body binding (C002)"),
    SnippetCheck("M256-C002-ARCH-02", "fail closed if that binding is missing or duplicated"),
    SnippetCheck("M256-C002-ARCH-03", "check:objc3c:m256-c002-lane-c-readiness"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-NDOC-01", "## Executable method-body binding (M256-C002)"),
    SnippetCheck("M256-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-C002-NDOC-03", "`error-on-missing-or-duplicate-implementation-binding`"),
    SnippetCheck("M256-C002-NDOC-04", "`tmp/reports/m256/M256-C002/method_body_binding_summary.json`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-SPC-01", "## M256 executable method-body binding (C002)"),
    SnippetCheck("M256-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-C002-SPC-03", "duplicate bindings for the same canonical method owner identity fail closed"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-META-01", "## M256 executable method-body binding metadata anchors (C002)"),
    SnippetCheck("M256-C002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-C002-META-03", "`implementation:Widget(Tracing)::instance_method:tracedValue`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-LHDR-01", "kObjc3ExecutableMethodBodyBindingContractId"),
    SnippetCheck("M256-C002-LHDR-02", "kObjc3ExecutableMethodBodyBindingSourceModel"),
    SnippetCheck("M256-C002-LHDR-03", "kObjc3ExecutableMethodBodyBindingRuntimeModel"),
    SnippetCheck("M256-C002-LHDR-04", "Objc3ExecutableMethodBodyBindingSummary()"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-LCPP-01", "Objc3ExecutableMethodBodyBindingSummary()"),
    SnippetCheck("M256-C002-LCPP-02", "M256-C002 executable method-body binding implementation anchor"),
    SnippetCheck("M256-C002-LCPP-03", "kObjc3ExecutableMethodBodyBindingFailClosedModel"),
)

PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-PARSE-01", "M256-C002 executable method-body binding anchor"),
)

SEMA_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-SEMA-01", "M256-C002 executable method-body binding anchor"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-IR-01", 'out << "; executable_method_body_binding = "'),
    SnippetCheck("M256-C002-IR-02", "missing executable method-body binding for owner identity"),
    SnippetCheck("M256-C002-IR-03", "duplicate executable method-body binding for owner identity"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M256-C002-PKG-01",
        '"check:objc3c:m256-c002-bind-method-bodies-to-runtime-metadata-entries": "python scripts/check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M256-C002-PKG-02",
        '"test:tooling:m256-c002-bind-method-bodies-to-runtime-metadata-entries": "python -m pytest tests/tooling/test_check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M256-C002-PKG-03",
        '"check:objc3c:m256-c002-lane-c-readiness": "python scripts/run_m256_c002_lane_c_readiness.py"',
    ),
)

FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-FIX-01", "module executableMethodBodyBinding;"),
    SnippetCheck("M256-C002-FIX-02", "- (i32) value:(i32)lhs extra:(i32)rhs;"),
    SnippetCheck("M256-C002-FIX-03", "+ (i32) classValue {"),
    SnippetCheck("M256-C002-FIX-04", "@implementation Widget (Tracing)"),
)

PROBE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C002-PRB-01", '#include "runtime/objc3_runtime_bootstrap_internal.h"'),
    SnippetCheck("M256-C002-PRB-02", 'objc3_runtime_dispatch_i32(1025, "value:extra:", 7, 8, 0, 0)'),
    SnippetCheck("M256-C002-PRB-03", 'objc3_runtime_dispatch_i32(1026, "classValue", 0, 0, 0, 0)'),
    SnippetCheck("M256-C002-PRB-04", 'objc3_runtime_dispatch_i32(1025, "tracedValue", 0, 0, 0, 0)'),
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
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default="clang++.exe")
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
            failures.append(
                Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}")
            )
    return passed


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd),
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


def resolve_tool(executable: str) -> Path | None:
    env_bin = os.environ.get("LLVM_BIN_DIR")
    if env_bin:
        candidate = Path(env_bin) / executable
        if candidate.exists():
            return candidate
    default_candidate = Path("C:/Program Files/LLVM/bin") / executable
    if default_candidate.exists():
        return default_candidate
    which = shutil.which(executable)
    if which:
        return Path(which)
    return None


def probe_payload_checks(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "dynamic_probe", check_id, detail, failures)

    registration_state = payload.get("registration_state", {})
    selector_table_state = payload.get("selector_table_state", {})
    instance_first_state = payload.get("instance_first_state", {})
    instance_second_state = payload.get("instance_second_state", {})
    class_state = payload.get("class_state", {})
    known_class_state = payload.get("known_class_state", {})
    category_state = payload.get("category_state", {})
    instance_entry = payload.get("instance_entry", {})
    class_entry = payload.get("class_entry", {})
    category_entry = payload.get("category_entry", {})

    check(payload.get("instance_first") == EXPECTED_INSTANCE_RESULT, "M256-C002-PROBE-01", "instance dispatch must execute the class implementation body")
    check(payload.get("instance_second") == EXPECTED_INSTANCE_RESULT, "M256-C002-PROBE-02", "repeat instance dispatch must preserve the class implementation result")
    check(payload.get("class_value") == EXPECTED_CLASS_RESULT, "M256-C002-PROBE-03", "class dispatch must execute the class method body")
    check(payload.get("known_class_value") == EXPECTED_CLASS_RESULT, "M256-C002-PROBE-04", "known-class dispatch must reuse the same class method result")
    check(payload.get("category_value") == EXPECTED_CATEGORY_RESULT, "M256-C002-PROBE-05", "category dispatch must execute the category implementation body")
    check(registration_state.get("registered_image_count") == 1, "M256-C002-PROBE-06", "runtime must register exactly one image for the C002 fixture")
    check(selector_table_state.get("selector_table_entry_count") == 3, "M256-C002-PROBE-07", "selector table must expose the three fixture selectors")
    check(selector_table_state.get("metadata_backed_selector_count") == 3, "M256-C002-PROBE-08", "all fixture selectors must be metadata-backed")
    check(selector_table_state.get("dynamic_selector_count") == 0, "M256-C002-PROBE-09", "C002 probe must not require dynamic selector materialization")
    check(instance_first_state.get("last_dispatch_resolved_live_method") == 1, "M256-C002-PROBE-10", "instance dispatch must resolve a live method")
    check(instance_first_state.get("last_dispatch_used_cache") == 0, "M256-C002-PROBE-11", "first instance dispatch must miss cache")
    check(instance_first_state.get("last_resolved_owner_identity") == EXPECTED_INSTANCE_OWNER, "M256-C002-PROBE-12", "instance dispatch must resolve the class implementation owner identity")
    check(instance_second_state.get("last_dispatch_used_cache") == 1, "M256-C002-PROBE-13", "repeat instance dispatch must hit cache")
    check(instance_second_state.get("last_resolved_owner_identity") == EXPECTED_INSTANCE_OWNER, "M256-C002-PROBE-14", "repeat instance dispatch must preserve owner identity")
    check(class_state.get("last_dispatch_resolved_live_method") == 1, "M256-C002-PROBE-15", "class dispatch must resolve a live method")
    check(class_state.get("last_dispatch_used_cache") == 0, "M256-C002-PROBE-16", "first class dispatch must miss cache")
    check(class_state.get("last_resolved_owner_identity") == EXPECTED_CLASS_OWNER, "M256-C002-PROBE-17", "class dispatch must resolve the class method owner identity")
    check(known_class_state.get("last_dispatch_used_cache") == 1, "M256-C002-PROBE-18", "known-class dispatch must reuse the normalized class cache entry")
    check(known_class_state.get("last_normalized_receiver_identity") == 1026, "M256-C002-PROBE-19", "known-class dispatch must normalize onto the metaclass identity")
    check(known_class_state.get("last_resolved_owner_identity") == EXPECTED_CLASS_OWNER, "M256-C002-PROBE-20", "known-class dispatch must preserve the class method owner identity")
    check(category_state.get("last_dispatch_resolved_live_method") == 1, "M256-C002-PROBE-21", "category dispatch must resolve a live method")
    check(category_state.get("last_dispatch_used_cache") == 0, "M256-C002-PROBE-22", "first category dispatch must miss cache")
    check(category_state.get("last_resolved_owner_identity") == EXPECTED_CATEGORY_OWNER, "M256-C002-PROBE-23", "category dispatch must resolve the category implementation owner identity")
    check(instance_entry.get("found") == 1 and instance_entry.get("resolved") == 1, "M256-C002-PROBE-24", "instance cache entry must resolve successfully")
    check(instance_entry.get("parameter_count") == 2, "M256-C002-PROBE-25", "instance cache entry must preserve the two-parameter method arity")
    check(instance_entry.get("resolved_owner_identity") == EXPECTED_INSTANCE_OWNER, "M256-C002-PROBE-26", "instance cache entry must retain the class implementation owner identity")
    check(class_entry.get("found") == 1 and class_entry.get("resolved") == 1, "M256-C002-PROBE-27", "class cache entry must resolve successfully")
    check(class_entry.get("parameter_count") == 0, "M256-C002-PROBE-28", "class cache entry must preserve zero method parameters")
    check(class_entry.get("resolved_owner_identity") == EXPECTED_CLASS_OWNER, "M256-C002-PROBE-29", "class cache entry must retain the class method owner identity")
    check(category_entry.get("found") == 1 and category_entry.get("resolved") == 1, "M256-C002-PROBE-30", "category cache entry must resolve successfully")
    check(category_entry.get("parameter_count") == 0, "M256-C002-PROBE-31", "category cache entry must preserve zero method parameters")
    check(category_entry.get("resolved_owner_identity") == EXPECTED_CATEGORY_OWNER, "M256-C002-PROBE-32", "category cache entry must retain the category method owner identity")
    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M256-C002-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M256-C002-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M256-C002-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M256-C002-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")

    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M256-C002-DYN-05", f"unable to resolve {args.clangxx}")
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
    check(compile_result.returncode == 0, "M256-C002-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    check(module_ir.exists(), "M256-C002-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M256-C002-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    ir_text = read_text(module_ir)
    boundary_line = next(
        (line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)),
        "",
    )
    object_boundary_line = next(
        (line for line in ir_text.splitlines() if line.startswith(OBJECT_BOUNDARY_COMMENT_PREFIX)),
        "",
    )
    check(bool(boundary_line), "M256-C002-DYN-09", "IR must publish the executable method-body binding summary")
    check(bool(object_boundary_line), "M256-C002-DYN-10", "IR must retain the M256-C001 object-artifact boundary summary")
    check("source_model=implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol" in boundary_line, "M256-C002-DYN-11", "binding summary must publish the canonical source model")
    check("runtime_model=emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32" in boundary_line, "M256-C002-DYN-12", "binding summary must publish the runtime model")
    check("fail_closed_model=error-on-missing-or-duplicate-implementation-binding" in boundary_line, "M256-C002-DYN-13", "binding summary must publish the fail-closed model")
    check("bound_method_entry_count=3" in boundary_line, "M256-C002-DYN-14", "binding summary must publish the expected bound method entry count")
    check("ptr @objc3_method_Widget_instance_value_extra_" in ir_text, "M256-C002-DYN-15", "IR must bind the class instance method implementation pointer")
    check("ptr @objc3_method_Widget_class_classValue" in ir_text, "M256-C002-DYN-16", "IR must bind the class method implementation pointer")
    check("ptr @objc3_method_Widget_instance_tracedValue" in ir_text, "M256-C002-DYN-17", "IR must bind the category method implementation pointer")
    check(module_obj.stat().st_size > 0, "M256-C002-DYN-18", "emitted object must be non-empty")

    probe_exe = probe_dir / "m256_c002_method_binding_probe.exe"
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
    check(probe_compile.returncode == 0, "M256-C002-DYN-19", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M256-C002-DYN-20", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M256-C002-DYN-21", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = probe_payload_checks(probe_payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "binding_boundary_line": boundary_line,
        "object_boundary_line": object_boundary_line,
        "payload": probe_payload,
    }


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
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
        (args.fixture, FIXTURE_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_case: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_case = {"skipped": True}
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
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
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
            print(
                f"- {finding.check_id} [{finding.artifact}] {finding.detail}",
                file=sys.stderr,
            )
        print(f"[info] summary: {display_path(summary_path)}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
