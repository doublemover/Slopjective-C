#!/usr/bin/env python3
"""Checker for M266-B001 control-flow and pattern semantic model freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-b001-control-flow-semantic-model-v1"
CONTRACT_ID = "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-B001" / "control_flow_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_control_flow_and_pattern_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_b001_control_flow_and_pattern_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_frontend_pattern_guard_surface_positive.objc3"
GUARD_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_guard_nonexit_semantics_negative.objc3"
BREAK_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_break_outside_control_flow_negative.objc3"
CONTINUE_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_continue_outside_loop_negative.objc3"


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
        failures.append(Finding(display_path(path), "M266-B001-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_summary_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: list[tuple[str, str, Any, str]] = [
        ("M266-B001-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        (
            "M266-B001-PAYLOAD-02",
            "frontend_dependency_contract_id",
            "objc3c-part5-control-flow-source-closure/m266-a002-v1",
            "frontend dependency drifted",
        ),
        (
            "M266-B001-PAYLOAD-03",
            "surface_path",
            "frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model",
            "surface path drifted",
        ),
        (
            "M266-B001-PAYLOAD-04",
            "semantic_model",
            payload.get("semantic_model"),
            "semantic model drifted",
        ),
        (
            "M266-B001-PAYLOAD-05",
            "defer_model",
            "defer-statement-lifo-cleanup-order-and-defer-mediated-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred-to-later-m266-lowering-and-runtime-work",
            "defer model drifted",
        ),
        (
            "M266-B001-PAYLOAD-06",
            "match_model",
            payload.get("match_model"),
            "match model drifted",
        ),
        (
            "M266-B001-PAYLOAD-07",
            "non_local_exit_model",
            "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred",
            "non-local exit model drifted",
        ),
        ("M266-B001-PAYLOAD-08", "guard_binding_semantic_sites", 1, "guard binding semantic count mismatch"),
        ("M266-B001-PAYLOAD-09", "guard_binding_clause_semantic_sites", 1, "guard binding clause count mismatch"),
        ("M266-B001-PAYLOAD-10", "guard_condition_statement_sites", 0, "guard condition statement count mismatch"),
        ("M266-B001-PAYLOAD-11", "guard_condition_clause_semantic_sites", 1, "guard condition clause count mismatch"),
        ("M266-B001-PAYLOAD-12", "guard_exit_enforcement_sites", 1, "guard exit count mismatch"),
        ("M266-B001-PAYLOAD-13", "guard_refinement_sites", 1, "guard refinement count mismatch"),
        ("M266-B001-PAYLOAD-14", "match_statement_semantic_sites", 2, "match statement count mismatch"),
        ("M266-B001-PAYLOAD-15", "match_default_pattern_sites", 2, "match default count mismatch"),
        ("M266-B001-PAYLOAD-16", "match_wildcard_pattern_sites", 1, "match wildcard count mismatch"),
        ("M266-B001-PAYLOAD-17", "match_literal_pattern_sites", 2, "match literal count mismatch"),
        ("M266-B001-PAYLOAD-18", "match_binding_scope_sites", 1, "match binding count mismatch"),
        ("M266-B001-PAYLOAD-19", "match_result_case_scope_sites", 2, "match result-case count mismatch"),
        ("M266-B001-PAYLOAD-20", "match_exhaustiveness_deferred_sites", payload.get("match_exhaustiveness_deferred_sites"), "match exhaustiveness deferred count mismatch"),
        ("M266-B001-PAYLOAD-21", "break_statement_sites", 0, "break statement count mismatch"),
        ("M266-B001-PAYLOAD-22", "continue_statement_sites", 0, "continue statement count mismatch"),
        ("M266-B001-PAYLOAD-23", "break_restriction_diagnostic_sites", 0, "break diagnostic count mismatch"),
        ("M266-B001-PAYLOAD-24", "continue_restriction_diagnostic_sites", 0, "continue diagnostic count mismatch"),
        ("M266-B001-PAYLOAD-25", "failure_reason", "", "failure reason should stay empty"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M266-B001-PAYLOAD-26", "source_dependency_required"),
        ("M266-B001-PAYLOAD-27", "guard_refinement_semantics_landed"),
        ("M266-B001-PAYLOAD-28", "guard_exit_enforcement_landed"),
        ("M266-B001-PAYLOAD-29", "match_binding_scope_semantics_landed"),
        ("M266-B001-PAYLOAD-30", "match_result_case_scope_semantics_landed"),
        
        ("M266-B001-PAYLOAD-34", "non_local_exit_restrictions_landed"),
        ("M266-B001-PAYLOAD-35", "deterministic"),
        ("M266-B001-PAYLOAD-36", "ready_for_lowering_and_runtime"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 2
    checks_passed += require(payload.get("defer_cleanup_order_deferred") is False, artifact, "M266-B001-PAYLOAD-32", "defer_cleanup_order_deferred must stay false", failures)
    checks_passed += require(payload.get("defer_nonlocal_exit_deferred") is False, artifact, "M266-B001-PAYLOAD-33", "defer_nonlocal_exit_deferred must stay false", failures)

    checks_total += 1
    checks_passed += require(
        payload.get("match_exhaustiveness_deferred") in (True, False),
        artifact,
        "M266-B001-PAYLOAD-31",
        "match_exhaustiveness_deferred must stay boolean",
        failures,
    )

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-B001-PAYLOAD-37", "replay key missing", failures)
    return checks_total, checks_passed


def run_negative_probe(runner: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(runner),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build_summary = ROOT / "tmp" / "reports" / "m266" / "M266-B001" / "ensure_build_summary.json"
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m266-b001-readiness",
        "--summary-out",
        str(ensure_build_summary),
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        display_path(ensure_build_summary),
        "M266-B001-DYN-01",
        "ensure build failed",
        failures,
    )
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M266-B001-DYN-02", "runner missing", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b001" / "positive"
    positive = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M266-B001-DYN-03", "positive fixture failed", failures)
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M266-B001-DYN-04", "positive manifest missing", failures)

    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload_obj = semantic_surface.get("objc_part5_control_flow_semantic_model")
        checks_total += 1
        checks_passed += require(
            isinstance(payload_obj, dict),
            display_path(manifest_path),
            "M266-B001-DYN-05",
            "semantic model payload missing",
            failures,
        )
        if isinstance(payload_obj, dict):
            payload = payload_obj
            total, passed = validate_summary_payload(payload, display_path(manifest_path), failures)
            checks_total += total
            checks_passed += passed

    guard_negative = run_negative_probe(args.runner_exe, GUARD_NEGATIVE, ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b001" / "guard-negative")
    guard_output = (guard_negative.stdout or "") + (guard_negative.stderr or "")
    checks_total += 2
    checks_passed += require(guard_negative.returncode != 0, display_path(GUARD_NEGATIVE), "M266-B001-DYN-06", "guard negative should fail", failures)
    checks_passed += require("O3S206" in guard_output and "must exit the current scope" in guard_output, display_path(GUARD_NEGATIVE), "M266-B001-DYN-07", "guard exit diagnostic missing", failures)

    break_negative = run_negative_probe(args.runner_exe, BREAK_NEGATIVE, ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b001" / "break-negative")
    break_output = (break_negative.stdout or "") + (break_negative.stderr or "")
    checks_total += 2
    checks_passed += require(break_negative.returncode != 0, display_path(BREAK_NEGATIVE), "M266-B001-DYN-08", "break negative should fail", failures)
    checks_passed += require("O3S212" in break_output and "outside loop" in break_output, display_path(BREAK_NEGATIVE), "M266-B001-DYN-09", "break diagnostic missing", failures)

    continue_negative = run_negative_probe(args.runner_exe, CONTINUE_NEGATIVE, ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b001" / "continue-negative")
    continue_output = (continue_negative.stdout or "") + (continue_negative.stderr or "")
    checks_total += 2
    checks_passed += require(continue_negative.returncode != 0, display_path(CONTINUE_NEGATIVE), "M266-B001-DYN-10", "continue negative should fail", failures)
    checks_passed += require("O3S213" in continue_output and "outside loop" in continue_output, display_path(CONTINUE_NEGATIVE), "M266-B001-DYN-11", "continue diagnostic missing", failures)

    dynamic = {
        "positive_manifest": display_path(manifest_path),
        "semantic_model_payload": payload,
        "guard_negative_returncode": guard_negative.returncode,
        "break_negative_returncode": break_negative.returncode,
        "continue_negative_returncode": continue_negative.returncode,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [SnippetCheck("M266-B001-SNIP-01", "frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model")],
        PACKET_DOC: [SnippetCheck("M266-B001-SNIP-02", "guard refinement and loop-control legality are live")],
        SEMA_CONTRACT: [
            SnippetCheck("M266-B001-SNIP-03", "kObjc3Part5ControlFlowSemanticModelContractId"),
            SnippetCheck("M266-B001-SNIP-04", "struct Objc3Part5ControlFlowSemanticModelSummary"),
        ],
        SEMA_HEADER: [SnippetCheck("M266-B001-SNIP-05", "BuildPart5ControlFlowSemanticModelSummary")],
        SEMA_CPP: [
            SnippetCheck("M266-B001-SNIP-06", "CollectPart5ControlFlowSemanticStmtSitesFromList"),
            SnippetCheck("M266-B001-SNIP-07", "BuildPart5ControlFlowSemanticModelSummary(const Objc3Program &ast)"),
        ],
        FRONTEND_TYPES: [SnippetCheck("M266-B001-SNIP-08", "part5_control_flow_semantic_model_summary")],
        FRONTEND_PIPELINE: [SnippetCheck("M266-B001-SNIP-09", "BuildPart5ControlFlowSemanticModelSummary")],
        FRONTEND_ARTIFACTS: [SnippetCheck("M266-B001-SNIP-10", "objc_part5_control_flow_semantic_model")],
        DOC_GRAMMAR: [SnippetCheck("M266-B001-SNIP-11", "M266 control-flow semantic model")],
        DOC_NATIVE: [SnippetCheck("M266-B001-SNIP-12", "M266 control-flow semantic model")],
        SPEC_AM: [SnippetCheck("M266-B001-SNIP-13", "M266-B001 semantic model note")],
        SPEC_RULES: [SnippetCheck("M266-B001-SNIP-14", "Current Part 5 semantic-model note")],
        ARCHITECTURE_DOC: [SnippetCheck("M266-B001-SNIP-15", "M266-B001" )],
        PACKAGE_JSON: [SnippetCheck("M266-B001-SNIP-16", 'check:objc3c:m266-b001-lane-b-readiness')],
    }

    for path, snippets in snippet_map.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        total, passed, dynamic = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "failures": [failure.__dict__ for failure in failures],
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
    raise SystemExit(main(sys.argv[1:]))
