#!/usr/bin/env python3
"""Fail-closed checker for M255-C002 runtime call ABI generation."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-c002-runtime-call-abi-generation-for-instance-and-class-sends-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1"
PREVIOUS_CONTRACT_ID = "objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1"
ACTIVE_MODEL = "instance-and-class-sends-lower-directly-to-canonical-runtime-entrypoint"
DEFERRED_MODEL = "super-dynamic-and-deferred-sends-stay-on-compatibility-bridge-until-m255-c003"
CANONICAL_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_SYMBOL = "objc3_msgsend_i32"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-C002" / "runtime_call_abi_generation_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_instance_class_runtime_dispatch.objc3"
DEFERRED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_dynamic_method_family_edges.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "c002-runtime-call-abi"


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


def compute_selector_score(selector: str) -> int:
    modulus = 2147483629
    value = 0
    for index, byte in enumerate(selector.encode("utf-8"), start=1):
        value = (value + (byte * index)) % modulus
    return value


def compute_dispatch_result(receiver: int, selector: str, a0: int = 0, a1: int = 0, a2: int = 0, a3: int = 0) -> int:
    modulus = 2147483629
    value = 41
    value += receiver * 97
    value += a0 * 7
    value += a1 * 11
    value += a2 * 13
    value += a3 * 17
    value += compute_selector_score(selector) * 19
    value %= modulus
    if value < 0:
        value += modulus
    return value


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def find_single(path: Path, pattern: str) -> Path | None:
    matches = sorted(path.glob(pattern))
    if len(matches) == 1:
        return matches[0]
    return None


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
    command = [
        resolve_clang(),
        str(obj_path),
        str(runtime_library_path),
        f"@{rsp_path.resolve()}",
        "-o",
        str(exe_path),
    ]
    result = run_command(command, cwd=out_dir)
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M255-C002-DOC-01", "# M255 Runtime Call ABI Generation For Instance And Class Sends Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M255-C002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C002-DOC-03", f"`{CANONICAL_SYMBOL}`"),
    SnippetCheck("M255-C002-DOC-04", f"`{COMPATIBILITY_SYMBOL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-C002-PKT-01", "# M255-C002 Runtime Call ABI Generation For Instance And Class Sends Core Feature Implementation Packet"),
    SnippetCheck("M255-C002-PKT-02", "Packet: `M255-C002`"),
    SnippetCheck("M255-C002-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C002-PKT-04", "Super/dynamic/deferred sends remain on `objc3_msgsend_i32` until `M255-C003`."),
)
DOC_SNIPPETS = (
    SnippetCheck("M255-C002-NDOC-01", "## Runtime call ABI generation for instance and class sends (M255-C002)"),
    SnippetCheck("M255-C002-NDOC-02", f"`{CANONICAL_SYMBOL}`"),
    SnippetCheck("M255-C002-NDOC-03", f"`{COMPATIBILITY_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-C002-SPC-01", "## M255 runtime call ABI generation for instance and class sends (C002)"),
    SnippetCheck("M255-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C002-SPC-03", "normalized class sends lower to `objc3_runtime_dispatch_i32`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-C002-META-01", "## M255 runtime call ABI metadata anchors (C002)"),
    SnippetCheck("M255-C002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C002-META-03", "normalized super/dynamic/deferred sends remain on `objc3_msgsend_i32`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-C002-HDR-01", "kObjc3RuntimeDispatchCallAbiGenerationContractId"),
    SnippetCheck("M255-C002-HDR-02", "kObjc3RuntimeDispatchCallAbiGenerationActiveLoweringModel"),
    SnippetCheck("M255-C002-HDR-03", "UsesCanonicalObjc3RuntimeDispatchEntrypoint("),
    SnippetCheck("M255-C002-HDR-04", "Objc3DispatchSurfaceRuntimeEntrypointSymbol("),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M255-C002-CPP-01", "M255-C002 runtime call ABI generation anchor"),
    SnippetCheck("M255-C002-CPP-02", "UsesCanonicalObjc3RuntimeDispatchEntrypoint("),
    SnippetCheck("M255-C002-CPP-03", "Objc3DispatchSurfaceRuntimeEntrypointSymbol("),
)
IR_SNIPPETS = (
    SnippetCheck("M255-C002-IR-01", "std::string dispatch_symbol = kObjc3RuntimeDispatchSymbol;"),
    SnippetCheck("M255-C002-IR-02", "lowered.dispatch_symbol ="),
    SnippetCheck("M255-C002-IR-03", "M255-C002 runtime call ABI generation anchor"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-C002-PARSE-01", "M255-C002 runtime call ABI generation anchor"),
    SnippetCheck("M255-C002-PARSE-02", "Lowering now cuts normalized instance and"),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-C002-SHIM-01", "M255-C002 runtime call ABI generation"),
    SnippetCheck("M255-C002-SHIM-02", "deferred super/dynamic sites"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M255-C002-FIX-01", "module instanceClassRuntimeDispatch;"),
    SnippetCheck("M255-C002-FIX-02", "let instance = [receiver ping];"),
    SnippetCheck("M255-C002-FIX-03", "let classValue = [Widget shared];"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-C002-PKG-01", '"check:objc3c:m255-c002-runtime-call-abi-generation-for-instance-and-class-sends-core-feature-implementation": "python scripts/check_m255_c002_runtime_call_abi_generation_for_instance_and_class_sends_core_feature_implementation.py"'),
    SnippetCheck("M255-C002-PKG-02", '"test:tooling:m255-c002-runtime-call-abi-generation-for-instance-and-class-sends-core-feature-implementation": "python -m pytest tests/tooling/test_check_m255_c002_runtime_call_abi_generation_for_instance_and_class_sends_core_feature_implementation.py -q"'),
    SnippetCheck("M255-C002-PKG-03", '"check:objc3c:m255-c002-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m255-c002-runtime-call-abi-generation-for-instance-and-class-sends-core-feature-implementation && npm run test:tooling:m255-c002-runtime-call-abi-generation-for-instance-and-class-sends-core-feature-implementation"'),
)


def run_positive_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "positive"
    result = compile_fixture(POSITIVE_FIXTURE, out_dir)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    exe_path = find_single(out_dir, "*.exe")
    backend_path = out_dir / "module.object-backend.txt"
    link_result: subprocess.CompletedProcess[str] | None = None
    if exe_path is None and result.returncode == 0:
        exe_path, link_result = link_executable(out_dir)

    case: dict[str, Any] = {
        "fixture": display_path(POSITIVE_FIXTURE),
        "out_dir": display_path(out_dir),
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "object_exists": obj_path.exists(),
        "executable_path": display_path(exe_path) if exe_path is not None else None,
        "backend_exists": backend_path.exists(),
    }
    if link_result is not None:
        case["link_returncode"] = link_result.returncode
        case["link_stdout"] = link_result.stdout
        case["link_stderr"] = link_result.stderr

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(out_dir), "M255-C002-POS-01", "positive native compile failed", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M255-C002-POS-02", "manifest missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M255-C002-POS-03", "IR missing", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M255-C002-POS-04", "object missing", failures)
    checks_total += 1
    checks_passed += require(exe_path is not None, display_path(out_dir), "M255-C002-POS-05", "expected one executable artifact", failures)
    checks_total += 1
    checks_passed += require(backend_path.exists(), display_path(backend_path), "M255-C002-POS-06", "backend marker missing", failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or exe_path is None or not backend_path.exists():
        return checks_total, checks_passed, case

    backend_text = read_text(backend_path).strip()
    case["object_backend"] = backend_text
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-C002-POS-07", f"expected llvm-direct backend, got {backend_text!r}", failures)

    manifest = json.loads(read_text(manifest_path))
    lowering = manifest.get("lowering", {}) if isinstance(manifest, dict) else {}
    case["lowering_runtime_dispatch_symbol"] = lowering.get("runtime_dispatch_symbol") if isinstance(lowering, dict) else None
    checks_total += 1
    checks_passed += require(isinstance(lowering, dict) and lowering.get("runtime_dispatch_symbol") == COMPATIBILITY_SYMBOL, display_path(manifest_path), "M255-C002-POS-08", "historical lowering boundary should remain compatibility bridge", failures)

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    checks_total += 1
    checks_passed += require(canonical_calls == 2, display_path(ir_path), "M255-C002-POS-09", f"expected 2 canonical runtime dispatch calls, got {canonical_calls}", failures)
    checks_total += 1
    checks_passed += require(compatibility_calls == 0, display_path(ir_path), "M255-C002-POS-10", f"expected 0 compatibility dispatch calls, got {compatibility_calls}", failures)

    run_result = run_command([str(exe_path)], cwd=out_dir)
    expected_exit = compute_dispatch_result(9, "ping") + compute_dispatch_result(1024, "shared")
    case["program_exit_code"] = run_result.returncode
    case["expected_exit_code"] = expected_exit
    case["program_stdout"] = run_result.stdout
    case["program_stderr"] = run_result.stderr
    checks_total += 1
    checks_passed += require(run_result.returncode == expected_exit, display_path(exe_path), "M255-C002-POS-11", f"expected exit {expected_exit}, got {run_result.returncode}", failures)

    return checks_total, checks_passed, case


def run_deferred_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "deferred"
    result = compile_fixture(DEFERRED_FIXTURE, out_dir)
    ir_path = out_dir / "module.ll"
    case: dict[str, Any] = {
        "fixture": display_path(DEFERRED_FIXTURE),
        "out_dir": display_path(out_dir),
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "ir_exists": ir_path.exists(),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(out_dir), "M255-C002-DEF-01", "deferred native compile failed", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M255-C002-DEF-02", "deferred IR missing", failures)
    if result.returncode != 0 or not ir_path.exists():
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    canonical_calls = ir_text.count(f"call i32 @{CANONICAL_SYMBOL}(")
    compatibility_calls = ir_text.count(f"call i32 @{COMPATIBILITY_SYMBOL}(")
    case["canonical_call_count"] = canonical_calls
    case["compatibility_call_count"] = compatibility_calls
    checks_total += 1
    checks_passed += require(compatibility_calls >= 1, display_path(ir_path), "M255-C002-DEF-03", "expected deferred super/dynamic probe to keep compatibility call sites", failures)
    checks_total += 1
    checks_passed += require(canonical_calls == 0, display_path(ir_path), "M255-C002-DEF-04", f"expected no canonical runtime dispatch calls in deferred probe, got {canonical_calls}", failures)
    return checks_total, checks_passed, case


def main() -> int:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    files_and_snippets = [
        (ROOT / "docs" / "contracts" / "m255_runtime_call_abi_generation_for_instance_and_class_sends_core_feature_implementation_c002_expectations.md", EXPECTATIONS_SNIPPETS),
        (ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_c002_runtime_call_abi_generation_for_instance_and_class_sends_core_feature_implementation_packet.md", PACKET_SNIPPETS),
        (ROOT / "docs" / "objc3c-native.md", DOC_SNIPPETS),
        (ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md", LOWERING_SPEC_SNIPPETS),
        (ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md", METADATA_SPEC_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h", LOWERING_HEADER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp", LOWERING_CPP_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp", IR_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp", PARSER_SNIPPETS),
        (ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c", SHIM_SNIPPETS),
        (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_instance_class_runtime_dispatch.objc3", FIXTURE_SNIPPETS),
        (ROOT / "package.json", PACKAGE_SNIPPETS),
    ]
    for path, snippets in files_and_snippets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    positive_total, positive_passed, positive_case = run_positive_probe(failures)
    checks_total += positive_total
    checks_passed += positive_passed
    deferred_total, deferred_passed, deferred_case = run_deferred_probe(failures)
    checks_total += deferred_total
    checks_passed += deferred_passed

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
            "canonical_symbol": CANONICAL_SYMBOL,
            "compatibility_symbol": COMPATIBILITY_SYMBOL,
        },
        "dynamic_probes": {
            "positive": positive_case,
            "deferred": deferred_case,
        },
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    json.dump(summary, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
