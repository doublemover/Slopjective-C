#!/usr/bin/env python3
"""Fail-closed checker for M256-A003 protocol/category source-surface completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-a003-protocol-category-source-surface-completion-v1"
CONTRACT_ID = "objc3c-executable-protocol-category-source-closure/m256-a003-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion_a003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion_packet.md"
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
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "protocol-category-source-surface"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-A003" / "protocol_category_source_surface_completion_for_executable_runtime_summary.json"


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
    SnippetCheck("M256-A003-DOC-EXP-01", "# M256 Protocol And Category Source Surface Completion For Executable Runtime Core Feature Expansion Expectations (A003)"),
    SnippetCheck("M256-A003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-A003-DOC-EXP-03", "`protocol_inheritance_identity_closure_complete`"),
    SnippetCheck("M256-A003-DOC-EXP-04", "`category_attachment_identity_complete`"),
    SnippetCheck("M256-A003-DOC-EXP-05", "`tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M256-A003-DOC-PKT-01", "# M256-A003 Protocol And Category Source Surface Completion For Executable Runtime Core Feature Expansion Packet"),
    SnippetCheck("M256-A003-DOC-PKT-02", "Packet: `M256-A003`"),
    SnippetCheck("M256-A003-DOC-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-A003-DOC-PKT-04", "`; executable_protocol_category_source_closure = ...`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M256-A003-NDOC-01", "## Protocol and category source-surface completion for executable runtime (M256-A003)"),
    SnippetCheck("M256-A003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-A003-NDOC-03", "`M256-B001`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M256-A003-SPC-01", "## M256 protocol/category source-surface completion for executable runtime (A003)"),
    SnippetCheck("M256-A003-SPC-02", "`category-declaration-owned-class-interface-implementation-attachment-identities`"),
    SnippetCheck("M256-A003-SPC-03", "`; executable_protocol_category_source_closure = ...`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M256-A003-META-01", "## M256 protocol/category source-surface completion metadata anchors (A003)"),
    SnippetCheck("M256-A003-META-02", "`protocol_category_source_closure_contract_id`"),
    SnippetCheck("M256-A003-META-03", "`conformance_identity_complete`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M256-A003-ARCH-01", "## M256 protocol/category source-surface completion for executable runtime (A003)"),
    SnippetCheck("M256-A003-ARCH-02", "protocol/category attachment and conformance edges must now be complete"),
    SnippetCheck("M256-A003-ARCH-03", "check:objc3c:m256-a003-lane-a-readiness"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M256-A003-PARSE-01", "M256-A003 protocol/category completion anchor"),
    SnippetCheck("M256-A003-PARSE-02", "feed fail-closed protocol inheritance and category attachment/conformance"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M256-A003-SEMA-01", "M256-A003 protocol/category completion anchor"),
    SnippetCheck("M256-A003-SEMA-02", "attachment and conformance closure consume one stable source model"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M256-A003-TYP-01", "kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId"),
    SnippetCheck("M256-A003-TYP-02", "protocol_category_source_closure_contract_id"),
    SnippetCheck("M256-A003-TYP-03", "protocol_inheritance_identity_closure_complete"),
    SnippetCheck("M256-A003-TYP-04", "category_attachment_identity_closure_complete"),
    SnippetCheck("M256-A003-TYP-05", "protocol_category_conformance_identity_closure_complete"),
    SnippetCheck("M256-A003-TYP-06", "inherited_protocol_identity_complete"),
    SnippetCheck("M256-A003-TYP-07", "attachment_identity_complete"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M256-A003-PIP-01", "node.inherited_protocol_identity_complete ="),
    SnippetCheck("M256-A003-PIP-02", "node.attachment_identity_complete ="),
    SnippetCheck("M256-A003-PIP-03", "node.conformance_identity_complete ="),
    SnippetCheck("M256-A003-PIP-04", "graph.protocol_category_declaration_closure_complete = true;"),
    SnippetCheck("M256-A003-PIP-05", 'has_graph_edge("category-to-protocol", node.owner_identity,'),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M256-A003-ART-01", "protocol_category_source_closure_contract_id"),
    SnippetCheck("M256-A003-ART-02", "inherited_protocol_identity_complete"),
    SnippetCheck("M256-A003-ART-03", "executable_protocol_category_source_closure_contract_id"),
    SnippetCheck("M256-A003-ART-04", "executable_protocol_category_conformance_identity_edge_count"),
)
IR_HEADER_SNIPPETS = (
    SnippetCheck("M256-A003-IRH-01", "executable_protocol_category_source_closure_contract_id"),
    SnippetCheck("M256-A003-IRH-02", "executable_protocol_inheritance_identity_edge_count"),
    SnippetCheck("M256-A003-IRH-03", "executable_protocol_category_conformance_identity_edge_count"),
)
IR_CPP_SNIPPETS = (
    SnippetCheck("M256-A003-IR-01", "M256-A003 executable protocol/category source-closure anchor"),
    SnippetCheck("M256-A003-IR-02", 'out << "; executable_protocol_category_source_closure = contract="'),
    SnippetCheck("M256-A003-IR-03", "executable_protocol_category_conformance_identity_edge_count"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M256-A003-PKG-01", '"check:objc3c:m256-a003-protocol-and-category-source-surface-completion-for-executable-runtime": "python scripts/check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py"'),
    SnippetCheck("M256-A003-PKG-02", '"test:tooling:m256-a003-protocol-and-category-source-surface-completion-for-executable-runtime": "python -m pytest tests/tooling/test_check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py -q"'),
    SnippetCheck("M256-A003-PKG-03", '"check:objc3c:m256-a003-lane-a-readiness": "npm run check:objc3c:m256-a002-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m256-a003-protocol-and-category-source-surface-completion-for-executable-runtime && npm run test:tooling:m256-a003-protocol-and-category-source-surface-completion-for-executable-runtime"'),
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
        failures.append(Finding(display_path(path), "M256-A003-MISSING", f"required artifact is missing: {display_path(path)}"))
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
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M256-A003-NATIVE-EXISTS", "objc3c-native.exe is missing; run npm run build:objc3c-native", failures)
    checks_total += 1
    checks_passed += require(args.fixture.exists(), display_path(args.fixture), "M256-A003-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures and any(f.check_id in {"M256-A003-NATIVE-EXISTS", "M256-A003-FIXTURE-EXISTS"} for f in failures):
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
    checks_passed += require(completed.returncode == 0, display_path(out_dir), "M256-A003-NATIVE-STATUS", f"native compile failed with exit code {completed.returncode}", failures)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    for path, check_id, detail in (
        (manifest_path, "M256-A003-MANIFEST", "module.manifest.json must exist"),
        (ir_path, "M256-A003-IR", "module.ll must exist"),
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
    checks_passed += require(graph is not None, display_path(manifest_path), "M256-A003-GRAPH", "missing frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph", failures)
    if graph is None:
        return checks_total, checks_passed, None

    for key, expected in (
        ("protocol_category_source_closure_contract_id", CONTRACT_ID),
        ("protocol_inheritance_identity_model", "protocol-declaration-owned-inherited-protocol-identities"),
        ("category_attachment_identity_model", "category-declaration-owned-class-interface-implementation-attachment-identities"),
        ("protocol_category_conformance_identity_model", "category-declaration-owned-adopted-protocol-conformance-identities"),
    ):
        checks_total += 1
        checks_passed += require(graph.get(key) == expected, display_path(manifest_path), f"M256-A003-GRAPH-{key.upper()}", f"unexpected {key}", failures)

    for key in (
        "protocol_category_declaration_closure_complete",
        "protocol_inheritance_identity_closure_complete",
        "category_attachment_identity_closure_complete",
        "protocol_category_conformance_identity_closure_complete",
        "source_graph_complete",
        "ready_for_semantic_closure",
    ):
        checks_total += 1
        checks_passed += require(graph.get(key) is True, display_path(manifest_path), f"M256-A003-GRAPH-{key.upper()}", f"expected {key} == true", failures)

    protocol_nodes = graph.get("protocol_node_entries")
    category_nodes = graph.get("category_node_entries")
    owner_edges = graph.get("owner_edges")
    checks_total += 3
    checks_passed += require(isinstance(protocol_nodes, list) and len(protocol_nodes) == 2, display_path(manifest_path), "M256-A003-PROTOCOL-NODES", "expected two protocol nodes", failures)
    checks_passed += require(isinstance(category_nodes, list) and len(category_nodes) == 1, display_path(manifest_path), "M256-A003-CATEGORY-NODES", "expected one category node", failures)
    checks_passed += require(isinstance(owner_edges, list), display_path(manifest_path), "M256-A003-OWNER-EDGES", "expected owner_edges list", failures)

    if isinstance(protocol_nodes, list):
        worker_protocol = next((node for node in protocol_nodes if isinstance(node, dict) and node.get("protocol_name") == "Worker"), None)
        base_protocol = next((node for node in protocol_nodes if isinstance(node, dict) and node.get("protocol_name") == "Base"), None)
        checks_total += 7
        checks_passed += require(isinstance(base_protocol, dict) and base_protocol.get("declaration_complete") is True, display_path(manifest_path), "M256-A003-BASE-DECLARATION", "Base protocol must mark declaration_complete", failures)
        checks_passed += require(isinstance(base_protocol, dict) and base_protocol.get("inherited_protocol_identity_complete") is True, display_path(manifest_path), "M256-A003-BASE-INHERITANCE", "Base protocol must mark inherited_protocol_identity_complete", failures)
        checks_passed += require(isinstance(worker_protocol, dict) and worker_protocol.get("owner_identity") == "protocol:Worker", display_path(manifest_path), "M256-A003-WORKER-OWNER", "Worker protocol must carry protocol:Worker", failures)
        checks_passed += require(isinstance(worker_protocol, dict) and worker_protocol.get("declaration_complete") is True, display_path(manifest_path), "M256-A003-WORKER-DECLARATION", "Worker protocol must mark declaration_complete", failures)
        checks_passed += require(isinstance(worker_protocol, dict) and worker_protocol.get("inherited_protocol_identity_complete") is True, display_path(manifest_path), "M256-A003-WORKER-INHERITANCE", "Worker protocol must mark inherited_protocol_identity_complete", failures)
        checks_passed += require(isinstance(worker_protocol, dict) and worker_protocol.get("inherited_protocol_owner_identities") == ["protocol:Base"], display_path(manifest_path), "M256-A003-WORKER-INHERITED-LIST", "Worker protocol must inherit protocol:Base", failures)
        checks_passed += require(isinstance(worker_protocol, dict) and worker_protocol.get("is_forward_declaration") is False, display_path(manifest_path), "M256-A003-WORKER-FORWARD", "Worker protocol must not be a forward declaration", failures)

    if isinstance(category_nodes, list):
        debug_category = next((node for node in category_nodes if isinstance(node, dict) and node.get("class_name") == "Widget" and node.get("category_name") == "Debug"), None)
        checks_total += 8
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("owner_identity") == "category:Widget(Debug)", display_path(manifest_path), "M256-A003-CATEGORY-OWNER", "Widget(Debug) must carry canonical category owner identity", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("class_owner_identity") == "class:Widget", display_path(manifest_path), "M256-A003-CATEGORY-CLASS", "Widget(Debug) must attach to class:Widget", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("interface_owner_identity") == "interface:Widget(Debug)", display_path(manifest_path), "M256-A003-CATEGORY-INTERFACE", "Widget(Debug) must preserve interface owner identity", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("implementation_owner_identity") == "implementation:Widget(Debug)", display_path(manifest_path), "M256-A003-CATEGORY-IMPLEMENTATION", "Widget(Debug) must preserve implementation owner identity", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("declaration_complete") is True, display_path(manifest_path), "M256-A003-CATEGORY-DECLARATION", "Widget(Debug) must mark declaration_complete", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("attachment_identity_complete") is True, display_path(manifest_path), "M256-A003-CATEGORY-ATTACHMENT", "Widget(Debug) must mark attachment_identity_complete", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("conformance_identity_complete") is True, display_path(manifest_path), "M256-A003-CATEGORY-CONFORMANCE", "Widget(Debug) must mark conformance_identity_complete", failures)
        checks_passed += require(isinstance(debug_category, dict) and debug_category.get("adopted_protocol_owner_identities") == ["protocol:Worker"], display_path(manifest_path), "M256-A003-CATEGORY-ADOPTED", "Widget(Debug) must adopt protocol:Worker", failures)

    if isinstance(owner_edges, list):
        edge_set = {
            (edge.get("edge_kind"), edge.get("source_owner_identity"), edge.get("target_owner_identity"))
            for edge in owner_edges
            if isinstance(edge, dict)
        }
        expected_edges = (
            ("protocol-to-inherited-protocol", "protocol:Worker", "protocol:Base", "M256-A003-EDGE-01"),
            ("category-to-class", "category:Widget(Debug)", "class:Widget", "M256-A003-EDGE-02"),
            ("category-to-interface", "category:Widget(Debug)", "interface:Widget(Debug)", "M256-A003-EDGE-03"),
            ("category-to-implementation", "category:Widget(Debug)", "implementation:Widget(Debug)", "M256-A003-EDGE-04"),
            ("category-to-protocol", "category:Widget(Debug)", "protocol:Worker", "M256-A003-EDGE-05"),
        )
        checks_total += len(expected_edges)
        for edge_kind, source, target, check_id in expected_edges:
            checks_passed += require((edge_kind, source, target) in edge_set, display_path(manifest_path), check_id, f"missing expected edge {edge_kind}: {source} -> {target}", failures)

    ir_text = ir_path.read_text(encoding="utf-8")
    expected_ir_snippets = (
        "; executable_protocol_category_source_closure = contract=objc3c-executable-protocol-category-source-closure/m256-a003-v1",
        "protocol_node_count=2",
        "category_node_count=1",
        "protocol_inheritance_edge_count=1",
        "category_attachment_edge_count=3",
        "protocol_category_conformance_edge_count=1",
    )
    checks_total += len(expected_ir_snippets)
    for index, snippet in enumerate(expected_ir_snippets, start=1):
        checks_passed += require(snippet in ir_text, display_path(ir_path), f"M256-A003-IR-SUMMARY-{index:02d}", f"missing IR summary snippet: {snippet}", failures)

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
