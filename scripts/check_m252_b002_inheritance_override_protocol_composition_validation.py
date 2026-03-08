#!/usr/bin/env python3
"""Fail-closed contract checker for M252-B002 executable metadata semantic validation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-b002-inheritance-override-protocol-composition-validation-v1"
CONTRACT_ID = "objc3c-executable-metadata-semantic-validation/m252-b002-v1"
SEMANTIC_CONSISTENCY_CONTRACT_ID = "objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_inheritance_override_protocol_composition_validation_b002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_b002_inheritance_override_protocol_composition_validation_packet.md"
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
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_executable_metadata_semantic_validation.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "semantic-validation"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-B002/executable_metadata_semantic_validation_summary.json")


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
    SnippetCheck("M252-B002-DOC-EXP-01", "# M252 Inheritance Override Protocol Composition Validation Expectations (B002)"),
    SnippetCheck("M252-B002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-B002-DOC-EXP-03", "`Objc3ExecutableMetadataSemanticValidationSurface`"),
    SnippetCheck("M252-B002-DOC-EXP-04", "`method-to-overridden-method`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-B002-DOC-PKT-01", "# M252-B002 Inheritance Override Protocol Composition Validation Packet"),
    SnippetCheck("M252-B002-DOC-PKT-02", "Packet: `M252-B002`"),
    SnippetCheck("M252-B002-DOC-PKT-03", "- `M252-B001`"),
    SnippetCheck("M252-B002-DOC-PKT-04", "- `M252-A002`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-B002-ARCH-01", "M252 lane-B B002 executable metadata semantic validation anchors explicit"),
    SnippetCheck("M252-B002-ARCH-02", "method-to-overridden-method"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-B002-NDOC-01", "## Inheritance override protocol composition validation (M252-B002)"),
    SnippetCheck("M252-B002-NDOC-02", "`Objc3ExecutableMetadataSemanticValidationSurface`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-B002-SPC-01", "## M252 inheritance override protocol composition validation (B002)"),
    SnippetCheck("M252-B002-SPC-02", "`method-to-overridden-method`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-B002-META-01", "## M252 inheritance override protocol composition validation metadata anchors (B002)"),
    SnippetCheck("M252-B002-META-02", "objc_executable_metadata_semantic_validation_surface"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-B002-PARSE-01", "M252-B002 anchor: interface superclass names plus the canonical"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-B002-SEMA-01", "M252-B002 anchor: lane-B executable metadata semantic validation consumes"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-B002-PASS-01", "M252-B002 anchor: protocol-composition site accounting stays deterministic"),
    SnippetCheck("M252-B002-PASS-02", "M252-B002 anchor: this superclass walk and signature check remain the"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-B002-TYP-01", "kObjc3ExecutableMetadataSemanticValidationContractId"),
    SnippetCheck("M252-B002-TYP-02", "struct Objc3ExecutableMetadataSemanticValidationSurface {"),
    SnippetCheck("M252-B002-TYP-03", "bool class_inheritance_edges_complete = false;"),
    SnippetCheck("M252-B002-TYP-04", "inline bool IsReadyObjc3ExecutableMetadataSemanticValidationSurface("),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M252-B002-PIPE-01", 'add_owner_edge("method-to-overridden-method", method_node.owner_identity,'),
    SnippetCheck("M252-B002-PIPE-02", "BuildExecutableMetadataSemanticValidationSurface("),
    SnippetCheck("M252-B002-PIPE-03", 'surface.failure_reason = "override validation is incomplete";'),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-B002-ART-01", "BuildExecutableMetadataSemanticValidationSurfaceJson("),
    SnippetCheck("M252-B002-ART-02", "objc_executable_metadata_semantic_validation_surface"),
    SnippetCheck("M252-B002-ART-03", "override_edge_count"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B002-FIX-01", "@protocol Worker<BaseWorker>"),
    SnippetCheck("M252-B002-FIX-02", "@interface Widget : Root"),
    SnippetCheck("M252-B002-FIX-03", "- (id<Worker>) currentWorker;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-B002-PKG-01", '"check:objc3c:m252-b002-inheritance-override-protocol-composition-validation": "python scripts/check_m252_b002_inheritance_override_protocol_composition_validation.py"'),
    SnippetCheck("M252-B002-PKG-02", '"test:tooling:m252-b002-inheritance-override-protocol-composition-validation": "python -m pytest tests/tooling/test_check_m252_b002_inheritance_override_protocol_composition_validation.py -q"'),
    SnippetCheck("M252-B002-PKG-03", '"check:objc3c:m252-b002-lane-b-readiness": "npm run check:objc3c:m252-b001-lane-b-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-b002-inheritance-override-protocol-composition-validation && npm run test:tooling:m252-b002-inheritance-override-protocol-composition-validation"'),
)


EXPECTED_SURFACE_COUNTS = {
    "class_inheritance_edge_count": 1,
    "protocol_inheritance_edge_count": 1,
    "metaclass_super_edge_count": 1,
    "override_edge_count": 2,
    "class_method_override_edge_count": 1,
    "instance_method_override_edge_count": 1,
    "override_lookup_sites": 3,
    "override_lookup_hits": 2,
    "override_lookup_misses": 1,
    "override_conflicts": 0,
    "unresolved_base_interfaces": 0,
    "protocol_composition_sites": 2,
    "protocol_composition_symbols": 2,
    "invalid_protocol_composition_sites": 0,
}


EXPECTED_EDGE_SET = {
    ("class-to-superclass", "class:Widget", "class:Root"),
    ("protocol-to-inherited-protocol", "protocol:Worker", "protocol:BaseWorker"),
    ("metaclass-to-super-metaclass", "metaclass:Widget", "metaclass:Root"),
    ("method-to-overridden-method", "interface:Widget::class_method:shared", "interface:Root::class_method:shared"),
    ("method-to-overridden-method", "interface:Widget::instance_method:token", "interface:Root::instance_method:token"),
}


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
    parser.add_argument("--sema-contract", dest="sema_contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
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


def locate_graph(payload: dict[str, object]) -> dict[str, object] | None:
    current = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_executable_metadata_source_graph"):
        next_value = current.get(key)
        if not isinstance(next_value, dict):
            return None
        current = next_value
    return current


def locate_surface(payload: dict[str, object]) -> dict[str, object] | None:
    current = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_executable_metadata_semantic_validation_surface"):
        next_value = current.get(key)
        if not isinstance(next_value, dict):
            return None
        current = next_value
    return current


def edge_tuples(owner_edges: object) -> set[tuple[str | None, str | None, str | None]]:
    if not isinstance(owner_edges, list):
        return set()
    return {
        (
            edge.get("edge_kind") if isinstance(edge, dict) else None,
            edge.get("source_owner_identity") if isinstance(edge, dict) else None,
            edge.get("target_owner_identity") if isinstance(edge, dict) else None,
        )
        for edge in owner_edges
    }


def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-B002-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(runner_exe), str(fixture_path), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"
    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-B002-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    if findings:
        return checks_total, findings, None
    summary_payload = json.loads(summary_path.read_text(encoding="utf-8"))
    manifest_rel = summary_payload.get("paths", {}).get("manifest")
    manifest_path = ROOT / manifest_rel if isinstance(manifest_rel, str) and not Path(manifest_rel).is_absolute() else Path(str(manifest_rel))
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-B002-RUNNER-SUCCESS", "runner summary success must be true", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-B002-RUNNER-STATUS", "runner status must be 0", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-B002-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-B002-MANIFEST-EXISTS", "runner manifest path is missing", findings)
    if findings and any(f.check_id == "M252-B002-MANIFEST-EXISTS" for f in findings):
        return checks_total, findings, None
    manifest_payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    graph = locate_graph(manifest_payload)
    surface = locate_surface(manifest_payload)
    checks_total += require(graph is not None, display_path(manifest_path), "M252-B002-GRAPH-PATH", "missing executable metadata source graph", findings)
    checks_total += require(surface is not None, display_path(manifest_path), "M252-B002-SURFACE-PATH", "missing executable metadata semantic validation surface", findings)
    if graph is None or surface is None:
        return checks_total, findings, None

    checks_total += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-B002-CONTRACT-ID", "validation surface contract id mismatch", findings)
    checks_total += require(surface.get("executable_metadata_semantic_consistency_contract_id") == SEMANTIC_CONSISTENCY_CONTRACT_ID, display_path(manifest_path), "M252-B002-DEPENDENCY-CONTRACT-ID", "semantic consistency dependency contract id mismatch", findings)
    for key in (
        "semantic_consistency_ready",
        "ready",
        "method_lookup_override_conflict_handoff_deterministic",
        "class_protocol_category_linking_deterministic",
        "class_inheritance_edges_complete",
        "protocol_inheritance_edges_complete",
        "metaclass_edges_complete",
        "inheritance_chain_cycle_free",
        "superclass_targets_resolved",
        "protocol_inheritance_targets_resolved",
        "metaclass_targets_resolved",
        "metaclass_lineage_aligned",
        "method_override_edges_complete",
        "override_lookup_complete",
        "override_conflicts_absent",
        "protocol_composition_valid",
        "inheritance_validation_ready",
        "override_validation_ready",
        "protocol_composition_validation_ready",
        "metaclass_relationship_validation_ready",
        "semantic_validation_complete",
        "fail_closed",
    ):
        checks_total += require(surface.get(key) is True, display_path(manifest_path), f"M252-B002-{key.upper()}", f"{key} must be true", findings)
    checks_total += require(surface.get("lowering_admission_ready") is False, display_path(manifest_path), "M252-B002-LOWERING-READY", "lowering_admission_ready must remain false", findings)

    for key, expected in EXPECTED_SURFACE_COUNTS.items():
        checks_total += require(surface.get(key) == expected, display_path(manifest_path), f"M252-B002-{key.upper()}", f"{key} mismatch: expected {expected}", findings)

    checks_total += require(graph.get("class_nodes") == 2, display_path(manifest_path), "M252-B002-CLASS-NODES", "expected two class nodes", findings)
    checks_total += require(graph.get("metaclass_nodes") == 2, display_path(manifest_path), "M252-B002-METACLASS-NODES", "expected two metaclass nodes", findings)
    checks_total += require(graph.get("protocol_nodes") == 2, display_path(manifest_path), "M252-B002-PROTOCOL-NODES", "expected two protocol nodes", findings)

    edges = edge_tuples(graph.get("owner_edges"))
    for edge_kind, source, target in EXPECTED_EDGE_SET:
        checks_total += require((edge_kind, source, target) in edges, display_path(manifest_path), f"M252-B002-{edge_kind.upper()}", f"missing expected edge {edge_kind}: {source} -> {target}", findings)

    return checks_total, findings, {
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "surface": surface,
        "override_edges": sorted(
            [list(edge) for edge in edges if edge[0] == "method-to-overridden-method"]
        ),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M252-B002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-B002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-B002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-B002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-B002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-B002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-B002-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-B002-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-B002-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_types, "M252-B002-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M252-B002-PIPE-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M252-B002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.fixture, "M252-B002-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.package_json, "M252-B002-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    runner_case: dict[str, object] | None = None
    if not args.skip_runner_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(Finding(display_path(args.runner_exe), "M252-B002-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
        else:
            count, findings, runner_case = run_runner_probe(runner_exe=args.runner_exe, fixture_path=args.fixture, out_dir=args.probe_root / "happy-path")
            checks_total += count
            failures.extend(findings)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runner_probes_executed": not args.skip_runner_probes,
        "runner_case": runner_case,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }
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
