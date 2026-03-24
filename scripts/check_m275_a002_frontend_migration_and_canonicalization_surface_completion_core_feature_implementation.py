#!/usr/bin/env python3
"""Checker for M275-A002 frontend migration/canonicalization surface completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-a002-part12-migration-canonicalization-source-completion-v1"
CONTRACT_ID = "objc3c-part12-migration-canonicalization-source-completion/m275-a002-v1"
DEPENDENCY_CONTRACT_ID = "objc3c-part12-diagnostics-fixit-migrator-source-inventory/m275-a001-v1"
SURFACE_KEY = "objc_part12_migration_and_canonicalization_source_completion"
DEPENDENCY_SURFACE_KEY = "objc_part12_diagnostics_fixit_and_migrator_source_inventory"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m275" / "M275-A002" / "frontend_migration_canonicalization_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_frontend_migration_and_canonicalization_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_a002_frontend_migration_and_canonicalization_surface_completion_core_feature_implementation_packet.md"
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
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_a002_frontend_migration_canonicalization_positive.objc3"


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
        failures.append(Finding(display_path(path), "M275-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m275-a002-readiness",
        "--summary-out",
        "tmp/reports/m275/M275-A002/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M275-A002-DYN-01", ensure_build.stderr or ensure_build.stdout or "fast build failed", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "a002" / "positive"
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
        "--objc3-compat-mode",
        "legacy",
        "--objc3-migration-assist",
    ])
    manifest_path = out_dir / "module.manifest.json"
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"

    checks_total += 4
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M275-A002-DYN-02", completed.stderr or completed.stdout or "frontend runner failed", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M275-A002-DYN-03", "manifest missing", failures)
    checks_passed += require(summary_path.exists(), display_path(summary_path), "M275-A002-DYN-04", "summary missing", failures)
    checks_passed += require(diagnostics_path.exists(), display_path(diagnostics_path), "M275-A002-DYN-05", "diagnostics missing", failures)

    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
    summary_payload = load_json(summary_path) if summary_path.exists() else {}
    diagnostics_payload = load_json(diagnostics_path) if diagnostics_path.exists() else {}

    frontend = manifest_payload.get("frontend", {}) if isinstance(manifest_payload, dict) else {}
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    dependency_packet = semantic_surface.get(DEPENDENCY_SURFACE_KEY, {}) if isinstance(semantic_surface, dict) else {}
    packet = semantic_surface.get(SURFACE_KEY, {}) if isinstance(semantic_surface, dict) else {}

    checks_total += 17
    checks_passed += require(frontend.get("compatibility_mode") == "legacy", display_path(manifest_path), "M275-A002-DYN-06", "frontend compatibility mode mismatch", failures)
    checks_passed += require(frontend.get("migration_assist") is True, display_path(manifest_path), "M275-A002-DYN-07", "migration assist should be enabled", failures)
    checks_passed += require(isinstance(packet, dict), display_path(manifest_path), "M275-A002-DYN-08", "part12 completion packet missing", failures)
    checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M275-A002-DYN-09", "contract id mismatch", failures)
    checks_passed += require(packet.get("dependency_contract_id") == DEPENDENCY_CONTRACT_ID, display_path(manifest_path), "M275-A002-DYN-10", "dependency contract mismatch", failures)
    checks_passed += require(packet.get("compatibility_mode") == "legacy", display_path(manifest_path), "M275-A002-DYN-11", "packet compatibility mode mismatch", failures)
    checks_passed += require(packet.get("migration_assist_enabled") is True, display_path(manifest_path), "M275-A002-DYN-12", "packet migration assist mismatch", failures)
    checks_passed += require(packet.get("legacy_yes_sites") == 1, display_path(manifest_path), "M275-A002-DYN-13", "legacy YES count drifted", failures)
    checks_passed += require(packet.get("legacy_no_sites") == 1, display_path(manifest_path), "M275-A002-DYN-14", "legacy NO count drifted", failures)
    checks_passed += require(packet.get("legacy_null_sites") == 1, display_path(manifest_path), "M275-A002-DYN-15", "legacy NULL count drifted", failures)
    checks_passed += require(packet.get("legacy_total_sites") == 3, display_path(manifest_path), "M275-A002-DYN-16", "legacy total drifted", failures)
    checks_passed += require(packet.get("canonicalization_candidate_sites") == 3, display_path(manifest_path), "M275-A002-DYN-17", "canonicalization candidate count drifted", failures)
    checks_passed += require(packet.get("fixit_candidate_sites") == 3, display_path(manifest_path), "M275-A002-DYN-18", "fix-it candidate count drifted", failures)
    checks_passed += require(packet.get("migrator_candidate_sites") == 3, display_path(manifest_path), "M275-A002-DYN-19", "migrator candidate count drifted", failures)
    checks_passed += require(packet.get("dependency_inventory_ready") is True, display_path(manifest_path), "M275-A002-DYN-20", "dependency inventory not ready", failures)
    checks_passed += require(packet.get("deterministic_handoff") is True, display_path(manifest_path), "M275-A002-DYN-21", "deterministic handoff missing", failures)
    checks_passed += require(packet.get("canonicalization_candidate_sites") == dependency_packet.get("canonicalization_hint_sites"), display_path(manifest_path), "M275-A002-DYN-22", "dependency canonicalization handoff drifted", failures)

    checks_total += 2
    checks_passed += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M275-A002-DYN-23", "semantic stage should run", failures)
    diag_list = diagnostics_payload.get("diagnostics", []) if isinstance(diagnostics_payload, dict) else []
    checks_passed += require(isinstance(diag_list, list) and len(diag_list) == 0, display_path(diagnostics_path), "M275-A002-DYN-24", "positive fixture emitted diagnostics", failures)

    return checks_total, checks_passed, {
        "manifest": display_path(manifest_path),
        "summary": display_path(summary_path),
        "diagnostics": display_path(diagnostics_path),
        "returncode": completed.returncode,
        "packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-A002-EXP-01", "# M275 Frontend Migration and Canonicalization Surface Completion - Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M275-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-A002-PKT-01", "# M275-A002 Packet: Frontend migration and canonicalization surface completion - Core feature implementation"),
            SnippetCheck("M275-A002-PKT-02", "frontend.pipeline.semantic_surface.objc_part12_migration_and_canonicalization_source_completion"),
            SnippetCheck("M275-A002-PKT-03", "M275-B001"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M275-A002-GRM-01", "Current implementation status (`M275-A002`):"),
            SnippetCheck("M275-A002-GRM-02", "objc_part12_migration_and_canonicalization_source_completion"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-A002-DOC-01", "Current implementation status (`M275-A002`):"),
            SnippetCheck("M275-A002-DOC-02", "objc_part12_migration_and_canonicalization_source_completion"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-A002-CHK-01", "objc_part12_migration_and_canonicalization_source_completion"),
            SnippetCheck("M275-A002-CHK-02", "legacy `YES` / `NO` / `NULL` canonicalization now has a dedicated"),
        ],
        SPEC_ABSTRACT: [
            SnippetCheck("M275-A002-ABS-01", "M275-A002 migration/canonicalization completion note:"),
            SnippetCheck("M275-A002-ABS-02", "objc_part12_migration_and_canonicalization_source_completion"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M275-A002-TOK-01", "kObjc3Part12MigrationCanonicalizationSourceCompletionContractId"),
            SnippetCheck("M275-A002-TOK-02", "kObjc3SourceOnlyFeatureClaimAdvancedCanonicalizationInventory"),
        ],
        LEXER_CPP: [
            SnippetCheck("M275-A002-LEX-01", "M275-A001/A002 source note:"),
        ],
        PARSER_CPP: [
            SnippetCheck("M275-A002-PAR-01", "M275-A001/A002 source note:"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M275-A002-TYPE-01", "kObjc3Part12MigrationCanonicalizationSourceCompletionSurfacePath"),
            SnippetCheck("M275-A002-TYPE-02", "struct Objc3FrontendPart12MigrationCanonicalizationSourceCompletionSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M275-A002-PIPE-01", "BuildPart12MigrationCanonicalizationSourceCompletionSummary"),
            SnippetCheck("M275-A002-PIPE-02", "BuildPart12MigrationCanonicalizationSourceCompletionReplayKey"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M275-A002-ART-01", "BuildPart12MigrationCanonicalizationSourceCompletionSummaryJson"),
            SnippetCheck("M275-A002-ART-02", "objc_part12_migration_and_canonicalization_source_completion"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-A002-PKG-01", '"check:objc3c:m275-a002-frontend-migration-and-canonicalization-surface-completion"'),
            SnippetCheck("M275-A002-PKG-02", '"check:objc3c:m275-a002-lane-a-readiness"'),
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
        "next_issue": "M275-B001",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M275-A002 frontend migration/canonicalization checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
