#!/usr/bin/env python3
"""Checker for M273-B003 macro safety/sandbox/determinism semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-b003-part10-macro-safety-sandbox-determinism-v1"
CONTRACT_ID = "objc3c-part10-macro-safety-sandbox-determinism-semantics/m273-b003-v1"
SURFACE_KEY = "objc_part10_macro_safety_sandbox_and_determinism_semantics"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-B003" / "macro_safety_sandbox_determinism_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_macro_safety_sandbox_and_determinism_semantics_core_feature_implementation_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_b003_macro_safety_sandbox_and_determinism_semantics_core_feature_implementation_packet.md"
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
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b003_macro_safety_sandbox_positive.objc3"
NEGATIVE_INVALID_PACKAGE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b003_macro_safety_sandbox_negative_invalid_package.objc3"
NEGATIVE_NONPURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_b003_macro_safety_sandbox_negative_nonpure.objc3"


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
        failures.append(Finding(display_path(path), "M273-B003-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m273-b003-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-B003/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-B003-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-B003-DYN-02", "frontend runner missing after build", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "b003" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    positive = run_command([str(args.runner_exe), str(POSITIVE_FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    positive_output = (positive.stdout or "") + (positive.stderr or "")
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M273-B003-DYN-03", f"positive fixture failed: {positive_output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M273-B003-DYN-04", "positive manifest missing", failures)

    packet: dict[str, object] = {}
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        packet = manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    expected = {
        "macro_marker_sites": 1,
        "macro_package_sites": 1,
        "macro_provenance_sites": 1,
        "expansion_visible_macro_sites": 1,
        "safe_macro_callable_sites": 1,
        "incomplete_macro_metadata_sites": 0,
        "orphan_macro_metadata_sites": 0,
        "invalid_package_sites": 0,
        "invalid_provenance_sites": 0,
        "nondeterministic_callable_sites": 0,
        "unsupported_callable_topology_sites": 0,
    }
    for index, (field, expected_value) in enumerate(expected.items(), start=5):
        checks_total += 1
        checks_passed += require(packet.get(field) == expected_value, display_path(manifest_path), f"M273-B003-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    bool_checks = [
        ("semantic_dependency_required", True, 16),
        ("metadata_completeness_enforced", True, 17),
        ("sandbox_namespace_enforced", True, 18),
        ("provenance_determinism_enforced", True, 19),
        ("callable_determinism_enforced", True, 20),
        ("macro_execution_deferred", True, 21),
        ("deterministic", True, 22),
        ("ready_for_lowering_and_runtime", True, 23),
    ]
    for field, expected_value, index in bool_checks:
        checks_total += 1
        checks_passed += require(packet.get(field) is expected_value, display_path(manifest_path), f"M273-B003-DYN-{index:02d}", f"packet field {field} mismatch", failures)

    invalid_package_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "b003" / "negative_invalid_package"
    invalid_package_out.mkdir(parents=True, exist_ok=True)
    invalid_package = run_command([str(args.runner_exe), str(NEGATIVE_INVALID_PACKAGE), "--out-dir", str(invalid_package_out), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    invalid_package_output = (invalid_package.stdout or "") + (invalid_package.stderr or "")
    checks_total += 1
    checks_passed += require(invalid_package.returncode == 1, display_path(NEGATIVE_INVALID_PACKAGE), "M273-B003-DYN-24", f"invalid-package fixture expected sema failure, got: {invalid_package_output}", failures)
    checks_total += 1
    checks_passed += require("O3S322" in invalid_package_output, display_path(NEGATIVE_INVALID_PACKAGE), "M273-B003-DYN-25", "invalid-package fixture missing O3S322 diagnostic", failures)

    nonpure_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "b003" / "negative_nonpure"
    nonpure_out.mkdir(parents=True, exist_ok=True)
    nonpure = run_command([str(args.runner_exe), str(NEGATIVE_NONPURE), "--out-dir", str(nonpure_out), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"])
    nonpure_output = (nonpure.stdout or "") + (nonpure.stderr or "")
    checks_total += 1
    checks_passed += require(nonpure.returncode == 1, display_path(NEGATIVE_NONPURE), "M273-B003-DYN-26", f"nonpure fixture expected sema failure, got: {nonpure_output}", failures)
    checks_total += 1
    checks_passed += require("O3S324" in nonpure_output, display_path(NEGATIVE_NONPURE), "M273-B003-DYN-27", "nonpure fixture missing O3S324 diagnostic", failures)

    return checks_total, checks_passed, {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(manifest_path),
        "negative_invalid_package_fixture": display_path(NEGATIVE_INVALID_PACKAGE),
        "negative_invalid_package_returncode": invalid_package.returncode,
        "negative_invalid_package_output": invalid_package_output.strip(),
        "negative_nonpure_fixture": display_path(NEGATIVE_NONPURE),
        "negative_nonpure_returncode": nonpure.returncode,
        "negative_nonpure_output": nonpure_output.strip(),
        "part10_macro_safety_sandbox_determinism_packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-B003-EXP-01", "# M273 Macro Safety, Sandbox, and Determinism Semantics Expectations (B003)"),
            SnippetCheck("M273-B003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-B003-PKT-01", "# M273-B003 Packet: Macro Safety, Sandbox, and Determinism Semantics - Core feature implementation"),
            SnippetCheck("M273-B003-PKT-02", "frontend.pipeline.semantic_surface.objc_part10_macro_safety_sandbox_and_determinism_semantics"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M273-B003-GRM-01", "## M273 macro safety, sandbox, and determinism semantics"),
            SnippetCheck("M273-B003-GRM-02", "objc_part10_macro_safety_sandbox_and_determinism_semantics"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-B003-DOC-01", "## M273 macro safety, sandbox, and determinism semantics"),
            SnippetCheck("M273-B003-DOC-02", "O3S320"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-B003-ATTR-01", "## M273 macro safety, sandbox, and determinism semantics (B003)"),
            SnippetCheck("M273-B003-ATTR-02", "macro-marked methods rejected with `O3S325`"),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-B003-META-01", "## M273 macro safety/sandbox semantics note"),
            SnippetCheck("M273-B003-META-02", CONTRACT_ID),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M273-B003-CON-01", "kObjc3Part10MacroSafetySandboxDeterminismContractId"),
            SnippetCheck("M273-B003-CON-02", "struct Objc3Part10MacroSafetySandboxDeterminismSummary"),
        ],
        SEMA_HEADER: [
            SnippetCheck("M273-B003-HDR-01", "BuildPart10MacroSafetySandboxDeterminismSummary"),
        ],
        SEMA_CPP: [
            SnippetCheck("M273-B003-SEMA-01", "BuildPart10MacroSafetySandboxDeterminismSummary"),
            SnippetCheck("M273-B003-SEMA-02", '"O3S320"'),
            SnippetCheck("M273-B003-SEMA-03", '"O3S321"'),
            SnippetCheck("M273-B003-SEMA-04", '"O3S322"'),
            SnippetCheck("M273-B003-SEMA-05", '"O3S323"'),
            SnippetCheck("M273-B003-SEMA-06", '"O3S324"'),
            SnippetCheck("M273-B003-SEMA-07", '"O3S325"'),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M273-B003-TYPE-01", "part10_macro_safety_sandbox_determinism_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M273-B003-PIPE-01", "BuildPart10MacroSafetySandboxDeterminismSummary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M273-B003-ART-01", "BuildPart10MacroSafetySandboxDeterminismSummaryJson"),
            SnippetCheck("M273-B003-ART-02", "objc_part10_macro_safety_sandbox_and_determinism_semantics"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-B003-PKG-01", '"check:objc3c:m273-b003-macro-safety-sandbox-and-determinism-semantics-core-feature-implementation"'),
            SnippetCheck("M273-B003-PKG-02", '"check:objc3c:m273-b003-lane-b-readiness"'),
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
    print(f"[ok] M273-B003 macro-safety checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
