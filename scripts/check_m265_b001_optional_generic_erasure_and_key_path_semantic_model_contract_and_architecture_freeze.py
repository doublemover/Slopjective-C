#!/usr/bin/env python3
"""Checker for M265-B001 Part 3 semantic model freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m265-b001-part3-semantic-model-v1"
CONTRACT_ID = "objc3c-part3-type-semantic-model/m265-b001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part3_type_semantic_model"
SEMANTIC_MODEL = (
    "optional-bindings-optional-sends-erased-generic-metadata-and-typed-keypath-shape-"
    "obey-one-fail-closed-sema-model-before-lowering"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-B001" / "optional_generic_erasure_keypath_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_b001_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART3 = ROOT / "spec" / "PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_OPTIONALS = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_binding_send_coalescing_keypath_positive.objc3"
POSITIVE_GENERIC = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_part3_type_source_closure_positive.objc3"
NEG_BINDING = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_binding_non_objc_source_fail_closed.objc3"
NEG_SEND = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_send_non_objc_receiver_fail_closed.objc3"
NEG_KEYPATH = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_missing_root_fail_closed.objc3"


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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M265-B001-MISSING", f"missing artifact: {display_path(path)}"))
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


def manifest_semantic_packet(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface in {display_path(manifest_path)}")
    packet = semantic_surface.get("objc_part3_type_semantic_model")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def validate_packet(packet: dict[str, Any], artifact: str, expected: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    scalar_expectations = {
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "semantic_model": SEMANTIC_MODEL,
        **expected,
    }
    for key, value in scalar_expectations.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == value, artifact, f"M265-B001-PKT-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(packet.get("deterministic") is True, artifact, "M265-B001-PKT-deterministic", "deterministic must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("ready_for_lowering_and_runtime") is True, artifact, "M265-B001-PKT-ready", "ready_for_lowering_and_runtime must be true", failures)
    checks_total += 1
    checks_passed += require(CONTRACT_ID in str(packet.get("replay_key", "")), artifact, "M265-B001-PKT-replay", "replay key missing contract id", failures)
    return checks_total, checks_passed


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-B001-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-B001-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def run_source_only_case(runner_exe: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(runner_exe),
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

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m265-b001-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-B001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-B001-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-B001-DYN-runner", "frontend runner missing after build", failures)

    optionals_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b001" / "positive-optionals"
    optionals_run = run_source_only_case(args.runner_exe, POSITIVE_OPTIONALS, optionals_out)
    checks_total += 1
    checks_passed += require(optionals_run.returncode == 0, display_path(POSITIVE_OPTIONALS), "M265-B001-DYN-pos-optionals", f"positive optionals case failed: {optionals_run.stderr or optionals_run.stdout}", failures)
    optionals_manifest = optionals_out / "module.manifest.json"
    optionals_diag = optionals_out / "module.diagnostics.json"
    checks_total += 1
    checks_passed += require(optionals_manifest.exists(), display_path(optionals_manifest), "M265-B001-DYN-pos-optionals-manifest", "positive optionals manifest missing", failures)
    checks_total += 1
    checks_passed += require(optionals_diag.exists(), display_path(optionals_diag), "M265-B001-DYN-pos-optionals-diag", "positive optionals diagnostics missing", failures)
    if optionals_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(optionals_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    optionals_packet: dict[str, Any] = {}
    if optionals_manifest.exists():
        optionals_packet = manifest_semantic_packet(optionals_manifest)
        sub_total, sub_passed = validate_packet(optionals_packet, display_path(optionals_manifest), {
            "optional_binding_sites": 2,
            "optional_binding_clause_sites": 2,
            "guard_binding_sites": 1,
            "optional_send_sites": 1,
            "nil_coalescing_sites": 1,
            "typed_keypath_literal_sites": 1,
            "typed_keypath_self_root_sites": 1,
            "generic_erasure_semantic_sites": 0,
            "nullability_semantic_sites": 1,
            "optional_binding_contract_violation_sites": 0,
            "optional_send_contract_violation_sites": 0,
            "typed_keypath_contract_violation_sites": 0,
        }, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    generic_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b001" / "positive-generic"
    generic_run = run_source_only_case(args.runner_exe, POSITIVE_GENERIC, generic_out)
    checks_total += 1
    checks_passed += require(generic_run.returncode == 0, display_path(POSITIVE_GENERIC), "M265-B001-DYN-pos-generic", f"positive generic case failed: {generic_run.stderr or generic_run.stdout}", failures)
    generic_manifest = generic_out / "module.manifest.json"
    generic_diag = generic_out / "module.diagnostics.json"
    checks_total += 1
    checks_passed += require(generic_manifest.exists(), display_path(generic_manifest), "M265-B001-DYN-pos-generic-manifest", "positive generic manifest missing", failures)
    checks_total += 1
    checks_passed += require(generic_diag.exists(), display_path(generic_diag), "M265-B001-DYN-pos-generic-diag", "positive generic diagnostics missing", failures)
    if generic_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(generic_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    generic_packet: dict[str, Any] = {}
    if generic_manifest.exists():
        generic_packet = manifest_semantic_packet(generic_manifest)
        sub_total, sub_passed = validate_packet(generic_packet, display_path(generic_manifest), {
            "optional_binding_sites": 0,
            "optional_binding_clause_sites": 0,
            "guard_binding_sites": 0,
            "optional_send_sites": 0,
            "nil_coalescing_sites": 0,
            "typed_keypath_literal_sites": 0,
            "typed_keypath_self_root_sites": 0,
            "generic_erasure_semantic_sites": 3,
            "nullability_semantic_sites": 3,
            "optional_binding_contract_violation_sites": 0,
            "optional_send_contract_violation_sites": 0,
            "typed_keypath_contract_violation_sites": 0,
        }, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    negative_specs = [
        ("binding-non-objc", NEG_BINDING, "type mismatch: optional binding requires an optional ObjC-reference-compatible source"),
        ("optional-send-non-objc", NEG_SEND, "type mismatch: optional send receiver for selector 'description' must be ObjC-reference-compatible"),
        ("typed-keypath-missing-root", NEG_KEYPATH, "type mismatch: typed key-path root 'missingRoot' must resolve to 'self', a known class type, or an ObjC-reference-compatible identifier"),
    ]
    negatives: list[dict[str, Any]] = []
    for case_id, fixture, expected_message in negative_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b001" / case_id
        completed = run_source_only_case(args.runner_exe, fixture, out_dir)
        diag_path = out_dir / "module.diagnostics.json"
        checks_total += 1
        checks_passed += require(completed.returncode != 0, display_path(fixture), f"M265-B001-DYN-neg-{case_id}-rc", "negative case unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(diag_path.exists(), display_path(diag_path), f"M265-B001-DYN-neg-{case_id}-diag", "negative diagnostics missing", failures)
        if diag_path.exists():
            diagnostics_payload = load_json(diag_path)
            raw_diags = diagnostics_payload.get("diagnostics")
            messages = [str(item.get("message", "")) for item in raw_diags if isinstance(item, dict)] if isinstance(raw_diags, list) else []
            checks_total += 1
            checks_passed += require(any(expected_message in message for message in messages), display_path(diag_path), f"M265-B001-DYN-neg-{case_id}-msg", f"expected diagnostic missing: {expected_message}", failures)
        negatives.append({
            "case_id": case_id,
            "fixture": display_path(fixture),
            "diagnostics_path": display_path(diag_path),
            "expected_message": expected_message,
        })

    dynamic = {
        "positive_optionals_fixture": display_path(POSITIVE_OPTIONALS),
        "positive_optionals_packet": optionals_packet,
        "positive_generic_fixture": display_path(POSITIVE_GENERIC),
        "positive_generic_packet": generic_packet,
        "negative_cases": negatives,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_targets: list[tuple[Path, list[SnippetCheck]]] = [
        (EXPECTATIONS_DOC, [SnippetCheck("M265-B001-DOC-01", CONTRACT_ID), SnippetCheck("M265-B001-DOC-02", SURFACE_PATH)]),
        (PACKET_DOC, [SnippetCheck("M265-B001-PKT-01", "semantic model"), SnippetCheck("M265-B001-PKT-02", SURFACE_PATH), SnippetCheck("M265-B001-PKT-03", "source-only sema-negative probes")]),
        (AST_HEADER, [SnippetCheck("M265-B001-AST-01", "optional_binding_surface_enabled"), SnippetCheck("M265-B001-AST-02", "guard_binding_surface_enabled"), SnippetCheck("M265-B001-AST-03", "optional_binding_clause_count")]),
        (PARSER_CPP, [SnippetCheck("M265-B001-PARSE-01", "stmt->if_stmt->optional_binding_surface_enabled = true;"), SnippetCheck("M265-B001-PARSE-02", "stmt->if_stmt->guard_binding_surface_enabled = true;"), SnippetCheck("M265-B001-PARSE-03", "stmt->if_stmt->then_body.push_back(std::move(synthetic));")]),
        (SEMA_CONTRACT_H, [SnippetCheck("M265-B001-SEMA-01", "struct Objc3Part3TypeSemanticModelSummary"), SnippetCheck("M265-B001-SEMA-02", "optional_binding_sites"), SnippetCheck("M265-B001-SEMA-03", "typed_keypath_contract_violation_sites")]),
        (SEMA_PASSES_H, [SnippetCheck("M265-B001-SEMAH-01", "BuildPart3TypeSemanticModelSummary")]),
        (SEMA_PASSES_CPP, [SnippetCheck("M265-B001-SEMACPP-01", "typed key-path root '"), SnippetCheck("M265-B001-SEMACPP-02", "optional binding requires an optional ObjC-reference-compatible source"), SnippetCheck("M265-B001-SEMACPP-03", "optional send receiver for selector '"), SnippetCheck("M265-B001-SEMACPP-04", "BuildPart3TypeSemanticModelSummary(")]),
        (FRONTEND_TYPES_H, [SnippetCheck("M265-B001-FRONTEND-01", "kObjc3Part3TypeSemanticModelContractId"), SnippetCheck("M265-B001-FRONTEND-02", "kObjc3Part3TypeSemanticModelSurfacePath")]),
        (FRONTEND_ARTIFACTS_CPP, [SnippetCheck("M265-B001-ARTIFACTS-01", "BuildPart3TypeSemanticModelSummaryJson"), SnippetCheck("M265-B001-ARTIFACTS-02", "objc_part3_type_semantic_model"), SnippetCheck("M265-B001-ARTIFACTS-03", "BuildPart3TypeSemanticModelSummary(")]),
        (DOC_SOURCE, [SnippetCheck("M265-B001-DOCSRC-01", "## M265 Part 3 semantic model"), SnippetCheck("M265-B001-DOCSRC-02", "Lane B now carries the live semantic/refinement slice")]),
        (DOC_NATIVE, [SnippetCheck("M265-B001-DOCNATIVE-01", "## M265 Part 3 semantic model"), SnippetCheck("M265-B001-DOCNATIVE-02", "optional sends now fail closed for non-ObjC-reference receivers")]),
        (SPEC_AM, [SnippetCheck("M265-B001-AM-01", "Lane B now carries live optional-flow semantics for optional bindings"), SnippetCheck("M265-B001-AM-02", "Typed key-path roots now fail closed unless they resolve to `self`, a known")]),
        (SPEC_ATTR, [SnippetCheck("M265-B001-ATTR-01", "Current implementation status (`M265-B003`)"), SnippetCheck("M265-B001-ATTR-02", "now sema-validated against admitted nullable ObjC-reference sources")]),
        (SPEC_PART3, [SnippetCheck("M265-B001-PART3-01", "Implementation note (`M265-B003`)"), SnippetCheck("M265-B001-PART3-02", "Typed key-path roots currently fail closed")]),
        (PACKAGE_JSON, [SnippetCheck("M265-B001-PKG-01", "check:objc3c:m265-b001-optional-generic-erasure-and-key-path-semantic-model-contract-and-architecture-freeze"), SnippetCheck("M265-B001-PKG-02", "test:tooling:m265-b001-optional-generic-erasure-and-key-path-semantic-model-contract-and-architecture-freeze"), SnippetCheck("M265-B001-PKG-03", "check:objc3c:m265-b001-lane-b-readiness")]),
    ]

    for path, snippets in snippet_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_payload["skipped"] = True
    else:
        dynamic_total, dynamic_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(f"[ok] M265-B001 semantic model verified -> {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
