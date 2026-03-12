#!/usr/bin/env python3
"""Fail-closed checker for M266-A001 Part 5 control-flow source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-a001-part5-control-flow-source-closure-v1"
CONTRACT_ID = "objc3c-part5-control-flow-source-closure/m266-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-A001" / "control_flow_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_defer_guard_match_and_pattern_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_a001_defer_guard_match_and_pattern_source_closure_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_part5_control_flow_source_closure_positive.objc3"
DEFER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_defer_statement_fail_closed_negative.objc3"
MATCH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_match_statement_fail_closed_negative.objc3"


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
        failures.append(Finding(display_path(path), "M266-A001-MISSING", f"missing artifact: {display_path(path)}"))
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
        ("M266-A001-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        (
            "M266-A001-PAYLOAD-02",
            "frontend_surface_path",
            "frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure",
            "surface path drifted",
        ),
        (
            "M266-A001-PAYLOAD-03",
            "supported_construct_ids",
            [
                "part5-source:guard-bindings",
                "part5-source:switch-case-patterns",
            ],
            "supported construct ids drifted",
        ),
        (
            "M266-A001-PAYLOAD-04",
            "fail_closed_construct_ids",
            [
                "part5-fail-closed:defer-statement",
                "part5-fail-closed:match-statement",
            ],
            "fail-closed construct ids drifted",
        ),
        ("M266-A001-PAYLOAD-05", "guard_binding_sites", 1, "guard binding site count mismatch"),
        ("M266-A001-PAYLOAD-06", "guard_binding_clause_sites", 1, "guard binding clause count mismatch"),
        ("M266-A001-PAYLOAD-07", "switch_case_pattern_sites", 2, "switch case pattern count mismatch"),
        ("M266-A001-PAYLOAD-08", "switch_default_pattern_sites", 1, "switch default pattern count mismatch"),
        ("M266-A001-PAYLOAD-09", "defer_keyword_sites", 0, "defer keyword site count mismatch"),
        ("M266-A001-PAYLOAD-10", "match_keyword_sites", 0, "match keyword site count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field in [
        ("M266-A001-PAYLOAD-11", "guard_binding_source_supported"),
        ("M266-A001-PAYLOAD-12", "switch_case_pattern_source_supported"),
        ("M266-A001-PAYLOAD-13", "defer_keyword_reserved"),
        ("M266-A001-PAYLOAD-14", "match_keyword_reserved"),
        ("M266-A001-PAYLOAD-15", "defer_fail_closed"),
        ("M266-A001-PAYLOAD-16", "match_fail_closed"),
        ("M266-A001-PAYLOAD-17", "deterministic_handoff"),
        ("M266-A001-PAYLOAD-18", "ready_for_semantic_expansion"),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is True, artifact, check_id, f"{field} must stay true", failures)

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-A001-PAYLOAD-19", "replay key missing", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command(
        [
            sys.executable,
            "scripts/ensure_objc3c_native_build.py",
            "--mode",
            "fast",
            "--reason",
            "m266-a001-readiness",
            "--summary-out",
            "tmp/reports/m266/M266-A001/ensure_build_summary.json",
        ]
    )
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        "ensure_objc3c_native_build.py",
        "M266-A001-DYN-01",
        f"fast build failed: {ensure_build.stderr or ensure_build.stdout}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        args.runner_exe.exists(),
        display_path(args.runner_exe),
        "M266-A001-DYN-02",
        "frontend runner missing after build",
        failures,
    )

    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "a001" / "positive"
    positive_out.mkdir(parents=True, exist_ok=True)
    positive = run_command(
        [
            str(args.runner_exe),
            str(POSITIVE_FIXTURE),
            "--out-dir",
            str(positive_out),
            "--emit-prefix",
            "module",
            "--no-emit-ir",
            "--no-emit-object",
        ]
    )
    checks_total += 1
    checks_passed += require(
        positive.returncode == 0,
        display_path(POSITIVE_FIXTURE),
        "M266-A001-DYN-03",
        f"positive fixture failed: {positive.stderr or positive.stdout}",
        failures,
    )
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(
        manifest_path.exists(),
        display_path(manifest_path),
        "M266-A001-DYN-04",
        "positive manifest missing",
        failures,
    )

    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload = semantic_surface.get("objc_part5_control_flow_source_closure", {})
        if not isinstance(payload, dict):
            payload = {}
        sub_total, sub_passed = validate_summary_payload(payload, display_path(manifest_path), failures)
        checks_total += sub_total
        checks_passed += sub_passed

    negative_specs = [
        (DEFER_FIXTURE, "defer", "unsupported 'defer' statement [O3P154]"),
        (MATCH_FIXTURE, "match", "unsupported 'match' statement [O3P155]"),
    ]
    negative_results: list[dict[str, Any]] = []
    for fixture, case_id, expected_snippet in negative_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "a001" / case_id
        out_dir.mkdir(parents=True, exist_ok=True)
        run = run_command(
            [
                str(args.runner_exe),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                "--no-emit-ir",
                "--no-emit-object",
            ]
        )
        combined = (run.stdout or "") + (run.stderr or "")
        checks_total += 1
        checks_passed += require(
            run.returncode != 0,
            display_path(fixture),
            f"M266-A001-DYN-{case_id}-01",
            "negative fixture unexpectedly succeeded",
            failures,
        )
        checks_total += 1
        checks_passed += require(
            expected_snippet in combined,
            display_path(fixture),
            f"M266-A001-DYN-{case_id}-02",
            f"expected diagnostic snippet missing: {expected_snippet}",
            failures,
        )
        negative_results.append(
            {
                "case_id": case_id,
                "fixture": display_path(fixture),
                "expected_snippet": expected_snippet,
                "returncode": run.returncode,
            }
        )

    dynamic = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_manifest": display_path(manifest_path),
        "part5_control_flow_source_closure": payload,
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
            SnippetCheck("M266-A001-EXP-01", "# M266 Defer, Guard, Match, And Pattern Source Closure Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M266-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M266-A001-EXP-03", "frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure"),
            SnippetCheck("M266-A001-EXP-04", "unsupported 'defer' statement [O3P154]"),
            SnippetCheck("M266-A001-EXP-05", "unsupported 'match' statement [O3P155]"),
        ],
        PACKET_DOC: [
            SnippetCheck("M266-A001-PKT-01", "# M266-A001 Packet: Defer, Guard, Match, And Pattern Source Closure Contract And Architecture Freeze"),
            SnippetCheck("M266-A001-PKT-02", "`switch` / `case` remains the only supported pattern carrier today"),
            SnippetCheck("M266-A001-PKT-03", "`native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M266-A001-GRM-01", "## M266 frontend control-flow and safety source closure"),
            SnippetCheck("M266-A001-GRM-02", "`defer` and `match` are now reserved as explicit frontend keywords and fail"),
            SnippetCheck("M266-A001-GRM-03", "frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M266-A001-DOC-01", "## M266 frontend control-flow and safety source closure"),
            SnippetCheck("M266-A001-DOC-02", "`switch` / `case` remains the only supported pattern carrier in the current"),
            SnippetCheck("M266-A001-DOC-03", "`defer` and `match` are now reserved as explicit frontend keywords and fail"),
        ],
        SPEC_AM: [
            SnippetCheck("M266-A001-AM-01", "The current Part 5 frontend boundary is now explicit: `guard` bindings and"),
            SnippetCheck("M266-A001-AM-02", "`defer` and `match` are reserved fail-closed keywords until the runnable"),
        ],
        SPEC_RULES: [
            SnippetCheck("M266-A001-RULE-01", "The live frontend only admits the existing `guard` binding surface plus"),
            SnippetCheck("M266-A001-RULE-02", "`defer` and `match` are reserved keywords with explicit fail-closed parser"),
        ],
        ARCHITECTURE_DOC: [
            SnippetCheck("M266-A001-ARCH-01", "`frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure`"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M266-A001-TOK-01", "KwDefer"),
            SnippetCheck("M266-A001-TOK-02", "KwMatch"),
            SnippetCheck("M266-A001-TOK-03", "kObjc3Part5FailClosedConstructDefer"),
        ],
        LEXER_CPP: [
            SnippetCheck("M266-A001-LEX-01", 'ident == "defer"'),
            SnippetCheck("M266-A001-LEX-02", 'ident == "match"'),
        ],
        PARSER_CPP: [
            SnippetCheck("M266-A001-PARSE-01", "\"unsupported 'defer' statement\""),
            SnippetCheck("M266-A001-PARSE-02", "\"unsupported 'match' statement\""),
            SnippetCheck("M266-A001-PARSE-03", "At(TokenKind::KwDefer)"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M266-A001-TYP-01", "kObjc3Part5ControlFlowSourceClosureContractId"),
            SnippetCheck("M266-A001-TYP-02", "struct Objc3FrontendPart5ControlFlowSourceClosureSummary"),
            SnippetCheck("M266-A001-TYP-03", "IsReadyObjc3FrontendPart5ControlFlowSourceClosureSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M266-A001-PIPE-01", "BuildPart5ControlFlowSourceClosureReplayKey("),
            SnippetCheck("M266-A001-PIPE-02", "BuildPart5ControlFlowSourceClosureSummary("),
            SnippetCheck("M266-A001-PIPE-03", "CollectPart5ControlFlowSourceClosureStmtSites("),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M266-A001-ART-01", "BuildPart5ControlFlowSourceClosureSummaryJson("),
            SnippetCheck("M266-A001-ART-02", '\",\\\"objc_part5_control_flow_source_closure\\\":\"'),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M266-A001-PKG-01", '"check:objc3c:m266-a001-defer-guard-match-and-pattern-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M266-A001-PKG-02", '"check:objc3c:m266-a001-lane-a-readiness"'),
        ],
    }

    for path, required in snippets.items():
        checks_total += len(required)
        checks_passed += ensure_snippets(path, required, failures)

    dynamic: dict[str, Any] = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
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
    print(f"[ok] M266-A001 Part 5 control-flow source closure checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
