#!/usr/bin/env python3
"""Checker for M268-A002 frontend async semantic packet."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-a002-part7-async-semantic-packet-v1"
CONTRACT_ID = "objc3c-part7-async-source-closure/m268-a002-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m268" / "M268-A002" / "async_semantic_packet_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_frontend_async_entry_and_await_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_a002_frontend_async_entry_and_await_surface_completion_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_async_await_executor_source_closure_positive.objc3"

EXPECTED_COUNTS = {
    "async_keyword_sites": 3,
    "async_function_sites": 1,
    "async_method_sites": 2,
    "await_keyword_sites": 2,
    "await_expression_sites": 2,
    "executor_attribute_sites": 3,
    "executor_main_sites": 2,
    "executor_global_sites": 0,
    "executor_named_sites": 1,
}

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
        failures.append(Finding(display_path(path), "M268-A002-MISSING", f"missing artifact: {display_path(path)}"))
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


def surface_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part7_async_source_closure"]


def validate_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    exact = {
        "contract_id": CONTRACT_ID,
        "frontend_surface_path": "frontend.pipeline.semantic_surface.objc_part7_async_source_closure",
        **EXPECTED_COUNTS,
    }
    for index, (field, expected) in enumerate(exact.items(), start=1):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M268-A002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate([
        "async_function_source_supported",
        "async_method_source_supported",
        "await_expression_source_supported",
        "executor_attribute_source_supported",
        "deterministic_handoff",
        "ready_for_semantic_expansion",
    ], start=20):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M268-A002-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M268-A002-PAYLOAD-40", "replay key missing", failures)
    return total, passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_command([sys.executable, "scripts/ensure_objc3c_native_build.py", "--mode", "fast", "--reason", "m268-a002-readiness", "--summary-out", "tmp/reports/m268/M268-A002/ensure_build_summary.json"])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M268-A002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M268-A002-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "a002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([str(args.runner_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    output = (run.stdout or "") + (run.stderr or "")
    total += 1
    passed += require(run.returncode == 0, display_path(FIXTURE), "M268-A002-DYN-03", f"positive fixture failed: {output}", failures)
    manifest_path = out_dir / "module.manifest.json"
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M268-A002-DYN-04", "positive manifest missing", failures)
    dynamic: dict[str, Any] = {"positive_fixture": display_path(FIXTURE), "positive_returncode": run.returncode, "positive_output": output.strip(), "positive_manifest": display_path(manifest_path)}
    if manifest_path.exists():
        payload = surface_payload(manifest_path)
        sub_total, sub_passed = validate_payload(payload, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["part7_async_source_closure"] = payload
    return total, passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    total = 0
    passed = 0
    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M268-A002-EXP-01", "# M268 Frontend Async Entry And Await Surface Completion Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M268-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M268-A002-EXP-03", "frontend.pipeline.semantic_surface.objc_part7_async_source_closure"),
        ],
        PACKET_DOC: [
            SnippetCheck("M268-A002-PKT-01", "# M268-A002 Packet: Frontend Async Entry And Await Surface Completion - Core Feature Implementation"),
            SnippetCheck("M268-A002-PKT-02", "publish a dedicated frontend semantic surface for async entry points"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M268-A002-GRM-01", "## M268 frontend async semantic packet"),
            SnippetCheck("M268-A002-GRM-02", "frontend.pipeline.semantic_surface.objc_part7_async_source_closure"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M268-A002-DOC-01", "## M268 frontend async semantic packet"),
            SnippetCheck("M268-A002-DOC-02", "frontend.pipeline.semantic_surface.objc_part7_async_source_closure"),
        ],
        SPEC_AM: [
            SnippetCheck("M268-A002-AM-01", "M268-A002 semantic-packet note:"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M268-A002-ATTR-01", "Current implementation status (`M268-A002`):"),
            SnippetCheck("M268-A002-ATTR-02", "frontend.pipeline.semantic_surface.objc_part7_async_source_closure"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M268-A002-TYP-01", "kObjc3Part7AsyncSourceClosureContractId"),
            SnippetCheck("M268-A002-TYP-02", "struct Objc3FrontendPart7AsyncSourceClosureSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M268-A002-PIPE-01", "BuildPart7AsyncSourceClosureSummary("),
            SnippetCheck("M268-A002-PIPE-02", "BuildPart7AsyncSourceClosureReplayKey("),
            SnippetCheck("M268-A002-PIPE-03", "result.part7_async_source_closure_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M268-A002-ART-01", "BuildPart7AsyncSourceClosureSummaryJson("),
            SnippetCheck("M268-A002-ART-02", "part7_async_source_closure_summary)"),
        ],
        PARSER_CPP: [
            SnippetCheck("M268-A002-PARSE-01", "rhs->await_expression_enabled = true;"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M268-A002-PKG-01", '"check:objc3c:m268-a002-frontend-async-entry-and-await-surface-completion-core-feature-implementation"'),
            SnippetCheck("M268-A002-PKG-02", '"check:objc3c:m268-a002-lane-a-readiness"'),
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
    summary = {"mode": MODE, "contract_id": CONTRACT_ID, "ok": not failures, "checks_total": total, "checks_passed": passed, "checks_failed": len(failures), "dynamic_probes_executed": dynamic_executed, "failures": [finding.__dict__ for finding in failures], "dynamic": dynamic}
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}")
        print(f"[info] wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"[ok] M268-A002 async semantic packet checks passed ({passed}/{total})")
    print(f"[info] wrote summary: {display_path(args.summary_out)}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
