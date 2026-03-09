#!/usr/bin/env python3
"""Fail-closed checker for M255-D004 protocol/category-aware method resolution."""

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
MODE = "m255-d004-protocol-and-category-aware-method-resolution-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-runtime-protocol-category-method-resolution/m255-d004-v1"
LOOKUP_DISPATCH_CONTRACT_ID = "objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1"
SELECTOR_TABLE_CONTRACT_ID = "objc3c-runtime-selector-lookup-tables/m255-d002-v1"
METHOD_CACHE_CONTRACT_ID = "objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1"
CATEGORY_RESOLUTION_MODEL = (
    "class-bodies-win-first-category-implementation-records-supply-next-live-method-tier"
)
PROTOCOL_DECLARATION_MODEL = (
    "adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution"
)
FALLBACK_MODEL = (
    "conflicting-category-or-protocol-resolution-fails-closed-to-compatibility-dispatch"
)
IR_COMMENT_PREFIX = (
    "; runtime_protocol_category_method_resolution = contract=" + CONTRACT_ID
)

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m255_protocol_and_category_aware_method_resolution_core_feature_expansion_d004_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m255"
    / "m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion_packet.md"
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
DEFAULT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_d004_protocol_category_dispatch.objc3"
)
DEFAULT_RUNTIME_PROBE = (
    ROOT / "tests" / "tooling" / "runtime" / "m255_d004_protocol_category_probe.cpp"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PROBE_ROOT = (
    ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "d004-protocol-category-method-resolution"
)
DEFAULT_SUMMARY_OUT = (
    ROOT / "tmp" / "reports" / "m255" / "M255-D004" / "protocol_category_method_resolution_summary.json"
)

EXPECTED_INSTANCE_RESULT = 33
EXPECTED_CLASS_RESULT = 44
EXPECTED_FALLBACK_RESULT = 511785
EXPECTED_CATEGORY_PROBE_COUNT = 1
EXPECTED_PROTOCOL_PROBE_COUNT = 2


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
    SnippetCheck("M255-D004-DOC-EXP-01", "# M255 Protocol and Category-Aware Method Resolution Core Feature Expansion Expectations (D004)"),
    SnippetCheck("M255-D004-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-D004-DOC-EXP-03", f"`{CATEGORY_RESOLUTION_MODEL}`"),
    SnippetCheck("M255-D004-DOC-EXP-04", f"`{PROTOCOL_DECLARATION_MODEL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-D004-PKT-01", "# M255-D004 Protocol and Category-Aware Method Resolution Core Feature Expansion Packet"),
    SnippetCheck("M255-D004-PKT-02", "Packet: `M255-D004`"),
    SnippetCheck("M255-D004-PKT-03", f"`{METHOD_CACHE_CONTRACT_ID}`"),
    SnippetCheck("M255-D004-PKT-04", "category implementation records provide the next live dispatch tier"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-D004-NDOC-01", "## Protocol and category-aware method resolution (M255-D004)"),
    SnippetCheck("M255-D004-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-D004-NDOC-03", f"`{FALLBACK_MODEL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-D004-SPC-01", "## M255 protocol and category-aware method resolution (D004)"),
    SnippetCheck("M255-D004-SPC-02", f"`{CATEGORY_RESOLUTION_MODEL}`"),
    SnippetCheck("M255-D004-SPC-03", f"`{PROTOCOL_DECLARATION_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-D004-META-01", "## M255 protocol/category-aware resolution metadata anchors (D004)"),
    SnippetCheck("M255-D004-META-02", "`last_category_probe_count`"),
    SnippetCheck("M255-D004-META-03", "`last_protocol_probe_count`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M255-D004-ARCH-01", "## M255 protocol and category-aware method resolution (D004)"),
    SnippetCheck("M255-D004-ARCH-02", "preferred category implementation records provide the next live dispatch tier"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D004-RTDOC-01", "`M255-D004` then extends that same runtime surface across protocol/category"),
    SnippetCheck("M255-D004-RTDOC-02", f"`{PROTOCOL_DECLARATION_MODEL}`"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D004-TRTDOC-01", "`M255-D004` adds the protocol/category-aware proof surface:"),
    SnippetCheck("M255-D004-TRTDOC-02", "protocol-declared but body-less selectors stay negative while reporting"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-D004-LHC-01", "kObjc3RuntimeProtocolCategoryMethodResolutionContractId"),
    SnippetCheck("M255-D004-LHC-02", CATEGORY_RESOLUTION_MODEL),
    SnippetCheck("M255-D004-LHC-03", PROTOCOL_DECLARATION_MODEL),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M255-D004-IR-01", "runtime_protocol_category_method_resolution = contract="),
    SnippetCheck("M255-D004-IR-02", "M255-D003/M255-D004 slow-path anchor: class and category"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-D004-PAR-01", "M255-D004 protocol/category-aware resolution anchor"),
    SnippetCheck("M255-D004-PAR-02", "category-backed live lookup and protocol-backed negative lookup evidence"),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M255-D004-RTS-01", "M255-D004 protocol/category-aware resolution anchor"),
    SnippetCheck("M255-D004-RTS-02", "last_category_probe_count"),
    SnippetCheck("M255-D004-RTS-03", "last_protocol_probe_count"),
)
BOOTSTRAP_HEADER_SNIPPETS = (
    SnippetCheck("M255-D004-BH-01", "last_category_probe_count"),
    SnippetCheck("M255-D004-BH-02", "last_protocol_probe_count"),
    SnippetCheck("M255-D004-BH-03", "category_probe_count"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M255-D004-FIX-01", "module liveProtocolCategoryDispatch;"),
    SnippetCheck("M255-D004-FIX-02", "@interface Widget (Tracing)<TraceWorker>"),
    SnippetCheck("M255-D004-FIX-03", "- (i32) tracedValue {"),
    SnippetCheck("M255-D004-FIX-04", "+ (i32) tracerClassValue {"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M255-D004-PRB-01", '#include "runtime/objc3_runtime_bootstrap_internal.h"'),
    SnippetCheck("M255-D004-PRB-02", 'const char *const fallback_selector = "protocolDeclaredOnly";'),
    SnippetCheck("M255-D004-PRB-03", 'objc3_runtime_dispatch_i32(1025, "tracedValue", 0, 0, 0, 0)'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-D004-PKG-01", '"check:objc3c:m255-d004-protocol-and-category-aware-method-resolution-core-feature-expansion": "python scripts/check_m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion.py"'),
    SnippetCheck("M255-D004-PKG-02", '"test:tooling:m255-d004-protocol-and-category-aware-method-resolution-core-feature-expansion": "python -m pytest tests/tooling/test_check_m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion.py -q"'),
    SnippetCheck("M255-D004-PKG-03", '"check:objc3c:m255-d004-lane-d-readiness": "python scripts/run_m255_d004_lane_d_readiness.py"'),
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
        failures.append(Finding(display_path(path), "M255-D004-MISSING", f"required artifact is missing: {display_path(path)}"))
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
        r"Name:\s*" + re.escape(section_name) + r".*?RawDataSize:\s*(\d+).*?RelocationCount:\s*(\d+)",
        re.DOTALL,
    )
    match = pattern.search(readobj_text)
    if not match:
        return None
    return {"raw_data_size": int(match.group(1)), "relocation_count": int(match.group(2))}


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

    check(payload.get("instance_first") == EXPECTED_INSTANCE_RESULT, "M255-D004-PROBE-01", "instance dispatch must resolve live category method body")
    check(payload.get("instance_second") == EXPECTED_INSTANCE_RESULT, "M255-D004-PROBE-02", "repeat instance dispatch must preserve live category result")
    check(payload.get("class_self") == EXPECTED_CLASS_RESULT, "M255-D004-PROBE-03", "class-self dispatch must resolve live category class method")
    check(payload.get("known_class") == EXPECTED_CLASS_RESULT, "M255-D004-PROBE-04", "known-class dispatch must resolve the same category class method")
    check(payload.get("fallback_first") == EXPECTED_FALLBACK_RESULT, "M255-D004-PROBE-05", "protocol-declared unresolved selector must preserve fallback result")
    check(payload.get("fallback_second") == EXPECTED_FALLBACK_RESULT, "M255-D004-PROBE-06", "repeat unresolved selector must preserve fallback result")
    check(payload.get("fallback_expected") == EXPECTED_FALLBACK_RESULT, "M255-D004-PROBE-07", "fallback expectation must match compatibility dispatch")
    check(registration_state.get("registered_image_count") == 1, "M255-D004-PROBE-08", "runtime must register one image for the D004 fixture")
    check(registration_state.get("registered_descriptor_total") == 6, "M255-D004-PROBE-09", "runtime must report the expected descriptor total")
    check(selector_table_state.get("selector_table_entry_count") == 3, "M255-D004-PROBE-10", "selector table must expose the three D004 fixture selectors")
    check(selector_table_state.get("metadata_backed_selector_count") == 3, "M255-D004-PROBE-11", "selector table must remain metadata-backed for all fixture selectors")
    check(selector_table_state.get("dynamic_selector_count") == 0, "M255-D004-PROBE-12", "D004 probe must not require dynamic selector materialization")
    check(instance_first_state.get("cache_miss_count") == 1, "M255-D004-PROBE-13", "first instance dispatch must miss cache")
    check(instance_first_state.get("last_category_probe_count") == EXPECTED_CATEGORY_PROBE_COUNT, "M255-D004-PROBE-14", "instance live lookup must report one category probe")
    check(instance_first_state.get("last_protocol_probe_count") == 0, "M255-D004-PROBE-15", "instance live lookup must not need protocol negative probes")
    check(instance_first_state.get("last_dispatch_resolved_live_method") == 1, "M255-D004-PROBE-16", "instance dispatch must resolve a live method")
    check(instance_first_state.get("last_resolved_owner_identity") == "implementation:Widget(Tracing)::instance_method:tracedValue", "M255-D004-PROBE-17", "instance live lookup must resolve the category implementation owner")
    check(instance_second_state.get("cache_hit_count") == 1, "M255-D004-PROBE-18", "repeat instance dispatch must hit cache")
    check(instance_second_state.get("last_dispatch_used_cache") == 1, "M255-D004-PROBE-19", "repeat instance dispatch must report cache reuse")
    check(class_self_state.get("last_category_probe_count") == EXPECTED_CATEGORY_PROBE_COUNT, "M255-D004-PROBE-20", "class-self live lookup must report one category probe")
    check(class_self_state.get("last_protocol_probe_count") == 0, "M255-D004-PROBE-21", "class-self live lookup must not require protocol negative probes")
    check(class_self_state.get("last_resolved_owner_identity") == "implementation:Widget(Tracing)::class_method:tracerClassValue", "M255-D004-PROBE-22", "class-self lookup must resolve the category class-method owner")
    check(known_class_state.get("last_dispatch_used_cache") == 1, "M255-D004-PROBE-23", "known-class dispatch must reuse the normalized class cache entry")
    check(known_class_state.get("last_normalized_receiver_identity") == 1026, "M255-D004-PROBE-24", "known-class dispatch must normalize onto the metaclass identity")
    check(fallback_first_state.get("last_dispatch_resolved_live_method") == 0, "M255-D004-PROBE-25", "protocol-declared unresolved selector must not resolve a live body")
    check(fallback_first_state.get("last_dispatch_fell_back") == 1, "M255-D004-PROBE-26", "protocol-declared unresolved selector must fall back")
    check(fallback_first_state.get("last_category_probe_count") == EXPECTED_CATEGORY_PROBE_COUNT, "M255-D004-PROBE-27", "negative lookup must still report one category probe")
    check(fallback_first_state.get("last_protocol_probe_count") == EXPECTED_PROTOCOL_PROBE_COUNT, "M255-D004-PROBE-28", "negative lookup must report adopted/inherited protocol probes")
    check(fallback_second_state.get("last_dispatch_used_cache") == 1, "M255-D004-PROBE-29", "repeat negative lookup must reuse cached negative result")
    check(fallback_second_state.get("last_protocol_probe_count") == EXPECTED_PROTOCOL_PROBE_COUNT, "M255-D004-PROBE-30", "cached negative lookup must retain protocol probe evidence")
    check(instance_entry.get("found") == 1 and instance_entry.get("resolved") == 1, "M255-D004-PROBE-31", "instance cache entry must be a resolved positive entry")
    check(instance_entry.get("category_probe_count") == EXPECTED_CATEGORY_PROBE_COUNT, "M255-D004-PROBE-32", "instance cache entry must retain category probe count")
    check(instance_entry.get("protocol_probe_count") == 0, "M255-D004-PROBE-33", "instance cache entry must retain zero protocol probes")
    check(class_entry.get("found") == 1 and class_entry.get("resolved") == 1, "M255-D004-PROBE-34", "class cache entry must be a resolved positive entry")
    check(class_entry.get("category_probe_count") == EXPECTED_CATEGORY_PROBE_COUNT, "M255-D004-PROBE-35", "class cache entry must retain category probe count")
    check(fallback_entry.get("found") == 1 and fallback_entry.get("resolved") == 0, "M255-D004-PROBE-36", "negative cache entry must remain unresolved")
    check(fallback_entry.get("category_probe_count") == EXPECTED_CATEGORY_PROBE_COUNT, "M255-D004-PROBE-37", "negative cache entry must retain category probe count")
    check(fallback_entry.get("protocol_probe_count") == EXPECTED_PROTOCOL_PROBE_COUNT, "M255-D004-PROBE-38", "negative cache entry must retain protocol probe count")
    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M255-D004-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M255-D004-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M255-D004-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M255-D004-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")

    clangxx = resolve_tool(args.clangxx)
    llvm_readobj = resolve_tool(args.llvm_readobj)
    check(clangxx is not None, "M255-D004-DYN-05", f"unable to resolve {args.clangxx}")
    check(llvm_readobj is not None, "M255-D004-DYN-06", f"unable to resolve {args.llvm_readobj}")
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
    check(compile_result.returncode == 0, "M255-D004-DYN-07", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    if compile_result.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "compile_stdout": compile_result.stdout, "compile_stderr": compile_result.stderr}

    module_ir = args.probe_root / "module.ll"
    module_obj = args.probe_root / "module.obj"
    probe_exe = args.probe_root / "m255_d004_protocol_category_probe.exe"
    check(module_ir.exists(), "M255-D004-DYN-08", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M255-D004-DYN-09", f"missing emitted object: {display_path(module_obj)}")
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
    check(probe_compile.returncode == 0, "M255-D004-DYN-10", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_compile_stdout": probe_compile.stdout, "probe_compile_stderr": probe_compile.stderr}

    probe_run = run_command([str(probe_exe)])
    check(probe_run.returncode == 0, "M255-D004-DYN-11", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_stdout": probe_run.stdout, "probe_stderr": probe_run.stderr}

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M255-D004-DYN-12", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}
    checks_total += 1
    checks_passed += 1

    payload_passed, payload_total = probe_payload_checks(probe_payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total

    ir_text = read_text(module_ir)
    check(IR_COMMENT_PREFIX in ir_text, "M255-D004-DYN-13", "IR must publish the D004 contract comment")
    check(re.search(r"@__objc3_meta_category_instance_method_list_ref_[0-9]+ = private global \{ i64, ptr, ptr \} \{ i64 1, ptr @__objc3_meta_category_owner_identity_[0-9]+, ptr @__objc3_meta_category_instance_methods_[0-9]+ \}", ir_text) is not None, "M255-D004-DYN-14", "category instance method-list refs must carry list-local count 1")
    check(re.search(r"@__objc3_meta_category_class_method_list_ref_[0-9]+ = private global \{ i64, ptr, ptr \} \{ i64 1, ptr @__objc3_meta_category_owner_identity_[0-9]+, ptr @__objc3_meta_category_class_methods_[0-9]+ \}", ir_text) is not None, "M255-D004-DYN-15", "category class method-list refs must carry list-local count 1")
    check(re.search(r"@__objc3_meta_protocol_instance_method_list_ref_[0-9]+ = private global \{ i64, ptr, ptr \} \{ i64 1, ptr @__objc3_meta_protocol_owner_identity_[0-9]+, ptr @__objc3_meta_protocol_instance_methods_[0-9]+ \}", ir_text) is not None, "M255-D004-DYN-16", "protocol instance method-list refs must carry list-local count 1")
    check(re.search(r"@__objc3_meta_protocol_class_method_list_ref_[0-9]+ = private global \{ i64, ptr, ptr \} \{ i64 1, ptr @__objc3_meta_protocol_owner_identity_[0-9]+, ptr @__objc3_meta_protocol_class_methods_[0-9]+ \}", ir_text) is not None, "M255-D004-DYN-17", "protocol class method-list refs must carry list-local count 1 when class methods exist")
    check("ptr @objc3_method_Widget_instance_tracedValue" in ir_text, "M255-D004-DYN-18", "IR must carry the live category instance implementation pointer")
    check("ptr @objc3_method_Widget_class_tracerClassValue" in ir_text, "M255-D004-DYN-19", "IR must carry the live category class implementation pointer")

    readobj_result = run_command([str(llvm_readobj), "--sections", str(module_obj)])
    check(readobj_result.returncode == 0, "M255-D004-DYN-20", f"llvm-readobj failed: {readobj_result.stdout}{readobj_result.stderr}")
    if readobj_result.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "probe_payload": probe_payload}

    readobj_text = readobj_result.stdout
    protocol_metrics = parse_section_metrics(readobj_text, "objc3.runtime.protocol_descriptors")
    category_metrics = parse_section_metrics(readobj_text, "objc3.runtime.category_descriptors")
    check(protocol_metrics is not None, "M255-D004-DYN-21", "object must expose protocol_descriptors section")
    check(category_metrics is not None, "M255-D004-DYN-22", "object must expose category_descriptors section")
    if protocol_metrics is not None:
        check(protocol_metrics["raw_data_size"] > 0, "M255-D004-DYN-23", "protocol_descriptors section must carry payload bytes")
        check(protocol_metrics["relocation_count"] > 0, "M255-D004-DYN-24", "protocol_descriptors section must carry relocations")
    if category_metrics is not None:
        check(category_metrics["raw_data_size"] > 0, "M255-D004-DYN-25", "category_descriptors section must carry payload bytes")
        check(category_metrics["relocation_count"] > 0, "M255-D004-DYN-26", "category_descriptors section must carry relocations")

    return checks_passed, checks_total, {
        "skipped": False,
        "compile_stdout": compile_result.stdout,
        "compile_stderr": compile_result.stderr,
        "probe_payload": probe_payload,
        "protocol_section": protocol_metrics,
        "category_section": category_metrics,
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
        "method_cache_contract_id": METHOD_CACHE_CONTRACT_ID,
        "category_resolution_model": CATEGORY_RESOLUTION_MODEL,
        "protocol_declaration_model": PROTOCOL_DECLARATION_MODEL,
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
