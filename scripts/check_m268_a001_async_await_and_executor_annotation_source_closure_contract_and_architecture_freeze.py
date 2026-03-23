#!/usr/bin/env python3
"""Checker for M268-A001 async source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-a001-part7-async-source-closure-v1"
CONTRACT_ID = "objc3c-part7-async-source-closure/m268-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m268" / "M268-A001" / "async_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_async_await_and_executor_annotation_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_a001_async_await_and_executor_annotation_source_closure_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_async_await_executor_source_closure_positive.objc3"


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
        failures.append(Finding(display_path(path), "M268-A001-MISSING", f"missing artifact: {display_path(path)}"))
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


def compute_fixture_summary(text: str) -> dict[str, int]:
    return {
        "async_fn_sites": text.count("async fn"),
        "async_method_sites": text.count(" async __attribute__((objc_executor(main)))"),
        "await_expression_sites": text.count("await "),
        "executor_attribute_sites": text.count("objc_executor("),
        "executor_named_sites": text.count("objc_executor(named("),
        "executor_main_sites": text.count("objc_executor(main)"),
    }


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m268-a001-readiness",
        "--summary-out",
        "tmp/reports/m268/M268-A001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M268-A001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M268-A001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "a001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M268-A001-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M268-A001-DYN-04", "positive manifest missing", failures)

    fixture_summary = compute_fixture_summary(read_text(FIXTURE))
    expected = {
        "async_fn_sites": 1,
        "async_method_sites": 2,
        "await_expression_sites": 2,
        "executor_attribute_sites": 3,
        "executor_named_sites": 1,
        "executor_main_sites": 2,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(
            fixture_summary.get(field) == expected_value,
            display_path(FIXTURE),
            f"M268-A001-DYN-{index:02d}",
            f"{field} mismatch",
            failures,
        )

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "fixture_summary": fixture_summary,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M268-A001-EXP-01", "# M268 Async, Await, And Executor-Annotation Source Closure Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M268-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M268-A001-EXP-03", "`async fn` is admitted as a source-owned frontend declaration form"),
            SnippetCheck("M268-A001-EXP-04", "`await <expr>` is admitted as a source-owned frontend expression marker"),
        ],
        PACKET_DOC: [
            SnippetCheck("M268-A001-PKT-01", "# M268-A001 Packet: Async, Await, And Executor-Annotation Source Closure Contract And Architecture Freeze"),
            SnippetCheck("M268-A001-PKT-02", "keep the claim strictly at source closure; no runnable continuation or executor runtime claim yet"),
            SnippetCheck("M268-A001-PKT-03", "`native/objc3c/src/ast/objc3_ast.h`"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M268-A001-GRM-01", "## M268 frontend async source closure"),
            SnippetCheck("M268-A001-GRM-02", "`async fn` is admitted as a parser-owned declaration form"),
            SnippetCheck("M268-A001-GRM-03", "`await <expr>` is admitted as a parser-owned expression marker"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M268-A001-DOC-01", "## M268 frontend async source closure"),
            SnippetCheck("M268-A001-DOC-02", "`__attribute__((objc_executor(named(\"...\"))))`"),
        ],
        SPEC_AM: [
            SnippetCheck("M268-A001-AM-01", "M268-A001 source-closure note:"),
            SnippetCheck("M268-A001-AM-02", "`async`, `await`, and canonical `objc_executor(...)` callable attributes are"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M268-A001-ATTR-01", "## M268 current async source boundary (implementation note)"),
            SnippetCheck("M268-A001-ATTR-02", "Current implementation status (`M268-A001`):"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M268-A001-TOK-01", "KwAsync"),
            SnippetCheck("M268-A001-TOK-02", "KwAwait"),
            SnippetCheck("M268-A001-TOK-03", "kObjc3SourceOnlyFeatureClaimExecutorAffinityAttributes"),
        ],
        LEXER_CPP: [
            SnippetCheck("M268-A001-LEX-01", "TokenKind::String"),
            SnippetCheck("M268-A001-LEX-02", 'ident == "async"'),
            SnippetCheck("M268-A001-LEX-03", 'ident == "await"'),
        ],
        PARSER_CPP: [
            SnippetCheck("M268-A001-PARSE-01", "bool AtAsyncClauseKeyword() const"),
            SnippetCheck("M268-A001-PARSE-02", "ParseOptionalAsyncClause(FunctionDecl &fn)"),
            SnippetCheck("M268-A001-PARSE-03", "ParseExecutorAttributePayload"),
            SnippetCheck("M268-A001-PARSE-04", "rhs->await_expression_enabled = true;"),
        ],
        AST_HEADER: [
            SnippetCheck("M268-A001-AST-01", "bool await_expression_enabled = false;"),
            SnippetCheck("M268-A001-AST-02", "bool async_declared = false;"),
            SnippetCheck("M268-A001-AST-03", "bool executor_affinity_declared = false;"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M268-A001-PKG-01", '"check:objc3c:m268-a001-async-await-and-executor-annotation-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M268-A001-PKG-02", '"check:objc3c:m268-a001-lane-a-readiness"'),
        ],
    }

    for path, required in snippets.items():
        checks_total += len(required)
        checks_passed += ensure_snippets(path, required, failures)

    dynamic = {}
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
    print(f"[ok] M268-A001 async source closure checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
