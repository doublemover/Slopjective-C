#!/usr/bin/env python3
"""Fail-closed contract checker for M252-A002 executable metadata graph completeness."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-a002-interface-implementation-class-metaclass-graph-completeness-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m252_interface_implementation_class_metaclass_graph_completeness_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m252"
    / "m252_a002_interface_implementation_class_metaclass_graph_completeness_packet.md"
)
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
DEFAULT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_executable_metadata_graph_class_metaclass.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "executable-metadata-graph-completeness"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-A002/executable_metadata_graph_completeness_summary.json")


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
    SnippetCheck("M252-A002-DOC-EXP-01", "# M252 Interface Implementation Class Metaclass Graph Completeness Expectations (A002)"),
    SnippetCheck("M252-A002-DOC-EXP-02", "Contract ID: `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`"),
    SnippetCheck("M252-A002-DOC-EXP-03", "`Objc3ExecutableMetadataSourceGraph`"),
    SnippetCheck("M252-A002-DOC-EXP-04", "`interface_node_entries`"),
    SnippetCheck("M252-A002-DOC-EXP-05", "`owner_edges`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-DOC-PKT-01", "# M252-A002 Interface Implementation Class Metaclass Graph Completeness Packet"),
    SnippetCheck("M252-A002-DOC-PKT-02", "Packet: `M252-A002`"),
    SnippetCheck("M252-A002-DOC-PKT-03", "Dependencies: `M252-A001`"),
    SnippetCheck("M252-A002-DOC-PKT-04", "`class:Class`"),
    SnippetCheck("M252-A002-DOC-PKT-05", "`metaclass:Class`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-ARCH-01", "M252 lane-A A002 executable metadata graph completeness anchors explicit"),
    SnippetCheck("M252-A002-ARCH-02", "docs/contracts/m252_interface_implementation_class_metaclass_graph_completeness_a002_expectations.md"),
    SnippetCheck("M252-A002-ARCH-03", "first-class interface, implementation, class, and metaclass nodes plus"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-NDOC-01", "## Interface implementation class metaclass graph completeness (M252-A002)"),
    SnippetCheck("M252-A002-NDOC-02", "`Objc3ExecutableMetadataSourceGraph`"),
    SnippetCheck("M252-A002-NDOC-03", "`frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-SPC-01", "## M252 executable metadata graph completeness (A002)"),
    SnippetCheck("M252-A002-SPC-02", "`objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`"),
    SnippetCheck("M252-A002-SPC-03", "`lexicographic-kind-source-target`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-META-01", "## M252 executable metadata graph completeness metadata anchors (A002)"),
    SnippetCheck("M252-A002-META-02", "`interface_node_entries`"),
    SnippetCheck("M252-A002-META-03", "`owner_edges`"),
)

PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-PARSE-01", "M252-A002 completeness: interface/implementation semantic-link symbols stay"),
    SnippetCheck("M252-A002-PARSE-02", "static std::string BuildObjcCategorySemanticLinkSymbol("),
)

SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-SEMA-01", "M252-A002 completeness anchor: these deterministic counts validate the"),
    SnippetCheck("M252-A002-SEMA-02", "struct Objc3ClassProtocolCategoryLinkingSummary {"),
)

SEMA_PASSES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-PASS-01", "M252-A002 completeness anchor: sema preserves deterministic interface"),
    SnippetCheck("M252-A002-PASS-02", "interface_info.super_name = interface_decl.super_name;"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-TYP-01", "struct Objc3ExecutableMetadataInterfaceGraphNode {"),
    SnippetCheck("M252-A002-TYP-02", "struct Objc3ExecutableMetadataImplementationGraphNode {"),
    SnippetCheck("M252-A002-TYP-03", "struct Objc3ExecutableMetadataClassGraphNode {"),
    SnippetCheck("M252-A002-TYP-04", "struct Objc3ExecutableMetadataMetaclassGraphNode {"),
    SnippetCheck("M252-A002-TYP-05", "struct Objc3ExecutableMetadataGraphEdge {"),
    SnippetCheck("M252-A002-TYP-06", "struct Objc3ExecutableMetadataSourceGraph {"),
    SnippetCheck("M252-A002-TYP-07", "Objc3ExecutableMetadataSourceGraph executable_metadata_source_graph;"),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-PIP-01", "std::string BuildRuntimeClassOwnerIdentity(const std::string &class_name) {"),
    SnippetCheck("M252-A002-PIP-02", "std::string BuildRuntimeMetaclassOwnerIdentity(const std::string &class_name) {"),
    SnippetCheck("M252-A002-PIP-03", "Objc3ExecutableMetadataSourceGraph BuildExecutableMetadataSourceGraph("),
    SnippetCheck("M252-A002-PIP-04", 'add_owner_edge("class-to-metaclass", class_node.owner_identity,'),
    SnippetCheck("M252-A002-PIP-05", 'add_owner_edge("metaclass-to-super-metaclass",'),
    SnippetCheck("M252-A002-PIP-06", "graph.ready_for_semantic_closure = graph.source_graph_complete;"),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-ART-01", "BuildExecutableMetadataSourceGraphJson("),
    SnippetCheck("M252-A002-ART-02", '\\"interface_node_entries\\":['),
    SnippetCheck("M252-A002-ART-03", '\\"metaclass_node_entries\\":['),
    SnippetCheck("M252-A002-ART-04", '\\"owner_edges\\":['),
    SnippetCheck("M252-A002-ART-05", "const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph ="),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-PKG-01", '"check:objc3c:m252-a002-interface-implementation-class-metaclass-graph-completeness": "python scripts/check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py"'),
    SnippetCheck("M252-A002-PKG-02", '"test:tooling:m252-a002-interface-implementation-class-metaclass-graph-completeness": "python -m pytest tests/tooling/test_check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py -q"'),
    SnippetCheck("M252-A002-PKG-03", '"check:objc3c:m252-a002-lane-a-readiness": "npm run check:objc3c:m252-a001-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-a002-interface-implementation-class-metaclass-graph-completeness && npm run test:tooling:m252-a002-interface-implementation-class-metaclass-graph-completeness"'),
)

FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A002-FXT-01", "@interface Base"),
    SnippetCheck("M252-A002-FXT-02", "@implementation Base"),
    SnippetCheck("M252-A002-FXT-03", "@interface Widget : Base"),
    SnippetCheck("M252-A002-FXT-04", "+ (id) sharedWidget;"),
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
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
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


def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-A002-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(runner_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-A002-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    if findings:
        return checks_total, findings, None

    try:
        summary_payload = json.loads(summary_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(summary_path), "M252-A002-RUNNER-SUMMARY-JSON", f"failed to parse runner summary JSON: {exc}"))
        return checks_total + 1, findings, None

    for stage_name in ("lex", "parse", "sema", "lower"):
        checks_total += 1
        stage_payload = summary_payload.get("stages", {}).get(stage_name)
        if not isinstance(stage_payload, dict) or stage_payload.get("diagnostics_errors") != 0:
            findings.append(Finding(display_path(summary_path), f"M252-A002-STAGE-{stage_name.upper()}-CLEAN", f"expected {stage_name} diagnostics_errors == 0"))

    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-A002-RUNNER-SUCCESS", "runner summary success must be true", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-A002-RUNNER-STATUS", "runner status must be 0", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-A002-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)

    manifest_rel = summary_payload.get("paths", {}).get("manifest")
    manifest_path = ROOT / manifest_rel if isinstance(manifest_rel, str) and not Path(manifest_rel).is_absolute() else Path(str(manifest_rel))
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-A002-MANIFEST-EXISTS", "runner manifest path is missing", findings)
    if findings and any(f.check_id == "M252-A002-MANIFEST-EXISTS" for f in findings):
        return checks_total, findings, None

    try:
        manifest_payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(manifest_path), "M252-A002-MANIFEST-JSON", f"failed to parse manifest JSON: {exc}"))
        return checks_total + 1, findings, None

    graph = locate_graph(manifest_payload)
    checks_total += require(graph is not None, display_path(manifest_path), "M252-A002-GRAPH-PATH", "missing frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph", findings)
    if graph is None:
        return checks_total, findings, None

    checks_total += require(graph.get("contract_id") == "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1", display_path(manifest_path), "M252-A002-GRAPH-CONTRACT-ID", "graph contract id mismatch", findings)
    checks_total += require(graph.get("source_graph_complete") is True, display_path(manifest_path), "M252-A002-GRAPH-COMPLETE", "source_graph_complete must be true", findings)
    checks_total += require(graph.get("ready_for_semantic_closure") is True, display_path(manifest_path), "M252-A002-GRAPH-SEMANTIC-CLOSURE", "ready_for_semantic_closure must be true", findings)
    checks_total += require(graph.get("ready_for_lowering") is False, display_path(manifest_path), "M252-A002-GRAPH-LOWERING", "ready_for_lowering must remain false", findings)

    checks_total += require(len(manifest_payload.get("interfaces", [])) == 2, display_path(manifest_path), "M252-A002-INTERFACES-COUNT", "expected two top-level interface entries", findings)
    checks_total += require(len(manifest_payload.get("implementations", [])) == 2, display_path(manifest_path), "M252-A002-IMPLEMENTATIONS-COUNT", "expected two top-level implementation entries", findings)

    interface_nodes = graph.get("interface_node_entries")
    implementation_nodes = graph.get("implementation_node_entries")
    class_nodes = graph.get("class_node_entries")
    metaclass_nodes = graph.get("metaclass_node_entries")
    owner_edges = graph.get("owner_edges")

    for key, expected_len, check_id in (
        ("interface_node_entries", 2, "M252-A002-INTERFACE-NODES"),
        ("implementation_node_entries", 2, "M252-A002-IMPLEMENTATION-NODES"),
        ("class_node_entries", 2, "M252-A002-CLASS-NODES"),
        ("metaclass_node_entries", 2, "M252-A002-METACLASS-NODES"),
    ):
        value = graph.get(key)
        checks_total += 1
        if not isinstance(value, list) or len(value) != expected_len:
            findings.append(Finding(display_path(manifest_path), check_id, f"expected {key} length == {expected_len}"))

    checks_total += 1
    if not isinstance(owner_edges, list) or len(owner_edges) < 8:
        findings.append(Finding(display_path(manifest_path), "M252-A002-OWNER-EDGES", "expected at least eight owner edges"))

    if isinstance(interface_nodes, list):
        interface_ids = sorted(node.get("owner_identity") for node in interface_nodes if isinstance(node, dict))
        checks_total += require(interface_ids == ["interface:Base", "interface:Widget"], display_path(manifest_path), "M252-A002-INTERFACE-IDS", "unexpected interface owner identities", findings)
    if isinstance(implementation_nodes, list):
        implementation_ids = sorted(node.get("owner_identity") for node in implementation_nodes if isinstance(node, dict))
        checks_total += require(implementation_ids == ["implementation:Base", "implementation:Widget"], display_path(manifest_path), "M252-A002-IMPLEMENTATION-IDS", "unexpected implementation owner identities", findings)
    if isinstance(class_nodes, list):
        class_ids = sorted(node.get("owner_identity") for node in class_nodes if isinstance(node, dict))
        checks_total += require(class_ids == ["class:Base", "class:Widget"], display_path(manifest_path), "M252-A002-CLASS-IDS", "unexpected class owner identities", findings)
        widget_class = next((node for node in class_nodes if isinstance(node, dict) and node.get("class_name") == "Widget"), None)
        checks_total += require(isinstance(widget_class, dict) and widget_class.get("super_class_owner_identity") == "class:Base", display_path(manifest_path), "M252-A002-WIDGET-SUPER", "Widget class node must point at class:Base", findings)
    if isinstance(metaclass_nodes, list):
        metaclass_ids = sorted(node.get("owner_identity") for node in metaclass_nodes if isinstance(node, dict))
        checks_total += require(metaclass_ids == ["metaclass:Base", "metaclass:Widget"], display_path(manifest_path), "M252-A002-METACLASS-IDS", "unexpected metaclass owner identities", findings)
        widget_metaclass = next((node for node in metaclass_nodes if isinstance(node, dict) and node.get("class_name") == "Widget"), None)
        checks_total += require(isinstance(widget_metaclass, dict) and widget_metaclass.get("super_metaclass_owner_identity") == "metaclass:Base", display_path(manifest_path), "M252-A002-WIDGET-SUPER-METACLASS", "Widget metaclass node must point at metaclass:Base", findings)

    if isinstance(owner_edges, list):
        edge_set = {
            (edge.get("edge_kind"), edge.get("source_owner_identity"), edge.get("target_owner_identity"))
            for edge in owner_edges
            if isinstance(edge, dict)
        }
        for edge_kind, source, target, check_id in (
            ("interface-to-class", "interface:Base", "class:Base", "M252-A002-EDGE-01"),
            ("interface-to-class", "interface:Widget", "class:Widget", "M252-A002-EDGE-02"),
            ("implementation-to-class", "implementation:Base", "class:Base", "M252-A002-EDGE-03"),
            ("implementation-to-interface", "implementation:Widget", "interface:Widget", "M252-A002-EDGE-04"),
            ("class-to-metaclass", "class:Widget", "metaclass:Widget", "M252-A002-EDGE-05"),
            ("class-to-superclass", "class:Widget", "class:Base", "M252-A002-EDGE-06"),
            ("metaclass-to-super-metaclass", "metaclass:Widget", "metaclass:Base", "M252-A002-EDGE-07"),
        ):
            checks_total += require((edge_kind, source, target) in edge_set, display_path(manifest_path), check_id, f"missing expected edge {edge_kind}: {source} -> {target}", findings)

    case_payload = {
        "fixture": display_path(fixture_path),
        "out_dir": display_path(out_dir),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "status": summary_payload.get("status"),
        "process_exit_code": summary_payload.get("process_exit_code"),
        "success": summary_payload.get("success"),
        "graph": graph,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M252-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-A002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-A002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-A002-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-A002-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-A002-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_types, "M252-A002-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M252-A002-PIP-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M252-A002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, "M252-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
        (args.fixture, "M252-A002-FXT-EXISTS", FIXTURE_SNIPPETS),
    )

    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    runner_case: dict[str, object] | None = None
    if not args.skip_runner_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(Finding(display_path(args.runner_exe), "M252-A002-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
        else:
            count, findings, runner_case = run_runner_probe(
                runner_exe=args.runner_exe,
                fixture_path=args.fixture,
                out_dir=args.probe_root,
            )
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
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
        ],
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
