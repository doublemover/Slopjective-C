#!/usr/bin/env python3
"""Checker for M273-A001 derive/macro/property-behavior source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-a001-part10-metaprogramming-source-closure-v1"
CONTRACT_ID = "objc3c-part10-metaprogramming-source-closure/m273-a001-v1"
SURFACE_KEY = "objc_part10_derive_macro_property_behavior_source_closure"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-A001" / "metaprogramming_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_derive_macro_and_property_behavior_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_a001_derive_macro_and_property_behavior_source_closure_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_a001_derive_macro_property_behavior_positive.objc3"


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
        failures.append(Finding(display_path(path), "M273-A001-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m273-a001-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-A001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-A001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-A001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "a001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([str(args.runner_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    output = (run.stdout or "") + (run.stderr or "")
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    checks_total += 1
    checks_passed += require(summary_path.exists(), display_path(summary_path), "M273-A001-DYN-03", "runner summary missing", failures)
    checks_total += 1
    checks_passed += require(diagnostics_path.exists(), display_path(diagnostics_path), "M273-A001-DYN-04", "runner diagnostics missing", failures)

    summary_payload: dict[str, object] = {}
    diagnostics_payload: dict[str, object] = {}
    if summary_path.exists():
      summary_payload = json.loads(summary_path.read_text(encoding="utf-8"))
    if diagnostics_path.exists():
      diagnostics_payload = json.loads(diagnostics_path.read_text(encoding="utf-8"))

    checks_total += 1
    checks_passed += require(run.returncode == 1, display_path(FIXTURE), "M273-A001-DYN-05", f"expected current pre-lowering export gate failure, got: {output}", failures)
    checks_total += 1
    checks_passed += require("runtime metadata export shape drift detected before lowering" in output, display_path(FIXTURE), "M273-A001-DYN-06", "expected current runtime export gate message missing", failures)
    checks_total += 1
    checks_passed += require("unsupported Objective-C container attribute 'objc_macro'" not in output, display_path(FIXTURE), "M273-A001-DYN-07", "objc_macro marker was rejected instead of admitted", failures)
    checks_total += 1
    checks_passed += require("@property attribute 'behavior' must not specify a value" not in output, display_path(FIXTURE), "M273-A001-DYN-08", "property behavior marker was rejected instead of admitted", failures)
    checks_total += 1
    checks_passed += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M273-A001-DYN-09", "semantic stage should run before the current export gate blocks lowering", failures)
    checks_total += 1
    checks_passed += require(summary_payload.get("last_error") == "error:1:1: runtime metadata export blocked: runtime metadata export shape drift detected before lowering (semantic type-metadata handoff is not deterministic) [O3S260]", display_path(summary_path), "M273-A001-DYN-10", "unexpected last_error in runner summary", failures)
    checks_total += 1
    checks_passed += require(any(item.get("code") == "O3S260" for item in diagnostics_payload.get("diagnostics", [])), display_path(diagnostics_path), "M273-A001-DYN-11", "expected O3S260 export gate diagnostic missing", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_summary": display_path(summary_path),
        "positive_diagnostics": display_path(diagnostics_path),
        "current_export_gate": summary_payload.get("last_error"),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-A001-EXP-01", "# M273 Derive, Macro, and Property-Behavior Source Closure Contract and Architecture Freeze Expectations (A001)"),
            SnippetCheck("M273-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-A001-PKT-01", "# M273-A001 Packet: Derive, Macro, and Property-Behavior Source Closure - Contract and Architecture Freeze"),
            SnippetCheck("M273-A001-PKT-02", "frontend.pipeline.semantic_surface.objc_part10_derive_macro_property_behavior_source_closure"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M273-A001-GRM-01", "## M273 derive, macro, and property-behavior source closure"),
            SnippetCheck("M273-A001-GRM-02", "the emitted frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part10_derive_macro_property_behavior_source_closure`"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-A001-DOC-01", "## M273 derive, macro, and property-behavior source closure"),
            SnippetCheck("M273-A001-DOC-02", "objc_part10_derive_macro_property_behavior_source_closure"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-A001-ATTR-01", "## M273 derive, macro, and property-behavior source closure (A001)"),
            SnippetCheck("M273-A001-ATTR-02", "behavior=..."),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-A001-META-01", "## M273 source-closure note"),
            SnippetCheck("M273-A001-META-02", "no macro-expansion records, derive-synthesis descriptors, or property-behavior runtime tables are claimed yet"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M273-A001-TOK-01", "kObjc3Part10MetaprogrammingSourceClosureContractId"),
            SnippetCheck("M273-A001-TOK-02", "kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers"),
        ],
        LEXER_CPP: [
            SnippetCheck("M273-A001-LEX-01", "M273-A001 source-closure note:"),
        ],
        AST_HEADER: [
            SnippetCheck("M273-A001-AST-01", "bool objc_macro_declared = false;"),
            SnippetCheck("M273-A001-AST-02", "bool property_behavior_declared = false;"),
            SnippetCheck("M273-A001-AST-03", "bool objc_derive_declared = false;"),
        ],
        PARSER_CPP: [
            SnippetCheck("M273-A001-PARSE-01", "ParseNamedStringAttributePayload"),
            SnippetCheck("M273-A001-PARSE-02", 'attribute_name.text == "objc_derive"'),
            SnippetCheck("M273-A001-PARSE-03", 'attribute_name.text == "objc_macro"'),
            SnippetCheck("M273-A001-PARSE-04", 'attribute.name == "behavior"'),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M273-A001-TYPE-01", "kObjc3Part10MetaprogrammingSourceClosureSurfacePath"),
            SnippetCheck("M273-A001-TYPE-02", "struct Objc3FrontendPart10MetaprogrammingSourceClosureSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M273-A001-PIPE-01", "BuildPart10MetaprogrammingSourceClosureSummary"),
            SnippetCheck("M273-A001-PIPE-02", "BuildPart10MetaprogrammingSourceClosureReplayKey"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M273-A001-ART-01", "BuildPart10MetaprogrammingSourceClosureSummaryJson"),
            SnippetCheck("M273-A001-ART-02", "objc_part10_derive_macro_property_behavior_source_closure"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-A001-PKG-01", '"check:objc3c:m273-a001-derive-macro-and-property-behavior-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M273-A001-PKG-02", '"check:objc3c:m273-a001-lane-a-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, object] = {"skipped": True}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
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
    print(f"[ok] M273-A001 source-closure checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
