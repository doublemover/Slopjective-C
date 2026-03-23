#!/usr/bin/env python3
"""Checker for M268-B002 await suspension and resume semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-b002-part7-await-suspension-resume-semantics-v1"
CONTRACT_ID = "objc3c-part7-await-suspension-resume-semantics/m268-b002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m268" / "M268-B002" / "await_suspension_resume_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_await_suspension_and_resume_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_b002_await_suspension_and_resume_semantics_core_feature_implementation_packet.md"
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
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_b002_non_async_await_rejected.objc3"

EXPECTED_POSITIVE = {
    "contract_id": CONTRACT_ID,
    "dependency_contract_id": "objc3c-part7-async-effect-suspension-semantic-model/m268-b001-v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics",
    "async_callable_sites": 3,
    "await_expression_sites": 2,
    "await_in_async_callable_sites": 2,
    "illegal_await_sites": 0,
    "await_suspension_point_sites": 1,
    "await_resume_sites": 0,
    "continuation_resume_sites": 0,
    "continuation_suspend_sites": 0,
}

EXPECTED_BOOLEAN_FIELDS = [
    "source_dependency_required",
    "await_placement_enforced",
    "suspension_profile_enforced",
    "resume_profile_enforced",
    "non_async_await_fail_closed",
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
        failures.append(Finding(display_path(path), "M268-B002-MISSING", f"missing artifact: {display_path(path)}"))
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
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_await_suspension_and_resume_semantics"]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected) in enumerate(EXPECTED_POSITIVE.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M268-B002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_BOOLEAN_FIELDS, start=30):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M268-B002-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M268-B002-PAYLOAD-50", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M268-B002-PAYLOAD-51", "replay key missing", failures)
    return total, passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m268-b002-readiness",
        "--summary-out",
        "tmp/reports/m268/M268-B002/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M268-B002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M268-B002-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "b002" / "positive"
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
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M268-B002-DYN-03", f"positive fixture failed: {positive_output}", failures)
    positive_manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(positive_manifest_path.exists(), display_path(positive_manifest_path), "M268-B002-DYN-04", "positive manifest missing", failures)

    negative_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "b002" / "negative"
    negative_out_dir.mkdir(parents=True, exist_ok=True)
    negative_run = run_command([
        str(args.runner_exe),
        str(NEGATIVE_FIXTURE),
        "--out-dir",
        str(negative_out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    negative_output = (negative_run.stdout or "") + (negative_run.stderr or "")
    total += 1
    passed += require(negative_run.returncode != 0, display_path(NEGATIVE_FIXTURE), "M268-B002-DYN-05", "negative fixture unexpectedly succeeded", failures)
    negative_summary_path = negative_out_dir / "module.c_api_summary.json"
    total += 1
    passed += require(negative_summary_path.exists(), display_path(negative_summary_path), "M268-B002-DYN-06", "negative summary missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive_run.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(positive_manifest_path),
        "negative_fixture": display_path(NEGATIVE_FIXTURE),
        "negative_returncode": negative_run.returncode,
        "negative_output": negative_output.strip(),
        "negative_summary": display_path(negative_summary_path),
    }
    if positive_manifest_path.exists():
        payload = surface_payload(positive_manifest_path)
        sub_total, sub_passed = validate_positive_payload(payload, display_path(positive_manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["part7_await_suspension_and_resume_semantics"] = payload
    if negative_summary_path.exists():
        negative_summary = load_json(negative_summary_path)
        last_error = negative_summary.get("last_error", "")
        total += 1
        passed += require("O3S223" in last_error, display_path(negative_summary_path), "M268-B002-DYN-07", "negative summary missing O3S223", failures)
        total += 1
        passed += require("await" in last_error and "async function or method" in last_error, display_path(negative_summary_path), "M268-B002-DYN-08", "negative summary missing await placement wording", failures)
        dynamic["negative_last_error"] = last_error
    return total, passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M268-B002-EXP-01", "# M268 Await Suspension And Resume Semantics Core Feature Implementation Expectations (B002)"),
            SnippetCheck("M268-B002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M268-B002-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics"),
            SnippetCheck("M268-B002-EXP-04", "O3S223"),
        ],
        PACKET_DOC: [
            SnippetCheck("M268-B002-PKT-01", "# M268-B002 Packet: Await Suspension And Resume Semantics - Core Feature Implementation"),
            SnippetCheck("M268-B002-PKT-02", "reject `await` outside async functions and Objective-C methods"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M268-B002-GRM-01", "## M268 await suspension and resume semantics"),
            SnippetCheck("M268-B002-GRM-02", "frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M268-B002-DOC-01", "## M268 await suspension and resume semantics"),
            SnippetCheck("M268-B002-DOC-02", "frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics"),
        ],
        SPEC_AM: [
            SnippetCheck("M268-B002-AM-01", "M268-B002 semantic-enforcement note:"),
            SnippetCheck("M268-B002-AM-02", "objc_part7_await_suspension_and_resume_semantics"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M268-B002-ATTR-01", "Current implementation status (`M268-B002`):"),
            SnippetCheck("M268-B002-ATTR-02", "non-async `await` sites fail closed with diagnostic `O3S223`"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M268-B002-SEMA-01", "kObjc3Part7AwaitSuspensionResumeSemanticSummaryContractId"),
            SnippetCheck("M268-B002-SEMA-02", "struct Objc3Part7AwaitSuspensionResumeSemanticSummary"),
            SnippetCheck("M268-B002-SEMA-03", "IsReadyObjc3Part7AwaitSuspensionResumeSemanticSummary("),
        ],
        SEMA_PASSES_H: [
            SnippetCheck("M268-B002-SEMAH-01", "BuildPart7AwaitSuspensionResumeSemanticSummary("),
        ],
        SEMA_PASSES_CPP: [
            SnippetCheck("M268-B002-SEMACPP-01", "BuildPart7AwaitSuspensionResumeSemanticSummary("),
            SnippetCheck("M268-B002-SEMACPP-02", "summary.illegal_await_sites = count_diagnostic_code(\"O3S223\");"),
            SnippetCheck("M268-B002-SEMACPP-03", "async semantics failed: 'await' is only valid inside an async function or method"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M268-B002-TYP-01", "part7_await_suspension_resume_semantic_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M268-B002-PIPE-01", "result.part7_await_suspension_resume_semantic_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M268-B002-ART-01", "BuildPart7AwaitSuspensionResumeSemanticSummaryJson("),
            SnippetCheck("M268-B002-ART-02", "objc_part7_await_suspension_and_resume_semantics"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M268-B002-PKG-01", "\"check:objc3c:m268-b002-await-suspension-and-resume-semantics-core-feature-implementation\""),
            SnippetCheck("M268-B002-PKG-02", "\"check:objc3c:m268-b002-lane-b-readiness\""),
        ],
        NEGATIVE_FIXTURE: [
            SnippetCheck("M268-B002-FIX-01", "return await fetch();"),
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
    print(f"[ok] M268-B002 await suspension and resume semantics checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
