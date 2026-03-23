#!/usr/bin/env python3
"""Checker for M270-D002 live actor mailbox and isolation runtime implementation."""

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
MODE = "m270-d002-part7-live-actor-mailbox-runtime-v1"
CONTRACT_ID = "objc3c-part7-live-actor-mailbox-and-isolation-runtime/m270-d002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m270" / "M270-D002" / "live_actor_mailbox_runtime_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_live_actor_mailbox_and_isolation_runtime_implementation_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_d002_live_actor_mailbox_and_isolation_runtime_implementation_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SEMANTIC_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
CONFORMANCE_SPEC = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_d002_live_actor_mailbox_runtime_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m270_d002_live_actor_mailbox_runtime_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "d002"
INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"


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
        SnippetCheck("M270-D002-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M270-D002-EXP-02", "objc3_runtime_actor_runtime_state_snapshot"),
        SnippetCheck("M270-D002-EXP-03", "M270-D003"),
    ),
    PACKET_DOC: (
        SnippetCheck("M270-D002-PKT-01", "Packet: `M270-D002`"),
        SnippetCheck("M270-D002-PKT-02", "Issue: `#7316`"),
        SnippetCheck("M270-D002-PKT-03", "M270-D003"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M270-D002-DOCSRC-01", "## M270 live actor mailbox and isolation runtime"),
        SnippetCheck("M270-D002-DOCSRC-02", "actor_mailbox_enqueue(...)"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M270-D002-DOC-01", "## M270 live actor mailbox and isolation runtime"),
        SnippetCheck("M270-D002-DOC-02", "artifacts/lib/objc3_runtime.lib"),
    ),
    SEMANTIC_SPEC: (
        SnippetCheck("M270-D002-SEM-01", "M270-D002 live-runtime note:"),
        SnippetCheck("M270-D002-SEM-02", "mailbox binding, enqueue,"),
    ),
    CONFORMANCE_SPEC: (
        SnippetCheck("M270-D002-CONF-01", "M270-D002 live-runtime note:"),
        SnippetCheck("M270-D002-CONF-02", "M270-D003"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M270-D002-ARCH-01", "## M270 Part 7 Live Actor Mailbox And Isolation Runtime (D002)"),
        SnippetCheck("M270-D002-ARCH-02", "objc3_runtime_actor_mailbox_enqueue_i32"),
        SnippetCheck("M270-D002-ARCH-03", "the next issue is `M270-D003`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M270-D002-RTR-01", "## M270 live actor mailbox runtime probe"),
        SnippetCheck("M270-D002-RTR-02", "tests/tooling/runtime/m270_d002_live_actor_mailbox_runtime_probe.cpp"),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M270-D002-RIH-01", "M270-D002 actor-mailbox/isolation-runtime anchor:"),
        SnippetCheck("M270-D002-RIH-02", "objc3_runtime_actor_mailbox_enqueue_i32"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M270-D002-RCPP-01", "M270-D002 actor-mailbox/isolation-runtime anchor:"),
        SnippetCheck("M270-D002-RCPP-02", "objc3_runtime_actor_mailbox_enqueue_i32"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M270-D002-IR-01", "actor_bind_executor"),
        SnippetCheck("M270-D002-IR-02", "kObjc3RuntimeActorMailboxDrainNextI32Symbol"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M270-D002-LOW-01", "kObjc3RuntimeActorBindExecutorI32Symbol"),
        SnippetCheck("M270-D002-LOW-02", "kObjc3RuntimeActorMailboxDrainNextI32Symbol"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M270-D002-PROC-01", "M270-D002 actor-mailbox/isolation-runtime anchor:"),
        SnippetCheck("M270-D002-PROC-02", "mailbox helpers become runnable"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M270-D002-PKG-01", '"check:objc3c:m270-d002-live-actor-mailbox-and-isolation-runtime-implementation-core-feature-implementation"'),
        SnippetCheck("M270-D002-PKG-02", '"test:tooling:m270-d002-live-actor-mailbox-and-isolation-runtime-implementation-core-feature-implementation"'),
        SnippetCheck("M270-D002-PKG-03", '"check:objc3c:m270-d002-lane-d-readiness"'),
    ),
    FIXTURE: (
        SnippetCheck("M270-D002-FIX-01", "actor_mailbox_enqueue"),
        SnippetCheck("M270-D002-FIX-02", "actor_mailbox_drain_next"),
        SnippetCheck("M270-D002-FIX-03", "actor_nonisolated_entry"),
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
        "m270-d002-dynamic-check",
        "--summary-out",
        str(ROOT / "tmp" / "reports" / "m270" / "M270-D002" / "ensure_build_summary.json"),
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        display_path(BUILD_HELPER),
        "M270-D002-DYN-01",
        f"fast build failed: {ensure_build.stdout}{ensure_build.stderr}",
        failures,
    )

    fixture_dir = PROBE_ROOT / "fixture"
    fixture_dir.mkdir(parents=True, exist_ok=True)
    compiled = run_command([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(fixture_dir),
        "--emit-prefix",
        "module",
    ])
    checks_total += 1
    checks_passed += require(
        compiled.returncode == 0,
        display_path(FIXTURE),
        "M270-D002-DYN-02",
        f"native compile failed: {compiled.stdout}{compiled.stderr}",
        failures,
    )

    ir_path = fixture_dir / "module.ll"
    manifest_path = fixture_dir / "module.runtime-registration-manifest.json"
    for check_id, path, detail in (
        ("M270-D002-DYN-03", ir_path, "module.ll missing"),
        ("M270-D002-DYN-04", manifest_path, "module.runtime-registration-manifest.json missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)

    if ir_path.exists():
        ir_text = read_text(ir_path)
        for check_id, snippet in (
            ("M270-D002-DYN-05", "objc3_runtime_actor_mailbox_enqueue_i32"),
            ("M270-D002-DYN-06", "objc3_runtime_actor_mailbox_drain_next_i32"),
        ):
            checks_total += 1
            checks_passed += require(
                snippet in ir_text,
                display_path(ir_path),
                check_id,
                f"missing IR snippet: {snippet}",
                failures,
            )

    if manifest_path.exists():
        manifest = json.loads(read_text(manifest_path))
        runtime_library_relative_path = manifest.get("runtime_support_library_archive_relative_path")
        checks_total += 1
        checks_passed += require(
            runtime_library_relative_path == "artifacts/lib/objc3_runtime.lib",
            display_path(manifest_path),
            "M270-D002-DYN-07",
            f"unexpected runtime library path: {runtime_library_relative_path}",
            failures,
        )

    probe_dir = PROBE_ROOT / "runtime_probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m270_d002_live_actor_mailbox_runtime_probe.exe"
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
        "M270-D002-DYN-08",
        f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}",
        failures,
    )

    probe_run_output: dict[str, str] = {}
    if probe_exe.exists():
        probe_run = run_command([str(probe_exe)])
        checks_total += 1
        checks_passed += require(
            probe_run.returncode == 0,
            display_path(probe_exe),
            "M270-D002-DYN-09",
            f"probe run failed: {probe_run.stdout}{probe_run.stderr}",
            failures,
        )
        probe_run_output = parse_probe_output(probe_run.stdout)
        expected_pairs = {
            "copy_status": "0",
            "replay": "1",
            "guard": "1",
            "isolation": "1",
            "bound": "1",
            "enqueued": "23",
            "drained": "23",
            "bind_executor_call_count": "1",
            "mailbox_enqueue_call_count": "1",
            "mailbox_drain_call_count": "1",
            "last_bound_actor_handle": "41",
            "last_bound_executor_tag": "1",
            "last_mailbox_actor_handle": "41",
            "last_mailbox_enqueued_value": "23",
            "last_mailbox_executor_tag": "1",
            "last_mailbox_depth": "0",
            "last_mailbox_drained_value": "23",
        }
        for key, expected in expected_pairs.items():
            checks_total += 1
            checks_passed += require(
                probe_run_output.get(key) == expected,
                display_path(probe_exe),
                f"M270-D002-DYN-CHECK-{key}",
                f"unexpected probe value for {key}: {probe_run_output.get(key)!r}",
                failures,
            )

    return checks_total, checks_passed, {
        "fixture_dir": display_path(fixture_dir),
        "probe_exe": display_path(probe_exe),
        "probe_output": probe_run_output,
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_details: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_details = run_dynamic_checks(failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_details,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
