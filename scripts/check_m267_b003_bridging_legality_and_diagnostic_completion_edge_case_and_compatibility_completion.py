#!/usr/bin/env python3
"""Checker for M267-B003 Part 6 bridge legality semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-b003-error-bridge-legality-v1"
CONTRACT_ID = "objc3c-part6-error-bridge-legality/m267-b003-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m267" / "M267-B003" / "bridging_legality_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion_packet.md"
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
POSITIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_positive.objc3"
NEGATIVES: list[tuple[Path, str]] = [
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_nserror_missing_out_negative.objc3", "objc_nserror requires an NSError out parameter [O3S275]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_nserror_bad_return_negative.objc3", "objc_nserror currently requires a BOOL-like success return [O3S276]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_throws_conflict_negative.objc3", "NSError/status bridge markers cannot currently be combined with throws [O3S277]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_marker_conflict_negative.objc3", "objc_nserror and objc_status_code cannot appear on the same callable [O3S278]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_bad_error_type_negative.objc3", "objc_status_code currently requires error_type: NSError [O3S280]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_missing_mapping_negative.objc3", "objc_status_code mapping symbol must resolve to a declared function [O3S282]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_bad_mapping_signature_negative.objc3", "objc_status_code mapping function must accept one matching status parameter and return NSError [O3S283]"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_bad_status_return_negative.objc3", "objc_status_code requires a BOOL-like or integer status return [O3S281]"),
]
NATIVE_FAIL_CLOSED = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_native_fail_closed.objc3"
SURFACE_KEY = "objc_part6_error_bridge_legality"
TRY_SURFACE_KEY = "objc_part6_try_do_catch_semantics"


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
        failures.append(Finding(display_path(path), "M267-B003-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: list[tuple[str, str, Any, str]] = [
        ("M267-B003-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M267-B003-PAYLOAD-02", "dependency_contract_id", "objc3c-part6-try-throw-do-catch-semantics/m267-b002-v1", "dependency contract drifted"),
        ("M267-B003-PAYLOAD-03", "surface_path", "frontend.pipeline.semantic_surface.objc_part6_error_bridge_legality", "surface path drifted"),
        ("M267-B003-PAYLOAD-04", "bridge_callable_sites", 2, "bridge callable count mismatch"),
        ("M267-B003-PAYLOAD-05", "objc_nserror_callable_sites", 1, "objc_nserror count mismatch"),
        ("M267-B003-PAYLOAD-06", "objc_status_code_callable_sites", 1, "objc_status_code count mismatch"),
        ("M267-B003-PAYLOAD-07", "semantically_valid_bridge_callable_sites", 2, "valid bridge count mismatch"),
        ("M267-B003-PAYLOAD-08", "try_eligible_bridge_callable_sites", 2, "try-eligible bridge count mismatch"),
        ("M267-B003-PAYLOAD-09", "missing_error_out_parameter_sites", 0, "unexpected missing NSError out parameter count"),
        ("M267-B003-PAYLOAD-10", "invalid_nserror_return_sites", 0, "unexpected objc_nserror return violation count"),
        ("M267-B003-PAYLOAD-11", "invalid_status_return_sites", 0, "unexpected status return violation count"),
        ("M267-B003-PAYLOAD-12", "invalid_error_type_sites", 0, "unexpected error_type violation count"),
        ("M267-B003-PAYLOAD-13", "missing_mapping_symbol_sites", 0, "unexpected missing mapping count"),
        ("M267-B003-PAYLOAD-14", "invalid_mapping_signature_sites", 0, "unexpected mapping signature count"),
        ("M267-B003-PAYLOAD-15", "throws_bridge_conflict_sites", 0, "unexpected throws conflict count"),
        ("M267-B003-PAYLOAD-16", "marker_conflict_sites", 0, "unexpected marker conflict count"),
        ("M267-B003-PAYLOAD-17", "unsupported_combination_sites", 0, "unexpected unsupported combination count"),
        ("M267-B003-PAYLOAD-18", "contract_violation_sites", 0, "unexpected contract violation count"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M267-B003-PAYLOAD-19", "source_dependency_required"),
        ("M267-B003-PAYLOAD-20", "bridge_legality_landed"),
        ("M267-B003-PAYLOAD-21", "try_bridge_filter_landed"),
        ("M267-B003-PAYLOAD-22", "unsupported_combinations_fail_closed"),
        ("M267-B003-PAYLOAD-23", "native_emit_remains_fail_closed"),
        ("M267-B003-PAYLOAD-24", "deterministic"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 1
    checks_passed += require(payload.get("ready_for_lowering_and_runtime") is False, artifact, "M267-B003-PAYLOAD-25", "ready_for_lowering_and_runtime must stay false", failures)
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
        "m267-b003-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-B003/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M267-B003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M267-B003-DYN-02", "frontend runner missing after build", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b003" / "positive"
    positive_out.mkdir(parents=True, exist_ok=True)
    positive = run_command([
        str(args.runner_exe),
        str(POSITIVE),
        "--out-dir",
        str(positive_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    positive_output = (positive.stdout or "") + (positive.stderr or "")
    checks_total += 1
    checks_passed += require(
        positive.returncode == 0 or "runtime-aware import/module frontend closure not ready" in positive_output,
        display_path(POSITIVE),
        "M267-B003-DYN-03",
        f"positive fixture did not reach expected manifest boundary: {positive_output}",
        failures,
    )
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M267-B003-DYN-04", "positive manifest missing", failures)

    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get(SURFACE_KEY, {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_positive_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed

        try_payload = semantic_surface.get(TRY_SURFACE_KEY, {})
        checks_total += 1
        checks_passed += require(
            isinstance(try_payload, dict) and try_payload.get("bridged_callable_try_sites") == 1,
            display_path(manifest_path),
            "M267-B003-DYN-05",
            "B002 try surface did not preserve bridged callable count after legality filtering",
            failures,
        )

    for index, (fixture, expected_diag) in enumerate(NEGATIVES, start=6):
        negative_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b003" / fixture.stem
        negative_out.mkdir(parents=True, exist_ok=True)
        negative = run_command([
            str(args.runner_exe),
            str(fixture),
            "--out-dir",
            str(negative_out),
            "--emit-prefix",
            "module",
            "--no-emit-ir",
            "--no-emit-object",
        ])
        negative_output = (negative.stdout or "") + (negative.stderr or "")
        checks_total += 1
        checks_passed += require(negative.returncode != 0, display_path(fixture), f"M267-B003-DYN-{index:02d}A", "negative fixture unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(expected_diag in negative_output, display_path(fixture), f"M267-B003-DYN-{index:02d}B", f"missing expected diagnostic: {expected_diag}", failures)

    native_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "b003" / "native_fail_closed"
    native_out.mkdir(parents=True, exist_ok=True)
    native = run_command([
        str(args.runner_exe),
        str(NATIVE_FAIL_CLOSED),
        "--out-dir",
        str(native_out),
        "--emit-prefix",
        "module",
    ])
    native_output = (native.stdout or "") + (native.stderr or "")
    checks_total += 1
    checks_passed += require(native.returncode != 0, display_path(NATIVE_FAIL_CLOSED), "M267-B003-DYN-20A", "native fail-closed fixture unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require("unsupported feature claim: NSError/status bridge legality is not yet runnable in Objective-C 3 native mode [O3S285]" in native_output, display_path(NATIVE_FAIL_CLOSED), "M267-B003-DYN-20B", "missing native fail-closed bridge diagnostic", failures)

    return checks_total, checks_passed, {"positive_payload": payload}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_snippets: list[tuple[Path, list[SnippetCheck]]] = [
        (EXPECTATIONS_DOC, [
            SnippetCheck("M267-B003-EXP-01", "# M267 Bridging Legality And Diagnostic Completion Expectations (B003)"),
            SnippetCheck("M267-B003-EXP-02", "objc_status_code mapping function must accept one matching status parameter and return `NSError`"),
        ]),
        (PACKET_DOC, [
            SnippetCheck("M267-B003-PKT-01", "# M267-B003 Packet: Bridging Legality And Diagnostic Completion"),
            SnippetCheck("M267-B003-PKT-02", "only semantically valid bridge call surfaces qualify for `try`"),
        ]),
        (DOC_SEMANTICS, [
            SnippetCheck("M267-B003-DOCS-01", "## M267 current Part 6 bridge legality boundary"),
            SnippetCheck("M267-B003-DOCS-02", "only semantically valid bridge call surfaces qualify for `try`"),
        ]),
        (SPEC_AM, [
            SnippetCheck("M267-B003-AM-01", "M267-B003 bridge legality note:"),
        ]),
        (SPEC_ATTR, [
            SnippetCheck("M267-B003-ATTR-01", "Current implementation status (`M267-B003`):"),
        ]),
        (SPEC_PART6, [
            SnippetCheck("M267-B003-P6-01", "Current implementation status (`M267-B003`):"),
        ]),
        (SEMA_CONTRACT, [
            SnippetCheck("M267-B003-SEMA-01", "kObjc3Part6ErrorBridgeLegalitySummaryContractId"),
            SnippetCheck("M267-B003-SEMA-02", "struct Objc3Part6ErrorBridgeLegalitySummary"),
        ]),
        (SEMA_HEADER, [
            SnippetCheck("M267-B003-H-01", "BuildPart6ErrorBridgeLegalitySummary("),
        ]),
        (SEMA_CPP, [
            SnippetCheck("M267-B003-CPP-01", "CallableHasSemanticallyValidNSErrorStatusBridge("),
            SnippetCheck("M267-B003-CPP-02", "objc_status_code mapping function must accept one matching status parameter and return NSError"),
            SnippetCheck("M267-B003-CPP-03", "unsupported feature claim: NSError/status bridge legality is not yet runnable in Objective-C 3 native mode"),
        ]),
        (FRONTEND_TYPES, [
            SnippetCheck("M267-B003-TYP-01", "Objc3Part6ErrorBridgeLegalitySummary part6_error_bridge_legality_summary;"),
        ]),
        (FRONTEND_PIPELINE, [
            SnippetCheck("M267-B003-PIPE-01", "result.part6_error_bridge_legality_summary ="),
        ]),
        (FRONTEND_ARTIFACTS, [
            SnippetCheck("M267-B003-ART-01", '"objc_part6_error_bridge_legality":'),
            SnippetCheck("M267-B003-ART-02", "BuildPart6ErrorBridgeLegalitySummaryJson("),
        ]),
        (PACKAGE_JSON, [
            SnippetCheck("M267-B003-PKG-01", '"check:objc3c:m267-b003-bridging-legality-and-diagnostic-completion-edge-case-and-compatibility-completion"'),
            SnippetCheck("M267-B003-PKG-02", '"check:objc3c:m267-b003-lane-b-readiness"'),
        ]),
    ]

    for path, snippets in static_snippets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

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
    args.summary_out.write_text(json.dumps(summary, indent=2), encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
