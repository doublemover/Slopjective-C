#!/usr/bin/env python3
"""Checker for M265-B003 generic erasure and key-path legality completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m265-b003-part3-keypath-generic-legality-v1"
CONTRACT_ID = "objc3c-part3-type-semantic-model/m265-b001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part3_type_semantic_model"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-B003" / "generic_erasure_keypath_legality_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_generic_erasure_and_key_path_legality_completion_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_b003_generic_erasure_and_key_path_legality_completion_edge_case_and_compatibility_completion_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART3 = ROOT / "spec" / "PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_CLASS_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_class_root_positive.objc3"
POSITIVE_GENERIC = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_part3_type_source_closure_positive.objc3"
NEG_INVALID_COMPONENT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_invalid_component_negative.objc3"
NEG_NON_OBJECT_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_non_object_root_negative.objc3"
NEG_NESTED_CHAIN = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_nested_chain_fail_closed.objc3"
NEG_GENERIC_METHOD = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_generic_method_reserved_syntax_negative.objc3"


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
        failures.append(Finding(display_path(path), "M265-B003-MISSING", f"missing artifact: {display_path(path)}"))
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


def semantic_packet_from_manifest(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part3_type_semantic_model")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-B003-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-B003-DIAG-empty", "expected zero diagnostics", failures)
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


def validate_positive_packet(packet: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expectations = {
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "typed_keypath_literal_sites": 1,
        "typed_keypath_class_root_sites": 1,
        "typed_keypath_root_legality_violation_sites": 0,
        "typed_keypath_member_path_contract_violation_sites": 0,
        "typed_keypath_contract_violation_sites": 0,
        "deterministic": True,
        "ready_for_lowering_and_runtime": True,
    }
    for key, value in expectations.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == value, artifact, f"M265-B003-POS-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require("keypath-class=1" in str(packet.get("replay_key", "")), artifact, "M265-B003-POS-replay-01", "replay key missing class-root count", failures)
    checks_total += 1
    checks_passed += require("keypath-member-violations=0" in str(packet.get("replay_key", "")), artifact, "M265-B003-POS-replay-02", "replay key missing member-path count", failures)
    return checks_total, checks_passed


def validate_generic_packet(packet: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(packet.get("generic_erasure_semantic_sites") == 3, artifact, "M265-B003-GEN-01", "generic erasure semantic count mismatch", failures)
    checks_total += 1
    checks_passed += require(packet.get("typed_keypath_literal_sites") == 0, artifact, "M265-B003-GEN-02", "generic positive should not introduce key-path sites", failures)
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
        "m265-b003-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-B003/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-B003-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-B003-DYN-runner", "frontend runner missing after build", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b003" / "positive-class-root"
    positive_run = run_source_only_case(args.runner_exe, POSITIVE_CLASS_ROOT, positive_out)
    checks_total += 1
    checks_passed += require(positive_run.returncode == 0, display_path(POSITIVE_CLASS_ROOT), "M265-B003-DYN-pos-root", f"positive class-root fixture failed: {positive_run.stderr or positive_run.stdout}", failures)
    positive_manifest = positive_out / "module.manifest.json"
    positive_diag = positive_out / "module.diagnostics.json"
    checks_total += 1
    checks_passed += require(positive_manifest.exists(), display_path(positive_manifest), "M265-B003-DYN-pos-root-manifest", "positive manifest missing", failures)
    checks_total += 1
    checks_passed += require(positive_diag.exists(), display_path(positive_diag), "M265-B003-DYN-pos-root-diag", "positive diagnostics missing", failures)
    if positive_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(positive_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    positive_packet: dict[str, Any] = {}
    if positive_manifest.exists():
        positive_packet = semantic_packet_from_manifest(positive_manifest)
        sub_total, sub_passed = validate_positive_packet(positive_packet, display_path(positive_manifest), failures)
        checks_total += sub_total
        checks_passed += sub_passed

    generic_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b003" / "positive-generic"
    generic_run = run_source_only_case(args.runner_exe, POSITIVE_GENERIC, generic_out)
    checks_total += 1
    checks_passed += require(generic_run.returncode == 0, display_path(POSITIVE_GENERIC), "M265-B003-DYN-pos-generic", f"positive generic fixture failed: {generic_run.stderr or generic_run.stdout}", failures)
    generic_manifest = generic_out / "module.manifest.json"
    if generic_manifest.exists():
        generic_packet = semantic_packet_from_manifest(generic_manifest)
        sub_total, sub_passed = validate_generic_packet(generic_packet, display_path(generic_manifest), failures)
        checks_total += sub_total
        checks_passed += sub_passed
    else:
        checks_total += 1
        checks_passed += require(False, display_path(generic_manifest), "M265-B003-DYN-pos-generic-manifest", "generic positive manifest missing", failures)

    negative_specs = [
        ("generic-method", NEG_GENERIC_METHOD, "generic Objective-C method declarations are reserved for a future Objective-C 3 revision"),
        ("invalid-component", NEG_INVALID_COMPONENT, "type mismatch: typed key-path component 'missing' is not a readable property on root 'Person'"),
        ("non-object-root", NEG_NON_OBJECT_ROOT, "type mismatch: typed key-path root 'count' must resolve to 'self', a known class type, or an ObjC-reference-compatible identifier"),
        ("nested-chain", NEG_NESTED_CHAIN, "type mismatch: typed key-path member chain 'address.street' is unsupported until executable typed key-path lowering is enabled"),
    ]
    negatives: list[dict[str, Any]] = []
    for case_id, fixture, expected_message in negative_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "b003" / case_id
        completed = run_source_only_case(args.runner_exe, fixture, out_dir)
        diag_path = out_dir / "module.diagnostics.json"
        checks_total += 1
        checks_passed += require(completed.returncode != 0, display_path(fixture), f"M265-B003-DYN-neg-{case_id}-rc", "negative case unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(diag_path.exists(), display_path(diag_path), f"M265-B003-DYN-neg-{case_id}-diag", "negative diagnostics missing", failures)
        if diag_path.exists():
            payload = load_json(diag_path)
            raw = payload.get("diagnostics")
            messages = [str(item.get("message", "")) for item in raw if isinstance(item, dict)] if isinstance(raw, list) else []
            checks_total += 1
            checks_passed += require(any(expected_message in message for message in messages), display_path(diag_path), f"M265-B003-DYN-neg-{case_id}-msg", f"expected diagnostic missing: {expected_message}", failures)
        negatives.append({
            "case_id": case_id,
            "fixture": display_path(fixture),
            "diagnostics_path": display_path(diag_path),
            "expected_message": expected_message,
        })

    dynamic = {
        "positive_class_root_fixture": display_path(POSITIVE_CLASS_ROOT),
        "positive_class_root_packet": positive_packet,
        "positive_generic_fixture": display_path(POSITIVE_GENERIC),
        "negative_cases": negatives,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_targets: list[tuple[Path, list[SnippetCheck]]] = [
        (EXPECTATIONS_DOC, [SnippetCheck("M265-B003-EXP-01", "objc3c-part3-type-semantic-model/m265-b001-v1"), SnippetCheck("M265-B003-EXP-02", "class-root key path")]),
        (PACKET_DOC, [SnippetCheck("M265-B003-PKT-01", "reserved generic Objective-C method syntax"), SnippetCheck("M265-B003-PKT-02", "multi-component typed key-path member chains")]),
        (PARSER_CPP, [SnippetCheck("M265-B003-PARSE-01", "generic Objective-C method declarations are reserved for a future Objective-C 3 revision")]),
        (SEMA_CONTRACT_H, [SnippetCheck("M265-B003-SEMA-01", "typed_keypath_class_root_sites"), SnippetCheck("M265-B003-SEMA-02", "typed_keypath_root_legality_violation_sites"), SnippetCheck("M265-B003-SEMA-03", "typed_keypath_member_path_contract_violation_sites")]),
        (SEMA_PASSES_CPP, [SnippetCheck("M265-B003-SEMACPP-01", "typed key-path component '"), SnippetCheck("M265-B003-SEMACPP-02", "typed key-path member chain '"), SnippetCheck("M265-B003-SEMACPP-03", "typed_keypath_class_root_sites")]),
        (FRONTEND_ARTIFACTS_CPP, [SnippetCheck("M265-B003-ART-01", "typed_keypath_class_root_sites"), SnippetCheck("M265-B003-ART-02", "typed_keypath_root_legality_violation_sites"), SnippetCheck("M265-B003-ART-03", "typed_keypath_member_path_contract_violation_sites")]),
        (DOC_SOURCE, [SnippetCheck("M265-B003-DOCSRC-01", "single-component class-root key paths such as `@keypath(Person, name)`"), SnippetCheck("M265-B003-DOCSRC-02", "generic Objective-C method declarations written as `- <T> ...` remain")]),
        (DOC_NATIVE, [SnippetCheck("M265-B003-DOCNATIVE-01", "single-component class-root key paths such as `@keypath(Person, name)`"), SnippetCheck("M265-B003-DOCNATIVE-02", "generic Objective-C method declarations written as `- <T> ...` remain")]),
        (SPEC_AM, [SnippetCheck("M265-B003-AM-01", "single-component paths fail closed unless the component names a readable"), SnippetCheck("M265-B003-AM-02", "Generic Objective-C method declarations written as `- <T> ...` remain")]),
        (SPEC_ATTR, [SnippetCheck("M265-B003-ATTR-01", "Current implementation status (`M265-B003`)"), SnippetCheck("M265-B003-ATTR-02", "class-root key paths such as `@keypath(Person, name)` now fail closed unless")]),
        (SPEC_PART3, [SnippetCheck("M265-B003-PART3-01", "Implementation note (`M265-B003`)"), SnippetCheck("M265-B003-PART3-02", "Multi-component typed key-path member chains still fail closed")]),
        (PACKAGE_JSON, [SnippetCheck("M265-B003-PKG-01", '"check:objc3c:m265-b003-generic-erasure-and-key-path-legality-completion-edge-case-and-compatibility-completion"'), SnippetCheck("M265-B003-PKG-02", '"test:tooling:m265-b003-generic-erasure-and-key-path-legality-completion-edge-case-and-compatibility-completion"'), SnippetCheck("M265-B003-PKG-03", '"check:objc3c:m265-b003-lane-b-readiness"')]),
    ]

    for path, snippets in snippet_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        print(json.dumps(summary, indent=2))
        return 1

    print(f"[ok] M265-B003 generic erasure/key-path legality verified -> {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
