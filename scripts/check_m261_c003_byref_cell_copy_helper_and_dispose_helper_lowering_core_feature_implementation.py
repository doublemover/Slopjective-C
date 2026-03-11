#!/usr/bin/env python3
"""Checker for M261-C003 byref/copy-dispose helper lowering."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-c003-byref-copy-dispose-helper-lowering-v1"
CONTRACT_ID = "objc3c-executable-block-byref-helper-lowering/m261-c003-v1"
ACTIVE_MODEL = (
    "native-lowering-emits-stack-byref-cells-and-copy-dispose-helper-bodies-for-nonescaping-block-captures"
)
DEFERRED_MODEL = (
    "heap-promotion-and-runtime-managed-block-copy-dispose-lifecycle-remain-deferred-until-m261-c004-and-m261-d002"
)
EXECUTION_EVIDENCE_MODEL = (
    "native-compile-link-run-proves-byref-mutation-and-owned-capture-helper-lowering-through-emitted-block-helper-bodies"
)
NEXT_ISSUE = "M261-C004"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-C003" / "byref_cell_copy_dispose_helper_lowering_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_c003_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
BASELINE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_executable_block_object_invoke_thunk_positive.objc3"
BYREF_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_byref_cell_copy_dispose_runtime_positive.objc3"
OWNED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_owned_object_capture_runtime_positive.objc3"
NONOWNING_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_nonowning_object_capture_runtime_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "c003-byref-copy-dispose-helper"


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
        SnippetCheck("M261-C003-EXP-01", "# M261 Byref-Cell Copy-Helper And Dispose-Helper Lowering Core Feature Implementation Expectations (C003)"),
        SnippetCheck("M261-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-C003-EXP-03", "Issue: `#7187`"),
        SnippetCheck("M261-C003-EXP-04", "`M261-C004`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-C003-PKT-01", "# M261-C003 Byref-Cell Copy-Helper And Dispose-Helper Lowering Core Feature Implementation Packet"),
        SnippetCheck("M261-C003-PKT-02", "Issue: `#7187`"),
        SnippetCheck("M261-C003-PKT-03", "Packet: `M261-C003`"),
        SnippetCheck("M261-C003-PKT-04", "`M261-C004` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-C003-SRC-01", "## M261 byref-cell, copy-helper, and dispose-helper lowering (M261-C003)"),
        SnippetCheck("M261-C003-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C003-SRC-03", "`m261_byref_cell_copy_dispose_runtime_positive.objc3` with exit `14`"),
        SnippetCheck("M261-C003-SRC-04", "`M261-C004` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-C003-NDOC-01", "## M261 byref-cell, copy-helper, and dispose-helper lowering (M261-C003)"),
        SnippetCheck("M261-C003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C003-NDOC-03", "`m261_owned_object_capture_runtime_positive.objc3` with exit `11`"),
        SnippetCheck("M261-C003-NDOC-04", "`M261-C004` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-C003-P0-01", "(`M261-C003`)"),
        SnippetCheck("M261-C003-P0-02", "stack byref cells plus copy/dispose helper bodies"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-C003-SPC-01", "## M261 byref-cell, copy-helper, and dispose-helper lowering (C003)"),
        SnippetCheck("M261-C003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C003-SPC-03", "manifest lowering surfaces publish deterministic byref layout symbols"),
        SnippetCheck("M261-C003-SPC-04", "`M261-C004` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-C003-ARCH-01", "## M261 Byref-Cell, Copy-Helper, And Dispose-Helper Lowering (C003)"),
        SnippetCheck("M261-C003-ARCH-02", "local byref mutation now lowers through emitted stack byref-cell storage"),
        SnippetCheck("M261-C003-ARCH-03", "the next issue is `M261-C004`"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-C003-AST-01", "kObjc3ExecutableBlockByrefHelperLoweringContractId"),
        SnippetCheck("M261-C003-AST-02", "kObjc3ExecutableBlockByrefHelperLoweringActiveModel"),
        SnippetCheck("M261-C003-AST-03", "kObjc3ExecutableBlockByrefHelperLoweringExecutionEvidenceModel"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-C003-SEMA-PM-01", "M261-C003 byref-cell/copy-helper/dispose-helper anchor"),
        SnippetCheck("M261-C003-SEMA-PM-02", "live byref layout and runtime helper fields"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-C003-SEMA-01", "M261-C003 byref-cell/copy-helper/dispose-helper anchor"),
        SnippetCheck("M261-C003-SEMA-02", "block_storage_byref_slot_count = expr->block_byref_capture_count"),
        SnippetCheck("M261-C003-SEMA-03", "block_storage_byref_layout_symbol"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-C003-LOWER-H-01", "kObjc3ExecutableBlockByrefHelperLoweringLaneContract"),
        SnippetCheck("M261-C003-LOWER-H-02", "std::string Objc3ExecutableBlockByrefHelperLoweringSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-C003-LOWER-CPP-01", "std::string Objc3ExecutableBlockByrefHelperLoweringSummary()"),
        SnippetCheck("M261-C003-LOWER-CPP-02", "execution_evidence_model="),
    ),
    IR_CPP: (
        SnippetCheck("M261-C003-IR-01", "M261-C003 byref-cell/copy-helper/dispose-helper anchor"),
        SnippetCheck("M261-C003-IR-02", "BlockLiteralRequiresEscapingRuntimeHooks"),
        SnippetCheck("M261-C003-IR-03", "EmitBlockCopyHelper(const Expr &expr)"),
        SnippetCheck("M261-C003-IR-04", "EmitBlockDisposeHelper(const Expr &expr)"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-C003-PKG-01", '"check:objc3c:m261-c003-byref-copy-dispose-helper-lowering": "python scripts/check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py"'),
        SnippetCheck("M261-C003-PKG-02", '"test:tooling:m261-c003-byref-copy-dispose-helper-lowering": "python -m pytest tests/tooling/test_check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py -q"'),
        SnippetCheck("M261-C003-PKG-03", '"check:objc3c:m261-c003-lane-c-readiness": "python scripts/run_m261_c003_lane_c_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-C003-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-C003-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-C003-RUN-03", "check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py"),
        SnippetCheck("M261-C003-RUN-04", "check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py"),
        SnippetCheck("M261-C003-RUN-05", "check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-C003-TEST-01", "def test_m261_c003_checker_emits_summary() -> None:"),
        SnippetCheck("M261-C003-TEST-02", CONTRACT_ID),
    ),
    BYREF_FIXTURE: (
        SnippetCheck("M261-C003-FIX-BYREF-01", "module m261_byref_cell_copy_dispose_runtime_positive;"),
        SnippetCheck("M261-C003-FIX-BYREF-02", "seed = seed + delta;"),
    ),
    OWNED_FIXTURE: (
        SnippetCheck("M261-C003-FIX-OWNED-01", "module m261_owned_object_capture_runtime_positive;"),
        SnippetCheck("M261-C003-FIX-OWNED-02", "fn helper(ownedValue: id, weakValue: __weak id) -> i32 {"),
    ),
    NONOWNING_FIXTURE: (
        SnippetCheck("M261-C003-FIX-NONOWNING-01", "module m261_nonowning_object_capture_runtime_positive;"),
        SnippetCheck("M261-C003-FIX-NONOWNING-02", "fn helper(weakValue: __weak id, borrowedValue: __unsafe_unretained id) -> i32 {"),
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


def diagnostics_codes(path: Path) -> list[str]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics", [])
    if not isinstance(diagnostics, list):
        return []
    return [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]


def semantic_surface(manifest: dict[str, Any], key: str) -> dict[str, Any]:
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    surfaces = pipeline.get("semantic_surface", {})
    surface = surfaces.get(key)
    if not isinstance(surface, dict):
        return {}
    return surface


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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    cases = [
        {
            "id": "baseline",
            "fixture": BASELINE_FIXTURE,
            "out_dir": PROBE_ROOT / "baseline",
            "expected_exit": 15,
            "expect_byref": False,
            "expect_owned_helpers": False,
            "expect_nonowning": False,
        },
        {
            "id": "byref",
            "fixture": BYREF_FIXTURE,
            "out_dir": PROBE_ROOT / "byref",
            "expected_exit": 14,
            "expect_byref": True,
            "expect_owned_helpers": False,
            "expect_nonowning": False,
        },
        {
            "id": "owned",
            "fixture": OWNED_FIXTURE,
            "out_dir": PROBE_ROOT / "owned",
            "expected_exit": 11,
            "expect_byref": False,
            "expect_owned_helpers": True,
            "expect_nonowning": False,
        },
        {
            "id": "nonowning",
            "fixture": NONOWNING_FIXTURE,
            "out_dir": PROBE_ROOT / "nonowning",
            "expected_exit": 9,
            "expect_byref": False,
            "expect_owned_helpers": False,
            "expect_nonowning": True,
        },
    ]

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-C003-DYN-01", "native binary is missing", failures)
    for index, case in enumerate(cases, start=2):
        checks_total += require(case["fixture"].exists(), display_path(case["fixture"]), f"M261-C003-DYN-{index:02d}", "fixture is missing", failures)
    if failures:
        return checks_total, payload

    for case in cases:
        out_dir = case["out_dir"]
        compile_result = compile_fixture(case["fixture"], out_dir)
        diag_path = out_dir / "module.diagnostics.json"
        ir_path = out_dir / "module.ll"
        obj_path = out_dir / "module.obj"
        backend_path = out_dir / "module.object-backend.txt"
        reg_manifest_path = out_dir / "module.runtime-registration-manifest.json"
        rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"

        case_failures_start = len(failures)
        checks_total += require(compile_result.returncode == 0, display_path(out_dir), f"M261-C003-{case['id'].upper()}-COMPILE", f"compile failed: {compile_result.stdout}{compile_result.stderr}", failures)
        checks_total += require(diag_path.exists(), display_path(diag_path), f"M261-C003-{case['id'].upper()}-DIAG", "diagnostics output is missing", failures)
        diag_codes: list[str] = diagnostics_codes(diag_path) if diag_path.exists() else []
        checks_total += require(diag_codes == [], display_path(diag_path), f"M261-C003-{case['id'].upper()}-DIAG-CODES", f"expected empty diagnostics, observed {diag_codes}", failures)
        checks_total += require(ir_path.exists(), display_path(ir_path), f"M261-C003-{case['id'].upper()}-IR", "IR output is missing", failures)
        checks_total += require(obj_path.exists() and obj_path.stat().st_size > 0, display_path(obj_path), f"M261-C003-{case['id'].upper()}-OBJ", "object output is missing or empty", failures)
        checks_total += require(backend_path.exists(), display_path(backend_path), f"M261-C003-{case['id'].upper()}-BACKEND", "object backend marker is missing", failures)
        backend_text = read_text(backend_path).strip() if backend_path.exists() else ""
        checks_total += require(backend_text == "llvm-direct", display_path(backend_path), f"M261-C003-{case['id'].upper()}-BACKEND-VALUE", f"unexpected object backend marker: {backend_text}", failures)
        checks_total += require(reg_manifest_path.exists(), display_path(reg_manifest_path), f"M261-C003-{case['id'].upper()}-REG", "registration manifest is missing", failures)
        checks_total += require(rsp_path.exists(), display_path(rsp_path), f"M261-C003-{case['id'].upper()}-RSP", "linker options rsp is missing", failures)

        ir_text = read_text(ir_path) if ir_path.exists() else ""
        checks_total += require("; executable_block_byref_helper_lowering = " in ir_text, display_path(ir_path), f"M261-C003-{case['id'].upper()}-SUMMARY", "IR is missing the C003 summary line", failures)

        manifest = load_json(reg_manifest_path.parent / "module.manifest.json") if (reg_manifest_path.parent / "module.manifest.json").exists() else {}
        storage_surface = semantic_surface(manifest, "objc_block_storage_escape_lowering_surface")
        copy_surface = semantic_surface(manifest, "objc_block_copy_dispose_lowering_surface")

        if case["expect_byref"]:
            checks_total += require("@__objc3_block_copy_helper_" in ir_text, display_path(ir_path), "M261-C003-BYREF-IR-COPY", "byref IR is missing the copy helper symbol", failures)
            checks_total += require("@__objc3_block_dispose_helper_" in ir_text, display_path(ir_path), "M261-C003-BYREF-IR-DISPOSE", "byref IR is missing the dispose helper symbol", failures)
            checks_total += require(int(storage_surface.get("byref_slot_count_total", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-BYREF-SFC-BYREF", "byref manifest did not publish byref slots", failures)
            checks_total += require(int(storage_surface.get("requires_byref_cells_sites", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-BYREF-SFC-CELLS", "byref manifest did not require byref cells", failures)
            checks_total += require(int(storage_surface.get("byref_layout_symbolized_sites", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-BYREF-SFC-LAYOUT", "byref manifest did not publish byref layout symbols", failures)
            checks_total += require(int(copy_surface.get("copy_helper_required_sites", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-BYREF-SFC-COPY", "byref manifest did not require copy helper", failures)
            checks_total += require(int(copy_surface.get("dispose_helper_required_sites", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-BYREF-SFC-DISPOSE", "byref manifest did not require dispose helper", failures)
        if case["expect_owned_helpers"]:
            checks_total += require("call i32 @objc3_runtime_retain_i32" in ir_text, display_path(ir_path), "M261-C003-OWNED-IR-RETAIN", "owned capture IR is missing retain helper calls", failures)
            checks_total += require("call i32 @objc3_runtime_release_i32" in ir_text, display_path(ir_path), "M261-C003-OWNED-IR-RELEASE", "owned capture IR is missing release helper calls", failures)
            checks_total += require(int(copy_surface.get("copy_helper_required_sites", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-OWNED-SFC-COPY", "owned capture manifest did not require copy helper", failures)
            checks_total += require(int(copy_surface.get("dispose_helper_required_sites", 0)) >= 1, display_path(out_dir / "module.manifest.json"), "M261-C003-OWNED-SFC-DISPOSE", "owned capture manifest did not require dispose helper", failures)
        if case["expect_nonowning"]:
            checks_total += require("define internal void @__objc3_block_copy_helper_" not in ir_text, display_path(ir_path), "M261-C003-NONOWNING-IR-COPY", "nonowning capture IR unexpectedly emitted copy helper bodies", failures)
            checks_total += require("define internal void @__objc3_block_dispose_helper_" not in ir_text, display_path(ir_path), "M261-C003-NONOWNING-IR-DISPOSE", "nonowning capture IR unexpectedly emitted dispose helper bodies", failures)
            checks_total += require(int(copy_surface.get("copy_helper_required_sites", 0)) == 0, display_path(out_dir / "module.manifest.json"), "M261-C003-NONOWNING-SFC-COPY", "nonowning manifest unexpectedly required copy helper", failures)
            checks_total += require(int(copy_surface.get("dispose_helper_required_sites", 0)) == 0, display_path(out_dir / "module.manifest.json"), "M261-C003-NONOWNING-SFC-DISPOSE", "nonowning manifest unexpectedly required dispose helper", failures)

        exe_path, link_result = link_executable(out_dir)
        checks_total += require(link_result is not None and link_result.returncode == 0, display_path(out_dir), f"M261-C003-{case['id'].upper()}-LINK", "link step failed", failures)
        checks_total += require(exe_path is not None and exe_path.exists(), display_path(out_dir), f"M261-C003-{case['id'].upper()}-EXE", "linked executable is missing", failures)
        run_result = run_executable(exe_path) if exe_path is not None and exe_path.exists() else None
        expected_exit = int(case["expected_exit"])
        checks_total += require(run_result is not None and run_result.returncode == expected_exit, display_path(exe_path if exe_path is not None else out_dir), f"M261-C003-{case['id'].upper()}-RUN", f"unexpected run exit: {None if run_result is None else run_result.returncode}, expected {expected_exit}", failures)

        payload[f"{case['id']}_case"] = {
            "fixture": display_path(case["fixture"]),
            "out_dir": display_path(out_dir),
            "compile_returncode": compile_result.returncode,
            "diagnostic_codes": diag_codes,
            "backend": backend_text,
            "link_returncode": None if link_result is None else link_result.returncode,
            "run_returncode": None if run_result is None else run_result.returncode,
            "storage_surface": storage_surface,
            "copy_dispose_surface": copy_surface,
            "case_findings": len(failures) - case_failures_start,
        }

    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        current_total, current_findings = check_static_contract(path, snippets)
        checks_total += current_total
        findings.extend(current_findings)

    dynamic_payload: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_payload = {
            "baseline_case": {"skipped": True},
            "byref_case": {"skipped": True},
            "owned_case": {"skipped": True},
            "nonowning_case": {"skipped": True},
        }
    else:
        current_total, dynamic_payload = run_dynamic_probes(findings)
        checks_total += current_total

    summary = {
        "ok": not findings,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "active_model": ACTIVE_MODEL,
        "deferred_model": DEFERRED_MODEL,
        "execution_evidence_model": EXECUTION_EVIDENCE_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in findings],
        **dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}")
        print(f"wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
