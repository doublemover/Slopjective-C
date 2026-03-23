#!/usr/bin/env python3
"""Checker for M271-A002 cleanup/resource/capture frontend completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-a002-part8-cleanup-resource-capture-source-completion-v1"
CONTRACT_ID = "objc3c-part8-cleanup-resource-capture-source-closure/m271-a002-v1"
SURFACE_KEY = "objc_part8_cleanup_resource_and_capture_source_completion"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m271" / "M271-A002" / "cleanup_resource_capture_source_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_frontend_cleanup_resource_and_capture_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_a002_frontend_cleanup_resource_and_capture_surface_completion_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_CONFORMANCE = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_cleanup_resource_capture_surface_positive.objc3"


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
        failures.append(Finding(display_path(path), "M271-A002-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m271-a002-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-A002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M271-A002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M271-A002-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "a002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([str(args.runner_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M271-A002-DYN-03", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M271-A002-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "cleanup_attribute_sites": 2,
        "cleanup_sugar_sites": 1,
        "resource_attribute_sites": 2,
        "resource_sugar_sites": 1,
        "resource_close_clause_sites": 2,
        "resource_invalid_clause_sites": 2,
        "explicit_capture_list_sites": 1,
        "explicit_capture_item_sites": 4,
        "explicit_capture_weak_sites": 1,
        "explicit_capture_unowned_sites": 1,
        "explicit_capture_move_sites": 1,
        "explicit_capture_plain_sites": 1,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M271-A002-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    checks_total += 1
    checks_passed += require(packet.get("deterministic_handoff") is True, display_path(manifest_path), "M271-A002-DYN-17", "deterministic_handoff must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("ready_for_semantic_expansion") is True, display_path(manifest_path), "M271-A002-DYN-18", "ready_for_semantic_expansion must be true", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part8_source_completion_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M271-A002-EXP-01", "# M271 Frontend Cleanup, Resource, And Capture Surface Completion Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M271-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M271-A002-EXP-03", "The compiler must admit local `@resource(CloseFn, invalid: Expr) let name = value;` declarations"),
        ],
        PACKET_DOC: [
            SnippetCheck("M271-A002-PKT-01", "# M271-A002 Packet: Frontend Cleanup, Resource, And Capture Surface Completion - Core Feature Implementation"),
            SnippetCheck("M271-A002-PKT-02", "frontend.pipeline.semantic_surface.objc_part8_cleanup_resource_and_capture_source_completion"),
            SnippetCheck("M271-A002-PKT-03", "`cleanup_attribute_sites = 2`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M271-A002-GRM-01", "## M271 frontend cleanup, resource, and capture surface completion"),
            SnippetCheck("M271-A002-GRM-02", "`@cleanup(CleanupFn) let name = value;` is admitted as local cleanup sugar"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M271-A002-DOC-01", "## M271 frontend cleanup, resource, and capture surface completion"),
            SnippetCheck("M271-A002-DOC-02", "`@resource(CloseFn, invalid: Expr) let name = value;` is admitted as local"),
        ],
        SPEC_AM: [
            SnippetCheck("M271-A002-AM-01", "M271-A002 frontend completion note:"),
            SnippetCheck("M271-A002-AM-02", "`cleanup` hook attributes, `@cleanup` sugar,"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M271-A002-ATTR-01", "M271-A002 frontend completion:"),
            SnippetCheck("M271-A002-ATTR-02", "`@cleanup(CleanupFn)` and `@resource(CloseFn, invalid: Expr)`"),
        ],
        SPEC_CONFORMANCE: [
            SnippetCheck("M271-A002-CONF-01", "M271-A002 implementation note:"),
            SnippetCheck("M271-A002-CONF-02", "cleanup-hook locals, local resource sugar, and all"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M271-A002-TOK-01", "kObjc3Part8CleanupResourceCaptureSurfaceCompletionContractId"),
            SnippetCheck("M271-A002-TOK-02", "kObjc3SourceOnlyFeatureClaimCleanupHookAnnotations"),
            SnippetCheck("M271-A002-TOK-03", "kObjc3SourceOnlyFeatureClaimCleanupResourceSugar"),
        ],
        LEXER_CPP: [
            SnippetCheck("M271-A002-LEX-01", "M271-A002 source-surface note:"),
            SnippetCheck("M271-A002-LEX-02", "`@cleanup` and `@resource`"),
        ],
        AST_HEADER: [
            SnippetCheck("M271-A002-AST-01", "bool cleanup_attribute_declared = false;"),
            SnippetCheck("M271-A002-AST-02", "bool resource_sugar_declared = false;"),
            SnippetCheck("M271-A002-AST-03", "std::string cleanup_function_symbol;"),
        ],
        PARSER_CPP: [
            SnippetCheck("M271-A002-PARSE-01", "ParseLocalStorageAttribute"),
            SnippetCheck("M271-A002-PARSE-02", "ParseLocalStorageSugar"),
            SnippetCheck("M271-A002-PARSE-03", "only cleanup and objc_resource are supported on local let bindings in this tranche"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M271-A002-TYPE-01", "kObjc3Part8CleanupResourceCaptureSourceCompletionSurfacePath"),
            SnippetCheck("M271-A002-TYPE-02", "struct Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M271-A002-PIPE-01", "BuildPart8CleanupResourceCaptureSourceCompletionSummary"),
            SnippetCheck("M271-A002-PIPE-02", "BuildPart8CleanupResourceCaptureSourceCompletionReplayKey"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M271-A002-ART-01", "BuildPart8CleanupResourceCaptureSourceCompletionSummaryJson"),
            SnippetCheck("M271-A002-ART-02", "objc_part8_cleanup_resource_and_capture_source_completion"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M271-A002-PKG-01", '"check:objc3c:m271-a002-frontend-cleanup-resource-and-capture-surface-completion-core-feature-implementation"'),
            SnippetCheck("M271-A002-PKG-02", '"check:objc3c:m271-a002-lane-a-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, object] = {"skipped": True}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_summary": dynamic_summary,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M271-A002 source-completion checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
