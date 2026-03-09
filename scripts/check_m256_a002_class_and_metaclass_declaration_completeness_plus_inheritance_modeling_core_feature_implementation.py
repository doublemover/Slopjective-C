#!/usr/bin/env python3
"""Fail-closed checker for M256-A002 class/metaclass declaration completeness."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-a002-class-metaclass-declaration-completeness-v1"
CONTRACT_ID = "objc3c-executable-class-metaclass-source-closure/m256-a002-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation_a002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_executable_metadata_graph_class_metaclass.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "class-metaclass-declaration-completeness"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-A002" / "class_metaclass_declaration_completeness_and_inheritance_modeling_summary.json"


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
    SnippetCheck("M256-A002-DOC-EXP-01", "# M256 Class And Metaclass Declaration Completeness Plus Inheritance Modeling Core Feature Implementation Expectations (A002)"),
    SnippetCheck("M256-A002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-A002-DOC-EXP-03", "`class_metaclass_parent_identity_closure_complete`"),
    SnippetCheck("M256-A002-DOC-EXP-04", "`interface-to-class-method-owner`"),
    SnippetCheck("M256-A002-DOC-EXP-05", "`tmp/reports/m256/M256-A002/class_metaclass_declaration_completeness_and_inheritance_modeling_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M256-A002-DOC-PKT-01", "# M256-A002 Class And Metaclass Declaration Completeness Plus Inheritance Modeling Core Feature Implementation Packet"),
    SnippetCheck("M256-A002-DOC-PKT-02", "Packet: `M256-A002`"),
    SnippetCheck("M256-A002-DOC-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-A002-DOC-PKT-04", "`; executable_class_metaclass_source_closure = ...`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M256-A002-NDOC-01", "## Class/metaclass declaration completeness plus inheritance modeling (M256-A002)"),
    SnippetCheck("M256-A002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-A002-NDOC-03", "`M256-A003`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M256-A002-SPC-01", "## M256 class/metaclass declaration completeness plus inheritance modeling (A002)"),
    SnippetCheck("M256-A002-SPC-02", "`declaration-owned-class-parent-plus-metaclass-parent-identities`"),
    SnippetCheck("M256-A002-SPC-03", "`; executable_class_metaclass_source_closure = ...`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M256-A002-META-01", "## M256 class/metaclass declaration completeness metadata anchors (A002)"),
    SnippetCheck("M256-A002-META-02", "`class_metaclass_source_closure_contract_id`"),
    SnippetCheck("M256-A002-META-03", "`realization_identity_complete`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M256-A002-ARCH-01", "## M256 class/metaclass declaration completeness plus inheritance modeling (A002)"),
    SnippetCheck("M256-A002-ARCH-02", "declaration-owned class/metaclass object identities"),
    SnippetCheck("M256-A002-ARCH-03", "check:objc3c:m256-a002-lane-a-readiness"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M256-A002-PARSE-01", "M256-A002 class/metaclass completion anchor"),
    SnippetCheck("M256-A002-PARSE-02", "stable runtime method-owner identities"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M256-A002-SEMA-01", "M256-A002 class/metaclass completion anchor"),
    SnippetCheck("M256-A002-SEMA-02", "declaration-owned class/metaclass parent identities and method-owner split"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M256-A002-TYP-01", "kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId"),
    SnippetCheck("M256-A002-TYP-02", "class_metaclass_source_closure_contract_id"),
    SnippetCheck("M256-A002-TYP-03", "class_metaclass_parent_identity_closure_complete"),
    SnippetCheck("M256-A002-TYP-04", "super_metaclass_owner_identity"),
    SnippetCheck("M256-A002-TYP-05", "instance_method_owner_identity"),
    SnippetCheck("M256-A002-TYP-06", "class_method_owner_identity"),
    SnippetCheck("M256-A002-TYP-07", "realization_identity_complete"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M256-A002-PIP-01", 'add_owner_edge("interface-to-metaclass"'),
    SnippetCheck("M256-A002-PIP-02", 'add_owner_edge("implementation-to-super-metaclass"'),
    SnippetCheck("M256-A002-PIP-03", 'add_owner_edge("implementation-to-class-method-owner"'),
    SnippetCheck("M256-A002-PIP-04", "graph.class_metaclass_parent_identity_closure_complete = true;"),
    SnippetCheck("M256-A002-PIP-05", "graph.class_metaclass_method_owner_identity_closure_complete = true;"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M256-A002-ART-01", "class_metaclass_source_closure_contract_id"),
    SnippetCheck("M256-A002-ART-02", "super_metaclass_owner_identity"),
    SnippetCheck("M256-A002-ART-03", "executable_class_metaclass_parent_identity_edge_count"),
    SnippetCheck("M256-A002-ART-04", "bundle.class_owner_identity"),
)
IR_HEADER_SNIPPETS = (
    SnippetCheck("M256-A002-IRH-01", "executable_class_metaclass_source_closure_contract_id"),
    SnippetCheck("M256-A002-IRH-02", "executable_class_metaclass_parent_identity_edge_count"),
    SnippetCheck("M256-A002-IRH-03", "class_owner_identity;"),
)
IR_CPP_SNIPPETS = (
    SnippetCheck("M256-A002-IR-01", "M256-A002 executable class/metaclass source-closure anchor"),
    SnippetCheck("M256-A002-IR-02", 'out << "; executable_class_metaclass_source_closure = contract="'),
    SnippetCheck("M256-A002-IR-03", "executable_class_metaclass_method_owner_identity_edge_count"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M256-A002-PKG-01", '"check:objc3c:m256-a002-class-and-metaclass-declaration-completeness-plus-inheritance-modeling": "python scripts/check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py"'),
    SnippetCheck("M256-A002-PKG-02", '"test:tooling:m256-a002-class-and-metaclass-declaration-completeness-plus-inheritance-modeling": "python -m pytest tests/tooling/test_check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py -q"'),
    SnippetCheck("M256-A002-PKG-03", '"check:objc3c:m256-a002-lane-a-readiness": "npm run check:objc3c:m256-a001-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m256-a002-class-and-metaclass-declaration-completeness-plus-inheritance-modeling && npm run test:tooling:m256-a002-class-and-metaclass-declaration-completeness-plus-inheritance-modeling"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-header", type=Path, default=DEFAULT_IR_HEADER)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> tuple[int, int]:
    checks_total = len(snippets)
    if not path.exists():
        failures.append(Finding(display_path(path), "M256-A002-MISSING", f"required artifact is missing: {display_path(path)}"))
        return checks_total, 0
    text = path.read_text(encoding="utf-8")
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return checks_total, passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def locate_graph(payload: dict[str, object]) -> dict[str, object] | None:
    current: object = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_executable_metadata_source_graph"):
        if not isinstance(current, dict):
            return None
        current = current.get(key)
    return current if isinstance(current, dict) else None


def run_command(command: list[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_dynamic_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object] | None]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root / "happy_path"
    out_dir.mkdir(parents=True, exist_ok=True)

    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M256-A002-NATIVE-EXISTS", "objc3c-native.exe is missing; run npm run build:objc3c-native", failures)
    checks_total += 1
    checks_passed += require(args.fixture.exists(), display_path(args.fixture), "M256-A002-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures and any(f.check_id in {"M256-A002-NATIVE-EXISTS", "M256-A002-FIXTURE-EXISTS"} for f in failures):
        return checks_total, checks_passed, None

    completed = run_command([
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
        "--out-dir",
        str(out_dir.resolve()),
        "--emit-prefix",
        "module",
    ], ROOT)
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(out_dir), "M256-A002-NATIVE-STATUS", f"native compile failed with exit code {completed.returncode}", failures)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    for path, check_id, detail in (
        (manifest_path, "M256-A002-MANIFEST", "module.manifest.json must exist"),
        (ir_path, "M256-A002-IR", "module.ll must exist"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if not manifest_path.exists() or not ir_path.exists() or completed.returncode != 0:
        return checks_total, checks_passed, {
            "fixture": display_path(args.fixture),
            "out_dir": display_path(out_dir),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    manifest_payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    graph = locate_graph(manifest_payload)
    checks_total += 1
    checks_passed += require(graph is not None, display_path(manifest_path), "M256-A002-GRAPH", "missing frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph", failures)
    if graph is None:
        return checks_total, checks_passed, None

    for key, expected in (
        ("class_metaclass_source_closure_contract_id", CONTRACT_ID),
        ("class_metaclass_parent_identity_model", "declaration-owned-class-parent-plus-metaclass-parent-identities"),
        ("class_metaclass_method_owner_identity_model", "declaration-owned-instance-class-method-owner-identities"),
        ("class_metaclass_object_identity_model", "declaration-owned-class-and-metaclass-object-identities"),
    ):
        checks_total += 1
        checks_passed += require(graph.get(key) == expected, display_path(manifest_path), f"M256-A002-GRAPH-{key.upper()}", f"unexpected {key}", failures)

    for key in (
        "class_metaclass_declaration_closure_complete",
        "class_metaclass_parent_identity_closure_complete",
        "class_metaclass_method_owner_identity_closure_complete",
        "class_metaclass_object_identity_closure_complete",
        "source_graph_complete",
        "ready_for_semantic_closure",
    ):
        checks_total += 1
        checks_passed += require(graph.get(key) is True, display_path(manifest_path), f"M256-A002-GRAPH-{key.upper()}", f"expected {key} == true", failures)

    interface_nodes = graph.get("interface_node_entries")
    implementation_nodes = graph.get("implementation_node_entries")
    class_nodes = graph.get("class_node_entries")
    owner_edges = graph.get("owner_edges")
    checks_total += 4
    checks_passed += require(isinstance(interface_nodes, list) and len(interface_nodes) == 2, display_path(manifest_path), "M256-A002-INTERFACE-NODES", "expected two interface nodes", failures)
    checks_passed += require(isinstance(implementation_nodes, list) and len(implementation_nodes) == 2, display_path(manifest_path), "M256-A002-IMPLEMENTATION-NODES", "expected two implementation nodes", failures)
    checks_passed += require(isinstance(class_nodes, list) and len(class_nodes) == 2, display_path(manifest_path), "M256-A002-CLASS-NODES", "expected two class nodes", failures)
    checks_passed += require(isinstance(owner_edges, list), display_path(manifest_path), "M256-A002-OWNER-EDGES", "expected owner_edges list", failures)

    if isinstance(interface_nodes, list):
        widget_interface = next((node for node in interface_nodes if isinstance(node, dict) and node.get("class_name") == "Widget"), None)
        base_interface = next((node for node in interface_nodes if isinstance(node, dict) and node.get("class_name") == "Base"), None)
        checks_total += 8
        checks_passed += require(isinstance(base_interface, dict) and base_interface.get("instance_method_owner_identity") == "class:Base", display_path(manifest_path), "M256-A002-BASE-INTERFACE-INSTANCE-OWNER", "Base interface must point instance methods at class:Base", failures)
        checks_passed += require(isinstance(base_interface, dict) and base_interface.get("class_method_owner_identity") == "metaclass:Base", display_path(manifest_path), "M256-A002-BASE-INTERFACE-CLASS-OWNER", "Base interface must point class methods at metaclass:Base", failures)
        checks_passed += require(isinstance(widget_interface, dict) and widget_interface.get("class_owner_identity") == "class:Widget", display_path(manifest_path), "M256-A002-WIDGET-INTERFACE-CLASS", "Widget interface must carry class:Widget", failures)
        checks_passed += require(isinstance(widget_interface, dict) and widget_interface.get("metaclass_owner_identity") == "metaclass:Widget", display_path(manifest_path), "M256-A002-WIDGET-INTERFACE-METACLASS", "Widget interface must carry metaclass:Widget", failures)
        checks_passed += require(isinstance(widget_interface, dict) and widget_interface.get("super_class_owner_identity") == "class:Base", display_path(manifest_path), "M256-A002-WIDGET-INTERFACE-SUPERCLASS", "Widget interface must carry class:Base", failures)
        checks_passed += require(isinstance(widget_interface, dict) and widget_interface.get("super_metaclass_owner_identity") == "metaclass:Base", display_path(manifest_path), "M256-A002-WIDGET-INTERFACE-SUPER-METACLASS", "Widget interface must carry metaclass:Base", failures)
        checks_passed += require(isinstance(widget_interface, dict) and widget_interface.get("instance_method_owner_identity") == "class:Widget", display_path(manifest_path), "M256-A002-WIDGET-INTERFACE-INSTANCE-OWNER", "Widget interface instance methods must target class:Widget", failures)
        checks_passed += require(isinstance(widget_interface, dict) and widget_interface.get("class_method_owner_identity") == "metaclass:Widget", display_path(manifest_path), "M256-A002-WIDGET-INTERFACE-CLASS-OWNER", "Widget interface class methods must target metaclass:Widget", failures)

    if isinstance(implementation_nodes, list):
        widget_impl = next((node for node in implementation_nodes if isinstance(node, dict) and node.get("class_name") == "Widget"), None)
        checks_total += 6
        checks_passed += require(isinstance(widget_impl, dict) and widget_impl.get("class_owner_identity") == "class:Widget", display_path(manifest_path), "M256-A002-WIDGET-IMPL-CLASS", "Widget implementation must carry class:Widget", failures)
        checks_passed += require(isinstance(widget_impl, dict) and widget_impl.get("metaclass_owner_identity") == "metaclass:Widget", display_path(manifest_path), "M256-A002-WIDGET-IMPL-METACLASS", "Widget implementation must carry metaclass:Widget", failures)
        checks_passed += require(isinstance(widget_impl, dict) and widget_impl.get("super_class_owner_identity") == "class:Base", display_path(manifest_path), "M256-A002-WIDGET-IMPL-SUPERCLASS", "Widget implementation must carry class:Base", failures)
        checks_passed += require(isinstance(widget_impl, dict) and widget_impl.get("super_metaclass_owner_identity") == "metaclass:Base", display_path(manifest_path), "M256-A002-WIDGET-IMPL-SUPER-METACLASS", "Widget implementation must carry metaclass:Base", failures)
        checks_passed += require(isinstance(widget_impl, dict) and widget_impl.get("instance_method_owner_identity") == "class:Widget", display_path(manifest_path), "M256-A002-WIDGET-IMPL-INSTANCE-OWNER", "Widget implementation instance methods must target class:Widget", failures)
        checks_passed += require(isinstance(widget_impl, dict) and widget_impl.get("class_method_owner_identity") == "metaclass:Widget", display_path(manifest_path), "M256-A002-WIDGET-IMPL-CLASS-OWNER", "Widget implementation class methods must target metaclass:Widget", failures)

    if isinstance(class_nodes, list):
        widget_class = next((node for node in class_nodes if isinstance(node, dict) and node.get("class_name") == "Widget"), None)
        checks_total += 4
        checks_passed += require(isinstance(widget_class, dict) and widget_class.get("super_metaclass_owner_identity") == "metaclass:Base", display_path(manifest_path), "M256-A002-WIDGET-CLASS-SUPER-METACLASS", "Widget class must carry metaclass:Base", failures)
        checks_passed += require(isinstance(widget_class, dict) and widget_class.get("instance_method_owner_identity") == "class:Widget", display_path(manifest_path), "M256-A002-WIDGET-CLASS-INSTANCE-OWNER", "Widget class must carry class:Widget as instance owner", failures)
        checks_passed += require(isinstance(widget_class, dict) and widget_class.get("class_method_owner_identity") == "metaclass:Widget", display_path(manifest_path), "M256-A002-WIDGET-CLASS-CLASS-OWNER", "Widget class must carry metaclass:Widget as class owner", failures)
        checks_passed += require(isinstance(widget_class, dict) and widget_class.get("realization_identity_complete") is True, display_path(manifest_path), "M256-A002-WIDGET-CLASS-REALIZATION", "Widget class must mark realization_identity_complete", failures)

    if isinstance(owner_edges, list):
        edge_set = {
            (edge.get("edge_kind"), edge.get("source_owner_identity"), edge.get("target_owner_identity"))
            for edge in owner_edges
            if isinstance(edge, dict)
        }
        expected_edges = (
            ("interface-to-metaclass", "interface:Widget", "metaclass:Widget", "M256-A002-EDGE-01"),
            ("interface-to-super-metaclass", "interface:Widget", "metaclass:Base", "M256-A002-EDGE-02"),
            ("interface-to-class-method-owner", "interface:Widget", "metaclass:Widget", "M256-A002-EDGE-03"),
            ("implementation-to-metaclass", "implementation:Widget", "metaclass:Widget", "M256-A002-EDGE-04"),
            ("implementation-to-superclass", "implementation:Widget", "class:Base", "M256-A002-EDGE-05"),
            ("implementation-to-super-metaclass", "implementation:Widget", "metaclass:Base", "M256-A002-EDGE-06"),
            ("implementation-to-instance-method-owner", "implementation:Widget", "class:Widget", "M256-A002-EDGE-07"),
            ("implementation-to-class-method-owner", "implementation:Widget", "metaclass:Widget", "M256-A002-EDGE-08"),
        )
        checks_total += len(expected_edges)
        for edge_kind, source, target, check_id in expected_edges:
            checks_passed += require((edge_kind, source, target) in edge_set, display_path(manifest_path), check_id, f"missing expected edge {edge_kind}: {source} -> {target}", failures)

    ir_text = ir_path.read_text(encoding="utf-8")
    expected_ir_snippets = (
        "; executable_class_metaclass_source_closure = contract=objc3c-executable-class-metaclass-source-closure/m256-a002-v1",
        "declaration_node_count=8",
        "parent_identity_edge_count=6",
        "method_owner_identity_edge_count=8",
        "class_object_identity_edge_count=10",
    )
    checks_total += len(expected_ir_snippets)
    for index, snippet in enumerate(expected_ir_snippets, start=1):
        checks_passed += require(snippet in ir_text, display_path(ir_path), f"M256-A002-IR-SUMMARY-{index:02d}", f"missing IR summary snippet: {snippet}", failures)

    probe_payload = {
        "fixture": display_path(args.fixture),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, checks_passed, probe_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_checks = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_header, IR_HEADER_SNIPPETS),
        (args.ir_cpp, IR_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_checks:
        total, passed = ensure_snippets(path, snippets, failures)
        checks_total += total
        checks_passed += passed

    dynamic_probe: dict[str, object] | None = None
    if not args.skip_dynamic_probes:
        dynamic_total, dynamic_passed, dynamic_probe = run_dynamic_probe(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
        "dynamic_probe": dynamic_probe,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"summary_path: {display_path(args.summary_out)}")
    print("status: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
