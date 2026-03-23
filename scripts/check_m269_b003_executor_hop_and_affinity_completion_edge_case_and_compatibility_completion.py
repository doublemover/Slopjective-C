#!/usr/bin/env python3
"""Checker for M269-B003 executor-hop and affinity compatibility completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-b003-part7-executor-hop-affinity-compatibility-v1"
CONTRACT_ID = "objc3c-part7-executor-hop-affinity-compatibility/m269-b003-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m269" / "M269-B003" / "executor_hop_affinity_compatibility_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_executor_hop_and_affinity_completion_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_b003_executor_hop_and_affinity_completion_edge_case_and_compatibility_completion_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_executor_hop_affinity_positive.objc3"
NEGATIVE_NO_AFFINITY = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_b003_missing_executor_affinity_rejected.objc3"
NEGATIVE_MAIN_DETACHED = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_b003_main_executor_detached_rejected.objc3"

EXPECTED_POSITIVE = {
    "contract_id": CONTRACT_ID,
    "dependency_contract_id": "objc3c-part7-structured-task-cancellation-semantics/m269-b002-v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion",
    "async_callable_sites": 2,
    "executor_affinity_sites": 2,
    "executor_main_sites": 0,
    "executor_global_sites": 1,
    "executor_named_sites": 1,
    "task_creation_sites": 2,
    "detached_task_creation_sites": 1,
    "illegal_missing_executor_affinity_sites": 0,
    "illegal_main_executor_detached_sites": 0,
}

EXPECTED_BOOLEAN_FIELDS = [
    "dependency_required",
    "executor_affinity_required_for_task_callables_enforced",
    "detached_task_hop_boundary_enforced",
    "runnable_lowering_deferred",
    "executor_runtime_deferred",
    "scheduler_runtime_deferred",
    "deterministic",
    "ready_for_lowering_and_runtime",
]

NEGATIVE_CASES = [
    (NEGATIVE_NO_AFFINITY, "o3s231", "O3S231", "objc_executor affinity"),
    (NEGATIVE_MAIN_DETACHED, "o3s232", "O3S232", "main executor affinity"),
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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M269-B003-MISSING", f"missing artifact: {display_path(path)}"))
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


def surface_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_executor_hop_and_affinity_compatibility_completion"]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_POSITIVE.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M269-B003-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_BOOLEAN_FIELDS, start=30):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M269-B003-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M269-B003-PAYLOAD-50", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M269-B003-PAYLOAD-51", "replay key missing", failures)
    return total, passed


def run_negative_case(case_index: int, fixture: Path, label: str, code: str, phrase: str, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "b003" / label
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    total += 1
    passed += require(run.returncode != 0, display_path(fixture), f"M269-B003-DYN-{case_index:02d}", "negative fixture unexpectedly succeeded", failures)
    summary_path = out_dir / "module.c_api_summary.json"
    total += 1
    passed += require(summary_path.exists(), display_path(summary_path), f"M269-B003-DYN-{case_index + 1:02d}", "negative summary missing", failures)
    details: dict[str, Any] = {
        "fixture": display_path(fixture),
        "returncode": run.returncode,
        "output": output.strip(),
        "summary": display_path(summary_path),
    }
    if summary_path.exists():
        payload = load_json(summary_path)
        last_error = payload.get("last_error", "")
        total += 1
        passed += require(code in last_error, display_path(summary_path), f"M269-B003-DYN-{case_index + 2:02d}", f"missing {code}", failures)
        total += 1
        passed += require(phrase in last_error, display_path(summary_path), f"M269-B003-DYN-{case_index + 3:02d}", f"missing wording: {phrase}", failures)
        details["last_error"] = last_error
    return total, passed, details


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m269-b003-readiness",
        "--summary-out",
        "tmp/reports/m269/M269-B003/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M269-B003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M269-B003-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "b003" / "positive"
    positive_out_dir.mkdir(parents=True, exist_ok=True)
    positive_run = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    positive_output = (positive_run.stdout or "") + (positive_run.stderr or "")
    total += 1
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M269-B003-DYN-03", f"positive fixture failed: {positive_output}", failures)
    positive_manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M269-B003-DYN-04", "positive manifest missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive_run.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(positive_manifest_path),
    }
    if positive_manifest_path.exists():
        payload = surface_payload(positive_manifest_path)
        sub_total, sub_passed = validate_positive_payload(payload, display_path(positive_manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["part7_executor_hop_and_affinity_compatibility_completion"] = payload

    next_case = 5
    negative_results: dict[str, Any] = {}
    for fixture, label, code, phrase in NEGATIVE_CASES:
        case_total, case_passed, details = run_negative_case(next_case, fixture, label, code, phrase, args, failures)
        total += case_total
        passed += case_passed
        negative_results[label] = details
        next_case += 4
    dynamic["negative_cases"] = negative_results
    return total, passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M269-B003-EXP-01", "# M269 Executor-Hop And Affinity Completion Edge-Case And Compatibility Completion Expectations (B003)"),
            SnippetCheck("M269-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M269-B003-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion"),
            SnippetCheck("M269-B003-EXP-04", "O3S231"),
            SnippetCheck("M269-B003-EXP-05", "O3S232"),
        ],
        PACKET_DOC: [
            SnippetCheck("M269-B003-PKT-01", "# M269-B003 Packet: Executor-Hop And Affinity Completion - Edge-Case And Compatibility Completion"),
            SnippetCheck("M269-B003-PKT-02", "reject async task callables that omit `objc_executor(...)` affinity"),
            SnippetCheck("M269-B003-PKT-03", "O3S231"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M269-B003-GRM-01", "## M269 executor hop and affinity compatibility completion"),
            SnippetCheck("M269-B003-GRM-02", "objc_part7_executor_hop_and_affinity_compatibility_completion"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M269-B003-DOC-01", "## M269 executor hop and affinity compatibility completion"),
            SnippetCheck("M269-B003-DOC-02", "O3S231"),
            SnippetCheck("M269-B003-DOC-03", "O3S232"),
        ],
        SPEC_AM: [
            SnippetCheck("M269-B003-AM-01", "M269-B003 executor-hop/affinity compatibility note:"),
            SnippetCheck("M269-B003-AM-02", "objc_part7_executor_hop_and_affinity_compatibility_completion"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M269-B003-ATTR-01", "Current implementation status (`M269-B003`):"),
            SnippetCheck("M269-B003-ATTR-02", "diagnostic `O3S231`"),
            SnippetCheck("M269-B003-ATTR-03", "diagnostic `O3S232`"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M269-B003-SEMA-01", "kObjc3Part7ExecutorHopAffinityCompatibilitySummaryContractId"),
            SnippetCheck("M269-B003-SEMA-02", "struct Objc3Part7ExecutorHopAffinityCompatibilitySummary"),
            SnippetCheck("M269-B003-SEMA-03", "IsReadyObjc3Part7ExecutorHopAffinityCompatibilitySummary("),
        ],
        SEMA_PASSES_H: [
            SnippetCheck("M269-B003-SEMAH-01", "BuildPart7ExecutorHopAffinityCompatibilitySummary("),
        ],
        SEMA_PASSES_CPP: [
            SnippetCheck("M269-B003-SEMACPP-01", "BuildPart7ExecutorHopAffinityCompatibilitySummary("),
            SnippetCheck("M269-B003-SEMACPP-02", "DiagnosePart7ExecutorAffinityCompletion("),
            SnippetCheck("M269-B003-SEMACPP-03", '"O3S231"'),
            SnippetCheck("M269-B003-SEMACPP-04", '"O3S232"'),
        ],
        FRONTEND_TYPES: [SnippetCheck("M269-B003-TYP-01", "part7_executor_hop_affinity_compatibility_summary")],
        FRONTEND_PIPELINE: [SnippetCheck("M269-B003-PIPE-01", "result.part7_executor_hop_affinity_compatibility_summary =")],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M269-B003-ART-01", "BuildPart7ExecutorHopAffinityCompatibilitySummaryJson("),
            SnippetCheck("M269-B003-ART-02", "objc_part7_executor_hop_and_affinity_compatibility_completion"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M269-B003-PKG-01", '"check:objc3c:m269-b003-executor-hop-and-affinity-completion-edge-case-and-compatibility-completion"'),
            SnippetCheck("M269-B003-PKG-02", '"check:objc3c:m269-b003-lane-b-readiness"'),
        ],
        NEGATIVE_NO_AFFINITY: [SnippetCheck("M269-B003-FIX-01", "task_spawn_child();")],
        NEGATIVE_MAIN_DETACHED: [SnippetCheck("M269-B003-FIX-02", "detached_task_create();")],
    }
    for path, required in snippets.items():
        total += len(required)
        passed += ensure_snippets(path, required, failures)

    dynamic = {}
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
    print(f"[ok] M269-B003 executor-hop/affinity compatibility checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
