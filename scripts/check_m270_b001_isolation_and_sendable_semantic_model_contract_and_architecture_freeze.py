#!/usr/bin/env python3
"""Checker for M270-B001 actor isolation/sendable semantic model."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m270-b001-part7-actor-isolation-sendable-semantic-model-v1"
CONTRACT_ID = "objc3c-part7-actor-isolation-sendable-semantic-model/m270-b001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendable_semantic_model"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m270" / "M270-B001" / "actor_isolation_sendable_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_isolation_and_sendable_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_b001_isolation_and_sendable_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_CONFORMANCE = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_actor_isolation_sendable_semantic_model_positive.objc3"


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
        failures.append(Finding(display_path(path), "M270-B001-MISSING", f"missing artifact: {display_path(path)}"))
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
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def extract_payload(manifest_path: Path) -> dict[str, object]:
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_actor_isolation_and_sendable_semantic_model"]


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m270-b001-readiness",
        "--summary-out",
        "tmp/reports/m270/M270-B001/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M270-B001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M270-B001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "b001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M270-B001-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M270-B001-DYN-04", "positive manifest missing", failures)

    payload: dict[str, object] = {}
    if manifest_path.exists():
        payload = extract_payload(manifest_path)

    expected = {
        "actor_interface_sites": 1,
        "actor_method_sites": 2,
        "actor_property_sites": 1,
        "objc_nonisolated_annotation_sites": 1,
        "actor_member_executor_annotation_sites": 1,
        "actor_async_method_sites": 1,
        "actor_member_metadata_sites": 3,
        "actor_isolation_sendability_sites": 0,
        "actor_isolation_decl_sites": 0,
        "actor_hop_sites": 0,
        "sendable_annotation_sites": 0,
        "non_sendable_crossing_sites": 0,
        "isolation_boundary_sites": 0,
        "normalized_sites": 0,
        "gate_blocked_sites": 0,
        "contract_violation_sites": 0,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        total += 1
        passed += require(payload.get(field) == expected_value, display_path(manifest_path), f"M270-B001-PAYLOAD-{index:02d}", f"{field} mismatch", failures)

    true_fields = [
        "source_dependency_required",
        "actor_member_source_supported",
        "actor_isolation_sendability_profile_normalized",
        "strict_concurrency_selection_fail_closed",
        "actor_runtime_deferred",
        "executor_runtime_deferred",
        "cross_actor_enforcement_deferred",
        "deterministic",
        "ready_for_semantic_expansion",
    ]
    for index, field in enumerate(true_fields, start=40):
        total += 1
        passed += require(payload.get(field) is True, display_path(manifest_path), f"M270-B001-PAYLOAD-{index:02d}", f"{field} must stay true", failures)

    total += 1
    passed += require(payload.get("surface_path") == SURFACE_PATH, display_path(manifest_path), "M270-B001-PAYLOAD-49", "surface_path mismatch", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), display_path(manifest_path), "M270-B001-PAYLOAD-50", "replay key missing", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", display_path(manifest_path), "M270-B001-PAYLOAD-51", "failure_reason must stay empty", failures)

    return total, passed, {"positive_fixture": display_path(FIXTURE), "positive_returncode": run.returncode, "positive_output": output.strip(), "positive_manifest": display_path(manifest_path), "part7_actor_isolation_and_sendable_semantic_model": payload}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M270-B001-EXP-01", "# M270 Isolation And Sendable Semantic Model Contract And Architecture Freeze Expectations (B001)"),
            SnippetCheck("M270-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M270-B001-EXP-03", "The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendable_semantic_model`."),
        ],
        PACKET_DOC: [
            SnippetCheck("M270-B001-PKT-01", "# M270-B001 Packet: Isolation And Sendable Semantic Model - Contract And Architecture Freeze"),
            SnippetCheck("M270-B001-PKT-02", "consume the existing `M270-A002` actor-member source packet"),
            SnippetCheck("M270-B001-PKT-03", "strict-concurrency selection/reporting remains fail-closed"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M270-B001-GRM-01", "## M270 actor isolation and sendable semantic model"),
            SnippetCheck("M270-B001-GRM-02", "objc_part7_actor_isolation_and_sendable_semantic_model"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M270-B001-DOC-01", "## M270 actor isolation and sendable semantic model"),
            SnippetCheck("M270-B001-DOC-02", "objc_part7_actor_isolation_and_sendable_semantic_model"),
        ],
        SPEC_AM: [
            SnippetCheck("M270-B001-AM-01", "M270-B001 actor/sendability semantic-model note:"),
            SnippetCheck("M270-B001-AM-02", "objc_part7_actor_isolation_and_sendable_semantic_model"),
        ],
        SPEC_CONFORMANCE: [
            SnippetCheck("M270-B001-CONF-01", "M270-B001 implementation note:"),
            SnippetCheck("M270-B001-CONF-02", "strict-concurrency selection/reporting remains fail-closed"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M270-B001-SEMA-01", "kObjc3Part7ActorIsolationSendableSemanticModelContractId"),
            SnippetCheck("M270-B001-SEMA-02", "struct Objc3Part7ActorIsolationSendableSemanticModelSummary"),
            SnippetCheck("M270-B001-SEMA-03", "IsReadyObjc3Part7ActorIsolationSendableSemanticModelSummary("),
        ],
        SEMA_HEADER: [
            SnippetCheck("M270-B001-SEMAH-01", "BuildPart7ActorIsolationSendableSemanticModelSummary("),
        ],
        SEMA_CPP: [
            SnippetCheck("M270-B001-SEMACPP-01", "BuildPart7ActorIsolationSendableSemanticModelSummary("),
            SnippetCheck("M270-B001-SEMACPP-02", "summary.strict_concurrency_selection_fail_closed = true;"),
            SnippetCheck("M270-B001-SEMACPP-03", "summary.ready_for_semantic_expansion = summary.deterministic;"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M270-B001-TYP-01", "part7_actor_isolation_sendable_semantic_model_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M270-B001-PIPE-01", "result.part7_actor_isolation_sendable_semantic_model_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M270-B001-ART-01", "BuildPart7ActorIsolationSendableSemanticModelSummaryJson("),
            SnippetCheck("M270-B001-ART-02", "objc_part7_actor_isolation_and_sendable_semantic_model"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M270-B001-PKG-01", '"check:objc3c:m270-b001-isolation-and-sendable-semantic-model-contract-and-architecture-freeze"'),
            SnippetCheck("M270-B001-PKG-02", '"check:objc3c:m270-b001-lane-b-readiness"'),
        ],
        FIXTURE: [
            SnippetCheck("M270-B001-FIX-01", "actor class M270Mailbox"),
            SnippetCheck("M270-B001-FIX-02", "__attribute__((objc_nonisolated))"),
            SnippetCheck("M270-B001-FIX-03", "__attribute__((objc_executor(main)))"),
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

    summary = {"mode": MODE, "contract_id": CONTRACT_ID, "surface_path": SURFACE_PATH, "ok": not failures, "checks_total": total, "checks_passed": passed, "checks_failed": len(failures), "dynamic_probes_executed": dynamic_executed, "failures": [finding.__dict__ for finding in failures], "dynamic": dynamic}
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}")
        print(f"[info] wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"[ok] M270-B001 actor/sendability semantic model checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
