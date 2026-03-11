#!/usr/bin/env python3
"""Checker for M261-D003 byref forwarding / heap promotion / ownership interop."""

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
MODE = "m261-d003-block-byref-forwarding-heap-promotion-interop-v1"
CONTRACT_ID = "objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1"
NEXT_ISSUE = "M261-E001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-D003" / "block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion_d003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion_packet.md"
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
READINESS_RUNNER = ROOT / "scripts" / "run_m261_d003_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m261_d003_block_runtime_byref_forwarding_probe.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
BYREF_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_byref_positive.objc3"
OWNED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_owned_capture_positive.objc3"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "d003"


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
        SnippetCheck("M261-D003-EXP-01", "# M261 Byref Forwarding Cells Heap Promotion And Ownership Interop For Escaping Blocks Core Feature Expansion Expectations (D003)"),
        SnippetCheck("M261-D003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-D003-EXP-03", "Issue: `#7191`"),
        SnippetCheck("M261-D003-EXP-04", "`M261-E001` is the next issue"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-D003-PKT-01", "# M261-D003 Byref Forwarding Cells Heap Promotion And Ownership Interop For Escaping Blocks Core Feature Expansion Packet"),
        SnippetCheck("M261-D003-PKT-02", "Packet: `M261-D003`"),
        SnippetCheck("M261-D003-PKT-03", "Issue: `#7191`"),
        SnippetCheck("M261-D003-PKT-04", "`M261-E001` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-D003-SRC-01", "## M261 byref forwarding, heap promotion, and ownership interop for escaping blocks (M261-D003)"),
        SnippetCheck("M261-D003-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D003-SRC-03", "`M261-E001` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-D003-NDOC-01", "## M261 byref forwarding, heap promotion, and ownership interop for escaping blocks (M261-D003)"),
        SnippetCheck("M261-D003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D003-NDOC-03", "`M261-E001` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-D003-P0-01", "(`M261-D003`)"),
        SnippetCheck("M261-D003-P0-02", "forwarding cells so byref mutation"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-D003-SPC-01", "## M261 byref forwarding, heap promotion, and ownership interop for escaping blocks (D003)"),
        SnippetCheck("M261-D003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D003-SPC-03", "`M261-E001` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-D003-ARCH-01", "## M261 Byref Forwarding, Heap Promotion, And Ownership Interop For Escaping Blocks (D003)"),
        SnippetCheck("M261-D003-ARCH-02", "runtime_block_byref_forwarding_heap_promotion_ownership_interop"),
        SnippetCheck("M261-D003-ARCH-03", "the next issue is `M261-E001`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-D003-PARSE-01", "M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-D003-SEMA-PM-01", "M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-D003-LOWER-H-01", "kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId"),
        SnippetCheck("M261-D003-LOWER-H-02", "std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-D003-LOWER-CPP-01", "std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary()"),
        SnippetCheck("M261-D003-LOWER-CPP-02", "forwarding_model=escaping-pointer-capture-slots-rewrite-to-runtime-owned-forwarding-cells"),
    ),
    IR_CPP: (
        SnippetCheck("M261-D003-IR-01", "M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor"),
        SnippetCheck("M261-D003-IR-02", "runtime_block_byref_forwarding_heap_promotion_ownership_interop = "),
    ),
    DRIVER_CPP: (
        SnippetCheck("M261-D003-DRV-01", "M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor"),
    ),
    RUNTIME_INTERNAL_HEADER: (
        SnippetCheck("M261-D003-RH-01", "M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor"),
        SnippetCheck("M261-D003-RH-02", "objc3_runtime_promote_block_i32"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M261-D003-RCPP-01", "M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor"),
        SnippetCheck("M261-D003-RCPP-02", "promoted_capture_cells"),
        SnippetCheck("M261-D003-RCPP-03", "PromotePointerCaptureCellsIntoRuntimeOwnedStorage"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-D003-PKG-01", '"check:objc3c:m261-d003-block-byref-forwarding-heap-promotion-interop": "python scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py"'),
        SnippetCheck("M261-D003-PKG-02", '"test:tooling:m261-d003-block-byref-forwarding-heap-promotion-interop": "python -m pytest tests/tooling/test_check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py -q"'),
        SnippetCheck("M261-D003-PKG-03", '"check:objc3c:m261-d003-lane-d-readiness": "python scripts/run_m261_d003_lane_d_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-D003-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M261-D003-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-D003-RUN-03", "check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py"),
        SnippetCheck("M261-D003-RUN-04", "check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py"),
        SnippetCheck("M261-D003-RUN-05", "check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-D003-TEST-01", "def test_m261_d003_checker_emits_summary() -> None:"),
        SnippetCheck("M261-D003-TEST-02", CONTRACT_ID),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M261-D003-PRB-01", "struct ProbeBlockStorage"),
        SnippetCheck("M261-D003-PRB-02", "objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-D003-PRB-03", "copy_count_after_promotion"),
        SnippetCheck("M261-D003-PRB-04", "second_invoke_result"),
        SnippetCheck("M261-D003-PRB-05", "last_disposed_value"),
    ),
    BYREF_FIXTURE: (
        SnippetCheck("M261-D003-FIX-BYREF-01", "module m261_escaping_block_runtime_hook_byref_positive;"),
        SnippetCheck("M261-D003-FIX-BYREF-02", "seed = seed + delta;"),
        SnippetCheck("M261-D003-FIX-BYREF-03", "return materialize(closure);"),
    ),
    OWNED_FIXTURE: (
        SnippetCheck("M261-D003-FIX-OWNED-01", "module m261_escaping_block_runtime_hook_owned_capture_positive;"),
        SnippetCheck("M261-D003-FIX-OWNED-02", "fn helper(ownedValue: id, weakValue: __weak id) -> i32 {"),
        SnippetCheck("M261-D003-FIX-OWNED-03", "return materialize(closure);"),
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
    check(isinstance(handle, int) and handle > 0, "M261-D003-PROBE-01", "runtime probe must publish a positive block handle")
    check(payload.get("copy_count_after_promotion") == 1, "M261-D003-PROBE-02", "promotion must run copy helper exactly once")
    check(payload.get("first_invoke_result") == 23, "M261-D003-PROBE-03", "first invoke must use promoted heap-cell captures")
    check(payload.get("second_invoke_result") == 25, "M261-D003-PROBE-04", "second invoke must preserve mutated forwarding-cell state")
    check(payload.get("dispose_count_before_final_release") == 0, "M261-D003-PROBE-05", "dispose helper must not run before final release")
    check(payload.get("dispose_count_after_final_release") == 1, "M261-D003-PROBE-06", "final release must run dispose helper exactly once")
    check(payload.get("last_disposed_value") == 11, "M261-D003-PROBE-07", "dispose helper must observe the promoted owned capture value")
    check(payload.get("final_release_result") == handle, "M261-D003-PROBE-08", "final release must round-trip the block handle")
    check(payload.get("invoke_after_release_result") == 0, "M261-D003-PROBE-09", "released block handle must no longer invoke")
    return passed, total


def validate_positive_fixture(fixture: Path, out_dir: Path, label: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    payload: dict[str, Any] = {}

    compile_result = compile_fixture(fixture, out_dir)
    payload[f"{label}_compile_rc"] = compile_result.returncode
    checks_total += require(compile_result.returncode == 0, display_path(fixture), f"M261-D003-{label.upper()}-COMPILE", f"fixture compile failed with rc={compile_result.returncode}", failures)
    if compile_result.returncode == 0:
        checks_passed += 1
        ll_path = out_dir / "module.ll"
        obj_path = out_dir / "module.obj"
        backend_path = out_dir / "module.object-backend.txt"
        ll_text = read_text(ll_path) if ll_path.exists() else ""
        backend = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
        for check_id, condition, detail in (
            (f"M261-D003-{label.upper()}-LL", ll_path.exists(), "missing emitted LLVM IR"),
            (f"M261-D003-{label.upper()}-OBJ", obj_path.exists(), "missing emitted object"),
            (f"M261-D003-{label.upper()}-BACKEND", backend == "llvm-direct", f"expected llvm-direct backend, got {backend!r}"),
            (f"M261-D003-{label.upper()}-SUMMARY", f"runtime_block_byref_forwarding_heap_promotion_ownership_interop = contract={CONTRACT_ID}" in ll_text, "missing D003 runtime boundary summary"),
            (f"M261-D003-{label.upper()}-PROMOTE", "@objc3_runtime_promote_block_i32" in ll_text and ", i32 1)" in ll_text, "missing pointer-capture promotion helper call"),
            (f"M261-D003-{label.upper()}-INVOKE", "@objc3_runtime_invoke_block_i32" in ll_text, "missing runtime invoke helper symbol in emitted IR"),
        ):
            checks_total += require(condition, display_path(out_dir), check_id, detail, failures)
            if condition:
                checks_passed += 1
    return checks_passed, checks_total, payload


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
            (NATIVE_EXE, "M261-D003-DYN-EXE"),
            (RUNTIME_LIBRARY, "M261-D003-DYN-LIB"),
            (BYREF_FIXTURE, "M261-D003-DYN-BYREF-FIXTURE"),
            (OWNED_FIXTURE, "M261-D003-DYN-OWNED-FIXTURE"),
            (RUNTIME_PROBE, "M261-D003-DYN-PROBE-SRC"),
        ):
            checks_total += require(artifact_path.exists(), display_path(artifact_path), check_id, f"missing required artifact: {display_path(artifact_path)}", failures)
            if artifact_path.exists():
                checks_passed += 1

        clangxx = resolve_tool("clang++")
        checks_total += require(clangxx is not None, "clang++", "M261-D003-DYN-CLANGXX", "unable to resolve clang++", failures)
        if clangxx is not None:
            checks_passed += 1

        if not failures:
            byref_passed, byref_total, byref_payload = validate_positive_fixture(BYREF_FIXTURE, PROBE_ROOT / "byref-positive", "byref", failures)
            checks_passed += byref_passed
            checks_total += byref_total
            dynamic.update(byref_payload)

            owned_passed, owned_total, owned_payload = validate_positive_fixture(OWNED_FIXTURE, PROBE_ROOT / "owned-positive", "owned", failures)
            checks_passed += owned_passed
            checks_total += owned_total
            dynamic.update(owned_payload)

            probe_dir = PROBE_ROOT / "runtime-probe"
            probe_dir.mkdir(parents=True, exist_ok=True)
            probe_exe = probe_dir / "m261_d003_block_runtime_byref_forwarding_probe.exe"
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
            checks_total += require(probe_compile.returncode == 0, display_path(RUNTIME_PROBE), "M261-D003-DYN-PROBE-COMPILE", f"runtime probe compile failed: {probe_compile.stderr.strip()}" or f"runtime probe compile failed: {probe_compile.stdout.strip()}", failures)
            if probe_compile.returncode == 0:
                checks_passed += 1
                probe_run = run_process([str(probe_exe)])
                dynamic["probe_run_rc"] = probe_run.returncode
                checks_total += require(probe_run.returncode == 0, display_path(probe_exe), "M261-D003-DYN-PROBE-RUN", f"runtime probe exited with {probe_run.returncode}", failures)
                if probe_run.returncode == 0:
                    checks_passed += 1
                    try:
                        payload = json.loads(probe_run.stdout)
                    except json.JSONDecodeError as exc:
                        failures.append(Finding(display_path(probe_exe), "M261-D003-DYN-PROBE-JSON", f"invalid runtime probe JSON: {exc}"))
                    else:
                        dynamic["probe_payload"] = payload
                        payload_passed, payload_total = validate_probe_payload(payload, failures)
                        checks_passed += payload_passed
                        checks_total += payload_total

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": "M261-D003",
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
