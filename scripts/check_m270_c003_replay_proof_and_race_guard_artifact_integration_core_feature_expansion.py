#!/usr/bin/env python3
"""Checker for M270-C003 replay-proof and race-guard artifact integration."""

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
MODE = "m270-c003-part7-actor-replay-proof-race-guard-integration-v1"
CONTRACT_ID = "objc3c-part7-actor-replay-proof-and-race-guard-integration/m270-c003-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m270" / "M270-C003" / "actor_replay_race_guard_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_replay_proof_and_race_guard_artifact_integration_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_c003_replay_proof_and_race_guard_artifact_integration_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SEMANTIC_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
CONFORMANCE_SPEC = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_c003_actor_replay_race_guard_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m270_c003_actor_replay_race_guard_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "c003"
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
        SnippetCheck("M270-C003-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M270-C003-EXP-02", "objc3_runtime_actor_record_replay_proof_i32"),
        SnippetCheck("M270-C003-EXP-03", "M270-D001"),
    ),
    PACKET_DOC: (
        SnippetCheck("M270-C003-PKT-01", "Packet: `M270-C003`"),
        SnippetCheck("M270-C003-PKT-02", "Issue: `#7314`"),
        SnippetCheck("M270-C003-PKT-03", "`M270-D001`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M270-C003-DOCSRC-01", "## M270 replay-proof and race-guard artifact integration"),
        SnippetCheck("M270-C003-DOCSRC-02", "objc3_runtime_actor_record_race_guard_i32"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M270-C003-DOC-01", "## M270 replay-proof and race-guard artifact integration"),
        SnippetCheck("M270-C003-DOC-02", "concurrency_replay_race_guard_lowering"),
    ),
    SEMANTIC_SPEC: (
        SnippetCheck("M270-C003-SEM-01", "M270-C003 artifact-integration note:"),
        SnippetCheck("M270-C003-SEM-02", "objc3_runtime_actor_record_replay_proof_i32"),
    ),
    CONFORMANCE_SPEC: (
        SnippetCheck("M270-C003-CONF-01", "M270-C003 artifact-integration note:"),
        SnippetCheck("M270-C003-CONF-02", "objc3_runtime_actor_record_race_guard_i32"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M270-C003-ARCH-01", "## M270 Part 7 Actor Replay/Race Artifact Integration (C003)"),
        SnippetCheck("M270-C003-ARCH-02", "objc3_runtime_actor_record_replay_proof_i32"),
        SnippetCheck("M270-C003-ARCH-03", "the next issue is `M270-D001`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M270-C003-RTR-01", "## M270 actor replay/race artifact probe"),
        SnippetCheck("M270-C003-RTR-02", "tests/tooling/runtime/m270_c003_actor_replay_race_guard_probe.cpp"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M270-C003-LHDR-01", "kObjc3RuntimeActorRecordReplayProofI32Symbol"),
        SnippetCheck("M270-C003-LHDR-02", "kObjc3RuntimeActorRecordRaceGuardI32Symbol"),
    ),
    IR_EMITTER: (
        SnippetCheck("M270-C003-IR-01", "kObjc3RuntimeActorRecordReplayProofI32Symbol"),
        SnippetCheck("M270-C003-IR-02", "kObjc3RuntimeActorRecordRaceGuardI32Symbol"),
        SnippetCheck("M270-C003-IR-03", "concurrency_replay_race_guard_lowering"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M270-C003-ART-01", "M270-C003 implementation anchor:"),
        SnippetCheck("M270-C003-ART-02", "strict-concurrency replay/race-guard lowering packet"),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M270-C003-RIH-01", "objc3_runtime_actor_record_replay_proof_i32"),
        SnippetCheck("M270-C003-RIH-02", "objc3_runtime_actor_record_race_guard_i32"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M270-C003-RCPP-01", "objc3_runtime_actor_record_replay_proof_i32"),
        SnippetCheck("M270-C003-RCPP-02", "objc3_runtime_actor_record_race_guard_i32"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M270-C003-PKG-01", '"check:objc3c:m270-c003-replay-proof-and-race-guard-artifact-integration-core-feature-expansion"'),
        SnippetCheck("M270-C003-PKG-02", '"test:tooling:m270-c003-replay-proof-and-race-guard-artifact-integration-core-feature-expansion"'),
        SnippetCheck("M270-C003-PKG-03", '"check:objc3c:m270-c003-lane-c-readiness"'),
    ),
    FIXTURE: (
        SnippetCheck("M270-C003-FIX-01", "replay_proof_step"),
        SnippetCheck("M270-C003-FIX-02", "race_guard_lock"),
        SnippetCheck("M270-C003-FIX-03", "actor_enter_isolation_thunk"),
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
        "m270-c003-dynamic-check",
        "--summary-out",
        str(ROOT / "tmp" / "reports" / "m270" / "M270-C003" / "ensure_build_summary.json"),
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        display_path(BUILD_HELPER),
        "M270-C003-DYN-01",
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
        "M270-C003-DYN-02",
        f"native compile failed: {compiled.stdout}{compiled.stderr}",
        failures,
    )

    ir_path = fixture_dir / "module.ll"
    object_path = fixture_dir / "module.obj"
    backend_path = fixture_dir / "module.object-backend.txt"
    for check_id, path, detail in (
        ("M270-C003-DYN-03", ir_path, "module.ll missing"),
        ("M270-C003-DYN-04", object_path, "module.obj missing"),
        ("M270-C003-DYN-05", backend_path, "module.object-backend.txt missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)

    if backend_path.exists():
        backend = read_text(backend_path).strip()
        checks_total += 1
        checks_passed += require(
            backend == "llvm-direct",
            display_path(backend_path),
            "M270-C003-DYN-06",
            f"unexpected object backend: {backend}",
            failures,
        )

    if ir_path.exists():
        ir_text = read_text(ir_path)
        for index, snippet in enumerate((
            "call i32 @objc3_runtime_actor_record_replay_proof_i32(",
            "call i32 @objc3_runtime_actor_record_race_guard_i32(",
            "call i32 @objc3_runtime_actor_enter_isolation_thunk_i32(",
            "call i32 @objc3_runtime_actor_hop_to_executor_i32(",
            "; actor_lowering_metadata_contract = ",
            "; concurrency_replay_race_guard_lowering = ",
            "; frontend_objc_concurrency_replay_race_guard_lowering_profile = ",
        ), start=7):
            checks_total += 1
            checks_passed += require(
                snippet in ir_text,
                display_path(ir_path),
                f"M270-C003-DYN-{index:02d}",
                f"IR missing snippet: {snippet}",
                failures,
            )

    probe_dir = PROBE_ROOT / "runtime_probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m270_c003_actor_replay_race_guard_probe.exe"
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
        "M270-C003-DYN-14",
        f"runtime probe compile failed: {probe_compile.stdout}{probe_compile.stderr}",
        failures,
    )

    parsed: dict[str, str] = {}
    if probe_compile.returncode == 0:
        probe_run = run_command([str(probe_exe)])
        checks_total += 1
        checks_passed += require(
            probe_run.returncode == 0,
            display_path(probe_exe),
            "M270-C003-DYN-15",
            f"runtime probe failed: {probe_run.stdout}{probe_run.stderr}",
            failures,
        )
        parsed = parse_probe_output(probe_run.stdout)
        expected = {
            "copy_status": "0",
            "replay": "1",
            "guard": "1",
            "isolation": "1",
            "nonisolated": "11",
            "hopped": "13",
            "replay_proof_call_count": "1",
            "race_guard_call_count": "1",
            "isolation_thunk_call_count": "1",
            "nonisolated_entry_call_count": "1",
            "hop_to_executor_call_count": "1",
            "last_replay_proof_executor_tag": "1",
            "last_race_guard_executor_tag": "1",
            "last_isolation_executor_tag": "1",
            "last_nonisolated_value": "11",
            "last_nonisolated_executor_tag": "0",
            "last_hop_value": "13",
            "last_hop_executor_tag": "1",
            "last_hop_result": "13",
        }
        for offset, (key, value) in enumerate(expected.items(), start=16):
            checks_total += 1
            checks_passed += require(
                parsed.get(key) == value,
                display_path(probe_exe),
                f"M270-C003-DYN-{offset:02d}",
                f"unexpected {key}: {parsed}",
                failures,
            )

    return checks_passed, checks_total, {
        "runtime_probe_output": parsed,
        "fixture_dir": display_path(fixture_dir),
        "runtime_probe_exe": display_path(probe_exe),
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    runtime_header_text = read_text(ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h")
    runtime_internal_text = read_text(RUNTIME_INTERNAL)
    for index, symbol in enumerate((
        "objc3_runtime_actor_record_replay_proof_i32",
        "objc3_runtime_actor_record_race_guard_i32",
        "objc3_runtime_copy_actor_runtime_state_for_testing",
    ), start=1):
        checks_total += 1
        checks_passed += require(
            symbol not in runtime_header_text,
            "native/objc3c/src/runtime/objc3_runtime.h",
            f"M270-C003-PUB-{index:02d}",
            f"public runtime header must not expose helper symbol {symbol}",
            failures,
        )
        checks_total += 1
        checks_passed += require(
            symbol in runtime_internal_text,
            display_path(RUNTIME_INTERNAL),
            f"M270-C003-PRI-{index:02d}",
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
