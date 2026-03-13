#!/usr/bin/env python3
"""Checker for M267-B002 Part 6 try/throw/do-catch semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-b002-try-do-catch-semantics-v1"
CONTRACT_ID = "objc3c-part6-try-throw-do-catch-semantics/m267-b002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m267" / "M267-B002" / "try_do_catch_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_try_do_catch_and_propagation_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_b002_try_do_catch_and_propagation_semantics_core_feature_implementation_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_SEMANTICS = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART6 = ROOT / "spec" / "PART_6_ERRORS_RESULTS_THROWS.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_do_catch_semantics_positive.objc3"
TRY_CONTEXT_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_requires_throwing_context_negative.objc3"
TRY_OPERAND_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_requires_throwing_or_bridged_operand_negative.objc3"
THROW_CONTEXT_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_throw_requires_throws_or_catch_negative.objc3"
CATCH_ORDER_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_catch_after_catch_all_negative.objc3"
NATIVE_FAIL_CLOSED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_do_catch_native_fail_closed.objc3"
SURFACE_KEY = "objc_part6_try_do_catch_semantics"


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
        failures.append(Finding(display_path(path), "M267-B002-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_common_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_common: list[tuple[str, str, Any, str]] = [
        ("M267-B002-COMMON-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M267-B002-COMMON-02", "dependency_contract_id", "objc3c-part6-error-semantic-model/m267-b001-v1", "dependency contract drifted"),
        ("M267-B002-COMMON-03", "surface_path", "frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics", "surface path drifted"),
        ("M267-B002-COMMON-04", "semantic_model", "try-throw-and-do-catch-parse-and-undergo-deterministic-legality-checking-in-source-only-native-validation-while-lowering-and-runtime-integration-remain-later-lane-work", "semantic model drifted"),
        ("M267-B002-COMMON-05", "deferred_model", "native-ir-object-execution-lowering-catch-transfer-and-thrown-error-abi-remain-deferred-to-lanes-c-and-d", "deferred model drifted"),
    ]
    for check_id, field, expected, detail in expected_common:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M267-B002-COMMON-06", "source_dependency_required"),
        ("M267-B002-COMMON-07", "try_surface_landed"),
        ("M267-B002-COMMON-08", "throw_surface_landed"),
        ("M267-B002-COMMON-09", "do_catch_surface_landed"),
        ("M267-B002-COMMON-10", "throwing_context_legality_enforced"),
        ("M267-B002-COMMON-11", "native_emit_remains_fail_closed"),
        ("M267-B002-COMMON-12", "deterministic"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 2
    checks_passed += require(payload.get("ready_for_lowering_and_runtime") is False, artifact, "M267-B002-COMMON-13", "ready_for_lowering_and_runtime must stay false", failures)
    checks_passed += require(payload.get("failure_reason") == "", artifact, "M267-B002-COMMON-14", "failure reason should stay empty", failures)

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M267-B002-COMMON-15", "replay key missing", failures)
    return checks_total, checks_passed


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total, checks_passed = validate_common_payload(payload, artifact, failures)
    expected = {
        "try_expression_sites": 3,
        "try_propagating_sites": 1,
        "try_optional_sites": 1,
        "try_forced_sites": 1,
        "throw_statement_sites": 1,
        "do_catch_sites": 1,
        "catch_clause_sites": 2,
        "catch_binding_sites": 1,
        "catch_all_sites": 1,
        "throwing_callable_try_sites": 2,
        "bridged_callable_try_sites": 3,
        "caller_propagation_sites": 1,
        "local_handler_sites": 0,
        "rethrow_sites": 0,
        "contract_violation_sites": 0,
    }
    for index, (field, value) in enumerate(expected.items(), start=16):
        checks_total += 1
        checks_passed += require(payload.get(field) == value, artifact, f"M267-B002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    return checks_total, checks_passed


def run_positive_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    combined = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(
        run.returncode == 0 or "runtime-aware import/module frontend closure not ready" in combined,
        display_path(POSITIVE_FIXTURE),
        "M267-B002-DYN-POS-01",
        f"positive fixture did not reach expected manifest emission boundary: {combined}",
        failures,
    )
    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M267-B002-DYN-POS-02", "positive manifest missing", failures)
    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get(SURFACE_KEY, {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_positive_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed
    return checks_total, checks_passed, payload


def run_negative_probe(args: argparse.Namespace, fixture: Path, case_id: str, expected_snippet: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b002" / case_id
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
    checks_passed += require(run.returncode != 0, display_path(fixture), f"M267-B002-DYN-{case_id}-01", "negative fixture unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require(expected_snippet in combined, display_path(fixture), f"M267-B002-DYN-{case_id}-02", f"missing expected diagnostic: {expected_snippet}", failures)
    return checks_total, checks_passed


def run_native_fail_closed_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b002" / "native-fail-closed"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(NATIVE_FAIL_CLOSED_FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    combined = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode != 0, display_path(NATIVE_FAIL_CLOSED_FIXTURE), "M267-B002-DYN-NATIVE-01", "native-emission probe unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require("unsupported feature claim: do/catch statements are not yet runnable in Objective-C 3 native mode [O3S267]" in combined, display_path(NATIVE_FAIL_CLOSED_FIXTURE), "M267-B002-DYN-NATIVE-02", "native fail-closed diagnostic missing", failures)
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
        "m267-b002-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-B002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M267-B002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M267-B002-DYN-02", "frontend runner missing after build", failures)

    pos_total, pos_passed, payload = run_positive_probe(args, failures)
    checks_total += pos_total
    checks_passed += pos_passed

    for fixture, case_id, snippet in [
        (TRY_CONTEXT_NEGATIVE, "TRY-CONTEXT", "propagating try requires a throws function or an enclosing do/catch [O3S272]"),
        (TRY_OPERAND_NEGATIVE, "TRY-OPERAND", "try operand must be a throwing or NSError-bridged call surface [O3S271]"),
        (THROW_CONTEXT_NEGATIVE, "THROW-CONTEXT", "throw statements require a throws function or a catch body [O3S274]"),
        (CATCH_ORDER_NEGATIVE, "CATCH-ORDER", "catch clauses after a catch-all are unreachable [O3S269]"),
    ]:
        sub_total, sub_passed = run_negative_probe(args, fixture, case_id, snippet, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    native_total, native_passed = run_native_fail_closed_probe(args, failures)
    checks_total += native_total
    checks_passed += native_passed

    return checks_total, checks_passed, {"positive_payload": payload}


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_checks: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M267-B002-STATIC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M267-B002-STATIC-02", "`frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M267-B002-STATIC-03", "Implement truthful Part 6 source-only semantic legality for `try`, `throw`, and `do/catch`"),
            SnippetCheck("M267-B002-STATIC-04", "`frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`"),
        ],
        AST_HEADER: [
            SnippetCheck("M267-B002-STATIC-05", "TryOperatorKind"),
            SnippetCheck("M267-B002-STATIC-06", "struct CatchClause"),
        ],
        PARSER_CPP: [
            SnippetCheck("M267-B002-STATIC-07", "BuildTryExpressionProfile("),
            SnippetCheck("M267-B002-STATIC-08", "BuildThrowStatementProfile("),
            SnippetCheck("M267-B002-STATIC-09", "BuildDoCatchProfile("),
            SnippetCheck("M267-B002-STATIC-10", "ParseCatchClause("),
            SnippetCheck("M267-B002-STATIC-11", "__objc3_try_expr"),
            SnippetCheck("M267-B002-STATIC-12", "__objc3_throw_stmt"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M267-B002-STATIC-13", "kObjc3Part6TryDoCatchSemanticSummaryContractId"),
            SnippetCheck("M267-B002-STATIC-14", "struct Objc3Part6TryDoCatchSemanticSummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M267-B002-STATIC-15", "BuildPart6TryDoCatchSemanticSummary("),
        ],
        SEMA_CPP: [
            SnippetCheck("M267-B002-STATIC-16", "WalkPart6TryDoCatchStmt("),
            SnippetCheck("M267-B002-STATIC-17", "WalkPart6TryDoCatchExpr("),
            SnippetCheck("M267-B002-STATIC-18", '"O3S271"'),
            SnippetCheck("M267-B002-STATIC-19", '"O3S272"'),
            SnippetCheck("M267-B002-STATIC-20", '"O3S274"'),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M267-B002-STATIC-21", "part6_try_do_catch_semantic_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M267-B002-STATIC-22", "BuildPart6TryDoCatchSemanticSummary("),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M267-B002-STATIC-23", "BuildPart6TryDoCatchSemanticSummaryJson("),
            SnippetCheck("M267-B002-STATIC-24", '\\"objc_part6_try_do_catch_semantics\\"'),
        ],
        DOC_SEMANTICS: [
            SnippetCheck("M267-B002-STATIC-25", "## M267 current Part 6 try/do/catch semantic boundary"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M267-B002-STATIC-26", "## M267 current Part 6 try/do/catch semantic boundary"),
        ],
        SPEC_AM: [
            SnippetCheck("M267-B002-STATIC-27", "M267-B002 try/do/catch semantic note:"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M267-B002-STATIC-28", "## M267 current Part 6 try/do/catch semantic boundary"),
        ],
        SPEC_PART6: [
            SnippetCheck("M267-B002-STATIC-29", "Current implementation status (`M267-B002`)"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M267-B002-STATIC-30", 'check:objc3c:m267-b002-try-do-catch-and-propagation-semantics-core-feature-implementation'),
            SnippetCheck("M267-B002-STATIC-31", 'check:objc3c:m267-b002-lane-b-readiness'),
        ],
    }
    for path, snippets in static_checks.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        sub_total, sub_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_payload": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
