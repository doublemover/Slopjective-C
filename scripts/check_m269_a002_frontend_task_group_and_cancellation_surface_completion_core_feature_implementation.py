#!/usr/bin/env python3
"""Checker for M269-A002 frontend task-group/cancellation surface completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-a002-part7-task-group-cancellation-source-closure-v1"
CONTRACT_ID = "objc3c-part7-task-group-cancellation-source-closure/m269-a002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m269" / "M269-A002" / "task_group_cancellation_source_closure_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_frontend_task_group_and_cancellation_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_a002_frontend_task_group_and_cancellation_surface_completion_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
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
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m269_frontend_task_group_cancellation_positive.objc3"


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
        failures.append(Finding(display_path(path), "M269-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m269-a002-readiness",
        "--summary-out",
        "tmp/reports/m269/M269-A002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(
        ensure_build.returncode == 0,
        "ensure_objc3c_native_build.py",
        "M269-A002-DYN-01",
        f"fast build failed: {ensure_build.stderr or ensure_build.stdout}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        args.runner_exe.exists(),
        display_path(args.runner_exe),
        "M269-A002-DYN-02",
        "frontend runner missing after build",
        failures,
    )

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m269" / "a002" / "positive"
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
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M269-A002-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M269-A002-DYN-04", "positive manifest missing", failures)

    manifest = {}
    task_packet = {}
    async_effect = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
        task_packet = semantic_surface["objc_part7_task_group_and_cancellation_source_closure"]
        async_effect = semantic_surface["objc_part7_async_effect_and_suspension_semantic_model"]

    task_expected = {
        "async_callable_sites": 2,
        "executor_attribute_sites": 2,
        "task_creation_sites": 2,
        "task_group_scope_sites": 1,
        "task_group_add_task_sites": 1,
        "task_group_wait_next_sites": 1,
        "task_group_cancel_all_sites": 1,
        "cancellation_check_sites": 2,
        "cancellation_handler_sites": 1,
    }
    for index, (field, expected_value) in enumerate(task_expected.items(), start=5):
        checks_total += 1
        checks_passed += require(
            task_packet.get(field) == expected_value,
            display_path(manifest_path),
            f"M269-A002-DYN-{index:02d}",
            f"task-group packet field {field} mismatch",
            failures,
        )

    checks_total += 1
    checks_passed += require(
        task_packet.get("deterministic_handoff") is True,
        display_path(manifest_path),
        "M269-A002-DYN-14",
        "task-group source packet must be deterministic",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        task_packet.get("ready_for_semantic_expansion") is True,
        display_path(manifest_path),
        "M269-A002-DYN-15",
        "task-group source packet must be ready for semantic expansion",
        failures,
    )

    effect_expected = {
        "task_runtime_interop_sites": 14,
        "runtime_hook_sites": 9,
        "cancellation_check_sites": 4,
        "cancellation_handler_sites": 1,
        "suspension_point_sites": 1,
        "cancellation_propagation_sites": 1,
    }
    for index, (field, expected_value) in enumerate(effect_expected.items(), start=16):
        checks_total += 1
        checks_passed += require(
            async_effect.get(field) == expected_value,
            display_path(manifest_path),
            f"M269-A002-DYN-{index:02d}",
            f"async effect field {field} mismatch",
            failures,
        )

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "task_group_packet": task_packet,
        "async_effect_semantics": async_effect,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M269-A002-EXP-01", "# M269 Frontend Task-Group And Cancellation Surface Completion Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M269-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M269-A002-EXP-03", "The frontend must publish one deterministic source packet for task creation"),
        ],
        PACKET_DOC: [
            SnippetCheck("M269-A002-PKT-01", "# M269-A002 Packet: Frontend Task-Group And Cancellation Surface Completion - Core Feature Implementation"),
            SnippetCheck("M269-A002-PKT-02", "publish one dedicated frontend semantic packet for task creation/task-group/cancellation source sites"),
            SnippetCheck("M269-A002-PKT-03", "the current supported task-group surface remains callable-identifier based"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M269-A002-GRM-01", "## M269 frontend task-group and cancellation packet"),
            SnippetCheck("M269-A002-GRM-02", "objc_part7_task_group_and_cancellation_source_closure"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M269-A002-DOC-01", "## M269 frontend task-group and cancellation packet"),
            SnippetCheck("M269-A002-DOC-02", "the packet counts task creation call sites admitted by the current frontend"),
        ],
        SPEC_AM: [
            SnippetCheck("M269-A002-AM-01", "M269-A002 frontend task-group/cancellation packet note:"),
            SnippetCheck("M269-A002-AM-02", "later `M269` lanes must consume this packet"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M269-A002-ATTR-01", "Current implementation status (`M269-A002`):"),
            SnippetCheck("M269-A002-ATTR-02", "objc_part7_task_group_and_cancellation_source_closure"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M269-A002-TOK-01", "kObjc3Part7TaskGroupCancellationSourceClosureContractId"),
            SnippetCheck("M269-A002-TOK-02", "kObjc3SourceOnlyFeatureClaimTaskCreationSites"),
            SnippetCheck("M269-A002-TOK-03", "kObjc3SourceOnlyFeatureClaimSupportedTaskGroupSurface"),
        ],
        LEXER_CPP: [
            SnippetCheck("M269-A002-LEX-01", "task-group/task-creation"),
        ],
        PARSER_CPP: [
            SnippetCheck("M269-A002-PARSE-01", 'lowered.find("task_group")'),
            SnippetCheck("M269-A002-PARSE-02", 'lowered.find("task_spawn")'),
            SnippetCheck("M269-A002-PARSE-03", 'lowered.find("wait_next")'),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M269-A002-TYPE-01", "struct Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary"),
            SnippetCheck("M269-A002-TYPE-02", "kObjc3Part7TaskGroupCancellationSourceClosureSurfacePath"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M269-A002-PIPE-01", "BuildPart7TaskGroupCancellationSourceClosureSummary("),
            SnippetCheck("M269-A002-PIPE-02", "CollectPart7TaskGroupCancellationExprSites("),
            SnippetCheck("M269-A002-PIPE-03", "IsPart7TaskGroupAddTaskSymbol("),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M269-A002-ART-01", "BuildPart7TaskGroupCancellationSourceClosureSummaryJson("),
            SnippetCheck("M269-A002-ART-02", "objc_part7_task_group_and_cancellation_source_closure"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M269-A002-PKG-01", '"check:objc3c:m269-a002-frontend-task-group-and-cancellation-surface-completion-core-feature-implementation"'),
            SnippetCheck("M269-A002-PKG-02", '"check:objc3c:m269-a002-lane-a-readiness"'),
        ],
        FIXTURE: [
            SnippetCheck("M269-A002-FIX-01", "task_spawn_child();"),
            SnippetCheck("M269-A002-FIX-02", "with_task_group_scope();"),
            SnippetCheck("M269-A002-FIX-03", "task_group_add_task();"),
            SnippetCheck("M269-A002-FIX-04", "task_group_wait_next();"),
            SnippetCheck("M269-A002-FIX-05", "task_group_cancel_all();"),
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
    print(f"[ok] M269-A002 task-group/cancellation frontend checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
