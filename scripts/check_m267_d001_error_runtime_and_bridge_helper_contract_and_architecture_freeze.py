#!/usr/bin/env python3
"""Validate M267-D001 error runtime and bridge-helper contract."""

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
MODE = "m267-d001-error-runtime-and-bridge-helper-contract-freeze-v1"
CONTRACT_ID = "objc3c-part6-error-runtime-and-bridge-helper-api/m267-d001-v1"
BOUNDARY_PREFIX = "; part6_error_runtime_bridge_helper = contract=" + CONTRACT_ID
NAMED_METADATA_LINE = "!objc3.objc_part6_error_runtime_bridge_helper = !{!89}"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-D001" / "error_runtime_bridge_helper_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_error_runtime_and_bridge_helper_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_d001_error_runtime_and_bridge_helper_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
SEMANTIC_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_d001_error_runtime_bridge_helper_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m267_d001_error_runtime_bridge_helper_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "d001"
INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
HELPER_SYMBOLS = (
    "objc3_runtime_store_thrown_error_i32",
    "objc3_runtime_load_thrown_error_i32",
    "objc3_runtime_bridge_status_error_i32",
    "objc3_runtime_bridge_nserror_error_i32",
    "objc3_runtime_catch_matches_error_i32",
    "objc3_runtime_copy_error_bridge_state_for_testing",
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


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M267-D001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-D001-EXP-02", "!objc3.objc_part6_error_runtime_bridge_helper = !{!89}"),
        SnippetCheck("M267-D001-EXP-03", "tests/tooling/runtime/m267_d001_error_runtime_bridge_helper_probe.cpp"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-D001-PKT-01", "Packet: `M267-D001`"),
        SnippetCheck("M267-D001-PKT-02", "Issue: `#7277`"),
        SnippetCheck("M267-D001-PKT-03", "`M267-D002` is the next issue"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M267-D001-DOCSRC-01", "## M267 Part 6 error runtime and bridge-helper contract (M267-D001)"),
        SnippetCheck("M267-D001-DOCSRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M267-D001-DOCSRC-03", "`objc3_runtime_copy_error_bridge_state_for_testing`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M267-D001-NDOC-01", "## M267 Part 6 error runtime and bridge-helper contract (M267-D001)"),
        SnippetCheck("M267-D001-NDOC-02", "!objc3.objc_part6_error_runtime_bridge_helper = !{!89}"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M267-D001-LSPEC-01", "## M267 Part 6 error runtime and bridge-helper contract (D001)"),
        SnippetCheck("M267-D001-LSPEC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M267-D001-LSPEC-03", "`objc3_runtime_catch_matches_error_i32`"),
    ),
    SEMANTIC_SPEC: (
        SnippetCheck("M267-D001-SEM-01", "M267-D001 runtime-helper note:"),
        SnippetCheck("M267-D001-SEM-02", "`objc3_runtime_copy_error_bridge_state_for_testing`"),
    ),
    SYNTAX_SPEC: (
        SnippetCheck("M267-D001-SYN-01", "Current implementation status (`M267-D001`):"),
        SnippetCheck("M267-D001-SYN-02", f"`{CONTRACT_ID}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M267-D001-ARCH-01", "## M267 Part 6 Error Runtime And Bridge-Helper Contract (D001)"),
        SnippetCheck("M267-D001-ARCH-02", "`objc3_runtime_store_thrown_error_i32`"),
        SnippetCheck("M267-D001-ARCH-03", "the next issue is `M267-D002`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M267-D001-RTR-01", "## M267 error runtime and bridge-helper probe"),
        SnippetCheck("M267-D001-RTR-02", "tests/tooling/runtime/m267_d001_error_runtime_bridge_helper_probe.cpp"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M267-D001-LHDR-01", "kObjc3Part6ErrorRuntimeBridgeHelperContractId"),
        SnippetCheck("M267-D001-LHDR-02", "kObjc3RuntimeStoreThrownErrorI32Symbol"),
        SnippetCheck("M267-D001-LHDR-03", "Objc3Part6ErrorRuntimeBridgeHelperSummary()"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M267-D001-LCPP-01", "Objc3Part6ErrorRuntimeBridgeHelperSummary()"),
        SnippetCheck("M267-D001-LCPP-02", ";next_issue=M267-D002"),
    ),
    IR_EMITTER: (
        SnippetCheck("M267-D001-IR-01", 'out << "; part6_error_runtime_bridge_helper = "'),
        SnippetCheck("M267-D001-IR-02", '!objc3.objc_part6_error_runtime_bridge_helper = !{!89}'),
        SnippetCheck("M267-D001-IR-03", 'kObjc3RuntimeCatchMatchesErrorI32Symbol'),
    ),
    RUNTIME_HEADER: (
        SnippetCheck("M267-D001-RH-01", "M267-D001 error-runtime/bridge-helper anchor"),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M267-D001-RIH-01", "typedef struct objc3_runtime_error_bridge_state_snapshot"),
        SnippetCheck("M267-D001-RIH-02", "objc3_runtime_store_thrown_error_i32"),
        SnippetCheck("M267-D001-RIH-03", "objc3_runtime_copy_error_bridge_state_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M267-D001-RCPP-01", "objc3_runtime_copy_error_bridge_state_for_testing"),
        SnippetCheck("M267-D001-RCPP-02", "objc3_runtime_bridge_status_error_i32"),
        SnippetCheck("M267-D001-RCPP-03", "objc3_runtime_catch_matches_error_i32"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M267-D001-PROC-01", "M267-D001 error-runtime/bridge-helper anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-D001-PKG-01", '"check:objc3c:m267-d001-error-runtime-bridge-helper-contract": "python scripts/check_m267_d001_error_runtime_and_bridge_helper_contract_and_architecture_freeze.py"'),
        SnippetCheck("M267-D001-PKG-02", '"test:tooling:m267-d001-error-runtime-bridge-helper-contract": "python -m pytest tests/tooling/test_check_m267_d001_error_runtime_and_bridge_helper_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M267-D001-PKG-03", '"check:objc3c:m267-d001-lane-d-readiness": "python scripts/run_m267_d001_lane_d_readiness.py"'),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
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


def parse_probe_output(text: str) -> dict[str, str]:
    out: dict[str, str] = {}
    for line in text.splitlines():
        if "=" not in line:
            continue
        key, value = line.split("=", 1)
        out[key.strip()] = value.strip()
    return out


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "dynamic_probe", check_id, detail, failures)

    check(BUILD_HELPER.exists(), "M267-D001-DYN-01", f"missing build helper: {display_path(BUILD_HELPER)}")
    check(NATIVE_EXE.exists(), "M267-D001-DYN-02", f"missing native executable: {display_path(NATIVE_EXE)}")
    check(RUNTIME_LIB.exists(), "M267-D001-DYN-03", f"missing runtime library: {display_path(RUNTIME_LIB)}")
    check(FIXTURE.exists(), "M267-D001-DYN-04", f"missing fixture: {display_path(FIXTURE)}")
    check(RUNTIME_PROBE.exists(), "M267-D001-DYN-05", f"missing runtime probe: {display_path(RUNTIME_PROBE)}")
    if failures:
        return checks_passed, checks_total, {"skipped": True}

    ensure_summary = ROOT / "tmp" / "reports" / "m267" / "M267-D001" / "ensure_objc3c_native_build_from_checker_summary.json"
    ensured = run_command(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m267-d001-dynamic-check",
            "--summary-out",
            str(ensure_summary),
        ]
    )
    check(ensured.returncode == 0, "M267-D001-DYN-06", f"fast build failed: {ensured.stdout}{ensured.stderr}")
    if ensured.returncode != 0:
        return checks_passed, checks_total, {"ensure_stdout": ensured.stdout, "ensure_stderr": ensured.stderr}

    fixture_dir = PROBE_ROOT / "fixture"
    fixture_dir.mkdir(parents=True, exist_ok=True)
    compiled = run_command([str(NATIVE_EXE), str(FIXTURE), "--out-dir", str(fixture_dir), "--emit-prefix", "module"])
    ir_path = fixture_dir / "module.ll"
    check(compiled.returncode == 0, "M267-D001-DYN-07", f"fixture compile failed: {compiled.stdout}{compiled.stderr}")
    check(ir_path.exists(), "M267-D001-DYN-08", f"missing emitted IR: {display_path(ir_path)}")
    if compiled.returncode != 0 or not ir_path.exists():
        return checks_passed, checks_total, {"compile_stdout": compiled.stdout, "compile_stderr": compiled.stderr}

    ir_text = read_text(ir_path)
    check(BOUNDARY_PREFIX in ir_text, "M267-D001-DYN-09", "missing Part 6 runtime-helper boundary line")
    check(NAMED_METADATA_LINE in ir_text, "M267-D001-DYN-10", "missing named metadata line")
    for index, symbol in enumerate(HELPER_SYMBOLS[:-1], start=11):
        check(symbol in ir_text, f"M267-D001-DYN-{index:02d}", f"missing helper symbol in emitted IR: {symbol}")

    probe_dir = PROBE_ROOT / "runtime_probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m267_d001_error_runtime_bridge_helper_probe.exe"
    clangxx = resolve_clangxx()
    probe_compile = run_command(
        [
            clangxx,
            "-std=c++20",
            "-I",
            str(INCLUDE_ROOT),
            str(RUNTIME_PROBE),
            str(RUNTIME_LIB),
            "-o",
            str(probe_exe),
        ]
    )
    check(probe_compile.returncode == 0, "M267-D001-DYN-17", f"runtime probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    check(probe_exe.exists(), "M267-D001-DYN-18", f"missing runtime probe executable: {display_path(probe_exe)}")
    if probe_compile.returncode != 0 or not probe_exe.exists():
        return checks_passed, checks_total, {"probe_compile_stdout": probe_compile.stdout, "probe_compile_stderr": probe_compile.stderr}

    probe_run = run_command([str(probe_exe)])
    check(probe_run.returncode == 0, "M267-D001-DYN-19", f"runtime probe failed: {probe_run.stdout}{probe_run.stderr}")
    parsed = parse_probe_output(probe_run.stdout)
    check(parsed.get("store_call_count") == "1", "M267-D001-DYN-20", f"unexpected store count: {parsed}")
    check(parsed.get("load_call_count") == "1", "M267-D001-DYN-21", f"unexpected load count: {parsed}")
    check(parsed.get("status_bridge_call_count") == "1", "M267-D001-DYN-22", f"unexpected status bridge count: {parsed}")
    check(parsed.get("nserror_bridge_call_count") == "1", "M267-D001-DYN-23", f"unexpected nserror bridge count: {parsed}")
    check(parsed.get("catch_match_call_count") == "3", "M267-D001-DYN-24", f"unexpected catch match count: {parsed}")
    check(parsed.get("last_catch_kind_name") == "unknown", "M267-D001-DYN-25", f"unexpected catch kind name: {parsed}")

    return checks_passed, checks_total, {
        "fixture_out_dir": display_path(fixture_dir),
        "runtime_probe_exe": display_path(probe_exe),
        "runtime_probe_output": parsed,
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    runtime_header_text = read_text(RUNTIME_HEADER)
    runtime_internal_text = read_text(RUNTIME_INTERNAL)
    for symbol in HELPER_SYMBOLS[:-1]:
        checks_total += 1
        checks_passed += require(symbol not in runtime_header_text, display_path(RUNTIME_HEADER), f"M267-D001-PUB-{checks_total:02d}", f"public runtime header must not expose helper symbol {symbol}", failures)
        checks_total += 1
        checks_passed += require(symbol in runtime_internal_text, display_path(RUNTIME_INTERNAL), f"M267-D001-PRI-{checks_total:02d}", f"private runtime header must expose helper symbol {symbol}", failures)

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    dynamic_checks_total = 0
    dynamic_checks_passed = 0
    if not args.skip_dynamic_probes:
        dynamic_checks_passed, dynamic_checks_total, dynamic_payload = run_dynamic_checks(failures)
        checks_total += dynamic_checks_total
        checks_passed += dynamic_checks_passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "checks_failed": len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic": dynamic_payload,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        print(canonical_json(payload), end="")
        return 1
    print(f"[ok] wrote {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
