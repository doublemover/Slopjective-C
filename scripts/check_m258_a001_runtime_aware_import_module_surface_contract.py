#!/usr/bin/env python3
"""Fail-closed checker for M258-A001 runtime-aware import/module surface."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-a001-runtime-aware-import-module-surface-contract-v1"
CONTRACT_ID = "objc3c-runtime-aware-import-module-surface/m258-a001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract"
SURFACE_KEY = "objc_runtime_aware_import_module_surface_contract"
SOURCE_MODEL = "runtime-aware-import-module-surface-freezes-frontend-owned-runtime-declaration-and-metadata-reference-boundaries-before-cross-translation-unit-realization"
NON_GOAL_MODEL = "no-imported-module-artifact-reader-no-imported-runtime-declaration-materialization-no-imported-runtime-metadata-reference-lowering"
FAILURE_MODEL = "fail-closed-on-runtime-aware-import-module-surface-drift-or-premature-capability-claims"
NEXT_ISSUE = "M258-A002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-A001" / "runtime_aware_import_module_surface_contract_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "a001-runtime-aware-import-module-surface"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_runtime_aware_import_and_module_surface_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_a001_runtime_aware_import_and_module_surface_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_API = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_a001_runtime_aware_import_module_surface_contract.py"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"

EXPECTED_MODULE = "runtimeMetadataClassRecords"
EXPECTED_PROTOCOL_COUNT = 2
EXPECTED_INTERFACE_COUNT = 1
EXPECTED_IMPLEMENTATION_COUNT = 1
EXPECTED_INTERFACE_CATEGORY_COUNT = 0
EXPECTED_IMPLEMENTATION_CATEGORY_COUNT = 0
EXPECTED_FUNCTION_COUNT = 1
EXPECTED_MODULE_IMPORT_GRAPH_SITES = 0
EXPECTED_IMPORT_EDGE_CANDIDATE_SITES = 0
EXPECTED_NAMESPACE_SEGMENT_SITES = 0
EXPECTED_OBJECT_POINTER_TYPE_SITES = 10
EXPECTED_POINTER_DECLARATOR_SITES = 0
EXPECTED_NORMALIZED_SITES = 0
EXPECTED_CONTRACT_VIOLATION_SITES = 0


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[str, tuple[Path, tuple[SnippetCheck, ...]]] = {
    "expectations": (
        EXPECTATIONS_DOC,
        (
            SnippetCheck("M258-A001-DOC-EXP-01", "# M258 Runtime-Aware Import and Module Surface Contract and Architecture Freeze Expectations (A001)"),
            SnippetCheck("M258-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M258-A001-DOC-EXP-03", "Issue: `#7158`"),
            SnippetCheck("M258-A001-DOC-EXP-04", f"`{SURFACE_PATH}`"),
            SnippetCheck("M258-A001-DOC-EXP-05", "The frozen landed flags remain explicitly `false`"),
        ),
    ),
    "packet": (
        PACKET_DOC,
        (
            SnippetCheck("M258-A001-DOC-PKT-01", "# M258-A001 Runtime-Aware Import and Module Surface Contract and Architecture Freeze Packet"),
            SnippetCheck("M258-A001-DOC-PKT-02", "Packet: `M258-A001`"),
            SnippetCheck("M258-A001-DOC-PKT-03", "Dependencies: none"),
            SnippetCheck("M258-A001-DOC-PKT-04", "Next issue: `M258-A002`"),
            SnippetCheck("M258-A001-DOC-PKT-05", f"`{CONTRACT_ID}`"),
        ),
    ),
    "native_doc": (
        NATIVE_DOC,
        (
            SnippetCheck("M258-A001-NDOC-01", "## Runtime-aware import and module surface (M258-A001)"),
            SnippetCheck("M258-A001-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M258-A001-NDOC-03", f"`{SURFACE_PATH}`"),
            SnippetCheck("M258-A001-NDOC-04", "the public frontend embedding ABI still exposes no imported-module handle"),
        ),
    ),
    "lowering_spec": (
        LOWERING_SPEC,
        (
            SnippetCheck("M258-A001-SPC-01", "## M258 runtime-aware import/module surface freeze (A001)"),
            SnippetCheck("M258-A001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M258-A001-SPC-03", f"`{NON_GOAL_MODEL}`"),
            SnippetCheck("M258-A001-SPC-04", f"`{FAILURE_MODEL}`"),
        ),
    ),
    "metadata_spec": (
        METADATA_SPEC,
        (
            SnippetCheck("M258-A001-META-01", "## M258 runtime-aware import/module metadata anchors (A001)"),
            SnippetCheck("M258-A001-META-02", f"`{SURFACE_PATH}`"),
            SnippetCheck("M258-A001-META-03", "imported runtime-owned declaration surface"),
            SnippetCheck("M258-A001-META-04", "imported runtime metadata reference surface"),
        ),
    ),
    "architecture": (
        ARCHITECTURE_DOC,
        (
            SnippetCheck("M258-A001-ARCH-01", "## M258 runtime-aware import and module surface (A001)"),
            SnippetCheck("M258-A001-ARCH-02", "objc_runtime_aware_import_module_surface_contract"),
            SnippetCheck("M258-A001-ARCH-03", "fail closed on drift or premature capability claims"),
        ),
    ),
    "frontend_artifacts": (
        FRONTEND_ARTIFACTS,
        (
            SnippetCheck("M258-A001-ART-01", "kObjc3RuntimeAwareImportModuleSurfaceContractId"),
            SnippetCheck("M258-A001-ART-02", "BuildRuntimeAwareImportModuleSurfaceSummaryJson("),
            SnippetCheck("M258-A001-ART-03", "objc_runtime_aware_import_module_surface_contract"),
        ),
    ),
    "ir_emitter": (
        IR_EMITTER,
        (
            SnippetCheck("M258-A001-IR-01", "M258-A001 runtime-aware import/module surface anchor"),
            SnippetCheck("M258-A001-IR-02", "Imported runtime-owned declarations and foreign metadata references"),
        ),
    ),
    "frontend_api": (
        FRONTEND_API,
        (
            SnippetCheck("M258-A001-API-01", "M258-A001 runtime-aware import/module surface anchor"),
            SnippetCheck("M258-A001-API-02", "does not yet expose imported module handles"),
        ),
    ),
    "package_json": (
        PACKAGE_JSON,
        (
            SnippetCheck("M258-A001-PKG-01", '"check:objc3c:m258-a001-runtime-aware-import-and-module-surface-contract"'),
            SnippetCheck("M258-A001-PKG-02", '"test:tooling:m258-a001-runtime-aware-import-and-module-surface-contract"'),
            SnippetCheck("M258-A001-PKG-03", '"check:objc3c:m258-a001-lane-a-readiness"'),
        ),
    ),
    "readiness_runner": (
        READINESS_RUNNER,
        (
            SnippetCheck("M258-A001-RUN-01", 'DEPENDENCY_TOKEN = "none"'),
            SnippetCheck("M258-A001-RUN-02", "check_m258_a001_runtime_aware_import_module_surface_contract.py"),
        ),
    ),
    "test_file": (
        TEST_FILE,
        (
            SnippetCheck("M258-A001-TEST-01", "def test_checker_passes_static_contract"),
            SnippetCheck("M258-A001-TEST-02", "def test_checker_fails_when_frontend_api_anchor_drifts"),
        ),
    ),
    "fixture": (
        FIXTURE,
        (
            SnippetCheck("M258-A001-FIX-01", "module runtimeMetadataClassRecords;"),
            SnippetCheck("M258-A001-FIX-02", "@protocol Worker<Base>"),
            SnippetCheck("M258-A001-FIX-03", "@implementation Widget"),
        ),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, failures
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def compile_fixture(out_dir: Path) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-A001-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(FIXTURE.exists(), display_path(FIXTURE), "M258-A001-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    manifest_path = out_dir / "module.manifest.json"
    backend_path = out_dir / "module.object-backend.txt"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M258-A001-COMPILE", "fixture must compile successfully", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M258-A001-MANIFEST", "module manifest is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M258-A001-BACKEND", "backend marker is missing", failures)
    if failures:
        return checks_total, failures, {
            "stdout": completed.stdout,
            "stderr": completed.stderr,
            "manifest_path": display_path(manifest_path),
            "backend_path": display_path(backend_path),
        }

    manifest = load_json(manifest_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
    surface = semantic_surface[SURFACE_KEY]
    module_import_graph_surface = semantic_surface["objc_module_import_graph_lowering_surface"]

    checks_total += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M258-A001-SURF-CONTRACT", "surface contract id mismatch", failures)
    checks_total += require(surface.get("surface_path") == SURFACE_PATH, display_path(manifest_path), "M258-A001-SURF-PATH", "surface path mismatch", failures)
    checks_total += require(surface.get("source_model") == SOURCE_MODEL, display_path(manifest_path), "M258-A001-SURF-SOURCE", "source model mismatch", failures)
    checks_total += require(surface.get("non_goal_model") == NON_GOAL_MODEL, display_path(manifest_path), "M258-A001-SURF-NONGOAL", "non-goal model mismatch", failures)
    checks_total += require(surface.get("failure_model") == FAILURE_MODEL, display_path(manifest_path), "M258-A001-SURF-FAIL", "failure model mismatch", failures)
    checks_total += require(surface.get("module_name") == EXPECTED_MODULE, display_path(manifest_path), "M258-A001-SURF-MODULE", "module name mismatch", failures)
    checks_total += require(surface.get("protocol_decl_count") == EXPECTED_PROTOCOL_COUNT, display_path(manifest_path), "M258-A001-SURF-PROTOCOLS", "protocol declaration count mismatch", failures)
    checks_total += require(surface.get("interface_decl_count") == EXPECTED_INTERFACE_COUNT, display_path(manifest_path), "M258-A001-SURF-INTERFACES", "interface declaration count mismatch", failures)
    checks_total += require(surface.get("implementation_decl_count") == EXPECTED_IMPLEMENTATION_COUNT, display_path(manifest_path), "M258-A001-SURF-IMPLEMENTATIONS", "implementation declaration count mismatch", failures)
    checks_total += require(surface.get("interface_category_decl_count") == EXPECTED_INTERFACE_CATEGORY_COUNT, display_path(manifest_path), "M258-A001-SURF-ICATS", "interface category count mismatch", failures)
    checks_total += require(surface.get("implementation_category_decl_count") == EXPECTED_IMPLEMENTATION_CATEGORY_COUNT, display_path(manifest_path), "M258-A001-SURF-MCATS", "implementation category count mismatch", failures)
    checks_total += require(surface.get("function_decl_count") == EXPECTED_FUNCTION_COUNT, display_path(manifest_path), "M258-A001-SURF-FNS", "function declaration count mismatch", failures)
    checks_total += require(surface.get("module_import_graph_sites") == EXPECTED_MODULE_IMPORT_GRAPH_SITES, display_path(manifest_path), "M258-A001-SURF-MIG", "module import graph site count mismatch", failures)
    checks_total += require(surface.get("import_edge_candidate_sites") == EXPECTED_IMPORT_EDGE_CANDIDATE_SITES, display_path(manifest_path), "M258-A001-SURF-EDGES", "import edge candidate count mismatch", failures)
    checks_total += require(surface.get("namespace_segment_sites") == EXPECTED_NAMESPACE_SEGMENT_SITES, display_path(manifest_path), "M258-A001-SURF-NS", "namespace segment site count mismatch", failures)
    checks_total += require(surface.get("object_pointer_type_sites") == EXPECTED_OBJECT_POINTER_TYPE_SITES, display_path(manifest_path), "M258-A001-SURF-OBJPTR", "object pointer site count mismatch", failures)
    checks_total += require(surface.get("pointer_declarator_sites") == EXPECTED_POINTER_DECLARATOR_SITES, display_path(manifest_path), "M258-A001-SURF-PTRDECL", "pointer declarator site count mismatch", failures)
    checks_total += require(surface.get("normalized_sites") == EXPECTED_NORMALIZED_SITES, display_path(manifest_path), "M258-A001-SURF-NORM", "normalized site count mismatch", failures)
    checks_total += require(surface.get("contract_violation_sites") == EXPECTED_CONTRACT_VIOLATION_SITES, display_path(manifest_path), "M258-A001-SURF-VIOL", "contract violation site count mismatch", failures)
    checks_total += require(surface.get("runtime_aware_import_declarations_landed") is False, display_path(manifest_path), "M258-A001-SURF-LANDED-IMPORTDECL", "runtime-aware import declarations must remain unlanded", failures)
    checks_total += require(surface.get("module_metadata_import_surface_landed") is False, display_path(manifest_path), "M258-A001-SURF-LANDED-META", "module metadata import surface must remain unlanded", failures)
    checks_total += require(surface.get("runtime_owned_declaration_import_landed") is False, display_path(manifest_path), "M258-A001-SURF-LANDED-RUNTIMEDECL", "runtime-owned declaration import surface must remain unlanded", failures)
    checks_total += require(surface.get("runtime_metadata_reference_import_landed") is False, display_path(manifest_path), "M258-A001-SURF-LANDED-RUNTIMEMETA", "runtime metadata reference surface must remain unlanded", failures)
    checks_total += require(surface.get("public_frontend_api_module_surface_landed") is False, display_path(manifest_path), "M258-A001-SURF-LANDED-API", "public frontend API module surface must remain unlanded", failures)
    checks_total += require(surface.get("fail_closed") is True, display_path(manifest_path), "M258-A001-SURF-FAILCLOSED", "surface must remain fail closed", failures)
    checks_total += require(surface.get("deterministic") is True, display_path(manifest_path), "M258-A001-SURF-DETERMINISTIC", "surface must remain deterministic", failures)
    checks_total += require(surface.get("ready_for_core_feature_implementation") is True, display_path(manifest_path), "M258-A001-SURF-READY", "surface should be marked ready for A002", failures)
    checks_total += require(surface.get("next_issue") == NEXT_ISSUE, display_path(manifest_path), "M258-A001-SURF-NEXT", "next issue mismatch", failures)
    checks_total += require(CONTRACT_ID in str(surface.get("replay_key", "")), display_path(manifest_path), "M258-A001-SURF-REPLAY", "replay key must carry contract id", failures)

    checks_total += require(module_import_graph_surface.get("module_import_graph_sites") == surface.get("module_import_graph_sites"), display_path(manifest_path), "M258-A001-MIG-SYNC-01", "module import graph site count must stay synchronized", failures)
    checks_total += require(module_import_graph_surface.get("import_edge_candidate_sites") == surface.get("import_edge_candidate_sites"), display_path(manifest_path), "M258-A001-MIG-SYNC-02", "import edge candidate count must stay synchronized", failures)
    checks_total += require(module_import_graph_surface.get("object_pointer_type_sites") == surface.get("object_pointer_type_sites"), display_path(manifest_path), "M258-A001-MIG-SYNC-03", "object pointer site count must stay synchronized", failures)
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M258-A001-BACKEND-TEXT", "backend marker must remain llvm-direct", failures)

    evidence = {
        "fixture": display_path(FIXTURE),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "backend_path": display_path(backend_path),
        "backend_text": backend_text,
        "module_name": surface.get("module_name"),
        "protocol_decl_count": surface.get("protocol_decl_count"),
        "interface_decl_count": surface.get("interface_decl_count"),
        "implementation_decl_count": surface.get("implementation_decl_count"),
        "object_pointer_type_sites": surface.get("object_pointer_type_sites"),
    }
    return checks_total, failures, evidence


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--frontend-api", type=Path, default=FRONTEND_API)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_sources = dict(STATIC_SNIPPETS)
    static_sources["expectations"] = (args.expectations_doc, static_sources["expectations"][1])
    static_sources["frontend_api"] = (args.frontend_api, static_sources["frontend_api"][1])

    for path, snippets in static_sources.values():
        count, findings = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(findings)

    dynamic_case: dict[str, object] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_count, dynamic_failures, dynamic_case = compile_fixture(PROBE_ROOT)
        checks_total += dynamic_count
        failures.extend(dynamic_failures)

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "source_model": SOURCE_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_case,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
