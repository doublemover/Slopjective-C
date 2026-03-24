#!/usr/bin/env python3
"""Checker for M274-B004 Part 11 Swift metadata and isolation mapping completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-b004-part11-swift-metadata-and-isolation-mapping-v1"
CONTRACT_ID = "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part11_swift_metadata_and_isolation_mapping"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-B004" / "part11_swift_metadata_and_isolation_mapping_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion_b004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_b004_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_GENERATED = ROOT / "docs" / "objc3c-native.md"
SPEC_CATALOG = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b004_part11_swift_metadata_and_isolation_mapping_completion_positive.objc3"
NEGATIVE_FIXTURE_337 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b004_part11_swift_metadata_and_isolation_mapping_completion_negative_o3s337.objc3"
NEGATIVE_FIXTURE_338 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b004_part11_swift_metadata_and_isolation_mapping_completion_negative_o3s338.objc3"
NEGATIVE_FIXTURE_339 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b004_part11_swift_metadata_and_isolation_mapping_completion_negative_o3s339.objc3"
NEGATIVE_FIXTURE_340 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b004_part11_swift_metadata_and_isolation_mapping_completion_negative_o3s340.objc3"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
        failures.append(Finding(display_path(path), "STATIC-MISSING", f"missing artifact: {display_path(path)}"))
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
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M274-B004-EXP-01", "# M274 Part 11 Swift Metadata And Isolation Mapping Completion Expectations (B004)"),
        SnippetCheck("M274-B004-EXP-02", "Issue: `#7366`"),
        SnippetCheck("M274-B004-EXP-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-B004-EXP-04", "O3S337"),
        SnippetCheck("M274-B004-EXP-05", "O3S338"),
        SnippetCheck("M274-B004-EXP-06", "O3S339"),
        SnippetCheck("M274-B004-EXP-07", "O3S340"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-B004-PKT-01", "# M274-B004 Packet: Part 11 Swift Metadata And Isolation Mapping Completion - Edge-case And Compatibility Completion"),
        SnippetCheck("M274-B004-PKT-02", f"semantic surface `{SURFACE_PATH}`"),
        SnippetCheck("M274-B004-PKT-03", "Dependencies: `M274-B003`, `M274-A002`, `M270-E002`"),
        SnippetCheck("M274-B004-PKT-04", "O3S337"),
        SnippetCheck("M274-B004-PKT-05", "O3S338"),
        SnippetCheck("M274-B004-PKT-06", "O3S339"),
        SnippetCheck("M274-B004-PKT-07", "O3S340"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M274-B004-DOCSRC-01", "## M274 Part 11 Swift-facing metadata and isolation mapping completion"),
        SnippetCheck("M274-B004-DOCSRC-02", "objc_part11_swift_metadata_and_isolation_mapping"),
        SnippetCheck("M274-B004-DOCSRC-03", "O3S337"),
        SnippetCheck("M274-B004-DOCSRC-04", "O3S338"),
        SnippetCheck("M274-B004-DOCSRC-05", "O3S339"),
        SnippetCheck("M274-B004-DOCSRC-06", "O3S340"),
    ),
    DOC_GENERATED: (
        SnippetCheck("M274-B004-DOCGEN-01", "## M274 Part 11 Swift-facing metadata and isolation mapping completion"),
        SnippetCheck("M274-B004-DOCGEN-02", "objc_part11_swift_metadata_and_isolation_mapping"),
    ),
    SPEC_CATALOG: (
        SnippetCheck("M274-B004-SPEC-01", "## M274 Part 11 Swift-facing metadata and isolation mapping completion (B004)"),
        SnippetCheck("M274-B004-SPEC-02", "objc_part11_swift_metadata_and_isolation_mapping"),
    ),
    SPEC_METADATA: (
        SnippetCheck("M274-B004-META-01", "## M274 Swift metadata and isolation mapping semantics note"),
        SnippetCheck("M274-B004-META-02", CONTRACT_ID),
    ),
    SEMA_CONTRACT: (
        SnippetCheck("M274-B004-SC-01", "kObjc3Part11SwiftInteropIsolationSummaryContractId"),
        SnippetCheck("M274-B004-SC-02", "kObjc3Part11SwiftInteropIsolationSummarySurfacePath"),
        SnippetCheck("M274-B004-SC-03", "Objc3Part11SwiftInteropIsolationSummary"),
        SnippetCheck("M274-B004-SC-04", "IsReadyObjc3Part11SwiftInteropIsolationSummary("),
        SnippetCheck("M274-B004-SC-05", "implementation_surface_rejection_sites"),
    ),
    SEMA_PASSES: (
        SnippetCheck("M274-B004-SEMA-01", "BuildPart11SwiftInteropIsolationSummary("),
        SnippetCheck("M274-B004-SEMA-02", "ValidatePart11SwiftInteropIsolation("),
        SnippetCheck("M274-B004-SEMA-03", "HasPart11SwiftInteropAnnotations("),
        SnippetCheck("M274-B004-SEMA-04", '"O3S337"'),
        SnippetCheck("M274-B004-SEMA-05", '"O3S338"'),
        SnippetCheck("M274-B004-SEMA-06", '"O3S339"'),
        SnippetCheck("M274-B004-SEMA-07", '"O3S340"'),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M274-B004-TYP-01", "part11_swift_interop_isolation_summary"),
    ),
    FRONTEND_PIPELINE: (
        SnippetCheck("M274-B004-PIPE-01", "result.part11_swift_interop_isolation_summary ="),
        SnippetCheck("M274-B004-PIPE-02", "BuildPart11SwiftInteropIsolationSummary("),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M274-B004-ART-01", "BuildPart11SwiftInteropIsolationSummaryJson("),
        SnippetCheck("M274-B004-ART-02", "objc_part11_swift_metadata_and_isolation_mapping"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M274-B004-ARCH-01", "## M274 Part 11 Swift-facing metadata and isolation mapping completion (B004)"),
        SnippetCheck("M274-B004-ARCH-02", "objc_part11_swift_metadata_and_isolation_mapping"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-B004-PKG-01", "check:objc3c:m274-b004-part11-swift-metadata-and-isolation-mapping-completion-edge-case-and-compatibility-completion"),
        SnippetCheck("M274-B004-PKG-02", "test:tooling:m274-b004-part11-swift-metadata-and-isolation-mapping-completion-edge-case-and-compatibility-completion"),
        SnippetCheck("M274-B004-PKG-03", "check:objc3c:m274-b004-lane-b-readiness"),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M274-B004-FIX-P-01", "module Part11SwiftMetadataIsolationPositive;"),
        SnippetCheck("M274-B004-FIX-P-02", "objc_swift_name(named(\"SwiftBridge\"))"),
        SnippetCheck("M274-B004-FIX-P-03", "objc_swift_private"),
        SnippetCheck("M274-B004-FIX-P-04", "@interface SwiftMetadataBridge"),
    ),
    NEGATIVE_FIXTURE_337: (
        SnippetCheck("M274-B004-FIX-337-01", "Expected diagnostic code(s): O3S337."),
        SnippetCheck("M274-B004-FIX-337-02", "objc_swift_private"),
    ),
    NEGATIVE_FIXTURE_338: (
        SnippetCheck("M274-B004-FIX-338-01", "Expected diagnostic code(s): O3S338."),
        SnippetCheck("M274-B004-FIX-338-02", "actor class SwiftMailbox"),
    ),
    NEGATIVE_FIXTURE_339: (
        SnippetCheck("M274-B004-FIX-339-01", "Expected diagnostic code(s): O3S339."),
        SnippetCheck("M274-B004-FIX-339-02", "objc_nonisolated"),
    ),
    NEGATIVE_FIXTURE_340: (
        SnippetCheck("M274-B004-FIX-340-01", "Expected diagnostic code(s): O3S340."),
        SnippetCheck("M274-B004-FIX-340-02", "@implementation SwiftMetadataBridge"),
    ),
}


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M274-B004-DYN-01", "frontend runner missing", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "b004" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    checks_total += 1
    checks_passed += require(run.returncode == 0, display_path(POSITIVE_FIXTURE), "M274-B004-DYN-02", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M274-B004-DYN-03", "positive manifest missing", failures)

    payload: dict[str, Any] = {}
    surface_present = False
    if manifest_path.exists():
        manifest = load_json(manifest_path)
        surface = semantic_surface(manifest).get("objc_part11_swift_metadata_and_isolation_mapping")
        if isinstance(surface, dict):
            payload = surface
            surface_present = True
            expected_counts = {
                "swift_interop_callable_sites": 2,
                "swift_named_callable_sites": 2,
                "swift_private_callable_sites": 1,
                "swift_private_without_name_sites": 0,
                "actor_owned_swift_callable_sites": 0,
                "nonisolated_swift_callable_sites": 0,
                "implementation_swift_callable_sites": 0,
                "swift_private_without_name_rejection_sites": 0,
                "actor_isolation_mapping_rejection_sites": 0,
                "nonisolated_mapping_rejection_sites": 0,
                "implementation_surface_rejection_sites": 0,
            }
            for index, (field, expected_value) in enumerate(expected_counts.items(), start=4):
                checks_total += 1
                checks_passed += require(payload.get(field) == expected_value, display_path(manifest_path), f"M274-B004-DYN-{index:02d}", f"{field} mismatch", failures)

            true_fields = [
                "dependency_required",
                "swift_metadata_profile_reused",
                "swift_private_requires_name_enforced",
                "actor_isolation_mapping_fail_closed",
                "nonisolated_mapping_fail_closed",
                "implementation_surface_fail_closed",
                "ffi_abi_lowering_deferred",
                "runtime_bridge_generation_deferred",
                "deterministic",
                "ready_for_lowering_and_runtime",
            ]
            for index, field in enumerate(true_fields, start=20):
                checks_total += 1
                checks_passed += require(payload.get(field) is True, display_path(manifest_path), f"M274-B004-DYN-{index:02d}", f"{field} must be true", failures)

            checks_total += 2
            checks_passed += require(payload.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M274-B004-DYN-30", "contract_id mismatch", failures)
            checks_passed += require(payload.get("surface_path") == SURFACE_PATH, display_path(manifest_path), "M274-B004-DYN-31", "surface_path mismatch", failures)
        else:
            payload = {
                "surface_present": False,
                "manifest_surface_keys": sorted(semantic_surface(manifest).keys()),
            }

    negatives: dict[str, Any] = {}
    for index, (fixture, code) in enumerate(((NEGATIVE_FIXTURE_337, "O3S337"), (NEGATIVE_FIXTURE_338, "O3S338"), (NEGATIVE_FIXTURE_339, "O3S339"), (NEGATIVE_FIXTURE_340, "O3S340")), start=32):
        negative_run = run_command([
            str(args.runner_exe),
            str(fixture),
            "--out-dir",
            str(ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "b004" / fixture.stem),
            "--emit-prefix",
            "module",
            "--no-emit-ir",
            "--no-emit-object",
        ])
        negative_output = (negative_run.stdout or "") + (negative_run.stderr or "")
        checks_total += 2
        checks_passed += require(negative_run.returncode != 0, display_path(fixture), f"M274-B004-DYN-{index:02d}", "negative fixture unexpectedly succeeded", failures)
        checks_passed += require(code in negative_output, display_path(fixture), f"M274-B004-DYN-{index + 1:02d}", f"missing diagnostic {code}", failures)
        negatives[fixture.name] = {
            "returncode": negative_run.returncode,
            "output": negative_output.strip(),
            "expected_code": code,
        }

    return checks_total, checks_passed, {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "surface_key": SURFACE_PATH,
        "surface_present": surface_present,
        "surface_payload": payload,
        "negative_runs": negatives,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_contract: dict[str, Any] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        passed = ensure_snippets(path, snippets, failures)
        checks_passed += passed
        static_contract[display_path(path)] = {
            "checks_total": len(snippets),
            "checks_passed": passed,
        }

    dynamic_executed = not args.skip_dynamic_probes
    if dynamic_executed:
        dyn_total, dyn_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed
    else:
        dynamic_summary = {"skipped": True}

    ok = not failures and checks_total == checks_passed
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "static_contract": static_contract,
        "dynamic_probes_executed": dynamic_executed,
        "dynamic_summary": dynamic_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if ok:
        print(f"[ok] M274-B004 checker passed ({checks_passed}/{checks_total} checks)")
        print(f"[ok] summary: {display_path(args.summary_out)}")
        return 0

    print(f"[fail] M274-B004 checker failed ({checks_passed}/{checks_total} checks)", file=sys.stderr)
    print(f"[fail] summary: {display_path(args.summary_out)}", file=sys.stderr)
    for finding in failures:
        print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
