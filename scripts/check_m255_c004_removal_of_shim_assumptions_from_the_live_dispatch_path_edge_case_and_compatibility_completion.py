#!/usr/bin/env python3
"""Fail-closed checker for M255-C004 live dispatch cutover."""

from __future__ import annotations

import json
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-c004-removal-of-shim-assumptions-from-the-live-dispatch-path-edge-case-and-compatibility-completion-v1"
CONTRACT_ID = "objc3c-runtime-call-abi-live-dispatch-cutover/m255-c004-v1"
PREVIOUS_CONTRACT_ID = "objc3c-runtime-call-abi-super-nil-direct-dispatch/m255-c003-v1"
ACTIVE_MODEL = "all-supported-sends-lower-directly-to-canonical-runtime-entrypoint"
COMPATIBILITY_MODEL = "compatibility-dispatch-symbol-remains-exported-but-not-emitted-on-live-path"
UNSUPPORTED_MODEL = "direct-dispatch-fails-closed-until-supported-surface-materializes"
CANONICAL_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_SYMBOL = "objc3_msgsend_i32"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-C004" / "live_dispatch_cutover_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NIL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_nil_runtime_dispatch.objc3"
DYNAMIC_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_dynamic_runtime_dispatch.objc3"
SUPER_DYNAMIC_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_dynamic_method_family_edges.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "c004-live-dispatch-cutover"


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


def selector_score(selector: str) -> int:
    total = 0
    for index, char in enumerate(selector, start=1):
        total += ord(char) * index
    return total


