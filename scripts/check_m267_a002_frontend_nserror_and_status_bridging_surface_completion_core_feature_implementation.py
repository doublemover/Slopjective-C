#!/usr/bin/env python3
"""Checker for M267-A002 canonical error bridge marker frontend completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-a002-error-bridge-marker-source-v1"
CONTRACT_ID = "objc3c-part6-error-bridge-markers/m267-a002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m267" / "M267-A002" / "error_bridge_marker_surface_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_frontend_nserror_and_status_bridging_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_a002_frontend_nserror_and_status_bridging_surface_completion_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_error_bridge_marker_surface_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_status_code_attribute_missing_mapping_negative.objc3"

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
        failures.append(Finding(display_path(path), "M267-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
        ("M267-A002-PAYLOAD-01", "contract_id", "objc3c-part6-error-source-closure/m267-a001-v1", "upstream Part 6 contract id drifted"),
        ("M267-A002-PAYLOAD-02", "frontend_surface_path", "frontend.pipeline.semantic_surface.objc_part6_error_source_closure", "surface path drifted"),
        ("M267-A002-PAYLOAD-03", "source_only_claim_ids", EXPECTED_SOURCE_ONLY, "source-only claim ids drifted"),
        ("M267-A002-PAYLOAD-04", "fail_closed_construct_ids", EXPECTED_FAIL_CLOSED, "fail-closed construct ids drifted"),
        ("M267-A002-PAYLOAD-05", "function_throws_declaration_sites", 0, "function throws count mismatch"),
        ("M267-A002-PAYLOAD-06", "method_throws_declaration_sites", 0, "method throws count mismatch"),
        ("M267-A002-PAYLOAD-07", "result_like_sites", 2, "result-like site count mismatch"),
        ("M267-A002-PAYLOAD-08", "result_success_sites", 0, "result success count mismatch"),
        ("M267-A002-PAYLOAD-09", "result_failure_sites", 1, "result failure count mismatch"),
        ("M267-A002-PAYLOAD-10", "result_branch_sites", 1, "result branch count mismatch"),
        ("M267-A002-PAYLOAD-11", "result_payload_sites", 1, "result payload count mismatch"),
        ("M267-A002-PAYLOAD-12", "ns_error_bridging_sites", 5, "NSError bridging count mismatch"),
        ("M267-A002-PAYLOAD-13", "ns_error_out_parameter_sites", 2, "NSError out-param count mismatch"),
        ("M267-A002-PAYLOAD-14", "ns_error_bridge_path_sites", 1, "NSError bridge path count mismatch"),
        ("M267-A002-PAYLOAD-15", "objc_nserror_attribute_sites", 1, "objc_nserror count mismatch"),
        ("M267-A002-PAYLOAD-16", "objc_status_code_attribute_sites", 1, "objc_status_code count mismatch"),
        ("M267-A002-PAYLOAD-17", "status_code_success_clause_sites", 1, "status_code success count mismatch"),
        ("M267-A002-PAYLOAD-18", "status_code_error_type_clause_sites", 1, "status_code error_type count mismatch"),
        ("M267-A002-PAYLOAD-19", "status_code_mapping_clause_sites", 1, "status_code mapping count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M267-A002-PAYLOAD-20", "throws_declaration_source_supported"),
        ("M267-A002-PAYLOAD-21", "result_carrier_source_supported"),
        ("M267-A002-PAYLOAD-22", "ns_error_bridging_source_supported"),
        ("M267-A002-PAYLOAD-23", "error_bridge_marker_source_supported"),
        ("M267-A002-PAYLOAD-24", "try_fail_closed"),
        ("M267-A002-PAYLOAD-25", "throw_fail_closed"),
        ("M267-A002-PAYLOAD-26", "do_catch_fail_closed"),
        ("M267-A002-PAYLOAD-27", "deterministic_handoff"),
        ("M267-A002-PAYLOAD-28", "ready_for_semantic_expansion"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 1
    checks_passed += require(
        "bridge_marker_sites=1:1:1:1:1" in str(payload.get("replay_key", "")),
        artifact,
        "M267-A002-PAYLOAD-29",
        "bridge marker replay key shape drifted",
        failures,
    )
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
        "m267-a002-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-A002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M267-A002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M267-A002-DYN-02", "frontend runner missing after build", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "a002" / "positive"
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
        "M267-A002-DYN-03",
        f"positive fixture did not reach the expected manifest-emission boundary: {positive_output}",
        failures,
    )
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M267-A002-DYN-04", "positive manifest missing", failures)

    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get("objc_part6_error_source_closure", {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_summary_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed

    negative_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "a002" / "missing-mapping"
    negative_out.mkdir(parents=True, exist_ok=True)
    negative = run_command([
        str(args.runner_exe),
        str(NEGATIVE_FIXTURE),
        "--out-dir",
        str(negative_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    negative_output = (negative.stdout or "") + (negative.stderr or "")
    checks_total += 1
    checks_passed += require(negative.returncode != 0, display_path(NEGATIVE_FIXTURE), "M267-A002-DYN-05", "negative fixture unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require(
        "objc_status_code clause value must not be empty [O3P274]" in negative_output,
        display_path(NEGATIVE_FIXTURE),
        "M267-A002-DYN-06",
        "expected O3P274 diagnostic missing",
        failures,
    )

    return checks_total, checks_passed, payload


def static_checks(failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    snippets_by_file: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M267-A002-EXP-01", "# M267 Frontend NSError And Status Bridging Surface Completion Expectations (A002)"),
            SnippetCheck("M267-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M267-A002-EXP-03", "`__attribute__((objc_nserror))`"),
            SnippetCheck("M267-A002-EXP-04", "`objc_status_code clause value must not be empty [O3P274]`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M267-A002-PKT-01", "# M267-A002 Packet: Frontend NSError And Status Bridging Surface Completion"),
            SnippetCheck("M267-A002-PKT-02", "Complete the truthful frontend/source-model surface for canonical Part 6 NSError and status bridging markers"),
            SnippetCheck("M267-A002-PKT-03", "`native/objc3c/src/parse/objc3_parser.cpp`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M267-A002-GRM-01", "## M267 frontend canonical bridge-marker completion"),
            SnippetCheck("M267-A002-GRM-02", "`__attribute__((objc_nserror))`"),
            SnippetCheck("M267-A002-GRM-03", "malformed `objc_status_code(...)` payloads fail closed in the parser"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M267-A002-DOC-01", "## M267 frontend canonical bridge-marker completion"),
            SnippetCheck("M267-A002-DOC-02", "`__attribute__((objc_status_code(success: ..., error_type: ..., mapping: ...)))`"),
        ],
        SPEC_AM: [
            SnippetCheck("M267-A002-AM-01", "M267-A002 source-closure note:"),
            SnippetCheck("M267-A002-AM-02", "canonical `objc_nserror` / `objc_status_code(...)` declaration markers"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M267-A002-ATTR-01", "## M267 current canonical error bridge-marker frontend boundary"),
            SnippetCheck("M267-A002-ATTR-02", "`__attribute__((objc_status_code(success: ..., error_type: ..., mapping: ...)))`"),
        ],
        AST_HEADER: [
            SnippetCheck("M267-A002-AST-01", "bool objc_nserror_declared = false;"),
            SnippetCheck("M267-A002-AST-02", "std::string error_bridge_marker_profile;"),
            SnippetCheck("M267-A002-AST-03", "std::size_t status_code_mapping_clause_sites = 0;"),
        ],
        PARSER_CPP: [
            SnippetCheck("M267-A002-PARSE-01", "ParseOptionalCallableBridgeAttributes("),
            SnippetCheck("M267-A002-PARSE-02", "FinalizeErrorBridgeMarkerProfile("),
            SnippetCheck("M267-A002-PARSE-03", "\"O3P280\""),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M267-A002-TYP-01", "std::size_t objc_nserror_attribute_sites = 0;"),
            SnippetCheck("M267-A002-TYP-02", "bool error_bridge_marker_source_supported = false;"),
            SnippetCheck("M267-A002-TYP-03", "status_code_mapping_clause_sites"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M267-A002-PIPE-01", "bridge_marker_sites="),
            SnippetCheck("M267-A002-PIPE-02", "summary.error_bridge_marker_source_supported = true;"),
            SnippetCheck("M267-A002-PIPE-03", "summary.objc_status_code_attribute_sites +="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M267-A002-ART-01", "\\\"objc_nserror_attribute_sites\\\""),
            SnippetCheck("M267-A002-ART-02", "\\\"error_bridge_marker_source_supported\\\""),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M267-A002-PKG-01", "\"check:objc3c:m267-a002-frontend-nserror-and-status-bridging-surface-completion-core-feature-implementation\""),
            SnippetCheck("M267-A002-PKG-02", "\"check:objc3c:m267-a002-lane-a-readiness\""),
        ],
    }
    for path, snippets in snippets_by_file.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)
    return checks_total, checks_passed


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_total, static_passed = static_checks(failures)
    checks_total += static_total
    checks_passed += static_passed

    payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dynamic_total, dynamic_passed, payload = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "payload": payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M267-A002 bridge-marker frontend checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
