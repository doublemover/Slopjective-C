#!/usr/bin/env python3
"""Checker for M273-D001 expansion host/runtime boundary freeze."""

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
MODE = "m273-d001-part10-expansion-host-runtime-boundary-freeze-v1"
CONTRACT_ID = "objc3c-part10-expansion-host-runtime-boundary/m273-d001-v1"
BOUNDARY_PREFIX = "; part10_expansion_host_runtime_boundary = "
NAMED_METADATA = "!objc3.objc_part10_expansion_host_and_runtime_boundary = !{!107}"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m273" / "M273-D001" / "expansion_host_runtime_boundary_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_expansion_host_and_runtime_boundary_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_d001_expansion_host_and_runtime_boundary_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_d001_expansion_host_runtime_boundary_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m273_d001_expansion_host_runtime_boundary_probe.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m273_d001_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m273_d001_expansion_host_and_runtime_boundary_contract_and_architecture_freeze.py"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "d001"
INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
SNAPSHOT_SYMBOL = "objc3_runtime_copy_part10_expansion_host_boundary_snapshot_for_testing"


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
        SnippetCheck("M273-D001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M273-D001-EXP-02", "Issue: `#7356`"),
        SnippetCheck("M273-D001-EXP-03", "part10_expansion_host_runtime_boundary"),
    ),
    PACKET_DOC: (
        SnippetCheck("M273-D001-PKT-01", "Packet: `M273-D001`"),
        SnippetCheck("M273-D001-PKT-02", "Issue: `#7356`"),
        SnippetCheck("M273-D001-PKT-03", "Next issue: `M273-D002`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M273-D001-DOCSRC-01", "## M273 expansion host and runtime boundary"),
        SnippetCheck("M273-D001-DOCSRC-02", "objc3_runtime_copy_part10_expansion_host_boundary_snapshot_for_testing"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M273-D001-DOC-01", "## M273 expansion host and runtime boundary"),
        SnippetCheck("M273-D001-DOC-02", "artifacts/lib/objc3_runtime.lib"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M273-D001-ATTR-01", "## M273 expansion host and runtime boundary contract (D001)"),
        SnippetCheck("M273-D001-ATTR-02", SNAPSHOT_SYMBOL),
    ),
    SPEC_METADATA: (
        SnippetCheck("M273-D001-META-01", "## M273 expansion host/runtime boundary note"),
        SnippetCheck("M273-D001-META-02", CONTRACT_ID),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M273-D001-LHDR-01", "kObjc3Part10ExpansionHostRuntimeBoundaryContractId"),
        SnippetCheck("M273-D001-LHDR-02", "Objc3Part10ExpansionHostRuntimeBoundarySummary"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M273-D001-LCPP-01", "Objc3Part10ExpansionHostRuntimeBoundarySummary"),
        SnippetCheck("M273-D001-LCPP-02", "M273-D001 host/runtime-boundary anchor"),
    ),
    IR_CPP: (
        SnippetCheck("M273-D001-IR-01", 'out << "; part10_expansion_host_runtime_boundary = "'),
        SnippetCheck("M273-D001-IR-02", NAMED_METADATA),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M273-D001-RIH-01", "M273-D001 expansion-host/runtime-boundary anchor"),
        SnippetCheck("M273-D001-RIH-02", SNAPSHOT_SYMBOL),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M273-D001-RCPP-01", "M273-D001 expansion-host/runtime-boundary anchor"),
        SnippetCheck("M273-D001-RCPP-02", SNAPSHOT_SYMBOL),
    ),
    PROCESS_CPP: (
        SnippetCheck("M273-D001-PROC-01", "M273-D001 expansion-host/runtime-boundary anchor"),
        SnippetCheck("M273-D001-PROC-02", "objc3_runtime.lib archive"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M273-D001-PKG-01", '"check:objc3c:m273-d001-expansion-host-and-runtime-boundary-contract-and-architecture-freeze"'),
        SnippetCheck("M273-D001-PKG-02", '"test:tooling:m273-d001-expansion-host-and-runtime-boundary-contract-and-architecture-freeze"'),
        SnippetCheck("M273-D001-PKG-03", '"check:objc3c:m273-d001-lane-d-readiness"'),
    ),
    FIXTURE: (
        SnippetCheck("M273-D001-FIX-01", "objc_macro(named(\"Trace\"))"),
        SnippetCheck("M273-D001-FIX-02", "behavior=Observed"),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M273-D001-PROBE-01", SNAPSHOT_SYMBOL),
        SnippetCheck("M273-D001-PROBE-02", "macro_host_execution_ready"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M273-D001-RUN-01", "M273-C003 + M273-D001"),
        SnippetCheck("M273-D001-RUN-02", "check_m273_d001_expansion_host_and_runtime_boundary_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M273-D001-TEST-01", "def test_checker_passes_static() -> None:"),
        SnippetCheck("M273-D001-TEST-02", "def test_checker_passes_dynamic() -> None:"),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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


def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


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


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def validate_public_private_boundary(failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    runtime_header_text = read_text(RUNTIME_HEADER)
    runtime_internal_text = read_text(RUNTIME_INTERNAL)
    checks_total += 1
    checks_passed += require(SNAPSHOT_SYMBOL not in runtime_header_text, display_path(RUNTIME_HEADER), "M273-D001-PUB-01", f"public runtime header must not expose {SNAPSHOT_SYMBOL}", failures)
    checks_total += 1
    checks_passed += require(SNAPSHOT_SYMBOL in runtime_internal_text, display_path(RUNTIME_INTERNAL), "M273-D001-PRI-01", f"private runtime header must expose {SNAPSHOT_SYMBOL}", failures)
    return checks_passed, checks_total


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, artifact: str, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    ensure_build = run_command([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m273-d001-dynamic-check",
        "--summary-out",
        str(ROOT / "tmp" / "reports" / "m273" / "M273-D001" / "ensure_build_summary.json"),
    ])
    check(ensure_build.returncode == 0, display_path(BUILD_HELPER), "M273-D001-DYN-01", f"fast build failed: {ensure_build.stdout}{ensure_build.stderr}")
    if ensure_build.returncode != 0:
        return checks_passed, checks_total, {"skipped": False}

    fixture_dir = PROBE_ROOT / "fixture"
    fixture_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(fixture_dir),
        "--emit-prefix",
        "module",
    ])
    manifest_path = fixture_dir / "module.runtime-registration-manifest.json"
    ir_path = fixture_dir / "module.ll"
    check(compile_result.returncode == 0, display_path(FIXTURE), "M273-D001-DYN-02", f"native compile failed: {compile_result.stdout}{compile_result.stderr}")
    check(manifest_path.exists(), display_path(manifest_path), "M273-D001-DYN-03", "runtime registration manifest missing")
    check(ir_path.exists(), display_path(ir_path), "M273-D001-DYN-04", "module.ll missing")
    if compile_result.returncode != 0 or not manifest_path.exists() or not ir_path.exists():
        return checks_passed, checks_total, {"skipped": False}

    registration_manifest = json.loads(read_text(manifest_path))
    check(registration_manifest.get("runtime_support_library_archive_relative_path") == "artifacts/lib/objc3_runtime.lib", display_path(manifest_path), "M273-D001-DYN-05", "unexpected runtime library archive path")

    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_PREFIX)), "")
    check(bool(boundary_line), display_path(ir_path), "M273-D001-DYN-06", "missing Part 10 host/runtime boundary IR comment")
    check(NAMED_METADATA in ir_text, display_path(ir_path), "M273-D001-DYN-07", "missing named Part 10 host/runtime metadata")
    for check_id, snippet in (
        ("M273-D001-DYN-08", f"contract={CONTRACT_ID}"),
        ("M273-D001-DYN-09", "property_runtime_ready=true"),
        ("M273-D001-DYN-10", "macro_host_execution_ready=false"),
        ("M273-D001-DYN-11", "macro_host_process_launch_ready=false"),
        ("M273-D001-DYN-12", "runtime_package_loader_ready=false"),
        ("M273-D001-DYN-13", "next_issue=M273-D002"),
    ):
        check(snippet in boundary_line, display_path(ir_path), check_id, f"boundary line missing snippet: {snippet}")

    probe_dir = PROBE_ROOT / "runtime_probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m273_d001_expansion_host_runtime_boundary_probe.exe"
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
    check(probe_compile.returncode == 0, display_path(RUNTIME_PROBE), "M273-D001-DYN-14", f"runtime probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {"skipped": False}

    probe_run = run_command([str(probe_exe)])
    check(probe_run.returncode == 0, display_path(probe_exe), "M273-D001-DYN-15", f"runtime probe failed: {probe_run.stdout}{probe_run.stderr}")
    parsed = parse_probe_output(probe_run.stdout)
    expected = {
        "copy_status": "0",
        "property_runtime_ready": "1",
        "macro_host_execution_ready": "0",
        "macro_host_process_launch_ready": "0",
        "runtime_package_loader_ready": "0",
        "deterministic": "1",
        "runtime_support_library_archive_relative_path": "artifacts/lib/objc3_runtime.lib",
        "property_behavior_runtime_model": "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks",
        "macro_expansion_host_model": "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed",
        "packaging_model": "native-driver-packaging-still-hands-off-part10-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests",
        "fail_closed_model": "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet",
    }
    for index, (key, value) in enumerate(expected.items(), start=16):
        check(parsed.get(key) == value, display_path(probe_exe), f"M273-D001-DYN-{index:02d}", f"unexpected {key}: {parsed}")

    return checks_passed, checks_total, {
        "skipped": False,
        "fixture_dir": display_path(fixture_dir),
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

    boundary_passed, boundary_total = validate_public_private_boundary(failures)
    checks_total += boundary_total
    checks_passed += boundary_passed

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_passed, dynamic_total, dynamic_payload = run_dynamic_checks(failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "ok": not failures,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic": dynamic_payload,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        print(canonical_json(summary), end="")
        return 1
    print(f"[ok] wrote {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
