#!/usr/bin/env python3
"""Checker for M269-B002 structured-task and cancellation legality semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-b002-part7-structured-task-cancellation-semantics-v1"
CONTRACT_ID = "objc3c-part7-structured-task-cancellation-semantics/m269-b002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m269" / "M269-B002" / "structured_task_cancellation_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_structured_task_and_cancellation_legality_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_b002_structured_task_and_cancellation_legality_semantics_core_feature_implementation_packet.md"
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
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_structured_task_cancellation_semantics_positive.objc3"
NEGATIVE_NON_ASYNC = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_b002_non_async_task_runtime_rejected.objc3"
NEGATIVE_NO_SCOPE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_b002_task_group_without_scope_rejected.objc3"
NEGATIVE_HIERARCHY = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_b002_detached_and_group_hierarchy_rejected.objc3"
NEGATIVE_CANCELLATION = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_b002_cancellation_without_check_rejected.objc3"

EXPECTED_POSITIVE = {
    "contract_id": CONTRACT_ID,
    "dependency_contract_id": "objc3c-part7-task-executor-cancellation-semantic-model/m269-b001-v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics",
    "async_callable_sites": 2,
    "task_creation_sites": 2,
    "task_group_scope_sites": 1,
    "task_group_add_task_sites": 1,
    "task_group_wait_next_sites": 1,
    "task_group_cancel_all_sites": 1,
    "cancellation_check_sites": 4,
    "cancellation_handler_sites": 1,
    "illegal_non_async_task_sites": 0,
    "illegal_task_group_scope_sites": 0,
    "illegal_task_hierarchy_sites": 0,
    "illegal_cancellation_usage_sites": 0,
}

EXPECTED_BOOLEAN_FIELDS = [
    "source_dependency_required",
    "async_task_boundary_enforced",
    "structured_task_scope_enforced",
    "task_hierarchy_enforced",
    "cancellation_usage_enforced",
    "runnable_lowering_deferred",
    "executor_runtime_deferred",
    "scheduler_runtime_deferred",
    "deterministic",
    "ready_for_lowering_and_runtime",
]

NEGATIVE_CASES = [
    (NEGATIVE_NON_ASYNC, "o3s227", "O3S227", "async function or method"),
    (NEGATIVE_NO_SCOPE, "o3s228", "O3S228", "task-group scope"),
    (NEGATIVE_HIERARCHY, "o3s229", "O3S229", "detached task creation"),
    (NEGATIVE_CANCELLATION, "o3s230", "O3S230", "cancellation check"),
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
        failures.append(Finding(display_path(path), "M269-B002-MISSING", f"missing artifact: {display_path(path)}"))
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
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_structured_task_and_cancellation_semantics"]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_POSITIVE.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M269-B002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_BOOLEAN_FIELDS, start=30):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M269-B002-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M269-B002-PAYLOAD-50", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M269-B002-PAYLOAD-51", "replay key missing", failures)
    return total, passed


def run_negative_case(case_index: int, fixture: Path, label: str, code: str, phrase: str, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "b002" / label
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
    passed += require(run.returncode != 0, display_path(fixture), f"M269-B002-DYN-{case_index:02d}", "negative fixture unexpectedly succeeded", failures)
    summary_path = out_dir / "module.c_api_summary.json"
    total += 1
    passed += require(summary_path.exists(), display_path(summary_path), f"M269-B002-DYN-{case_index + 1:02d}", "negative summary missing", failures)
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
        passed += require(code in last_error, display_path(summary_path), f"M269-B002-DYN-{case_index + 2:02d}", f"missing {code}", failures)
        total += 1
        passed += require(phrase in last_error, display_path(summary_path), f"M269-B002-DYN-{case_index + 3:02d}", f"missing wording: {phrase}", failures)
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
        "m269-b002-readiness",
        "--summary-out",
        "tmp/reports/m269/M269-B002/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M269-B002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M269-B002-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "b002" / "positive"
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
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M269-B002-DYN-03", f"positive fixture failed: {positive_output}", failures)
    positive_manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M269-B002-DYN-04", "positive manifest missing", failures)

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
        dynamic["part7_structured_task_and_cancellation_semantics"] = payload

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
            SnippetCheck("M269-B002-EXP-01", "# M269 Structured-Task And Cancellation Legality Semantics Core Feature Implementation Expectations (B002)"),
            SnippetCheck("M269-B002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M269-B002-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics"),
            SnippetCheck("M269-B002-EXP-04", "O3S227"),
            SnippetCheck("M269-B002-EXP-05", "O3S230"),
        ],
        PACKET_DOC: [
            SnippetCheck("M269-B002-PKT-01", "# M269-B002 Packet: Structured-Task And Cancellation Legality Semantics - Core Feature Implementation"),
            SnippetCheck("M269-B002-PKT-02", "reject task-runtime, task-group, and cancellation calls outside async functions and methods"),
            SnippetCheck("M269-B002-PKT-03", "O3S227`, `O3S228`, `O3S229`, and `O3S230`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M269-B002-GRM-01", "## M269 structured task and cancellation semantics"),
            SnippetCheck("M269-B002-GRM-02", "objc_part7_structured_task_and_cancellation_semantics"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M269-B002-DOC-01", "## M269 structured task and cancellation semantics"),
            SnippetCheck("M269-B002-DOC-02", "O3S227"),
            SnippetCheck("M269-B002-DOC-03", "O3S230"),
        ],
        SPEC_AM: [
            SnippetCheck("M269-B002-AM-01", "M269-B002 structured-task/cancellation enforcement note:"),
            SnippetCheck("M269-B002-AM-02", "objc_part7_structured_task_and_cancellation_semantics"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M269-B002-ATTR-01", "Current implementation status (`M269-B002`):"),
            SnippetCheck("M269-B002-ATTR-02", "diagnostic `O3S227`"),
            SnippetCheck("M269-B002-ATTR-03", "diagnostic `O3S230`"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M269-B002-SEMA-01", "kObjc3Part7StructuredTaskCancellationSemanticSummaryContractId"),
            SnippetCheck("M269-B002-SEMA-02", "struct Objc3Part7StructuredTaskCancellationSemanticSummary"),
            SnippetCheck("M269-B002-SEMA-03", "IsReadyObjc3Part7StructuredTaskCancellationSemanticSummary("),
        ],
        SEMA_PASSES_H: [
            SnippetCheck("M269-B002-SEMAH-01", "BuildPart7StructuredTaskCancellationSemanticSummary("),
        ],
        SEMA_PASSES_CPP: [
            SnippetCheck("M269-B002-SEMACPP-01", "BuildPart7StructuredTaskCancellationSemanticSummary("),
            SnippetCheck("M269-B002-SEMACPP-02", "DiagnosePart7TaskCallableLegality("),
            SnippetCheck("M269-B002-SEMACPP-03", 'count_diagnostic_code("O3S227")'),
            SnippetCheck("M269-B002-SEMACPP-04", '"O3S230"'),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M269-B002-TYP-01", "part7_structured_task_cancellation_semantic_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M269-B002-PIPE-01", "result.part7_structured_task_cancellation_semantic_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M269-B002-ART-01", "BuildPart7StructuredTaskCancellationSemanticSummaryJson("),
            SnippetCheck("M269-B002-ART-02", "objc_part7_structured_task_and_cancellation_semantics"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M269-B002-PKG-01", '"check:objc3c:m269-b002-structured-task-and-cancellation-legality-semantics-core-feature-implementation"'),
            SnippetCheck("M269-B002-PKG-02", '"check:objc3c:m269-b002-lane-b-readiness"'),
        ],
        NEGATIVE_NON_ASYNC: [SnippetCheck("M269-B002-FIX-01", "task_spawn_child();")],
        NEGATIVE_NO_SCOPE: [SnippetCheck("M269-B002-FIX-02", "task_group_add_task();")],
        NEGATIVE_HIERARCHY: [SnippetCheck("M269-B002-FIX-03", "detached_task_create();")],
        NEGATIVE_CANCELLATION: [SnippetCheck("M269-B002-FIX-04", "task_runtime_on_cancel();")],
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
    print(f"[ok] M269-B002 structured-task/cancellation semantics checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
