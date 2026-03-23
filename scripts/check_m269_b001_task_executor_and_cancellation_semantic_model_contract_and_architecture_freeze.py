#!/usr/bin/env python3
"""Checker for M269-B001 task/executor/cancellation semantic model."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-b001-part7-task-executor-cancellation-semantic-model-v1"
CONTRACT_ID = "objc3c-part7-task-executor-cancellation-semantic-model/m269-b001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m269" / "M269-B001" / "task_executor_cancellation_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_task_executor_and_cancellation_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_b001_task_executor_and_cancellation_semantic_model_contract_and_architecture_freeze_packet.md"
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
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_task_executor_cancellation_semantic_model_positive.objc3"

EXPECTED_COUNTS = {
    "async_callable_sites": 2,
    "executor_attribute_sites": 2,
    "task_creation_sites": 2,
    "task_group_scope_sites": 1,
    "task_group_add_task_sites": 1,
    "task_group_wait_next_sites": 1,
    "task_group_cancel_all_sites": 1,
    "task_runtime_interop_sites": 14,
    "runtime_hook_sites": 9,
    "cancellation_check_sites": 4,
    "cancellation_handler_sites": 1,
    "suspension_point_sites": 1,
    "cancellation_propagation_sites": 1,
}


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
        failures.append(Finding(display_path(path), "M269-B001-MISSING", f"missing artifact: {display_path(path)}"))
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
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_task_executor_and_cancellation_semantic_model"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    exact = {
        "contract_id": CONTRACT_ID,
        "frontend_dependency_contract_id": "objc3c-part7-task-group-cancellation-source-closure/m269-a002-v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_part7_task_executor_and_cancellation_semantic_model",
        **EXPECTED_COUNTS,
    }
    for index, (field, expected) in enumerate(exact.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M269-B001-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(
        [
            "source_dependency_required",
            "task_lifetime_semantics_landed",
            "executor_affinity_semantics_landed",
            "cancellation_observation_semantics_landed",
            "structured_task_legality_semantics_landed",
            "runnable_lowering_deferred",
            "executor_runtime_deferred",
            "scheduler_runtime_deferred",
            "deterministic",
            "ready_for_lowering_and_runtime",
        ],
        start=40,
    ):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M269-B001-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M269-B001-PAYLOAD-60", "replay key missing", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M269-B001-PAYLOAD-61", "failure_reason must stay empty", failures)
    return total, passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command(
        [
            sys.executable,
            "scripts/ensure_objc3c_native_build.py",
            "--mode",
            "fast",
            "--reason",
            "m269-b001-readiness",
            "--summary-out",
            "tmp/reports/m269/M269-B001/ensure_build_summary.json",
        ]
    )
    total += 1
    passed += require(
        ensure_build.returncode == 0,
        "ensure_objc3c_native_build.py",
        "M269-B001-DYN-01",
        f"fast build failed: {ensure_build.stderr or ensure_build.stdout}",
        failures,
    )
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M269-B001-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "b001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command(
        [
            str(args.runner_exe),
            str(FIXTURE),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            "--no-emit-ir",
            "--no-emit-object",
        ]
    )
    output = (run.stdout or "") + (run.stderr or "")
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M269-B001-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M269-B001-DYN-04", "positive manifest missing", failures)
    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
    }
    if manifest_path.exists():
        payload = surface_payload(manifest_path)
        sub_total, sub_passed = validate_payload(payload, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["part7_task_executor_and_cancellation_semantic_model"] = payload
    return total, passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M269-B001-EXP-01", "# M269 Task Executor And Cancellation Semantic Model Contract And Architecture Freeze Expectations (B001)"),
            SnippetCheck("M269-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M269-B001-EXP-03", "The frontend must publish one deterministic semantic packet at `frontend.pipeline.semantic_surface.objc_part7_task_executor_and_cancellation_semantic_model`."),
        ],
        PACKET_DOC: [
            SnippetCheck("M269-B001-PKT-01", "# M269-B001 Packet: Task, Executor, And Cancellation Semantic Model - Contract And Architecture Freeze"),
            SnippetCheck("M269-B001-PKT-02", "consume the existing `M269-A002` task-group/cancellation source packet"),
            SnippetCheck("M269-B001-PKT-03", "semantic legality is live, runnable lowering and runtime behavior remain deferred"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M269-B001-GRM-01", "## M269 task executor and cancellation semantic model"),
            SnippetCheck("M269-B001-GRM-02", "objc_part7_task_executor_and_cancellation_semantic_model"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M269-B001-DOC-01", "## M269 task executor and cancellation semantic model"),
            SnippetCheck("M269-B001-DOC-02", "executor-affinity legality, cancellation observation legality, and structured"),
        ],
        SPEC_AM: [
            SnippetCheck("M269-B001-AM-01", "M269-B001 task/executor/cancellation semantic-model note:"),
            SnippetCheck("M269-B001-AM-02", "objc_part7_task_executor_and_cancellation_semantic_model"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M269-B001-ATTR-01", "Current implementation status (`M269-B001`):"),
            SnippetCheck("M269-B001-ATTR-02", "objc_part7_task_executor_and_cancellation_semantic_model"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M269-B001-SEMA-01", "kObjc3Part7TaskExecutorCancellationSemanticModelContractId"),
            SnippetCheck("M269-B001-SEMA-02", "struct Objc3Part7TaskExecutorCancellationSemanticModelSummary"),
            SnippetCheck("M269-B001-SEMA-03", "IsReadyObjc3Part7TaskExecutorCancellationSemanticModelSummary("),
        ],
        SEMA_PASSES_H: [
            SnippetCheck("M269-B001-SEMAH-01", "BuildPart7TaskExecutorCancellationSemanticModelSummary("),
        ],
        SEMA_PASSES_CPP: [
            SnippetCheck("M269-B001-SEMACPP-01", "BuildPart7TaskExecutorCancellationSemanticModelSummary("),
            SnippetCheck("M269-B001-SEMACPP-02", "summary.executor_affinity_semantics_landed ="),
            SnippetCheck("M269-B001-SEMACPP-03", "summary.structured_task_legality_semantics_landed ="),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M269-B001-TYP-01", "part7_task_executor_cancellation_semantic_model_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M269-B001-PIPE-01", "result.part7_task_executor_cancellation_semantic_model_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M269-B001-ART-01", "BuildPart7TaskExecutorCancellationSemanticModelSummaryJson("),
            SnippetCheck("M269-B001-ART-02", "objc_part7_task_executor_and_cancellation_semantic_model"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M269-B001-PKG-01", "\"check:objc3c:m269-b001-task-executor-and-cancellation-semantic-model-contract-and-architecture-freeze\""),
            SnippetCheck("M269-B001-PKG-02", "\"check:objc3c:m269-b001-lane-b-readiness\""),
        ],
        FIXTURE: [
            SnippetCheck("M269-B001-FIX-01", "task_spawn_child();"),
            SnippetCheck("M269-B001-FIX-02", "with_task_group_scope();"),
            SnippetCheck("M269-B001-FIX-03", "task_group_wait_next();"),
            SnippetCheck("M269-B001-FIX-04", "task_runtime_on_cancel();"),
        ],
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
    print(f"[ok] M269-B001 task/executor/cancellation semantic model checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
