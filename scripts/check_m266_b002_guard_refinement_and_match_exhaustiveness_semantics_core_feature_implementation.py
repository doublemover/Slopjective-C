#!/usr/bin/env python3
"""Checker for M266-B002 guard refinement and match exhaustiveness semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-b002-guard-refinement-and-match-exhaustiveness-v1"
CONTRACT_ID = "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-B002" / "guard_match_exhaustiveness_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_guard_refinement_and_match_exhaustiveness_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_b002_guard_refinement_and_match_exhaustiveness_semantics_core_feature_implementation_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_guard_match_exhaustiveness_positive.objc3"
BOOL_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_match_nonexhaustive_bool_negative.objc3"
RESULT_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_match_nonexhaustive_result_negative.objc3"


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
        failures.append(Finding(display_path(path), "M266-B002-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: list[tuple[str, str, Any, str]] = [
        ("M266-B002-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M266-B002-PAYLOAD-02", "surface_path", "frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model", "surface path drifted"),
        ("M266-B002-PAYLOAD-03", "semantic_model", "guard-refinement-plus-statement-match-exhaustiveness-and-defer-legality-semantics-are-live-while-defer-cleanup-lowering-remains-a-later-lane-c-runtime-step", "semantic model drifted"),
        ("M266-B002-PAYLOAD-04", "match_model", "statement-match-enforces-catch-all-bool-and-result-case-exhaustiveness-with-case-local-binding-scopes-while-result-payload-typing-remains-deferred", "match model drifted"),
        ("M266-B002-PAYLOAD-05", "guard_binding_semantic_sites", 1, "guard binding count mismatch"),
        ("M266-B002-PAYLOAD-06", "guard_binding_clause_semantic_sites", 1, "guard binding clause count mismatch"),
        ("M266-B002-PAYLOAD-07", "guard_condition_statement_sites", 0, "guard condition statement count mismatch"),
        ("M266-B002-PAYLOAD-08", "guard_condition_clause_semantic_sites", 1, "guard condition clause count mismatch"),
        ("M266-B002-PAYLOAD-09", "guard_exit_enforcement_sites", 1, "guard exit count mismatch"),
        ("M266-B002-PAYLOAD-10", "guard_refinement_sites", 1, "guard refinement count mismatch"),
        ("M266-B002-PAYLOAD-11", "match_statement_semantic_sites", 2, "match statement count mismatch"),
        ("M266-B002-PAYLOAD-12", "match_default_pattern_sites", 0, "match default count mismatch"),
        ("M266-B002-PAYLOAD-13", "match_wildcard_pattern_sites", 0, "match wildcard count mismatch"),
        ("M266-B002-PAYLOAD-14", "match_literal_pattern_sites", 2, "match literal count mismatch"),
        ("M266-B002-PAYLOAD-15", "match_binding_scope_sites", 0, "match binding count mismatch"),
        ("M266-B002-PAYLOAD-16", "match_result_case_scope_sites", 2, "match result-case count mismatch"),
        ("M266-B002-PAYLOAD-17", "match_exhaustive_statement_sites", 2, "exhaustive match count mismatch"),
        ("M266-B002-PAYLOAD-18", "match_bool_exhaustive_sites", 1, "bool exhaustive count mismatch"),
        ("M266-B002-PAYLOAD-19", "match_result_case_exhaustive_sites", 1, "result exhaustive count mismatch"),
        ("M266-B002-PAYLOAD-20", "match_non_exhaustive_diagnostic_sites", 0, "non-exhaustive diagnostic count mismatch"),
        ("M266-B002-PAYLOAD-21", "match_exhaustiveness_deferred_sites", 0, "deferred exhaustiveness count mismatch"),
        ("M266-B002-PAYLOAD-22", "failure_reason", "", "failure reason should stay empty"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)
    for check_id, field, expected in [
        ("M266-B002-PAYLOAD-23", "guard_refinement_semantics_landed", True),
        ("M266-B002-PAYLOAD-24", "guard_exit_enforcement_landed", True),
        ("M266-B002-PAYLOAD-25", "match_binding_scope_semantics_landed", True),
        ("M266-B002-PAYLOAD-26", "match_result_case_scope_semantics_landed", True),
        ("M266-B002-PAYLOAD-27", "match_exhaustiveness_semantics_landed", True),
        ("M266-B002-PAYLOAD-28", "match_exhaustiveness_deferred", False),
        ("M266-B002-PAYLOAD-29", "defer_cleanup_order_deferred", False),
        ("M266-B002-PAYLOAD-30", "defer_nonlocal_exit_deferred", False),
        ("M266-B002-PAYLOAD-31", "non_local_exit_restrictions_landed", True),
        ("M266-B002-PAYLOAD-32", "deterministic", True),
        ("M266-B002-PAYLOAD-33", "ready_for_lowering_and_runtime", True),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is expected, artifact, check_id, f"{field} drifted", failures)
    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-B002-PAYLOAD-34", "replay key missing", failures)
    return checks_total, checks_passed


def run_negative_probe(runner: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(runner), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    ensure_build = run_command([sys.executable, "scripts/ensure_objc3c_native_build.py", "--mode", "fast", "--reason", "m266-b002-readiness", "--summary-out", "tmp/reports/m266/M266-B002/ensure_build_summary.json"])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M266-B002-DYN-01", "ensure build failed", failures)
    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b002" / "positive"
    positive = run_command([str(args.runner_exe), str(POSITIVE_FIXTURE), "--out-dir", str(positive_out), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M266-B002-DYN-02", "positive fixture failed", failures)
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M266-B002-DYN-03", "positive manifest missing", failures)
    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload_obj = semantic_surface.get("objc_part5_control_flow_semantic_model")
        checks_total += 1
        checks_passed += require(isinstance(payload_obj, dict), display_path(manifest_path), "M266-B002-DYN-04", "semantic model payload missing", failures)
        if isinstance(payload_obj, dict):
            payload = payload_obj
            total, passed = validate_payload(payload, display_path(manifest_path), failures)
            checks_total += total
            checks_passed += passed
    bool_negative = run_negative_probe(args.runner_exe, BOOL_NEGATIVE, ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b002" / "bool-negative")
    bool_output = (bool_negative.stdout or "") + (bool_negative.stderr or "")
    checks_total += 2
    checks_passed += require(bool_negative.returncode != 0, display_path(BOOL_NEGATIVE), "M266-B002-DYN-05", "bool negative should fail", failures)
    checks_passed += require("O3S206" in bool_output and "match statement must be exhaustive" in bool_output, display_path(BOOL_NEGATIVE), "M266-B002-DYN-06", "bool exhaustiveness diagnostic missing", failures)
    result_negative = run_negative_probe(args.runner_exe, RESULT_NEGATIVE, ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b002" / "result-negative")
    result_output = (result_negative.stdout or "") + (result_negative.stderr or "")
    checks_total += 2
    checks_passed += require(result_negative.returncode != 0, display_path(RESULT_NEGATIVE), "M266-B002-DYN-07", "result negative should fail", failures)
    checks_passed += require("O3S206" in result_output and "match statement must be exhaustive" in result_output, display_path(RESULT_NEGATIVE), "M266-B002-DYN-08", "result exhaustiveness diagnostic missing", failures)
    dynamic = {
        "positive_manifest": display_path(manifest_path),
        "semantic_model_payload": payload,
        "bool_negative_returncode": bool_negative.returncode,
        "result_negative_returncode": result_negative.returncode,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [SnippetCheck("M266-B002-SNIP-01", "admitted `match` statements must be semantically exhaustive")],
        PACKET_DOC: [SnippetCheck("M266-B002-SNIP-02", "Implement live match exhaustiveness semantics")],
        SEMA_CONTRACT: [SnippetCheck("M266-B002-SNIP-03", "match_exhaustiveness_semantics_landed")],
        SEMA_CPP: [
            SnippetCheck("M266-B002-SNIP-04", "ClassifyMatchExhaustiveness"),
            SnippetCheck("M266-B002-SNIP-05", "match statement must be exhaustive for the admitted pattern surface"),
        ],
        FRONTEND_TYPES: [SnippetCheck("M266-B002-SNIP-06", "part5_control_flow_semantic_model_summary")],
        FRONTEND_ARTIFACTS: [SnippetCheck("M266-B002-SNIP-07", "match_exhaustiveness_semantics_landed")],
        DOC_GRAMMAR: [SnippetCheck("M266-B002-SNIP-08", "live bool/result-case exhaustiveness")],
        DOC_NATIVE: [SnippetCheck("M266-B002-SNIP-09", "live bool/result-case exhaustiveness")],
        SPEC_AM: [SnippetCheck("M266-B002-SNIP-10", "M266-B002 implementation note")],
        SPEC_RULES: [SnippetCheck("M266-B002-SNIP-11", "Current Part 5 exhaustiveness note")],
        ARCHITECTURE_DOC: [SnippetCheck("M266-B002-SNIP-12", "M266-B002")],
        PACKAGE_JSON: [SnippetCheck("M266-B002-SNIP-13", "check:objc3c:m266-b002-lane-b-readiness")],
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
