#!/usr/bin/env python3
"""Checker for M272-B003 dispatch-control compatibility diagnostics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m272-b003-part9-dispatch-control-compatibility-v1"
CONTRACT_ID = "objc3c-part9-dynamism-control-compatibility-diagnostics/m272-b003-v1"
DEPENDENCY_CONTRACT_ID = "objc3c-part9-override-finality-sealing-legality/m272-b002-v1"
SURFACE_KEY = "objc_part9_dynamism_control_compatibility_diagnostics"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part9_dynamism_control_compatibility_diagnostics"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m272" / "M272-B003" / "dispatch_control_compatibility_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_dispatch_control_compatibility_positive.objc3"
NEGATIVE_CASES = [
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_direct_dynamic_conflict_rejected.objc3", "direct_dynamic", "O3S311", "cannot combine objc_direct and objc_dynamic"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_final_dynamic_conflict_rejected.objc3", "final_dynamic", "O3S312", "cannot combine objc_final and objc_dynamic"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_function_dispatch_intent_rejected.objc3", "function", "O3S313", "cannot use Part 9 dispatch-control callable attributes outside Objective-C methods"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_protocol_dispatch_intent_rejected.objc3", "protocol", "O3S314", "cannot use Part 9 dispatch-control callable attributes"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_category_method_dispatch_intent_rejected.objc3", "category_method", "O3S315", "cannot use Part 9 dispatch-control callable attributes"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_b003_category_container_dispatch_intent_rejected.objc3", "category_container", "O3S316", "cannot use objc_direct_members, objc_final, or objc_sealed container attributes"),
]
EXPECTED_COUNTS = {
    "callable_dispatch_intent_sites": 4,
    "container_dispatch_intent_sites": 1,
    "illegal_direct_dynamic_conflict_sites": 0,
    "illegal_final_dynamic_conflict_sites": 0,
    "illegal_non_method_callable_sites": 0,
    "illegal_protocol_method_sites": 0,
    "illegal_category_method_sites": 0,
    "illegal_category_container_sites": 0,
}
EXPECTED_TRUE_FIELDS = [
    "dependency_required",
    "callable_conflict_fail_closed",
    "unsupported_callable_topology_fail_closed",
    "unsupported_container_topology_fail_closed",
    "lowering_runtime_deferred",
    "deterministic",
    "ready_for_lowering_and_runtime",
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
        failures.append(Finding(display_path(path), "M272-B003-MISSING", f"missing artifact: {display_path(path)}"))
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
    assert isinstance(payload, dict)
    return payload


def extract_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == CONTRACT_ID, artifact, "M272-B003-PAYLOAD-01", "contract_id mismatch", failures)
    total += 1
    passed += require(payload.get("dependency_contract_id") == DEPENDENCY_CONTRACT_ID, artifact, "M272-B003-PAYLOAD-02", "dependency_contract_id mismatch", failures)
    total += 1
    passed += require(payload.get("surface_path") == SURFACE_PATH, artifact, "M272-B003-PAYLOAD-03", "surface_path mismatch", failures)
    for index, (field, expected) in enumerate(EXPECTED_COUNTS.items(), start=4):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M272-B003-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_TRUE_FIELDS, start=20):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M272-B003-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M272-B003-PAYLOAD-40", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M272-B003-PAYLOAD-41", "replay_key missing", failures)
    return total, passed


def run_negative_case(case_index: int, fixture: Path, label: str, code: str, phrase: str, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m272" / "b003" / label
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
    output = (run.stdout or "") + (run.stderr or "")
    total += 1
    passed += require(run.returncode != 0, display_path(fixture), f"M272-B003-DYN-{case_index:02d}", "negative fixture unexpectedly succeeded", failures)
    summary_path = out_dir / "module.c_api_summary.json"
    total += 1
    passed += require(summary_path.exists(), display_path(summary_path), f"M272-B003-DYN-{case_index + 1:02d}", "negative summary missing", failures)
    details: dict[str, Any] = {
        "fixture": display_path(fixture),
        "returncode": run.returncode,
        "output": output.strip(),
        "summary": display_path(summary_path),
    }
    if summary_path.exists():
        payload = load_json(summary_path)
        last_error = payload.get("last_error", "")
        diagnostics_path = payload.get("paths", {}).get("diagnostics", "")
        diagnostics_blob = last_error
        if diagnostics_path:
            diagnostics_file = ROOT / diagnostics_path
            if diagnostics_file.exists():
                diagnostics_blob += "\n" + diagnostics_file.read_text(encoding="utf-8")
                details["diagnostics"] = display_path(diagnostics_file)
        total += 1
        passed += require(code in diagnostics_blob, display_path(summary_path), f"M272-B003-DYN-{case_index + 2:02d}", f"missing {code}", failures)
        total += 1
        passed += require(phrase in diagnostics_blob, display_path(summary_path), f"M272-B003-DYN-{case_index + 3:02d}", f"missing wording: {phrase}", failures)
        details["last_error"] = last_error
    return total, passed, details


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m272-b003-readiness",
        "--summary-out",
        "tmp/reports/m272/M272-B003/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M272-B003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M272-B003-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m272" / "b003" / "positive"
    positive_out_dir.mkdir(parents=True, exist_ok=True)
    positive_run = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    positive_output = (positive_run.stdout or "") + (positive_run.stderr or "")
    total += 1
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M272-B003-DYN-03", f"positive fixture failed: {positive_output}", failures)

    manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M272-B003-DYN-04", "positive manifest missing", failures)
    positive_payload: dict[str, Any] = {}
    if manifest_path.exists():
        positive_payload = extract_payload(manifest_path)
        extra_total, extra_passed = validate_positive_payload(positive_payload, display_path(manifest_path), failures)
        total += extra_total
        passed += extra_passed

    negative_details: list[dict[str, Any]] = []
    next_index = 50
    for fixture, label, code, phrase in NEGATIVE_CASES:
        dyn_total, dyn_passed, details = run_negative_case(next_index, fixture, label, code, phrase, args, failures)
        total += dyn_total
        passed += dyn_passed
        negative_details.append(details)
        next_index += 10

    return total, passed, {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive_run.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part9_dispatch_intent_compatibility_packet": positive_payload,
        "negative_cases": negative_details,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M272-B003-EXP-01", "# M272 Compatibility Diagnostics For Dynamism Controls Edge-Case And Compatibility Completion Expectations (B003)"),
            SnippetCheck("M272-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M272-B003-EXP-03", "O3S316"),
        ],
        PACKET_DOC: [
            SnippetCheck("M272-B003-PKT-01", "# M272-B003 Packet: Compatibility Diagnostics For Dynamism Controls - Edge-Case And Compatibility Completion"),
            SnippetCheck("M272-B003-PKT-02", SURFACE_PATH),
            SnippetCheck("M272-B003-PKT-03", "O3S311"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M272-B003-GRM-01", "## M272 compatibility diagnostics for dynamism controls"),
            SnippetCheck("M272-B003-GRM-02", SURFACE_PATH),
        ],
        DOC_NATIVE: [
            SnippetCheck("M272-B003-DOC-01", "## M272 compatibility diagnostics for dynamism controls"),
            SnippetCheck("M272-B003-DOC-02", SURFACE_PATH),
        ],
        SPEC_ATTR: [
            SnippetCheck("M272-B003-ATTR-01", "## M272 dispatch-control compatibility diagnostics (B003)"),
            SnippetCheck("M272-B003-ATTR-02", "O3S316"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M272-B003-DEC-01", "## D-028: Part 9 dispatch-intent compatibility must fail closed on unsupported topologies before lowering begins"),
            SnippetCheck("M272-B003-DEC-02", "O3S313"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M272-B003-SEMA-01", "kObjc3Part9DispatchIntentCompatibilitySummaryContractId"),
            SnippetCheck("M272-B003-SEMA-02", "struct Objc3Part9DispatchIntentCompatibilitySummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M272-B003-SEMAH-01", "BuildPart9DispatchIntentCompatibilitySummary("),
        ],
        SEMA_CPP: [
            SnippetCheck("M272-B003-SEMACPP-01", "ValidatePart9DispatchIntentCompatibility("),
            SnippetCheck("M272-B003-SEMACPP-02", '"O3S311"'),
            SnippetCheck("M272-B003-SEMACPP-03", '"O3S316"'),
            SnippetCheck("M272-B003-SEMACPP-04", "BuildPart9DispatchIntentCompatibilitySummary("),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M272-B003-TYPE-01", "part9_dispatch_intent_compatibility_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M272-B003-PIPE-01", "BuildPart9DispatchIntentCompatibilitySummary("),
            SnippetCheck("M272-B003-PIPE-02", "result.part9_dispatch_intent_compatibility_summary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M272-B003-ART-01", "BuildPart9DispatchIntentCompatibilitySummaryJson("),
            SnippetCheck("M272-B003-ART-02", SURFACE_KEY),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M272-B003-PKG-01", '"check:objc3c:m272-b003-compatibility-diagnostics-for-dynamism-controls-edge-case-and-compatibility-completion"'),
            SnippetCheck("M272-B003-PKG-02", '"check:objc3c:m272-b003-lane-b-readiness"'),
        ],
    }

    for path, snippet_checks in snippets.items():
        checks_total += len(snippet_checks)
        checks_passed += ensure_snippets(path, snippet_checks, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        **dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
