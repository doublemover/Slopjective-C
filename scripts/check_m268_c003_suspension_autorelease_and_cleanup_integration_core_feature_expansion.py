#!/usr/bin/env python3
"""Checker for M268-C003 async cleanup integration."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-c003-part7-suspension-autorelease-cleanup-integration-v1"
CONTRACT_ID = "objc3c-part7-suspension-autorelease-cleanup-integration/m268-c003-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m268" / "M268-C003" / "async_cleanup_integration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_suspension_autorelease_and_cleanup_integration_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_c003_suspension_autorelease_and_cleanup_integration_core_feature_expansion_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
COMPILER_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_c003_async_cleanup_integration_positive.objc3"

EXPECTED_PACKET = {
    "contract_id": CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration",
    "async_lowering_contract_id": "objc3c-part7-async-direct-call-lowering/m268-c002-v1",
    "control_flow_contract_id": "objc3c-part5-control-flow-safety-lowering/m266-c001-v1",
    "autoreleasepool_lane_contract_id": "m163-autoreleasepool-scope-lowering-v1",
    "continuation_lane_contract_id": "m186-async-continuation-lowering-v1",
    "await_lane_contract_id": "m187-await-lowering-suspension-state-lowering-v1",
    "defer_statement_sites": 2,
    "live_defer_cleanup_sites": 2,
    "continuation_allocation_sites": 0,
    "continuation_resume_sites": 0,
    "continuation_suspend_sites": 0,
    "await_resume_sites": 0,
    "await_state_machine_sites": 0,
    "await_continuation_sites": 0,
}

FUNCTION_PATTERNS = {
    "runTask": re.compile(
        r"define i32 @runTask\(\) \{\s*entry:\s*"
        r"call void @objc3_runtime_push_autoreleasepool_scope\(\)\s*"
        r"%t\d+ = call i32 @fetchValue\(\)\s*"
        r"%t\d+ = call i32 @objc3_runtime_allocate_async_continuation_i32\(i32 \d+, i32 \d+\)\s*"
        r"%t\d+ = call i32 @objc3_runtime_handoff_async_continuation_to_executor_i32\(i32 %t\d+, i32 \d+\)\s*"
        r"%t\d+ = call i32 @objc3_runtime_resume_async_continuation_i32\(i32 %t\d+, i32 %t\d+\)\s*"
        r"call void @objc3_runtime_pop_autoreleasepool_scope\(\)\s*"
        r"%t\d+ = call i32 @fetchValue\(\)\s*"
        r"ret i32 %t\d+\s*\}",
        re.MULTILINE,
    ),
    "loadValue": re.compile(
        r"define i32 @objc3_method_Loader_instance_loadValue\(\) \{\s*entry:\s*"
        r"call void @objc3_runtime_push_autoreleasepool_scope\(\)\s*"
        r"%t\d+ = call i32 @fetchValue\(\)\s*"
        r"%t\d+ = call i32 @objc3_runtime_allocate_async_continuation_i32\(i32 \d+, i32 \d+\)\s*"
        r"%t\d+ = call i32 @objc3_runtime_handoff_async_continuation_to_executor_i32\(i32 %t\d+, i32 \d+\)\s*"
        r"%t\d+ = call i32 @objc3_runtime_resume_async_continuation_i32\(i32 %t\d+, i32 %t\d+\)\s*"
        r"call void @objc3_runtime_pop_autoreleasepool_scope\(\)\s*"
        r"%t\d+ = call i32 @fetchValue\(\)\s*"
        r"ret i32 %t\d+\s*\}",
        re.MULTILINE,
    ),
}

IR_SNIPPETS = [
    "; autoreleasepool_scope_lowering = ",
    "; part5_control_flow_safety_lowering = ",
    "; async_continuation_lowering = async_continuation_sites=5",
    "; await_lowering_suspension_state_lowering = await_suspension_sites=3",
    "declare void @objc3_runtime_push_autoreleasepool_scope()",
    "declare void @objc3_runtime_pop_autoreleasepool_scope()",
]


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
    parser.add_argument("--compiler-exe", type=Path, default=COMPILER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M268-C003-MISSING", f"missing artifact: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def packet_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_suspension_autorelease_and_cleanup_integration"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_PACKET.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M268-C003-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate([
        "autoreleasepool_scope_supported",
        "defer_cleanup_supported",
        "direct_call_lowering_supported",
        "deterministic",
        "ready_for_ir_object_emission",
    ], start=30):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M268-C003-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    for index, field in enumerate(["suspension_runtime_required", "state_machine_emission_present"], start=40):
        total += 1
        passed += require(payload.get(field) is False, artifact, f"M268-C003-PAYLOAD-{index:02d}", f"{field} must stay false", failures)
    for index, field in enumerate(["control_flow_replay_key", "autoreleasepool_replay_key", "continuation_replay_key", "await_replay_key"], start=50):
        total += 1
        passed += require(bool(payload.get(field)), artifact, f"M268-C003-PAYLOAD-{index:02d}", f"{field} missing", failures)
    return total, passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m268-c003-readiness",
        "--summary-out",
        "tmp/reports/m268/M268-C003/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M268-C003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.compiler_exe.exists(), display_path(args.compiler_exe), "M268-C003-DYN-02", "native compiler missing after build", failures)
    if ensure_build.returncode != 0 or not args.compiler_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "c003" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.compiler_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M268-C003-DYN-03", f"positive fixture failed: {output}", failures)
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M268-C003-DYN-04", "manifest missing", failures)
    total += 1
    passed += require(ir_path.exists(), display_path(ir_path), "M268-C003-DYN-05", "IR missing", failures)
    total += 1
    passed += require(obj_path.exists(), display_path(obj_path), "M268-C003-DYN-06", "object missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "manifest": display_path(manifest_path),
        "ir": display_path(ir_path),
        "object": display_path(obj_path),
    }
    if manifest_path.exists():
        surface = packet_payload(manifest_path)
        sub_total, sub_passed = validate_payload(surface, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["async_cleanup_integration"] = surface
    if ir_path.exists():
        ir_text = ir_path.read_text(encoding="utf-8")
        for index, snippet in enumerate(IR_SNIPPETS, start=20):
            total += 1
            passed += require(snippet in ir_text, display_path(ir_path), f"M268-C003-DYN-{index:02d}", f"IR missing snippet: {snippet}", failures)
        for index, (name, pattern) in enumerate(FUNCTION_PATTERNS.items(), start=40):
            total += 1
            passed += require(bool(pattern.search(ir_text)), display_path(ir_path), f"M268-C003-DYN-{index:02d}", f"IR missing expected cleanup ordering for {name}", failures)
    return total, passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M268-C003-EXP-01", "# M268 Suspension Autorelease And Cleanup Integration Core Feature Expansion Expectations (C003)"),
            SnippetCheck("M268-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M268-C003-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration"),
        ],
        PACKET_DOC: [
            SnippetCheck("M268-C003-PKT-01", "# M268-C003 Packet: Suspension Autorelease And Cleanup Integration - Core Feature Expansion"),
            SnippetCheck("M268-C003-PKT-02", "the current supported non-suspending async slice composes with existing autoreleasepool scope lowering and defer-cleanup lowering"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M268-C003-GRM-01", "## M268 async cleanup integration slice"),
            SnippetCheck("M268-C003-GRM-02", "frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M268-C003-DOC-01", "## M268 async cleanup integration slice"),
            SnippetCheck("M268-C003-DOC-02", "frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration"),
        ],
        SPEC_AM: [
            SnippetCheck("M268-C003-AM-01", "M268-C003 lowering note:"),
            SnippetCheck("M268-C003-AM-02", "autoreleasepool scopes lower through the live push/pop hooks"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M268-C003-ATTR-01", "Current implementation status (`M268-C003`):"),
            SnippetCheck("M268-C003-ATTR-02", "frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration"),
        ],
        LOWERING_CONTRACT: [
            SnippetCheck("M268-C003-LWR-01", "M268-C003 integration anchor:"),
            SnippetCheck("M268-C003-LWR-02", "existing autoreleasepool-scope and defer-cleanup lowering"),
        ],
        IR_EMITTER: [
            SnippetCheck("M268-C003-IR-01", "M268-C003 integration anchor:"),
            SnippetCheck("M268-C003-IR-02", "reuses the existing autoreleasepool scope hooks"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M268-C003-ART-01", "kObjc3Part7SuspensionCleanupIntegrationContractId"),
            SnippetCheck("M268-C003-ART-02", "objc_part7_suspension_autorelease_and_cleanup_integration"),
            SnippetCheck("M268-C003-ART-03", "BuildPart7SuspensionCleanupIntegrationJson("),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M268-C003-PKG-01", '"check:objc3c:m268-c003-suspension-autorelease-and-cleanup-integration-core-feature-expansion"'),
            SnippetCheck("M268-C003-PKG-02", '"check:objc3c:m268-c003-lane-c-readiness"'),
        ],
    }
    for path, required in snippets.items():
        total += len(required)
        passed += ensure_snippets(path, required, failures)

    dynamic: dict[str, Any] = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": total,
        "checks_passed": passed,
        "checks_failed": len(failures),
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}")
        print(f"[info] wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"[ok] M268-C003 async cleanup integration checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
