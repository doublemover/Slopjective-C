#!/usr/bin/env python3
"""Checker for M261-D001 block runtime API/object-layout freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-d001-block-runtime-api-object-layout-contract-v1"
CONTRACT_ID = "objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1"
NEXT_ISSUE = "M261-D002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-D001" / "block_runtime_api_object_layout_contract_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_block_runtime_api_and_object_layout_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze_packet.md"
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
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_d001_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_argument_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "d001"


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
        SnippetCheck("M261-D001-EXP-01", "# M261 Block Runtime API And Object Layout Contract And Architecture Freeze Expectations (D001)"),
        SnippetCheck("M261-D001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-D001-EXP-03", "Issue: `#7189`"),
        SnippetCheck("M261-D001-EXP-04", "`python scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-D001-PKT-01", "# M261-D001 Block Runtime API And Object Layout Contract And Architecture Freeze Packet"),
        SnippetCheck("M261-D001-PKT-02", "Packet: `M261-D001`"),
        SnippetCheck("M261-D001-PKT-03", "Issue: `#7189`"),
        SnippetCheck("M261-D001-PKT-04", "`M261-D002` is the explicit next issue after this freeze lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-D001-SRC-01", "## M261 block runtime API and object layout (M261-D001)"),
        SnippetCheck("M261-D001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D001-SRC-03", "`objc3_runtime_promote_block_i32`"),
        SnippetCheck("M261-D001-SRC-04", "`M261-D002` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-D001-NDOC-01", "## M261 block runtime API and object layout (M261-D001)"),
        SnippetCheck("M261-D001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D001-NDOC-03", "`objc3_runtime_invoke_block_i32`"),
        SnippetCheck("M261-D001-NDOC-04", "`M261-D002` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-D001-P0-01", "(`M261-D001`)"),
        SnippetCheck("M261-D001-P0-02", "runtime-owned block records"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-D001-SPC-01", "## M261 block runtime API and object layout (D001)"),
        SnippetCheck("M261-D001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-D001-SPC-03", "`; runtime_block_api_object_layout = ...`"),
        SnippetCheck("M261-D001-SPC-04", "`M261-D002` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-D001-ARCH-01", "## M261 Block Runtime API And Object Layout (D001)"),
        SnippetCheck("M261-D001-ARCH-02", "runtime_block_api_object_layout"),
        SnippetCheck("M261-D001-ARCH-03", "the next issue is `M261-D002`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-D001-PARSE-01", "M261-D001 block-runtime API/object-layout freeze anchor"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-D001-SEMA-PM-01", "M261-D001 block-runtime API/object-layout freeze anchor"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-D001-LOWER-H-01", "kObjc3RuntimeBlockApiObjectLayoutContractId"),
        SnippetCheck("M261-D001-LOWER-H-02", "std::string Objc3RuntimeBlockApiObjectLayoutSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-D001-LOWER-CPP-01", "std::string Objc3RuntimeBlockApiObjectLayoutSummary()"),
        SnippetCheck("M261-D001-LOWER-CPP-02", "M261-D001 block-runtime API/object-layout freeze anchor"),
        SnippetCheck("M261-D001-LOWER-CPP-03", "no-public-block-object-abi-no-generalized-runtime-copy-dispose-allocation-surface"),
    ),
    IR_CPP: (
        SnippetCheck("M261-D001-IR-01", "M261-D001 block-runtime API/object-layout freeze anchor"),
        SnippetCheck("M261-D001-IR-02", 'runtime_block_api_object_layout = '),
    ),
    DRIVER_CPP: (
        SnippetCheck("M261-D001-DRV-01", "M261-D001 block-runtime API/object-layout freeze anchor"),
    ),
    RUNTIME_HEADER: (
        SnippetCheck("M261-D001-RH-01", "M261-D001 block-runtime API/object-layout freeze anchor"),
        SnippetCheck("M261-D001-RH-02", "objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-D001-RH-03", "objc3_runtime_invoke_block_i32"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M261-D001-RCPP-01", "M261-D001 block-runtime API/object-layout freeze anchor"),
        SnippetCheck("M261-D001-RCPP-02", "struct RuntimeBlockRecord"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-D001-PKG-01", '"check:objc3c:m261-d001-block-runtime-api-object-layout-contract": "python scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py"'),
        SnippetCheck("M261-D001-PKG-02", '"test:tooling:m261-d001-block-runtime-api-object-layout-contract": "python -m pytest tests/tooling/test_check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M261-D001-PKG-03", '"check:objc3c:m261-d001-lane-d-readiness": "python scripts/run_m261_d001_lane_d_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-D001-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M261-D001-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-D001-RUN-03", "check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py"),
        SnippetCheck("M261-D001-RUN-04", "check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-D001-TEST-01", "def test_m261_d001_checker_emits_summary() -> None:"),
        SnippetCheck("M261-D001-TEST-02", CONTRACT_ID),
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


def resolve_clang() -> str:
    candidates = (
        ROOT / "tmp" / "llvm-build-21.1.8-ninja-dia" / "bin" / "clang.exe",
        Path(r"C:\Program Files\LLVM\bin\clang.exe"),
    )
    for candidate in candidates:
        if candidate.exists():
            return str(candidate)
    return "clang"


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
    clang = resolve_clang()
    completed = run_process(
        [
            clang,
            str(obj_path),
            f"@{rsp_path}",
            str(runtime_library_path),
            "-o",
            str(exe_path),
        ]
    )
    return exe_path if exe_path.exists() else None, completed


def run_executable(exe_path: Path) -> subprocess.CompletedProcess[str]:
    return run_process([str(exe_path)], cwd=exe_path.parent)


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    args = parser.parse_args(argv)

    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    probe_results: dict[str, Any] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        total, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += total - len(static_failures)
        failures.extend(static_failures)

    if not args.skip_dynamic_probes:
        checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-D001-DYN-EXE", "missing native compiler executable", failures)
        if NATIVE_EXE.exists():
            checks_passed += 1

        probe_dir = PROBE_ROOT / "positive"
        compile_result = compile_fixture(POSITIVE_FIXTURE, probe_dir)
        probe_results["positive_compile_rc"] = compile_result.returncode
        checks_total += require(compile_result.returncode == 0, display_path(POSITIVE_FIXTURE), "M261-D001-DYN-COMPILE", f"expected successful compile, got rc={compile_result.returncode}", failures)
        if compile_result.returncode == 0:
            checks_passed += 1
            ll_path = probe_dir / "module.ll"
            ll_text = read_text(ll_path) if ll_path.exists() else ""
            backend = (probe_dir / "module.object-backend.txt").read_text(encoding="utf-8").strip() if (probe_dir / "module.object-backend.txt").exists() else ""
            for check_id, condition, detail in (
                ("M261-D001-DYN-LL", ll_path.exists(), "missing emitted LLVM IR"),
                ("M261-D001-DYN-BACKEND", backend == "llvm-direct", f"expected llvm-direct backend, got {backend!r}"),
                ("M261-D001-DYN-SUMMARY", "runtime_block_api_object_layout = contract=objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1" in ll_text, "missing runtime block API/object layout summary"),
                ("M261-D001-DYN-PROMOTE", "@objc3_runtime_promote_block_i32" in ll_text, "missing promote helper symbol in emitted IR"),
                ("M261-D001-DYN-INVOKE", "@objc3_runtime_invoke_block_i32" in ll_text, "missing invoke helper symbol in emitted IR"),
            ):
                checks_total += require(condition, display_path(probe_dir), check_id, detail, failures)
                if condition:
                    checks_passed += 1
            exe_path, link_result = link_executable(probe_dir)
            probe_results["positive_link_rc"] = None if link_result is None else link_result.returncode
            checks_total += require(exe_path is not None and link_result is not None and link_result.returncode == 0, display_path(probe_dir), "M261-D001-DYN-LINK", "failed to link positive freeze probe", failures)
            if exe_path is not None and link_result is not None and link_result.returncode == 0:
                checks_passed += 1
                run_result = run_executable(exe_path)
                probe_results["positive_exit"] = run_result.returncode
                checks_total += require(run_result.returncode == 14, display_path(exe_path), "M261-D001-DYN-RUN", f"expected exit 14, got {run_result.returncode}", failures)
                if run_result.returncode == 14:
                    checks_passed += 1

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": "M261-D001",
        "contract_id": CONTRACT_ID,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "probe_results": probe_results,
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
