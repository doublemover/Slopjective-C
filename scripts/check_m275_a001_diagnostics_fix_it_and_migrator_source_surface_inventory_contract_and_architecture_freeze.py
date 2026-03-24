#!/usr/bin/env python3
"""Checker for M275-A001 diagnostics/fix-it/migrator source inventory."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-a001-part12-diagnostics-fixit-migrator-source-inventory-v1"
CONTRACT_ID = "objc3c-part12-diagnostics-fixit-migrator-source-inventory/m275-a001-v1"
SURFACE_KEY = "objc_part12_diagnostics_fixit_and_migrator_source_inventory"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m275" / "M275-A001" / "diagnostics_fixit_migrator_source_inventory_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_diagnostics_fix_it_and_migrator_source_surface_inventory_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_a001_diagnostics_fix_it_and_migrator_source_surface_inventory_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_ABSTRACT = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_a001_diagnostics_migrator_source_inventory_positive.objc3"


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M275-A001-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m275-a001-readiness",
        "--summary-out",
        "tmp/reports/m275/M275-A001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M275-A001-DYN-01", ensure_build.stderr or ensure_build.stdout or "fast build failed", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M275-A001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "a001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    manifest_path = out_dir / "module.manifest.json"
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    checks_total += 3
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M275-A001-DYN-03", completed.stderr or completed.stdout or "frontend runner failed", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M275-A001-DYN-04", "manifest missing", failures)
    checks_passed += require(summary_path.exists(), display_path(summary_path), "M275-A001-DYN-05", "summary missing", failures)
    checks_total += 1
    checks_passed += require(diagnostics_path.exists(), display_path(diagnostics_path), "M275-A001-DYN-06", "diagnostics missing", failures)

    manifest_payload: dict[str, Any] = {}
    summary_payload: dict[str, Any] = {}
    diagnostics_payload: dict[str, Any] = {}
    if manifest_path.exists():
        manifest_payload = load_json(manifest_path)
    if summary_path.exists():
        summary_payload = load_json(summary_path)
    if diagnostics_path.exists():
        diagnostics_payload = load_json(diagnostics_path)

    frontend_payload = manifest_payload.get("frontend", {}) if isinstance(manifest_payload, dict) else {}
    pipeline_payload = frontend_payload.get("pipeline", {}) if isinstance(frontend_payload, dict) else {}
    semantic_surface = pipeline_payload.get("semantic_surface", {}) if isinstance(pipeline_payload, dict) else {}
    part12 = semantic_surface.get(SURFACE_KEY, {}) if isinstance(semantic_surface, dict) else {}

    checks_total += 10
    checks_passed += require(isinstance(part12, dict), display_path(manifest_path), "M275-A001-DYN-07", "part12 semantic surface missing", failures)
    checks_passed += require(part12.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M275-A001-DYN-08", "contract id mismatch", failures)
    checks_passed += require(part12.get("advanced_feature_family_count") == 6, display_path(manifest_path), "M275-A001-DYN-09", "advanced feature family count drifted", failures)
    checks_passed += require(part12.get("dependency_surface_count") == 14, display_path(manifest_path), "M275-A001-DYN-10", "dependency surface count drifted", failures)
    checks_passed += require(part12.get("diagnostic_surface_sites", 0) > 0, display_path(manifest_path), "M275-A001-DYN-11", "diagnostic surface inventory is empty", failures)
    checks_passed += require(part12.get("error_surface_sites", 0) > 0, display_path(manifest_path), "M275-A001-DYN-12", "part6 surfaces were not aggregated", failures)
    checks_passed += require(part12.get("concurrency_surface_sites", 0) > 0, display_path(manifest_path), "M275-A001-DYN-13", "part7 surfaces were not aggregated", failures)
    checks_passed += require(part12.get("system_surface_sites", 0) > 0, display_path(manifest_path), "M275-A001-DYN-14", "part8 surfaces were not aggregated", failures)
    checks_passed += require(part12.get("metaprogramming_surface_sites", 0) > 0, display_path(manifest_path), "M275-A001-DYN-15", "part10 surfaces were not aggregated", failures)
    checks_passed += require(part12.get("interop_surface_sites", 0) > 0, display_path(manifest_path), "M275-A001-DYN-16", "part11 surfaces were not aggregated", failures)

    checks_total += 5
    checks_passed += require(part12.get("diagnostics_inventory_source_supported") is True, display_path(manifest_path), "M275-A001-DYN-17", "diagnostic inventory support flag missing", failures)
    checks_passed += require(part12.get("fixit_inventory_source_supported") is True, display_path(manifest_path), "M275-A001-DYN-18", "fix-it inventory support flag missing", failures)
    checks_passed += require(part12.get("migrator_inventory_source_supported") is True, display_path(manifest_path), "M275-A001-DYN-19", "migrator inventory support flag missing", failures)
    checks_passed += require(part12.get("deterministic_handoff") is True, display_path(manifest_path), "M275-A001-DYN-20", "deterministic handoff flag missing", failures)
    checks_passed += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M275-A001-DYN-21", "semantic stage should run", failures)

    checks_total += 1
    diag_list = diagnostics_payload.get("diagnostics", []) if isinstance(diagnostics_payload, dict) else []
    checks_passed += require(isinstance(diag_list, list) and len(diag_list) == 0, display_path(diagnostics_path), "M275-A001-DYN-22", "positive fixture emitted diagnostics", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "manifest": display_path(manifest_path),
        "summary": display_path(summary_path),
        "diagnostics": display_path(diagnostics_path),
        "returncode": completed.returncode,
        "part12_surface": part12,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-A001-EXP-01", "# M275 Diagnostics, Fix-It, and Migrator Source Surface Inventory Contract and Architecture Freeze Expectations (A001)"),
            SnippetCheck("M275-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-A001-PKT-01", "# M275-A001 Packet: Diagnostics, Fix-It, and Migrator Source Surface Inventory - Contract and Architecture Freeze"),
            SnippetCheck("M275-A001-PKT-02", "frontend.pipeline.semantic_surface.objc_part12_diagnostics_fixit_and_migrator_source_inventory"),
            SnippetCheck("M275-A001-PKT-03", "M275-A002"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M275-A001-GRM-01", "## M275 diagnostics, fix-it, and migrator source surface inventory"),
            SnippetCheck("M275-A001-GRM-02", "frontend.pipeline.semantic_surface.objc_part12_diagnostics_fixit_and_migrator_source_inventory"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-A001-DOC-01", "## M275 diagnostics, fix-it, and migrator source surface inventory"),
            SnippetCheck("M275-A001-DOC-02", "objc_part12_diagnostics_fixit_and_migrator_source_inventory"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-A001-CHK-01", "## M275 advanced diagnostics/fix-it/migrator inventory (implementation note)"),
            SnippetCheck("M275-A001-CHK-02", "frontend.pipeline.semantic_surface.objc_part12_diagnostics_fixit_and_migrator_source_inventory"),
        ],
        SPEC_ABSTRACT: [
            SnippetCheck("M275-A001-ABS-01", "M275-A001 source-inventory note:"),
            SnippetCheck("M275-A001-ABS-02", "frontend.pipeline.semantic_surface.objc_part12_diagnostics_fixit_and_migrator_source_inventory"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M275-A001-TOK-01", "kObjc3Part12DiagnosticsMigratorSourceInventoryContractId"),
            SnippetCheck("M275-A001-TOK-02", "kObjc3SourceOnlyFeatureClaimAdvancedMigratorInventory"),
        ],
        LEXER_CPP: [
            SnippetCheck("M275-A001-LEX-01", "M275-A001 source-inventory note:"),
        ],
        PARSER_CPP: [
            SnippetCheck("M275-A001-PAR-01", "M275-A001 source-inventory note:"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M275-A001-TYPE-01", "kObjc3Part12DiagnosticsMigratorSourceInventorySurfacePath"),
            SnippetCheck("M275-A001-TYPE-02", "struct Objc3FrontendPart12DiagnosticsMigratorSourceInventorySummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M275-A001-PIPE-01", "BuildPart12DiagnosticsMigratorSourceInventorySummary"),
            SnippetCheck("M275-A001-PIPE-02", "BuildPart12DiagnosticsMigratorSourceInventoryReplayKey"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M275-A001-ART-01", "BuildPart12DiagnosticsMigratorSourceInventorySummaryJson"),
            SnippetCheck("M275-A001-ART-02", "objc_part12_diagnostics_fixit_and_migrator_source_inventory"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-A001-PKG-01", '"check:objc3c:m275-a001-diagnostics-fix-it-and-migrator-source-surface-inventory-contract-and-architecture-freeze"'),
            SnippetCheck("M275-A001-PKG-02", '"check:objc3c:m275-a001-lane-a-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, object] = {"skipped": True}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_summary": dynamic_summary,
        "next_issue": "M275-A002",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M275-A001 source inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
