#!/usr/bin/env python3
"""Validate M266-D001 cleanup/unwind integration contract freeze."""

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
MODE = "m266-d001-cleanup-unwind-integration-freeze-v1"
CONTRACT_ID = "objc3c-part5-cleanup-unwind-integration-freeze/m266-d001-v1"
LOWERING_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m266" / "M266-D001" / "cleanup_unwind_integration_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_cleanup_and_unwind_integration_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m266_d001_cleanup_unwind_runtime_probe.cpp"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "d001"
BOUNDARY_PREFIX = "; part5_control_flow_safety_lowering = "
PRIVATE_RUNTIME_SYMBOLS = (
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
    "objc3_runtime_copy_memory_management_state_for_testing",
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
        SnippetCheck("M266-D001-EXP-01", "# M266 Cleanup And Unwind Integration Contract And Architecture Freeze Expectations (D001)"),
        SnippetCheck("M266-D001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M266-D001-EXP-03", "`objc3_runtime_push_autoreleasepool_scope`"),
        SnippetCheck("M266-D001-EXP-04", "`tmp/reports/m266/M266-D001/cleanup_unwind_integration_contract_summary.json`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M266-D001-PKT-01", "# M266-D001 Cleanup And Unwind Integration Contract And Architecture Freeze Packet"),
        SnippetCheck("M266-D001-PKT-02", "Issue: `#7265`"),
        SnippetCheck("M266-D001-PKT-03", "Packet: `M266-D001`"),
        SnippetCheck("M266-D001-PKT-04", "`M266-D002` is the next issue."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M266-D001-DOCSRC-01", "## M266 cleanup and unwind integration contract"),
        SnippetCheck("M266-D001-DOCSRC-02", "`M266-D001` freezes the current runtime/toolchain boundary"),
        SnippetCheck("M266-D001-DOCSRC-03", "`objc3_runtime_copy_memory_management_state_for_testing`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M266-D001-NDOC-01", "## M266 cleanup and unwind integration contract"),
        SnippetCheck("M266-D001-NDOC-02", "`M266-D002` is the next issue"),
    ),
    SPEC_AM: (
        SnippetCheck("M266-D001-AM-01", "M266-D001 runtime/toolchain note:"),
        SnippetCheck("M266-D001-AM-02", "the current runnable cleanup/unwind boundary does not add a public runtime cleanup ABI"),
    ),
    SPEC_RULES: (
        SnippetCheck("M266-D001-RULES-01", "Current Part 5 cleanup/unwind runtime note:"),
        SnippetCheck("M266-D001-RULES-02", "`M266-D001` freezes the current toolchain/runtime boundary"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M266-D001-ARCH-01", "- `M266-D001` freezes the current runtime/toolchain boundary for that runnable"),
        SnippetCheck("M266-D001-ARCH-02", "autoreleasepool push/pop plus memory-state snapshot surface"),
    ),
    RUNTIME_INTERNAL_HEADER: (
        SnippetCheck("M266-D001-RIH-01", "M266-D001 cleanup-unwind integration anchor"),
        SnippetCheck("M266-D001-RIH-02", "objc3_runtime_push_autoreleasepool_scope"),
        SnippetCheck("M266-D001-RIH-03", "objc3_runtime_copy_memory_management_state_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M266-D001-RCPP-01", "M266-D001 cleanup-unwind integration anchor"),
        SnippetCheck("M266-D001-RCPP-02", "g_runtime_last_drained_autorelease_value"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M266-D001-PROC-01", "M266-D001 cleanup-unwind integration anchor"),
        SnippetCheck("M266-D001-PROC-02", "runtime_support_library_archive_relative_path"),
        SnippetCheck("M266-D001-PROC-03", "linker_response_payload"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M266-D001-PKG-01", "\"check:objc3c:m266-d001-cleanup-and-unwind-integration-contract-and-architecture-freeze\""),
        SnippetCheck("M266-D001-PKG-02", "\"test:tooling:m266-d001-cleanup-and-unwind-integration-contract-and-architecture-freeze\""),
        SnippetCheck("M266-D001-PKG-03", "\"check:objc3c:m266-d001-lane-d-readiness\""),
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


def write_defer_probe(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        """module m266CleanupUnwindPositive;

fn compute(flag: bool) -> i32 {
  defer {
    flag;
  }
  if (flag) {
    return 7;
  }
  return 3;
}

fn main() -> i32 {
  return compute(true);
}
""",
        encoding="utf-8",
    )


def validate_public_private_boundary(runtime_header_text: str, runtime_internal_text: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for symbol in PRIVATE_RUNTIME_SYMBOLS:
        total += 1
        passed += require(symbol not in runtime_header_text, display_path(RUNTIME_HEADER), f"M266-D001-PUB-{total:02d}", f"public runtime header must not expose private helper symbol {symbol}", failures)
    for symbol in PRIVATE_RUNTIME_SYMBOLS:
        total += 1
        passed += require(symbol in runtime_internal_text, display_path(RUNTIME_INTERNAL_HEADER), f"M266-D001-PRI-{total:02d}", f"private runtime header must expose symbol {symbol}", failures)
    return passed, total


def validate_defer_manifest(manifest_path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    manifest = load_json(manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part5_control_flow_safety_lowering_contract")
    if not isinstance(packet, dict):
        failures.append(Finding(display_path(manifest_path), "M266-D001-MAN-01", "missing Part 5 lowering packet"))
        return passed, total + 1, {}
    for check_id, field, expected in (
        ("M266-D001-MAN-02", "surface_path", LOWERING_SURFACE_PATH),
        ("M266-D001-MAN-03", "defer_statement_sites", 1),
        ("M266-D001-MAN-04", "live_defer_cleanup_sites", 1),
        ("M266-D001-MAN-05", "ready_for_native_defer_lowering", True),
    ):
        total += 1
        passed += require(packet.get(field) == expected, display_path(manifest_path), check_id, f"{field} mismatch", failures)
    return passed, total, packet


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    evidence: dict[str, Any] = {}

    def check(condition: bool, artifact: str, check_id: str, detail: str) -> None:
        nonlocal total, passed
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)

    ensure_summary = ROOT / "tmp" / "reports" / "m266" / "M266-D001" / "ensure_build_summary.json"
    ensure_build = run_command([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m266-d001-readiness",
        "--summary-out",
        str(ensure_summary),
    ])
    check(ensure_build.returncode == 0, display_path(ensure_summary), "M266-D001-DYN-01", f"ensure build failed: {ensure_build.stderr or ensure_build.stdout}")
    check(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M266-D001-DYN-02", "native executable missing")
    check(RUNTIME_LIBRARY.exists(), display_path(RUNTIME_LIBRARY), "M266-D001-DYN-03", "runtime library missing")
    check(RUNTIME_PROBE.exists(), display_path(RUNTIME_PROBE), "M266-D001-DYN-04", "runtime probe missing")
    if failures:
        return passed, total, evidence

    defer_src = PROBE_ROOT / "generated" / "defer_positive.objc3"
    write_defer_probe(defer_src)
    defer_out = PROBE_ROOT / "defer_positive"
    defer_out.mkdir(parents=True, exist_ok=True)
    compile_result = run_command([str(NATIVE_EXE), str(defer_src), "--out-dir", str(defer_out), "--emit-prefix", "module"])
    check(compile_result.returncode == 0, display_path(defer_src), "M266-D001-DYN-05", f"native probe compile failed: {compile_result.stderr or compile_result.stdout}")

    manifest_path = defer_out / "module.manifest.json"
    ir_path = defer_out / "module.ll"
    obj_path = defer_out / "module.obj"
    rsp_path = defer_out / "module.runtime-metadata-linker-options.rsp"
    reg_path = defer_out / "module.runtime-registration-manifest.json"
    for check_id, path in (
        ("M266-D001-DYN-06", manifest_path),
        ("M266-D001-DYN-07", ir_path),
        ("M266-D001-DYN-08", obj_path),
        ("M266-D001-DYN-09", rsp_path),
        ("M266-D001-DYN-10", reg_path),
    ):
        check(path.exists(), display_path(path), check_id, f"missing artifact: {display_path(path)}")

    if manifest_path.exists():
        sub_passed, sub_total, packet = validate_defer_manifest(manifest_path, failures)
        passed += sub_passed
        total += sub_total
        evidence["defer_lowering_packet"] = packet

    if ir_path.exists():
        ir_text = read_text(ir_path)
        check(BOUNDARY_PREFIX in ir_text, display_path(ir_path), "M266-D001-DYN-11", "Part 5 lowering boundary comment missing")

    exe_path, link_result = link_executable(defer_out)
    check(exe_path is not None, display_path(defer_out), "M266-D001-DYN-12", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}")
    if exe_path is not None:
        run_result = run_command([str(exe_path)], cwd=defer_out)
        check(run_result.returncode == 7, display_path(exe_path), "M266-D001-DYN-13", f"expected exit code 7, got {run_result.returncode}")
        evidence["defer_positive_exit_code"] = run_result.returncode

    runtime_probe_out = PROBE_ROOT / "runtime_probe"
    runtime_probe_out.mkdir(parents=True, exist_ok=True)
    probe_exe = runtime_probe_out / "m266_d001_cleanup_unwind_runtime_probe.exe"
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
    check(probe_compile.returncode == 0 and probe_exe.exists(), display_path(RUNTIME_PROBE), "M266-D001-DYN-14", f"runtime probe compile failed: {probe_compile.stderr or probe_compile.stdout}")
    if probe_exe.exists():
        probe_run = run_command([str(probe_exe)], cwd=runtime_probe_out)
        check(probe_run.returncode == 0, display_path(probe_exe), "M266-D001-DYN-15", f"runtime probe failed: {probe_run.stderr or probe_run.stdout}")
        if probe_run.returncode == 0:
            try:
                probe_payload = json.loads(probe_run.stdout)
            except json.JSONDecodeError as exc:
                failures.append(Finding(display_path(probe_exe), "M266-D001-DYN-16", f"invalid runtime probe JSON: {exc}"))
            else:
                for check_id, key, expected in (
                    ("M266-D001-DYN-17", "before_depth", 0),
                    ("M266-D001-DYN-18", "inside_depth", 1),
                    ("M266-D001-DYN-19", "inside_queue_count", 1),
                    ("M266-D001-DYN-20", "inside_last_autoreleased", 11),
                    ("M266-D001-DYN-21", "after_depth", 0),
                    ("M266-D001-DYN-22", "after_drained_count", 1),
                    ("M266-D001-DYN-23", "after_last_drained", 11),
                ):
                    check(probe_payload.get(key) == expected, display_path(probe_exe), check_id, f"{key} mismatch")
                evidence["runtime_probe_payload"] = probe_payload

    return passed, total, evidence


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    runtime_header_text = read_text(RUNTIME_HEADER)
    runtime_internal_text = read_text(RUNTIME_INTERNAL_HEADER)
    passed, total = validate_public_private_boundary(runtime_header_text, runtime_internal_text, failures)
    checks_passed += passed
    checks_total += total

    dynamic_evidence: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        passed, total, dynamic_evidence = run_dynamic_checks(failures)
        checks_passed += passed
        checks_total += total

    ok = not failures
    summary = {
        "ok": ok,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
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
    print(f"[ok] M266-D001 cleanup/unwind integration freeze validated ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
