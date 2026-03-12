#!/usr/bin/env python3
"""Validate M266-D002 runtime cleanup/unwind integration."""

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
MODE = "m266-d002-runtime-cleanup-unwind-integration-v1"
ISSUE = "M266-D002"
ISSUE_NUMBER = "#7266"
NEXT_ISSUE = "M266-E001"
CONTRACT_ID = "objc3c-runtime-cleanup-unwind-integration/m266-d002-v1"
LOWERING_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m266" / "M266-D002" / "runtime_cleanup_and_unwind_integration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_runtime_cleanup_and_unwind_integration_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m266_d002_cleanup_unwind_runtime_probe.cpp"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "d002"
BOUNDARY_PREFIX = "; part5_control_flow_safety_lowering = "
LINK_MODEL = "linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs"


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
        SnippetCheck("M266-D002-EXP-01", f"Issue: `{ISSUE_NUMBER}`"),
        SnippetCheck("M266-D002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M266-D002-EXP-03", "guard-mediated early return with deferred cleanup execution"),
        SnippetCheck("M266-D002-EXP-04", f"`{NEXT_ISSUE}` remains the next issue"),
    ),
    PACKET_DOC: (
        SnippetCheck("M266-D002-PKT-01", "Packet: `M266-D002`"),
        SnippetCheck("M266-D002-PKT-02", f"Issue: `{ISSUE_NUMBER}`"),
        SnippetCheck("M266-D002-PKT-03", "Nested-scope return unwind preserves inner-to-outer cleanup ordering"),
        SnippetCheck("M266-D002-PKT-04", f"`{NEXT_ISSUE}` is the explicit next issue"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M266-D002-DOCSRC-01", "ordinary lexical exit executes deferred cleanups through native executable"),
        SnippetCheck("M266-D002-DOCSRC-02", "guard-mediated early return executes deferred cleanups through native"),
        SnippetCheck("M266-D002-DOCSRC-03", f"`{NEXT_ISSUE}` is the next issue after this runtime implementation lands"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M266-D002-NDOC-01", "ordinary lexical exit executes deferred cleanups through native executable"),
        SnippetCheck("M266-D002-NDOC-02", f"`{NEXT_ISSUE}` is the next issue after this runtime implementation lands"),
    ),
    SPEC_AM: (
        SnippetCheck("M266-D002-AM-01", "`M266-D002` now widens that runnable execution surface"),
    ),
    SPEC_RULES: (
        SnippetCheck("M266-D002-RULES-01", "`M266-D002` widens native runnable proof across ordinary exit"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M266-D002-ARCH-01", "`M266-D002` widens that runnable proof through ordinary lexical exit"),
    ),
    RUNTIME_INTERNAL_HEADER: (
        SnippetCheck("M266-D002-RIH-01", "M266-D002 runtime cleanup/unwind implementation anchor"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M266-D002-RCPP-01", "M266-D002 runtime cleanup/unwind implementation anchor"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M266-D002-PROC-01", "cleanup_unwind_runtime_link_model"),
        SnippetCheck("M266-D002-PROC-02", LINK_MODEL),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M266-D002-PKG-01", '"check:objc3c:m266-d002-runtime-cleanup-and-unwind-integration-core-feature-implementation"'),
        SnippetCheck("M266-D002-PKG-02", '"test:tooling:m266-d002-runtime-cleanup-and-unwind-integration-core-feature-implementation"'),
        SnippetCheck("M266-D002-PKG-03", '"check:objc3c:m266-d002-lane-d-readiness"'),
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


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def resolve_clangxx() -> str:
    candidates = (
        shutil.which("clang++.exe"),
        shutil.which("clang++"),
        r"C:\Program Files\LLVM\bin\clang++.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang++"


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = load_json(registration_manifest_path)
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    result = run_command(
        [
            resolve_clangxx(),
            str(obj_path),
            str(runtime_library_path),
            f"@{rsp_path.resolve()}",
            "-o",
            str(exe_path),
        ],
        cwd=out_dir,
    )
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def make_ordinary_exit_source() -> str:
    return """module m266RuntimeCleanupOrdinary;

let counter = 0;

fn mark(value: i32) -> void {
  counter = (counter * 10) + value;
}

fn run() -> void {
  defer {
    mark(1);
  }
  defer {
    mark(2);
  }
}

fn main() -> i32 {
  counter = 0;
  run();
  return counter;
}
"""


def make_guard_return_source() -> str:
    return """module m266RuntimeCleanupGuard;

let counter = 0;

fn mark(value: i32) -> void {
  counter = (counter * 10) + value;
}

fn run(flag: bool) -> i32 {
  defer {
    mark(3);
  }
  guard flag else {
    return 7;
  }
  return 4;
}

fn main() -> i32 {
  counter = 0;
  let value = run(false);
  return (counter * 10) + value;
}
"""


def make_nested_return_source() -> str:
    return """module m266RuntimeCleanupNested;

let counter = 0;

fn mark(value: i32) -> void {
  counter = (counter * 10) + value;
}

fn run() -> i32 {
  defer {
    mark(4);
  }
  {
    defer {
      mark(5);
    }
    return 9;
  }
}

fn main() -> i32 {
  counter = 0;
  let value = run();
  return (value * 100) + counter;
}
"""


def write_probe(path: Path, source: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(source, encoding="utf-8")


def validate_lowering_manifest(manifest_path: Path, registration_manifest_path: Path, failures: list[Finding], *, expected_defer_sites: int, expected_live_sites: int) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    manifest = load_json(manifest_path)
    registration_manifest = load_json(registration_manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part5_control_flow_safety_lowering_contract")
    total += 1
    passed += require(isinstance(packet, dict), display_path(manifest_path), "M266-D002-MAN-01", "missing Part 5 lowering packet", failures)
    if not isinstance(packet, dict):
        return total, passed, {}
    for check_id, field, expected in (
        ("M266-D002-MAN-02", "surface_path", LOWERING_SURFACE_PATH),
        ("M266-D002-MAN-03", "defer_statement_sites", expected_defer_sites),
        ("M266-D002-MAN-04", "live_defer_cleanup_sites", expected_live_sites),
        ("M266-D002-MAN-05", "ready_for_native_defer_lowering", True),
    ):
        total += 1
        passed += require(packet.get(field) == expected, display_path(manifest_path), check_id, f"{field} mismatch", failures)
    total += 1
    passed += require(
        registration_manifest.get("cleanup_unwind_runtime_link_model") == LINK_MODEL,
        display_path(registration_manifest_path),
        "M266-D002-MAN-06",
        "cleanup_unwind_runtime_link_model missing or drifted",
        failures,
    )
    return total, passed, packet


def validate_compilation_artifacts(out_dir: Path, failures: list[Finding], *, expected_defer_sites: int, expected_live_sites: int, case_id: str) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    reg_path = out_dir / "module.runtime-registration-manifest.json"
    for check_id, path in (
        (f"{case_id}-ART-01", manifest_path),
        (f"{case_id}-ART-02", ir_path),
        (f"{case_id}-ART-03", obj_path),
        (f"{case_id}-ART-04", rsp_path),
        (f"{case_id}-ART-05", reg_path),
    ):
        total += 1
        passed += require(path.exists(), display_path(path), check_id, f"missing artifact: {display_path(path)}", failures)
    packet: dict[str, Any] = {}
    if manifest_path.exists():
        extra_total, extra_passed, packet = validate_lowering_manifest(
            manifest_path,
            reg_path,
            failures,
            expected_defer_sites=expected_defer_sites,
            expected_live_sites=expected_live_sites,
        )
        total += extra_total
        passed += extra_passed
    if ir_path.exists():
        ir_text = read_text(ir_path)
        total += 1
        passed += require(BOUNDARY_PREFIX in ir_text, display_path(ir_path), f"{case_id}-IR-01", "Part 5 lowering boundary comment missing", failures)
    return total, passed, packet


def compile_native_probe(source_path: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(NATIVE_EXE), str(source_path), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def compile_and_run_case(case_name: str, source_text: str, expected_exit_code: int, *, expected_defer_sites: int, expected_live_sites: int, failures: list[Finding], evidence: dict[str, Any]) -> tuple[int, int]:
    total = 0
    passed = 0
    source_path = PROBE_ROOT / "generated" / f"{case_name}.objc3"
    out_dir = PROBE_ROOT / case_name
    write_probe(source_path, source_text)
    compile_result = compile_native_probe(source_path, out_dir)
    total += 1
    passed += require(compile_result.returncode == 0, display_path(source_path), f"{case_name}-COMPILE-01", f"native compile failed: {compile_result.stderr or compile_result.stdout}", failures)
    if compile_result.returncode != 0:
        evidence[case_name] = {"compile_returncode": compile_result.returncode}
        return total, passed
    extra_total, extra_passed, packet = validate_compilation_artifacts(
        out_dir,
        failures,
        expected_defer_sites=expected_defer_sites,
        expected_live_sites=expected_live_sites,
        case_id=case_name.upper(),
    )
    total += extra_total
    passed += extra_passed
    exe_path, link_result = link_executable(out_dir)
    total += 1
    passed += require(exe_path is not None, display_path(out_dir), f"{case_name}-LINK-01", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}", failures)
    run_returncode = None
    if exe_path is not None:
        run_result = run_command([str(exe_path)], cwd=out_dir)
        run_returncode = run_result.returncode
        total += 1
        passed += require(run_result.returncode == expected_exit_code, display_path(exe_path), f"{case_name}-RUN-01", f"expected exit code {expected_exit_code}, got {run_result.returncode}", failures)
    evidence[case_name] = {
        "compile_returncode": compile_result.returncode,
        "run_returncode": run_returncode,
        "lowering_packet": packet,
    }
    return total, passed


def run_runtime_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    evidence: dict[str, Any] = {}
    runtime_probe_out = PROBE_ROOT / "runtime_probe"
    runtime_probe_out.mkdir(parents=True, exist_ok=True)
    probe_exe = runtime_probe_out / "m266_d002_cleanup_unwind_runtime_probe.exe"
    probe_compile = run_command([
        resolve_clangxx(),
        "-std=c++20",
        "-I",
        str(RUNTIME_INCLUDE_ROOT),
        str(RUNTIME_PROBE),
        str(RUNTIME_LIBRARY),
        "-o",
        str(probe_exe),
    ], cwd=runtime_probe_out)
    total += 1
    passed += require(probe_compile.returncode == 0 and probe_exe.exists(), display_path(RUNTIME_PROBE), "M266-D002-RTP-01", f"runtime probe compile failed: {probe_compile.stderr or probe_compile.stdout}", failures)
    if not probe_exe.exists():
        return total, passed, evidence
    probe_run = run_command([str(probe_exe)], cwd=runtime_probe_out)
    total += 1
    passed += require(probe_run.returncode == 0, display_path(probe_exe), "M266-D002-RTP-02", f"runtime probe failed: {probe_run.stderr or probe_run.stdout}", failures)
    if probe_run.returncode != 0:
        return total, passed, evidence
    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), "M266-D002-RTP-03", f"invalid runtime probe JSON: {exc}"))
        return total + 1, passed, evidence
    for check_id, key, expected in (
        ("M266-D002-RTP-04", "before_depth", 0),
        ("M266-D002-RTP-05", "nested_depth", 2),
        ("M266-D002-RTP-06", "nested_max_depth", 2),
        ("M266-D002-RTP-07", "nested_queue_count", 2),
        ("M266-D002-RTP-08", "nested_last_autoreleased", 22),
        ("M266-D002-RTP-09", "after_inner_depth", 1),
        ("M266-D002-RTP-10", "after_inner_queue_count", 1),
        ("M266-D002-RTP-11", "after_inner_drained_count", 1),
        ("M266-D002-RTP-12", "after_inner_last_drained", 22),
        ("M266-D002-RTP-13", "after_outer_depth", 0),
        ("M266-D002-RTP-14", "after_outer_queue_count", 0),
        ("M266-D002-RTP-15", "after_outer_drained_count", 2),
        ("M266-D002-RTP-16", "after_outer_last_drained", 11),
    ):
        total += 1
        passed += require(probe_payload.get(key) == expected, display_path(probe_exe), check_id, f"{key} mismatch", failures)
    evidence = probe_payload
    return total, passed, evidence


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    evidence: dict[str, Any] = {}

    ensure_summary = ROOT / "tmp" / "reports" / "m266" / "M266-D002" / "ensure_build_summary.json"
    ensure_build = run_command([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m266-d002-readiness",
        "--summary-out",
        str(ensure_summary),
    ])
    total += 3
    passed += require(ensure_build.returncode == 0, display_path(ensure_summary), "M266-D002-DYN-01", f"ensure build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M266-D002-DYN-02", "native executable missing", failures)
    passed += require(RUNTIME_LIBRARY.exists(), display_path(RUNTIME_LIBRARY), "M266-D002-DYN-03", "runtime library missing", failures)
    if failures:
        return total, passed, evidence

    ordinary_total, ordinary_passed = compile_and_run_case(
        "ordinary_exit",
        make_ordinary_exit_source(),
        21,
        expected_defer_sites=2,
        expected_live_sites=2,
        failures=failures,
        evidence=evidence,
    )
    total += ordinary_total
    passed += ordinary_passed

    guard_total, guard_passed = compile_and_run_case(
        "guard_return",
        make_guard_return_source(),
        37,
        expected_defer_sites=1,
        expected_live_sites=1,
        failures=failures,
        evidence=evidence,
    )
    total += guard_total
    passed += guard_passed

    nested_total, nested_passed = compile_and_run_case(
        "nested_return",
        make_nested_return_source(),
        954,
        expected_defer_sites=2,
        expected_live_sites=2,
        failures=failures,
        evidence=evidence,
    )
    total += nested_total
    passed += nested_passed

    runtime_total, runtime_passed, runtime_evidence = run_runtime_probe(failures)
    total += runtime_total
    passed += runtime_passed
    evidence["runtime_probe"] = runtime_evidence
    return total, passed, evidence


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_evidence: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        passed, total, dynamic_evidence = run_dynamic_checks(failures)
        checks_passed += passed
        checks_total += total

    ok = not failures
    summary = {
        "ok": ok,
        "issue": ISSUE,
        "contract_id": CONTRACT_ID,
        "next_issue": NEXT_ISSUE,
        "mode": MODE,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "evidence": dynamic_evidence,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if not ok:
        for failure in failures:
            print(f"[fail] {failure.artifact} {failure.check_id}: {failure.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M266-D002 runtime cleanup/unwind integration validated ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
