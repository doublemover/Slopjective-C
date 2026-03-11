#!/usr/bin/env python3
"""Checker for M261-D002 block runtime allocation/copy-dispose/invoke support."""

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
MODE = "m261-d002-block-runtime-copy-dispose-invoke-support-v1"
CONTRACT_ID = "objc3c-runtime-block-allocation-copy-dispose-invoke-support/m261-d002-v1"
NEXT_ISSUE = "M261-D003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-D002" / "block_runtime_copy_dispose_invoke_support_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_d002_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m261_d002_block_runtime_copy_dispose_invoke_probe.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_argument_positive.objc3"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "d002"


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
        SnippetCheck("M261-D002-EXP-01", "# M261 Block Object Allocation Copy-Dispose And Invoke Support Core Feature Implementation Expectations (D002)"),
        SnippetCheck("M261-D002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-D002-EXP-03", "Issue: `#7190`"),
        SnippetCheck("M261-D002-EXP-04", "`M261-D003` remains the next issue"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-D002-PKT-01", "# M261-D002 Block Object Allocation Copy-Dispose And Invoke Support Core Feature Implementation Packet"),
        SnippetCheck("M261-D002-PKT-02", "Packet: `M261-D002`"),
        SnippetCheck("M261-D002-PKT-03", "Issue: `#7190`"),
        SnippetCheck("M261-D002-PKT-04", "`M261-D003` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-D002-SRC-01", "## M261 block object allocation, copy-dispose, and invoke support (M261-D002)"),
        SnippetCheck("M261-D002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D002-SRC-03", "`M261-D003` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-D002-NDOC-01", "## M261 block object allocation, copy-dispose, and invoke support (M261-D002)"),
        SnippetCheck("M261-D002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D002-NDOC-03", "`M261-D003` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-D002-P0-01", "(`M261-D002`)"),
        SnippetCheck("M261-D002-P0-02", "helper-mediated copy/dispose and pointer-capture invoke"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-D002-SPC-01", "## M261 block object allocation, copy-dispose, and invoke support (D002)"),
        SnippetCheck("M261-D002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D002-SPC-03", "`; runtime_block_allocation_copy_dispose_invoke_support = ...`"),
        SnippetCheck("M261-D002-SPC-04", "`M261-D003` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-D002-ARCH-01", "## M261 Block Object Allocation, Copy-Dispose, And Invoke Support (D002)"),
        SnippetCheck("M261-D002-ARCH-02", "runtime_block_allocation_copy_dispose_invoke_support"),
        SnippetCheck("M261-D002-ARCH-03", "the next issue is `M261-D003`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-D002-PARSE-01", "M261-D002 block-runtime allocation/copy-dispose/invoke anchor"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-D002-SEMA-PM-01", "M261-D002 block-runtime allocation/copy-dispose/invoke anchor"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-D002-LOWER-H-01", "kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId"),
        SnippetCheck("M261-D002-LOWER-H-02", "std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-D002-LOWER-CPP-01", "std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary()"),
        SnippetCheck("M261-D002-LOWER-CPP-02", "M261-D002 block-runtime allocation/copy-dispose/invoke implementation"),
        SnippetCheck("M261-D002-LOWER-CPP-03", "pointer-capture-promotion-runs-copy-helper-and-final-release-runs-dispose-helper"),
    ),
    IR_CPP: (
        SnippetCheck("M261-D002-IR-01", "M261-D002 block-runtime allocation/copy-dispose/invoke anchor"),
        SnippetCheck("M261-D002-IR-02", "runtime_block_allocation_copy_dispose_invoke_support = "),
    ),
    DRIVER_CPP: (
        SnippetCheck("M261-D002-DRV-01", "M261-D002 block-runtime allocation/copy-dispose/invoke anchor"),
    ),
    RUNTIME_INTERNAL_HEADER: (
        SnippetCheck("M261-D002-RH-01", "M261-D002 block-runtime allocation/copy-dispose/invoke anchor"),
        SnippetCheck("M261-D002-RH-02", "objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-D002-RH-03", "objc3_runtime_invoke_block_i32"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M261-D002-RCPP-01", "M261-D002 block-runtime allocation/copy-dispose/invoke anchor"),
        SnippetCheck("M261-D002-RCPP-02", "RuntimeBlockCopyHelperFn"),
        SnippetCheck("M261-D002-RCPP-03", "record.copy_helper"),
        SnippetCheck("M261-D002-RCPP-04", "record.dispose_helper"),
        SnippetCheck("M261-D002-RCPP-05", "storage_words"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-D002-PKG-01", '"check:objc3c:m261-d002-block-runtime-copy-dispose-invoke-support": "python scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py"'),
        SnippetCheck("M261-D002-PKG-02", '"test:tooling:m261-d002-block-runtime-copy-dispose-invoke-support": "python -m pytest tests/tooling/test_check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py -q"'),
        SnippetCheck("M261-D002-PKG-03", '"check:objc3c:m261-d002-lane-d-readiness": "python scripts/run_m261_d002_lane_d_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-D002-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M261-D002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-D002-RUN-03", "check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py"),
        SnippetCheck("M261-D002-RUN-04", "check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-D002-TEST-01", "def test_m261_d002_checker_emits_summary() -> None:"),
        SnippetCheck("M261-D002-TEST-02", CONTRACT_ID),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M261-D002-PRB-01", "struct ProbeBlockStorage"),
        SnippetCheck("M261-D002-PRB-02", "objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-D002-PRB-03", "objc3_runtime_invoke_block_i32"),
        SnippetCheck("M261-D002-PRB-04", "copy_count_after_promotion"),
        SnippetCheck("M261-D002-PRB-05", "dispose_count_after_final_release"),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> Any:
    return json.loads(read_text(path))


def resolve_tool(name: str) -> str | None:
    candidates = [shutil.which(name)]
    if sys.platform == "win32":
        if not name.lower().endswith(".exe"):
            candidates.append(shutil.which(f"{name}.exe"))
        llvm_bin = ROOT / "tmp" / "llvm-build-21.1.8-ninja-dia" / "bin"
        candidates.append(str(llvm_bin / (name if name.lower().endswith(".exe") else f"{name}.exe")))
        candidates.append(str(Path(r"C:\Program Files\LLVM\bin") / (name if name.lower().endswith(".exe") else f"{name}.exe")))
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(Path(candidate))
    return None


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


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
    clang = resolve_tool("clang")
    if clang is None:
        return None, None
    completed = run_process([
        clang,
        str(obj_path),
        f"@{rsp_path}",
        str(runtime_library_path),
        "-o",
        str(exe_path),
    ])
    return exe_path if exe_path.exists() else None, completed


def run_executable(exe_path: Path) -> subprocess.CompletedProcess[str]:
    return run_process([str(exe_path)], cwd=exe_path.parent)


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    passed = 0
    total = 0
    artifact = "runtime_probe_payload"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal passed, total
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)

    handle = payload.get("handle")
    check(isinstance(handle, int) and handle > 0, "M261-D002-PROBE-01", "runtime probe must publish a positive block handle")
    check(payload.get("pointer_capture_enabled") == 1, "M261-D002-PROBE-02", "runtime probe must prove pointer-capture storage is enabled")
    check(payload.get("copy_helper_present") == 1, "M261-D002-PROBE-03", "runtime probe must prove a copy helper is present")
    check(payload.get("dispose_helper_present") == 1, "M261-D002-PROBE-04", "runtime probe must prove a dispose helper is present")
    check(payload.get("copy_count_after_promotion") == 1, "M261-D002-PROBE-05", "promotion must run the copy helper exactly once")
    check(payload.get("invoke_result") == 117, "M261-D002-PROBE-06", "runtime invoke result drifted")
    check(payload.get("retain_result") == handle, "M261-D002-PROBE-07", "retain must round-trip the block handle")
    check(payload.get("release_result") == handle, "M261-D002-PROBE-08", "non-final release must round-trip the block handle")
    check(payload.get("dispose_count_before_final_release") == 0, "M261-D002-PROBE-09", "dispose helper must not run before final release")
    check(payload.get("final_release_result") == handle, "M261-D002-PROBE-10", "final release must still round-trip the block handle")
    check(payload.get("dispose_count_after_final_release") == 1, "M261-D002-PROBE-11", "final release must run the dispose helper exactly once")
    check(payload.get("invoke_after_release_result") == 0, "M261-D002-PROBE-12", "released block handle must no longer invoke successfully")
    return passed, total


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    args = parser.parse_args(argv)

    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic: dict[str, Any] = {"skipped": args.skip_dynamic_probes}

    for path, snippets in STATIC_SNIPPETS.items():
        total, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += total - len(static_failures)
        failures.extend(static_failures)

    if not args.skip_dynamic_probes:
        for artifact_path, check_id in (
            (NATIVE_EXE, "M261-D002-DYN-EXE"),
            (RUNTIME_LIBRARY, "M261-D002-DYN-LIB"),
            (FIXTURE, "M261-D002-DYN-FIXTURE"),
            (RUNTIME_PROBE, "M261-D002-DYN-PROBE-SRC"),
        ):
            checks_total += require(artifact_path.exists(), display_path(artifact_path), check_id, f"missing required artifact: {display_path(artifact_path)}", failures)
            if artifact_path.exists():
                checks_passed += 1

        clangxx = resolve_tool("clang++")
        checks_total += require(clangxx is not None, "clang++", "M261-D002-DYN-CLANGXX", "unable to resolve clang++", failures)
        if clangxx is not None:
            checks_passed += 1

        if not failures:
            positive_dir = PROBE_ROOT / "c004_positive"
            compile_result = compile_fixture(FIXTURE, positive_dir)
            dynamic["fixture_compile_rc"] = compile_result.returncode
            checks_total += require(compile_result.returncode == 0, display_path(FIXTURE), "M261-D002-DYN-COMPILE", f"fixture compile failed with rc={compile_result.returncode}", failures)
            if compile_result.returncode == 0:
                checks_passed += 1
                ll_path = positive_dir / "module.ll"
                obj_path = positive_dir / "module.obj"
                backend_path = positive_dir / "module.object-backend.txt"
                ll_text = read_text(ll_path) if ll_path.exists() else ""
                backend = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
                for check_id, condition, detail in (
                    ("M261-D002-DYN-LL", ll_path.exists(), "missing emitted LLVM IR"),
                    ("M261-D002-DYN-OBJ", obj_path.exists(), "missing emitted object"),
                    ("M261-D002-DYN-BACKEND", backend == "llvm-direct", f"expected llvm-direct backend, got {backend!r}"),
                    ("M261-D002-DYN-SUMMARY", f"runtime_block_allocation_copy_dispose_invoke_support = contract={CONTRACT_ID}" in ll_text, "missing D002 runtime boundary summary"),
                    ("M261-D002-DYN-PROMOTE", "@objc3_runtime_promote_block_i32" in ll_text, "missing promote helper symbol in emitted IR"),
                    ("M261-D002-DYN-INVOKE", "@objc3_runtime_invoke_block_i32" in ll_text, "missing invoke helper symbol in emitted IR"),
                ):
                    checks_total += require(condition, display_path(positive_dir), check_id, detail, failures)
                    if condition:
                        checks_passed += 1
                exe_path, link_result = link_executable(positive_dir)
                dynamic["fixture_link_rc"] = None if link_result is None else link_result.returncode
                checks_total += require(exe_path is not None and link_result is not None and link_result.returncode == 0, display_path(positive_dir), "M261-D002-DYN-LINK", "failed to link positive fixture", failures)
                if exe_path is not None and link_result is not None and link_result.returncode == 0:
                    checks_passed += 1
                    run_result = run_executable(exe_path)
                    dynamic["fixture_exit"] = run_result.returncode
                    checks_total += require(run_result.returncode == 14, display_path(exe_path), "M261-D002-DYN-RUN", f"expected fixture exit 14, got {run_result.returncode}", failures)
                    if run_result.returncode == 14:
                        checks_passed += 1

            probe_dir = PROBE_ROOT / "runtime_probe"
            probe_dir.mkdir(parents=True, exist_ok=True)
            probe_exe = probe_dir / "m261_d002_block_runtime_copy_dispose_invoke_probe.exe"
            probe_compile = run_process([
                str(clangxx),
                "-std=c++20",
                "-I",
                str(RUNTIME_INCLUDE_ROOT),
                str(RUNTIME_PROBE),
                str(RUNTIME_LIBRARY),
                "-o",
                str(probe_exe),
            ])
            dynamic["probe_compile_rc"] = probe_compile.returncode
            checks_total += require(probe_compile.returncode == 0, display_path(RUNTIME_PROBE), "M261-D002-DYN-PROBE-COMPILE", f"runtime probe compile failed: {probe_compile.stderr.strip()}" or f"runtime probe compile failed: {probe_compile.stdout.strip()}", failures)
            if probe_compile.returncode == 0:
                checks_passed += 1
                probe_run = run_process([str(probe_exe)])
                dynamic["probe_run_rc"] = probe_run.returncode
                checks_total += require(probe_run.returncode == 0, display_path(probe_exe), "M261-D002-DYN-PROBE-RUN", f"runtime probe exited with {probe_run.returncode}", failures)
                if probe_run.returncode == 0:
                    checks_passed += 1
                    try:
                        payload = json.loads(probe_run.stdout)
                    except json.JSONDecodeError as exc:
                        failures.append(Finding(display_path(probe_exe), "M261-D002-DYN-PROBE-JSON", f"invalid runtime probe JSON: {exc}"))
                    else:
                        dynamic["probe_payload"] = payload
                        payload_passed, payload_total = validate_probe_payload(payload, failures)
                        checks_passed += payload_passed
                        checks_total += payload_total

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": "M261-D002",
        "contract_id": CONTRACT_ID,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic": dynamic,
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    print(args.summary_out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
