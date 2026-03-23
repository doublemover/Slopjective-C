#!/usr/bin/env python3
"""Checker for M270-B002 actor isolation/sendability enforcement."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m270-b002-part7-actor-isolation-sendability-enforcement-v1"
CONTRACT_ID = "objc3c-part7-actor-isolation-sendability-enforcement/m270-b002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m270" / "M270-B002" / "actor_isolation_sendability_enforcement_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_actor_isolation_and_sendability_enforcement_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_b002_actor_isolation_and_sendability_enforcement_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_actor_isolation_sendability_enforcement_positive.objc3"
NEGATIVE_CASES = [
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_b002_non_actor_nonisolated_rejected.objc3", "non_actor_nonisolated", "O3S286", "objc_nonisolated is only valid on actor methods"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_b002_actor_nonisolated_async_rejected.objc3", "nonisolated_async", "O3S287", "cannot also be async"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_b002_actor_nonisolated_executor_rejected.objc3", "nonisolated_executor", "O3S288", "cannot declare objc_executor affinity"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_b002_actor_hop_without_async_rejected.objc3", "actor_hop_without_async", "O3S289", "actor hop sites require an async actor method"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_b002_non_sendable_crossing_rejected.objc3", "non_sendable_crossing", "O3S290", "non-sendable cross-actor crossings remain unsupported in actor methods"),
]

EXPECTED_POSITIVE = {
    "contract_id": CONTRACT_ID,
    "dependency_contract_id": "objc3c-part7-actor-isolation-sendable-semantic-model/m270-b001-v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendability_enforcement",
    "actor_interface_sites": 1,
    "actor_method_sites": 2,
    "objc_nonisolated_annotation_sites": 1,
    "actor_member_executor_annotation_sites": 1,
    "actor_async_method_sites": 1,
    "actor_hop_sites": 1,
    "non_sendable_crossing_sites": 0,
    "total_nonisolated_method_sites": 1,
    "illegal_non_actor_nonisolated_sites": 0,
    "illegal_nonisolated_async_sites": 0,
    "illegal_nonisolated_executor_sites": 0,
    "illegal_actor_hop_without_async_sites": 0,
    "illegal_non_sendable_crossing_sites": 0,
}

EXPECTED_BOOLEAN_FIELDS = [
    "dependency_required",
    "non_actor_nonisolated_fail_closed",
    "nonisolated_combination_fail_closed",
    "actor_hop_async_boundary_enforced",
    "non_sendable_crossing_fail_closed",
    "runnable_lowering_deferred",
    "actor_runtime_deferred",
    "executor_runtime_deferred",
    "deterministic",
    "ready_for_lowering_and_runtime",
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
        failures.append(Finding(display_path(path), "M270-B002-MISSING", f"missing artifact: {display_path(path)}"))
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
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_actor_isolation_and_sendability_enforcement"]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_POSITIVE.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M270-B002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_BOOLEAN_FIELDS, start=30):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M270-B002-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M270-B002-PAYLOAD-50", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M270-B002-PAYLOAD-51", "replay key missing", failures)
    return total, passed


def run_negative_case(case_index: int, fixture: Path, label: str, code: str, phrase: str, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "b002" / label
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
    passed += require(run.returncode != 0, display_path(fixture), f"M270-B002-DYN-{case_index:02d}", "negative fixture unexpectedly succeeded", failures)
    summary_path = out_dir / "module.c_api_summary.json"
    total += 1
    passed += require(summary_path.exists(), display_path(summary_path), f"M270-B002-DYN-{case_index + 1:02d}", "negative summary missing", failures)
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
        passed += require(code in last_error, display_path(summary_path), f"M270-B002-DYN-{case_index + 2:02d}", f"missing {code}", failures)
        total += 1
        passed += require(phrase in last_error, display_path(summary_path), f"M270-B002-DYN-{case_index + 3:02d}", f"missing wording: {phrase}", failures)
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
        "m270-b002-readiness",
        "--summary-out",
        "tmp/reports/m270/M270-B002/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M270-B002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M270-B002-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "b002" / "positive"
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
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M270-B002-DYN-03", f"positive fixture failed: {positive_output}", failures)
    positive_manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M270-B002-DYN-04", "positive manifest missing", failures)

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
        dynamic["objc_part7_actor_isolation_and_sendability_enforcement"] = payload

    next_case = 5
    negative_results = []
    for fixture, label, code, phrase in NEGATIVE_CASES:
        sub_total, sub_passed, details = run_negative_case(next_case, fixture, label, code, phrase, args, failures)
        total += sub_total
        passed += sub_passed
        negative_results.append(details)
        next_case += 4
    dynamic["negative_cases"] = negative_results
    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0

    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [SnippetCheck("M270-B002-DOC-01", CONTRACT_ID)],
        PACKET_DOC: [SnippetCheck("M270-B002-DOC-02", "M270-B002")],
        DOC_GRAMMAR: [SnippetCheck("M270-B002-DOC-03", "M270-B002 actor isolation/sendability enforcement")],
        DOC_NATIVE: [SnippetCheck("M270-B002-DOC-04", "M270-B002 actor isolation/sendability enforcement")],
        SPEC_AM: [SnippetCheck("M270-B002-DOC-05", "M270-B002 actor enforcement note")],
        SPEC_CHECKLIST: [SnippetCheck("M270-B002-DOC-06", "M270-B002 enforcement note")],
        SEMA_CONTRACT: [
            SnippetCheck("M270-B002-CODE-01", "kObjc3Part7ActorIsolationSendabilityEnforcementContractId"),
            SnippetCheck("M270-B002-CODE-02", "struct Objc3Part7ActorIsolationSendabilityEnforcementSummary"),
        ],
        SEMA_PASSES_H: [SnippetCheck("M270-B002-CODE-04", "BuildPart7ActorIsolationSendabilityEnforcementSummary")],
        SEMA_PASSES_CPP: [
            SnippetCheck("M270-B002-CODE-03", '"O3S286"'),
            SnippetCheck("M270-B002-CODE-05", "DiagnosePart7ActorMethodIsolationRules"),
            SnippetCheck("M270-B002-CODE-06", '"O3S290"'),
        ],
        FRONTEND_TYPES: [SnippetCheck("M270-B002-CODE-08", "part7_actor_isolation_sendability_enforcement_summary")],
        FRONTEND_PIPELINE: [SnippetCheck("M270-B002-CODE-09", "BuildPart7ActorIsolationSendabilityEnforcementSummary")],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M270-B002-CODE-10", "BuildPart7ActorIsolationSendabilityEnforcementSummaryJson"),
            SnippetCheck("M270-B002-CODE-11", "objc_part7_actor_isolation_and_sendability_enforcement"),
        ],
        PACKAGE_JSON: [SnippetCheck("M270-B002-CODE-12", "check:objc3c:m270-b002-actor-isolation-and-sendability-enforcement-core-feature-implementation")],
    }

    for path, checks in snippet_map.items():
        total += len(checks)
        passed += ensure_snippets(path, checks, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "total_checks": total,
        "passed_checks": passed,
        "failed_checks": total - passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M270-B002 checker passed ({passed}/{total} checks)")
        return 0
    print(json.dumps(payload, indent=2))
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
