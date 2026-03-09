#!/usr/bin/env python3
"""Fail-closed checker for M255-D003 method cache and slow-path lookup."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-d003-method-cache-and-slow-path-lookup-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1"
LOOKUP_DISPATCH_CONTRACT_ID = "objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1"
SELECTOR_TABLE_CONTRACT_ID = "objc3c-runtime-selector-lookup-tables/m255-d002-v1"
METHOD_CACHE_STATE_SYMBOL = "objc3_runtime_copy_method_cache_state_for_testing"
METHOD_CACHE_ENTRY_SYMBOL = "objc3_runtime_copy_method_cache_entry_for_testing"
RECEIVER_NORMALIZATION_MODEL = (
    "known-class-and-class-self-receivers-normalize-to-one-metaclass-cache-key"
)
SLOW_PATH_RESOLUTION_MODEL = (
    "registered-class-and-metaclass-records-drive-deterministic-slow-path-method-resolution"
)
CACHE_MODEL = "normalized-receiver-plus-selector-stable-id-positive-and-negative-cache"
FALLBACK_MODEL = (
    "unsupported-or-ambiguous-runtime-resolution-falls-back-to-compatibility-dispatch-formula"
)
IR_COMMENT_PREFIX = "; runtime_method_cache_slow_path_lookup = contract=" + CONTRACT_ID

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m255_method_cache_and_slow_path_lookup_core_feature_implementation_d003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m255"
    / "m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation_packet.md"
)
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_LOWERING_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
)
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_BOOTSTRAP_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
)
DEFAULT_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_d003_live_method_dispatch.objc3"
DEFAULT_RUNTIME_PROBE = (
    ROOT / "tests" / "tooling" / "runtime" / "m255_d003_method_cache_slow_path_probe.cpp"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "d003-method-cache-slow-path"
DEFAULT_SUMMARY_OUT = (
    ROOT / "tmp" / "reports" / "m255" / "M255-D003" / "method_cache_and_slow_path_lookup_summary.json"
)

EXPECTED_INSTANCE_RESULT = 11
EXPECTED_CLASS_RESULT = 22
EXPECTED_FALLBACK_RESULT = 633551


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
    SnippetCheck("M255-D003-DOC-EXP-01", "# M255 Method Cache and Slow-Path Lookup Core Feature Implementation Expectations (D003)"),
    SnippetCheck("M255-D003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-D003-DOC-EXP-03", f"`{METHOD_CACHE_STATE_SYMBOL}`"),
    SnippetCheck("M255-D003-DOC-EXP-04", f"`{METHOD_CACHE_ENTRY_SYMBOL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-D003-PKT-01", "# M255-D003 Method Cache and Slow-Path Lookup Core Feature Implementation Packet"),
    SnippetCheck("M255-D003-PKT-02", "Packet: `M255-D003`"),
    SnippetCheck("M255-D003-PKT-03", f"`{SELECTOR_TABLE_CONTRACT_ID}`"),
    SnippetCheck("M255-D003-PKT-04", "normalized instance/class dispatch can resolve callable emitted method bodies"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-D003-NDOC-01", "## Method cache and slow-path lookup (M255-D003)"),
    SnippetCheck("M255-D003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-D003-NDOC-03", f"`{CACHE_MODEL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-D003-SPC-01", "## M255 method-cache and slow-path lookup (D003)"),
    SnippetCheck("M255-D003-SPC-02", f"`{RECEIVER_NORMALIZATION_MODEL}`"),
    SnippetCheck("M255-D003-SPC-03", f"`{FALLBACK_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-D003-META-01", "## M255 method-cache / slow-path metadata anchors (D003)"),
    SnippetCheck("M255-D003-META-02", f"`{METHOD_CACHE_STATE_SYMBOL}`"),
    SnippetCheck("M255-D003-META-03", f"`{METHOD_CACHE_ENTRY_SYMBOL}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M255-D003-ARCH-01", "## M255 method cache and slow-path lookup (D003)"),
    SnippetCheck("M255-D003-ARCH-02", "repeat dispatches reuse deterministic positive and negative cache entries"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D003-RTDOC-01", "`M255-D003` then turns live dispatch into a method-cache plus deterministic"),
    SnippetCheck("M255-D003-RTDOC-02", f"`{METHOD_CACHE_STATE_SYMBOL}`"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D003-TRTDOC-01", "`M255-D003` adds the live method-cache / slow-path proof surface:"),
    SnippetCheck("M255-D003-TRTDOC-02", f"`{METHOD_CACHE_ENTRY_SYMBOL}`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-D003-LHC-01", "kObjc3RuntimeMethodCacheSlowPathContractId"),
    SnippetCheck("M255-D003-LHC-02", "kObjc3RuntimeMethodCacheSlowPathReceiverNormalizationModel"),
    SnippetCheck("M255-D003-LHC-03", CACHE_MODEL),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M255-D003-IR-01", "runtime_method_cache_slow_path_lookup = contract="),
    SnippetCheck("M255-D003-IR-02", "M255-D003 slow-path anchor: class-implementation method-table"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-D003-PAR-01", "M255-D003 method-cache / slow-path anchor"),
    SnippetCheck("M255-D003-PAR-02", "runtime now consumes for deterministic class/metaclass slow-path lookup"),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M255-D003-RTS-01", "M255-D003: live lookup walks the emitted class/metaclass graph"),
    SnippetCheck("M255-D003-RTS-02", METHOD_CACHE_STATE_SYMBOL),
    SnippetCheck("M255-D003-RTS-03", METHOD_CACHE_ENTRY_SYMBOL),
)
BOOTSTRAP_HEADER_SNIPPETS = (
    SnippetCheck("M255-D003-BH-01", "typedef struct objc3_runtime_method_cache_state_snapshot"),
    SnippetCheck("M255-D003-BH-02", METHOD_CACHE_STATE_SYMBOL),
    SnippetCheck("M255-D003-BH-03", METHOD_CACHE_ENTRY_SYMBOL),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-D003-SHIM-01", "M255-D003 method-cache / slow-path lookup"),
    SnippetCheck("M255-D003-SHIM-02", "compatibility evidence only"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M255-D003-FIX-01", "module liveMethodDispatch;"),
    SnippetCheck("M255-D003-FIX-02", "- (i32) currentValue {"),
    SnippetCheck("M255-D003-FIX-03", "+ (i32) shared {"),
    SnippetCheck("M255-D003-FIX-04", "return [Widget shared];"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M255-D003-PRB-01", '#include "runtime/objc3_runtime_bootstrap_internal.h"'),
    SnippetCheck("M255-D003-PRB-02", 'const char *const fallback_selector = "missingRuntimeSelector:";'),
    SnippetCheck("M255-D003-PRB-03", 'extern "C" int callSharedThroughKnownClass(void);'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M255-D003-PKG-01",
        '"check:objc3c:m255-d003-method-cache-and-slow-path-lookup-core-feature-implementation": "python scripts/check_m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M255-D003-PKG-02",
        '"test:tooling:m255-d003-method-cache-and-slow-path-lookup-core-feature-implementation": "python -m pytest tests/tooling/test_check_m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M255-D003-PKG-03",
        '"check:objc3c:m255-d003-lane-d-readiness": "python scripts/run_m255_d003_lane_d_readiness.py"',
    ),
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
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--tooling-runtime-readme", type=Path, default=DEFAULT_TOOLING_RUNTIME_README)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--bootstrap-header", type=Path, default=DEFAULT_BOOTSTRAP_HEADER)
    parser.add_argument("--shim-source", type=Path, default=DEFAULT_SHIM)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--llvm-readobj", default="llvm-readobj.exe")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M255-D003-MISSING", f"required artifact is missing: {display_path(path)}"))
        return 1 + len(snippets)
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd or ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


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


def parse_section_metrics(readobj_text: str, section_name: str) -> dict[str, int] | None:
    pattern = re.compile(
        r"Name:\s*"
        + re.escape(section_name)
        + r".*?RawDataSize:\s*(\d+).*?RelocationCount:\s*(\d+)",
        re.DOTALL,
    )
    match = pattern.search(readobj_text)
    if not match:
        return None
    return {
        "raw_data_size": int(match.group(1)),
        "relocation_count": int(match.group(2)),
    }


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
    class_self_state = payload.get("class_self_state", {})
    known_class_state = payload.get("known_class_state", {})
    fallback_first_state = payload.get("fallback_first_state", {})
    fallback_second_state = payload.get("fallback_second_state", {})
    instance_entry = payload.get("instance_entry", {})
    class_entry = payload.get("class_entry", {})
    fallback_entry = payload.get("fallback_entry", {})

    check(payload.get("instance_first") == EXPECTED_INSTANCE_RESULT, "M255-D003-PROBE-01", "instance_first must resolve live method body")
    check(payload.get("instance_second") == EXPECTED_INSTANCE_RESULT, "M255-D003-PROBE-02", "instance_second must preserve live result")
    check(payload.get("class_self") == EXPECTED_CLASS_RESULT, "M255-D003-PROBE-03", "class_self must resolve live class method body")
    check(payload.get("known_class") == EXPECTED_CLASS_RESULT, "M255-D003-PROBE-04", "known_class must resolve same class method body")
    check(payload.get("fallback_first") == EXPECTED_FALLBACK_RESULT, "M255-D003-PROBE-05", "fallback_first must preserve compatibility result")
    check(payload.get("fallback_second") == EXPECTED_FALLBACK_RESULT, "M255-D003-PROBE-06", "fallback_second must preserve compatibility result")
    check(payload.get("fallback_expected") == EXPECTED_FALLBACK_RESULT, "M255-D003-PROBE-07", "fallback_expected must match compatibility result")
    check(registration_state.get("registered_image_count") == 1, "M255-D003-PROBE-08", "startup registration must have one registered image")
    check(selector_table_state.get("selector_table_entry_count") == 4, "M255-D003-PROBE-09", "selector table must expose four selectors")
    check(selector_table_state.get("metadata_backed_selector_count") == 4, "M255-D003-PROBE-10", "selector table must remain metadata backed")
    check(instance_first_state.get("cache_entry_count") == 1, "M255-D003-PROBE-11", "first instance dispatch must populate one cache entry")
    check(instance_first_state.get("cache_hit_count") == 0, "M255-D003-PROBE-12", "first instance dispatch must not be a cache hit")
    check(instance_first_state.get("cache_miss_count") == 1, "M255-D003-PROBE-13", "first instance dispatch must record one miss")
    check(instance_first_state.get("slow_path_lookup_count") == 1, "M255-D003-PROBE-14", "first instance dispatch must use slow path")
    check(instance_first_state.get("last_selector") == "currentValue", "M255-D003-PROBE-15", "instance slow path must resolve currentValue")
    check(instance_first_state.get("last_normalized_receiver_identity") == 1025, "M255-D003-PROBE-16", "instance slow path must preserve instance identity")
    check(instance_first_state.get("last_dispatch_used_cache") == 0, "M255-D003-PROBE-17", "first instance dispatch must not use cache")
    check(instance_first_state.get("last_dispatch_resolved_live_method") == 1, "M255-D003-PROBE-18", "first instance dispatch must resolve live method")
    check(instance_first_state.get("last_resolved_owner_identity") == "implementation:Widget::instance_method:currentValue", "M255-D003-PROBE-19", "instance slow path must resolve currentValue implementation")
    check(instance_second_state.get("cache_hit_count") == 1, "M255-D003-PROBE-20", "second instance dispatch must hit cache")
    check(instance_second_state.get("last_dispatch_used_cache") == 1, "M255-D003-PROBE-21", "second instance dispatch must report cache use")
    check(class_self_state.get("cache_entry_count") == 2, "M255-D003-PROBE-22", "class dispatch must add a second cache entry")
    check(class_self_state.get("cache_miss_count") == 2, "M255-D003-PROBE-23", "class dispatch must record second miss")
    check(class_self_state.get("last_selector") == "shared", "M255-D003-PROBE-24", "class dispatch must resolve shared")
    check(class_self_state.get("last_normalized_receiver_identity") == 1026, "M255-D003-PROBE-25", "class dispatch must normalize to metaclass identity")
    check(known_class_state.get("cache_hit_count") == 2, "M255-D003-PROBE-26", "known-class dispatch must hit existing class cache")
    check(known_class_state.get("last_dispatch_used_cache") == 1, "M255-D003-PROBE-27", "known-class dispatch must reuse class cache")
    check(known_class_state.get("last_normalized_receiver_identity") == 1026, "M255-D003-PROBE-28", "known-class receiver must normalize to class-self cache key")
    check(fallback_first_state.get("cache_entry_count") == 3, "M255-D003-PROBE-29", "first fallback must add a negative cache entry")
    check(fallback_first_state.get("fallback_dispatch_count") == 1, "M255-D003-PROBE-30", "first fallback must increment fallback count")
    check(fallback_first_state.get("last_dispatch_resolved_live_method") == 0, "M255-D003-PROBE-31", "fallback path must not resolve live method")
    check(fallback_first_state.get("last_dispatch_fell_back") == 1, "M255-D003-PROBE-32", "fallback path must report fallback")
    check(fallback_second_state.get("cache_hit_count") == 3, "M255-D003-PROBE-33", "second fallback must hit negative cache")
    check(fallback_second_state.get("fallback_dispatch_count") == 2, "M255-D003-PROBE-34", "second fallback must preserve fallback count")
    check(fallback_second_state.get("last_dispatch_used_cache") == 1, "M255-D003-PROBE-35", "second fallback must report cache use")
    check(instance_entry.get("found") == 1 and instance_entry.get("resolved") == 1, "M255-D003-PROBE-36", "instance cache entry must be resolved")
    check(instance_entry.get("dispatch_family_is_class") == 0, "M255-D003-PROBE-37", "instance cache entry must be instance family")
    check(instance_entry.get("normalized_receiver_identity") == 1025, "M255-D003-PROBE-38", "instance cache entry must retain instance identity")
    check(instance_entry.get("selector") == "currentValue", "M255-D003-PROBE-39", "instance cache entry selector mismatch")
    check(instance_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:currentValue", "M255-D003-PROBE-40", "instance cache entry owner mismatch")
    check(class_entry.get("found") == 1 and class_entry.get("resolved") == 1, "M255-D003-PROBE-41", "class cache entry must be resolved")
    check(class_entry.get("dispatch_family_is_class") == 1, "M255-D003-PROBE-42", "class cache entry must be class family")
    check(class_entry.get("normalized_receiver_identity") == 1026, "M255-D003-PROBE-43", "class cache entry must use normalized metaclass identity")
    check(class_entry.get("selector") == "shared", "M255-D003-PROBE-44", "class cache entry selector mismatch")
    check(fallback_entry.get("found") == 1 and fallback_entry.get("resolved") == 0, "M255-D003-PROBE-45", "fallback cache entry must be negative")
    check(fallback_entry.get("normalized_receiver_identity") == 1025, "M255-D003-PROBE-46", "negative cache entry must use instance identity")
    check(fallback_entry.get("selector") == "missingRuntimeSelector:", "M255-D003-PROBE-47", "negative cache entry selector mismatch")
    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M255-D003-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M255-D003-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M255-D003-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M255-D003-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")

    clangxx = resolve_tool(args.clangxx)
    llvm_readobj = resolve_tool(args.llvm_readobj)
    check(clangxx is not None, "M255-D003-DYN-05", f"unable to resolve {args.clangxx}")
    check(llvm_readobj is not None, "M255-D003-DYN-06", f"unable to resolve {args.llvm_readobj}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    args.probe_root.mkdir(parents=True, exist_ok=True)
    compile_result = run_command(
        [
            str(args.native_exe),
            str(args.fixture),
            "--out-dir",
            str(args.probe_root),
            "--emit-prefix",
            "module",
        ]
    )
    check(compile_result.returncode == 0, "M255-D003-DYN-07", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    if compile_result.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "compile_stdout": compile_result.stdout, "compile_stderr": compile_result.stderr}

    module_ir = args.probe_root / "module.ll"
    module_obj = args.probe_root / "module.obj"
    probe_exe = args.probe_root / "m255_d003_method_cache_slow_path_probe.exe"
    check(module_ir.exists(), "M255-D003-DYN-08", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M255-D003-DYN-09", f"missing emitted object: {display_path(module_obj)}")
    if not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {"skipped": False}

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
        ]
    )
    check(probe_compile.returncode == 0, "M255-D003-DYN-10", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_compile_stdout": probe_compile.stdout, "probe_compile_stderr": probe_compile.stderr}

    probe_run = run_command([str(probe_exe)])
    check(probe_run.returncode == 0, "M255-D003-DYN-11", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_stdout": probe_run.stdout, "probe_stderr": probe_run.stderr}
    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M255-D003-DYN-12", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}
    checks_total += 1
    checks_passed += 1

    payload_passed, payload_total = probe_payload_checks(probe_payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total

    ir_text = read_text(module_ir)
    check(IR_COMMENT_PREFIX in ir_text, "M255-D003-DYN-13", "IR must publish D003 slow-path contract comment")
    check("ptr @objc3_method_Widget_instance_currentValue" in ir_text, "M255-D003-DYN-14", "IR must carry instance implementation pointer in method list")
    check("ptr @objc3_method_Widget_class_shared" in ir_text, "M255-D003-DYN-15", "IR must carry class implementation pointer in method list")
    check("{ i64, ptr, ptr, [2 x { ptr, ptr, ptr, i64, ptr, i64 }] }" in ir_text, "M255-D003-DYN-16", "IR must emit expanded method-list entry layout")

    readobj_result = run_command([str(llvm_readobj), "--sections", str(module_obj)])
    check(readobj_result.returncode == 0, "M255-D003-DYN-17", f"llvm-readobj failed: {readobj_result.stdout}{readobj_result.stderr}")
    if readobj_result.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_payload": probe_payload}

    readobj_text = readobj_result.stdout
    class_metrics = parse_section_metrics(readobj_text, "objc3.runtime.class_descriptors")
    selector_metrics = parse_section_metrics(readobj_text, "objc3.runtime.selector_pool")
    string_metrics = parse_section_metrics(readobj_text, "objc3.runtime.string_pool")
    check(class_metrics is not None, "M255-D003-DYN-18", "object must expose class_descriptors section")
    check(selector_metrics is not None, "M255-D003-DYN-19", "object must expose selector_pool section")
    check(string_metrics is not None, "M255-D003-DYN-20", "object must expose string_pool section")
    if class_metrics is not None:
        check(class_metrics["raw_data_size"] > 0, "M255-D003-DYN-21", "class_descriptors section must carry payload bytes")
        check(class_metrics["relocation_count"] > 0, "M255-D003-DYN-22", "class_descriptors section must carry relocations")
    if selector_metrics is not None:
        check(selector_metrics["raw_data_size"] > 0, "M255-D003-DYN-23", "selector_pool section must carry payload bytes")
    if string_metrics is not None:
        check(string_metrics["raw_data_size"] > 0, "M255-D003-DYN-24", "string_pool section must carry payload bytes")
    check("objc3.runtime.discovery_root" in readobj_text, "M255-D003-DYN-25", "object must preserve discovery_root section")
    check("objc3.runtime.linker_anchor" in readobj_text, "M255-D003-DYN-26", "object must preserve linker_anchor section")

    return checks_passed, checks_total, {
        "skipped": False,
        "compile_stdout": compile_result.stdout,
        "compile_stderr": compile_result.stderr,
        "probe_payload": probe_payload,
        "class_section": class_metrics,
        "selector_section": selector_metrics,
        "string_section": string_metrics,
        "readobj_excerpt": readobj_text,
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_groups = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        (args.bootstrap_header, BOOTSTRAP_HEADER_SNIPPETS),
        (args.shim_source, SHIM_SNIPPETS),
        (args.fixture, FIXTURE_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_groups:
        checks_total += 1 + len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_case: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_case = {"skipped": True}
    else:
        dyn_passed, dyn_total, dynamic_case = run_dynamic_case(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "lookup_dispatch_contract_id": LOOKUP_DISPATCH_CONTRACT_ID,
        "selector_table_contract_id": SELECTOR_TABLE_CONTRACT_ID,
        "receiver_normalization_model": RECEIVER_NORMALIZATION_MODEL,
        "slow_path_resolution_model": SLOW_PATH_RESOLUTION_MODEL,
        "cache_model": CACHE_MODEL,
        "fallback_model": FALLBACK_MODEL,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "ok": not failures,
        "findings": [finding.__dict__ for finding in failures],
        "dynamic_case": dynamic_case,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")
    sys.stdout.write(canonical_json(summary_payload))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
