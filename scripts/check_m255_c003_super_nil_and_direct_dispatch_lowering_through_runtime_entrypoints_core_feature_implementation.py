#!/usr/bin/env python3
"""Fail-closed checker for M255-C003 super/nil/direct runtime dispatch cutover."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-c003-super-nil-and-direct-dispatch-lowering-through-runtime-entrypoints-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-call-abi-super-nil-direct-dispatch/m255-c003-v1"
PREVIOUS_CONTRACT_ID = "objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1"
ACTIVE_MODEL = "instance-class-super-and-nil-sends-lower-directly-to-canonical-runtime-entrypoint"
DEFERRED_MODEL = "dynamic-sends-stay-on-compatibility-bridge-until-m255-c004"
UNSUPPORTED_MODEL = "direct-dispatch-fails-closed-until-supported-surface-materializes"
CANONICAL_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_SYMBOL = "objc3_msgsend_i32"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-C003" / "super_nil_direct_runtime_dispatch_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NIL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_nil_runtime_dispatch.objc3"
SUPER_DYNAMIC_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_dynamic_method_family_edges.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "c003-super-nil-direct-runtime-call-abi"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


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


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


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


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = json.loads(read_text(registration_manifest_path))
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    result = run_command(
        [resolve_clang(), str(obj_path), str(runtime_library_path), f"@{rsp_path.resolve()}", "-o", str(exe_path)],
        cwd=out_dir,
    )
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M255-C003-DOC-01", "# M255 Super, Nil, And Direct Dispatch Lowering Through Runtime Entrypoints Core Feature Implementation Expectations (C003)"),
    SnippetCheck("M255-C003-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C003-DOC-03", f"`{CANONICAL_SYMBOL}`"),
    SnippetCheck("M255-C003-DOC-04", f"`{UNSUPPORTED_MODEL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-C003-PKT-01", "# M255-C003 Super, Nil, And Direct Dispatch Lowering Through Runtime Entrypoints Core Feature Implementation Packet"),
    SnippetCheck("M255-C003-PKT-02", "Packet: `M255-C003`"),
    SnippetCheck("M255-C003-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C003-PKT-04", "Dynamic sends remain on `objc3_msgsend_i32` until `M255-C004`."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-C003-NDOC-01", "## Super, nil, and direct runtime call ABI cutover (M255-C003)"),
    SnippetCheck("M255-C003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C003-NDOC-03", "canonical nil-receiver sends stop compile-time eliding in IR"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-C003-SPC-01", "## M255 super, nil, and direct runtime call ABI cutover (C003)"),
    SnippetCheck("M255-C003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C003-SPC-03", "reserved direct-dispatch surfaces fail closed if they reach IR emission"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-C003-META-01", "## M255 super/nil runtime call ABI metadata anchors (C003)"),
    SnippetCheck("M255-C003-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C003-META-03", "normalized dynamic sends remain on `objc3_msgsend_i32` until `M255-C004`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-C003-HDR-01", "kObjc3RuntimeDispatchSuperNilContractId"),
    SnippetCheck("M255-C003-HDR-02", "kObjc3RuntimeDispatchSuperNilActiveLoweringModel"),
    SnippetCheck("M255-C003-HDR-03", "kObjc3RuntimeDispatchSuperNilUnsupportedFallbackModel"),
    SnippetCheck("M255-C003-HDR-04", "RequiresFailClosedObjc3RuntimeDispatchFallback("),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M255-C003-CPP-01", "dispatch_surface_family == kObjc3DispatchSurfaceSuperFamily"),
    SnippetCheck("M255-C003-CPP-02", "RequiresFailClosedObjc3RuntimeDispatchFallback("),
    SnippetCheck("M255-C003-CPP-03", "M255-C003 runtime call ABI generation anchor"),
)
IR_SNIPPETS = (
    SnippetCheck("M255-C003-IR-01", "uses_canonical_runtime_entrypoint"),
    SnippetCheck("M255-C003-IR-02", "direct dispatch lowering remains unsupported on the live runtime path"),
    SnippetCheck("M255-C003-IR-03", "nil semantics for canonical"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-C003-PARSE-01", "M255-C003 runtime call ABI generation anchor"),
    SnippetCheck("M255-C003-PARSE-02", "normalized super sends and nil-receiver canonical surfaces onto"),
)
RUNTIME_SNIPPETS = (
    SnippetCheck("M255-C003-RUN-01", "receiver == 0"),
    SnippetCheck("M255-C003-RUN-02", "owns nil-receiver semantics for lowered instance/class/super surfaces"),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-C003-SHIM-01", "M255-C003 runtime call ABI generation"),
    SnippetCheck("M255-C003-SHIM-02", "deferred dynamic sites"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M255-C003-FIX-01", "module nilRuntimeDispatch;"),
    SnippetCheck("M255-C003-FIX-02", "let sent = [nil ping];"),
    SnippetCheck("M255-C003-FIX-03", "return sent + 9;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-C003-PKG-01", '"check:objc3c:m255-c003-super-nil-and-direct-dispatch-lowering-through-runtime-entrypoints-core-feature-implementation": "python scripts/check_m255_c003_super_nil_and_direct_dispatch_lowering_through_runtime_entrypoints_core_feature_implementation.py"'),
    SnippetCheck("M255-C003-PKG-02", '"test:tooling:m255-c003-super-nil-and-direct-dispatch-lowering-through-runtime-entrypoints-core-feature-implementation": "python -m pytest tests/tooling/test_check_m255_c003_super_nil_and_direct_dispatch_lowering_through_runtime_entrypoints_core_feature_implementation.py -q"'),
    SnippetCheck("M255-C003-PKG-03", '"check:objc3c:m255-c003-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m255-c003-super-nil-and-direct-dispatch-lowering-through-runtime-entrypoints-core-feature-implementation && npm run test:tooling:m255-c003-super-nil-and-direct-dispatch-lowering-through-runtime-entrypoints-core-feature-implementation"'),
)


def run_nil_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "nil"
    result = compile_fixture(NIL_FIXTURE, out_dir)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    exe_path, link_result = link_executable(out_dir)
    case: dict[str, Any] = {
        "fixture": display_path(NIL_FIXTURE),
        "out_dir": display_path(out_dir),
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "object_exists": obj_path.exists(),
        "backend_exists": backend_path.exists(),
        "executable_path": display_path(exe_path) if exe_path is not None else None,
    }
    if link_result is not None:
        case["link_returncode"] = link_result.returncode
        case["link_stdout"] = link_result.stdout
        case["link_stderr"] = link_result.stderr

    checks_total = 0
    checks_passed = 0
    for check_id, condition, artifact, detail in (
        ("M255-C003-NIL-01", result.returncode == 0, display_path(out_dir), "nil native compile failed"),
        ("M255-C003-NIL-02", manifest_path.exists(), display_path(manifest_path), "manifest missing"),
        ("M255-C003-NIL-03", ir_path.exists(), display_path(ir_path), "IR missing"),
        ("M255-C003-NIL-04", obj_path.exists(), display_path(obj_path), "object missing"),
        ("M255-C003-NIL-05", backend_path.exists(), display_path(backend_path), "backend marker missing"),
        ("M255-C003-NIL-06", exe_path is not None, display_path(out_dir), "expected linked executable artifact"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or not backend_path.exists() or exe_path is None:
        return checks_total, checks_passed, case

    backend_text = read_text(backend_path).strip()
    case["object_backend"] = backend_text
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-C003-NIL-07", f"expected llvm-direct backend, got {backend_text!r}", failures)

    manifest = json.loads(read_text(manifest_path))
    lowering = manifest.get("lowering", {}) if isinstance(manifest, dict) else {}
    case["lowering_runtime_dispatch_symbol"] = lowering.get("runtime_dispatch_symbol") if isinstance(lowering, dict) else None
    checks_total += 1
    checks_passed += require(isinstance(lowering, dict) and lowering.get("runtime_dispatch_symbol") == COMPATIBILITY_SYMBOL, display_path(manifest_path), "M255-C003-NIL-08", "historical lowering boundary should remain compatibility bridge", failures)

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    checks_total += 1
    checks_passed += require(canonical_calls == 1, display_path(ir_path), "M255-C003-NIL-09", f"expected 1 canonical runtime dispatch call, got {canonical_calls}", failures)
    checks_total += 1
    checks_passed += require(compatibility_calls == 0, display_path(ir_path), "M255-C003-NIL-10", f"expected 0 compatibility dispatch calls, got {compatibility_calls}", failures)

    run_result = run_command([str(exe_path)], cwd=out_dir)
    case["program_exit_code"] = run_result.returncode
    case["expected_exit_code"] = 9
    case["program_stdout"] = run_result.stdout
    case["program_stderr"] = run_result.stderr
    checks_total += 1
    checks_passed += require(run_result.returncode == 9, display_path(exe_path), "M255-C003-NIL-11", f"expected exit 9, got {run_result.returncode}", failures)
    return checks_total, checks_passed, case


def run_super_dynamic_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "super-dynamic"
    result = compile_fixture(SUPER_DYNAMIC_FIXTURE, out_dir)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    backend_path = out_dir / "module.object-backend.txt"
    case: dict[str, Any] = {
        "fixture": display_path(SUPER_DYNAMIC_FIXTURE),
        "out_dir": display_path(out_dir),
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "backend_exists": backend_path.exists(),
    }
    checks_total = 0
    checks_passed = 0
    for check_id, condition, artifact, detail in (
        ("M255-C003-SUP-01", result.returncode == 0, display_path(out_dir), "super/dynamic native compile failed"),
        ("M255-C003-SUP-02", manifest_path.exists(), display_path(manifest_path), "manifest missing"),
        ("M255-C003-SUP-03", ir_path.exists(), display_path(ir_path), "IR missing"),
        ("M255-C003-SUP-04", backend_path.exists(), display_path(backend_path), "backend marker missing"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or not backend_path.exists():
        return checks_total, checks_passed, case

    backend_text = read_text(backend_path).strip()
    case["object_backend"] = backend_text
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-C003-SUP-05", f"expected llvm-direct backend, got {backend_text!r}", failures)

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    checks_total += 1
    checks_passed += require(canonical_calls == 4, display_path(ir_path), "M255-C003-SUP-06", f"expected 4 canonical super dispatch calls, got {canonical_calls}", failures)
    checks_total += 1
    checks_passed += require(compatibility_calls == 3, display_path(ir_path), "M255-C003-SUP-07", f"expected 3 compatibility dynamic dispatch calls, got {compatibility_calls}", failures)
    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    doc_checks: tuple[tuple[Path, str, tuple[SnippetCheck, ...]], ...] = (
        (ROOT / "docs" / "contracts" / "m255_super_nil_and_direct_dispatch_lowering_through_runtime_entrypoints_core_feature_implementation_c003_expectations.md", "M255-C003-DOC-EXISTS", EXPECTATIONS_SNIPPETS),
        (ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_c003_super_nil_and_direct_dispatch_lowering_through_runtime_entrypoints_core_feature_implementation_packet.md", "M255-C003-PKT-EXISTS", PACKET_SNIPPETS),
        (ROOT / "docs" / "objc3c-native.md", "M255-C003-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md", "M255-C003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md", "M255-C003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h", "M255-C003-HDR-EXISTS", LOWERING_HEADER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp", "M255-C003-CPP-EXISTS", LOWERING_CPP_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp", "M255-C003-IR-EXISTS", IR_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp", "M255-C003-PARSE-EXISTS", PARSER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp", "M255-C003-RUN-EXISTS", RUNTIME_SNIPPETS),
        (ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c", "M255-C003-SHIM-EXISTS", SHIM_SNIPPETS),
        (NIL_FIXTURE, "M255-C003-FIX-EXISTS", FIXTURE_SNIPPETS),
        (ROOT / "package.json", "M255-C003-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), exists_check_id, "required file missing", failures)
        if path.exists():
            checks_total += len(snippets)
            checks_passed += ensure_snippets(path, snippets, failures)

    nil_total, nil_passed, nil_case = run_nil_probe(failures)
    sup_total, sup_passed, sup_case = run_super_dynamic_probe(failures)
    checks_total += nil_total + sup_total
    checks_passed += nil_passed + sup_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "findings": [finding.__dict__ for finding in failures],
        "models": {
            "active_lowering_model": ACTIVE_MODEL,
            "deferred_lowering_model": DEFERRED_MODEL,
            "unsupported_fallback_model": UNSUPPORTED_MODEL,
            "canonical_symbol": CANONICAL_SYMBOL,
            "compatibility_symbol": COMPATIBILITY_SYMBOL,
        },
        "dynamic_probes": {
            "nil": nil_case,
            "super_dynamic": sup_case,
        },
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
