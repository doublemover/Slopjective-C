#!/usr/bin/env python3
"""Checker for M271-D002 live cleanup/runtime integration."""

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
MODE = "m271-d002-live-cleanup-retainable-runtime-integration-v1"
CONTRACT_ID = "objc3c-part8-live-cleanup-retainable-runtime-integration/m271-d002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m271" / "M271-D002" / "live_cleanup_retainable_runtime_integration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_live_cleanup_helpers_and_retainable_family_integration_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_d002_live_cleanup_helpers_and_retainable_family_integration_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_CONFORMANCE = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_d002_live_cleanup_retainable_integration_positive.objc3"
PROBE = ROOT / "tests" / "tooling" / "runtime" / "m271_d002_live_cleanup_retainable_integration_probe.cpp"
CLANGXX = "clang++"
BOUNDARY_PREFIX = "; part8_live_cleanup_retainable_runtime_integration = "
NAMED_METADATA = "!objc3.objc_part8_live_cleanup_retainable_runtime_integration = !{!101}"
IR_SNIPPETS = (
    "define i32 @helperSurface(i32 %arg0, i32 %arg1) {",
    "call void @objc3_runtime_push_autoreleasepool_scope()",
    "call i32 @CFRetain(i32",
    "call i32 @CFAutorelease(i32",
    "call void @CFRelease(i32",
    "call void @objc3_runtime_pop_autoreleasepool_scope()",
    "call void @CloseFd(i32",
    "call void @ReleaseTemp(i32",
)
EXPECTED_PROBE = {
    "result": "77",
    "close_fd_count": "1",
    "last_close_fd_value": "4",
    "release_temp_count": "1",
    "last_release_temp_value": "1",
    "memory_status": "0",
    "arc_status": "0",
    "autoreleasepool_depth": "0",
    "drained_autorelease_value_count": "1",
    "last_drained_autorelease_value": "77",
    "retain_call_count": "1",
    "release_call_count": "1",
    "autorelease_call_count": "1",
    "autoreleasepool_push_count": "1",
    "autoreleasepool_pop_count": "1",
    "last_retain_value": "77",
    "last_release_value": "77",
    "last_autorelease_value": "77",
}


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
        SnippetCheck("M271-D002-EXP-01", "# M271 Live Cleanup Helpers And Retainable-Family Integration Expectations (D002)"),
        SnippetCheck("M271-D002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M271-D002-EXP-03", "Prove direct cleanup execution for `CloseFd` and `ReleaseTemp`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M271-D002-PKT-01", "# M271-D002 Packet: Live Cleanup Helpers And Retainable-Family Integration - Core Feature Implementation"),
        SnippetCheck("M271-D002-PKT-02", "link a runtime probe against the emitted object"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M271-D002-DOCSRC-01", "## M271 live cleanup helpers and retainable-family integration"),
        SnippetCheck("M271-D002-DOCSRC-02", "Current implementation status (`M271-D002`):"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M271-D002-NATDOC-01", "## M271 live cleanup helpers and retainable-family integration"),
        SnippetCheck("M271-D002-NATDOC-02", "Current implementation status (`M271-D002`):"),
    ),
    SPEC_AM: (
        SnippetCheck("M271-D002-AM-01", "M271-D002 live cleanup/runtime integration note:"),
        SnippetCheck("M271-D002-AM-02", "linked runtime probes now observe retain/release/autorelease helper traffic"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M271-D002-ATTR-01", "Current implementation status (`M271-D002`):"),
        SnippetCheck("M271-D002-ATTR-02", "linked runtime probes now call the emitted `helperSurface` function"),
    ),
    SPEC_CONFORMANCE: (
        SnippetCheck("M271-D002-CONF-01", "M271-D002 implementation note:"),
        SnippetCheck("M271-D002-CONF-02", "live probes now observe both direct cleanup execution and retainable-family"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M271-D002-LWH-01", "M271-D002 live runtime-integration anchor:"),
        SnippetCheck("M271-D002-LWH-02", "kObjc3Part8LiveCleanupRetainableIntegrationContractId"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M271-D002-LWC-01", "std::string Objc3Part8LiveCleanupRetainableIntegrationSummary()"),
        SnippetCheck("M271-D002-LWC-02", "linked execution through emitted"),
    ),
    IR_EMITTER: (
        SnippetCheck("M271-D002-IR-01", "; part8_live_cleanup_retainable_runtime_integration = "),
        SnippetCheck("M271-D002-IR-02", NAMED_METADATA),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M271-D002-RTH-01", "M271-D002 live cleanup/runtime integration anchor:"),
        SnippetCheck("M271-D002-RTH-02", "retainable-family stubs proving live helper"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M271-D002-RTC-01", "M271-D002 live cleanup/runtime integration anchor:"),
        SnippetCheck("M271-D002-RTC-02", "calls remain ordinary lowered function calls in the compiled module body."),
    ),
    PROCESS_CPP: (
        SnippetCheck("M271-D002-PRC-01", "M271-D002 live cleanup/runtime integration anchor:"),
        SnippetCheck("M271-D002-PRC-02", "no separate resource-runtime package boundary"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M271-D002-PKG-01", '"check:objc3c:m271-d002-live-cleanup-helpers-and-retainable-family-integration-core-feature-implementation"'),
        SnippetCheck("M271-D002-PKG-02", '"check:objc3c:m271-d002-lane-d-readiness"'),
    ),
}


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


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def parse_probe_output(text: str) -> dict[str, str]:
    values: dict[str, str] = {}
    for raw_line in text.splitlines():
        if "=" not in raw_line:
            continue
        key, value = raw_line.split("=", 1)
        values[key.strip()] = value.strip()
    return values


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run_dynamic_case(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m271-d002-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-D002/ensure_objc3c_native_build_summary.json",
    ])
    total += 1
    passed += require(
        ensure_build.returncode == 0,
        "ensure_objc3c_native_build.py",
        "M271-D002-DYN-01",
        f"fast build failed: {(ensure_build.stdout or '')}{(ensure_build.stderr or '')}",
        failures,
    )
    total += 1
    passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M271-D002-DYN-02", "native compiler missing after build", failures)
    total += 1
    passed += require(RUNTIME_LIBRARY.exists(), display_path(RUNTIME_LIBRARY), "M271-D002-DYN-03", "runtime library missing after build", failures)
    clangxx = resolve_tool(CLANGXX)
    total += 1
    passed += require(clangxx is not None, "clang++", "M271-D002-DYN-04", "unable to resolve clang++", failures)
    if failures:
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "d002" / "positive"
    positive_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(positive_dir),
        "--emit-prefix",
        "module",
    ])
    manifest = positive_dir / "module.manifest.json"
    ir_path = positive_dir / "module.ll"
    obj_path = positive_dir / "module.obj"
    backend_path = positive_dir / "module.object-backend.txt"
    metadata_path = positive_dir / "module.runtime-metadata.bin"
    total += 1
    passed += require(compile_result.returncode == 0, display_path(FIXTURE), "M271-D002-DYN-05", f"fixture compile failed: {(compile_result.stdout or '')}{(compile_result.stderr or '')}", failures)
    for index, path in enumerate((manifest, ir_path, obj_path, backend_path, metadata_path), start=6):
        total += 1
        passed += require(path.exists(), display_path(path), f"M271-D002-DYN-{index:02d}", f"missing artifact: {display_path(path)}", failures)
    if compile_result.returncode != 0 or not ir_path.exists() or not obj_path.exists():
        return total, passed, {"compile_stdout": compile_result.stdout, "compile_stderr": compile_result.stderr}

    ir_text = read_text(ir_path)
    total += 1
    passed += require(BOUNDARY_PREFIX in ir_text, display_path(ir_path), "M271-D002-DYN-11", "missing live Part 8 runtime-integration boundary line", failures)
    total += 1
    passed += require(NAMED_METADATA in ir_text, display_path(ir_path), "M271-D002-DYN-12", "missing live Part 8 runtime-integration named metadata", failures)
    for index, snippet in enumerate(IR_SNIPPETS, start=13):
        total += 1
        passed += require(snippet in ir_text, display_path(ir_path), f"M271-D002-DYN-{index:02d}", f"missing IR snippet: {snippet}", failures)

    probe_exe = positive_dir / "m271_d002_live_cleanup_retainable_integration_probe.exe"
    probe_compile = run_command([
        str(clangxx),
        "-std=c++20",
        "-I",
        str(RUNTIME_INCLUDE_ROOT),
        str(PROBE),
        str(obj_path),
        str(RUNTIME_LIBRARY),
        "-o",
        str(probe_exe),
    ])
    total += 1
    passed += require(probe_compile.returncode == 0, display_path(PROBE), "M271-D002-DYN-21", f"probe compile failed: {(probe_compile.stdout or '')}{(probe_compile.stderr or '')}", failures)
    if probe_compile.returncode != 0:
        return total, passed, {"probe_compile_stdout": probe_compile.stdout, "probe_compile_stderr": probe_compile.stderr}

    probe_run = run_command([str(probe_exe)])
    total += 1
    passed += require(probe_run.returncode == 0, display_path(probe_exe), "M271-D002-DYN-22", f"probe execution failed: {(probe_run.stdout or '')}{(probe_run.stderr or '')}", failures)
    probe_payload = parse_probe_output(probe_run.stdout or "")
    for index, (key, expected) in enumerate(EXPECTED_PROBE.items(), start=23):
        total += 1
        passed += require(probe_payload.get(key) == expected, "probe_payload", f"M271-D002-DYN-{index:02d}", f"{key} mismatch", failures)

    return total, passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_dir": display_path(positive_dir),
        "manifest": display_path(manifest),
        "ir": display_path(ir_path),
        "object": display_path(obj_path),
        "object_backend": read_text(backend_path).strip() if backend_path.exists() else "",
        "runtime_metadata": display_path(metadata_path),
        "probe_exe": display_path(probe_exe),
        "probe_payload": probe_payload,
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    if args.skip_dynamic_probes:
        dynamic: dict[str, Any] = {"skipped": True}
    else:
        total, passed, dynamic = run_dynamic_case(failures)
        checks_total += total
        checks_passed += passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic": dynamic,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}", file=sys.stderr)
        print(f"[info] summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
