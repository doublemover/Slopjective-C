#!/usr/bin/env python3
"""Checker for M274-B001 Part 11 interop semantic model freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-b001-part11-interop-semantic-model-v1"
CONTRACT_ID = "objc3c-part11-interop-semantic-model/m274-b001-v1"
SURFACE_KEY = "objc_part11_interop_semantic_model"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-B001" / "part11_interop_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_part11_interop_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_b001_part11_interop_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b001_part11_interop_semantic_model_positive.objc3"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_b001_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m274_b001_part11_interop_semantic_model_contract_and_architecture_freeze.py"


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
        SnippetCheck("M274-B001-EXP-01", "# M274 Part 11 Interop Semantic Model Contract And Architecture Freeze Expectations (B001)"),
        SnippetCheck("M274-B001-EXP-02", "Issue: `#7363`"),
        SnippetCheck("M274-B001-EXP-03", f"Contract ID: `{CONTRACT_ID}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-B001-PKT-01", "# M274-B001 Packet: Part 11 Interop Semantic Model - Contract And Architecture Freeze"),
        SnippetCheck("M274-B001-PKT-02", f"semantic surface `frontend.pipeline.semantic_surface.{SURFACE_KEY}`"),
        SnippetCheck("M274-B001-PKT-03", "Dependencies: `M274-A003`, `M267-E002`, `M268-E002`, `M270-E002`"),
        SnippetCheck("M274-B001-PKT-04", "Next issue: `M274-B002`"),
    ),
    DOC_GRAMMAR: (
        SnippetCheck("M274-B001-DOCSRC-01", "## M274 Part 11 interop semantic model"),
        SnippetCheck("M274-B001-DOCSRC-02", SURFACE_KEY),
    ),
    DOC_NATIVE: (
        SnippetCheck("M274-B001-DOC-01", "## M274 Part 11 interop semantic model"),
        SnippetCheck("M274-B001-DOC-02", SURFACE_KEY),
    ),
    SPEC_ATTR: (
        SnippetCheck("M274-B001-ATTR-01", "## M274 Part 11 interop semantic model (B001)"),
        SnippetCheck("M274-B001-ATTR-02", SURFACE_KEY),
    ),
    SPEC_METADATA: (
        SnippetCheck("M274-B001-META-01", "## M274 interop semantic-model note"),
        SnippetCheck("M274-B001-META-02", CONTRACT_ID),
    ),
    SEMA_CONTRACT: (
        SnippetCheck("M274-B001-SC-01", "kObjc3Part11InteropSemanticModelContractId"),
        SnippetCheck("M274-B001-SC-02", "Objc3Part11InteropSemanticModelSummary"),
        SnippetCheck("M274-B001-SC-03", SURFACE_KEY),
    ),
    SEMA_PASSES: (
        SnippetCheck("M274-B001-SP-01", "BuildPart11InteropSemanticModelSummary("),
        SnippetCheck("M274-B001-SP-02", "M274-B001 semantic freeze anchor"),
        SnippetCheck("M274-B001-SP-03", "summary.interop_metadata_annotation_sites ="),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M274-B001-TYP-01", "Objc3Part11InteropSemanticModelSummary"),
        SnippetCheck("M274-B001-TYP-02", "part11_interop_semantic_model_summary"),
    ),
    FRONTEND_PIPELINE: (
        SnippetCheck("M274-B001-PIPE-01", "result.part11_interop_semantic_model_summary ="),
        SnippetCheck("M274-B001-PIPE-02", "BuildPart11InteropSemanticModelSummary("),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M274-B001-ART-01", "BuildPart11InteropSemanticModelSummaryJson("),
        SnippetCheck("M274-B001-ART-02", "objc_part11_interop_semantic_model"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M274-B001-ARCH-01", "## M274 Part 11 interop semantic model (B001)"),
        SnippetCheck("M274-B001-ARCH-02", SURFACE_KEY),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-B001-PKG-01", '"check:objc3c:m274-b001-part11-interop-semantic-model-contract-and-architecture-freeze"'),
        SnippetCheck("M274-B001-PKG-02", '"test:tooling:m274-b001-part11-interop-semantic-model-contract-and-architecture-freeze"'),
        SnippetCheck("M274-B001-PKG-03", '"check:objc3c:m274-b001-lane-b-readiness"'),
    ),
    FIXTURE: (
        SnippetCheck("M274-B001-FIX-01", "module Part11InteropSemanticModelProvider;"),
        SnippetCheck("M274-B001-FIX-02", "objc_import_module(named(\"SampleKit\"))"),
        SnippetCheck("M274-B001-FIX-03", "objc_swift_name(named(\"SwiftBridge\"))"),
        SnippetCheck("M274-B001-FIX-04", "objc_header_name(named(\"HeaderBridge\"))"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-B001-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M274-B001-RUN-02", "check_m274_b001_part11_interop_semantic_model_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M274-B001-TEST-01", "def test_m274_b001_checker_emits_summary() -> None:"),
        SnippetCheck("M274-B001-TEST-02", CONTRACT_ID),
    ),
}


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


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m274-b001-readiness",
        "--summary-out",
        "tmp/reports/m274/M274-B001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M274-B001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M274-B001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "b001" / "positive"
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
    checks_passed += require(run.returncode == 0, display_path(FIXTURE), "M274-B001-DYN-03", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M274-B001-DYN-04", "positive manifest missing", failures)

    packet: dict[str, Any] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "foreign_callable_sites": 2,
        "import_module_annotation_sites": 1,
        "imported_module_name_sites": 1,
        "swift_name_annotation_sites": 1,
        "swift_private_annotation_sites": 1,
        "cpp_name_annotation_sites": 1,
        "header_name_annotation_sites": 1,
        "named_annotation_payload_sites": 3,
        "retainable_family_callable_sites": 0,
        "bridge_callable_sites": 0,
        "async_executor_affinity_sites": 0,
        "actor_hazard_sites": 0,
        "interop_metadata_annotation_sites": 7,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M274-B001-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    true_fields = [
        "source_dependency_required",
        "foreign_annotation_source_supported",
        "ownership_interaction_profile_frozen",
        "error_bridge_profile_reused",
        "async_affinity_profile_reused",
        "actor_hazard_profile_reused",
        "metadata_payload_profile_frozen",
        "ffi_abi_lowering_deferred",
        "runtime_bridge_generation_deferred",
        "deterministic",
        "ready_for_semantic_expansion",
    ]
    for index, field in enumerate(true_fields, start=18):
        checks_total += 1
        checks_passed += require(packet.get(field) is True, display_path(manifest_path), f"M274-B001-DYN-{index:02d}", f"{field} must be true", failures)

    checks_total += 2
    checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M274-B001-DYN-29", "contract_id mismatch", failures)
    checks_passed += require(packet.get("surface_path") == f"frontend.pipeline.semantic_surface.{SURFACE_KEY}", display_path(manifest_path), "M274-B001-DYN-30", "surface_path mismatch", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        SURFACE_KEY: packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_summary: dict[str, Any] = {"skipped": True}
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
    print(f"[ok] M274-B001 interop semantic-model checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
