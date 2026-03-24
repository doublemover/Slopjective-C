#!/usr/bin/env python3
"""Checker for M273-B004 property-behavior legality completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-b004-part10-property-behavior-legality-completion-v1"
CONTRACT_ID = "objc3c-part10-property-behavior-legality-interaction-completion/m273-b004-v1"
SURFACE_KEY = "objc_part10_property_behavior_legality_and_interaction_completion"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-B004" / "property_behavior_legality_completion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_property_behavior_legality_and_interaction_completion_edge_case_and_compatibility_completion_b004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_b004_property_behavior_legality_and_interaction_completion_edge_case_and_compatibility_completion_packet.md"
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
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b004_property_behavior_legality_positive.objc3"
NEGATIVE_UNSUPPORTED = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b004_property_behavior_legality_negative_unsupported.objc3"
NEGATIVE_PROTOCOL_OBSERVED = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b004_property_behavior_legality_negative_protocol_observed.objc3"
NEGATIVE_PROJECTED_WRITABLE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b004_property_behavior_legality_negative_projected_writable.objc3"
NEGATIVE_NONOBJECT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b004_property_behavior_legality_negative_nonobject.objc3"


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
        failures.append(Finding(display_path(path), "M273-B004-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_fixture(args: argparse.Namespace, fixture: Path, out_leaf: str) -> tuple[subprocess.CompletedProcess[str], str, Path]:
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "b004" / out_leaf
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([str(args.runner_exe), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    output = (completed.stdout or "") + (completed.stderr or "")
    return completed, output, out_dir / "module.manifest.json"


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m273-b004-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-B004/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-B004-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-B004-DYN-02", "frontend runner missing after build", failures)

    positive, positive_output, manifest_path = run_fixture(args, POSITIVE_FIXTURE, "positive")
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M273-B004-DYN-03", f"positive fixture failed: {positive_output}", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M273-B004-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "property_behavior_sites": 5,
        "supported_behavior_sites": 5,
        "unsupported_behavior_sites": 0,
        "observed_behavior_sites": 2,
        "projected_behavior_sites": 3,
        "observed_on_protocol_sites": 0,
        "observed_readonly_conflict_sites": 0,
        "projected_writable_conflict_sites": 0,
        "non_object_behavior_sites": 0,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M273-B004-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    bool_checks = [
        ("semantic_dependency_required", True, 14),
        ("supported_behavior_inventory_landed", True, 15),
        ("unsupported_behavior_fail_closed", True, 16),
        ("owner_topology_fail_closed", True, 17),
        ("interaction_legality_fail_closed", True, 18),
        ("storage_legality_fail_closed", True, 19),
        ("runtime_materialization_deferred", True, 20),
        ("deterministic", True, 21),
        ("ready_for_lowering_and_runtime", True, 22),
    ]
    for field, expected_value, index in bool_checks:
        checks_total += 1
        checks_passed += require(packet.get(field) is expected_value, display_path(manifest_path), f"M273-B004-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    negative_cases = [
        (NEGATIVE_UNSUPPORTED, "O3S326", "unsupported property behavior", 23),
        (NEGATIVE_PROTOCOL_OBSERVED, "O3S328", "observed property behavior requires a concrete interface or implementation property", 26),
        (NEGATIVE_PROJECTED_WRITABLE, "O3S330", "projected property behavior requires a readonly getter-only property", 29),
        (NEGATIVE_NONOBJECT, "O3S327", "requires an Objective-C object property", 32),
    ]

    negative_summaries: list[dict[str, object]] = []
    for fixture, code, needle, start_index in negative_cases:
        completed, output, _manifest = run_fixture(args, fixture, fixture.stem)
        checks_total += 1
        checks_passed += require(completed.returncode == 1, display_path(fixture), f"M273-B004-DYN-{start_index:02d}", f"negative fixture expected sema failure, got: {output}", failures)
        checks_total += 1
        checks_passed += require(code in output, display_path(fixture), f"M273-B004-DYN-{start_index + 1:02d}", f"negative fixture missing {code}", failures)
        checks_total += 1
        checks_passed += require(needle in output, display_path(fixture), f"M273-B004-DYN-{start_index + 2:02d}", f"negative fixture missing semantic diagnostic", failures)
        negative_summaries.append({
            "fixture": display_path(fixture),
            "returncode": completed.returncode,
            "output": output.strip(),
        })

    return checks_total, checks_passed, {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(manifest_path),
        "part10_property_behavior_legality_packet": packet,
        "negative_cases": negative_summaries,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-B004-EXP-01", "# M273 Property-Behavior Legality and Interaction Completion Edge-case and Compatibility Completion Expectations (B004)"),
            SnippetCheck("M273-B004-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-B004-PKT-01", "# M273-B004 Packet: Property-Behavior Legality and Interaction Completion - Edge-case and Compatibility Completion"),
            SnippetCheck("M273-B004-PKT-02", SURFACE_KEY),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M273-B004-GRM-01", "## M273 property behavior legality and interaction completion"),
            SnippetCheck("M273-B004-GRM-02", SURFACE_KEY),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-B004-DOC-01", "## M273 property behavior legality and interaction completion"),
            SnippetCheck("M273-B004-DOC-02", "Observed` and `Projected"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-B004-ATTR-01", "## M273 property behavior legality and interaction completion (B004)"),
            SnippetCheck("M273-B004-ATTR-02", "unsupported property behavior names are rejected with `O3S326`"),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-B004-META-01", "## M273 property-behavior legality/interactions note"),
            SnippetCheck("M273-B004-META-02", CONTRACT_ID),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M273-B004-CON-01", "kObjc3Part10PropertyBehaviorLegalityCompatibilityContractId"),
            SnippetCheck("M273-B004-CON-02", "struct Objc3Part10PropertyBehaviorLegalityCompatibilitySummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M273-B004-HDR-01", "BuildPart10PropertyBehaviorLegalityCompatibilitySummary"),
        ],
        SEMA_CPP: [
            SnippetCheck("M273-B004-SEMA-01", "NormalizePart10SupportedPropertyBehaviorName"),
            SnippetCheck("M273-B004-SEMA-02", "O3S330"),
            SnippetCheck("M273-B004-SEMA-03", "BuildPart10PropertyBehaviorLegalityCompatibilitySummary"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M273-B004-TYPE-01", "part10_property_behavior_legality_compatibility_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M273-B004-PIPE-01", "BuildPart10PropertyBehaviorLegalityCompatibilitySummary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M273-B004-ART-01", "BuildPart10PropertyBehaviorLegalityCompatibilitySummaryJson"),
            SnippetCheck("M273-B004-ART-02", SURFACE_KEY),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-B004-PKG-01", '"check:objc3c:m273-b004-property-behavior-legality-and-interaction-completion-edge-case-and-compatibility-completion"'),
            SnippetCheck("M273-B004-PKG-02", '"check:objc3c:m273-b004-lane-b-readiness"'),
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
        "surface_key": SURFACE_KEY,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_executed": dynamic_executed,
        "dynamic_summary": dynamic_summary,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[fail] {failure.check_id} {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1

    print(f"[ok] M273-B004 property-behavior legality checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
