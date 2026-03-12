#!/usr/bin/env python3
"""Checker for M265-A002 parser-owned Part 3 frontend support."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m265-a002-part3-frontend-support-v1"
CONTRACT_ID = "objc3c-part3-type-source-closure/m265-a002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-A002" / "frontend_support_optional_binding_send_coalescing_keypath_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_a002_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_binding_send_coalescing_keypath_positive.objc3"
OPTIONAL_ACCESS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_fail_closed.objc3"
EXPECTED_SOURCE_ONLY = [
    "source-only:protocol-optional-partitions",
    "source-only:object-pointer-nullability-suffixes",
    "source-only:pragmatic-generic-suffixes",
    "source-only:optional-bindings",
    "source-only:optional-sends",
    "source-only:nil-coalescing",
    "source-only:typed-keypath-literals",
]
EXPECTED_UNSUPPORTED: list[str] = []


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
        failures.append(Finding(display_path(path), "M265-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
    scalar_expectations: list[tuple[str, str, Any]] = [
        ("M265-A002-PAYLOAD-01", "contract_id", CONTRACT_ID),
        ("M265-A002-PAYLOAD-02", "frontend_surface_path", "frontend.pipeline.semantic_surface.objc_part3_type_source_closure"),
        ("M265-A002-PAYLOAD-03", "source_model", "protocol-optional-partitions-object-pointer-nullability-generic-suffixes-optional-bindings-optional-sends-optional-member-access-nil-coalescing-and-typed-keypaths-are-live-parser-owned-source-surfaces"),
        ("M265-A002-PAYLOAD-04", "failure_model", "typed-keypath-literals-remain-source-sema-surfaces-and-fail-closed-on-native-lowering-until-later-m265-issues"),
    ]
    for check_id, field, expected in scalar_expectations:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, f"{field} drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("source_only_claim_ids") == EXPECTED_SOURCE_ONLY, artifact, "M265-A002-PAYLOAD-05", "source-only claim ids drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("unsupported_claim_ids") == EXPECTED_UNSUPPORTED, artifact, "M265-A002-PAYLOAD-06", "unsupported claim ids drifted", failures)
    for check_id, field in [
        ("M265-A002-PAYLOAD-07", "protocol_optional_partition_source_supported"),
        ("M265-A002-PAYLOAD-08", "object_pointer_nullability_source_supported"),
        ("M265-A002-PAYLOAD-09", "pragmatic_generic_suffix_source_supported"),
        ("M265-A002-PAYLOAD-10", "optional_binding_source_supported"),
        ("M265-A002-PAYLOAD-11", "optional_send_source_supported"),
        ("M265-A002-PAYLOAD-12", "nil_coalescing_source_supported"),
        ("M265-A002-PAYLOAD-13", "typed_keypath_literal_source_supported"),
        ("M265-A002-PAYLOAD-14", "deterministic_handoff"),
        ("M265-A002-PAYLOAD-15", "ready_for_semantic_expansion"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must be true", failures)
    for check_id, field in [
        ("M265-A002-PAYLOAD-16", "optional_member_access_fail_closed"),
        ("M265-A002-PAYLOAD-17", "nil_coalescing_fail_closed"),
        ("M265-A002-PAYLOAD-18", "typed_keypath_literal_fail_closed"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is False, artifact, check_id, f"{field} must be false", failures)
    checks_total += 1
    checks_passed += require(payload.get("optional_send_sites") == 1, artifact, "M265-A002-PAYLOAD-19", "optional_send_sites mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("nil_coalescing_sites") == 1, artifact, "M265-A002-PAYLOAD-20", "nil_coalescing_sites mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("typed_keypath_literal_sites") == 1, artifact, "M265-A002-PAYLOAD-21", "typed_keypath_literal_sites mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("optional_member_access_sites") == 0, artifact, "M265-A002-PAYLOAD-22", "optional_member_access_sites must stay zero", failures)
    checks_total += 1
    checks_passed += require(payload.get("nullability_suffix_entries") == 1, artifact, "M265-A002-PAYLOAD-23", "nullability suffix count mismatch", failures)
    checks_total += 1
    checks_passed += require("optional_sites=" in str(payload.get("replay_key", "")), artifact, "M265-A002-PAYLOAD-24", "replay key missing optional site inventory", failures)
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
        "m265-a002-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-A002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-A002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-A002-DYN-02", "frontend runner missing after build", failures)

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "a002" / "positive"
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
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M265-A002-DYN-03", f"positive fixture failed: {positive.stderr or positive.stdout}", failures)
    positive_text = read_text(POSITIVE_FIXTURE)
    for check_id, snippet in [
        ("M265-A002-DYN-04", "if let chosen = maybeValue"),
        ("M265-A002-DYN-05", "guard let ready = maybeValue"),
        ("M265-A002-DYN-06", "maybeValue ?? fallback"),
        ("M265-A002-DYN-07", "[maybeValue? description]"),
        ("M265-A002-DYN-08", "@keypath(self, title)"),
    ]:
        checks_total += 1
        checks_passed += require(snippet in positive_text, display_path(POSITIVE_FIXTURE), check_id, f"positive fixture lost surface snippet: {snippet}", failures)

    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M265-A002-DYN-09", "positive manifest missing", failures)
    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get("objc_part3_type_source_closure", {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_summary_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed

    dynamic = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_manifest": display_path(manifest_path),
        "part3_type_source_closure": payload,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-A002-EXP-01", "# M265 Frontend Support For Optional Sends, Binds, Coalescing, And Typed Key Paths Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M265-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M265-A002-EXP-03", "[receiver? selector]"),
            SnippetCheck("M265-A002-EXP-04", "@keypath(self, title)"),
            SnippetCheck("M265-A002-EXP-05", "optional-member access now lowers through the same nil-short-circuit path"),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-A002-PKT-01", "# M265-A002 Packet"),
            SnippetCheck("M265-A002-PKT-02", "`native/objc3c/src/parse/objc3_parser.cpp`"),
            SnippetCheck("M265-A002-PKT-03", "the positive fixture proves parser-owned admission for `if let` / `guard let`, optional sends, `??`, and `@keypath(...)`"),
            SnippetCheck("M265-A002-PKT-04", "optional-member access no longer remains outside the admitted frontend surface"),
        ],
        AST_HEADER: [
            SnippetCheck("M265-A002-AST-01", "optional_send_enabled"),
            SnippetCheck("M265-A002-AST-02", "typed_keypath_literal_enabled"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M265-A002-TOK-01", "kObjc3SourceOnlyFeatureClaimOptionalBindings"),
            SnippetCheck("M265-A002-TOK-02", "kObjc3SourceOnlyFeatureClaimOptionalSends"),
            SnippetCheck("M265-A002-TOK-03", "kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals"),
        ],
        LEXER_CPP: [
            SnippetCheck("M265-A002-LEX-01", "TokenKind::QuestionQuestion"),
            SnippetCheck("M265-A002-LEX-02", '"@keypath"'),
            SnippetCheck("M265-A002-LEX-03", "TokenKind::QuestionDot"),
        ],
        PARSER_CPP: [
            SnippetCheck("M265-A002-PARSE-01", "ParseOptionalBindingClauses"),
            SnippetCheck("M265-A002-PARSE-02", "ParseNilCoalescing"),
            SnippetCheck("M265-A002-PARSE-03", "BuildTypedKeyPathLiteralProfile"),
            SnippetCheck("M265-A002-PARSE-04", "optional_member_access_enabled = true"),
        ],
        SEMA_CPP: [
            SnippetCheck("M265-A002-SEMA-01", 'expr->op == "??"'),
            SnippetCheck("M265-A002-SEMA-02", "typed_keypath_literal_enabled"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M265-A002-TYP-01", "objc3c-part3-type-source-closure/m265-a002-v1"),
            SnippetCheck("M265-A002-TYP-02", "optional_binding_source_supported"),
            SnippetCheck("M265-A002-TYP-03", "optional_send_source_supported"),
            SnippetCheck("M265-A002-TYP-04", "typed_keypath_literal_source_supported"),
            SnippetCheck("M265-A002-TYP-05", "optional_member_access_fail_closed = false"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M265-A002-PIPE-01", "CollectPart3TypeSourceClosureExprSites"),
            SnippetCheck("M265-A002-PIPE-02", "summary.optional_send_sites"),
            SnippetCheck("M265-A002-PIPE-03", "summary.nil_coalescing_sites"),
            SnippetCheck("M265-A002-PIPE-04", "summary.typed_keypath_literal_sites"),
            SnippetCheck("M265-A002-PIPE-05", "summary.optional_member_access_sites"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M265-A002-ART-01", "optional_send_sites"),
            SnippetCheck("M265-A002-ART-02", "typed_keypath_literal_source_supported"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-A002-DOC-01", "optional binding forms `if let`, `if var`, `guard let`, and `guard var`"),
            SnippetCheck("M265-A002-DOC-02", "optional sends written as `[receiver? selector]`"),
            SnippetCheck("M265-A002-DOC-03", "`?.` optional-member access now lowers natively"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-A002-AM-01", "parser-owned optional binding, optional-send,"),
            SnippetCheck("M265-A002-AM-02", "Optional-member access written as `?.` now lowers"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-A002-ATTR-01", "Current implementation status (`M265-C001`):"),
            SnippetCheck("M265-A002-ATTR-02", "nil-coalescing `??` is admitted as a parser-owned source form"),
            SnippetCheck("M265-A002-ATTR-03", "optional-member access `?.` now lowers natively"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-A002-PKG-01", '"check:objc3c:m265-a002-frontend-support-for-optional-sends-binds-coalescing-and-typed-key-paths-core-feature-implementation"'),
            SnippetCheck("M265-A002-PKG-02", '"test:tooling:m265-a002-frontend-support-for-optional-sends-binds-coalescing-and-typed-key-paths-core-feature-implementation"'),
            SnippetCheck("M265-A002-PKG-03", '"check:objc3c:m265-a002-lane-a-readiness"'),
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
    print(f"[ok] M265-A002 frontend support checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
