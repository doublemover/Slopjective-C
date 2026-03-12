#!/usr/bin/env python3
"""Fail-closed checker for M262-D003 ARC debug instrumentation."""

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
CONTRACT_ID = "objc3c-runtime-arc-debug-instrumentation/m262-d003-v1"
MODE = "m262-d003-arc-debug-instrumentation-core-feature-expansion-v1"
BOUNDARY_PREFIX = "; runtime_arc_debug_instrumentation = contract=objc3c-runtime-arc-debug-instrumentation/m262-d003-v1"
NAMED_METADATA_LINE = "!objc3.objc_runtime_arc_debug_instrumentation = !{!85}"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion_d003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_NATIVE_DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
DEFAULT_RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
DEFAULT_BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_block_autorelease_return_positive.objc3"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m262_d003_arc_debug_instrumentation_probe.cpp"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "d003"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-D003" / "arc_debug_instrumentation_summary.json"


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
    SnippetCheck("M262-D003-DOC-EXP-01", "# M262 Ownership Debug Instrumentation And Runtime Validation Hooks For ARC Core Feature Expansion Expectations (D003)"),
    SnippetCheck("M262-D003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M262-D003-DOC-EXP-03", "`!objc3.objc_runtime_arc_debug_instrumentation = !{!85}`"),
    SnippetCheck("M262-D003-DOC-EXP-04", "`tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M262-D003-DOC-PKT-01", "# M262-D003 Ownership Debug Instrumentation And Runtime Validation Hooks For ARC Core Feature Expansion Packet"),
    SnippetCheck("M262-D003-DOC-PKT-02", "Packet: `M262-D003`"),
    SnippetCheck("M262-D003-DOC-PKT-03", "Issue: `#7205`"),
    SnippetCheck("M262-D003-DOC-PKT-04", "M262-D002"),
    SnippetCheck("M262-D003-DOC-PKT-05", "M262-E001"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M262-D003-ARCH-01", "## M262 ARC Ownership Debug Instrumentation And Runtime Validation Hooks (D003)"),
    SnippetCheck("M262-D003-ARCH-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D003-ARCH-03", "`!objc3.objc_runtime_arc_debug_instrumentation`"),
    SnippetCheck("M262-D003-ARCH-04", "the next issue is `M262-E001`"),
)
NATIVE_DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M262-D003-NSRC-01", "## M262 ARC ownership debug instrumentation and runtime validation hooks (M262-D003)"),
    SnippetCheck("M262-D003-NSRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D003-NSRC-03", "`!objc3.objc_runtime_arc_debug_instrumentation`"),
    SnippetCheck("M262-D003-NSRC-04", "`M262-E001` is the next issue."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M262-D003-NDOC-01", "## M262 ARC ownership debug instrumentation and runtime validation hooks (M262-D003)"),
    SnippetCheck("M262-D003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D003-NDOC-03", "`!objc3.objc_runtime_arc_debug_instrumentation`"),
    SnippetCheck("M262-D003-NDOC-04", "`M262-E001` is the next issue."),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M262-D003-SPC-01", "## M262 ARC ownership debug instrumentation and runtime validation hooks (D003)"),
    SnippetCheck("M262-D003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D003-SPC-03", "`!objc3.objc_runtime_arc_debug_instrumentation`"),
    SnippetCheck("M262-D003-SPC-04", "`M262-E001` is the next issue."),
)
SYNTAX_SPEC_SNIPPETS = (
    SnippetCheck("M262-D003-SYN-01", "`M262-D003` now layers private ARC debug counters"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M262-D003-PARSE-01", "M262-D003 ownership-debug/runtime-validation anchor:"),
)
SEMA_PM_SNIPPETS = (
    SnippetCheck("M262-D003-SEMA-01", "M262-D003 ownership-debug/runtime-validation anchor:"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M262-D003-DRV-01", "M262-D003 ownership-debug/runtime-validation anchor:"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M262-D003-LHDR-01", "kObjc3RuntimeArcDebugInstrumentationContractId"),
    SnippetCheck("M262-D003-LHDR-02", "kObjc3RuntimeArcDebugInstrumentationCoverageModel"),
    SnippetCheck("M262-D003-LHDR-03", "Objc3RuntimeArcDebugInstrumentationSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M262-D003-LCPP-01", "Objc3RuntimeArcDebugInstrumentationSummary()"),
    SnippetCheck("M262-D003-LCPP-02", "M262-D003 ownership-debug/runtime-validation anchor"),
    SnippetCheck("M262-D003-LCPP-03", ";next_issue=M262-E001"),
)
IR_SNIPPETS = (
    SnippetCheck("M262-D003-IR-01", 'out << "; runtime_arc_debug_instrumentation = "'),
    SnippetCheck("M262-D003-IR-02", '!objc3.objc_runtime_arc_debug_instrumentation = !{!85}'),
    SnippetCheck("M262-D003-IR-03", 'out << "!85 = !{!\\""'),
)
RUNTIME_INTERNAL_SNIPPETS = (
    SnippetCheck("M262-D003-RIH-01", "typedef struct objc3_runtime_arc_debug_state_snapshot {"),
    SnippetCheck("M262-D003-RIH-02", "last_property_owner_identity;"),
    SnippetCheck("M262-D003-RIH-03", "int objc3_runtime_copy_arc_debug_state_for_testing("),
)
RUNTIME_CPP_SNIPPETS = (
    SnippetCheck("M262-D003-RCP-01", "g_runtime_arc_debug_retain_call_count"),
    SnippetCheck("M262-D003-RCP-02", "RecordArcDebugPropertyContext"),
    SnippetCheck("M262-D003-RCP-03", "int objc3_runtime_copy_arc_debug_state_for_testing("),
)
RUNTIME_PROBE_SNIPPETS = (
    SnippetCheck("M262-D003-RTP-01", "objc3_runtime_arc_debug_state_snapshot inside{};"),
    SnippetCheck("M262-D003-RTP-02", "objc3_runtime_copy_arc_debug_state_for_testing(&inside);"),
    SnippetCheck("M262-D003-RTP-03", "\"last_property_name\""),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M262-D003-PKG-01", '"check:objc3c:m262-d003-arc-debug-instrumentation-contract": "python scripts/check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py"'),
    SnippetCheck("M262-D003-PKG-02", '"test:tooling:m262-d003-arc-debug-instrumentation-contract": "python -m pytest tests/tooling/test_check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py -q"'),
    SnippetCheck("M262-D003-PKG-03", '"check:objc3c:m262-d003-lane-d-readiness": "python scripts/run_m262_d003_lane_d_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--native-doc-source", type=Path, default=DEFAULT_NATIVE_DOC_SOURCE)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--syntax-spec", type=Path, default=DEFAULT_SYNTAX_SPEC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-pass-manager", type=Path, default=DEFAULT_SEMA_PASS_MANAGER)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--runtime-internal-header", type=Path, default=DEFAULT_RUNTIME_INTERNAL_HEADER)
    parser.add_argument("--runtime-cpp", type=Path, default=DEFAULT_RUNTIME_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--build-helper", type=Path, default=DEFAULT_BUILD_HELPER)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--property-fixture", type=Path, default=DEFAULT_PROPERTY_FIXTURE)
    parser.add_argument("--block-fixture", type=Path, default=DEFAULT_BLOCK_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> Any:
    return json.loads(read_text(path))


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


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def resolve_clangxx() -> str:
    candidates = (
        shutil.which("clang++"),
        shutil.which("clang++.exe"),
        r"C:\Program Files\LLVM\bin\clang++.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang++"


def resolve_clangcl() -> str:
    candidates = (
        shutil.which("clang-cl"),
        shutil.which("clang-cl.exe"),
        r"C:\Program Files\LLVM\bin\clang-cl.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang-cl"


def ensure_native_build(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    summary_path = args.summary_out.parent / "ensure_objc3c_native_build_summary.json"
    completed = run_command(
        [
            sys.executable,
            str(args.build_helper),
            "--mode",
            "fast",
            "--reason",
            "m262-d003-dynamic-check",
            "--summary-out",
            str(summary_path),
        ]
    )
    total = 1
    passed = require(completed.returncode == 0, display_path(args.build_helper), "M262-D003-DYN-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
    payload: dict[str, Any] = {
        "build_helper_returncode": completed.returncode,
        "build_helper_summary": display_path(summary_path),
    }
    if summary_path.exists():
        payload["build_helper_payload"] = load_json(summary_path)
    return passed, payload


def compile_fixture(native_exe: Path, fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(native_exe), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", *extra_args])


def boundary_line(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(BOUNDARY_PREFIX):
            return line
    return ""


def check_property_fixture(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    out_dir = args.probe_root / "property"
    completed = compile_fixture(args.native_exe, args.property_fixture, out_dir, ["-fobjc-arc"])
    case: dict[str, Any] = {
        "fixture": display_path(args.property_fixture),
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    ir_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    obj_path = out_dir / "module.obj"
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(args.property_fixture), "M262-D003-PROP-01", f"property fixture compile failed: {completed.stdout}{completed.stderr}", failures)
    if completed.returncode != 0:
        return checks_total, checks_passed, case
    checks_total += 4
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-D003-PROP-02", "property IR output missing", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M262-D003-PROP-03", "property manifest missing", failures)
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-D003-PROP-04", "property object output missing", failures)
    if not (ir_path.exists() and manifest_path.exists() and obj_path.exists()):
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    manifest = load_json(manifest_path)
    case["boundary_line"] = boundary_line(ir_text)
    checks_total += 4
    checks_passed += require(case["boundary_line"] != "", display_path(ir_path), "M262-D003-PROP-05", "property IR missing D003 boundary line", failures)
    checks_passed += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path), "M262-D003-PROP-06", "property IR missing D003 named metadata", failures)
    checks_passed += require("call i32 @objc3_runtime_load_weak_current_property_i32" in ir_text, display_path(ir_path), "M262-D003-PROP-07", "property IR missing weak load helper call", failures)
    checks_passed += require("call void @objc3_runtime_store_weak_current_property_i32" in ir_text, display_path(ir_path), "M262-D003-PROP-08", "property IR missing weak store helper call", failures)
    property_records = list(manifest.get("runtime_metadata_source_records", {}).get("properties", []))
    current_value = next((record for record in property_records if record.get("property_name") == "currentValue"), None)
    weak_value = next((record for record in property_records if record.get("property_name") == "weakValue"), None)
    checks_total += 2
    checks_passed += require(current_value is not None and current_value.get("ownership_lifetime_profile") == "strong-owned", display_path(manifest_path), "M262-D003-PROP-09", "currentValue ownership profile drifted", failures)
    checks_passed += require(weak_value is not None and weak_value.get("ownership_lifetime_profile") == "weak", display_path(manifest_path), "M262-D003-PROP-10", "weakValue ownership profile drifted", failures)
    return checks_total, checks_passed, case


def check_block_fixture(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    out_dir = args.probe_root / "block"
    completed = compile_fixture(args.native_exe, args.block_fixture, out_dir, ["-fobjc-arc"])
    case: dict[str, Any] = {
        "fixture": display_path(args.block_fixture),
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(args.block_fixture), "M262-D003-BLOCK-01", f"block fixture compile failed: {completed.stdout}{completed.stderr}", failures)
    if completed.returncode != 0:
        return checks_total, checks_passed, case
    checks_total += 2
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-D003-BLOCK-02", "block IR output missing", failures)
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-D003-BLOCK-03", "block object output missing", failures)
    if not (ir_path.exists() and obj_path.exists()):
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    case["boundary_line"] = boundary_line(ir_text)
    checks_total += 3
    checks_passed += require(case["boundary_line"] != "", display_path(ir_path), "M262-D003-BLOCK-04", "block IR missing D003 boundary line", failures)
    checks_passed += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path), "M262-D003-BLOCK-05", "block IR missing D003 named metadata", failures)
    checks_passed += require("call i32 @objc3_runtime_autorelease_i32" in ir_text, display_path(ir_path), "M262-D003-BLOCK-06", "block IR missing autorelease helper call", failures)
    return checks_total, checks_passed, case


def run_runtime_probe(args: argparse.Namespace, property_object_path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    out_dir = args.probe_root / "runtime-probe"
    out_dir.mkdir(parents=True, exist_ok=True)
    exe_path = out_dir / "m262_d003_arc_debug_instrumentation_probe.exe"
    command = [
        resolve_clangcl(),
        "/std:c++20",
        "/D_ITERATOR_DEBUG_LEVEL=0",
        "/MD",
        "/I",
        str(args.runtime_include_root),
        str(args.runtime_probe),
        str(property_object_path),
        str(args.runtime_library),
        "/link",
        "/DEFAULTLIB:ucrt",
        "/DEFAULTLIB:vcruntime",
        "/DEFAULTLIB:msvcrt",
        "/NODEFAULTLIB:msvcrtd",
        "/NODEFAULTLIB:libcmt",
        f"/OUT:{exe_path}",
    ]
    compile_result = run_command(command, cwd=out_dir)
    case: dict[str, Any] = {
        "probe_source": display_path(args.runtime_probe),
        "property_object": display_path(property_object_path),
        "runtime_library": display_path(args.runtime_library),
        "compile_returncode": compile_result.returncode,
        "compile_stdout": compile_result.stdout,
        "compile_stderr": compile_result.stderr,
        "exe_path": display_path(exe_path),
    }
    checks_total += 3
    checks_passed += require(args.runtime_probe.exists(), display_path(args.runtime_probe), "M262-D003-RTP-04", "runtime probe source is missing", failures)
    checks_passed += require(property_object_path.exists(), display_path(property_object_path), "M262-D003-RTP-04A", "property object for runtime probe is missing", failures)
    checks_passed += require(args.runtime_library.exists(), display_path(args.runtime_library), "M262-D003-RTP-05", "runtime library is missing", failures)
    checks_passed += require(compile_result.returncode == 0, display_path(exe_path), "M262-D003-RTP-06", f"runtime probe compile failed: {compile_result.stdout}{compile_result.stderr}", failures)
    if compile_result.returncode != 0:
        return checks_total, checks_passed, case
    run_result = run_command([str(exe_path)], cwd=out_dir)
    case["run_returncode"] = run_result.returncode
    case["run_stdout"] = run_result.stdout
    case["run_stderr"] = run_result.stderr
    checks_total += 1
    checks_passed += require(run_result.returncode == 0, display_path(exe_path), "M262-D003-RTP-07", f"runtime probe execution failed: {run_result.stdout}{run_result.stderr}", failures)
    if run_result.returncode != 0:
        return checks_total, checks_passed, case
    try:
        payload = json.loads(run_result.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(exe_path), "M262-D003-RTP-08", f"runtime probe output is not valid JSON: {exc}"))
        return checks_total + 1, checks_passed, case
    case["payload"] = payload
    inside = payload.get("inside", {})
    after = payload.get("after", {})
    checks = (
        ("M262-D003-RTP-09", payload.get("retained") == 9, "retained value drifted"),
        ("M262-D003-RTP-10", payload.get("autoreleased") == 9, "autoreleased value drifted"),
        ("M262-D003-RTP-11", payload.get("released") == 9, "released value drifted"),
        ("M262-D003-RTP-12", int(inside.get("current_property_exchange_count", 0)) == 2, "inside snapshot must observe two current-property exchange calls"),
        ("M262-D003-RTP-13", int(inside.get("current_property_write_count", 0)) == 1, "inside snapshot must observe one current-property write call"),
        ("M262-D003-RTP-14", int(inside.get("current_property_read_count", 0)) == 2, "inside snapshot must observe two current-property reads"),
        ("M262-D003-RTP-15", int(inside.get("weak_current_property_store_count", 0)) == 1, "inside snapshot must observe one weak-property store"),
        ("M262-D003-RTP-16", int(inside.get("weak_current_property_load_count", 0)) == 1, "inside snapshot must observe one weak-property load before pool pop"),
        ("M262-D003-RTP-17", int(after.get("weak_current_property_load_count", 0)) == 2, "after snapshot must observe two weak-property loads"),
        ("M262-D003-RTP-18", int(after.get("autoreleasepool_push_count", 0)) == 1, "after snapshot must observe one autoreleasepool push"),
        ("M262-D003-RTP-19", int(after.get("autoreleasepool_pop_count", 0)) == 1, "after snapshot must observe one autoreleasepool pop"),
        ("M262-D003-RTP-20", int(after.get("retain_call_count", 0)) >= 1, "after snapshot must observe retain helper traffic"),
        ("M262-D003-RTP-21", int(after.get("release_call_count", 0)) >= 1, "after snapshot must observe release helper traffic"),
        ("M262-D003-RTP-22", int(after.get("autorelease_call_count", 0)) >= 1, "after snapshot must observe autorelease helper traffic"),
        ("M262-D003-RTP-23", after.get("last_property_name") == "weakValue", "after snapshot must publish weakValue as the last property name"),
        ("M262-D003-RTP-24", bool(after.get("last_property_owner_identity")), "after snapshot must publish a last property owner identity"),
        ("M262-D003-RTP-25", int(after.get("last_property_read_value", -1)) == int(payload.get("weak_after_pool", -1)), "after snapshot must publish a last property-read value consistent with the final weak getter"),
        ("M262-D003-RTP-26", int(after.get("last_weak_loaded_value", -1)) == int(payload.get("weak_after_pool", -1)), "after snapshot must publish the final weak-loaded value consistently"),
    )
    for check_id, condition, detail in checks:
        checks_total += 1
        checks_passed += require(condition, display_path(exe_path), check_id, detail, failures)
    return checks_total, checks_passed, case


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc_source, NATIVE_DOC_SOURCE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.syntax_spec, SYNTAX_SPEC_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_pass_manager, SEMA_PM_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ir_emitter, IR_SNIPPETS),
        (args.runtime_internal_header, RUNTIME_INTERNAL_SNIPPETS),
        (args.runtime_cpp, RUNTIME_CPP_SNIPPETS),
        (args.runtime_probe, RUNTIME_PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        build_passed, build_case = ensure_native_build(args, failures)
        checks_total += 1
        checks_passed += build_passed
        dynamic_payload["build"] = build_case

        property_total, property_passed, property_case = check_property_fixture(args, failures)
        checks_total += property_total
        checks_passed += property_passed
        dynamic_payload["property_fixture"] = property_case

        block_total, block_passed, block_case = check_block_fixture(args, failures)
        checks_total += block_total
        checks_passed += block_passed
        dynamic_payload["block_fixture"] = block_case

        probe_total, probe_passed, probe_case = run_runtime_probe(
            args, args.probe_root / "property" / "module.obj", failures
        )
        checks_total += probe_total
        checks_passed += probe_passed
        dynamic_payload["runtime_probe"] = probe_case

    summary = {
        "contract_id": CONTRACT_ID,
        "mode": MODE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_payload": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"[fail] M262-D003 ARC debug instrumentation check failed ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        return 1

    print(f"[ok] M262-D003 ARC debug instrumentation validated ({checks_passed}/{checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
