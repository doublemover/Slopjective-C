#!/usr/bin/env python3
"""Fail-closed checker for M252-E002 integrated metadata-closure corpus sync."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure-v1"
CONTRACT_ID = "objc3c-metadata-graph-closure-conformance-corpus-doc-sync/m252-e002-v1"
RUNNER_SURFACE_KEYS = (
    "objc_executable_metadata_source_graph",
    "objc_executable_metadata_typed_lowering_handoff",
    "objc_executable_metadata_debug_projection",
    "objc_executable_metadata_runtime_ingest_packaging_contract",
    "objc_executable_metadata_runtime_ingest_binary_boundary",
)

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m252_conformance_corpus_and_docs_sync_for_metadata_graph_closure_e002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m252"
    / "m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_RUNNER_SCRIPT = ROOT / "scripts" / "run_m252_e002_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_E001_SUMMARY = ROOT / "tmp" / "reports" / "m252" / "M252-E001" / "metadata_semantic_closure_gate_summary.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "e002-metadata-closure-conformance"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json")
DEFAULT_CLASS_GRAPH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_GRAPH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_CLASS_SYNTHESIS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_class_property_synthesis_ready.objc3"
DEFAULT_CATEGORY_EXPORT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_category_property_export_only.objc3"
DEFAULT_MISSING_INTERFACE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_missing_interface_property.objc3"
DEFAULT_INCOMPATIBLE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_incompatible_property_signature.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-DOC-EXP-01", "# M252 Conformance Corpus And Docs Sync For Metadata Graph Closure Expectations (E002)"),
    SnippetCheck("M252-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-E002-DOC-EXP-03", "`class-protocol-property-ivar-runtime-graph`"),
    SnippetCheck("M252-E002-DOC-EXP-04", "`category-protocol-property-runtime-graph`"),
    SnippetCheck("M252-E002-DOC-EXP-05", "`missing-interface-property-diagnostic`"),
    SnippetCheck("M252-E002-DOC-EXP-06", "`scripts/run_m252_e002_lane_e_readiness.py`"),
    SnippetCheck("M252-E002-DOC-EXP-07", "`M252-E001`"),
    SnippetCheck("M252-E002-DOC-EXP-08", "`tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json`"),
)
PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-DOC-PKT-01", "# M252-E002 Conformance Corpus And Docs Sync For Metadata Graph Closure Packet"),
    SnippetCheck("M252-E002-DOC-PKT-02", "Packet: `M252-E002`"),
    SnippetCheck("M252-E002-DOC-PKT-03", "Issue: `#7084`"),
    SnippetCheck("M252-E002-DOC-PKT-04", "`M252-A003`"),
    SnippetCheck("M252-E002-DOC-PKT-05", "`M252-B004`"),
    SnippetCheck("M252-E002-DOC-PKT-06", "`M252-C003`"),
    SnippetCheck("M252-E002-DOC-PKT-07", "`M252-D002`"),
    SnippetCheck("M252-E002-DOC-PKT-08", "`M252-E001`"),
    SnippetCheck("M252-E002-DOC-PKT-09", "`scripts/run_m252_e002_lane_e_readiness.py`"),
)
ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-ARCH-01", "M252 lane-E E002 metadata-closure corpus sync anchors explicit integrated"),
    SnippetCheck("M252-E002-ARCH-02", "`class-protocol-property-ivar-runtime-graph`"),
    SnippetCheck("M252-E002-ARCH-03", "`scripts/run_m252_e002_lane_e_readiness.py`"),
)
NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-NDOC-01", "## Conformance corpus and docs sync for metadata graph closure (M252-E002)"),
    SnippetCheck("M252-E002-NDOC-02", "`artifacts/bin/objc3c-frontend-c-api-runner.exe`"),
    SnippetCheck("M252-E002-NDOC-03", "`class-property-synthesis-ready`"),
)
LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-SPC-01", "## M252 conformance corpus and docs sync for metadata graph closure (E002)"),
    SnippetCheck("M252-E002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M252-E002-SPC-03", "real frontend runner path"),
)
METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-META-01", "## M252 metadata closure conformance corpus metadata anchors (E002)"),
    SnippetCheck("M252-E002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M252-E002-META-03", "`tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json`"),
)
PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-PARSE-01", "M252-E002 corpus-sync anchor: representative class/category/property/ivar"),
)
SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-SEMA-01", "M252-E002 corpus-sync anchor: representative legality corpus cases keep"),
)
SEMA_PASSES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-PASS-01", "M252-E002 corpus-sync anchor: the integrated corpus gate replays this"),
)
FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-ART-01", "M252-E002 corpus-sync anchor: integrated corpus probes must"),
)
RUNNER_SCRIPT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-RUN-01", '"check:objc3c:m252-a003-protocol-category-property-ivar-export-graph-completion"'),
    SnippetCheck("M252-E002-RUN-02", '"check:objc3c:m252-b004-property-ivar-export-legality-synthesis-preconditions"'),
    SnippetCheck("M252-E002-RUN-03", '"check:objc3c:m252-c003-metadata-debug-projection-and-replay-anchors"'),
    SnippetCheck("M252-E002-RUN-04", '"check:objc3c:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads"'),
    SnippetCheck("M252-E002-RUN-05", '"check:objc3c:m252-e001-metadata-semantic-closure-gate"'),
    SnippetCheck("M252-E002-RUN-06", '"scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py"'),
)
PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E002-PKG-01", '"check:objc3c:m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure": "python scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py"'),
    SnippetCheck("M252-E002-PKG-02", '"test:tooling:m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure": "python -m pytest tests/tooling/test_check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py -q"'),
    SnippetCheck("M252-E002-PKG-03", '"check:objc3c:m252-e002-lane-e-readiness": "python scripts/run_m252_e002_lane_e_readiness.py"'),
)


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


def load_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    failures: list[Finding] = []
    if not path.exists():
        failures.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, failures
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--runner-script", type=Path, default=DEFAULT_RUNNER_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--e001-summary", type=Path, default=DEFAULT_E001_SUMMARY)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--class-graph-fixture", type=Path, default=DEFAULT_CLASS_GRAPH_FIXTURE)
    parser.add_argument("--category-graph-fixture", type=Path, default=DEFAULT_CATEGORY_GRAPH_FIXTURE)
    parser.add_argument("--class-synthesis-fixture", type=Path, default=DEFAULT_CLASS_SYNTHESIS_FIXTURE)
    parser.add_argument("--category-export-fixture", type=Path, default=DEFAULT_CATEGORY_EXPORT_FIXTURE)
    parser.add_argument("--missing-interface-fixture", type=Path, default=DEFAULT_MISSING_INTERFACE_FIXTURE)
    parser.add_argument("--incompatible-fixture", type=Path, default=DEFAULT_INCOMPATIBLE_FIXTURE)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def run_runner(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, check=False)


def normalize_case_output(out_dir: Path) -> tuple[Path, Path, Path, Path]:
    return (
        out_dir / "module.c_api_summary.json",
        out_dir / "module.manifest.json",
        out_dir / "module.diagnostics.json",
        out_dir / "module.runtime-metadata.bin",
    )


def load_diagnostics(path: Path) -> list[dict[str, Any]]:
    if not path.exists():
        return []
    payload = json.loads(path.read_text(encoding="utf-8"))
    if isinstance(payload, list):
        return [entry for entry in payload if isinstance(entry, dict)]
    if isinstance(payload, dict):
        diagnostics = payload.get("diagnostics")
        if isinstance(diagnostics, list):
            return [entry for entry in diagnostics if isinstance(entry, dict)]
    raise TypeError(f"expected diagnostics payload in {display_path(path)}")


def run_positive_graph_case(*, runner_exe: Path, fixture: Path, out_dir: Path, check_prefix: str, case_id: str) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture.exists(), display_path(fixture), f"{check_prefix}-FIXTURE", "fixture is missing", failures)
    checks_total += require(runner_exe.exists(), display_path(runner_exe), f"{check_prefix}-RUNNER", "runner executable is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(runner_exe), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    completed = run_runner(command)
    summary_path, manifest_path, diagnostics_path, binary_path = normalize_case_output(out_dir)
    checks_total += require(summary_path.exists(), display_path(summary_path), f"{check_prefix}-SUMMARY", "runner summary missing", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), f"{check_prefix}-MANIFEST", "runner manifest missing", failures)
    checks_total += require(binary_path.exists(), display_path(binary_path), f"{check_prefix}-BINARY", "runtime metadata binary missing", failures)
    if failures:
        return checks_total, failures, {}

    summary_payload = load_json_object(summary_path)
    manifest_payload = load_json_object(manifest_path)
    diagnostics = load_diagnostics(diagnostics_path)
    checks_total += require(completed.returncode == 0, display_path(summary_path), f"{check_prefix}-EXIT", "runner exit code mismatch", failures)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), f"{check_prefix}-SUCCESS", "runner success mismatch", failures)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), f"{check_prefix}-STATUS", "runner status mismatch", failures)
    checks_total += require(diagnostics == [], display_path(diagnostics_path), f"{check_prefix}-DIAGNOSTICS", "positive graph case must stay diagnostic-clean", failures)

    frontend = manifest_payload.get("frontend") if isinstance(manifest_payload.get("frontend"), dict) else {}
    pipeline = frontend.get("pipeline") if isinstance(frontend.get("pipeline"), dict) else {}
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline.get("semantic_surface"), dict) else {}
    for surface_key in RUNNER_SURFACE_KEYS:
        checks_total += require(isinstance(semantic_surface.get(surface_key), dict), display_path(manifest_path), f"{check_prefix}-SURFACE-{surface_key.upper()}", f"required semantic surface missing: {surface_key}", failures)
    source_graph = semantic_surface.get("objc_executable_metadata_source_graph") if isinstance(semantic_surface.get("objc_executable_metadata_source_graph"), dict) else {}
    checks_total += require(source_graph.get("source_graph_complete") is True, display_path(manifest_path), f"{check_prefix}-COMPLETE", "source graph must remain complete", failures)
    if case_id == "class-protocol-property-ivar-runtime-graph":
        checks_total += require(source_graph.get("protocol_nodes", 0) >= 2, display_path(manifest_path), f"{check_prefix}-PROTOCOLS", "class graph case must preserve protocol nodes", failures)
        checks_total += require(source_graph.get("property_nodes", 0) >= 3, display_path(manifest_path), f"{check_prefix}-PROPERTIES", "class graph case must preserve property nodes", failures)
        checks_total += require(source_graph.get("ivar_nodes", 0) >= 1, display_path(manifest_path), f"{check_prefix}-IVARS", "class graph case must preserve ivar nodes", failures)
    else:
        checks_total += require(source_graph.get("category_nodes", 0) >= 1, display_path(manifest_path), f"{check_prefix}-CATEGORIES", "category graph case must preserve category nodes", failures)
        checks_total += require(source_graph.get("property_nodes", 0) >= 3, display_path(manifest_path), f"{check_prefix}-PROPERTIES", "category graph case must preserve property nodes", failures)

    return checks_total, failures, {
        "case_id": case_id,
        "fixture": display_path(fixture),
        "success": True,
        "runtime_metadata_binary_path": display_path(binary_path),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def run_positive_legality_case(*, runner_exe: Path, fixture: Path, out_dir: Path, check_prefix: str, case_id: str, expected_synthesis_sites: int, expected_binding_resolved: int) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture.exists(), display_path(fixture), f"{check_prefix}-FIXTURE", "fixture is missing", failures)
    checks_total += require(runner_exe.exists(), display_path(runner_exe), f"{check_prefix}-RUNNER", "runner executable is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(runner_exe), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    completed = run_runner(command)
    summary_path, manifest_path, diagnostics_path, _ = normalize_case_output(out_dir)
    checks_total += require(summary_path.exists(), display_path(summary_path), f"{check_prefix}-SUMMARY", "runner summary missing", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), f"{check_prefix}-MANIFEST", "runner manifest missing", failures)
    if failures:
        return checks_total, failures, {}

    summary_payload = load_json_object(summary_path)
    manifest_payload = load_json_object(manifest_path)
    diagnostics = load_diagnostics(diagnostics_path)
    checks_total += require(completed.returncode == 0, display_path(summary_path), f"{check_prefix}-EXIT", "runner exit code mismatch", failures)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), f"{check_prefix}-SUCCESS", "runner success mismatch", failures)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), f"{check_prefix}-STATUS", "runner status mismatch", failures)
    checks_total += require(diagnostics == [], display_path(diagnostics_path), f"{check_prefix}-DIAGNOSTICS", "positive legality case must stay diagnostic-clean", failures)

    frontend = manifest_payload.get("frontend") if isinstance(manifest_payload.get("frontend"), dict) else {}
    pipeline = frontend.get("pipeline") if isinstance(frontend.get("pipeline"), dict) else {}
    sema = pipeline.get("sema_pass_manager") if isinstance(pipeline.get("sema_pass_manager"), dict) else {}
    checks_total += require(sema.get("property_synthesis_sites") == expected_synthesis_sites, display_path(manifest_path), f"{check_prefix}-SYNTHESIS", "property synthesis count drifted", failures)
    checks_total += require(sema.get("ivar_binding_resolved") == expected_binding_resolved, display_path(manifest_path), f"{check_prefix}-BINDING-RESOLVED", "ivar binding resolution count drifted", failures)

    return checks_total, failures, {
        "case_id": case_id,
        "fixture": display_path(fixture),
        "success": True,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def run_negative_legality_case(*, runner_exe: Path, fixture: Path, out_dir: Path, check_prefix: str, case_id: str) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture.exists(), display_path(fixture), f"{check_prefix}-FIXTURE", "fixture is missing", failures)
    checks_total += require(runner_exe.exists(), display_path(runner_exe), f"{check_prefix}-RUNNER", "runner executable is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(runner_exe), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    completed = run_runner(command)
    summary_path, _, diagnostics_path, _ = normalize_case_output(out_dir)
    checks_total += require(summary_path.exists(), display_path(summary_path), f"{check_prefix}-SUMMARY", "runner summary missing", failures)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), f"{check_prefix}-DIAGNOSTICS-FILE", "runner diagnostics missing", failures)
    if failures:
        return checks_total, failures, {}

    summary_payload = load_json_object(summary_path)
    diagnostics = load_diagnostics(diagnostics_path)
    observed_codes = [entry.get("code") for entry in diagnostics if isinstance(entry.get("code"), str)]
    checks_total += require(completed.returncode == 1, display_path(summary_path), f"{check_prefix}-EXIT", "negative legality case must fail with exit code 1", failures)
    checks_total += require(summary_payload.get("success") is False, display_path(summary_path), f"{check_prefix}-SUCCESS", "negative legality case must report success=false", failures)
    checks_total += require(summary_payload.get("status") == 1, display_path(summary_path), f"{check_prefix}-STATUS", "negative legality case must report status=1", failures)
    checks_total += require(observed_codes == ["O3S206"], display_path(diagnostics_path), f"{check_prefix}-O3S206", f"expected ['O3S206'], observed {observed_codes}", failures)

    return checks_total, failures, {
        "case_id": case_id,
        "fixture": display_path(fixture),
        "success": False,
        "observed_codes": observed_codes,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0

    docs_to_check = (
        (args.expectations_doc, "M252-E002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-E002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-E002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-E002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-E002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-E002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-E002-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-E002-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-E002-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_artifacts, "M252-E002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.runner_script, "M252-E002-RUN-EXISTS", RUNNER_SCRIPT_SNIPPETS),
        (args.package_json, "M252-E002-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in docs_to_check:
        total, doc_failures = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += total
        failures.extend(doc_failures)

    checks_total += require(args.e001_summary.exists(), display_path(args.e001_summary), "M252-E002-E001-EXISTS", "E001 summary is missing", failures)
    if args.e001_summary.exists():
        e001_payload = load_json_object(args.e001_summary)
        checks_total += require(e001_payload.get("ok") is True, display_path(args.e001_summary), "M252-E002-E001-OK", "E001 summary must report ok: true", failures)
        checks_total += require(e001_payload.get("aggregate_semantic_closure_ready_for_section_emission") is True, display_path(args.e001_summary), "M252-E002-E001-READY", "E001 summary must preserve section-emission readiness", failures)

    cases: list[dict[str, Any]] = []
    case_specs = (
        (run_positive_graph_case, args.class_graph_fixture, "class_protocol_property_ivar", "M252-E002-CASE-CLASS-GRAPH", "class-protocol-property-ivar-runtime-graph", {}),
        (run_positive_graph_case, args.category_graph_fixture, "category_protocol_property", "M252-E002-CASE-CATEGORY-GRAPH", "category-protocol-property-runtime-graph", {}),
        (run_positive_legality_case, args.class_synthesis_fixture, "class_property_synthesis_ready", "M252-E002-CASE-CLASS-SYNTH", "class-property-synthesis-ready", {"expected_synthesis_sites": 1, "expected_binding_resolved": 1}),
        (run_positive_legality_case, args.category_export_fixture, "category_property_export_only", "M252-E002-CASE-CATEGORY-EXPORT", "category-property-export-only", {"expected_synthesis_sites": 0, "expected_binding_resolved": 0}),
        (run_negative_legality_case, args.missing_interface_fixture, "missing_interface_property", "M252-E002-CASE-MISSING", "missing-interface-property-diagnostic", {}),
        (run_negative_legality_case, args.incompatible_fixture, "incompatible_property_signature", "M252-E002-CASE-INCOMPATIBLE", "incompatible-property-signature-diagnostic", {}),
    )

    for runner, fixture, directory_name, check_prefix, case_id, extra in case_specs:
        out_dir = args.probe_root / directory_name
        total, case_failures, case_payload = runner(
            runner_exe=args.runner_exe,
            fixture=fixture,
            out_dir=out_dir,
            check_prefix=check_prefix,
            case_id=case_id,
            **extra,
        )
        checks_total += total
        failures.extend(case_failures)
        if case_payload:
            cases.append(case_payload)

    positive_cases = sum(1 for case in cases if case.get("success") is True)
    negative_cases = sum(1 for case in cases if case.get("success") is False)
    checks_total += require(positive_cases == 4, "aggregate", "M252-E002-POSITIVE-COUNT", f"expected 4 positive corpus cases, observed {positive_cases}", failures)
    checks_total += require(negative_cases == 2, "aggregate", "M252-E002-NEGATIVE-COUNT", f"expected 2 negative corpus cases, observed {negative_cases}", failures)

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "dependency_summary": display_path(args.e001_summary),
        "dynamic_probes_executed": True,
        "corpus_cases": cases,
        "positive_case_count": positive_cases,
        "negative_case_count": negative_cases,
        "real_integrated_path": True,
    }

    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[FAIL] M252-E002 metadata-closure corpus sync drift detected; summary: {display_path(summary_out)}")
        return 1

    print(f"[PASS] M252-E002 metadata-closure corpus sync preserved; summary: {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
