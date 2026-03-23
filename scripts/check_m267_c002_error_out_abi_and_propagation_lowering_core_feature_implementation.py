#!/usr/bin/env python3
"""Checker for M267-C002 runnable error-out ABI and propagation lowering."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-c002-error-out-abi-and-propagation-lowering-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_error_out_abi_and_propagation_lowering_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation_packet.md"
DOC_NATIVE_SRC = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_LOWER = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
SPEC_PART6 = ROOT / "spec" / "PART_6_ERRORS_RESULTS_THROWS.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_c002_error_out_abi_positive.objc3"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-C002" / "error_out_abi_and_propagation_lowering_summary.json"
OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "c002-error-out-abi-propagation"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M267-C002-EXP-01", "# M267 Error-Out ABI And Propagation Lowering Core Feature Implementation Expectations (C002)"),
        SnippetCheck("M267-C002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-C002-EXP-03", "ready_for_runtime_execution=true"),
        SnippetCheck("M267-C002-EXP-04", "linked executable exits with code `102`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-C002-PKT-01", "# M267-C002 Error-Out ABI And Propagation Lowering Core Feature Implementation Packet"),
        SnippetCheck("M267-C002-PKT-02", "Issue: `#7275`"),
        SnippetCheck("M267-C002-PKT-03", "`tests/tooling/fixtures/native/m267_c002_error_out_abi_positive.objc3`"),
        SnippetCheck("M267-C002-PKT-04", "`next_issue=M267-C003`"),
    ),
    DOC_NATIVE_SRC: (
        SnippetCheck("M267-C002-DSRC-01", "## M267 Part 6 runnable error-out ABI and propagation lowering (M267-C002)"),
        SnippetCheck("M267-C002-DSRC-02", "`objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M267-C002-DNAT-01", "## M267 Part 6 runnable error-out ABI and propagation lowering (M267-C002)"),
        SnippetCheck("M267-C002-DNAT-02", "ready_for_runtime_execution=true"),
    ),
    SPEC_AM: (
        SnippetCheck("M267-C002-SAM-01", "M267-C002 lowering implementation note:"),
        SnippetCheck("M267-C002-SAM-02", "`try`, `try?`, and `try!` branch over success and propagated-error paths"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M267-C002-SAT-01", "Current implementation status (`M267-C002`):"),
        SnippetCheck("M267-C002-SAT-02", "`ready_for_runtime_execution=true`"),
    ),
    SPEC_LOWER: (
        SnippetCheck("M267-C002-SLOW-01", "## M267 Part 6 runnable error-out ABI and propagation lowering (C002)"),
        SnippetCheck("M267-C002-SLOW-02", "`objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`"),
    ),
    SPEC_PART6: (
        SnippetCheck("M267-C002-SP6-01", "Current implementation status (`M267-C002`):"),
        SnippetCheck("M267-C002-SP6-02", "`ready_for_runtime_execution=true`"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M267-C002-LH-01", "objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1"),
        SnippetCheck("M267-C002-LH-02", "native-lowering-emits-hidden-error-out-abi-propagation-operators-and-do-catch-control-flow-through-real-ir-and-object-artifacts"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M267-C002-LCPP-01", "// M267-C002 Part 6 lowering implementation anchor"),
        SnippetCheck("M267-C002-LCPP-02", ";next_issue=M267-C003"),
    ),
    IR_EMITTER: (
        SnippetCheck("M267-C002-IR-01", 'if (expr->ident == "__objc3_throw_stmt")'),
        SnippetCheck("M267-C002-IR-02", 'if (expr->ident == "__objc3_try_expr")'),
        SnippetCheck("M267-C002-IR-03", 'if (block_stmt->is_do_catch_scope) {'),
        SnippetCheck("M267-C002-IR-04", 'ctx.function_error_out_param = "%error_out";'),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M267-C002-FA-01", "ready_for_runtime_execution=true"),
        SnippetCheck("M267-C002-FA-02", '\\",\\"next_issue\\":\\"M267-C003\\"'),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-C002-PKG-01", '"check:objc3c:m267-c002-error-out-abi-and-propagation-lowering-core-feature-implementation"'),
        SnippetCheck("M267-C002-PKG-02", '"test:tooling:m267-c002-error-out-abi-and-propagation-lowering-core-feature-implementation"'),
        SnippetCheck("M267-C002-PKG-03", '"check:objc3c:m267-c002-lane-c-readiness"'),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd or ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def semantic_surface_from_manifest(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(manifest_path)}")
    return semantic_surface


def resolve_clangxx() -> str | None:
    for candidate in (shutil.which("clang++.exe"), shutil.which("clang++"), r"C:\Program Files\LLVM\bin\clang++.exe"):
        if candidate and Path(candidate).exists():
            return candidate
    return None


def ensure_native_build(failures: list[Finding]) -> tuple[int, int]:
    command = [
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m267-c002-check",
        "--summary-out",
        "tmp/reports/m267/M267-C002/ensure_objc3c_native_build_summary.json",
    ]
    result = run_command(command)
    combined = (result.stdout or "") + (result.stderr or "")
    total = 1
    passed = require(result.returncode == 0, display_path(BUILD_HELPER), "M267-C002-BUILD-01", f"ensure build failed: {combined}", failures)
    return total, passed


def run_positive_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    payload: dict[str, Any] = {}
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    compile_command = [str(NATIVE_EXE), str(FIXTURE), "--out-dir", str(OUT_DIR), "--emit-prefix", "module"]
    compile_result = run_command(compile_command)
    payload["compile_command"] = compile_command
    payload["compile_exit_code"] = compile_result.returncode
    total += 1; passed += require(compile_result.returncode == 0, display_path(FIXTURE), "M267-C002-DYN-01", f"native compile failed: {(compile_result.stderr or compile_result.stdout)}", failures)

    ir_path = OUT_DIR / "module.ll"
    manifest_path = OUT_DIR / "module.manifest.json"
    obj_path = OUT_DIR / "module.obj"
    backend_path = OUT_DIR / "module.object-backend.txt"
    exe_path = OUT_DIR / "module.exe"

    total += 1; passed += require(ir_path.exists(), display_path(ir_path), "M267-C002-DYN-02", "module.ll missing", failures)
    total += 1; passed += require(manifest_path.exists(), display_path(manifest_path), "M267-C002-DYN-03", "module.manifest.json missing", failures)
    total += 1; passed += require(obj_path.exists(), display_path(obj_path), "M267-C002-DYN-04", "module.obj missing", failures)
    total += 1; passed += require(backend_path.exists(), display_path(backend_path), "M267-C002-DYN-05", "module.object-backend.txt missing", failures)

    if backend_path.exists():
        backend = read_text(backend_path).strip()
        payload["object_backend"] = backend
        total += 1; passed += require(backend == "llvm-direct", display_path(backend_path), "M267-C002-DYN-06", f"unexpected object backend: {backend}", failures)

    if ir_path.exists():
        ir_text = read_text(ir_path)
        payload["ir_comment_present"] = "; part6_throws_abi_propagation_lowering = " in ir_text
        payload["ir_metadata_present"] = "!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}" in ir_text
        checks = (
            ("M267-C002-DYN-07", re.search(r"define i32 @recover\(ptr %error_out\)", ir_text) is not None, "recover must carry hidden error-out parameter"),
            ("M267-C002-DYN-08", re.search(r"define i32 @bubble\(ptr %error_out\)", ir_text) is not None, "bubble must carry hidden error-out parameter"),
            ("M267-C002-DYN-09", "!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}" in ir_text, "IR metadata anchor missing"),
            ("M267-C002-DYN-10", "; part6_throws_abi_propagation_lowering = " in ir_text, "IR comment anchor missing"),
            ("M267-C002-DYN-11", re.search(r"br i1 .*label %try_fail_.*label %try_success_", ir_text) is not None, "try lowering branch surface missing"),
            ("M267-C002-DYN-12", "do_catch_dispatch_" in ir_text and "catch_" in ir_text, "do/catch dispatch surface missing"),
            ("M267-C002-DYN-13", re.search(r"call i32 @bridgeMapping\(i32 %t\d+\)", ir_text) is not None, "bridge mapping call missing"),
        )
        for check_id, condition, detail in checks:
            total += 1
            passed += require(condition, display_path(ir_path), check_id, detail, failures)

    if manifest_path.exists():
        surface = semantic_surface_from_manifest(manifest_path)
        lowering = surface.get("objc_part6_throws_abi_propagation_lowering")
        if not isinstance(lowering, dict):
            lowering = {}
        payload["manifest_contract_id"] = lowering.get("contract_id")
        payload["manifest_ready_for_runtime_execution"] = lowering.get("ready_for_runtime_execution")
        payload["manifest_next_issue"] = lowering.get("next_issue")
        checks = (
            ("M267-C002-DYN-14", lowering.get("contract_id") == CONTRACT_ID, "manifest contract id drifted"),
            ("M267-C002-DYN-15", lowering.get("ready_for_runtime_execution") is True, "manifest must publish runnable readiness"),
            ("M267-C002-DYN-16", lowering.get("next_issue") == "M267-C003", "manifest next_issue must advance to M267-C003"),
        )
        for check_id, condition, detail in checks:
            total += 1
            passed += require(condition, display_path(manifest_path), check_id, detail, failures)

    clangxx = resolve_clangxx()
    total += 1; passed += require(clangxx is not None, "clang++", "M267-C002-DYN-17", "unable to resolve clang++", failures)
    if clangxx is not None and obj_path.exists() and RUNTIME_LIB.exists():
        link_command = [clangxx, str(obj_path), str(RUNTIME_LIB), "-o", str(exe_path)]
        link_result = run_command(link_command)
        payload["link_command"] = link_command
        payload["link_exit_code"] = link_result.returncode
        payload["link_stderr"] = link_result.stderr
        total += 1; passed += require(link_result.returncode == 0, display_path(obj_path), "M267-C002-DYN-18", f"link failed: {(link_result.stderr or link_result.stdout)}", failures)
        if link_result.returncode == 0:
            run_result = run_command([str(exe_path)])
            payload["exe_exit_code"] = run_result.returncode
            total += 1; passed += require(run_result.returncode == 102, display_path(exe_path), "M267-C002-DYN-19", f"expected exit 102, saw {run_result.returncode}", failures)
    return total, passed, payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    for path, snippets in SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic: dict[str, Any] = {"executed": False}
    if not args.skip_dynamic_probes:
        dynamic["executed"] = True
        total, passed = ensure_native_build(failures)
        checks_total += total; checks_passed += passed
        total, passed, payload = run_positive_probe(failures)
        checks_total += total; checks_passed += passed; dynamic["positive_case"] = payload

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "checks_total": checks_total,
        "dynamic_probes_executed": dynamic["executed"],
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
