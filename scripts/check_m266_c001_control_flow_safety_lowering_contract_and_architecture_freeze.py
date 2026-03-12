#!/usr/bin/env python3
"""Checker for M266-C001 control-flow safety lowering contract freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-c001-control-flow-safety-lowering-v1"
CONTRACT_ID = "objc3c-part5-control-flow-safety-lowering/m266-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-C001" / "control_flow_safety_lowering_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_control_flow_safety_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_c001_control_flow_safety_lowering_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
MATCH_POSITIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_guard_match_exhaustiveness_positive.objc3"
GUARD_POSITIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_frontend_pattern_guard_surface_positive.objc3"
DEFER_POSITIVE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_defer_legality_positive.objc3"


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
        failures.append(Finding(display_path(path), "M266-C001-MISSING", f"missing artifact: {display_path(path)}"))
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


def read_contract_payload(manifest_path: Path) -> dict[str, Any]:
    semantic_surface = semantic_surface_from_manifest(manifest_path)
    payload = semantic_surface.get("objc_part5_control_flow_safety_lowering_contract")
    if not isinstance(payload, dict):
        raise TypeError(f"missing lowering payload in {display_path(manifest_path)}")
    return payload


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding], *, expected_guard_statements: int, expected_guard_clauses: int, expected_match_statements: int, expected_defer_statements: int) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: list[tuple[str, str, Any, str]] = [
        ("M266-C001-PAYLOAD-01", "contract_id", CONTRACT_ID, "contract id drifted"),
        ("M266-C001-PAYLOAD-02", "surface_path", SURFACE_PATH, "surface path drifted"),
        ("M266-C001-PAYLOAD-03", "source_semantic_contract_id", "objc3c-part5-control-flow-semantic-model/m266-b001-v1", "source semantic contract drifted"),
        ("M266-C001-PAYLOAD-04", "guard_model", "frontend-and-sema-admit-guard-clauses-while-native-lowering-remains-explicitly-fail-closed", "guard model drifted"),
        ("M266-C001-PAYLOAD-05", "match_model", "frontend-and-sema-admit-statement-match-exhaustiveness-while-native-lowering-remains-explicitly-fail-closed", "match model drifted"),
        ("M266-C001-PAYLOAD-06", "defer_model", "source-only-defer-legality-is-sema-owned-while-native-cleanup-lowering-remains-explicitly-fail-closed", "defer model drifted"),
        ("M266-C001-PAYLOAD-07", "authority_model", "part5-source-closure-plus-part5-semantic-model-own-the-current-lowering-boundary", "authority model drifted"),
        ("M266-C001-PAYLOAD-08", "fail_closed_model", "native-ir-emission-fails-closed-with-o3l300-on-unlowered-guard-match-and-defer-control-flow-sites", "fail-closed model drifted"),
        ("M266-C001-PAYLOAD-09", "guard_statement_sites", expected_guard_statements, "guard statement count mismatch"),
        ("M266-C001-PAYLOAD-10", "guard_clause_sites", expected_guard_clauses, "guard clause count mismatch"),
        ("M266-C001-PAYLOAD-11", "match_statement_sites", expected_match_statements, "match statement count mismatch"),
        ("M266-C001-PAYLOAD-12", "defer_statement_sites", expected_defer_statements, "defer statement count mismatch"),
        ("M266-C001-PAYLOAD-13", "live_guard_short_circuit_sites", 0, "guard lowering should remain deferred"),
        ("M266-C001-PAYLOAD-14", "live_match_dispatch_sites", 0, "match lowering should remain deferred"),
        ("M266-C001-PAYLOAD-15", "live_defer_cleanup_sites", 0, "defer lowering should remain deferred"),
        ("M266-C001-PAYLOAD-16", "fail_closed_guard_short_circuit_sites", expected_guard_statements, "guard fail-closed count mismatch"),
        ("M266-C001-PAYLOAD-17", "fail_closed_match_dispatch_sites", expected_match_statements, "match fail-closed count mismatch"),
        ("M266-C001-PAYLOAD-18", "fail_closed_defer_cleanup_sites", expected_defer_statements, "defer fail-closed count mismatch"),
        ("M266-C001-PAYLOAD-19", "deterministic_fail_closed_sites", expected_guard_statements + expected_match_statements + expected_defer_statements, "deterministic fail-closed count mismatch"),
        ("M266-C001-PAYLOAD-20", "contract_violation_sites", 0, "contract violation count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field, expected in [
        ("M266-C001-PAYLOAD-21", "deterministic", True),
        ("M266-C001-PAYLOAD-22", "fail_closed", True),
        ("M266-C001-PAYLOAD-23", "source_semantic_model_ready", True),
        ("M266-C001-PAYLOAD-24", "ready_for_native_guard_lowering", False),
        ("M266-C001-PAYLOAD-25", "ready_for_native_match_lowering", False),
        ("M266-C001-PAYLOAD-26", "ready_for_native_defer_lowering", False),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is expected, artifact, check_id, f"{field} drifted", failures)

    checks_total += 2
    checks_passed += require(bool(payload.get("semantic_summary_replay_key")), artifact, "M266-C001-PAYLOAD-27", "semantic summary replay key missing", failures)
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-C001-PAYLOAD-28", "replay key missing", failures)
    return checks_total, checks_passed


def run_source_only_probe(runner: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(runner),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])


def run_native_probe(runner: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(runner),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build_summary = ROOT / "tmp" / "reports" / "m266" / "M266-C001" / "ensure_build_summary.json"
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m266-c001-readiness",
        "--summary-out",
        str(ensure_build_summary),
    ])
    checks_total += 2
    checks_passed += require(ensure_build.returncode == 0, display_path(ensure_build_summary), "M266-C001-DYN-01", "ensure build failed", failures)
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M266-C001-DYN-02", "runner missing", failures)

    source_match_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c001" / "source_match"
    source_match = run_source_only_probe(args.runner_exe, MATCH_POSITIVE, source_match_out)
    checks_total += 2
    checks_passed += require(source_match.returncode == 0, display_path(MATCH_POSITIVE), "M266-C001-DYN-03", "source-only guard+match probe failed", failures)
    source_match_manifest = source_match_out / "module.manifest.json"
    checks_passed += require(source_match_manifest.exists(), display_path(source_match_manifest), "M266-C001-DYN-04", "source-only guard+match manifest missing", failures)
    match_payload: dict[str, Any] = {}
    if source_match_manifest.exists():
        match_payload = read_contract_payload(source_match_manifest)
        total, passed = validate_payload(match_payload, display_path(source_match_manifest), failures, expected_guard_statements=1, expected_guard_clauses=2, expected_match_statements=2, expected_defer_statements=0)
        checks_total += total
        checks_passed += passed

    source_defer_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c001" / "source_defer"
    source_defer = run_source_only_probe(args.runner_exe, DEFER_POSITIVE, source_defer_out)
    checks_total += 2
    checks_passed += require(source_defer.returncode == 0, display_path(DEFER_POSITIVE), "M266-C001-DYN-05", "source-only defer probe failed", failures)
    source_defer_manifest = source_defer_out / "module.manifest.json"
    checks_passed += require(source_defer_manifest.exists(), display_path(source_defer_manifest), "M266-C001-DYN-06", "source-only defer manifest missing", failures)
    defer_payload: dict[str, Any] = {}
    if source_defer_manifest.exists():
        defer_payload = read_contract_payload(source_defer_manifest)
        total, passed = validate_payload(defer_payload, display_path(source_defer_manifest), failures, expected_guard_statements=0, expected_guard_clauses=0, expected_match_statements=0, expected_defer_statements=1)
        checks_total += total
        checks_passed += passed

    native_match_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c001" / "live_match"
    native_match = run_native_probe(args.runner_exe, MATCH_POSITIVE, native_match_out)
    native_match_summary = load_json(native_match_out / "module.c_api_summary.json")
    checks_total += 3
    checks_passed += require(native_match.returncode != 0, display_path(MATCH_POSITIVE), "M266-C001-DYN-07", "runnable guard+match probe should fail", failures)
    checks_passed += require("O3L300" in str(native_match_summary.get("last_error", "")), display_path(native_match_out / "module.c_api_summary.json"), "M266-C001-DYN-08", "runnable guard+match probe should fail with O3L300", failures)
    checks_passed += require("unresolved identifier 'value'" in str(native_match_summary.get("last_error", "")), display_path(native_match_out / "module.c_api_summary.json"), "M266-C001-DYN-09", "runnable guard+match probe lost the expected lowering detail", failures)

    native_guard_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c001" / "live_guard"
    native_guard = run_native_probe(args.runner_exe, GUARD_POSITIVE, native_guard_out)
    native_guard_summary = load_json(native_guard_out / "module.c_api_summary.json")
    checks_total += 3
    checks_passed += require(native_guard.returncode != 0, display_path(GUARD_POSITIVE), "M266-C001-DYN-10", "runnable guard probe should fail", failures)
    checks_passed += require("O3L300" in str(native_guard_summary.get("last_error", "")), display_path(native_guard_out / "module.c_api_summary.json"), "M266-C001-DYN-11", "runnable guard probe should fail with O3L300", failures)
    checks_passed += require("unresolved identifier 'capture'" in str(native_guard_summary.get("last_error", "")), display_path(native_guard_out / "module.c_api_summary.json"), "M266-C001-DYN-12", "runnable guard probe lost the expected lowering detail", failures)

    native_defer_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c001" / "live_defer"
    native_defer = run_native_probe(args.runner_exe, DEFER_POSITIVE, native_defer_out)
    native_defer_summary = load_json(native_defer_out / "module.c_api_summary.json")
    checks_total += 3
    checks_passed += require(native_defer.returncode != 0, display_path(DEFER_POSITIVE), "M266-C001-DYN-13", "runnable defer probe should fail", failures)
    checks_passed += require("O3S221" in str(native_defer_summary.get("last_error", "")), display_path(native_defer_out / "module.c_api_summary.json"), "M266-C001-DYN-14", "runnable defer probe should fail with O3S221", failures)
    checks_passed += require("defer statements are not yet runnable" in str(native_defer_summary.get("last_error", "")), display_path(native_defer_out / "module.c_api_summary.json"), "M266-C001-DYN-15", "runnable defer probe lost the expected source-only claim detail", failures)

    dynamic = {
        "source_match_manifest": display_path(source_match_manifest),
        "source_match_payload": match_payload,
        "source_defer_manifest": display_path(source_defer_manifest),
        "source_defer_payload": defer_payload,
        "native_failures": {
            "match": str(native_match_summary.get("last_error", "")),
            "guard": str(native_guard_summary.get("last_error", "")),
            "defer": str(native_defer_summary.get("last_error", "")),
        },
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [SnippetCheck("M266-C001-SNIP-01", "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract")],
        PACKET_DOC: [SnippetCheck("M266-C001-SNIP-02", "Freeze the current lowering contract for admitted Part 5 control-flow constructs")],
        LOWERING_H: [
            SnippetCheck("M266-C001-SNIP-03", "Objc3Part5ControlFlowSafetyLoweringContract"),
            SnippetCheck("M266-C001-SNIP-04", "kObjc3Part5ControlFlowSafetyLoweringContractId"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M266-C001-SNIP-05", "Objc3Part5ControlFlowSafetyLoweringSummary"),
            SnippetCheck("M266-C001-SNIP-06", "Objc3Part5ControlFlowSafetyLoweringReplayKey"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M266-C001-SNIP-07", "kObjc3Part5ControlFlowSafetyLoweringSurfacePath"),
            SnippetCheck("M266-C001-SNIP-08", "BuildPart5ControlFlowSafetyLoweringContractJson"),
            SnippetCheck("M266-C001-SNIP-09", "objc_part5_control_flow_safety_lowering_contract"),
        ],
        IR_EMITTER: [SnippetCheck("M266-C001-SNIP-10", "M266-C001 control-flow safety lowering freeze anchor")],
        DOC_GRAMMAR: [SnippetCheck("M266-C001-SNIP-11", "objc_part5_control_flow_safety_lowering_contract")],
        DOC_NATIVE: [SnippetCheck("M266-C001-SNIP-12", "objc_part5_control_flow_safety_lowering_contract")],
        SPEC_AM: [SnippetCheck("M266-C001-SNIP-13", "M266-C001 lowering note")],
        SPEC_RULES: [SnippetCheck("M266-C001-SNIP-14", "Current Part 5 lowering note")],
        ARCHITECTURE_DOC: [SnippetCheck("M266-C001-SNIP-15", "M266-C001")],
        PACKAGE_JSON: [SnippetCheck("M266-C001-SNIP-16", "check:objc3c:m266-c001-lane-c-readiness")],
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
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
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
