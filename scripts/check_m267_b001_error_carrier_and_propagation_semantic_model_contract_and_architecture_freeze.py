#!/usr/bin/env python3
"""Checker for M267-B001 Part 6 error semantic model freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-b001-error-semantic-model-v1"
CONTRACT_ID = "objc3c-part6-error-semantic-model/m267-b001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m267" / "M267-B001" / "error_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_error_carrier_and_propagation_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_b001_error_carrier_and_propagation_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_SEMANTICS = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART6 = ROOT / "spec" / "PART_6_ERRORS_RESULTS_THROWS.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_A001 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_part6_error_source_closure_positive.objc3"
POSITIVE_A002 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_error_bridge_marker_surface_positive.objc3"
TRY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_expression_fail_closed_negative.objc3"
THROW_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_throw_statement_fail_closed_negative.objc3"
DO_CATCH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_do_catch_fail_closed_negative.objc3"
STATUS_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_status_code_attribute_missing_mapping_negative.objc3"
SURFACE_KEY = "objc_part6_error_semantic_model"


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
        failures.append(Finding(display_path(path), "M267-B001-MISSING", f"missing artifact: {display_path(path)}"))
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
        ("M267-B001-COMMON-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M267-B001-COMMON-02", "frontend_dependency_contract_id", "objc3c-part6-error-source-closure/m267-a001-v1", "frontend dependency drifted"),
        ("M267-B001-COMMON-03", "surface_path", "frontend.pipeline.semantic_surface.objc_part6_error_semantic_model", "surface path drifted"),
        ("M267-B001-COMMON-04", "semantic_model", "throws-declaration-semantics-plus-deterministic-result-and-nserror-profile-carriage-are-live-while-try-throw-do-catch-propagation-and-native-error-runtime-behavior-remain-deferred", "semantic model drifted"),
        ("M267-B001-COMMON-05", "deferred_model", "try-throw-do-catch-postfix-propagation-status-to-error-execution-bridge-temporaries-and-native-thrown-error-abi-remain-fail-closed-or-later-lane-work", "deferred model drifted"),
    ]
    for check_id, field, expected, detail in expected_common:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M267-B001-COMMON-06", "source_dependency_required"),
        ("M267-B001-COMMON-07", "throws_declaration_semantics_landed"),
        ("M267-B001-COMMON-08", "result_carrier_profile_semantics_landed"),
        ("M267-B001-COMMON-09", "ns_error_bridging_profile_semantics_landed"),
        ("M267-B001-COMMON-10", "parser_fail_closed_boundary_required"),
        ("M267-B001-COMMON-11", "parser_fail_closed_boundary_preserved"),
        ("M267-B001-COMMON-12", "propagation_runtime_deferred"),
        ("M267-B001-COMMON-13", "status_to_error_runtime_deferred"),
        ("M267-B001-COMMON-14", "native_error_abi_deferred"),
        ("M267-B001-COMMON-15", "placeholder_throws_summary_carried"),
        ("M267-B001-COMMON-16", "deterministic"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 2
    checks_passed += require(payload.get("ready_for_lowering_and_runtime") is False, artifact, "M267-B001-COMMON-17", "ready_for_lowering_and_runtime must stay false", failures)
    checks_passed += require(payload.get("failure_reason") == "", artifact, "M267-B001-COMMON-18", "failure reason should stay empty", failures)

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M267-B001-COMMON-19", "replay key missing", failures)
    return checks_total, checks_passed


def validate_positive_payload(payload: dict[str, Any], artifact: str, expected: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total, checks_passed = validate_common_payload(payload, artifact, failures)
    for index, (field, value) in enumerate(expected.items(), start=20):
        checks_total += 1
        checks_passed += require(payload.get(field) == value, artifact, f"M267-B001-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    return checks_total, checks_passed


def run_positive_probe(args: argparse.Namespace, fixture: Path, out_dir: Path, expected: dict[str, Any], failures: list[Finding], check_prefix: str) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
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
    checks_passed += require(
        run.returncode == 0 or "runtime-aware import/module frontend closure not ready" in combined,
        display_path(fixture),
        f"{check_prefix}-01",
        f"positive fixture did not reach expected manifest emission boundary: {combined}",
        failures,
    )
    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), f"{check_prefix}-02", "manifest missing", failures)
    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get(SURFACE_KEY, {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_positive_payload(payload, display_path(manifest_path), expected, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    return checks_total, checks_passed, payload


def run_negative_probe(args: argparse.Namespace, fixture: Path, out_dir: Path, expected_snippet: str, failures: list[Finding], check_prefix: str) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
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
    checks_passed += require(run.returncode != 0, display_path(fixture), f"{check_prefix}-01", "negative fixture unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require(expected_snippet in combined, display_path(fixture), f"{check_prefix}-02", f"missing expected diagnostic: {expected_snippet}", failures)
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
        "m267-b001-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-B001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M267-B001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M267-B001-DYN-02", "frontend runner missing after build", failures)

    a001_expected = {
        "throws_declaration_sites": 1,
        "function_throws_declaration_sites": 1,
        "method_throws_declaration_sites": 0,
        "result_like_sites": 7,
        "result_success_sites": 1,
        "result_failure_sites": 2,
        "result_branch_sites": 4,
        "result_payload_sites": 3,
        "ns_error_bridging_sites": 3,
        "ns_error_out_parameter_sites": 1,
        "ns_error_bridge_path_sites": 1,
        "objc_nserror_attribute_sites": 0,
        "objc_status_code_attribute_sites": 0,
        "status_code_success_clause_sites": 0,
        "status_code_error_type_clause_sites": 0,
        "status_code_mapping_clause_sites": 0,
        "bridge_marker_semantics_landed": True,
        "placeholder_throws_propagation_sites": 0,
        "placeholder_unwind_cleanup_sites": 0,
    }
    a002_expected = {
        "throws_declaration_sites": 0,
        "function_throws_declaration_sites": 0,
        "method_throws_declaration_sites": 0,
        "result_like_sites": 2,
        "result_success_sites": 0,
        "result_failure_sites": 1,
        "result_branch_sites": 1,
        "result_payload_sites": 1,
        "ns_error_bridging_sites": 7,
        "ns_error_out_parameter_sites": 3,
        "ns_error_bridge_path_sites": 1,
        "objc_nserror_attribute_sites": 1,
        "objc_status_code_attribute_sites": 1,
        "status_code_success_clause_sites": 1,
        "status_code_error_type_clause_sites": 1,
        "status_code_mapping_clause_sites": 1,
        "bridge_marker_semantics_landed": True,
        "placeholder_throws_propagation_sites": 0,
        "placeholder_unwind_cleanup_sites": 0,
    }

    a001_total, a001_passed, a001_payload = run_positive_probe(
        args,
        POSITIVE_A001,
        ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b001" / "a001-positive",
        a001_expected,
        failures,
        "M267-B001-DYN-A001",
    )
    checks_total += a001_total
    checks_passed += a001_passed

    a002_total, a002_passed, a002_payload = run_positive_probe(
        args,
        POSITIVE_A002,
        ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b001" / "a002-positive",
        a002_expected,
        failures,
        "M267-B001-DYN-A002",
    )
    checks_total += a002_total
    checks_passed += a002_passed

    for fixture, case_id, snippet in [
        (TRY_FIXTURE, "try", "unsupported 'try' expression [O3P268]"),
        (THROW_FIXTURE, "throw", "unsupported 'throw' statement [O3P267]"),
        (DO_CATCH_FIXTURE, "do-catch", "unsupported 'do/catch' statement [O3P269]"),
        (STATUS_NEGATIVE, "status-mapping", "objc_status_code clause value must not be empty [O3P274]"),
    ]:
        sub_total, sub_passed = run_negative_probe(
            args,
            fixture,
            ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b001" / case_id,
            snippet,
            failures,
            f"M267-B001-DYN-{case_id.upper()}",
        )
        checks_total += sub_total
        checks_passed += sub_passed

    return checks_total, checks_passed, {
        "a001_payload": a001_payload,
        "a002_payload": a002_payload,
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_checks: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M267-B001-STATIC-01", "Contract ID: `objc3c-part6-error-semantic-model/m267-b001-v1`"),
            SnippetCheck("M267-B001-STATIC-02", "`frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M267-B001-STATIC-03", "Freeze the truthful Part 6 semantic boundary as implemented today"),
            SnippetCheck("M267-B001-STATIC-04", "`frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M267-B001-STATIC-05", "kObjc3Part6ErrorSemanticModelContractId"),
            SnippetCheck("M267-B001-STATIC-06", "struct Objc3Part6ErrorSemanticModelSummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M267-B001-STATIC-07", "BuildPart6ErrorSemanticModelSummary("),
        ],
        SEMA_CPP: [
            SnippetCheck("M267-B001-STATIC-08", "BuildPart6ErrorSemanticModelSummary("),
            SnippetCheck("M267-B001-STATIC-09", "summary.propagation_runtime_deferred = true;"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M267-B001-STATIC-10", "part6_error_semantic_model_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M267-B001-STATIC-11", "result.part6_error_semantic_model_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M267-B001-STATIC-12", "BuildPart6ErrorSemanticModelSummaryJson("),
            SnippetCheck("M267-B001-STATIC-13", '\\\"objc_part6_error_semantic_model\\\":'),
        ],
        DOC_SEMANTICS: [
            SnippetCheck("M267-B001-STATIC-14", "## M267 current Part 6 semantic boundary"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M267-B001-STATIC-15", "## M267 current Part 6 semantic boundary"),
        ],
        SPEC_AM: [
            SnippetCheck("M267-B001-STATIC-16", "M267-B001 semantic-boundary note:"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M267-B001-STATIC-17", "## M267 current Part 6 semantic boundary"),
        ],
        SPEC_PART6: [
            SnippetCheck("M267-B001-STATIC-18", "## M267 current implementation semantic boundary"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M267-B001-STATIC-19", 'check:objc3c:m267-b001-error-carrier-and-propagation-semantic-model-contract-and-architecture-freeze'),
            SnippetCheck("M267-B001-STATIC-20", 'check:objc3c:m267-b001-lane-b-readiness'),
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
