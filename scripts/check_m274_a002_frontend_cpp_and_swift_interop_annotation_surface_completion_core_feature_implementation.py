#!/usr/bin/env python3
"""Checker for M274-A002 frontend Cpp/Swift interop annotation source completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-a002-part11-cpp-swift-interop-annotation-source-completion-v1"
CONTRACT_ID = "objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1"
SURFACE_KEY = "objc_part11_cpp_and_swift_interop_annotation_source_completion"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-A002" / "cpp_swift_interop_annotation_source_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation_packet.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_a002_cpp_swift_annotation_positive.objc3"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation.py"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M274-A002-EXP-01", "# M274 Frontend Cpp And Swift Interop Annotation Surface Completion Core Feature Implementation Expectations (A002)"),
        SnippetCheck("M274-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-A002-EXP-03", "objc_swift_name(named(\"...\"))"),
        SnippetCheck("M274-A002-EXP-04", "objc_header_name(named(\"...\"))"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-A002-PKT-01", "# M274-A002 Packet: Frontend Cpp And Swift Interop Annotation Surface Completion - Core Feature Implementation"),
        SnippetCheck("M274-A002-PKT-02", "frontend.pipeline.semantic_surface.objc_part11_cpp_and_swift_interop_annotation_source_completion"),
        SnippetCheck("M274-A002-PKT-03", "`M274-B001`"),
    ),
    TOKEN_HEADER: (
        SnippetCheck("M274-A002-TOK-01", "kObjc3Part11CppSwiftInteropAnnotationSourceCompletionContractId"),
        SnippetCheck("M274-A002-TOK-02", "kObjc3SourceOnlyFeatureClaimSwiftFacingAnnotationMarkers"),
        SnippetCheck("M274-A002-TOK-03", "kObjc3SourceOnlyFeatureClaimCppFacingAnnotationMarkers"),
        SnippetCheck("M274-A002-TOK-04", "kObjc3SourceOnlyFeatureClaimInteropMetadataAnnotationMarkers"),
        SnippetCheck("M274-A002-TOK-05", "kObjc3SourceOnlyFeatureClaimInteropNamedMetadataPayloads"),
    ),
    LEXER_CPP: (
        SnippetCheck("M274-A002-LEX-01", "M274-A002 source-completion note:"),
        SnippetCheck("M274-A002-LEX-02", "`objc_swift_private`, `objc_cxx_name`, and `objc_header_name`"),
    ),
    AST_HEADER: (
        SnippetCheck("M274-A002-AST-01", "bool objc_swift_name_declared = false;"),
        SnippetCheck("M274-A002-AST-02", "bool objc_swift_private_declared = false;"),
        SnippetCheck("M274-A002-AST-03", "bool objc_cxx_name_declared = false;"),
        SnippetCheck("M274-A002-AST-04", "bool objc_header_name_declared = false;"),
    ),
    PARSER_CPP: (
        SnippetCheck("M274-A002-PARSE-01", 'attribute_name.text == "objc_swift_name"'),
        SnippetCheck("M274-A002-PARSE-02", 'attribute_name.text == "objc_swift_private"'),
        SnippetCheck("M274-A002-PARSE-03", 'attribute_name.text == "objc_cxx_name"'),
        SnippetCheck("M274-A002-PARSE-04", 'attribute_name.text == "objc_header_name"'),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M274-A002-TYPE-01", "struct Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary"),
        SnippetCheck("M274-A002-TYPE-02", "std::size_t swift_name_annotation_sites = 0;"),
        SnippetCheck("M274-A002-TYPE-03", "std::size_t named_annotation_payload_sites = 0;"),
    ),
    FRONTEND_PIPELINE: (
        SnippetCheck("M274-A002-PIPE-01", "BuildPart11CppSwiftInteropAnnotationSourceCompletionSummary"),
        SnippetCheck("M274-A002-PIPE-02", "summary.swift_annotation_source_supported = true;"),
        SnippetCheck("M274-A002-PIPE-03", "summary.deterministic_handoff ="),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M274-A002-ART-01", "BuildPart11CppSwiftInteropAnnotationSourceCompletionSummaryJson"),
        SnippetCheck("M274-A002-ART-02", "\\\"swift_name_annotation_sites\\\""),
        SnippetCheck("M274-A002-ART-03", "\\\"named_annotation_payload_sites\\\""),
    ),
    FIXTURE: (
        SnippetCheck("M274-A002-FIX-01", "objc_swift_name(named(\"SwiftBridge\"))"),
        SnippetCheck("M274-A002-FIX-02", "objc_swift_private"),
        SnippetCheck("M274-A002-FIX-03", "objc_cxx_name(named(\"CppBridge\"))"),
        SnippetCheck("M274-A002-FIX-04", "objc_header_name(named(\"HeaderBridge\"))"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-A002-RUN-01", "check_m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation.py"),
        SnippetCheck("M274-A002-RUN-02", "test_check_m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M274-A002-TEST-01", "def test_m274_a002_checker_emits_summary() -> None:"),
        SnippetCheck("M274-A002-TEST-02", CONTRACT_ID),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = path.read_text(encoding="utf-8")
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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m274-a002-readiness",
        "--summary-out",
        "tmp/reports/m274/M274-A002/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M274-A002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M274-A002-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "a002" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M274-A002-DYN-03", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M274-A002-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "swift_name_annotation_sites": 1,
        "swift_private_annotation_sites": 1,
        "cpp_name_annotation_sites": 1,
        "header_name_annotation_sites": 1,
        "interop_metadata_annotation_sites": 4,
        "named_annotation_payload_sites": 3,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M274-A002-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    checks_total += 1
    checks_passed += require(packet.get("swift_annotation_source_supported") is True, display_path(manifest_path), "M274-A002-DYN-11", "swift annotation support must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("cpp_annotation_source_supported") is True, display_path(manifest_path), "M274-A002-DYN-12", "cpp annotation support must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("interop_metadata_source_supported") is True, display_path(manifest_path), "M274-A002-DYN-13", "interop metadata support must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("deterministic_handoff") is True, display_path(manifest_path), "M274-A002-DYN-14", "deterministic_handoff must be true", failures)
    checks_total += 1
    checks_passed += require(packet.get("ready_for_semantic_expansion") is True, display_path(manifest_path), "M274-A002-DYN-15", "ready_for_semantic_expansion must be true", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part11_cpp_and_swift_interop_annotation_source_completion_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

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
    print(f"[ok] M274-A002 source-completion checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
