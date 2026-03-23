#!/usr/bin/env python3
"""Checker for M267-D002 live catch, bridge, and runtime integration."""

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
MODE = "m267-d002-live-catch-bridge-and-runtime-integration-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-part6-live-error-runtime-integration/m267-d002-v1"
BOUNDARY_PREFIX = "; part6_live_error_runtime_integration = contract=" + CONTRACT_ID
NAMED_METADATA_LINE = "!objc3.objc_part6_live_error_runtime_integration = !{!90}"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-D002" / "live_error_runtime_integration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_live_catch_bridge_and_runtime_integration_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation_packet.md"
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
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_d002_live_error_runtime_integration_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m267_d002_live_error_runtime_integration_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "d002"
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
        SnippetCheck("M267-D002-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-D002-EXP-02", "`!objc3.objc_part6_live_error_runtime_integration = !{!90}`"),
        SnippetCheck("M267-D002-EXP-03", "`tests/tooling/runtime/m267_d002_live_error_runtime_integration_probe.cpp`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-D002-PKT-01", "Packet: `M267-D002`"),
        SnippetCheck("M267-D002-PKT-02", "Issue: `#7278`"),
        SnippetCheck("M267-D002-PKT-03", "`M267-D003`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M267-D002-DOCSRC-01", "## M267 Part 6 live catch, bridge, and runtime integration (M267-D002)"),
        SnippetCheck("M267-D002-DOCSRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M267-D002-DOCSRC-03", "`!objc3.objc_part6_live_error_runtime_integration = !{!90}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M267-D002-NDOC-01", "## M267 Part 6 live catch, bridge, and runtime integration (M267-D002)"),
        SnippetCheck("M267-D002-NDOC-02", f"`{CONTRACT_ID}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M267-D002-LSPEC-01", "## M267 Part 6 live catch, bridge, and runtime integration (D002)"),
        SnippetCheck("M267-D002-LSPEC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M267-D002-LSPEC-03", "`objc3_runtime_copy_error_bridge_state_for_testing`"),
    ),
    SEMANTIC_SPEC: (
        SnippetCheck("M267-D002-SEM-01", "M267-D002 live-runtime note:"),
        SnippetCheck("M267-D002-SEM-02", "`!objc3.objc_part6_live_error_runtime_integration = !{!90}`"),
    ),
    SYNTAX_SPEC: (
        SnippetCheck("M267-D002-SYN-01", "Current implementation status (`M267-D002`):"),
        SnippetCheck("M267-D002-SYN-02", f"`{CONTRACT_ID}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M267-D002-ARCH-01", "## M267 Part 6 Live Catch, Bridge, And Runtime Integration (D002)"),
        SnippetCheck("M267-D002-ARCH-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M267-D002-ARCH-03", "the next issue is `M267-D003`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M267-D002-RTR-01", "## M267 live error runtime integration probe"),
        SnippetCheck("M267-D002-RTR-02", "tests/tooling/runtime/m267_d002_live_error_runtime_integration_probe.cpp"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M267-D002-LHDR-01", "kObjc3Part6LiveErrorRuntimeIntegrationContractId"),
        SnippetCheck("M267-D002-LHDR-02", "kObjc3Part6LiveErrorRuntimeIntegrationPackagingModel"),
        SnippetCheck("M267-D002-LHDR-03", "Objc3Part6LiveErrorRuntimeIntegrationSummary()"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M267-D002-LCPP-01", "Objc3Part6LiveErrorRuntimeIntegrationSummary()"),
        SnippetCheck("M267-D002-LCPP-02", "M267-D002 live catch/bridge/runtime integration anchor"),
        SnippetCheck("M267-D002-LCPP-03", ";next_issue=M267-D003"),
    ),
    IR_EMITTER: (
        SnippetCheck("M267-D002-IR-01", 'out << "; part6_live_error_runtime_integration = "'),
        SnippetCheck("M267-D002-IR-02", '!objc3.objc_part6_live_error_runtime_integration = !{!90}'),
        SnippetCheck("M267-D002-IR-03", 'out << "!90 = !{!\\""'),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M267-D002-RIH-01", "M267-D002 live catch/bridge/runtime integration anchor:"),
        SnippetCheck("M267-D002-RIH-02", "objc3_runtime_copy_error_bridge_state_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M267-D002-RCPP-01", "M267-D002 live catch/bridge/runtime integration anchor:"),
        SnippetCheck("M267-D002-RCPP-02", "object-code probes now execute through these helpers"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M267-D002-PROC-01", "M267-D002 live catch/bridge/runtime integration anchor:"),
        SnippetCheck("M267-D002-PROC-02", "runtime-library archive path"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-D002-PKG-01", '"check:objc3c:m267-d002-live-catch-bridge-and-runtime-integration-core-feature-implementation": "python scripts/check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py"'),
        SnippetCheck("M267-D002-PKG-02", '"test:tooling:m267-d002-live-catch-bridge-and-runtime-integration-core-feature-implementation": "python -m pytest tests/tooling/test_check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py -q"'),
        SnippetCheck("M267-D002-PKG-03", '"check:objc3c:m267-d002-lane-d-readiness": "python scripts/run_m267_d002_lane_d_readiness.py"'),
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


def ensure_native_build(failures: list[Finding], summary_out: Path) -> tuple[int, int, dict[str, Any]]:
    ensure_summary = summary_out.parent / "ensure_objc3c_native_build_summary.json"
    completed = run_command(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m267-d002-dynamic-check",
            "--summary-out",
            str(ensure_summary),
        ]
    )
    passed = require(
        completed.returncode == 0,
        display_path(BUILD_HELPER),
        "M267-D002-BUILD-01",
        f"fast build failed: {completed.stdout}{completed.stderr}",
        failures,
    )
    return 1, passed, {"ensure_summary": display_path(ensure_summary), "returncode": completed.returncode}


def run_dynamic_checks(failures: list[Finding], summary_out: Path) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    payload: dict[str, Any] = {}

    total, passed, ensure_payload = ensure_native_build(failures, summary_out)
    checks_total += total
    checks_passed += passed
    payload["ensure_native_build"] = ensure_payload
    if passed != total:
        return checks_passed, checks_total, payload

    fixture_dir = PROBE_ROOT / "fixture"
    fixture_dir.mkdir(parents=True, exist_ok=True)
    compile_command = [str(NATIVE_EXE), str(FIXTURE), "--out-dir", str(fixture_dir), "--emit-prefix", "module"]
    compiled = run_command(compile_command)
    payload["compile_command"] = compile_command
    payload["compile_exit_code"] = compiled.returncode
    checks_total += 1
    checks_passed += require(compiled.returncode == 0, display_path(FIXTURE), "M267-D002-DYN-01", f"native compile failed: {compiled.stdout}{compiled.stderr}", failures)

    ir_path = fixture_dir / "module.ll"
    manifest_path = fixture_dir / "module.manifest.json"
    object_path = fixture_dir / "module.obj"
    backend_path = fixture_dir / "module.object-backend.txt"
    registration_manifest_path = fixture_dir / "module.runtime-registration-manifest.json"

    for check_id, path, detail in (
        ("M267-D002-DYN-02", ir_path, "module.ll missing"),
        ("M267-D002-DYN-03", manifest_path, "module.manifest.json missing"),
        ("M267-D002-DYN-04", object_path, "module.obj missing"),
        ("M267-D002-DYN-05", backend_path, "module.object-backend.txt missing"),
        ("M267-D002-DYN-06", registration_manifest_path, "module.runtime-registration-manifest.json missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)

    if backend_path.exists():
        backend = read_text(backend_path).strip()
        payload["object_backend"] = backend
        checks_total += 1
        checks_passed += require(backend == "llvm-direct", display_path(backend_path), "M267-D002-DYN-07", f"unexpected object backend: {backend}", failures)

    if ir_path.exists():
        ir_text = read_text(ir_path)
        payload["boundary_present"] = BOUNDARY_PREFIX in ir_text
        payload["named_metadata_present"] = NAMED_METADATA_LINE in ir_text
        for check_id, condition, detail in (
            ("M267-D002-DYN-08", BOUNDARY_PREFIX in ir_text, "IR boundary line missing"),
            ("M267-D002-DYN-09", NAMED_METADATA_LINE in ir_text, "IR named metadata line missing"),
            ("M267-D002-DYN-10", "call void @objc3_runtime_store_thrown_error_i32(" in ir_text, "IR must call thrown-error store helper"),
            ("M267-D002-DYN-11", "call i32 @objc3_runtime_load_thrown_error_i32(" in ir_text, "IR must call thrown-error load helper"),
            ("M267-D002-DYN-12", "call i32 @objc3_runtime_bridge_status_error_i32(" in ir_text, "IR must call status-bridge helper"),
            ("M267-D002-DYN-13", "call i32 @objc3_runtime_catch_matches_error_i32(" in ir_text, "IR must call catch-match helper"),
        ):
            checks_total += 1
            checks_passed += require(condition, display_path(ir_path), check_id, detail, failures)

    if registration_manifest_path.exists():
        registration_manifest = load_json(registration_manifest_path)
        runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
        payload["runtime_support_library_archive_relative_path"] = runtime_library_relative_path
        checks_total += 1
        checks_passed += require(
            runtime_library_relative_path == "artifacts/lib/objc3_runtime.lib",
            display_path(registration_manifest_path),
            "M267-D002-DYN-14",
            "runtime registration manifest must publish artifacts/lib/objc3_runtime.lib",
            failures,
        )

    probe_exe = fixture_dir / "live_error_runtime_probe.exe"
    clangxx = resolve_clangxx()
    link_command = [
        clangxx,
        "-std=c++20",
        "-I",
        str(INCLUDE_ROOT),
        str(RUNTIME_PROBE),
        str(object_path),
        str(RUNTIME_LIB),
        "-o",
        str(probe_exe),
    ]
    linked = run_command(link_command)
    payload["link_command"] = link_command
    payload["link_exit_code"] = linked.returncode
    payload["link_stderr"] = linked.stderr
    checks_total += 1
    checks_passed += require(linked.returncode == 0, display_path(probe_exe), "M267-D002-DYN-15", f"probe link failed: {linked.stdout}{linked.stderr}", failures)

    if linked.returncode == 0:
        ran = run_command([str(probe_exe)])
        payload["probe_exit_code"] = ran.returncode
        payload["probe_stdout"] = ran.stdout
        checks_total += 1
        checks_passed += require(ran.returncode == 0, display_path(probe_exe), "M267-D002-DYN-16", f"probe execution failed: {ran.stdout}{ran.stderr}", failures)
        try:
            probe_payload = json.loads(ran.stdout)
        except json.JSONDecodeError as exc:
            probe_payload = {}
            failures.append(Finding(display_path(probe_exe), "M267-D002-DYN-17", f"probe output is not valid JSON: {exc}: {ran.stdout}"))
            checks_total += 1
        else:
            payload["probe_payload"] = probe_payload
            for check_id, key, expected in (
                ("M267-D002-DYN-18", "rc", 54),
                ("M267-D002-DYN-19", "status", 0),
                ("M267-D002-DYN-20", "store_call_count", 1),
                ("M267-D002-DYN-21", "load_call_count", 1),
                ("M267-D002-DYN-22", "status_bridge_call_count", 1),
                ("M267-D002-DYN-23", "nserror_bridge_call_count", 0),
                ("M267-D002-DYN-24", "catch_match_call_count", 1),
                ("M267-D002-DYN-25", "last_stored_error_value", 45),
                ("M267-D002-DYN-26", "last_loaded_error_value", 45),
                ("M267-D002-DYN-27", "last_status_bridge_status_value", 5),
                ("M267-D002-DYN-28", "last_status_bridge_error_value", 45),
                ("M267-D002-DYN-29", "last_catch_match_kind", 1),
                ("M267-D002-DYN-30", "last_catch_match_is_catch_all", 0),
                ("M267-D002-DYN-31", "last_catch_match_result", 1),
                ("M267-D002-DYN-32", "last_catch_kind_name", "nserror"),
            ):
                checks_total += 1
                checks_passed += require(
                    probe_payload.get(key) == expected,
                    display_path(probe_exe),
                    check_id,
                    f"expected {key}={expected!r}, saw {probe_payload.get(key)!r}",
                    failures,
                )

    return checks_passed, checks_total, payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic: dict[str, Any] = {"executed": False}
    if not args.skip_dynamic_probes:
        dynamic["executed"] = True
        passed, total, payload = run_dynamic_checks(failures, args.summary_out)
        checks_passed += passed
        checks_total += total
        dynamic["live_runtime_probe"] = payload

    summary = {
        "mode": MODE,
        "issue": "M267-D002",
        "contract_id": CONTRACT_ID,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "checks_total": checks_total,
        "ok": not failures,
        "dynamic_probes_executed": dynamic["executed"],
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
