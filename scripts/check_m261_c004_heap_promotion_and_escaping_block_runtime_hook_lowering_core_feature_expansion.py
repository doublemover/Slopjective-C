#!/usr/bin/env python3
"""Checker for M261-C004 escaping-block runtime hook lowering."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-c004-block-escape-runtime-hook-lowering-v1"
CONTRACT_ID = "objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1"
ACTIVE_MODEL = (
    "native-lowering-emits-runtime-block-promotion-and-invoke-hooks-for-readonly-scalar-escaping-block-values"
)
DEFERRED_MODEL = (
    "byref-forwarding-owned-capture-escape-lifetimes-and-runtime-managed-copy-dispose-remain-deferred-until-m261-d002-and-m261-d003"
)
EXECUTION_EVIDENCE_MODEL = (
    "native-compile-link-run-proves-returned-and-argument-passed-readonly-scalar-block-values-through-runtime-promotion-hooks"
)
NEXT_ISSUE = "M261-D001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-C004" / "escaping_block_runtime_hook_lowering_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion_c004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_c004_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
ARGUMENT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_argument_positive.objc3"
RETURN_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_return_positive.objc3"
BYREF_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_byref_negative.objc3"
OWNED_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_owned_capture_negative.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "c004-block-escape-runtime-hook"


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
        SnippetCheck("M261-C004-EXP-01", "# M261 Heap-Promotion And Escaping-Block Runtime Hook Lowering Core Feature Expansion Expectations (C004)"),
        SnippetCheck("M261-C004-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-C004-EXP-03", "Issue: `#7188`"),
        SnippetCheck("M261-C004-EXP-04", "`M261-D001`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-C004-PKT-01", "# M261-C004 Heap-Promotion And Escaping-Block Runtime Hook Lowering Core Feature Expansion Packet"),
        SnippetCheck("M261-C004-PKT-02", "Issue: `#7188`"),
        SnippetCheck("M261-C004-PKT-03", "Packet: `M261-C004`"),
        SnippetCheck("M261-C004-PKT-04", "`M261-D001` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-C004-SRC-01", "## M261 heap-promotion and escaping-block runtime hook lowering (M261-C004)"),
        SnippetCheck("M261-C004-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C004-SRC-03", "`m261_escaping_block_runtime_hook_argument_positive.objc3` with exit `14`"),
        SnippetCheck("M261-C004-SRC-04", "`M261-D001` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-C004-NDOC-01", "## M261 heap-promotion and escaping-block runtime hook lowering (M261-C004)"),
        SnippetCheck("M261-C004-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C004-NDOC-03", "`m261_escaping_block_runtime_hook_return_positive.objc3` with exit `0`"),
        SnippetCheck("M261-C004-NDOC-04", "`M261-D001` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-C004-P0-01", "(`M261-C004`)"),
        SnippetCheck("M261-C004-P0-02", "private runtime promotion/invoke hooks"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-C004-SPC-01", "## M261 heap-promotion and escaping-block runtime hook lowering (C004)"),
        SnippetCheck("M261-C004-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C004-SPC-03", "@objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-C004-SPC-04", "`M261-D001` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-C004-ARCH-01", "## M261 Heap-Promotion And Escaping-Block Runtime Hook Lowering (C004)"),
        SnippetCheck("M261-C004-ARCH-02", "objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-C004-ARCH-03", "the next issue is `M261-D001`"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-C004-AST-01", "kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId"),
        SnippetCheck("M261-C004-AST-02", "kObjc3ExecutableBlockEscapeRuntimeHookLoweringActiveModel"),
        SnippetCheck("M261-C004-AST-03", "kObjc3ExecutableBlockEscapeRuntimeHookLoweringExecutionEvidenceModel"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-C004-PARSE-01", "M261-C004 escaping-block runtime-hook anchor"),
        SnippetCheck("M261-C004-PARSE-02", "truthful heap-promotion candidates"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-C004-SEMA-PM-01", "M261-C004 escaping-block runtime-hook anchor"),
        SnippetCheck("M261-C004-SEMA-PM-02", "readonly-scalar escaping slice"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-C004-SEMA-01", "M261-C004 escaping-block runtime-hook anchor"),
        SnippetCheck("M261-C004-SEMA-02", "block_storage_escape_to_heap ="),
        SnippetCheck("M261-C004-SEMA-03", "IsEscapingBlockRuntimeHandleCompatible"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-C004-LOWER-H-01", "kObjc3RuntimePromoteBlockI32Symbol"),
        SnippetCheck("M261-C004-LOWER-H-02", "kObjc3RuntimeInvokeBlockI32Symbol"),
        SnippetCheck("M261-C004-LOWER-H-03", "std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-C004-LOWER-CPP-01", "std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary()"),
        SnippetCheck("M261-C004-LOWER-CPP-02", "kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract"),
    ),
    IR_CPP: (
        SnippetCheck("M261-C004-IR-01", "M261-C004 escaping-block runtime-hook anchor"),
        SnippetCheck("M261-C004-IR-02", "EmitPromotedBlockHandle("),
        SnippetCheck("M261-C004-IR-03", "EmitPromotedBlockHandleLoad("),
        SnippetCheck("M261-C004-IR-04", "kObjc3RuntimePromoteBlockI32Symbol"),
        SnippetCheck("M261-C004-IR-05", "kObjc3RuntimeInvokeBlockI32Symbol"),
    ),
    RUNTIME_HEADER: (
        SnippetCheck("M261-C004-RH-01", "objc3_runtime_promote_block_i32"),
        SnippetCheck("M261-C004-RH-02", "objc3_runtime_invoke_block_i32"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M261-C004-RCPP-01", 'extern "C" int objc3_runtime_promote_block_i32'),
        SnippetCheck("M261-C004-RCPP-02", 'extern "C" int objc3_runtime_invoke_block_i32'),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-C004-PKG-01", '"check:objc3c:m261-c004-block-escape-runtime-hook-lowering": "python scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py"'),
        SnippetCheck("M261-C004-PKG-02", '"test:tooling:m261-c004-block-escape-runtime-hook-lowering": "python -m pytest tests/tooling/test_check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py -q"'),
        SnippetCheck("M261-C004-PKG-03", '"check:objc3c:m261-c004-lane-c-readiness": "python scripts/run_m261_c004_lane_c_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-C004-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M261-C004-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-C004-RUN-03", "check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py"),
        SnippetCheck("M261-C004-RUN-04", "check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py"),
        SnippetCheck("M261-C004-RUN-05", "check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-C004-TEST-01", "def test_m261_c004_checker_emits_summary() -> None:"),
        SnippetCheck("M261-C004-TEST-02", CONTRACT_ID),
    ),
    ARGUMENT_FIXTURE: (
        SnippetCheck("M261-C004-FIX-ARG-01", "module m261_escaping_block_runtime_hook_argument_positive;"),
        SnippetCheck("M261-C004-FIX-ARG-02", "let handle = materialize(closure);"),
        SnippetCheck("M261-C004-FIX-ARG-03", "return closure(5);"),
    ),
    RETURN_FIXTURE: (
        SnippetCheck("M261-C004-FIX-RET-01", "module m261_escaping_block_runtime_hook_return_positive;"),
        SnippetCheck("M261-C004-FIX-RET-02", "return closure;"),
    ),
    BYREF_NEGATIVE_FIXTURE: (
        SnippetCheck("M261-C004-FIX-BYREF-01", "module m261_escaping_block_runtime_hook_byref_negative;"),
        SnippetCheck("M261-C004-FIX-BYREF-02", "seed = seed + delta;"),
    ),
    OWNED_NEGATIVE_FIXTURE: (
        SnippetCheck("M261-C004-FIX-OWNED-01", "module m261_escaping_block_runtime_hook_owned_capture_negative;"),
        SnippetCheck("M261-C004-FIX-OWNED-02", "fn helper(ownedValue: id, weakValue: __weak id) -> i32 {"),
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


def diagnostics_text(out_dir: Path) -> str:
    diagnostics_path = out_dir / "module.diagnostics.txt"
    if diagnostics_path.exists():
        return read_text(diagnostics_path)
    payload_path = out_dir / "module.diagnostics.json"
    if payload_path.exists():
        payload = load_json(payload_path)
        diagnostics = payload.get("diagnostics")
        if isinstance(diagnostics, list):
            return json.dumps(diagnostics, indent=2)
    return ""


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
        checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-C004-DYN-EXE", "missing native compiler executable", failures)
        if NATIVE_EXE.exists():
            checks_passed += 1

        # Positive: call-argument escape plus promoted invoke.
        arg_dir = PROBE_ROOT / "argument-positive"
        arg_compile = compile_fixture(ARGUMENT_FIXTURE, arg_dir)
        probe_results["argument_positive_compile_rc"] = arg_compile.returncode
        checks_total += require(arg_compile.returncode == 0, display_path(ARGUMENT_FIXTURE), "M261-C004-DYN-ARG-COMPILE", f"expected successful compile, got rc={arg_compile.returncode}", failures)
        if arg_compile.returncode == 0:
            checks_passed += 1
            arg_ll = arg_dir / "module.ll"
            arg_backend = (arg_dir / "module.object-backend.txt").read_text(encoding="utf-8").strip() if (arg_dir / "module.object-backend.txt").exists() else ""
            arg_ll_text = read_text(arg_ll) if arg_ll.exists() else ""
            checks_total += require(arg_ll.exists(), display_path(arg_ll), "M261-C004-DYN-ARG-LL", "missing emitted LLVM IR", failures)
            if arg_ll.exists():
                checks_passed += 1
            checks_total += require(arg_backend == "llvm-direct", display_path(arg_dir / "module.object-backend.txt"), "M261-C004-DYN-ARG-BACKEND", f"expected llvm-direct backend, got {arg_backend!r}", failures)
            if arg_backend == "llvm-direct":
                checks_passed += 1
            for check_id, snippet in (
                ("M261-C004-DYN-ARG-SUMMARY", "executable_block_escape_runtime_hook_lowering"),
                ("M261-C004-DYN-ARG-PROMOTE-DECL", "declare i32 @objc3_runtime_promote_block_i32"),
                ("M261-C004-DYN-ARG-INVOKE-DECL", "declare i32 @objc3_runtime_invoke_block_i32"),
                ("M261-C004-DYN-ARG-PROMOTE-CALL", "call i32 @objc3_runtime_promote_block_i32"),
                ("M261-C004-DYN-ARG-INVOKE-CALL", "call i32 @objc3_runtime_invoke_block_i32"),
            ):
                checks_total += require(snippet in arg_ll_text, display_path(arg_ll), check_id, f"missing IR snippet: {snippet}", failures)
                if snippet in arg_ll_text:
                    checks_passed += 1
            exe_path, link_result = link_executable(arg_dir)
            probe_results["argument_positive_link_rc"] = None if link_result is None else link_result.returncode
            checks_total += require(exe_path is not None and link_result is not None and link_result.returncode == 0, display_path(arg_dir), "M261-C004-DYN-ARG-LINK", "failed to link positive escaping-block executable", failures)
            if exe_path is not None and link_result is not None and link_result.returncode == 0:
                checks_passed += 1
                run_result = run_executable(exe_path)
                probe_results["argument_positive_exit"] = run_result.returncode
                checks_total += require(run_result.returncode == 14, display_path(exe_path), "M261-C004-DYN-ARG-RUN", f"expected exit 14, got {run_result.returncode}", failures)
                if run_result.returncode == 14:
                    checks_passed += 1

        # Positive: return-value escape.
        return_dir = PROBE_ROOT / "return-positive"
        return_compile = compile_fixture(RETURN_FIXTURE, return_dir)
        probe_results["return_positive_compile_rc"] = return_compile.returncode
        checks_total += require(return_compile.returncode == 0, display_path(RETURN_FIXTURE), "M261-C004-DYN-RET-COMPILE", f"expected successful compile, got rc={return_compile.returncode}", failures)
        if return_compile.returncode == 0:
            checks_passed += 1
            return_ll = return_dir / "module.ll"
            return_backend = (return_dir / "module.object-backend.txt").read_text(encoding="utf-8").strip() if (return_dir / "module.object-backend.txt").exists() else ""
            return_ll_text = read_text(return_ll) if return_ll.exists() else ""
            checks_total += require(return_ll.exists(), display_path(return_ll), "M261-C004-DYN-RET-LL", "missing emitted LLVM IR", failures)
            if return_ll.exists():
                checks_passed += 1
            checks_total += require(return_backend == "llvm-direct", display_path(return_dir / "module.object-backend.txt"), "M261-C004-DYN-RET-BACKEND", f"expected llvm-direct backend, got {return_backend!r}", failures)
            if return_backend == "llvm-direct":
                checks_passed += 1
            for check_id, snippet in (
                ("M261-C004-DYN-RET-SUMMARY", "executable_block_escape_runtime_hook_lowering"),
                ("M261-C004-DYN-RET-PROMOTE-DECL", "declare i32 @objc3_runtime_promote_block_i32"),
                ("M261-C004-DYN-RET-PROMOTE-CALL", "call i32 @objc3_runtime_promote_block_i32"),
            ):
                checks_total += require(snippet in return_ll_text, display_path(return_ll), check_id, f"missing IR snippet: {snippet}", failures)
                if snippet in return_ll_text:
                    checks_passed += 1
            exe_path, link_result = link_executable(return_dir)
            probe_results["return_positive_link_rc"] = None if link_result is None else link_result.returncode
            checks_total += require(exe_path is not None and link_result is not None and link_result.returncode == 0, display_path(return_dir), "M261-C004-DYN-RET-LINK", "failed to link return-value escaping-block executable", failures)
            if exe_path is not None and link_result is not None and link_result.returncode == 0:
                checks_passed += 1
                run_result = run_executable(exe_path)
                probe_results["return_positive_exit"] = run_result.returncode
                checks_total += require(run_result.returncode == 0, display_path(exe_path), "M261-C004-DYN-RET-RUN", f"expected exit 0, got {run_result.returncode}", failures)
                if run_result.returncode == 0:
                    checks_passed += 1

        # Historical compatibility: byref escape originally failed closed under
        # C004, but later lane-D issues may widen that slice. Accept either the
        # historical failure or successful lowering under D003+.
        byref_dir = PROBE_ROOT / "byref-negative"
        byref_compile = compile_fixture(BYREF_NEGATIVE_FIXTURE, byref_dir)
        byref_text = diagnostics_text(byref_dir)
        probe_results["byref_negative_compile_rc"] = byref_compile.returncode
        checks_total += require(byref_compile.returncode != 0 or (byref_dir / "module.obj").exists(), display_path(BYREF_NEGATIVE_FIXTURE), "M261-C004-DYN-BYREF-RC", "expected either historical compile failure or forward-compatible object emission", failures)
        if byref_compile.returncode != 0 or (byref_dir / "module.obj").exists():
            checks_passed += 1
        if byref_compile.returncode != 0:
            for check_id, snippet in (
                ("M261-C004-DYN-BYREF-DIAG-CODE", "O3L300"),
                ("M261-C004-DYN-BYREF-DIAG-TEXT", "later M261 issues"),
            ):
                checks_total += require(snippet in byref_text, display_path(byref_dir / "module.diagnostics.txt"), check_id, f"missing diagnostics snippet: {snippet}", failures)
                if snippet in byref_text:
                    checks_passed += 1

        # Historical compatibility: owned-object escape originally failed
        # closed under C004, but later lane-D issues may widen that slice.
        owned_dir = PROBE_ROOT / "owned-negative"
        owned_compile = compile_fixture(OWNED_NEGATIVE_FIXTURE, owned_dir)
        owned_text = diagnostics_text(owned_dir)
        probe_results["owned_negative_compile_rc"] = owned_compile.returncode
        checks_total += require(owned_compile.returncode != 0 or (owned_dir / "module.obj").exists(), display_path(OWNED_NEGATIVE_FIXTURE), "M261-C004-DYN-OWNED-RC", "expected either historical compile failure or forward-compatible object emission", failures)
        if owned_compile.returncode != 0 or (owned_dir / "module.obj").exists():
            checks_passed += 1
        if owned_compile.returncode != 0:
            for check_id, snippet in (
                ("M261-C004-DYN-OWNED-DIAG-CODE", "O3L300"),
                ("M261-C004-DYN-OWNED-DIAG-TEXT", "later M261 issues"),
            ):
                checks_total += require(snippet in owned_text, display_path(owned_dir / "module.diagnostics.txt"), check_id, f"missing diagnostics snippet: {snippet}", failures)
                if snippet in owned_text:
                    checks_passed += 1

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": "M261-C004",
        "contract_id": CONTRACT_ID,
        "active_model": ACTIVE_MODEL,
        "deferred_model": DEFERRED_MODEL,
        "execution_evidence_model": EXECUTION_EVIDENCE_MODEL,
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
