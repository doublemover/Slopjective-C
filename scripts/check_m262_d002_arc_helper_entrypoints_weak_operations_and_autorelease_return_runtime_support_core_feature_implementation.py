#!/usr/bin/env python3
"""Fail-closed checker for M262-D002 ARC helper runtime support."""

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
CONTRACT_ID = "objc3c-runtime-arc-helper-runtime-support/m262-d002-v1"
MODE = "m262-d002-arc-helper-runtime-support-core-feature-implementation-v1"
BOUNDARY_PREFIX = "; runtime_arc_helper_runtime_support = contract=objc3c-runtime-arc-helper-runtime-support/m262-d002-v1"
NAMED_METADATA_LINE = "!objc3.objc_runtime_arc_helper_runtime_support = !{!84}"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation_d002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation_packet.md"
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
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEFAULT_PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
DEFAULT_BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_block_autorelease_return_positive.objc3"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m262_d002_arc_helper_runtime_support_probe.cpp"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "d002"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-D002" / "arc_helper_runtime_support_summary.json"


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
    SnippetCheck("M262-D002-DOC-EXP-01", "# M262 ARC Helper Entrypoints, Weak Operations, And Autorelease-Return Runtime Support Core Feature Implementation Expectations (D002)"),
    SnippetCheck("M262-D002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M262-D002-DOC-EXP-03", "`!objc3.objc_runtime_arc_helper_runtime_support = !{!84}`"),
    SnippetCheck("M262-D002-DOC-EXP-04", "`tests/tooling/runtime/m262_d002_arc_helper_runtime_support_probe.cpp`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M262-D002-DOC-PKT-01", "# M262-D002 ARC Helper Entrypoints, Weak Operations, And Autorelease-Return Runtime Support Core Feature Implementation Packet"),
    SnippetCheck("M262-D002-DOC-PKT-02", "Packet: `M262-D002`"),
    SnippetCheck("M262-D002-DOC-PKT-03", "Issue: `#7204`"),
    SnippetCheck("M262-D002-DOC-PKT-04", "M260-D002"),
    SnippetCheck("M262-D002-DOC-PKT-05", "M262-D003"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M262-D002-ARCH-01", "## M262 Runtime ARC Helper Runtime Support (D002)"),
    SnippetCheck("M262-D002-ARCH-02", "`objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`"),
    SnippetCheck("M262-D002-ARCH-03", "`!objc3.objc_runtime_arc_helper_runtime_support`"),
    SnippetCheck("M262-D002-ARCH-04", "the next issue is `M262-D003`"),
)
NATIVE_DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M262-D002-NSRC-01", "## M262 runtime ARC helper runtime support (M262-D002)"),
    SnippetCheck("M262-D002-NSRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D002-NSRC-03", "`objc3_runtime_load_weak_current_property_i32`"),
    SnippetCheck("M262-D002-NSRC-04", "`M262-D003` is the next issue."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M262-D002-NDOC-01", "## M262 runtime ARC helper runtime support (M262-D002)"),
    SnippetCheck("M262-D002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D002-NDOC-03", "`!objc3.objc_runtime_arc_helper_runtime_support`"),
    SnippetCheck("M262-D002-NDOC-04", "`M262-D003` is the next issue."),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M262-D002-SPC-01", "## M262 runtime ARC helper runtime support (D002)"),
    SnippetCheck("M262-D002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-D002-SPC-03", "`!objc3.objc_runtime_arc_helper_runtime_support`"),
    SnippetCheck("M262-D002-SPC-04", "`M262-D003` is the next issue."),
)
SYNTAX_SPEC_SNIPPETS = (
    SnippetCheck("M262-D002-SYN-01", "Current implementation status (`M262-D002`):"),
    SnippetCheck("M262-D002-SYN-02", "`objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`"),
    SnippetCheck("M262-D002-SYN-03", "linked native ARC programs"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M262-D002-PARSE-01", "M262-D002 runtime ARC helper implementation anchor:"),
)
SEMA_PM_SNIPPETS = (
    SnippetCheck("M262-D002-SEMA-01", "M262-D002 runtime ARC helper implementation anchor:"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M262-D002-DRV-01", "M262-D002 runtime ARC helper implementation anchor:"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M262-D002-LHDR-01", "kObjc3RuntimeArcHelperRuntimeSupportContractId"),
    SnippetCheck("M262-D002-LHDR-02", "kObjc3RuntimeArcHelperRuntimeSupportDependencyModel"),
    SnippetCheck("M262-D002-LHDR-03", "kObjc3RuntimeArcHelperRuntimeSupportWeakModel"),
    SnippetCheck("M262-D002-LHDR-04", "Objc3RuntimeArcHelperRuntimeSupportSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M262-D002-LCPP-01", "Objc3RuntimeArcHelperRuntimeSupportSummary()"),
    SnippetCheck("M262-D002-LCPP-02", "M262-D002 runtime ARC helper implementation anchor"),
    SnippetCheck("M262-D002-LCPP-03", ";next_issue=M262-D003"),
)
IR_SNIPPETS = (
    SnippetCheck("M262-D002-IR-01", 'out << "; runtime_arc_helper_runtime_support = "'),
    SnippetCheck("M262-D002-IR-02", '!objc3.objc_runtime_arc_helper_runtime_support = !{!84}'),
    SnippetCheck("M262-D002-IR-03", 'out << "!84 = !{!\\""'),
)
RUNTIME_INTERNAL_SNIPPETS = (
    SnippetCheck("M262-D002-RIH-01", "M262-D002 runtime ARC helper implementation anchor:"),
    SnippetCheck("M262-D002-RIH-02", "int objc3_runtime_load_weak_current_property_i32(void);"),
    SnippetCheck("M262-D002-RIH-03", "int objc3_runtime_autorelease_i32(int value);"),
)
RUNTIME_CPP_SNIPPETS = (
    SnippetCheck("M262-D002-RCP-01", "M262-D002 runtime ARC helper implementation anchor:"),
    SnippetCheck("M262-D002-RCP-02", "extern \"C\" int objc3_runtime_load_weak_current_property_i32(void)"),
    SnippetCheck("M262-D002-RCP-03", "extern \"C\" int objc3_runtime_autorelease_i32(int value)"),
)
RUNTIME_PROBE_SNIPPETS = (
    SnippetCheck("M262-D002-RTP-01", "#include \"runtime/objc3_runtime.h\""),
    SnippetCheck("M262-D002-RTP-02", "#include \"runtime/objc3_runtime_bootstrap_internal.h\""),
    SnippetCheck("M262-D002-RTP-03", "inside_queue_count"),
    SnippetCheck("M262-D002-RTP-04", "objc3_runtime_autorelease_i32(retained)"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M262-D002-PKG-01", '"check:objc3c:m262-d002-runtime-arc-helper-support-contract": "python scripts/check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py"'),
    SnippetCheck("M262-D002-PKG-02", '"test:tooling:m262-d002-runtime-arc-helper-support-contract": "python -m pytest tests/tooling/test_check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py -q"'),
    SnippetCheck("M262-D002-PKG-03", '"check:objc3c:m262-d002-lane-d-readiness": "python scripts/run_m262_d002_lane_d_readiness.py"'),
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
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--build-helper", type=Path, default=DEFAULT_BUILD_HELPER)
    parser.add_argument("--property-fixture", type=Path, default=DEFAULT_PROPERTY_FIXTURE)
    parser.add_argument("--block-fixture", type=Path, default=DEFAULT_BLOCK_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
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


def resolve_clang() -> str:
    candidates = (
        shutil.which("clang"),
        shutil.which("clang.exe"),
        r"C:\Program Files\LLVM\bin\clang.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang"

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


def ensure_native_build(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    summary_path = args.summary_out.parent / "ensure_objc3c_native_build_summary.json"
    completed = run_command(
        [
            sys.executable,
            str(args.build_helper),
            "--mode",
            "fast",
            "--reason",
            "m262-d002-dynamic-check",
            "--summary-out",
            str(summary_path),
        ]
    )
    total = 1
    passed = require(completed.returncode == 0, display_path(args.build_helper), "M262-D002-DYN-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
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


def extract_property_records(manifest_payload: dict[str, Any]) -> list[dict[str, Any]]:
    return list(manifest_payload.get("runtime_metadata_source_records", {}).get("properties", []))


def find_property_record(records: list[dict[str, Any]], owner_kind: str, property_name: str) -> dict[str, Any] | None:
    for record in records:
        if record.get("owner_kind") == owner_kind and record.get("property_name") == property_name:
            return record
    return None


def resolve_runtime_library(registration_manifest_path: Path) -> Path | None:
    payload = load_json(registration_manifest_path)
    runtime_library_relative_path = payload.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None
    return runtime_library_path


def run_property_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    out_dir = args.probe_root / "property"
    completed = compile_fixture(args.native_exe, args.property_fixture, out_dir, ["-fobjc-arc"])
    manifest_path = out_dir / "module.manifest.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    case: dict[str, Any] = {
        "fixture": display_path(args.property_fixture),
        "out_dir": display_path(out_dir),
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    for check_id, path, detail in (
        ("M262-D002-PROP-01", out_dir, "property fixture compile failed"),
        ("M262-D002-PROP-02", manifest_path, "property manifest missing"),
        ("M262-D002-PROP-03", diagnostics_path, "property diagnostics missing"),
        ("M262-D002-PROP-04", ir_path, "property IR missing"),
        ("M262-D002-PROP-05", obj_path, "property object missing"),
    ):
        checks_total += 1
        if path == out_dir:
            checks_passed += require(completed.returncode == 0, display_path(out_dir), check_id, f"{detail}: {completed.stdout}{completed.stderr}", failures)
        else:
            checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if completed.returncode != 0 or not manifest_path.exists() or not diagnostics_path.exists() or not ir_path.exists() or not obj_path.exists():
        return checks_total, checks_passed, case

    diagnostics = load_json(diagnostics_path)
    manifest = load_json(manifest_path)
    ir_text = read_text(ir_path)
    case["boundary"] = boundary_line(ir_text)
    case["manifest_path"] = display_path(manifest_path)
    case["ir_path"] = display_path(ir_path)
    case["obj_path"] = display_path(obj_path)
    checks_total += 1
    checks_passed += require(not diagnostics.get("diagnostics"), display_path(diagnostics_path), "M262-D002-PROP-06", "property diagnostics must be empty", failures)
    checks_total += 1
    checks_passed += require(bool(case["boundary"]), display_path(ir_path), "M262-D002-PROP-07", "property IR must publish the D002 boundary line", failures)
    checks_total += 1
    checks_passed += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path), "M262-D002-PROP-08", "property IR must publish the D002 named metadata", failures)
    checks_total += 1
    checks_passed += require("call i32 @objc3_runtime_load_weak_current_property_i32()" in ir_text, display_path(ir_path), "M262-D002-PROP-09", "property IR must call the weak load helper", failures)
    checks_total += 1
    checks_passed += require("call void @objc3_runtime_store_weak_current_property_i32(i32 " in ir_text, display_path(ir_path), "M262-D002-PROP-10", "property IR must call the weak store helper", failures)
    checks_total += 1
    checks_passed += require("call i32 @objc3_runtime_retain_i32(" in ir_text, display_path(ir_path), "M262-D002-PROP-11", "property IR must retain strong property values", failures)
    checks_total += 1
    checks_passed += require("call i32 @objc3_runtime_release_i32(" in ir_text, display_path(ir_path), "M262-D002-PROP-12", "property IR must release replaced strong property values", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0, display_path(obj_path), "M262-D002-PROP-13", "property object output must be non-empty", failures)

    property_records = extract_property_records(manifest)
    current_value = find_property_record(property_records, "class-interface", "currentValue")
    weak_value = find_property_record(property_records, "class-interface", "weakValue")
    case["currentValue"] = current_value
    case["weakValue"] = weak_value
    checks_total += 1
    checks_passed += require(current_value is not None, display_path(manifest_path), "M262-D002-PROP-14", "currentValue property record missing", failures)
    checks_total += 1
    checks_passed += require(weak_value is not None, display_path(manifest_path), "M262-D002-PROP-15", "weakValue property record missing", failures)
    if current_value is not None:
        checks_total += 1
        checks_passed += require(current_value.get("ownership_lifetime_profile") == "strong-owned", display_path(manifest_path), "M262-D002-PROP-16", "currentValue lifetime must remain strong-owned", failures)
        checks_total += 1
        checks_passed += require("ownership_lifetime=strong-owned" in str(current_value.get("accessor_ownership_profile", "")), display_path(manifest_path), "M262-D002-PROP-17", "currentValue accessor ownership profile must publish strong-owned", failures)
    if weak_value is not None:
        checks_total += 1
        checks_passed += require(weak_value.get("ownership_lifetime_profile") == "weak", display_path(manifest_path), "M262-D002-PROP-18", "weakValue lifetime must remain weak", failures)
        checks_total += 1
        checks_passed += require(weak_value.get("ownership_runtime_hook_profile") == "objc-weak-side-table", display_path(manifest_path), "M262-D002-PROP-19", "weakValue runtime hook must remain objc-weak-side-table", failures)
    return checks_total, checks_passed, case


def run_block_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    out_dir = args.probe_root / "block-autorelease"
    completed = compile_fixture(args.native_exe, args.block_fixture, out_dir, ["-fobjc-arc"])
    manifest_path = out_dir / "module.manifest.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    case: dict[str, Any] = {
        "fixture": display_path(args.block_fixture),
        "out_dir": display_path(out_dir),
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    for check_id, path, detail in (
        ("M262-D002-BLOCK-01", out_dir, "block fixture compile failed"),
        ("M262-D002-BLOCK-02", manifest_path, "block manifest missing"),
        ("M262-D002-BLOCK-03", diagnostics_path, "block diagnostics missing"),
        ("M262-D002-BLOCK-04", ir_path, "block IR missing"),
        ("M262-D002-BLOCK-05", obj_path, "block object missing"),
        ("M262-D002-BLOCK-06", registration_manifest_path, "block runtime registration manifest missing"),
    ):
        checks_total += 1
        if path == out_dir:
            checks_passed += require(completed.returncode == 0, display_path(out_dir), check_id, f"{detail}: {completed.stdout}{completed.stderr}", failures)
        else:
            checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if completed.returncode != 0 or not manifest_path.exists() or not diagnostics_path.exists() or not ir_path.exists() or not obj_path.exists() or not registration_manifest_path.exists():
        return checks_total, checks_passed, case

    diagnostics = load_json(diagnostics_path)
    ir_text = read_text(ir_path)
    case["boundary"] = boundary_line(ir_text)
    checks_total += 1
    checks_passed += require(not diagnostics.get("diagnostics"), display_path(diagnostics_path), "M262-D002-BLOCK-07", "block diagnostics must be empty", failures)
    checks_total += 1
    checks_passed += require(bool(case["boundary"]), display_path(ir_path), "M262-D002-BLOCK-08", "block IR must publish the D002 boundary line", failures)
    checks_total += 1
    checks_passed += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path), "M262-D002-BLOCK-09", "block IR must publish the D002 named metadata", failures)
    checks_total += 1
    checks_passed += require("call i32 @objc3_runtime_autorelease_i32" in ir_text, display_path(ir_path), "M262-D002-BLOCK-10", "block IR must call the autorelease helper", failures)
    checks_total += 1
    checks_passed += require("call i32 @objc3_runtime_promote_block_i32" in ir_text, display_path(ir_path), "M262-D002-BLOCK-11", "block IR must promote escaping blocks through the runtime hook", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0, display_path(obj_path), "M262-D002-BLOCK-12", "block object output must be non-empty", failures)

    runtime_library_path = resolve_runtime_library(registration_manifest_path)
    checks_total += 1
    checks_passed += require(runtime_library_path is not None, display_path(registration_manifest_path), "M262-D002-BLOCK-13", "runtime registration manifest must resolve a runtime library path", failures)
    case["runtime_library_path"] = display_path(runtime_library_path) if runtime_library_path is not None else None
    return checks_total, checks_passed, case


def run_runtime_probe(args: argparse.Namespace, runtime_library_path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    out_dir = args.probe_root / "runtime-probe"
    out_dir.mkdir(parents=True, exist_ok=True)
    exe_path = out_dir / "m262_d002_arc_helper_runtime_support_probe.exe"
    command = [
        resolve_clangxx(),
        "-std=c++20",
        "-I",
        str(args.runtime_include_root),
        str(args.runtime_probe),
        str(runtime_library_path),
        "-Xlinker",
        "/DEFAULTLIB:ucrtd",
        "-Xlinker",
        "/DEFAULTLIB:vcruntimed",
        "-Xlinker",
        "/DEFAULTLIB:msvcrtd",
        "-Xlinker",
        "/NODEFAULTLIB:msvcrt",
        "-Xlinker",
        "/NODEFAULTLIB:libcmt",
        "-o",
        str(exe_path),
    ]
    compile_result = run_command(command, cwd=out_dir)
    case: dict[str, Any] = {
        "probe_source": display_path(args.runtime_probe),
        "runtime_library_path": display_path(runtime_library_path),
        "compile_returncode": compile_result.returncode,
        "compile_stdout": compile_result.stdout,
        "compile_stderr": compile_result.stderr,
        "exe_path": display_path(exe_path),
    }
    checks_total += 1
    checks_passed += require(args.runtime_probe.exists(), display_path(args.runtime_probe), "M262-D002-RTP-01", "runtime probe source is missing", failures)
    checks_total += 1
    checks_passed += require(args.runtime_include_root.exists(), display_path(args.runtime_include_root), "M262-D002-RTP-02", "runtime include root is missing", failures)
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(exe_path), "M262-D002-RTP-03", f"runtime probe compile failed: {compile_result.stdout}{compile_result.stderr}", failures)
    checks_total += 1
    checks_passed += require(exe_path.exists(), display_path(exe_path), "M262-D002-RTP-04", "runtime probe executable missing", failures)
    if compile_result.returncode != 0 or not exe_path.exists():
        return checks_total, checks_passed, case

    run_result = run_command([str(exe_path)], cwd=out_dir)
    case["run_returncode"] = run_result.returncode
    case["run_stdout"] = run_result.stdout
    case["run_stderr"] = run_result.stderr
    checks_total += 1
    checks_passed += require(run_result.returncode == 0, display_path(exe_path), "M262-D002-RTP-05", f"runtime probe execution failed: {run_result.stdout}{run_result.stderr}", failures)
    if run_result.returncode != 0:
        return checks_total, checks_passed, case

    try:
        payload = json.loads(run_result.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(exe_path), "M262-D002-RTP-06", f"runtime probe output is not valid JSON: {exc}"))
        return checks_total + 1, checks_passed, case

    case["payload"] = payload
    for check_id, key, expected in (
        ("M262-D002-RTP-07", "retained", 9),
        ("M262-D002-RTP-08", "autoreleased", 9),
        ("M262-D002-RTP-09", "released", 9),
        ("M262-D002-RTP-10", "inside_last_autoreleased", 9),
        ("M262-D002-RTP-11", "after_last_drained", 9),
    ):
        checks_total += 1
        checks_passed += require(payload.get(key) == expected, display_path(exe_path), check_id, f"runtime probe field {key!r} expected {expected}, got {payload.get(key)!r}", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("inside_depth", 0)) >= 1, display_path(exe_path), "M262-D002-RTP-12", "runtime probe must observe autoreleasepool depth inside the scope", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("inside_queue_count", 0)) >= 1, display_path(exe_path), "M262-D002-RTP-13", "runtime probe must observe queued autorelease values inside the scope", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("after_depth", -1)) == 0, display_path(exe_path), "M262-D002-RTP-14", "runtime probe must observe zero autoreleasepool depth after pop", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("after_drained_count", 0)) >= 1, display_path(exe_path), "M262-D002-RTP-15", "runtime probe must observe drained autorelease values after pop", failures)
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
        build_passed, build_payload = ensure_native_build(args, failures)
        checks_total += 1
        checks_passed += build_passed
        dynamic_payload["build"] = build_payload
        if not failures:
            property_total, property_passed, property_case = run_property_case(args, failures)
            block_total, block_passed, block_case = run_block_case(args, failures)
            checks_total += property_total + block_total
            checks_passed += property_passed + block_passed
            dynamic_payload["property_case"] = property_case
            dynamic_payload["block_case"] = block_case
            runtime_library_path = resolve_runtime_library(args.probe_root / "block-autorelease" / "module.runtime-registration-manifest.json")
            if runtime_library_path is not None:
                probe_total, probe_passed, probe_case = run_runtime_probe(args, runtime_library_path, failures)
                checks_total += probe_total
                checks_passed += probe_passed
                dynamic_payload["runtime_probe"] = probe_case

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "issue": "M262-D002",
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M262-D002 ARC helper runtime support satisfied")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
