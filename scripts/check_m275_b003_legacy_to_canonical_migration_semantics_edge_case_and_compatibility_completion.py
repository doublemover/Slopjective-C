#!/usr/bin/env python3
"""Checker for M275-B003 legacy/canonical migration semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-b003-part12-legacy-canonical-migration-semantics-v1"
CONTRACT_ID = "objc3c-part12-legacy-canonical-migration-semantics/m275-b003-v1"
DEPENDENCY_CONTRACT_ID = "objc3c-part12-feature-specific-fixit-synthesis/m275-b002-v1"
SURFACE_KEY = "objc_part12_legacy_canonical_migration_semantics"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m275" / "M275-B003" / "legacy_canonical_migration_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_legacy_to_canonical_migration_semantics_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_b003_legacy_to_canonical_migration_semantics_edge_case_and_compatibility_completion_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_ABSTRACT = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_b003_legacy_canonical_migration_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_b003_legacy_canonical_migration_negative.objc3"


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
        failures.append(Finding(display_path(path), "M275-B003-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m275-b003-readiness",
        "--summary-out",
        "tmp/reports/m275/M275-B003/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M275-B003-DYN-01", ensure_build.stderr or ensure_build.stdout or "fast build failed", failures)

    positive_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "b003" / "positive"
    positive_dir.mkdir(parents=True, exist_ok=True)
    positive_run = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
        "--objc3-compat-mode",
        "canonical",
        "--objc3-migration-assist",
    ])
    positive_manifest = positive_dir / "module.manifest.json"
    positive_summary = positive_dir / "module.c_api_summary.json"
    positive_diags = positive_dir / "module.diagnostics.json"

    checks_total += 4
    checks_passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M275-B003-DYN-02", positive_run.stderr or positive_run.stdout or "positive runner failed", failures)
    checks_passed += require(positive_manifest.exists(), display_path(positive_manifest), "M275-B003-DYN-03", "positive manifest missing", failures)
    checks_passed += require(positive_summary.exists(), display_path(positive_summary), "M275-B003-DYN-04", "positive summary missing", failures)
    checks_passed += require(positive_diags.exists(), display_path(positive_diags), "M275-B003-DYN-05", "positive diagnostics missing", failures)

    manifest_payload = load_json(positive_manifest) if positive_manifest.exists() else {}
    summary_payload = load_json(positive_summary) if positive_summary.exists() else {}
    diagnostics_payload = load_json(positive_diags) if positive_diags.exists() else {}
    frontend = manifest_payload.get("frontend", {}) if isinstance(manifest_payload, dict) else {}
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    packet = semantic_surface.get(SURFACE_KEY, {}) if isinstance(semantic_surface, dict) else {}

    checks_total += 14
    checks_passed += require(isinstance(packet, dict), display_path(positive_manifest), "M275-B003-DYN-06", "part12 migration packet missing", failures)
    checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(positive_manifest), "M275-B003-DYN-07", "contract id mismatch", failures)
    checks_passed += require(packet.get("dependency_contract_id") == DEPENDENCY_CONTRACT_ID, display_path(positive_manifest), "M275-B003-DYN-08", "dependency contract mismatch", failures)
    checks_passed += require(packet.get("effective_compatibility_mode") == "canonical", display_path(positive_manifest), "M275-B003-DYN-09", "positive mode should be canonical", failures)
    checks_passed += require(packet.get("migration_assist_enabled") is True, display_path(positive_manifest), "M275-B003-DYN-10", "migration assist should be enabled", failures)
    checks_passed += require(packet.get("current_run_legacy_literal_sites") == 0, display_path(positive_manifest), "M275-B003-DYN-11", "canonical happy path should not report legacy sites", failures)
    checks_passed += require(packet.get("current_run_canonicalization_candidate_sites") == 0, display_path(positive_manifest), "M275-B003-DYN-12", "canonical happy path should not report rewrite candidates", failures)
    checks_passed += require(packet.get("fixit_family_count") == 2, display_path(positive_manifest), "M275-B003-DYN-13", "fix-it family count drifted", failures)
    checks_passed += require(packet.get("fail_closed") is True, display_path(positive_manifest), "M275-B003-DYN-14", "fail-closed flag missing", failures)
    checks_passed += require(packet.get("compatibility_mode_semantics_landed") is True, display_path(positive_manifest), "M275-B003-DYN-15", "compatibility semantics flag missing", failures)
    checks_passed += require(packet.get("migration_assist_semantics_landed") is True, display_path(positive_manifest), "M275-B003-DYN-16", "migration assist semantics flag missing", failures)
    checks_passed += require(packet.get("canonical_mode_rejection_code") == "O3S216", display_path(positive_manifest), "M275-B003-DYN-17", "diagnostic code drifted", failures)
    checks_passed += require(packet.get("canonical_mode_rejection_ready") is True, display_path(positive_manifest), "M275-B003-DYN-18", "canonical rejection readiness missing", failures)
    checks_passed += require(packet.get("ready_for_lowering_and_runtime") is True, display_path(positive_manifest), "M275-B003-DYN-19", "readiness flag missing", failures)

    checks_total += 2
    checks_passed += require(summary_payload.get("semantic_skipped") is False, display_path(positive_summary), "M275-B003-DYN-20", "semantic stage should run", failures)
    pos_diag_list = diagnostics_payload.get("diagnostics", []) if isinstance(diagnostics_payload, dict) else []
    checks_passed += require(isinstance(pos_diag_list, list) and len(pos_diag_list) == 0, display_path(positive_diags), "M275-B003-DYN-21", "positive fixture emitted diagnostics", failures)

    negative_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "b003" / "negative"
    negative_dir.mkdir(parents=True, exist_ok=True)
    negative_run = run_command([
        str(args.runner_exe),
        str(NEGATIVE_FIXTURE),
        "--out-dir",
        str(negative_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
        "--objc3-compat-mode",
        "canonical",
        "--objc3-migration-assist",
    ])
    negative_summary = negative_dir / "module.c_api_summary.json"
    negative_diags = negative_dir / "module.diagnostics.json"

    checks_total += 5
    checks_passed += require(negative_run.returncode != 0, display_path(NEGATIVE_FIXTURE), "M275-B003-DYN-22", "negative fixture should fail closed", failures)
    checks_passed += require(negative_summary.exists(), display_path(negative_summary), "M275-B003-DYN-23", "negative summary missing", failures)
    checks_passed += require(negative_diags.exists(), display_path(negative_diags), "M275-B003-DYN-24", "negative diagnostics missing", failures)
    negative_summary_payload = load_json(negative_summary) if negative_summary.exists() else {}
    checks_passed += require(negative_summary_payload.get("success") is False, display_path(negative_summary), "M275-B003-DYN-25", "negative run should be unsuccessful", failures)
    checks_passed += require(negative_summary_payload.get("stages", {}).get("sema", {}).get("diagnostics_errors") == 3, display_path(negative_summary), "M275-B003-DYN-26", "negative sema diagnostic count drifted", failures)

    negative_diag_payload = load_json(negative_diags) if negative_diags.exists() else {}
    negative_diag_list = negative_diag_payload.get("diagnostics", []) if isinstance(negative_diag_payload, dict) else []
    checks_total += 4
    checks_passed += require(isinstance(negative_diag_list, list) and len(negative_diag_list) == 3, display_path(negative_diags), "M275-B003-DYN-27", "negative diagnostics should include three entries", failures)
    checks_passed += require(all(isinstance(item, dict) and item.get("code") == "O3S216" for item in negative_diag_list), display_path(negative_diags), "M275-B003-DYN-28", "negative diagnostics must all be O3S216", failures)
    negative_messages = [str(item.get("message", "")) for item in negative_diag_list if isinstance(item, dict)]
    checks_passed += require(any("'true'" in message and "'YES'" in message for message in negative_messages), display_path(negative_diags), "M275-B003-DYN-29", "YES rewrite guidance missing", failures)
    checks_passed += require(any("'false'" in message and "'NO'" in message for message in negative_messages) and any("'nil'" in message and "'NULL'" in message for message in negative_messages), display_path(negative_diags), "M275-B003-DYN-30", "NO/NULL rewrite guidance missing", failures)

    return checks_total, checks_passed, {
        "positive_manifest": display_path(positive_manifest),
        "positive_summary": display_path(positive_summary),
        "negative_summary": display_path(negative_summary),
        "negative_diagnostics": display_path(negative_diags),
        "positive_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-B003-EXP-01", "# M275 Legacy-to-Canonical Migration Semantics - Edge-case and Compatibility Completion Expectations (B003)"),
            SnippetCheck("M275-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-B003-PKT-01", "# M275-B003 Packet: Legacy-to-canonical migration semantics - Edge-case and compatibility completion"),
            SnippetCheck("M275-B003-PKT-02", "frontend.pipeline.semantic_surface.objc_part12_legacy_canonical_migration_semantics"),
            SnippetCheck("M275-B003-PKT-03", "M275-C001"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M275-B003-GRM-01", "Current implementation status (`M275-B003`):"),
            SnippetCheck("M275-B003-GRM-02", "objc_part12_legacy_canonical_migration_semantics"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-B003-DOC-01", "Current implementation status (`M275-B003`):"),
            SnippetCheck("M275-B003-DOC-02", "objc_part12_legacy_canonical_migration_semantics"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-B003-CHK-01", "objc_part12_legacy_canonical_migration_semantics"),
        ],
        SPEC_ABSTRACT: [
            SnippetCheck("M275-B003-ABS-01", "M275-B003 legacy/canonical migration semantics note:"),
            SnippetCheck("M275-B003-ABS-02", "objc_part12_legacy_canonical_migration_semantics"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M275-B003-SEM-01", "kObjc3Part12LegacyCanonicalMigrationSemanticsContractId"),
            SnippetCheck("M275-B003-SEM-02", "kObjc3Part12LegacyCanonicalMigrationDiagnosticCode"),
        ],
        SEMA_PASSES: [
            SnippetCheck("M275-B003-SEMPASS-01", "M275-B003 semantic migration anchor:"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M275-B003-TYPE-01", "kObjc3Part12LegacyCanonicalMigrationSemanticsSurfacePath"),
            SnippetCheck("M275-B003-TYPE-02", "struct Objc3Part12LegacyCanonicalMigrationSemanticsSummary"),
        ],
        FRONTEND_ARTIFACTS_H: [
            SnippetCheck("M275-B003-ARTH-01", "part12_legacy_canonical_migration_semantics_summary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M275-B003-ART-01", "BuildPart12LegacyCanonicalMigrationSemanticsSummary"),
            SnippetCheck("M275-B003-ART-02", "objc_part12_legacy_canonical_migration_semantics"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-B003-PKG-01", '"check:objc3c:m275-b003-legacy-canonical-migration-semantics"'),
            SnippetCheck("M275-B003-PKG-02", '"check:objc3c:m275-b003-lane-b-readiness"'),
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
        "next_issue": "M275-C001",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M275-B003 legacy/canonical migration semantics checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
