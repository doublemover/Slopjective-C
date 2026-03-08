#!/usr/bin/env python3
"""Fail-closed contract checker for M252-B001 executable metadata semantic consistency freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-b001-executable-metadata-semantic-consistency-freeze-v1"
CONTRACT_ID = "objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1"
GRAPH_CONTRACT_ID = "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_executable_metadata_semantic_consistency_freeze_b001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_b001_executable_metadata_semantic_consistency_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "semantic-consistency-freeze"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-B001/executable_metadata_semantic_consistency_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M252-B001-DOC-EXP-01", "# M252 Executable Metadata Semantic Consistency Freeze Expectations (B001)"),
    SnippetCheck("M252-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-B001-DOC-EXP-03", "`Objc3ExecutableMetadataSemanticConsistencyBoundary`"),
    SnippetCheck("M252-B001-DOC-EXP-04", "does not yet admit lowering"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-B001-DOC-PKT-01", "# M252-B001 Executable Metadata Semantic Consistency Freeze Packet"),
    SnippetCheck("M252-B001-DOC-PKT-02", "Packet: `M252-B001`"),
    SnippetCheck("M252-B001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M252-B001-DOC-PKT-04", GRAPH_CONTRACT_ID),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-B001-ARCH-01", "M252 lane-B B001 executable metadata semantic consistency freeze anchors"),
    SnippetCheck("M252-B001-ARCH-02", "m252_executable_metadata_semantic_consistency_freeze_b001_expectations.md"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-B001-NDOC-01", "## Metadata semantic consistency freeze (M252-B001)"),
    SnippetCheck("M252-B001-NDOC-02", "`Objc3ExecutableMetadataSemanticConsistencyBoundary`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-B001-SPC-01", "## M252 executable metadata semantic consistency freeze (B001)"),
    SnippetCheck("M252-B001-SPC-02", "lowering_admission_ready == false"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-B001-META-01", "## M252 executable metadata semantic consistency metadata anchors (B001)"),
    SnippetCheck("M252-B001-META-02", "objc_executable_metadata_semantic_consistency_boundary"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-B001-PARSE-01", "M252-B001 freeze: the same canonical protocol: owner identities remain the"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-B001-SEMA-01", "M252-B001 freeze anchor: these counts remain the canonical semantic"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-B001-PASS-01", "M252-B001 freeze anchor: the same deterministic inventories remain the"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-B001-TYP-01", "kObjc3ExecutableMetadataSemanticConsistencyContractId"),
    SnippetCheck("M252-B001-TYP-02", "struct Objc3ExecutableMetadataSemanticConsistencyBoundary {"),
    SnippetCheck("M252-B001-TYP-03", "inline bool IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary("),
    SnippetCheck("M252-B001-TYP-04", "Objc3ExecutableMetadataSemanticConsistencyBoundary"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M252-B001-PIPE-01", "BuildExecutableMetadataSemanticConsistencyBoundary("),
    SnippetCheck("M252-B001-PIPE-02", "boundary.protocol_inheritance_edges_complete = true;"),
    SnippetCheck("M252-B001-PIPE-03", "metadata semantic consistency freeze is not fail-closed"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-B001-ART-01", "BuildExecutableMetadataSemanticConsistencyBoundaryJson("),
    SnippetCheck("M252-B001-ART-02", '\\"objc_executable_metadata_semantic_consistency_boundary\\"'),
    SnippetCheck("M252-B001-ART-03", '\\"semantic_boundary_frozen\\":'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-B001-PKG-01", '"check:objc3c:m252-b001-executable-metadata-semantic-consistency-freeze": "python scripts/check_m252_b001_executable_metadata_semantic_consistency_freeze.py"'),
    SnippetCheck("M252-B001-PKG-02", '"test:tooling:m252-b001-executable-metadata-semantic-consistency-freeze": "python -m pytest tests/tooling/test_check_m252_b001_executable_metadata_semantic_consistency_freeze.py -q"'),
    SnippetCheck("M252-B001-PKG-03", '"check:objc3c:m252-b001-lane-b-readiness": "npm run check:objc3c:m252-a003-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-b001-executable-metadata-semantic-consistency-freeze && npm run test:tooling:m252-b001-executable-metadata-semantic-consistency-freeze"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


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
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-runner-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def locate_boundary(payload: dict[str, object]) -> dict[str, object] | None:
    current = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_executable_metadata_semantic_consistency_boundary"):
        next_value = current.get(key)
        if not isinstance(next_value, dict):
            return None
        current = next_value
    return current


def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-B001-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(runner_exe), str(fixture_path), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"
    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-B001-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    if findings:
        return checks_total, findings, None
    summary_payload = json.loads(summary_path.read_text(encoding="utf-8"))
    manifest_rel = summary_payload.get("paths", {}).get("manifest")
    manifest_path = ROOT / manifest_rel if isinstance(manifest_rel, str) and not Path(manifest_rel).is_absolute() else Path(str(manifest_rel))
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-B001-MANIFEST-EXISTS", "runner manifest path is missing", findings)
    if findings and any(f.check_id == "M252-B001-MANIFEST-EXISTS" for f in findings):
        return checks_total, findings, None
    manifest_payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    boundary = locate_boundary(manifest_payload)
    checks_total += require(boundary is not None, display_path(manifest_path), "M252-B001-BOUNDARY-PATH", "missing semantic consistency boundary path", findings)
    if boundary is None:
        return checks_total, findings, None
    checks_total += require(summary_payload.get("status") == 0 and summary_payload.get("success") is True, display_path(manifest_path), "M252-B001-RUNNER-STATUS", "runner status must be 0 and success true", findings)
    checks_total += require(boundary.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-B001-CONTRACT-ID", "boundary contract id mismatch", findings)
    checks_total += require(boundary.get("executable_metadata_source_graph_contract_id") == GRAPH_CONTRACT_ID, display_path(manifest_path), "M252-B001-GRAPH-CONTRACT-ID", "graph contract id mismatch", findings)
    checks_total += require(boundary.get("semantic_boundary_frozen") is True, display_path(manifest_path), "M252-B001-FROZEN", "semantic_boundary_frozen must be true", findings)
    checks_total += require(boundary.get("fail_closed") is True, display_path(manifest_path), "M252-B001-FAIL-CLOSED", "fail_closed must be true", findings)
    checks_total += require(boundary.get("ready") is True, display_path(manifest_path), "M252-B001-READY", "ready must be true", findings)
    checks_total += require(boundary.get("lowering_admission_ready") is False, display_path(manifest_path), "M252-B001-LOWERING-READY", "lowering_admission_ready must remain false", findings)
    checks_total += require(boundary.get("semantic_conflict_diagnostics_enforcement_pending") is True, display_path(manifest_path), "M252-B001-PENDING-01", "semantic conflict diagnostics must remain pending", findings)
    checks_total += require(boundary.get("duplicate_export_owner_enforcement_pending") is True, display_path(manifest_path), "M252-B001-PENDING-02", "duplicate export-owner enforcement must remain pending", findings)
    checks_total += require(boundary.get("lowering_admission_pending") is True, display_path(manifest_path), "M252-B001-PENDING-03", "lowering admission must remain pending", findings)
    return checks_total, findings, {"fixture": display_path(fixture_path), "manifest_path": display_path(manifest_path), "boundary": boundary, "stdout": completed.stdout, "stderr": completed.stderr}


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M252-B001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-B001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-B001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-B001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-B001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-B001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-B001-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-B001-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-B001-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_types, "M252-B001-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M252-B001-PIPE-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M252-B001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, "M252-B001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    runner_cases: dict[str, object] = {}
    if not args.skip_runner_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(Finding(display_path(args.runner_exe), "M252-B001-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
        else:
            class_count, class_findings, class_case = run_runner_probe(runner_exe=args.runner_exe, fixture_path=args.class_fixture, out_dir=args.probe_root / "class-fixture")
            checks_total += class_count
            failures.extend(class_findings)
            if class_case is not None:
                runner_cases["class_fixture"] = class_case
                boundary = class_case["boundary"]
                checks_total += require(boundary.get("category_node_count") == 0, str(class_case["manifest_path"]), "M252-B001-CLASS-CATEGORY-COUNT", "class fixture must report zero category nodes", failures)
            category_count, category_findings, category_case = run_runner_probe(runner_exe=args.runner_exe, fixture_path=args.category_fixture, out_dir=args.probe_root / "category-fixture")
            checks_total += category_count
            failures.extend(category_findings)
            if category_case is not None:
                runner_cases["category_fixture"] = category_case
                boundary = category_case["boundary"]
                checks_total += require(boundary.get("category_node_count") == 1, str(category_case["manifest_path"]), "M252-B001-CATEGORY-NODE-COUNT", "category fixture must report one category node", failures)

    checks_passed = checks_total - len(failures)
    summary_payload = {"mode": MODE, "ok": not failures, "checks_total": checks_total, "checks_passed": checks_passed, "runner_probes_executed": not args.skip_runner_probes, "runner_cases": runner_cases, "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures]}
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
