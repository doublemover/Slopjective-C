#!/usr/bin/env python3
"""Validate M269-D002 live task runtime and executor implementation."""

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
MODE = "m269-d002-part7-live-task-runtime-integration-v1"
CONTRACT_ID = "objc3c-part7-live-task-runtime-integration/m269-d002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m269" / "M269-D002" / "live_task_runtime_integration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_live_task_runtime_and_executor_implementation_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_d002_live_task_runtime_and_executor_implementation_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
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
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_d002_live_task_runtime_and_executor_implementation_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m269_d002_live_task_runtime_and_executor_implementation_probe.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "d002"

HELPER_SYMBOLS = (
    "objc3_runtime_spawn_task_i32",
    "objc3_runtime_enter_task_group_scope_i32",
    "objc3_runtime_add_task_group_task_i32",
    "objc3_runtime_wait_task_group_next_i32",
    "objc3_runtime_cancel_task_group_i32",
    "objc3_runtime_task_is_cancelled_i32",
    "objc3_runtime_task_on_cancel_i32",
    "objc3_runtime_executor_hop_i32",
)
RUNTIME_TEST_SYMBOLS = ("objc3_runtime_copy_task_runtime_state_for_testing",)


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
        SnippetCheck("M269-D002-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M269-D002-EXP-02", "!objc3.objc_part7_live_task_runtime_integration = !{!95}"),
        SnippetCheck("M269-D002-EXP-03", "O3S260` / `O3L300`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M269-D002-PKT-01", "Issue: `#7303`"),
        SnippetCheck("M269-D002-PKT-02", "part7_live_task_runtime_integration"),
        SnippetCheck("M269-D002-PKT-03", "`M269-D003` is the next issue"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M269-D002-DOCSRC-01", "## M269 live task runtime and executor implementation"),
        SnippetCheck("M269-D002-DOCSRC-02", "!objc3.objc_part7_live_task_runtime_integration = !{!95}"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M269-D002-DOC-01", "## M269 live task runtime and executor implementation"),
        SnippetCheck("M269-D002-DOC-02", "!objc3.objc_part7_live_task_runtime_integration = !{!95}"),
    ),
    SPEC_AM: (
        SnippetCheck("M269-D002-AM-01", "M269-D002 live task runtime note:"),
        SnippetCheck("M269-D002-AM-02", "!objc3.objc_part7_live_task_runtime_integration = !{!95}"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M269-D002-ATTR-01", "Current implementation status (`M269-D002`):"),
        SnippetCheck("M269-D002-ATTR-02", "!objc3.objc_part7_live_task_runtime_integration = !{!95}"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M269-D002-ARCH-01", "## M269 Part 7 Live Task Runtime And Executor Implementation (D002)"),
        SnippetCheck("M269-D002-ARCH-02", "objc3_runtime_copy_task_runtime_state_for_testing"),
        SnippetCheck("M269-D002-ARCH-03", "the next issue is `M269-D003`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M269-D002-RTR-01", "## M269 live task runtime and executor implementation probe"),
        SnippetCheck("M269-D002-RTR-02", "tests/tooling/runtime/m269_d002_live_task_runtime_and_executor_implementation_probe.cpp"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M269-D002-LHDR-01", "kObjc3Part7LiveTaskRuntimeIntegrationContractId"),
        SnippetCheck("M269-D002-LHDR-02", "kObjc3Part7LiveTaskRuntimeIntegrationExecutionModel"),
        SnippetCheck("M269-D002-LHDR-03", "Objc3Part7LiveTaskRuntimeIntegrationSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M269-D002-LCPP-01", "Objc3Part7LiveTaskRuntimeIntegrationSummary()"),
        SnippetCheck("M269-D002-LCPP-02", "objc3_runtime_copy_task_runtime_state_for_testing"),
        SnippetCheck("M269-D002-LCPP-03", ";next_issue=M269-D003"),
    ),
    IR_EMITTER: (
        SnippetCheck("M269-D002-IR-01", 'out << "; part7_live_task_runtime_integration = "'),
        SnippetCheck("M269-D002-IR-02", '!objc3.objc_part7_live_task_runtime_integration = !{!95}'),
        SnippetCheck("M269-D002-IR-03", 'kObjc3Part7LiveTaskRuntimeIntegrationContractId'),
        SnippetCheck("M269-D002-IR-04", 'objc3_runtime_copy_task_runtime_state_for_testing'),
    ),
    RUNTIME_HEADER: (
        SnippetCheck("M269-D002-RH-01", "M269-D002 live task runtime anchor"),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M269-D002-RIH-01", "M269-D002 live task runtime anchor"),
        SnippetCheck("M269-D002-RIH-02", "objc3_runtime_executor_hop_i32"),
        SnippetCheck("M269-D002-RIH-03", "objc3_runtime_copy_task_runtime_state_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M269-D002-RCPP-01", "M269-D002 live task runtime anchor"),
        SnippetCheck("M269-D002-RCPP-02", "objc3_runtime_spawn_task_i32"),
        SnippetCheck("M269-D002-RCPP-03", "g_runtime_task_executor_hop_call_count"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M269-D002-PROC-01", "M269-D002 live task runtime anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M269-D002-PKG-01", '"check:objc3c:m269-d002-live-task-runtime-and-executor-implementation-core-feature-implementation"'),
        SnippetCheck("M269-D002-PKG-02", '"test:tooling:m269-d002-live-task-runtime-and-executor-implementation-core-feature-implementation"'),
        SnippetCheck("M269-D002-PKG-03", '"check:objc3c:m269-d002-lane-d-readiness"'),
    ),
    FIXTURE: (
        SnippetCheck("M269-D002-FIX-01", "module M269D002;"),
        SnippetCheck("M269-D002-FIX-02", "task_group_wait_next"),
        SnippetCheck("M269-D002-FIX-03", "detached_task_create"),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M269-D002-PROBE-01", "objc3_runtime_spawn_task_i32"),
        SnippetCheck("M269-D002-PROBE-02", "objc3_runtime_copy_task_runtime_state_for_testing"),
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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def canonical_json(payload: Any) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
    result: dict[str, str] = {}
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or "=" not in line:
            continue
        key, value = line.split("=", 1)
        result[key.strip()] = value.strip()
    return result


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m269-d002-dynamic-check",
        "--summary-out",
        str(ROOT / "tmp" / "reports" / "m269" / "M269-D002" / "ensure_build_summary.json"),
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        display_path(BUILD_HELPER),
        "M269-D002-DYN-01",
        f"fast build failed: {ensure_build.stdout}{ensure_build.stderr}",
        failures,
    )

    probe_dir = PROBE_ROOT / "runtime_probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m269_d002_live_task_runtime_and_executor_implementation_probe.exe"
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
        str(RUNTIME_LIB),
        "-o",
        str(probe_exe),
    ])
    checks_total += 1
    checks_passed += require(
        probe_compile.returncode == 0,
        display_path(RUNTIME_PROBE),
        "M269-D002-DYN-02",
        f"runtime probe compile failed: {probe_compile.stdout}{probe_compile.stderr}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        probe_exe.exists(),
        display_path(probe_exe),
        "M269-D002-DYN-03",
        "missing runtime probe executable",
        failures,
    )

    parsed: dict[str, str] = {}
    if probe_compile.returncode == 0 and probe_exe.exists():
        probe_run = run_command([str(probe_exe)])
        checks_total += 1
        checks_passed += require(
            probe_run.returncode == 0,
            display_path(probe_exe),
            "M269-D002-DYN-04",
            f"runtime probe failed: {probe_run.stdout}{probe_run.stderr}",
            failures,
        )
        parsed = parse_probe_output(probe_run.stdout)
        expected = {
            "copy_status": "0",
            "spawn_group": "111",
            "scope": "1",
            "add_task": "1",
            "cancelled": "0",
            "wait_next": "23",
            "hop": "23",
            "cancel_all": "31",
            "on_cancel": "41",
            "spawn_detached": "121",
            "spawn_call_count": "2",
            "scope_call_count": "1",
            "add_task_call_count": "1",
            "wait_next_call_count": "1",
            "cancel_all_call_count": "1",
            "cancellation_poll_call_count": "1",
            "on_cancel_call_count": "1",
            "executor_hop_call_count": "1",
            "last_spawn_kind": "2",
            "last_spawn_executor_tag": "3",
            "last_wait_next_result": "23",
            "last_executor_hop_executor_tag": "2",
            "last_executor_hop_value": "23",
        }
        for index, (key, value) in enumerate(expected.items(), start=5):
            checks_total += 1
            checks_passed += require(
                parsed.get(key) == value,
                display_path(probe_exe),
                f"M269-D002-DYN-{index:02d}",
                f"unexpected {key}: {parsed}",
                failures,
            )

    retained_frontdoor_probe: dict[str, Any] = {}
    if NATIVE_EXE.exists():
        retained_dir = PROBE_ROOT / "retained_frontdoor_probe"
        retained_dir.mkdir(parents=True, exist_ok=True)
        retained_run = run_command([
            str(NATIVE_EXE),
            str(FIXTURE),
            "--out-dir",
            str(retained_dir),
            "--emit-prefix",
            "module",
        ])
        diagnostics_path = retained_dir / "module.diagnostics.txt"
        diagnostics_text = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""
        retained_frontdoor_probe = {
            "returncode": retained_run.returncode,
            "diagnostics_path": display_path(diagnostics_path),
            "diagnostics": diagnostics_text,
        }
        checks_total += 1
        checks_passed += require(
            "O3S260" in diagnostics_text or "O3L300" in diagnostics_text,
            display_path(diagnostics_path),
            "M269-D002-DYN-29",
            "retained front-door probe should stay explicitly fail-closed if it does not compile through end-to-end",
            failures,
        )

    return checks_passed, checks_total, {
        "runtime_probe_exe": display_path(probe_exe),
        "runtime_probe_output": parsed,
        "retained_frontdoor_probe": retained_frontdoor_probe,
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
    for index, symbol in enumerate((*HELPER_SYMBOLS, *RUNTIME_TEST_SYMBOLS), start=1):
        checks_total += 1
        checks_passed += require(
            symbol not in runtime_header_text,
            display_path(RUNTIME_HEADER),
            f"M269-D002-PUB-{index:02d}",
            f"public runtime header must not expose helper symbol {symbol}",
            failures,
        )
        checks_total += 1
        checks_passed += require(
            symbol in runtime_internal_text,
            display_path(RUNTIME_INTERNAL),
            f"M269-D002-PRI-{index:02d}",
            f"private runtime header must expose helper symbol {symbol}",
            failures,
        )

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_passed, dynamic_total, dynamic_payload = run_dynamic_checks(failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

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
