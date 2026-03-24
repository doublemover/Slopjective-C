#!/usr/bin/env python3
"""Checker for M272-D001 runtime fast-path integration contract freeze."""

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
MODE = "m272-d001-part9-runtime-fast-path-integration-freeze-v1"
CONTRACT_ID = "objc3c-part9-runtime-fast-path-integration-contract/m272-d001-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m272" / "M272-D001" / "runtime_fast_path_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_runtime_fast_path_integration_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_d001_runtime_fast_path_integration_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_d001_runtime_fast_path_contract_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m272_d001_runtime_fast_path_contract_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m272" / "d001"
INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"

IR_REQUIRED_SNIPPETS = [
    "call i32 @objc3_method_PolicyBox_class_implicitDirect()",
    "call i32 @objc3_runtime_dispatch_i32(",
    "call i32 @objc3_method_PolicyBox_class_explicitDirect()",
    "i64 1, i1 1, i1 0",
    "i1 1, i1 1",
]


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
        SnippetCheck("M272-D001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M272-D001-EXP-02", "objc3_runtime_copy_method_cache_state_for_testing"),
        SnippetCheck("M272-D001-EXP-03", "M272-D002"),
    ),
    PACKET_DOC: (
        SnippetCheck("M272-D001-PKT-01", "Packet: `M272-D001`"),
        SnippetCheck("M272-D001-PKT-02", "Issue: `#7342`"),
        SnippetCheck("M272-D001-PKT-03", "`M272-D002`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M272-D001-DOCSRC-01", "## M272 runtime fast-path integration contract"),
        SnippetCheck("M272-D001-DOCSRC-02", "objc3_runtime_copy_method_cache_state_for_testing"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M272-D001-DOC-01", "## M272 runtime fast-path integration contract"),
        SnippetCheck("M272-D001-DOC-02", "objc3_runtime_dispatch_i32"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M272-D001-ATTR-01", "## M272 runtime fast-path integration contract (D001)"),
        SnippetCheck("M272-D001-ATTR-02", "objc3_runtime_copy_method_cache_entry_for_testing"),
    ),
    SPEC_DECISIONS: (
        SnippetCheck("M272-D001-DEC-01", "## D-032: Part 9 freezes the existing runtime cache and fallback surface before widening live fast paths"),
        SnippetCheck("M272-D001-DEC-02", "objc3_runtime_dispatch_i32"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M272-D001-ARCH-01", "## M272 Part 9 Runtime Fast-Path Integration Contract (D001)"),
        SnippetCheck("M272-D001-ARCH-02", "objc3_runtime_copy_method_cache_state_for_testing"),
    ),
    TOOLING_RUNTIME_README: (
        SnippetCheck("M272-D001-TRUN-01", "## M272 runtime fast-path integration contract"),
        SnippetCheck("M272-D001-TRUN-02", "m272_d001_runtime_fast_path_contract_probe.cpp"),
    ),
    RUNTIME_README: (
        SnippetCheck("M272-D001-RTR-01", "objc3c-part9-runtime-fast-path-integration-contract/m272-d001-v1"),
        SnippetCheck("M272-D001-RTR-02", "objc3_runtime_copy_method_cache_state_for_testing"),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M272-D001-RIH-01", "M272-D001 runtime-fast-path-integration anchor:"),
        SnippetCheck("M272-D001-RIH-02", "objc3_runtime_method_cache_state_snapshot"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M272-D001-RCPP-01", "M272-D001 runtime-fast-path-integration anchor:"),
        SnippetCheck("M272-D001-RCPP-02", "IR-lowered direct sites"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M272-D001-PROC-01", "M272-D001 runtime-fast-path-integration anchor:"),
        SnippetCheck("M272-D001-PROC-02", "direct-surface artifact paths visible"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M272-D001-PKG-01", '"check:objc3c:m272-d001-runtime-fast-path-integration-contract-and-architecture-freeze"'),
        SnippetCheck("M272-D001-PKG-02", '"test:tooling:m272-d001-runtime-fast-path-integration-contract-and-architecture-freeze"'),
        SnippetCheck("M272-D001-PKG-03", '"check:objc3c:m272-d001-lane-d-readiness"'),
    ),
    FIXTURE: (
        SnippetCheck("M272-D001-FIX-01", "fn callImplicit() -> i32 {"),
        SnippetCheck("M272-D001-FIX-02", "fn callExplicit() -> i32 {"),
        SnippetCheck("M272-D001-FIX-03", "fn callMixed() -> i32 {"),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M272-D001-PRB-01", 'extern "C" int callImplicit(void);'),
        SnippetCheck("M272-D001-PRB-02", 'extern "C" int callMixed(void);'),
        SnippetCheck("M272-D001-PRB-03", 'const char *const fallback_selector = "missingDispatch:";'),
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


def canonical_json(payload: Any) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


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
    parsed: dict[str, str] = {}
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or "=" not in line:
            continue
        key, value = line.split("=", 1)
        parsed[key.strip()] = value.strip()
    return parsed


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0

    ensure_build = run_command([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m272-d001-dynamic-check",
        "--summary-out",
        str(ROOT / "tmp" / "reports" / "m272" / "M272-D001" / "ensure_build_summary.json"),
    ])
    total += 1
    passed += require(
        ensure_build.returncode == 0,
        display_path(BUILD_HELPER),
        "M272-D001-DYN-01",
        f"fast build failed: {ensure_build.stdout}{ensure_build.stderr}",
        failures,
    )

    fixture_dir = PROBE_ROOT / "fixture"
    fixture_dir.mkdir(parents=True, exist_ok=True)
    compile_fixture = run_command([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(fixture_dir),
        "--emit-prefix",
        "module",
    ])
    total += 1
    passed += require(
        compile_fixture.returncode == 0,
        display_path(FIXTURE),
        "M272-D001-DYN-02",
        f"native compile failed: {compile_fixture.stdout}{compile_fixture.stderr}",
        failures,
    )

    ir_path = fixture_dir / "module.ll"
    obj_path = fixture_dir / "module.obj"
    manifest_path = fixture_dir / "module.runtime-registration-manifest.json"
    for check_id, path, detail in (
        ("M272-D001-DYN-03", ir_path, "module.ll missing"),
        ("M272-D001-DYN-04", obj_path, "module.obj missing"),
        ("M272-D001-DYN-05", manifest_path, "module.runtime-registration-manifest.json missing"),
    ):
        total += 1
        passed += require(path.exists(), display_path(path), check_id, detail, failures)

    if manifest_path.exists():
        manifest = json.loads(read_text(manifest_path))
        runtime_library_relative_path = manifest.get("runtime_support_library_archive_relative_path")
        total += 1
        passed += require(
            runtime_library_relative_path == "artifacts/lib/objc3_runtime.lib",
            display_path(manifest_path),
            "M272-D001-DYN-06",
            f"unexpected runtime library path: {runtime_library_relative_path}",
            failures,
        )

    ir_hits: list[str] = []
    if ir_path.exists():
        ir_text = read_text(ir_path)
        for index, snippet in enumerate(IR_REQUIRED_SNIPPETS, start=10):
            total += 1
            present = snippet in ir_text
            passed += require(present, display_path(ir_path), f"M272-D001-IR-{index}", f"missing IR snippet: {snippet}", failures)
            if present:
                ir_hits.append(snippet)

    probe_dir = PROBE_ROOT / "runtime_probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m272_d001_runtime_fast_path_contract_probe.exe"
    clangxx = resolve_clangxx()
    probe_compile = run_command([
        clangxx,
        "-std=c++20",
        "-D_DLL",
        "-D_MT",
        "-Xclang",
        "--dependent-lib=msvcrt",
        "-I",
        str(INCLUDE_ROOT),
        str(RUNTIME_PROBE),
        str(obj_path),
        str(RUNTIME_LIB),
        "-o",
        str(probe_exe),
    ])
    total += 1
    passed += require(
        probe_compile.returncode == 0,
        display_path(RUNTIME_PROBE),
        "M272-D001-DYN-20",
        f"runtime probe compile failed: {probe_compile.stdout}{probe_compile.stderr}",
        failures,
    )
    total += 1
    passed += require(probe_exe.exists(), display_path(probe_exe), "M272-D001-DYN-21", "runtime probe executable missing", failures)

    probe_output = ""
    parsed: dict[str, str] = {}
    if probe_exe.exists():
        probe_run = run_command([str(probe_exe)])
        probe_output = (probe_run.stdout or "") + (probe_run.stderr or "")
        total += 1
        passed += require(
            probe_run.returncode == 0,
            display_path(probe_exe),
            "M272-D001-DYN-22",
            f"runtime probe failed: {probe_output}",
            failures,
        )
        parsed = parse_probe_output(probe_output)
        expected_values = {
            "implicit_value": "3",
            "explicit_value": "5",
            "mixed_first": "12",
            "mixed_second": "12",
            "direct_delta_cache_entry_count": "0",
            "direct_delta_cache_hit_count": "0",
            "direct_delta_cache_miss_count": "0",
            "direct_delta_live_dispatch_count": "0",
            "mixed_first_delta_cache_entry_count": "1",
            "mixed_first_delta_cache_miss_count": "1",
            "mixed_first_delta_slow_path_lookup_count": "1",
            "mixed_first_delta_live_dispatch_count": "1",
            "mixed_first_state_last_dispatch_used_cache": "0",
            "mixed_first_state_last_dispatch_resolved_live_method": "1",
            "mixed_first_state_last_dispatch_fell_back": "0",
            "mixed_first_state_last_selector": "dynamicEscape",
            "mixed_second_delta_cache_hit_count": "1",
            "mixed_second_delta_live_dispatch_count": "1",
            "mixed_second_state_last_dispatch_used_cache": "1",
            "mixed_second_state_last_dispatch_resolved_live_method": "1",
            "mixed_second_state_last_dispatch_fell_back": "0",
            "fallback_first_status": "0",
            "fallback_second_status": "0",
            "fallback_first_delta_cache_entry_count": "1",
            "fallback_first_delta_cache_miss_count": "1",
            "fallback_first_delta_slow_path_lookup_count": "1",
            "fallback_first_delta_fallback_dispatch_count": "1",
            "fallback_first_state_last_dispatch_used_cache": "0",
            "fallback_first_state_last_dispatch_resolved_live_method": "0",
            "fallback_first_state_last_dispatch_fell_back": "1",
            "fallback_second_delta_cache_hit_count": "1",
            "fallback_second_delta_fallback_dispatch_count": "1",
            "fallback_second_state_last_dispatch_used_cache": "1",
            "fallback_second_state_last_dispatch_resolved_live_method": "0",
            "fallback_second_state_last_dispatch_fell_back": "1",
            "dynamic_entry_found": "1",
            "dynamic_entry_resolved": "1",
            "fallback_entry_found": "1",
            "fallback_entry_resolved": "0",
        }
        for index, (key, expected) in enumerate(expected_values.items(), start=30):
            total += 1
            passed += require(
                parsed.get(key) == expected,
                display_path(probe_exe),
                f"M272-D001-PROBE-{index}",
                f"{key} mismatch: expected {expected}, saw {parsed.get(key)!r}",
                failures,
            )
        total += 1
        passed += require(
            parsed.get("fallback_first") == parsed.get("fallback_expected") == parsed.get("fallback_second"),
            display_path(probe_exe),
            "M272-D001-PROBE-80",
            "fallback values must match deterministic expected dispatch result",
            failures,
        )

    dynamic = {
        "fixture": display_path(FIXTURE),
        "fixture_out_dir": display_path(fixture_dir),
        "ir_snippet_hits": ir_hits,
        "probe_exe": display_path(probe_exe),
        "probe_output": probe_output.strip(),
        "probe_values": parsed,
    }
    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total += len(snippets)
        passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_checks(failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "total_checks": total,
        "passed_checks": passed,
        "failed_checks": total - passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if ok:
        print(f"[ok] M272-D001 checker passed ({passed}/{total} checks)")
        return 0
    print(json.dumps(payload, indent=2))
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
