#!/usr/bin/env python3
"""Checker for M273-A002 frontend macro package/provenance completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-a002-part10-macro-package-provenance-source-completion-v1"
CONTRACT_ID = "objc3c-part10-macro-package-provenance-source-completion/m273-a002-v1"
SURFACE_KEY = "objc_part10_macro_package_and_provenance_source_completion"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-A002" / "macro_package_provenance_source_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_frontend_macro_package_and_provenance_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_a002_frontend_macro_package_and_provenance_surface_completion_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_a002_macro_package_provenance_positive.objc3"


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
        failures.append(Finding(display_path(path), "M273-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m273-a002-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-A002/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-A002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-A002-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "a002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([str(args.runner_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M273-A002-DYN-03", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M273-A002-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "macro_marker_sites": 1,
        "macro_package_sites": 1,
        "macro_provenance_sites": 1,
        "expansion_visible_macro_sites": 1,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M273-A002-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    checks_total += 1
    checks_passed += require(packet.get("macro_package_source_supported") is True, display_path(manifest_path), "M273-A002-DYN-09", "macro package support must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("macro_provenance_source_supported") is True, display_path(manifest_path), "M273-A002-DYN-10", "macro provenance support must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("expansion_visible_source_supported") is True, display_path(manifest_path), "M273-A002-DYN-11", "expansion-visible support must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("deterministic_handoff") is True, display_path(manifest_path), "M273-A002-DYN-12", "deterministic_handoff must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("ready_for_semantic_expansion") is True, display_path(manifest_path), "M273-A002-DYN-13", "ready_for_semantic_expansion must be true", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part10_macro_package_provenance_source_completion_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-A002-EXP-01", "# M273 Frontend Macro Package and Provenance Surface Completion Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M273-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-A002-PKT-01", "# M273-A002 Packet: Frontend Macro Package and Provenance Surface Completion - Core Feature Implementation"),
            SnippetCheck("M273-A002-PKT-02", "frontend.pipeline.semantic_surface.objc_part10_macro_package_and_provenance_source_completion"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M273-A002-GRM-01", "## M273 macro package and provenance source completion"),
            SnippetCheck("M273-A002-GRM-02", "objc_part10_macro_package_and_provenance_source_completion"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-A002-DOC-01", "## M273 macro package and provenance source completion"),
            SnippetCheck("M273-A002-DOC-02", "expansion-visible macro source sites"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-A002-ATTR-01", "## M273 macro package and provenance source completion (A002)"),
            SnippetCheck("M273-A002-ATTR-02", "objc_macro_package(named(\"...\"))"),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-A002-META-01", "## M273 macro package/provenance note"),
            SnippetCheck("M273-A002-META-02", "objc3c-part10-macro-package-provenance-source-completion/m273-a002-v1"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M273-A002-TOK-01", "kObjc3Part10MacroPackageProvenanceSourceCompletionContractId"),
            SnippetCheck("M273-A002-TOK-02", "kObjc3SourceOnlyFeatureClaimMacroPackageMarkers"),
        ],
        LEXER_CPP: [
            SnippetCheck("M273-A002-LEX-01", "M273-A002 source-completion note:"),
        ],
        AST_HEADER: [
            SnippetCheck("M273-A002-AST-01", "bool objc_macro_package_declared = false;"),
            SnippetCheck("M273-A002-AST-02", "bool objc_macro_provenance_declared = false;"),
        ],
        PARSER_CPP: [
            SnippetCheck("M273-A002-PARSE-01", 'attribute_name.text == "objc_macro_package"'),
            SnippetCheck("M273-A002-PARSE-02", 'attribute_name.text == "objc_macro_provenance"'),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M273-A002-TYPE-01", "kObjc3Part10MacroPackageProvenanceSourceCompletionSurfacePath"),
            SnippetCheck("M273-A002-TYPE-02", "struct Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M273-A002-PIPE-01", "BuildPart10MacroPackageProvenanceSourceCompletionSummary"),
            SnippetCheck("M273-A002-PIPE-02", "expansion_visible_macro_sites"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M273-A002-ART-01", "BuildPart10MacroPackageProvenanceSourceCompletionSummaryJson"),
            SnippetCheck("M273-A002-ART-02", "objc_part10_macro_package_and_provenance_source_completion"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-A002-PKG-01", '"check:objc3c:m273-a002-frontend-macro-package-and-provenance-surface-completion-core-feature-implementation"'),
            SnippetCheck("M273-A002-PKG-02", '"check:objc3c:m273-a002-lane-a-readiness"'),
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
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M273-A002 source-completion checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
