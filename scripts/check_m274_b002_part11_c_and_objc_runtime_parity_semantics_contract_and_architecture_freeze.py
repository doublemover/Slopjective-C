#!/usr/bin/env python3
"""Checker for M274-B002 Part 11 C and Objective-C runtime parity semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-b002-part11-c-and-objc-runtime-parity-semantics-v1"
CONTRACT_ID = "objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part11_c_and_objc_runtime_parity_semantics"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-B002" / "part11_c_and_objc_runtime_parity_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_b002_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze_packet.md"
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
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b002_part11_c_and_objc_runtime_parity_semantics_positive.objc3"
NEGATIVE_FIXTURE_331 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b002_part11_c_and_objc_runtime_parity_semantics_negative_o3s331.objc3"
NEGATIVE_FIXTURE_332 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b002_part11_c_and_objc_runtime_parity_semantics_negative_o3s332.objc3"
NEGATIVE_FIXTURE_333 = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_b002_part11_c_and_objc_runtime_parity_semantics_negative_o3s333.objc3"
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
        SnippetCheck("M274-B002-EXP-01", "# M274 Part 11 C And Objective-C Runtime Parity Semantics Contract And Architecture Freeze Expectations (B002)"),
        SnippetCheck("M274-B002-EXP-02", "Issue: `#7364`"),
        SnippetCheck("M274-B002-EXP-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-B002-EXP-04", "O3S331"),
        SnippetCheck("M274-B002-EXP-05", "O3S332"),
        SnippetCheck("M274-B002-EXP-06", "O3S333"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-B002-PKT-01", "# M274-B002 Packet: Part 11 C And Objective-C Runtime Parity Semantics - Contract And Architecture Freeze"),
        SnippetCheck("M274-B002-PKT-02", f"semantic surface `{SURFACE_PATH}`"),
        SnippetCheck("M274-B002-PKT-03", "Dependencies: `M274-B001`, `M274-A001`, `M274-A002`, `M267-E002`, `M268-E002`, `M270-E002`"),
        SnippetCheck("M274-B002-PKT-04", "O3S331"),
        SnippetCheck("M274-B002-PKT-05", "O3S332"),
        SnippetCheck("M274-B002-PKT-06", "O3S333"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M274-B002-DOCSRC-01", "## M274 Part 11 C and Objective-C runtime parity semantics"),
        SnippetCheck("M274-B002-DOCSRC-02", "objc_part11_c_and_objc_runtime_parity_semantics"),
        SnippetCheck("M274-B002-DOCSRC-03", "O3S331"),
        SnippetCheck("M274-B002-DOCSRC-04", "O3S332"),
        SnippetCheck("M274-B002-DOCSRC-05", "O3S333"),
    ),
    DOC_GENERATED: (
        SnippetCheck("M274-B002-DOCGEN-01", "## M274 Part 11 C and Objective-C runtime parity semantics"),
        SnippetCheck("M274-B002-DOCGEN-02", "objc_part11_c_and_objc_runtime_parity_semantics"),
    ),
    SPEC_CATALOG: (
        SnippetCheck("M274-B002-SPEC-01", "## M274 Part 11 C and Objective-C runtime parity semantics (B002)"),
        SnippetCheck("M274-B002-SPEC-02", "objc_part11_c_and_objc_runtime_parity_semantics"),
    ),
    SPEC_METADATA: (
        SnippetCheck("M274-B002-META-01", "## M274 C/Objective-C runtime parity semantics note"),
        SnippetCheck("M274-B002-META-02", CONTRACT_ID),
    ),
    SEMA_CONTRACT: (
        SnippetCheck("M274-B002-SC-01", "kObjc3Part11InteropRuntimeParitySummaryContractId"),
        SnippetCheck("M274-B002-SC-02", "kObjc3Part11InteropRuntimeParitySummarySurfacePath"),
        SnippetCheck("M274-B002-SC-03", "Objc3Part11InteropRuntimeParitySummary"),
        SnippetCheck("M274-B002-SC-04", "IsReadyObjc3Part11InteropRuntimeParitySummary("),
        SnippetCheck("M274-B002-SC-05", "foreign_definition_rejection_sites"),
        SnippetCheck("M274-B002-SC-06", "implementation_annotation_rejection_sites"),
    ),
    SEMA_PASSES: (
        SnippetCheck("M274-B002-SEMA-01", "BuildPart11InteropRuntimeParitySummary("),
        SnippetCheck("M274-B002-SEMA-02", "ValidatePart11InteropRuntimeParity("),
        SnippetCheck("M274-B002-SEMA-03", "\"O3S331\""),
        SnippetCheck("M274-B002-SEMA-04", "\"O3S332\""),
        SnippetCheck("M274-B002-SEMA-05", "\"O3S333\""),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M274-B002-TYP-01", "part11_interop_runtime_parity_summary"),
    ),
    FRONTEND_PIPELINE: (
        SnippetCheck("M274-B002-PIPE-01", "result.part11_interop_runtime_parity_summary ="),
        SnippetCheck("M274-B002-PIPE-02", "BuildPart11InteropRuntimeParitySummary("),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M274-B002-ART-01", "BuildPart11InteropRuntimeParitySummaryJson("),
        SnippetCheck("M274-B002-ART-02", "objc_part11_c_and_objc_runtime_parity_semantics"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M274-B002-ARCH-01", "## M274 Part 11 C and Objective-C runtime parity semantics (B002)"),
        SnippetCheck("M274-B002-ARCH-02", "objc_part11_c_and_objc_runtime_parity_semantics"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-B002-PKG-01", "check:objc3c:m274-b002-part11-c-and-objc-runtime-parity-semantics-contract-and-architecture-freeze"),
        SnippetCheck("M274-B002-PKG-02", "test:tooling:m274-b002-part11-c-and-objc-runtime-parity-semantics-contract-and-architecture-freeze"),
        SnippetCheck("M274-B002-PKG-03", "check:objc3c:m274-b002-lane-b-readiness"),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M274-B002-FIX-P-01", "module Part11CAndObjcRuntimeParityPositive;"),
        SnippetCheck("M274-B002-FIX-P-02", "objc_foreign"),
        SnippetCheck("M274-B002-FIX-P-03", "objc_import_module(named(\"SampleKit\"))"),
        SnippetCheck("M274-B002-FIX-P-04", "@protocol RuntimeParityProtocol"),
        SnippetCheck("M274-B002-FIX-P-05", "@interface RuntimeParityBridge"),
        SnippetCheck("M274-B002-FIX-P-06", "@implementation RuntimeParityBridge"),
    ),
    NEGATIVE_FIXTURE_331: (
        SnippetCheck("M274-B002-FIX-331-01", "Expected diagnostic code(s): O3S331."),
        SnippetCheck("M274-B002-FIX-331-02", "objc_foreign"),
    ),
    NEGATIVE_FIXTURE_332: (
        SnippetCheck("M274-B002-FIX-332-01", "Expected diagnostic code(s): O3S332."),
        SnippetCheck("M274-B002-FIX-332-02", "objc_import_module(named(\"SampleKit\"))"),
    ),
    NEGATIVE_FIXTURE_333: (
        SnippetCheck("M274-B002-FIX-333-01", "Expected diagnostic code(s): O3S333."),
        SnippetCheck("M274-B002-FIX-333-02", "@implementation RuntimeParityBridge (Debug)"),
        SnippetCheck("M274-B002-FIX-333-03", "objc_import_module(named(\"SampleKit\"))"),
    ),
}


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M274-B002-DYN-01", "frontend runner missing", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "b002" / "positive"
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
    checks_passed += require(run.returncode == 0, display_path(POSITIVE_FIXTURE), "M274-B002-DYN-02", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M274-B002-DYN-03", "positive manifest missing", failures)

    payload: dict[str, Any] = {}
    surface_present = False
    if manifest_path.exists():
        manifest = load_json(manifest_path)
        surface = semantic_surface(manifest).get("objc_part11_c_and_objc_runtime_parity_semantics")
        if isinstance(surface, dict):
            payload = surface
            surface_present = True
            expected_counts = {
                "foreign_callable_sites": 4,
                "c_foreign_callable_sites": 2,
                "objc_method_foreign_callable_sites": 2,
                "import_module_annotation_sites": 2,
                "import_module_foreign_callable_sites": 2,
                "objc_runtime_parity_callable_sites": 3,
                "foreign_definition_rejection_sites": 0,
                "import_without_foreign_rejection_sites": 0,
                "implementation_annotation_rejection_sites": 0,
            }
            for index, (field, expected_value) in enumerate(expected_counts.items(), start=4):
                checks_total += 1
                checks_passed += require(payload.get(field) == expected_value, display_path(manifest_path), f"M274-B002-DYN-{index:02d}", f"{field} mismatch", failures)

            true_fields = [
                "dependency_required",
                "declaration_only_foreign_c_enforced",
                "import_module_requires_foreign_enforced",
                "implementation_annotations_fail_closed",
                "objc_runtime_parity_classified",
                "ffi_abi_lowering_deferred",
                "runtime_bridge_generation_deferred",
                "deterministic",
                "ready_for_lowering_and_runtime",
            ]
            for index, field in enumerate(true_fields, start=20):
                checks_total += 1
                checks_passed += require(payload.get(field) is True, display_path(manifest_path), f"M274-B002-DYN-{index:02d}", f"{field} must be true", failures)

            checks_total += 2
            checks_passed += require(payload.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M274-B002-DYN-29", "contract_id mismatch", failures)
            checks_passed += require(payload.get("surface_path") == SURFACE_PATH, display_path(manifest_path), "M274-B002-DYN-30", "surface_path mismatch", failures)
        else:
            payload = {
                "surface_present": False,
                "manifest_surface_keys": sorted(semantic_surface(manifest).keys()),
            }

    return checks_total, checks_passed, {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": run.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "surface_key": SURFACE_PATH,
        "surface_present": surface_present,
        "surface_payload": payload,
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
        print(f"[ok] M274-B002 checker passed ({checks_passed}/{checks_total} checks)")
        print(f"[ok] summary: {display_path(args.summary_out)}")
        return 0

    print(f"[fail] M274-B002 checker failed ({checks_passed}/{checks_total} checks)", file=sys.stderr)
    print(f"[fail] summary: {display_path(args.summary_out)}", file=sys.stderr)
    for finding in failures:
        print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