def compute_dispatch_result(receiver: int, selector: str, a0: int = 0, a1: int = 0, a2: int = 0, a3: int = 0) -> int:
    modulus = 2147483629
    value = 41
    value += receiver * 97
    value += a0 * 7
    value += a1 * 11
    value += a2 * 13
    value += a3 * 17
    value += selector_score(selector) * 19
    value %= modulus
    if value < 0:
        value += modulus
    return value


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M255-C004-DOC-01", "# M255 Removal Of Shim Assumptions From The Live Dispatch Path Edge-Case And Compatibility Completion Expectations (C004)"),
    SnippetCheck("M255-C004-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C004-DOC-03", f"`{ACTIVE_MODEL}`"),
    SnippetCheck("M255-C004-DOC-04", f"`{COMPATIBILITY_MODEL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-C004-PKT-01", "# M255-C004 Removal Of Shim Assumptions From The Live Dispatch Path Edge-Case And Compatibility Completion Packet"),
    SnippetCheck("M255-C004-PKT-02", "Packet: `M255-C004`"),
    SnippetCheck("M255-C004-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C004-PKT-04", "`objc3_msgsend_i32` remains exported but is not emitted by the live path."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-C004-NDOC-01", "## Live dispatch cutover and shim-removal boundary (M255-C004)"),
    SnippetCheck("M255-C004-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C004-NDOC-03", "`@objc3_msgsend_i32` remains exported only as compatibility/test evidence and"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-C004-SPC-01", "## M255 live dispatch cutover and shim-removal boundary (C004)"),
    SnippetCheck("M255-C004-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C004-SPC-03", "all supported live sends lower to `objc3_runtime_dispatch_i32`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-C004-META-01", "## M255 live dispatch cutover metadata anchors (C004)"),
    SnippetCheck("M255-C004-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C004-META-03", "`objc3_msgsend_i32` remains exported but is not emitted on the live path"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-C004-HDR-01", 'kObjc3RuntimeDispatchSymbol ='),
    SnippetCheck("M255-C004-HDR-02", '"objc3_runtime_dispatch_i32"'),
    SnippetCheck("M255-C004-HDR-03", "kObjc3RuntimeDispatchLiveCutoverContractId"),
    SnippetCheck("M255-C004-HDR-04", "kObjc3RuntimeDispatchLiveCutoverCompatibilityModel"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M255-C004-CPP-01", "dispatch_surface_family == kObjc3DispatchSurfaceDynamicFamily"),
    SnippetCheck("M255-C004-CPP-02", "compatibility symbol remains a non-emitted alias/test surface"),
)
IR_SNIPPETS = (
    SnippetCheck("M255-C004-IR-01", "supported dynamic sends now"),
    SnippetCheck("M255-C004-IR-02", "compatibility symbol is"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-C004-PARSE-01", 'constexpr const char *kRuntimeShimHostLinkDispatchSymbol ='),
    SnippetCheck("M255-C004-PARSE-02", '"objc3_runtime_dispatch_i32"'),
    SnippetCheck("M255-C004-PARSE-03", "M255-C004 live-dispatch cutover anchor"),
)
PIPELINE_SNIPPETS = (
    SnippetCheck("M255-C004-PIPE-01", "expr->runtime_dispatch_bridge_symbol ="),
    SnippetCheck("M255-C004-PIPE-02", "Objc3DispatchSurfaceRuntimeEntrypointSymbol("),
)
RUNTIME_SNIPPETS = (
    SnippetCheck("M255-C004-RUN-01", "live lowering no longer emits this symbol"),
    SnippetCheck("M255-C004-RUN-02", "formula-parity alias and compatibility/test surface"),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-C004-SHIM-01", "M255-C004 live-dispatch cutover"),
    SnippetCheck("M255-C004-SHIM-02", "is no longer a live lowering"),
)
CLI_DEFAULT_SNIPPETS = (
    SnippetCheck("M255-C004-CLI-01", 'runtime_dispatch_symbol = "objc3_runtime_dispatch_i32";'),
)
PIPELINE_DEFAULT_SNIPPETS = (
    SnippetCheck("M255-C004-FPC-01", 'kRuntimeDispatchDefaultSymbol ='),
    SnippetCheck("M255-C004-FPC-02", '"objc3_runtime_dispatch_i32"'),
)
SEMA_DEFAULT_SNIPPETS = (
    SnippetCheck("M255-C004-SEMA-01", 'kObjc3RuntimeShimHostLinkDefaultDispatchSymbol ='),
    SnippetCheck("M255-C004-SEMA-02", '"objc3_runtime_dispatch_i32"'),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M255-C004-FIX-01", "module dynamicRuntimeDispatch;"),
    SnippetCheck("M255-C004-FIX-02", "return [(toggle ? local : 0) copy];"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-C004-PKG-01", '"check:objc3c:m255-c004-removal-of-shim-assumptions-from-the-live-dispatch-path-edge-case-and-compatibility-completion": "python scripts/check_m255_c004_removal_of_shim_assumptions_from_the_live_dispatch_path_edge_case_and_compatibility_completion.py"'),
    SnippetCheck("M255-C004-PKG-02", '"test:tooling:m255-c004-removal-of-shim-assumptions-from-the-live-dispatch-path-edge-case-and-compatibility-completion": "python -m pytest tests/tooling/test_check_m255_c004_removal_of_shim_assumptions_from_the_live_dispatch_path_edge_case_and_compatibility_completion.py -q"'),
    SnippetCheck("M255-C004-PKG-03", '"check:objc3c:m255-c004-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m255-c004-removal-of-shim-assumptions-from-the-live-dispatch-path-edge-case-and-compatibility-completion && npm run test:tooling:m255-c004-removal-of-shim-assumptions-from-the-live-dispatch-path-edge-case-and-compatibility-completion"'),
)


def load_manifest_case(manifest_path: Path) -> dict[str, Any]:
    manifest = json.loads(read_text(manifest_path))
    lowering = manifest.get("lowering", {}) if isinstance(manifest, dict) else {}
    return {
        "lowering_runtime_dispatch_symbol": lowering.get("runtime_dispatch_symbol") if isinstance(lowering, dict) else None,
        "runtime_shim_host_link_runtime_dispatch_symbol": manifest.get("runtime_shim_host_link_runtime_dispatch_symbol"),
        "runtime_support_library_link_wiring_runtime_dispatch_symbol": manifest.get("runtime_support_library_link_wiring_runtime_dispatch_symbol"),
    }


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
        ("M255-C004-NIL-01", result.returncode == 0, display_path(out_dir), "nil native compile failed"),
        ("M255-C004-NIL-02", manifest_path.exists(), display_path(manifest_path), "manifest missing"),
        ("M255-C004-NIL-03", ir_path.exists(), display_path(ir_path), "IR missing"),
        ("M255-C004-NIL-04", obj_path.exists(), display_path(obj_path), "object missing"),
        ("M255-C004-NIL-05", backend_path.exists(), display_path(backend_path), "backend marker missing"),
        ("M255-C004-NIL-06", exe_path is not None, display_path(out_dir), "expected linked executable artifact"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or not backend_path.exists() or exe_path is None:
        return checks_total, checks_passed, case

    backend_text = read_text(backend_path).strip()
    case["object_backend"] = backend_text
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-C004-NIL-07", f"expected llvm-direct backend, got {backend_text!r}", failures)

    manifest_case = load_manifest_case(manifest_path)
    case.update(manifest_case)
    for check_id, actual_key in (
        ("M255-C004-NIL-08", "lowering_runtime_dispatch_symbol"),
        ("M255-C004-NIL-09", "runtime_shim_host_link_runtime_dispatch_symbol"),
        ("M255-C004-NIL-10", "runtime_support_library_link_wiring_runtime_dispatch_symbol"),
    ):
        checks_total += 1
        checks_passed += require(
            manifest_case.get(actual_key) == CANONICAL_SYMBOL,
            display_path(manifest_path),
            check_id,
            f"expected {actual_key} to be {CANONICAL_SYMBOL!r}, got {manifest_case.get(actual_key)!r}",
            failures,
        )

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    compatibility_decl = f"declare i32 @{COMPATIBILITY_SYMBOL}(" in ir_text
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    case["compatibility_decl_emitted"] = compatibility_decl
    checks_total += 1
    checks_passed += require(canonical_calls == 1, display_path(ir_path), "M255-C004-NIL-11", f"expected 1 canonical runtime dispatch call, got {canonical_calls}", failures)
    checks_total += 1
    checks_passed += require(compatibility_calls == 0, display_path(ir_path), "M255-C004-NIL-12", f"expected 0 compatibility dispatch calls, got {compatibility_calls}", failures)
    checks_total += 1
    checks_passed += require(not compatibility_decl, display_path(ir_path), "M255-C004-NIL-13", "live IR should not declare the compatibility dispatch symbol", failures)

    run_result = run_command([str(exe_path)], cwd=out_dir)
    case["program_exit_code"] = run_result.returncode
    case["expected_exit_code"] = 9
    case["program_stdout"] = run_result.stdout
    case["program_stderr"] = run_result.stderr
    checks_total += 1
    checks_passed += require(run_result.returncode == 9, display_path(exe_path), "M255-C004-NIL-14", f"expected exit 9, got {run_result.returncode}", failures)
    return checks_total, checks_passed, case


def run_dynamic_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "dynamic"
    result = compile_fixture(DYNAMIC_FIXTURE, out_dir)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    exe_path, link_result = link_executable(out_dir)
    case: dict[str, Any] = {
        "fixture": display_path(DYNAMIC_FIXTURE),
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
        ("M255-C004-DYN-01", result.returncode == 0, display_path(out_dir), "dynamic native compile failed"),
        ("M255-C004-DYN-02", manifest_path.exists(), display_path(manifest_path), "manifest missing"),
        ("M255-C004-DYN-03", ir_path.exists(), display_path(ir_path), "IR missing"),
        ("M255-C004-DYN-04", obj_path.exists(), display_path(obj_path), "object missing"),
        ("M255-C004-DYN-05", backend_path.exists(), display_path(backend_path), "backend marker missing"),
        ("M255-C004-DYN-06", exe_path is not None, display_path(out_dir), "expected linked executable artifact"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or not backend_path.exists() or exe_path is None:
        return checks_total, checks_passed, case

    backend_text = read_text(backend_path).strip()
    case["object_backend"] = backend_text
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-C004-DYN-07", f"expected llvm-direct backend, got {backend_text!r}", failures)

    manifest_case = load_manifest_case(manifest_path)
    case.update(manifest_case)
    for check_id, actual_key in (
        ("M255-C004-DYN-08", "lowering_runtime_dispatch_symbol"),
        ("M255-C004-DYN-09", "runtime_shim_host_link_runtime_dispatch_symbol"),
        ("M255-C004-DYN-10", "runtime_support_library_link_wiring_runtime_dispatch_symbol"),
    ):
        checks_total += 1
        checks_passed += require(
            manifest_case.get(actual_key) == CANONICAL_SYMBOL,
            display_path(manifest_path),
            check_id,
            f"expected {actual_key} to be {CANONICAL_SYMBOL!r}, got {manifest_case.get(actual_key)!r}",
            failures,
        )

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    compatibility_decl = f"declare i32 @{COMPATIBILITY_SYMBOL}(" in ir_text
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    case["compatibility_decl_emitted"] = compatibility_decl
    checks_total += 1
    checks_passed += require(canonical_calls == 1, display_path(ir_path), "M255-C004-DYN-11", f"expected 1 canonical runtime dispatch call, got {canonical_calls}", failures)
    checks_total += 1
    checks_passed += require(compatibility_calls == 0, display_path(ir_path), "M255-C004-DYN-12", f"expected 0 compatibility dispatch calls, got {compatibility_calls}", failures)
    checks_total += 1
    checks_passed += require(not compatibility_decl, display_path(ir_path), "M255-C004-DYN-13", "live IR should not declare the compatibility dispatch symbol", failures)

    run_result = run_command([str(exe_path)], cwd=out_dir)
    expected_exit_code = compute_dispatch_result(5, "copy")
    case["program_exit_code"] = run_result.returncode
    case["expected_exit_code"] = expected_exit_code
    case["program_stdout"] = run_result.stdout
    case["program_stderr"] = run_result.stderr
    checks_total += 1
    checks_passed += require(run_result.returncode == expected_exit_code, display_path(exe_path), "M255-C004-DYN-14", f"expected exit {expected_exit_code}, got {run_result.returncode}", failures)
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
        ("M255-C004-SUP-01", result.returncode == 0, display_path(out_dir), "super/dynamic native compile failed"),
        ("M255-C004-SUP-02", manifest_path.exists(), display_path(manifest_path), "manifest missing"),
        ("M255-C004-SUP-03", ir_path.exists(), display_path(ir_path), "IR missing"),
        ("M255-C004-SUP-04", backend_path.exists(), display_path(backend_path), "backend marker missing"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or not backend_path.exists():
        return checks_total, checks_passed, case

    backend_text = read_text(backend_path).strip()
    case["object_backend"] = backend_text
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-C004-SUP-05", f"expected llvm-direct backend, got {backend_text!r}", failures)

    manifest_case = load_manifest_case(manifest_path)
    case.update(manifest_case)
    for check_id, actual_key in (
        ("M255-C004-SUP-06", "lowering_runtime_dispatch_symbol"),
        ("M255-C004-SUP-07", "runtime_shim_host_link_runtime_dispatch_symbol"),
        ("M255-C004-SUP-08", "runtime_support_library_link_wiring_runtime_dispatch_symbol"),
    ):
        checks_total += 1
        checks_passed += require(
            manifest_case.get(actual_key) == CANONICAL_SYMBOL,
            display_path(manifest_path),
            check_id,
            f"expected {actual_key} to be {CANONICAL_SYMBOL!r}, got {manifest_case.get(actual_key)!r}",
            failures,
        )

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    compatibility_decl = f"declare i32 @{COMPATIBILITY_SYMBOL}(" in ir_text
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    case["compatibility_decl_emitted"] = compatibility_decl
    checks_total += 1
    checks_passed += require(canonical_calls == 7, display_path(ir_path), "M255-C004-SUP-09", f"expected 7 canonical runtime dispatch calls, got {canonical_calls}", failures)
    checks_total += 1
    checks_passed += require(compatibility_calls == 0, display_path(ir_path), "M255-C004-SUP-10", f"expected 0 compatibility dispatch calls, got {compatibility_calls}", failures)
    checks_total += 1
    checks_passed += require(not compatibility_decl, display_path(ir_path), "M255-C004-SUP-11", "live IR should not declare the compatibility dispatch symbol", failures)
    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    doc_checks: tuple[tuple[Path, str, tuple[SnippetCheck, ...]], ...] = (
        (ROOT / "docs" / "contracts" / "m255_removal_of_shim_assumptions_from_the_live_dispatch_path_edge_case_and_compatibility_completion_c004_expectations.md", "M255-C004-DOC-EXISTS", EXPECTATIONS_SNIPPETS),
        (ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_c004_removal_of_shim_assumptions_from_the_live_dispatch_path_edge_case_and_compatibility_completion_packet.md", "M255-C004-PKT-EXISTS", PACKET_SNIPPETS),
        (ROOT / "docs" / "objc3c-native.md", "M255-C004-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md", "M255-C004-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md", "M255-C004-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h", "M255-C004-HDR-EXISTS", LOWERING_HEADER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp", "M255-C004-CPP-EXISTS", LOWERING_CPP_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp", "M255-C004-IR-EXISTS", IR_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp", "M255-C004-PARSE-EXISTS", PARSER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp", "M255-C004-PIPE-EXISTS", PIPELINE_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp", "M255-C004-RUN-EXISTS", RUNTIME_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h", "M255-C004-CLI-EXISTS", CLI_DEFAULT_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h", "M255-C004-FPC-EXISTS", PIPELINE_DEFAULT_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h", "M255-C004-SEMA-EXISTS", SEMA_DEFAULT_SNIPPETS),
        (ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c", "M255-C004-SHIM-EXISTS", SHIM_SNIPPETS),
        (DYNAMIC_FIXTURE, "M255-C004-FIX-EXISTS", FIXTURE_SNIPPETS),
        (ROOT / "package.json", "M255-C004-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), exists_check_id, "required file missing", failures)
        if path.exists():
            checks_total += len(snippets)
            checks_passed += ensure_snippets(path, snippets, failures)

    nil_total, nil_passed, nil_case = run_nil_probe(failures)
    dynamic_total, dynamic_passed, dynamic_case = run_dynamic_probe(failures)
    sup_total, sup_passed, sup_case = run_super_dynamic_probe(failures)
    checks_total += nil_total + dynamic_total + sup_total
    checks_passed += nil_passed + dynamic_passed + sup_passed

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
            "compatibility_model": COMPATIBILITY_MODEL,
            "unsupported_fallback_model": UNSUPPORTED_MODEL,
            "canonical_symbol": CANONICAL_SYMBOL,
            "compatibility_symbol": COMPATIBILITY_SYMBOL,
        },
        "dynamic_probes": {
            "nil": nil_case,
            "dynamic": dynamic_case,
            "super_dynamic": sup_case,
        },
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
