#!/usr/bin/env python3
"""Fail-closed checker for M267-A001 Part 6 error source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-a001-part6-error-source-closure-v1"
CONTRACT_ID = "objc3c-part6-error-source-closure/m267-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m267" / "M267-A001" / "error_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_throws_try_do_catch_result_and_bridging_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_a001_throws_try_do_catch_result_and_bridging_source_closure_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_part6_error_source_closure_positive.objc3"
TRY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_expression_fail_closed_negative.objc3"
THROW_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_throw_statement_fail_closed_negative.objc3"
DO_CATCH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_do_catch_fail_closed_negative.objc3"

EXPECTED_SOURCE_ONLY = [
    "source-only:throws-declarations",
    "source-only:result-carrier-profiles",
    "source-only:nserror-bridging-profiles",
]
EXPECTED_FAIL_CLOSED = [
    "part6-fail-closed:try-expressions",
    "part6-fail-closed:throw-statements",
    "part6-fail-closed:do-catch-statements",
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
        failures.append(Finding(display_path(path), "M267-A001-MISSING", f"missing artifact: {display_path(path)}"))
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
        ("M267-A001-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M267-A001-PAYLOAD-02", "frontend_surface_path", "frontend.pipeline.semantic_surface.objc_part6_error_source_closure", "surface path drifted"),
        ("M267-A001-PAYLOAD-03", "source_only_claim_ids", EXPECTED_SOURCE_ONLY, "source-only claim ids drifted"),
        ("M267-A001-PAYLOAD-04", "fail_closed_construct_ids", EXPECTED_FAIL_CLOSED, "fail-closed construct ids drifted"),
        ("M267-A001-PAYLOAD-05", "function_throws_declaration_sites", 1, "function throws count mismatch"),
        ("M267-A001-PAYLOAD-06", "method_throws_declaration_sites", 0, "method throws count mismatch"),
        ("M267-A001-PAYLOAD-07", "result_like_sites", 7, "result-like site count mismatch"),
        ("M267-A001-PAYLOAD-08", "result_success_sites", 1, "result success count mismatch"),
        ("M267-A001-PAYLOAD-09", "result_failure_sites", 2, "result failure count mismatch"),
        ("M267-A001-PAYLOAD-10", "result_branch_sites", 4, "result branch count mismatch"),
        ("M267-A001-PAYLOAD-11", "result_payload_sites", 3, "result payload count mismatch"),
        ("M267-A001-PAYLOAD-12", "ns_error_bridging_sites", 3, "NSError bridging count mismatch"),
        ("M267-A001-PAYLOAD-13", "ns_error_out_parameter_sites", 1, "NSError out-param count mismatch"),
        ("M267-A001-PAYLOAD-14", "ns_error_bridge_path_sites", 1, "NSError bridge path count mismatch"),
        ("M267-A001-PAYLOAD-15", "try_keyword_sites", 0, "try keyword count mismatch"),
        ("M267-A001-PAYLOAD-16", "throw_keyword_sites", 0, "throw keyword count mismatch"),
        ("M267-A001-PAYLOAD-17", "catch_keyword_sites", 0, "catch keyword count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M267-A001-PAYLOAD-18", "throws_declaration_source_supported"),
        ("M267-A001-PAYLOAD-19", "result_carrier_source_supported"),
        ("M267-A001-PAYLOAD-20", "ns_error_bridging_source_supported"),
        ("M267-A001-PAYLOAD-21", "try_keyword_reserved"),
        ("M267-A001-PAYLOAD-22", "throw_keyword_reserved"),
        ("M267-A001-PAYLOAD-23", "catch_keyword_reserved"),
        ("M267-A001-PAYLOAD-24", "try_fail_closed"),
        ("M267-A001-PAYLOAD-25", "throw_fail_closed"),
        ("M267-A001-PAYLOAD-26", "do_catch_fail_closed"),
        ("M267-A001-PAYLOAD-27", "deterministic_handoff"),
        ("M267-A001-PAYLOAD-28", "ready_for_semantic_expansion"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M267-A001-PAYLOAD-29", "replay key missing", failures)
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
        "m267-a001-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-A001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M267-A001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M267-A001-DYN-02", "frontend runner missing after build", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "a001" / "positive"
    positive_out.mkdir(parents=True, exist_ok=True)
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
    positive_output = (positive.stdout or "") + (positive.stderr or "")
    positive_status_ok = positive.returncode == 0 or "runtime-aware import/module frontend closure not ready" in positive_output
    checks_total += 1
    checks_passed += require(
        positive_status_ok,
        display_path(POSITIVE_FIXTURE),
        "M267-A001-DYN-03",
        f"positive fixture did not reach the expected manifest-emission boundary: {positive_output}",
        failures,
    )
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M267-A001-DYN-04", "positive manifest missing", failures)

    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get("objc_part6_error_source_closure", {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_summary_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed

    negative_specs = [
        (TRY_FIXTURE, "try", "unsupported 'try' expression [O3P268]"),
        (THROW_FIXTURE, "throw", "unsupported 'throw' statement [O3P267]"),
        (DO_CATCH_FIXTURE, "do-catch", "unsupported 'do/catch' statement [O3P269]"),
    ]
    negative_results: list[dict[str, Any]] = []
    for fixture, case_id, expected_snippet in negative_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "a001" / case_id
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
        combined = (run.stdout or "") + (run.stderr or "")
        checks_total += 1
        checks_passed += require(run.returncode != 0, display_path(fixture), f"M267-A001-DYN-{case_id}-01", "negative fixture unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(expected_snippet in combined, display_path(fixture), f"M267-A001-DYN-{case_id}-02", f"expected diagnostic snippet missing: {expected_snippet}", failures)
        negative_results.append({
            "case_id": case_id,
            "fixture": display_path(fixture),
            "expected_snippet": expected_snippet,
            "returncode": run.returncode,
        })

    dynamic = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part6_error_source_closure": payload,
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
            SnippetCheck("M267-A001-EXP-01", "# M267 Throws, Try, Do/Catch, Result, And Bridging Source Closure Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M267-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M267-A001-EXP-03", "frontend.pipeline.semantic_surface.objc_part6_error_source_closure"),
            SnippetCheck("M267-A001-EXP-04", "unsupported 'try' expression [O3P268]"),
            SnippetCheck("M267-A001-EXP-05", "unsupported 'throw' statement [O3P267]"),
            SnippetCheck("M267-A001-EXP-06", "unsupported 'do/catch' statement [O3P269]"),
        ],
        PACKET_DOC: [
            SnippetCheck("M267-A001-PKT-01", "# M267-A001 Packet: Throws, Try, Do/Catch, Result, And Bridging Source Closure Contract And Architecture Freeze"),
            SnippetCheck("M267-A001-PKT-02", "Publish one manifest summary instead of relying on older `M181` / `M182` / `M183` contract shards."),
            SnippetCheck("M267-A001-PKT-03", "`native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M267-A001-GRM-01", "## M267 frontend Part 6 error source closure"),
            SnippetCheck("M267-A001-GRM-02", "`throws` declarations during frontend-only validation"),
            SnippetCheck("M267-A001-GRM-03", "`try`, `throw`, and `do/catch` remain reserved fail-closed parser constructs"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M267-A001-DOC-01", "## M267 frontend Part 6 error source closure"),
            SnippetCheck("M267-A001-DOC-02", "deterministic result-like carrier profiling"),
            SnippetCheck("M267-A001-DOC-03", "`NSError` bridging profiling remains emitted as deterministic frontend state"),
        ],
        SPEC_AM: [
            SnippetCheck("M267-A001-AM-01", "M267-A001 source-closure note:"),
            SnippetCheck("M267-A001-AM-02", "`try`, `throw`, and `do/catch` are reserved fail-closed parser constructs"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M267-A001-ATTR-01", "## M267 current Part 6 error source boundary (implementation note)"),
            SnippetCheck("M267-A001-ATTR-02", "`try`, `throw`, and `do/catch` are reserved parser-owned fail-closed"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M267-A001-TOK-01", "KwTry"),
            SnippetCheck("M267-A001-TOK-02", "kObjc3SourceOnlyFeatureClaimThrowsDeclarations"),
            SnippetCheck("M267-A001-TOK-03", "kObjc3Part6FailClosedConstructDoCatchStatements"),
        ],
        LEXER_CPP: [
            SnippetCheck("M267-A001-LEX-01", 'ident == "try"'),
            SnippetCheck("M267-A001-LEX-02", 'ident == "throw"'),
            SnippetCheck("M267-A001-LEX-03", 'ident == "catch"'),
        ],
        PARSER_CPP: [
            SnippetCheck("M267-A001-PARSE-01", "unsupported 'throw' statement"),
            SnippetCheck("M267-A001-PARSE-02", "unsupported 'do/catch' statement"),
            SnippetCheck("M267-A001-PARSE-03", "unsupported 'try' expression"),
            SnippetCheck("M267-A001-PARSE-04", 'Peek().text == "throws"'),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M267-A001-SEMAC-01", "allow_source_only_error_runtime_surface"),
        ],
        SEMA_PASSES_H: [
            SnippetCheck("M267-A001-SEMAH-01", "bool allow_source_only_error_runtime_surface"),
        ],
        SEMA_PASSES_CPP: [
            SnippetCheck("M267-A001-SEMA-01", "allow_source_only_error_runtime_surface"),
            SnippetCheck("M267-A001-SEMA-02", "if (fn.throws_declared && !context.allow_source_only_error_runtime_surface)"),
            SnippetCheck("M267-A001-SEMA-03", "if (method.throws_declared && !context.allow_source_only_error_runtime_surface)"),
        ],
        SEMA_PASS_MANAGER: [
            SnippetCheck("M267-A001-SEMAPM-01", "input.validation_options.allow_source_only_error_runtime_surface"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M267-A001-TYP-01", "kObjc3Part6ErrorSourceClosureContractId"),
            SnippetCheck("M267-A001-TYP-02", "struct Objc3FrontendPart6ErrorSourceClosureSummary"),
            SnippetCheck("M267-A001-TYP-03", "IsReadyObjc3FrontendPart6ErrorSourceClosureSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M267-A001-PIPE-01", "BuildPart6ErrorSourceClosureReplayKey("),
            SnippetCheck("M267-A001-PIPE-02", "BuildPart6ErrorSourceClosureSummary("),
            SnippetCheck("M267-A001-PIPE-03", "result.part6_error_source_closure_summary ="),
            SnippetCheck("M267-A001-PIPE-04", "semantic_options.allow_source_only_error_runtime_surface ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M267-A001-ART-01", "BuildPart6ErrorSourceClosureSummaryJson("),
            SnippetCheck("M267-A001-ART-02", '\\"objc_part6_error_source_closure\\"'),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M267-A001-PKG-01", '"check:objc3c:m267-a001-throws-try-do-catch-result-and-bridging-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M267-A001-PKG-02", '"check:objc3c:m267-a001-lane-a-readiness"'),
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
    print(f"[ok] M267-A001 Part 6 error source closure checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
