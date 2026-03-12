#!/usr/bin/env python3
"""Checker for M266-B003 defer legality, cleanup order, and non-local exit rules."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-b003-defer-legality-cleanup-order-and-non-local-exit-v1"
CONTRACT_ID = "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-B003" / "defer_legality_cleanup_order_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_defer_legality_cleanup_order_and_non_local_exit_rules_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_b003_defer_legality_cleanup_order_and_non_local_exit_rules_edge_case_and_compatibility_completion_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_defer_legality_positive.objc3"
RETURN_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_defer_return_negative.objc3"
BREAK_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_defer_break_negative.objc3"
CONTINUE_NEGATIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_defer_continue_negative.objc3"


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
        failures.append(Finding(display_path(path), "M266-B003-MISSING", f"missing artifact: {display_path(path)}"))
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


def semantic_surface_from_manifest(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(manifest_path)}")
    return semantic_surface


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: list[tuple[str, str, Any, str]] = [
        ("M266-B003-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M266-B003-PAYLOAD-02", "surface_path", "frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model", "surface path drifted"),
        ("M266-B003-PAYLOAD-03", "semantic_model", "guard-refinement-plus-statement-match-exhaustiveness-and-defer-legality-semantics-are-live-while-defer-cleanup-lowering-remains-a-later-lane-c-runtime-step", "semantic model drifted"),
        ("M266-B003-PAYLOAD-04", "defer_model", "defer-statement-lifo-cleanup-order-and-defer-mediated-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred-to-later-m266-lowering-and-runtime-work", "defer model drifted"),
        ("M266-B003-PAYLOAD-05", "non_local_exit_model", "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred", "non-local exit model drifted"),
        ("M266-B003-PAYLOAD-06", "defer_statement_semantic_sites", 1, "defer statement count mismatch"),
        ("M266-B003-PAYLOAD-07", "defer_scope_cleanup_order_sites", 1, "defer cleanup-order count mismatch"),
        ("M266-B003-PAYLOAD-08", "defer_nonlocal_exit_diagnostic_sites", 0, "defer diagnostic count mismatch"),
        ("M266-B003-PAYLOAD-09", "continue_statement_sites", 1, "continue count mismatch"),
        ("M266-B003-PAYLOAD-10", "break_statement_sites", 0, "break count mismatch"),
        ("M266-B003-PAYLOAD-11", "failure_reason", "", "failure reason should stay empty"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)
    for check_id, field, expected in [
        ("M266-B003-PAYLOAD-12", "defer_cleanup_order_semantics_landed", True),
        ("M266-B003-PAYLOAD-13", "defer_nonlocal_exit_semantics_landed", True),
        ("M266-B003-PAYLOAD-14", "defer_cleanup_order_deferred", False),
        ("M266-B003-PAYLOAD-15", "defer_nonlocal_exit_deferred", False),
        ("M266-B003-PAYLOAD-16", "non_local_exit_restrictions_landed", True),
        ("M266-B003-PAYLOAD-17", "deterministic", True),
        ("M266-B003-PAYLOAD-18", "ready_for_lowering_and_runtime", True),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is expected, artifact, check_id, f"{field} drifted", failures)
    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-B003-PAYLOAD-19", "replay key missing", failures)
    return checks_total, checks_passed


def run_negative_probe(runner: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([str(runner), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    ensure_build = run_command([sys.executable, "scripts/ensure_objc3c_native_build.py", "--mode", "fast", "--reason", "m266-b003-readiness", "--summary-out", "tmp/reports/m266/M266-B003/ensure_build_summary.json"])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M266-B003-DYN-01", "ensure build failed", failures)
    positive_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b003" / "positive"
    positive = run_command([str(args.runner_exe), str(POSITIVE_FIXTURE), "--out-dir", str(positive_out), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M266-B003-DYN-02", "positive fixture failed", failures)
    manifest_path = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M266-B003-DYN-03", "positive manifest missing", failures)
    payload: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = semantic_surface_from_manifest(manifest_path)
        payload_obj = semantic_surface.get("objc_part5_control_flow_semantic_model")
        checks_total += 1
        checks_passed += require(isinstance(payload_obj, dict), display_path(manifest_path), "M266-B003-DYN-04", "semantic model payload missing", failures)
        if isinstance(payload_obj, dict):
            payload = payload_obj
            total, passed = validate_payload(payload, display_path(manifest_path), failures)
            checks_total += total
            checks_passed += passed
    negative_specs = [
        (RETURN_NEGATIVE, "return-negative", "return' is not allowed inside defer body"),
        (BREAK_NEGATIVE, "break-negative", "'break' may not exit a defer body"),
        (CONTINUE_NEGATIVE, "continue-negative", "'continue' may not exit a defer body"),
    ]
    negatives: list[dict[str, Any]] = []
    for fixture, case_id, expected_snippet in negative_specs:
        negative = run_negative_probe(args.runner_exe, fixture, ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "b003" / case_id)
        output = (negative.stdout or "") + (negative.stderr or "")
        checks_total += 2
        checks_passed += require(negative.returncode != 0, display_path(fixture), f"M266-B003-DYN-{case_id}-01", "negative fixture should fail", failures)
        checks_passed += require("O3S222" in output and expected_snippet in output, display_path(fixture), f"M266-B003-DYN-{case_id}-02", "expected defer diagnostic missing", failures)
        negatives.append({"fixture": display_path(fixture), "returncode": negative.returncode})
    dynamic = {
        "positive_manifest": display_path(manifest_path),
        "semantic_model_payload": payload,
        "negative_probes": negatives,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [SnippetCheck("M266-B003-SNIP-01", "deterministic cleanup-order accounting")],
        PACKET_DOC: [SnippetCheck("M266-B003-SNIP-02", "Implement live `defer` legality semantics")],
        SEMA_CONTRACT: [
            SnippetCheck("M266-B003-SNIP-03", "defer_cleanup_order_semantics_landed"),
            SnippetCheck("M266-B003-SNIP-04", "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred"),
        ],
        SEMA_CPP: [
            SnippetCheck("M266-B003-SNIP-05", "control-flow misuse: 'return' is not allowed inside defer body"),
            SnippetCheck("M266-B003-SNIP-06", "defer_scope_cleanup_order_sites"),
        ],
        FRONTEND_TYPES: [SnippetCheck("M266-B003-SNIP-07", "defer_statement_source_supported")],
        FRONTEND_ARTIFACTS: [SnippetCheck("M266-B003-SNIP-08", "defer_statement_semantic_sites")],
        DOC_GRAMMAR: [SnippetCheck("M266-B003-SNIP-09", "source-only `defer { ... }` statements are now admitted")],
        DOC_NATIVE: [SnippetCheck("M266-B003-SNIP-10", "source-only `defer { ... }` statements are now admitted")],
        SPEC_AM: [SnippetCheck("M266-B003-SNIP-11", "M266-B003 implementation note")],
        SPEC_RULES: [SnippetCheck("M266-B003-SNIP-12", "Current Part 5 defer note")],
        ARCHITECTURE_DOC: [SnippetCheck("M266-B003-SNIP-13", "M266-B003")],
        PACKAGE_JSON: [SnippetCheck("M266-B003-SNIP-14", "check:objc3c:m266-b003-lane-b-readiness")],
    }
    for path, snippets in snippet_map.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)
    dynamic: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        total, passed, dynamic = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "failures": [failure.__dict__ for failure in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
