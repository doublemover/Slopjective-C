#!/usr/bin/env python3
"""Checker for M265-B002 optional flow/binding/refinement semantics."""

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
MODE = "m265-b002-optional-flow-binding-refinement-semantics-v1"
CONTRACT_ID = "objc3c-optional-flow-binding-refinement-semantics/m265-b002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-B002" / "optional_flow_binding_refinement_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_optional_flow_binding_and_refinement_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_b002_optional_flow_binding_and_refinement_semantics_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART3 = ROOT / "spec" / "PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_flow_binding_refinement_positive.objc3"
NEG_GUARD = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_guard_binding_missing_exit_negative.objc3"
NEG_ORDINARY_SEND = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_ordinary_send_nullable_receiver_negative.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b002"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M265-B002-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def manifest_semantic_packet(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part3_type_semantic_model")
    if not isinstance(packet, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface.objc_part3_type_semantic_model in {display_path(manifest_path)}")
    return packet


def resolve_clang() -> str:
    candidates = (
        shutil.which("clang"),
        shutil.which("clang.exe"),
        r"C:\Program Files\LLVM\bin\clang.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang"


def compile_native_fixture(native_exe: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(native_exe), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def run_source_only_case(runner_exe: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(runner_exe),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = json.loads(read_text(registration_manifest_path))
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    result = run_command(
        [resolve_clang(), str(obj_path), str(runtime_library_path), f"@{rsp_path.resolve()}", "-o", str(exe_path)],
        cwd=out_dir,
    )
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def validate_packet(packet: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: dict[str, Any] = {
        "contract_id": "objc3c-part3-type-semantic-model/m265-b001-v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_part3_type_semantic_model",
        "optional_binding_sites": 2,
        "optional_binding_clause_sites": 2,
        "guard_binding_sites": 1,
        "optional_send_sites": 1,
        "nil_coalescing_sites": 2,
        "optional_propagation_sites": 2,
        "optional_flow_refinement_sites": 3,
        "guard_binding_exit_enforcement_sites": 1,
        "optional_binding_contract_violation_sites": 0,
        "optional_send_contract_violation_sites": 0,
        "optional_flow_contract_violation_sites": 0,
    }
    for key, expected in expected_exact.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == expected, artifact, f"M265-B002-PKT-{key}", f"{key} mismatch", failures)
    for key in ("deterministic", "ready_for_lowering_and_runtime"):
        checks_total += 1
        checks_passed += require(packet.get(key) is True, artifact, f"M265-B002-PKT-{key}", f"{key} must be true", failures)
    checks_total += 1
    checks_passed += require(CONTRACT_ID.split("/")[0] not in str(packet.get("replay_key", "")) and "m265-b001" in str(packet.get("replay_key", "")), artifact, "M265-B002-PKT-replay", "replay key must remain anchored to the semantic packet lineage", failures)
    return checks_total, checks_passed


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-B002-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-B002-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m265-b002-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-B002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-B002-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-B002-DYN-runner", "frontend runner missing after build", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M265-B002-DYN-native", "native compiler missing after build", failures)

    positive_out = PROBE_ROOT / "positive"
    positive_run = compile_native_fixture(args.native_exe, POSITIVE_FIXTURE, positive_out)
    positive_manifest = positive_out / "module.manifest.json"
    positive_ir = positive_out / "module.ll"
    positive_obj = positive_out / "module.obj"
    positive_backend = positive_out / "module.object-backend.txt"
    positive_diag = positive_out / "module.diagnostics.json"
    exe_path, link_result = link_executable(positive_out)
    checks_total += 1
    checks_passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M265-B002-DYN-pos-rc", f"positive native compile failed: {positive_run.stderr or positive_run.stdout}", failures)
    for check_id, path in (
        ("M265-B002-DYN-pos-manifest", positive_manifest),
        ("M265-B002-DYN-pos-ir", positive_ir),
        ("M265-B002-DYN-pos-obj", positive_obj),
        ("M265-B002-DYN-pos-backend", positive_backend),
        ("M265-B002-DYN-pos-diag", positive_diag),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, f"missing artifact: {display_path(path)}", failures)
    checks_total += 1
    checks_passed += require(exe_path is not None, display_path(positive_out), "M265-B002-DYN-pos-link", "expected linked executable artifact", failures)
    positive_packet: dict[str, Any] = {}
    positive_case: dict[str, Any] = {
        "fixture": display_path(POSITIVE_FIXTURE),
        "out_dir": display_path(positive_out),
        "returncode": positive_run.returncode,
        "manifest_exists": positive_manifest.exists(),
        "ir_exists": positive_ir.exists(),
        "object_exists": positive_obj.exists(),
        "backend_exists": positive_backend.exists(),
        "executable_path": display_path(exe_path) if exe_path is not None else None,
    }
    if link_result is not None:
        positive_case["link_returncode"] = link_result.returncode
    if positive_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(positive_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if positive_backend.exists():
        backend_text = read_text(positive_backend).strip()
        positive_case["object_backend"] = backend_text
        checks_total += 1
        checks_passed += require(backend_text == "llvm-direct", display_path(positive_backend), "M265-B002-DYN-pos-backend-value", f"expected llvm-direct backend, got {backend_text!r}", failures)
    if positive_manifest.exists():
        positive_packet = manifest_semantic_packet(positive_manifest)
        sub_total, sub_passed = validate_packet(positive_packet, display_path(positive_manifest), failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if positive_ir.exists():
        ir_text = read_text(positive_ir)
        checks_total += 1
        checks_passed += require("coalesce_merge_" in ir_text, display_path(positive_ir), "M265-B002-DYN-pos-ir-coalesce", "expected nil-coalescing lowering labels in IR", failures)
        checks_total += 1
        checks_passed += require("if_bind_success_" in ir_text or "guard_success_" in ir_text, display_path(positive_ir), "M265-B002-DYN-pos-ir-bind", "expected optional-binding lowering labels in IR", failures)
        checks_total += 1
        checks_passed += require("unsupported binary operator '??'" not in ir_text, display_path(positive_ir), "M265-B002-DYN-pos-ir-no-unsupported", "IR still contains unsupported nil-coalescing fallback", failures)
    if exe_path is not None:
        run_result = run_command([str(exe_path)], cwd=positive_out)
        positive_case["program_exit_code"] = run_result.returncode
        positive_case["expected_exit_code"] = 36
        checks_total += 1
        checks_passed += require(run_result.returncode == 36, display_path(exe_path), "M265-B002-DYN-pos-exit", f"expected exit 36, got {run_result.returncode}", failures)
    positive_case["semantic_packet"] = positive_packet

    negative_specs = [
        (
            "guard-missing-exit",
            NEG_GUARD,
            "type mismatch: guard binding else block must exit the current scope",
        ),
        (
            "ordinary-send-nullable-receiver",
            NEG_ORDINARY_SEND,
            "type mismatch: ordinary send receiver for selector 'description' must be proven nonnull or use optional send syntax",
        ),
    ]
    negative_cases: list[dict[str, Any]] = []
    for case_id, fixture, expected_message in negative_specs:
        out_dir = PROBE_ROOT / case_id
        completed = run_source_only_case(args.runner_exe, fixture, out_dir)
        diag_path = out_dir / "module.diagnostics.json"
        case_payload = {
            "case_id": case_id,
            "fixture": display_path(fixture),
            "diagnostics_path": display_path(diag_path),
            "returncode": completed.returncode,
            "expected_message": expected_message,
        }
        checks_total += 1
        checks_passed += require(completed.returncode != 0, display_path(fixture), f"M265-B002-DYN-neg-{case_id}-rc", "negative case unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(diag_path.exists(), display_path(diag_path), f"M265-B002-DYN-neg-{case_id}-diag", "negative diagnostics missing", failures)
        if diag_path.exists():
            payload = load_json(diag_path)
            raw_diags = payload.get("diagnostics")
            messages = [str(item.get("message", "")) for item in raw_diags if isinstance(item, dict)] if isinstance(raw_diags, list) else []
            checks_total += 1
            checks_passed += require(any(expected_message in message for message in messages), display_path(diag_path), f"M265-B002-DYN-neg-{case_id}-msg", f"expected diagnostic missing: {expected_message}", failures)
            case_payload["messages"] = messages
        negative_cases.append(case_payload)

    dynamic = {
        "positive": positive_case,
        "negative_cases": negative_cases,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_targets: list[tuple[Path, list[SnippetCheck]]] = [
        (EXPECTATIONS_DOC, [
            SnippetCheck("M265-B002-DOC-01", CONTRACT_ID),
            SnippetCheck("M265-B002-DOC-02", "guard let"),
            SnippetCheck("M265-B002-DOC-03", "ordinary-send diagnostics"),
        ]),
        (PACKET_DOC, [
            SnippetCheck("M265-B002-PKT-01", "Packet: `M265-B002`"),
            SnippetCheck("M265-B002-PKT-02", CONTRACT_ID),
            SnippetCheck("M265-B002-PKT-03", "optional-flow"),
        ]),
        (DOC_SOURCE, [
            SnippetCheck("M265-B002-DOCSRC-01", "ordinary sends now fail closed for nullable receivers"),
            SnippetCheck("M265-B002-DOCSRC-02", "guard let` / `guard var` `else` blocks now fail closed unless they exit the"),
            SnippetCheck("M265-B002-DOCSRC-03", "nil-coalescing `??` now lowers as a real short-circuit path in native IR"),
        ]),
        (DOC_NATIVE, [
            SnippetCheck("M265-B002-DOCNATIVE-01", "ordinary sends now fail closed for nullable receivers"),
            SnippetCheck("M265-B002-DOCNATIVE-02", "guard let` / `guard var` `else` blocks now fail closed unless they exit the"),
        ]),
        (SPEC_AM, [
            SnippetCheck("M265-B002-AM-01", "Lane B now carries live optional-flow semantics"),
            SnippetCheck("M265-B002-AM-02", "Optional sends fail closed for non-ObjC-reference receivers, ordinary sends"),
        ]),
        (SPEC_ATTR, [
            SnippetCheck("M265-B002-ATTR-01", "Current implementation status (`M265-B002`)"),
            SnippetCheck("M265-B002-ATTR-02", "guard let` / `guard var` `else` blocks now fail closed unless they exit the"),
        ]),
        (SPEC_PART3, [
            SnippetCheck("M265-B002-PART3-01", "Implementation note (`M265-B002`)"),
            SnippetCheck("M265-B002-PART3-02", "Nil-coalescing `??` now lowers as a real short-circuit path."),
        ]),
        (SEMA_CONTRACT_H, [
            SnippetCheck("M265-B002-SEMAH-01", "optional_propagation_sites"),
            SnippetCheck("M265-B002-SEMAH-02", "optional_flow_refinement_sites"),
            SnippetCheck("M265-B002-SEMAH-03", "guard_binding_exit_enforcement_sites"),
        ]),
        (SEMA_PASSES_CPP, [
            SnippetCheck("M265-B002-SEMACPP-01", "guard binding else block must exit the current scope"),
            SnippetCheck("M265-B002-SEMACPP-02", "ordinary send receiver for selector '"),
            SnippetCheck("M265-B002-SEMACPP-03", "AnalyzeNonnullBranchRefinement"),
        ]),
        (IR_CPP, [
            SnippetCheck("M265-B002-IR-01", "EmitOptionalBindingIfStatement("),
            SnippetCheck("M265-B002-IR-02", "expr->op == \"??\""),
            SnippetCheck("M265-B002-IR-03", "coalesce_merge_"),
        ]),
        (FRONTEND_ARTIFACTS_CPP, [
            SnippetCheck("M265-B002-ART-01", "optional_propagation_sites"),
            SnippetCheck("M265-B002-ART-02", "optional_flow_refinement_sites"),
            SnippetCheck("M265-B002-ART-03", "guard_binding_exit_enforcement_sites"),
        ]),
        (PACKAGE_JSON, [
            SnippetCheck("M265-B002-PKG-01", "check:objc3c:m265-b002-optional-flow-binding-and-refinement-semantics-core-feature-implementation"),
            SnippetCheck("M265-B002-PKG-02", "test:tooling:m265-b002-optional-flow-binding-and-refinement-semantics-core-feature-implementation"),
            SnippetCheck("M265-B002-PKG-03", "check:objc3c:m265-b002-lane-b-readiness"),
        ]),
    ]
    for path, snippets in snippet_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_payload["skipped"] = True
    else:
        dynamic_total, dynamic_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(f"[ok] M265-B002 optional flow semantics verified -> {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
