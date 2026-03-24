#!/usr/bin/env python3
"""Checker for M273-B001 expansion/behavior semantic model freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-b001-part10-expansion-behavior-semantic-model-v1"
CONTRACT_ID = "objc3c-part10-expansion-behavior-semantic-model/m273-b001-v1"
SURFACE_KEY = "objc_part10_expansion_and_behavior_semantic_model"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-B001" / "expansion_behavior_semantic_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_expansion_and_behavior_semantic_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_b001_expansion_and_behavior_semantic_model_contract_and_architecture_freeze_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b001_expansion_behavior_semantic_model_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b001_expansion_behavior_semantic_model_negative_behavior_value.objc3"


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
        failures.append(Finding(display_path(path), "M273-B001-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m273-b001-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-B001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-B001-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-B001-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "b001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    positive = run_command([str(args.runner_exe), str(POSITIVE_FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    positive_output = (positive.stdout or "") + (positive.stderr or "")
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M273-B001-DYN-03", f"positive fixture failed: {positive_output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M273-B001-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "derive_marker_sites": 0,
        "macro_marker_sites": 1,
        "macro_package_sites": 1,
        "macro_provenance_sites": 1,
        "expansion_visible_macro_sites": 1,
        "property_behavior_sites": 2,
        "interface_property_behavior_sites": 1,
        "implementation_property_behavior_sites": 1,
        "protocol_property_behavior_sites": 0,
        "synthesized_binding_visible_sites": 2,
        "synthesized_getter_visible_sites": 2,
        "synthesized_setter_visible_sites": 2,
        "property_behavior_contract_violation_sites": 0,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M273-B001-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    bool_checks = [
        ("source_dependency_required", True, 18),
        ("derive_macro_source_supported", True, 19),
        ("macro_package_provenance_surface_reused", True, 20),
        ("property_behavior_source_supported", True, 21),
        ("synthesized_visibility_surface_reused", True, 22),
        ("derive_synthesis_deferred", True, 23),
        ("macro_execution_deferred", True, 24),
        ("property_behavior_runtime_deferred", True, 25),
        ("deterministic", True, 26),
        ("ready_for_core_implementation", True, 27),
    ]
    for field, expected_value, index in bool_checks:
        checks_total += 1
        checks_passed += require(packet.get(field) is expected_value, display_path(manifest_path), f"M273-B001-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    negative_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "b001" / "negative_behavior_value"
    negative_out_dir.mkdir(parents=True, exist_ok=True)
    negative = run_command([str(args.runner_exe), str(NEGATIVE_FIXTURE), "--out-dir", str(negative_out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    negative_output = (negative.stdout or "") + (negative.stderr or "")
    checks_total += 1
    checks_passed += require(negative.returncode == 1, display_path(NEGATIVE_FIXTURE), "M273-B001-DYN-28", f"negative fixture expected sema failure, got: {negative_output}", failures)
    checks_total += 1
    checks_passed += require("O3S206" in negative_output, display_path(NEGATIVE_FIXTURE), "M273-B001-DYN-29", "negative fixture missing O3S206 diagnostic", failures)
    checks_total += 1
    checks_passed += require("behavior attribute requires a non-empty value" in negative_output, display_path(NEGATIVE_FIXTURE), "M273-B001-DYN-30", "negative fixture missing behavior-value semantic diagnostic", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(manifest_path),
        "negative_fixture": display_path(NEGATIVE_FIXTURE),
        "negative_returncode": negative.returncode,
        "negative_output": negative_output.strip(),
        "part10_expansion_behavior_semantic_model_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-B001-EXP-01", "# M273 Expansion and Behavior Semantic Model Contract and Architecture Freeze Expectations (B001)"),
            SnippetCheck("M273-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-B001-PKT-01", "# M273-B001 Packet: Expansion and Behavior Semantic Model - Contract and Architecture Freeze"),
            SnippetCheck("M273-B001-PKT-02", "frontend.pipeline.semantic_surface.objc_part10_expansion_and_behavior_semantic_model"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M273-B001-GRM-01", "## M273 expansion and behavior semantic model"),
            SnippetCheck("M273-B001-GRM-02", "objc_part10_expansion_and_behavior_semantic_model"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-B001-DOC-01", "## M273 expansion and behavior semantic model"),
            SnippetCheck("M273-B001-DOC-02", "real derive expansion, macro execution/sandboxing, and property-behavior runtime hooks remain deferred"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-B001-ATTR-01", "## M273 expansion and behavior semantic model (B001)"),
            SnippetCheck("M273-B001-ATTR-02", "invalid `@property(..., behavior)` usage still rejected with `O3S206`"),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-B001-META-01", "## M273 expansion/behavior semantic-model note"),
            SnippetCheck("M273-B001-META-02", "objc3c-part10-expansion-behavior-semantic-model/m273-b001-v1"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M273-B001-CON-01", "kObjc3Part10ExpansionBehaviorSemanticModelContractId"),
            SnippetCheck("M273-B001-CON-02", "struct Objc3Part10ExpansionBehaviorSemanticModelSummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M273-B001-HDR-01", "BuildPart10ExpansionBehaviorSemanticModelSummary"),
        ],
        SEMA_CPP: [
            SnippetCheck("M273-B001-SEMA-01", "BuildPart10ExpansionBehaviorSemanticModelSummary"),
            SnippetCheck("M273-B001-SEMA-02", "property_behavior_contract_violation_sites"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M273-B001-TYPE-01", "part10_expansion_behavior_semantic_model_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M273-B001-PIPE-01", "BuildPart10ExpansionBehaviorSemanticModelSummary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M273-B001-ART-01", "BuildPart10ExpansionBehaviorSemanticModelSummaryJson"),
            SnippetCheck("M273-B001-ART-02", "objc_part10_expansion_and_behavior_semantic_model"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-B001-PKG-01", '"check:objc3c:m273-b001-expansion-and-behavior-semantic-model-contract-and-architecture-freeze"'),
            SnippetCheck("M273-B001-PKG-02", '"check:objc3c:m273-b001-lane-b-readiness"'),
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
    print(f"[ok] M273-B001 semantic-model checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
