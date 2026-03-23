#!/usr/bin/env python3
"""Checker for M268-B003 async diagnostics and compatibility completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-b003-part7-async-diagnostics-compatibility-v1"
CONTRACT_ID = "objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m268" / "M268-B003" / "async_diagnostics_compatibility_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_async_diagnostics_and_compatibility_completion_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_b003_async_diagnostics_and_compatibility_completion_edge_case_and_compatibility_completion_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_async_await_executor_source_closure_positive.objc3"
NEGATIVE_EXECUTOR = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_b003_executor_on_sync_rejected.objc3"
NEGATIVE_PROTO = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_b003_async_prototype_rejected.objc3"
NEGATIVE_THROWS = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_b003_async_throws_rejected.objc3"

EXPECTED_POSITIVE = {
    "contract_id": CONTRACT_ID,
    "dependency_contract_id": "objc3c-part7-await-suspension-resume-semantics/m268-b002-v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion",
    "async_callable_sites": 3,
    "executor_affinity_sites": 3,
    "illegal_non_async_executor_sites": 0,
    "illegal_async_function_prototype_sites": 0,
    "illegal_async_throws_sites": 0,
    "compatibility_diagnostic_sites": 0,
    "supported_async_callable_sites": 3,
}

EXPECTED_BOOLEAN_FIELDS = [
    "dependency_required",
    "executor_affinity_requires_async_enforced",
    "async_function_prototypes_fail_closed",
    "async_throws_fail_closed",
    "unsupported_topology_fail_closed",
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
        failures.append(Finding(display_path(path), "M268-B003-MISSING", f"missing artifact: {display_path(path)}"))
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
    assert isinstance(payload, dict)
    return payload


def surface_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_async_diagnostics_and_compatibility_completion"]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_POSITIVE.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M268-B003-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_BOOLEAN_FIELDS, start=30):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M268-B003-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M268-B003-PAYLOAD-50", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M268-B003-PAYLOAD-51", "replay key missing", failures)
    return total, passed


def run_negative_probe(runner_exe: Path, fixture: Path, out_dir: Path) -> tuple[int, str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(runner_exe),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    summary = load_json(out_dir / "module.c_api_summary.json")
    return run.returncode, str(summary.get("last_error", ""))


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m268-b003-readiness",
        "--summary-out",
        "tmp/reports/m268/M268-B003/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M268-B003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M268-B003-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "b003" / "positive"
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
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M268-B003-DYN-03", f"positive fixture failed: {positive_output}", failures)
    positive_manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M268-B003-DYN-04", "positive manifest missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive_run.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(positive_manifest_path),
    }
    if positive_manifest_path.exists():
        payload = surface_payload(positive_manifest_path)
        sub_total, sub_passed = validate_positive_payload(payload, display_path(positive_manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["part7_async_diagnostics_and_compatibility_completion"] = payload

    negatives = [
        (NEGATIVE_EXECUTOR, "O3S224", "objc_executor requires an async function or method", "executor_on_sync"),
        (NEGATIVE_PROTO, "O3S225", "async function prototypes remain unsupported", "async_prototype"),
        (NEGATIVE_THROWS, "O3S226", "async throws functions remain unsupported", "async_throws"),
    ]
    for index, (fixture, code, wording, key) in enumerate(negatives, start=5):
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "b003" / key
        returncode, last_error = run_negative_probe(args.runner_exe, fixture, out_dir)
        total += 1
        passed += require(returncode != 0, display_path(fixture), f"M268-B003-DYN-{index:02d}", "negative fixture unexpectedly succeeded", failures)
        total += 1
        passed += require(code in last_error, display_path(out_dir / 'module.c_api_summary.json'), f"M268-B003-DYN-{index+10:02d}", f"negative summary missing {code}", failures)
        total += 1
        passed += require(wording in last_error, display_path(out_dir / 'module.c_api_summary.json'), f"M268-B003-DYN-{index+20:02d}", f"negative summary missing wording: {wording}", failures)
        dynamic[key] = {
            "fixture": display_path(fixture),
            "returncode": returncode,
            "last_error": last_error,
            "summary": display_path(out_dir / "module.c_api_summary.json"),
        }
    return total, passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M268-B003-EXP-01", "# M268 Async Diagnostics And Compatibility Completion Edge-Case And Compatibility Completion Expectations (B003)"),
            SnippetCheck("M268-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M268-B003-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion"),
            SnippetCheck("M268-B003-EXP-04", "O3S224"),
            SnippetCheck("M268-B003-EXP-05", "O3S225"),
            SnippetCheck("M268-B003-EXP-06", "O3S226"),
        ],
        PACKET_DOC: [
            SnippetCheck("M268-B003-PKT-01", "# M268-B003 Packet: Async Diagnostics And Compatibility Completion - Edge-Case And Compatibility Completion"),
            SnippetCheck("M268-B003-PKT-02", "reject `objc_executor(...)` on non-async functions and Objective-C methods"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M268-B003-GRM-01", "## M268 async diagnostics and compatibility completion"),
            SnippetCheck("M268-B003-GRM-02", "objc_part7_async_diagnostics_and_compatibility_completion"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M268-B003-DOC-01", "## M268 async diagnostics and compatibility completion"),
            SnippetCheck("M268-B003-DOC-02", "objc_part7_async_diagnostics_and_compatibility_completion"),
        ],
        SPEC_AM: [
            SnippetCheck("M268-B003-AM-01", "M268-B003 compatibility note:"),
            SnippetCheck("M268-B003-AM-02", "non-async `objc_executor(...)`"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M268-B003-ATTR-01", "Current implementation status (`M268-B003`):"),
            SnippetCheck("M268-B003-ATTR-02", "async function prototypes fail closed with diagnostic `O3S225`"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M268-B003-SEMA-01", "kObjc3Part7AsyncDiagnosticsCompatibilitySummaryContractId"),
            SnippetCheck("M268-B003-SEMA-02", "struct Objc3Part7AsyncDiagnosticsCompatibilitySummary"),
            SnippetCheck("M268-B003-SEMA-03", "IsReadyObjc3Part7AsyncDiagnosticsCompatibilitySummary("),
        ],
        SEMA_PASSES_H: [
            SnippetCheck("M268-B003-SEMAH-01", "BuildPart7AsyncDiagnosticsCompatibilitySummary("),
        ],
        SEMA_PASSES_CPP: [
            SnippetCheck("M268-B003-SEMACPP-01", "BuildPart7AsyncDiagnosticsCompatibilitySummary("),
            SnippetCheck("M268-B003-SEMACPP-02", "O3S224"),
            SnippetCheck("M268-B003-SEMACPP-03", "O3S225"),
            SnippetCheck("M268-B003-SEMACPP-04", "O3S226"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M268-B003-TYP-01", "part7_async_diagnostics_compatibility_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M268-B003-PIPE-01", "result.part7_async_diagnostics_compatibility_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M268-B003-ART-01", "BuildPart7AsyncDiagnosticsCompatibilitySummaryJson("),
            SnippetCheck("M268-B003-ART-02", "objc_part7_async_diagnostics_and_compatibility_completion"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M268-B003-PKG-01", "\"check:objc3c:m268-b003-async-diagnostics-and-compatibility-completion-edge-case-and-compatibility-completion\""),
            SnippetCheck("M268-B003-PKG-02", "\"check:objc3c:m268-b003-lane-b-readiness\""),
        ],
        NEGATIVE_EXECUTOR: [
            SnippetCheck("M268-B003-FIX-01", "objc_executor(main)"),
        ],
        NEGATIVE_PROTO: [
            SnippetCheck("M268-B003-FIX-02", "async fn fetch() -> i32;"),
        ],
        NEGATIVE_THROWS: [
            SnippetCheck("M268-B003-FIX-03", "async fn main() throws -> i32"),
        ],
    }
    for path, required in snippets.items():
        total += len(required)
        passed += ensure_snippets(path, required, failures)

    dynamic = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": total,
        "checks_passed": passed,
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
    print(f"[ok] M268-B003 async diagnostics and compatibility completion checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
