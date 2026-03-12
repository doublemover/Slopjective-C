#!/usr/bin/env python3
"""Fail-closed checker for M265-A001 Part 3 type source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m265-a001-part3-type-source-closure-v1"
CONTRACT_ID = "objc3c-part3-type-source-closure/m265-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-A001" / "optionals_nullability_pragmatic_generics_and_key_path_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_optionals_nullability_pragmatic_generics_and_key_path_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_a001_optionals_nullability_pragmatic_generics_and_key_path_source_closure_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_part3_type_source_closure_positive.objc3"
OPTIONAL_ACCESS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_fail_closed.objc3"
NIL_COALESCING_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_nil_coalescing_fail_closed.objc3"
KEYPATH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_literal_fail_closed.objc3"
EXPECTED_SOURCE_ONLY = [
    "source-only:protocol-optional-partitions",
    "source-only:object-pointer-nullability-suffixes",
    "source-only:pragmatic-generic-suffixes",
]
EXPECTED_UNSUPPORTED = [
    "unsupported:optional-member-access",
    "unsupported:nil-coalescing",
    "unsupported:typed-keypath-literals",
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
        failures.append(Finding(display_path(path), "M265-A001-MISSING", f"missing artifact: {display_path(path)}"))
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
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == CONTRACT_ID, artifact, "M265-A001-PAYLOAD-01", "contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("frontend_surface_path") == "frontend.pipeline.semantic_surface.objc_part3_type_source_closure", artifact, "M265-A001-PAYLOAD-02", "surface path drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("source_only_claim_ids") == EXPECTED_SOURCE_ONLY, artifact, "M265-A001-PAYLOAD-03", "source-only claim ids drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("unsupported_claim_ids") == EXPECTED_UNSUPPORTED, artifact, "M265-A001-PAYLOAD-04", "unsupported claim ids drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("protocol_required_method_count") == 1, artifact, "M265-A001-PAYLOAD-05", "required protocol method count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("protocol_optional_method_count") == 1, artifact, "M265-A001-PAYLOAD-06", "optional protocol method count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("protocol_required_property_count") == 1, artifact, "M265-A001-PAYLOAD-07", "required protocol property count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("protocol_optional_property_count") == 1, artifact, "M265-A001-PAYLOAD-08", "optional protocol property count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("object_pointer_type_spelling_sites") == 0, artifact, "M265-A001-PAYLOAD-09", "object pointer spelling count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("pointer_declarator_entries") == 5, artifact, "M265-A001-PAYLOAD-10", "pointer declarator count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("nullability_suffix_entries") == 5, artifact, "M265-A001-PAYLOAD-11", "nullability suffix count mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("generic_suffix_entries") == 5, artifact, "M265-A001-PAYLOAD-12", "generic suffix count mismatch", failures)
    for check_id, field in [
        ("M265-A001-PAYLOAD-13", "protocol_optional_partition_source_supported"),
        ("M265-A001-PAYLOAD-14", "object_pointer_nullability_source_supported"),
        ("M265-A001-PAYLOAD-15", "pragmatic_generic_suffix_source_supported"),
        ("M265-A001-PAYLOAD-16", "optional_member_access_fail_closed"),
        ("M265-A001-PAYLOAD-17", "nil_coalescing_fail_closed"),
        ("M265-A001-PAYLOAD-18", "typed_keypath_literal_fail_closed"),
        ("M265-A001-PAYLOAD-19", "deterministic_handoff"),
        ("M265-A001-PAYLOAD-20", "ready_for_semantic_expansion"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)
    for check_id, field in [
        ("M265-A001-PAYLOAD-21", "optional_member_access_sites"),
        ("M265-A001-PAYLOAD-22", "nil_coalescing_sites"),
        ("M265-A001-PAYLOAD-23", "typed_keypath_literal_sites"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) == 0, artifact, check_id, f"{field} must stay zero", failures)
    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M265-A001-PAYLOAD-24", "replay key missing", failures)
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
        "m265-a001-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-A001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-A001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-A001-DYN-02", "frontend runner missing after build", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "a001" / "positive"
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
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M265-A001-DYN-03", f"positive fixture failed: {positive.stderr or positive.stdout}", failures)
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M265-A001-DYN-04", "positive manifest missing", failures)
    payload: dict[str, Any] = {}
    object_pointer_payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get("objc_part3_type_source_closure", {})
        object_pointer_payload = semantic_surface.get("objc_object_pointer_nullability_generics_surface", {})
        if not isinstance(payload, dict):
            payload = {}
        if not isinstance(object_pointer_payload, dict):
            object_pointer_payload = {}
        sub_total, sub_passed = validate_summary_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed
        for check_id, field in [
            ("M265-A001-DYN-05", "object_pointer_type_spelling_sites"),
            ("M265-A001-DYN-06", "pointer_declarator_entries"),
            ("M265-A001-DYN-07", "nullability_suffix_entries"),
            ("M265-A001-DYN-08", "generic_suffix_entries"),
        ]:
            left = payload.get(field)
            right = object_pointer_payload.get(field if field != "object_pointer_type_spelling_sites" else "object_pointer_type_spellings")
            checks_total += 1
            checks_passed += require(left == right, display_path(manifest_path), check_id, f"{field} drifted from object pointer summary", failures)

    negative_specs = [
        (OPTIONAL_ACCESS_FIXTURE, "optional-member-access", "unexpected character '.' [O3L001]"),
        (NIL_COALESCING_FIXTURE, "nil-coalescing", "invalid expression [O3P103]"),
        (KEYPATH_FIXTURE, "typed-keypath", "unsupported '@' directive '@keypath' [O3L001]"),
    ]
    negative_results: list[dict[str, Any]] = []
    for fixture, case_id, expected_snippet in negative_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "a001" / case_id
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
        checks_passed += require(run.returncode != 0, display_path(fixture), f"M265-A001-DYN-{case_id}-01", "negative fixture unexpectedly succeeded", failures)
        checks_total += 1
        checks_passed += require(expected_snippet in combined, display_path(fixture), f"M265-A001-DYN-{case_id}-02", f"expected diagnostic snippet missing: {expected_snippet}", failures)
        negative_results.append({
            "case_id": case_id,
            "fixture": display_path(fixture),
            "expected_snippet": expected_snippet,
            "returncode": run.returncode,
        })

    dynamic = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_manifest": display_path(manifest_path),
        "part3_type_source_closure": payload,
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
            SnippetCheck("M265-A001-EXP-01", "# M265 Optionals, Nullability, Pragmatic Generics, And Key-Path Source Closure Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M265-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M265-A001-EXP-03", "frontend.pipeline.semantic_surface.objc_part3_type_source_closure"),
            SnippetCheck("M265-A001-EXP-04", "unexpected character '.' [O3L001]"),
            SnippetCheck("M265-A001-EXP-05", "invalid expression [O3P103]"),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-A001-PKT-01", "# M265-A001 Packet"),
            SnippetCheck("M265-A001-PKT-02", "`native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`"),
            SnippetCheck("M265-A001-PKT-03", "`docs/objc3c-native.md`"),
            SnippetCheck("M265-A001-PKT-04", "`?.`, `??`, and `@keypath`"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M265-A001-TOK-01", "kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions"),
            SnippetCheck("M265-A001-TOK-02", "kObjc3UnsupportedFeatureClaimOptionalMemberAccess"),
            SnippetCheck("M265-A001-TOK-03", "kObjc3UnsupportedFeatureClaimTypedKeyPathLiterals"),
        ],
        LEXER_CPP: [
            SnippetCheck("M265-A001-LEX-01", "Optional-member access '?.' and nil-coalescing '??' remain fail-closed"),
        ],
        PARSER_CPP: [
            SnippetCheck("M265-A001-PARSE-01", "Optional chaining and nil-coalescing are still fail-closed"),
            SnippetCheck("M265-A001-PARSE-02", "Typed key-path literals and"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M265-A001-TYP-01", "kObjc3Part3TypeSourceClosureContractId"),
            SnippetCheck("M265-A001-TYP-02", "struct Objc3FrontendPart3TypeSourceClosureSummary"),
            SnippetCheck("M265-A001-TYP-03", "IsReadyObjc3FrontendPart3TypeSourceClosureSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M265-A001-PIPE-01", "BuildPart3TypeSourceClosureSummary("),
            SnippetCheck("M265-A001-PIPE-02", "BuildPart3TypeSourceClosureReplayKey("),
            SnippetCheck("M265-A001-PIPE-03", "result.part3_type_source_closure_summary = BuildPart3TypeSourceClosureSummary("),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M265-A001-ART-01", "BuildPart3TypeSourceClosureSummaryJson("),
            SnippetCheck("M265-A001-ART-02", '\",\\\"objc_part3_type_source_closure\\\":\"'),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-A001-DOC-01", "## M265 frontend Part 3 type source closure"),
            SnippetCheck("M265-A001-DOC-02", "protocol `@required` / `@optional` partitions"),
            SnippetCheck("M265-A001-DOC-03", "typed key-path literals such as `@keypath(...)` are not admitted yet"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-A001-AM-01", "Current implementation note:"),
            SnippetCheck("M265-A001-AM-02", "Optional chaining/send, nil-coalescing, and typed key-path execution remain"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-A001-ATTR-01", "### B.2.2.1 Current Part 3 type-surface boundary (implementation note)"),
            SnippetCheck("M265-A001-ATTR-02", "optional-member access `?.` is still fail-closed"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-A001-PKG-01", '"check:objc3c:m265-a001-optionals-nullability-pragmatic-generics-and-key-path-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M265-A001-PKG-02", '"check:objc3c:m265-a001-lane-a-readiness"'),
        ],
    }

    for path, required in snippets.items():
        checks_total += len(required)
        checks_passed += ensure_snippets(path, required, failures)

    dynamic: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
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
    print(f"[ok] M265-A001 Part 3 type source closure checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
