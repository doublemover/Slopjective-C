#!/usr/bin/env python3
"""Fail-closed contract checker for M252-A003 executable metadata export graph completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-a003-protocol-category-property-ivar-export-graph-completion-v1"
GRAPH_CONTRACT_ID = "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m252_protocol_category_property_ivar_export_graph_completion_a003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m252"
    / "m252_a003_protocol_category_property_ivar_export_graph_completion_packet.md"
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
DEFAULT_CLASS_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_CATEGORY_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "export-graph-completion"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-A003/executable_metadata_export_graph_completion_summary.json")


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
    SnippetCheck("M252-A003-DOC-EXP-01", "# M252 Protocol Category Property Ivar Export Graph Completion Expectations (A003)"),
    SnippetCheck("M252-A003-DOC-EXP-02", "`protocol_node_entries`"),
    SnippetCheck("M252-A003-DOC-EXP-03", "`category_node_entries`"),
    SnippetCheck("M252-A003-DOC-EXP-04", "`property_node_entries`"),
    SnippetCheck("M252-A003-DOC-EXP-05", "`ivar_node_entries`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-DOC-PKT-01", "# M252-A003 Protocol Category Property Ivar Export Graph Completion Packet"),
    SnippetCheck("M252-A003-DOC-PKT-02", "Packet: `M252-A003`"),
    SnippetCheck("M252-A003-DOC-PKT-03", "Dependencies: `M252-A002`"),
    SnippetCheck("M252-A003-DOC-PKT-04", "`category:Class(Category)`"),
    SnippetCheck("M252-A003-DOC-PKT-05", GRAPH_CONTRACT_ID),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-ARCH-01", "M252 lane-A A003 executable metadata export graph completion anchors explicit"),
    SnippetCheck("M252-A003-ARCH-02", "docs/contracts/m252_protocol_category_property_ivar_export_graph_completion_a003_expectations.md"),
    SnippetCheck("M252-A003-ARCH-03", "protocol/category/property/method/ivar node packets"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-NDOC-01", "## Protocol category property ivar graph completion (M252-A003)"),
    SnippetCheck("M252-A003-NDOC-02", "`protocol_node_entries`"),
    SnippetCheck("M252-A003-NDOC-03", "`category_node_entries`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-SPC-01", "## M252 executable metadata export graph completion (A003)"),
    SnippetCheck("M252-A003-SPC-02", "canonical category node owners on `category:Class(Category)`"),
    SnippetCheck("M252-A003-SPC-03", "ready_for_lowering == false"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-META-01", "## M252 executable metadata export graph completion metadata anchors (A003)"),
    SnippetCheck("M252-A003-META-02", "`protocol_node_entries`"),
    SnippetCheck("M252-A003-META-03", "`category_node_entries`"),
)

PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-PARSE-01", "M252-A003 completion: protocol inheritance/adoption targets remain on the"),
    SnippetCheck("M252-A003-PARSE-02", "M252-A003 completion: category export packets keep category:Class(Category)"),
)

SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-SEMA-01", "M252-A003 completion anchor: protocol/category composition counts stay"),
)

SEMA_PASSES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-PASS-01", "M252-A003 completion anchor: sema must preserve protocol/category/member"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-TYP-01", "struct Objc3ExecutableMetadataProtocolGraphNode {"),
    SnippetCheck("M252-A003-TYP-02", "struct Objc3ExecutableMetadataCategoryGraphNode {"),
    SnippetCheck("M252-A003-TYP-03", "struct Objc3ExecutableMetadataPropertyGraphNode {"),
    SnippetCheck("M252-A003-TYP-04", "struct Objc3ExecutableMetadataMethodGraphNode {"),
    SnippetCheck("M252-A003-TYP-05", "struct Objc3ExecutableMetadataIvarGraphNode {"),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-PIP-01", "std::string BuildRuntimeCategoryOwnerIdentity(const std::string &class_name,"),
    SnippetCheck("M252-A003-PIP-02", 'add_owner_edge("protocol-to-inherited-protocol", node.owner_identity,'),
    SnippetCheck("M252-A003-PIP-03", 'add_owner_edge("category-to-class", node.owner_identity,'),
    SnippetCheck("M252-A003-PIP-04", 'add_owner_edge("property-to-export-owner", node.owner_identity,'),
    SnippetCheck("M252-A003-PIP-05", 'add_owner_edge("ivar-to-property", ivar_node.owner_identity,'),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-ART-01", '\\"protocol_node_entries\\":['),
    SnippetCheck("M252-A003-ART-02", '\\"category_node_entries\\":['),
    SnippetCheck("M252-A003-ART-03", '\\"property_node_entries\\":['),
    SnippetCheck("M252-A003-ART-04", '\\"method_node_entries\\":['),
    SnippetCheck("M252-A003-ART-05", '\\"ivar_node_entries\\":['),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-PKG-01", '"check:objc3c:m252-a003-protocol-category-property-ivar-export-graph-completion": "python scripts/check_m252_a003_protocol_category_property_ivar_export_graph_completion.py"'),
    SnippetCheck("M252-A003-PKG-02", '"test:tooling:m252-a003-protocol-category-property-ivar-export-graph-completion": "python -m pytest tests/tooling/test_check_m252_a003_protocol_category_property_ivar_export_graph_completion.py -q"'),
    SnippetCheck("M252-A003-PKG-03", '"check:objc3c:m252-a003-lane-a-readiness": "npm run check:objc3c:m252-a002-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-a003-protocol-category-property-ivar-export-graph-completion && npm run test:tooling:m252-a003-protocol-category-property-ivar-export-graph-completion"'),
)

CLASS_FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-CFX-01", "@protocol Worker<Base>"),
    SnippetCheck("M252-A003-CFX-02", "@interface Widget<Worker>"),
    SnippetCheck("M252-A003-CFX-03", "setter=setCurrentValue:"),
)

CATEGORY_FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-A003-GFX-01", "@interface Widget (Debug)<Worker>"),
    SnippetCheck("M252-A003-GFX-02", "@implementation Widget (Debug)"),
    SnippetCheck("M252-A003-GFX-03", "getter=tokenValue"),
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


def locate_graph(payload: dict[str, object]) -> dict[str, object] | None:
    current = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_executable_metadata_source_graph"):
        next_value = current.get(key)
        if not isinstance(next_value, dict):
            return None
        current = next_value
    return current


def edge_tuples(owner_edges: object) -> set[tuple[str | None, str | None, str | None]]:
    if not isinstance(owner_edges, list):
        return set()
    return {
        (edge.get("edge_kind"), edge.get("source_owner_identity"), edge.get("target_owner_identity"))
        for edge in owner_edges
        if isinstance(edge, dict)
    }


def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-A003-FIXTURE-EXISTS", "fixture is missing", findings)
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

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-A003-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    if findings:
        return checks_total, findings, None

    try:
        summary_payload = json.loads(summary_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(summary_path), "M252-A003-RUNNER-SUMMARY-JSON", f"failed to parse runner summary JSON: {exc}"))
        return checks_total + 1, findings, None

    for stage_name in ("lex", "parse", "sema", "lower"):
        checks_total += 1
        stage_payload = summary_payload.get("stages", {}).get(stage_name)
        if not isinstance(stage_payload, dict) or stage_payload.get("diagnostics_errors") != 0:
            findings.append(Finding(display_path(summary_path), f"M252-A003-STAGE-{stage_name.upper()}-CLEAN", f"expected {stage_name} diagnostics_errors == 0"))

    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-A003-RUNNER-SUCCESS", "runner summary success must be true", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-A003-RUNNER-STATUS", "runner status must be 0", findings)

    manifest_rel = summary_payload.get("paths", {}).get("manifest")
    manifest_path = ROOT / manifest_rel if isinstance(manifest_rel, str) and not Path(manifest_rel).is_absolute() else Path(str(manifest_rel))
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-A003-MANIFEST-EXISTS", "runner manifest path is missing", findings)
    if findings and any(f.check_id == "M252-A003-MANIFEST-EXISTS" for f in findings):
        return checks_total, findings, None

    try:
        manifest_payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(manifest_path), "M252-A003-MANIFEST-JSON", f"failed to parse manifest JSON: {exc}"))
        return checks_total + 1, findings, None

    graph = locate_graph(manifest_payload)
    checks_total += require(graph is not None, display_path(manifest_path), "M252-A003-GRAPH-PATH", "missing frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph", findings)
    if graph is None:
        return checks_total, findings, None

    checks_total += require(graph.get("contract_id") == GRAPH_CONTRACT_ID, display_path(manifest_path), "M252-A003-GRAPH-CONTRACT-ID", "graph contract id mismatch", findings)
    checks_total += require(graph.get("source_graph_complete") is True, display_path(manifest_path), "M252-A003-GRAPH-COMPLETE", "source_graph_complete must be true", findings)
    checks_total += require(graph.get("ready_for_semantic_closure") is True, display_path(manifest_path), "M252-A003-GRAPH-SEMANTIC-CLOSURE", "ready_for_semantic_closure must be true", findings)
    checks_total += require(graph.get("ready_for_lowering") is False, display_path(manifest_path), "M252-A003-GRAPH-LOWERING", "ready_for_lowering must remain false", findings)

    for key in (
        "protocol_node_entries",
        "category_node_entries",
        "property_node_entries",
        "method_node_entries",
        "ivar_node_entries",
        "owner_edges",
    ):
        checks_total += 1
        if not isinstance(graph.get(key), list):
            findings.append(Finding(display_path(manifest_path), f"M252-A003-{key.upper()}-LIST", f"expected {key} to be a list"))

    case_payload = {
        "fixture": display_path(fixture_path),
        "out_dir": display_path(out_dir),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "graph": graph,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def validate_class_fixture(case: dict[str, object], failures: list[Finding]) -> int:
    checks_total = 0
    artifact = str(case["manifest_path"])
    graph = case["graph"]
    assert isinstance(graph, dict)

    checks_total += require(graph.get("protocol_nodes") == 2, artifact, "M252-A003-CLASS-PROTOCOL-COUNT", "expected two protocol nodes on class fixture", failures)
    checks_total += require(graph.get("category_nodes") == 0, artifact, "M252-A003-CLASS-CATEGORY-COUNT", "expected zero category nodes on class fixture", failures)
    checks_total += require(graph.get("property_nodes") == 3, artifact, "M252-A003-CLASS-PROPERTY-COUNT", "expected three property nodes on class fixture", failures)
    checks_total += require(graph.get("method_nodes") == 5, artifact, "M252-A003-CLASS-METHOD-COUNT", "expected five method nodes on class fixture", failures)
    checks_total += require(graph.get("ivar_nodes") == 1, artifact, "M252-A003-CLASS-IVAR-COUNT", "expected one ivar node on class fixture", failures)

    protocol_entries = graph.get("protocol_node_entries")
    property_entries = graph.get("property_node_entries")
    method_entries = graph.get("method_node_entries")
    ivar_entries = graph.get("ivar_node_entries")
    owner_edges = graph.get("owner_edges")

    checks_total += require(isinstance(protocol_entries, list) and len(protocol_entries) == 2, artifact, "M252-A003-CLASS-PROTOCOL-ENTRIES", "expected two protocol entries on class fixture", failures)
    checks_total += require(isinstance(property_entries, list) and len(property_entries) == 3, artifact, "M252-A003-CLASS-PROPERTY-ENTRIES", "expected three property entries on class fixture", failures)
    checks_total += require(isinstance(method_entries, list) and len(method_entries) == 5, artifact, "M252-A003-CLASS-METHOD-ENTRIES", "expected five method entries on class fixture", failures)
    checks_total += require(isinstance(ivar_entries, list) and len(ivar_entries) == 1, artifact, "M252-A003-CLASS-IVAR-ENTRIES", "expected one ivar entry on class fixture", failures)

    if isinstance(protocol_entries, list):
        worker = next((entry for entry in protocol_entries if isinstance(entry, dict) and entry.get("protocol_name") == "Worker"), None)
        checks_total += require(isinstance(worker, dict) and worker.get("inherited_protocol_owner_identities") == ["protocol:Base"], artifact, "M252-A003-CLASS-PROTOCOL-INHERITANCE", "Worker must inherit protocol:Base", failures)

    if isinstance(property_entries, list):
        widget_interface_property = next((entry for entry in property_entries if isinstance(entry, dict) and entry.get("owner_identity") == "interface:Widget::property:value"), None)
        checks_total += require(isinstance(widget_interface_property, dict) and widget_interface_property.get("export_owner_identity") == "class:Widget", artifact, "M252-A003-CLASS-PROPERTY-EXPORT", "Widget interface property must export to class:Widget", failures)

    if isinstance(ivar_entries, list):
        ivar_entry = ivar_entries[0] if ivar_entries else None
        checks_total += require(isinstance(ivar_entry, dict) and ivar_entry.get("property_owner_identity") == "interface:Widget::property:value", artifact, "M252-A003-CLASS-IVAR-PROPERTY", "class ivar must reference interface:Widget::property:value", failures)

    edges = edge_tuples(owner_edges)
    checks_total += require(("protocol-to-inherited-protocol", "protocol:Worker", "protocol:Base") in edges, artifact, "M252-A003-CLASS-EDGE-01", "missing protocol inheritance edge", failures)
    checks_total += require(("method-to-export-owner", "implementation:Widget::class_method:shared", "metaclass:Widget") in edges, artifact, "M252-A003-CLASS-EDGE-02", "missing class-method export edge", failures)
    checks_total += require(("method-to-export-owner", "implementation:Widget::instance_method:currentValue", "class:Widget") in edges, artifact, "M252-A003-CLASS-EDGE-03", "missing instance-method export edge", failures)
    checks_total += require(("property-to-getter-method", "interface:Widget::property:value", "interface:Widget::instance_method:currentValue") in edges, artifact, "M252-A003-CLASS-EDGE-04", "missing property-to-getter edge", failures)
    checks_total += require(("property-to-ivar", "interface:Widget::property:value", "interface:Widget::ivar_binding:_value") in edges, artifact, "M252-A003-CLASS-EDGE-05", "missing property-to-ivar edge", failures)
    checks_total += require(("ivar-to-property", "interface:Widget::ivar_binding:_value", "interface:Widget::property:value") in edges, artifact, "M252-A003-CLASS-EDGE-06", "missing ivar-to-property edge", failures)
    return checks_total


def validate_category_fixture(case: dict[str, object], failures: list[Finding]) -> int:
    checks_total = 0
    artifact = str(case["manifest_path"])
    graph = case["graph"]
    assert isinstance(graph, dict)

    checks_total += require(graph.get("protocol_nodes") == 2, artifact, "M252-A003-CAT-PROTOCOL-COUNT", "expected two protocol nodes on category fixture", failures)
    checks_total += require(graph.get("category_nodes") == 1, artifact, "M252-A003-CAT-CATEGORY-COUNT", "expected one category node on category fixture", failures)
    checks_total += require(graph.get("property_nodes") == 3, artifact, "M252-A003-CAT-PROPERTY-COUNT", "expected three property nodes on category fixture", failures)
    checks_total += require(graph.get("method_nodes") == 3, artifact, "M252-A003-CAT-METHOD-COUNT", "expected three method nodes on category fixture", failures)
    checks_total += require(graph.get("ivar_nodes") == 1, artifact, "M252-A003-CAT-IVAR-COUNT", "expected one ivar node on category fixture", failures)

    category_entries = graph.get("category_node_entries")
    property_entries = graph.get("property_node_entries")
    owner_edges = graph.get("owner_edges")

    checks_total += require(isinstance(category_entries, list) and len(category_entries) == 1, artifact, "M252-A003-CAT-CATEGORY-ENTRIES", "expected one category entry on category fixture", failures)
    checks_total += require(isinstance(property_entries, list) and len(property_entries) == 3, artifact, "M252-A003-CAT-PROPERTY-ENTRIES", "expected three property entries on category fixture", failures)

    if isinstance(category_entries, list):
        entry = category_entries[0] if category_entries else None
        checks_total += require(isinstance(entry, dict) and entry.get("owner_identity") == "category:Widget(Debug)", artifact, "M252-A003-CAT-CATEGORY-OWNER", "category owner identity mismatch", failures)
        checks_total += require(isinstance(entry, dict) and entry.get("adopted_protocol_owner_identities") == ["protocol:Worker"], artifact, "M252-A003-CAT-CATEGORY-PROTOCOL", "category adopted protocol identity mismatch", failures)

    if isinstance(property_entries, list):
        shadow_property = next((entry for entry in property_entries if isinstance(entry, dict) and entry.get("owner_identity") == "interface:Widget(Debug)::property:shadow"), None)
        checks_total += require(isinstance(shadow_property, dict) and shadow_property.get("export_owner_identity") == "category:Widget(Debug)", artifact, "M252-A003-CAT-PROPERTY-EXPORT", "category property must export to category:Widget(Debug)", failures)

    edges = edge_tuples(owner_edges)
    checks_total += require(("category-to-class", "category:Widget(Debug)", "class:Widget") in edges, artifact, "M252-A003-CAT-EDGE-01", "missing category-to-class edge", failures)
    checks_total += require(("category-to-interface", "category:Widget(Debug)", "interface:Widget(Debug)") in edges, artifact, "M252-A003-CAT-EDGE-02", "missing category-to-interface edge", failures)
    checks_total += require(("category-to-implementation", "category:Widget(Debug)", "implementation:Widget(Debug)") in edges, artifact, "M252-A003-CAT-EDGE-03", "missing category-to-implementation edge", failures)
    checks_total += require(("category-to-protocol", "category:Widget(Debug)", "protocol:Worker") in edges, artifact, "M252-A003-CAT-EDGE-04", "missing category-to-protocol edge", failures)
    checks_total += require(("property-to-getter-method", "protocol:Worker::property:token", "protocol:Worker::instance_method:tokenValue") in edges, artifact, "M252-A003-CAT-EDGE-05", "missing protocol property getter edge on category fixture", failures)
    checks_total += require(("property-to-ivar", "interface:Widget(Debug)::property:shadow", "interface:Widget(Debug)::ivar_binding:_shadow") in edges, artifact, "M252-A003-CAT-EDGE-06", "missing category property-to-ivar edge", failures)
    return checks_total


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M252-A003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-A003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-A003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-A003-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-A003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-A003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-A003-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-A003-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-A003-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_types, "M252-A003-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M252-A003-PIP-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M252-A003-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, "M252-A003-PKG-EXISTS", PACKAGE_SNIPPETS),
        (args.class_fixture, "M252-A003-CFX-EXISTS", CLASS_FIXTURE_SNIPPETS),
        (args.category_fixture, "M252-A003-GFX-EXISTS", CATEGORY_FIXTURE_SNIPPETS),
    )

    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    runner_cases: dict[str, object] = {}
    if not args.skip_runner_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(Finding(display_path(args.runner_exe), "M252-A003-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
        else:
            class_count, class_findings, class_case = run_runner_probe(
                runner_exe=args.runner_exe,
                fixture_path=args.class_fixture,
                out_dir=args.probe_root / "class-probe",
            )
            checks_total += class_count
            failures.extend(class_findings)
            if class_case is not None:
                runner_cases["class_fixture"] = class_case
                checks_total += validate_class_fixture(class_case, failures)

            category_count, category_findings, category_case = run_runner_probe(
                runner_exe=args.runner_exe,
                fixture_path=args.category_fixture,
                out_dir=args.probe_root / "category-probe",
            )
            checks_total += category_count
            failures.extend(category_findings)
            if category_case is not None:
                runner_cases["category_fixture"] = category_case
                checks_total += validate_category_fixture(category_case, failures)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runner_probes_executed": not args.skip_runner_probes,
        "runner_cases": runner_cases,
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
