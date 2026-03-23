#!/usr/bin/env python3
"""Checker for M269-A001 task/executor/cancellation source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-a001-part7-task-executor-cancellation-source-closure-v1"
CONTRACT_ID = "objc3c-part7-task-executor-cancellation-source-closure/m269-a001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m269" / "M269-A001" / "task_executor_cancellation_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_task_executor_and_cancellation_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_a001_task_executor_and_cancellation_source_closure_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_task_executor_cancellation_source_closure_positive.objc3"


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
        failures.append(Finding(display_path(path), "M269-A001-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m269-a001-readiness",
        "--summary-out",
        "tmp/reports/m269/M269-A001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        "ensure_objc3c_native_build.py",
        "M269-A001-DYN-01",
        f"fast build failed: {ensure_build.stderr or ensure_build.stdout}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        args.runner_exe.exists(),
        display_path(args.runner_exe),
        "M269-A001-DYN-02",
        "frontend runner missing after build",
        failures,
    )

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "a001" / "positive"
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
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M269-A001-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M269-A001-DYN-04", "positive manifest missing", failures)

    manifest = {}
    async_source = {}
    async_effect = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
        async_source = semantic_surface["objc_part7_async_source_closure"]
        async_effect = semantic_surface["objc_part7_async_effect_and_suspension_semantic_model"]

    source_expected = {
        "async_function_sites": 2,
        "await_expression_sites": 2,
        "executor_attribute_sites": 2,
        "executor_global_sites": 1,
        "executor_named_sites": 1,
    }
    for index, (field, expected_value) in enumerate(source_expected.items(), start=5):
        checks_total += 1
        checks_passed += require(
            async_source.get(field) == expected_value,
            display_path(manifest_path),
            f"M269-A001-DYN-{index:02d}",
            f"async source field {field} mismatch",
            failures,
        )

    effect_expected = {
        "task_runtime_interop_sites": 8,
        "runtime_hook_sites": 4,
        "cancellation_check_sites": 2,
        "cancellation_handler_sites": 1,
        "suspension_point_sites": 2,
        "cancellation_propagation_sites": 1,
    }
    for index, (field, expected_value) in enumerate(effect_expected.items(), start=10):
        checks_total += 1
        checks_passed += require(
            async_effect.get(field) == expected_value,
            display_path(manifest_path),
            f"M269-A001-DYN-{index:02d}",
            f"async effect field {field} mismatch",
            failures,
        )

    checks_total += 1
    checks_passed += require(
        async_effect.get("task_runtime_cancellation_semantics_landed") is False,
        display_path(manifest_path),
        "M269-A001-DYN-16",
        "task runtime cancellation semantics should remain deferred at A001",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        async_effect.get("executor_runtime_deferred") is True,
        display_path(manifest_path),
        "M269-A001-DYN-17",
        "executor runtime must remain deferred at A001",
        failures,
    )

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "async_source_closure": async_source,
        "async_effect_semantics": async_effect,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M269-A001-EXP-01", "# M269 Task, Executor, And Cancellation Source Closure Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M269-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M269-A001-EXP-03", "The compiler must admit one truthful frontend source boundary for task-runtime and cancellation-oriented callable code"),
        ],
        PACKET_DOC: [
            SnippetCheck("M269-A001-PKT-01", "# M269-A001 Packet: Task, Executor, And Cancellation Source Closure Contract And Architecture Freeze"),
            SnippetCheck("M269-A001-PKT-02", "no dedicated `task` or `cancel` keyword is claimed here"),
            SnippetCheck("M269-A001-PKT-03", "parser-owned identifier profiles"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M269-A001-GRM-01", "## M269 task, executor, and cancellation source closure"),
            SnippetCheck("M269-A001-GRM-02", "no dedicated `task` or `cancel` keyword is claimed by this issue"),
            SnippetCheck("M269-A001-GRM-03", "parser-owned symbol profiling now serves as the deterministic source contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M269-A001-DOC-01", "## M269 task, executor, and cancellation source closure"),
            SnippetCheck("M269-A001-DOC-02", "the happy path is proven through the frontend C API runner"),
        ],
        SPEC_AM: [
            SnippetCheck("M269-A001-AM-01", "M269-A001 task/executor/cancellation source-closure note:"),
            SnippetCheck("M269-A001-AM-02", "no dedicated `task` or `cancel` keyword is claimed yet"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M269-A001-ATTR-01", "## M269 task/runtime cancellation source boundary (implementation note)"),
            SnippetCheck("M269-A001-ATTR-02", "task-runtime hooks, cancellation checks, cancellation handlers"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M269-A001-TOK-01", "kObjc3TaskExecutorCancellationSourceClosureContractId"),
            SnippetCheck("M269-A001-TOK-02", "kObjc3SourceOnlyFeatureClaimTaskExecutorCancellationProfiles"),
        ],
        LEXER_CPP: [
            SnippetCheck("M269-A001-LEX-01", "M269-A001 source-closure note:"),
            SnippetCheck("M269-A001-LEX-02", "do not add dedicated lexer keywords yet"),
        ],
        PARSER_CPP: [
            SnippetCheck("M269-A001-PARSE-01", "M269-A001 source-closure anchor:"),
            SnippetCheck("M269-A001-PARSE-02", "static bool IsTaskRuntimeHookSymbol"),
            SnippetCheck("M269-A001-PARSE-03", "static bool IsCancellationCheckSymbol"),
            SnippetCheck("M269-A001-PARSE-04", "static bool IsCancellationHandlerSymbol"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M269-A001-PKG-01", '"check:objc3c:m269-a001-task-executor-and-cancellation-source-closure-contract-and-architecture-freeze"'),
            SnippetCheck("M269-A001-PKG-02", '"check:objc3c:m269-a001-lane-a-readiness"'),
        ],
        FIXTURE: [
            SnippetCheck("M269-A001-FIX-01", "async fn task_runtime_cancel_handler_probe()"),
            SnippetCheck("M269-A001-FIX-02", "objc_executor(named(\"com.example.worker\"))"),
            SnippetCheck("M269-A001-FIX-03", "async fn executor_runtime_global_probe()"),
            SnippetCheck("M269-A001-FIX-04", "objc_executor(global)"),
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
    print(f"[ok] M269-A001 task/executor/cancellation source closure checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
