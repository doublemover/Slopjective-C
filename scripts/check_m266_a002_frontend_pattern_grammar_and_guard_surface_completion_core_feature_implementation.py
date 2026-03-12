#!/usr/bin/env python3
"""Checker for M266-A002 frontend pattern grammar and guard surface completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-a002-frontend-pattern-grammar-and-guard-surface-v1"
CONTRACT_ID = "objc3c-part5-control-flow-source-closure/m266-a002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-A002" / "frontend_pattern_guard_surface_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_frontend_pattern_grammar_and_guard_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_a002_frontend_pattern_grammar_and_guard_surface_completion_core_feature_implementation_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_frontend_pattern_guard_surface_positive.objc3"
MATCH_EXPR_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_match_expression_fail_closed_negative.objc3"
GUARDED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_match_guarded_pattern_fail_closed_negative.objc3"
TYPE_TEST_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_match_type_test_pattern_fail_closed_negative.objc3"


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
        failures.append(Finding(display_path(path), "M266-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
        ("M266-A002-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        (
            "M266-A002-PAYLOAD-02",
            "frontend_surface_path",
            "frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure",
            "surface path drifted",
        ),
        (
            "M266-A002-PAYLOAD-03",
            "supported_construct_ids",
            [
                "part5-source:guard-bindings",
                "part5-source:guard-condition-lists",
                "part5-source:switch-case-patterns",
                "part5-source:match-statement",
                "part5-source:match-wildcard-patterns",
                "part5-source:match-literal-patterns",
                "part5-source:match-binding-patterns",
                "part5-source:match-result-case-patterns",
            ],
            "supported construct ids drifted",
        ),
        (
            "M266-A002-PAYLOAD-04",
            "fail_closed_construct_ids",
            [
                "part5-fail-closed:defer-statement",
                "part5-fail-closed:match-expression",
                "part5-fail-closed:guarded-match-patterns",
                "part5-fail-closed:match-type-test-patterns",
            ],
            "fail-closed construct ids drifted",
        ),
        ("M266-A002-PAYLOAD-05", "guard_binding_sites", 1, "guard binding site count mismatch"),
        ("M266-A002-PAYLOAD-06", "guard_binding_clause_sites", 1, "guard binding clause count mismatch"),
        ("M266-A002-PAYLOAD-07", "guard_boolean_condition_sites", 1, "guard boolean condition count mismatch"),
        ("M266-A002-PAYLOAD-08", "match_statement_sites", 2, "match statement count mismatch"),
        ("M266-A002-PAYLOAD-09", "match_case_pattern_sites", 6, "match case pattern count mismatch"),
        ("M266-A002-PAYLOAD-10", "match_default_sites", 2, "match default count mismatch"),
        ("M266-A002-PAYLOAD-11", "match_wildcard_pattern_sites", 1, "match wildcard count mismatch"),
        ("M266-A002-PAYLOAD-12", "match_literal_pattern_sites", 2, "match literal count mismatch"),
        ("M266-A002-PAYLOAD-13", "match_binding_pattern_sites", 1, "match binding count mismatch"),
        ("M266-A002-PAYLOAD-14", "match_result_case_pattern_sites", 2, "match result-case count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M266-A002-PAYLOAD-15", "guard_binding_source_supported"),
        ("M266-A002-PAYLOAD-16", "guard_condition_list_source_supported"),
        ("M266-A002-PAYLOAD-17", "switch_case_pattern_source_supported"),
        ("M266-A002-PAYLOAD-18", "match_statement_source_supported"),
        ("M266-A002-PAYLOAD-19", "match_wildcard_pattern_source_supported"),
        ("M266-A002-PAYLOAD-20", "match_literal_pattern_source_supported"),
        ("M266-A002-PAYLOAD-21", "match_binding_pattern_source_supported"),
        ("M266-A002-PAYLOAD-22", "match_result_case_pattern_source_supported"),
        ("M266-A002-PAYLOAD-23", "defer_fail_closed"),
        ("M266-A002-PAYLOAD-24", "match_expression_fail_closed"),
        ("M266-A002-PAYLOAD-25", "guarded_pattern_fail_closed"),
        ("M266-A002-PAYLOAD-26", "type_test_pattern_fail_closed"),
        ("M266-A002-PAYLOAD-27", "deterministic_handoff"),
        ("M266-A002-PAYLOAD-28", "ready_for_semantic_expansion"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-A002-PAYLOAD-29", "replay key missing", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build_summary = ROOT / "tmp" / "reports" / "m266" / "M266-A002" / "ensure_build_summary.json"
    ensure_build = run_command(
        [
            sys.executable,
            "scripts/ensure_objc3c_native_build.py",
            "--mode",
            "fast",
            "--reason",
            "m266-a002-readiness",
            "--summary-out",
            str(ensure_build_summary),
        ]
    )
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        display_path(ensure_build_summary),
        "M266-A002-DYN-00",
        "ensure build failed",
        failures,
    )

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "a002" / "positive"
    positive_out_dir.mkdir(parents=True, exist_ok=True)
    positive_run = run_command(
        [
            str(args.runner_exe),
            str(POSITIVE_FIXTURE),
            "--out-dir",
            str(positive_out_dir),
            "--emit-prefix",
            "module",
            "--no-emit-ir",
            "--no-emit-object",
        ]
    )
    checks_total += 1
    checks_passed += require(
        positive_run.returncode == 0,
        display_path(POSITIVE_FIXTURE),
        "M266-A002-DYN-01",
        "positive fixture failed",
        failures,
    )
    manifest_path = positive_out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M266-A002-DYN-02", "manifest missing", failures)

    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload_obj = semantic_surface.get("objc_part5_control_flow_source_closure")
        checks_total += 1
        checks_passed += require(
            isinstance(payload_obj, dict),
            display_path(manifest_path),
            "M266-A002-DYN-03",
            "part5 semantic-surface payload missing",
            failures,
        )
        if isinstance(payload_obj, dict):
            payload = payload_obj
            total, passed = validate_summary_payload(payload, display_path(manifest_path), failures)
            checks_total += total
            checks_passed += passed

    negative_specs = [
        (MATCH_EXPR_FIXTURE, "match_expression", "reserved expression-form 'match' arm [O3P156]"),
        (GUARDED_FIXTURE, "guarded_pattern", "unsupported guarded match pattern [O3P157]"),
        (TYPE_TEST_FIXTURE, "type_test_pattern", "unsupported type-test match pattern [O3P158]"),
    ]
    negative_results: list[dict[str, Any]] = []
    for fixture, case_id, expected_snippet in negative_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "a002" / case_id
        out_dir.mkdir(parents=True, exist_ok=True)
        run = run_command(
            [
                str(args.runner_exe),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                "--no-emit-ir",
                "--no-emit-object",
            ]
        )
        combined = (run.stdout or "") + (run.stderr or "")
        checks_total += 1
        checks_passed += require(run.returncode != 0, display_path(fixture), f"M266-A002-DYN-{case_id}-01", "negative fixture unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(expected_snippet in combined, display_path(fixture), f"M266-A002-DYN-{case_id}-02", f"expected diagnostic snippet missing: {expected_snippet}", failures)
        negative_results.append({
            "case_id": case_id,
            "fixture": display_path(fixture),
            "expected_snippet": expected_snippet,
            "returncode": run.returncode,
        })

    dynamic = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_manifest": display_path(manifest_path),
        "part5_control_flow_source_closure": payload,
        "negative_cases": negative_results,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M266-A002-EXP-01", "# M266 Frontend Pattern Grammar And Guard Surface Completion Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M266-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M266-A002-EXP-03", "statement-form `match (...) { ... }`"),
            SnippetCheck("M266-A002-EXP-04", "reserved expression-form 'match' arm [O3P156]"),
        ],
        PACKET_DOC: [
            SnippetCheck("M266-A002-PKT-01", "# M266-A002 Packet: Frontend Pattern Grammar And Guard Surface Completion Core Feature Implementation"),
            SnippetCheck("M266-A002-PKT-02", "guard` condition lists containing optional bindings followed by boolean clauses"),
            SnippetCheck("M266-A002-PKT-03", "result-case patterns such as `.Ok(let value)` and `.Err(let error)`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M266-A002-GRM-01", "## M266 frontend pattern grammar and guard surface"),
            SnippetCheck("M266-A002-GRM-02", "statement-form `match (...) { ... }` is now admitted as a frontend-owned"),
            SnippetCheck("M266-A002-GRM-03", "expression-form `match` arms using `=>`, guarded patterns using `where`, and"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M266-A002-DOC-01", "## M266 frontend pattern grammar and guard surface"),
            SnippetCheck("M266-A002-DOC-02", "guard` now also accepts comma-separated boolean clauses"),
            SnippetCheck("M266-A002-DOC-03", "statement-form `match (...) { ... }` is now admitted as a frontend-owned"),
        ],
        SPEC_AM: [
            SnippetCheck("M266-A002-AM-01", "Current Part 5 frontend note:"),
            SnippetCheck("M266-A002-AM-02", "statement-form `match` is now parsed as a frontend-owned control-flow surface"),
        ],
        SPEC_RULES: [
            SnippetCheck("M266-A002-RULE-01", "Statement-form `match` is now admitted as a frontend-owned control-flow"),
            SnippetCheck("M266-A002-RULE-02", "`defer`, expression-form `match`, guarded patterns, and type-test patterns"),
        ],
        ARCHITECTURE_DOC: [
            SnippetCheck("M266-A002-ARCH-01", "M266-A002"),
            SnippetCheck("M266-A002-ARCH-02", "guard-condition lists and statement-form `match` patterns"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M266-A002-TOK-01", "kObjc3Part5SourceSurfaceGuardConditionLists"),
            SnippetCheck("M266-A002-TOK-02", "kObjc3Part5SourceSurfaceMatchResultCasePatterns"),
            SnippetCheck("M266-A002-TOK-03", "kObjc3Part5FailClosedConstructMatchExpression"),
        ],
        AST_HEADER: [
            SnippetCheck("M266-A002-AST-01", "guard_condition_list_surface_enabled"),
            SnippetCheck("M266-A002-AST-02", "enum class MatchPatternKind"),
            SnippetCheck("M266-A002-AST-03", "match_result_case_name"),
        ],
        PARSER_CPP: [
            SnippetCheck("M266-A002-PARSE-01", "ParseGuardConditionClauses()"),
            SnippetCheck("M266-A002-PARSE-02", "ParseMatchPattern(SwitchCase &case_stmt)"),
            SnippetCheck("M266-A002-PARSE-03", "reserved expression-form 'match' arm"),
            SnippetCheck("M266-A002-PARSE-04", "unsupported guarded match pattern"),
            SnippetCheck("M266-A002-PARSE-05", "unsupported type-test match pattern"),
        ],
        SEMA_CPP: [
            SnippetCheck("M266-A002-SEMA-01", "guard_condition_exprs"),
            SnippetCheck("M266-A002-SEMA-02", "match_surface_enabled"),
            SnippetCheck("M266-A002-SEMA-03", "guard condition must be bool-compatible"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M266-A002-TYP-01", "objc3c-part5-control-flow-source-closure/m266-a002-v1"),
            SnippetCheck("M266-A002-TYP-02", "guard_boolean_condition_sites"),
            SnippetCheck("M266-A002-TYP-03", "match_result_case_pattern_sites"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M266-A002-PIPE-01", "guard_boolean_condition_sites"),
            SnippetCheck("M266-A002-PIPE-02", "match_statement_sites"),
            SnippetCheck("M266-A002-PIPE-03", "match_result_case_pattern_sites"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M266-A002-ART-01", '<< ",\\"guard_boolean_condition_sites\\":"'),
            SnippetCheck("M266-A002-ART-02", '<< ",\\"match_statement_sites\\":"'),
            SnippetCheck("M266-A002-ART-03", '<< ",\\"match_result_case_pattern_sites\\":"'),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M266-A002-PKG-01", '"check:objc3c:m266-a002-frontend-pattern-grammar-and-guard-surface-completion-core-feature-implementation"'),
            SnippetCheck("M266-A002-PKG-02", '"test:tooling:m266-a002-frontend-pattern-grammar-and-guard-surface-completion-core-feature-implementation"'),
            SnippetCheck("M266-A002-PKG-03", '"check:objc3c:m266-a002-lane-a-readiness"'),
        ],
    }

    for path, required in snippets.items():
        checks_total += len(required)
        checks_passed += ensure_snippets(path, required, failures)

    dynamic: dict[str, Any] = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
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
    print(f"[ok] M266-A002 frontend pattern grammar and guard surface checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
