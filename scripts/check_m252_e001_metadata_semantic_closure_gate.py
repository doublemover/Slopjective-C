#!/usr/bin/env python3
"""Fail-closed checker for M252-E001 metadata semantic-closure gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-e001-metadata-semantic-closure-gate-v1"
CONTRACT_ID = "objc3c-executable-metadata-semantic-closure-gate/m252-e001-v1"
DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m252_metadata_semantic_closure_gate_contract_and_architecture_freeze_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m252"
    / "m252_e001_metadata_semantic_closure_gate_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m252" / "M252-A003" / "executable_metadata_export_graph_completion_summary.json"
)
DEFAULT_B004_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m252"
    / "M252-B004"
    / "property_ivar_export_legality_synthesis_preconditions_summary.json"
)
DEFAULT_C003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m252" / "M252-C003" / "metadata_debug_projection_and_replay_anchors_summary.json"
)
DEFAULT_D002_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m252"
    / "M252-D002"
    / "artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json"
)
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json")


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
    SnippetCheck(
        "M252-E001-DOC-EXP-01",
        "# M252 Metadata Semantic-Closure Gate Contract And Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck("M252-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck(
        "M252-E001-DOC-EXP-03",
        "`tmp/reports/m252/M252-A003/executable_metadata_export_graph_completion_summary.json`",
    ),
    SnippetCheck(
        "M252-E001-DOC-EXP-04",
        "`tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json`",
    ),
    SnippetCheck(
        "M252-E001-DOC-EXP-05",
        "`tmp/reports/m252/M252-C003/metadata_debug_projection_and_replay_anchors_summary.json`",
    ),
    SnippetCheck(
        "M252-E001-DOC-EXP-06",
        "`tmp/reports/m252/M252-D002/artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json`",
    ),
    SnippetCheck("M252-E001-DOC-EXP-07", "`M253-A001`"),
    SnippetCheck(
        "M252-E001-DOC-EXP-08",
        "`check:objc3c:m252-e001-metadata-semantic-closure-gate`",
    ),
    SnippetCheck("M252-E001-DOC-EXP-09", "`check:objc3c:m252-e001-lane-e-readiness`"),
    SnippetCheck(
        "M252-E001-DOC-EXP-10",
        "`tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json`",
    ),
)
PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-DOC-PKT-01",
        "# M252-E001 Metadata Semantic-Closure Gate Contract And Architecture Freeze Packet",
    ),
    SnippetCheck("M252-E001-DOC-PKT-02", "Packet: `M252-E001`"),
    SnippetCheck("M252-E001-DOC-PKT-03", "Issue: `#7083`"),
    SnippetCheck("M252-E001-DOC-PKT-04", "- none"),
    SnippetCheck("M252-E001-DOC-PKT-05", "`M252-A003`"),
    SnippetCheck("M252-E001-DOC-PKT-06", "`M252-B004`"),
    SnippetCheck("M252-E001-DOC-PKT-07", "`M252-C003`"),
    SnippetCheck("M252-E001-DOC-PKT-08", "`M252-D002`"),
    SnippetCheck("M252-E001-DOC-PKT-09", "`M253-A001`"),
    SnippetCheck(
        "M252-E001-DOC-PKT-10",
        "`scripts/check_m252_e001_metadata_semantic_closure_gate.py`",
    ),
    SnippetCheck(
        "M252-E001-DOC-PKT-11",
        "`tests/tooling/test_check_m252_e001_metadata_semantic_closure_gate.py`",
    ),
)
ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-ARCH-01",
        "M252 lane-E E001 metadata semantic-closure gate anchors explicit aggregate",
    ),
    SnippetCheck("M252-E001-ARCH-02", "`M252-A003`, `M252-B004`, `M252-C003`, and `M252-D002`"),
    SnippetCheck("M252-E001-ARCH-03", "`M253-A001`"),
)
NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E001-NDOC-01", "## Metadata semantic-closure gate (M252-E001)"),
    SnippetCheck("M252-E001-NDOC-02", "`Objc3ExecutableMetadataSemanticClosureGateSummary`"),
    SnippetCheck("M252-E001-NDOC-03", "`module.runtime-metadata.bin`"),
    SnippetCheck("M252-E001-NDOC-04", "`M253-A001`"),
)
LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E001-SPC-01", "## M252 metadata semantic-closure gate (E001)"),
    SnippetCheck("M252-E001-SPC-02", "`Objc3ExecutableMetadataSemanticClosureGateSummary`"),
    SnippetCheck("M252-E001-SPC-03", f"`{CONTRACT_ID}`"),
    SnippetCheck("M252-E001-SPC-04", "`M253-A001` section emission"),
)
METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M252-E001-META-01", "## M252 metadata semantic-closure gate metadata anchors (E001)"),
    SnippetCheck("M252-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M252-E001-META-03",
        "`tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json`",
    ),
)
PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-PARSE-01",
        "M252-E001 semantic-closure gate anchor: lane-E freezes this parser-owned",
    ),
)
SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-SEMA-01",
        "M252-E001 semantic-closure gate anchor: lane-E consumes this property/ivar",
    ),
)
SEMA_PASSES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-PASS-01",
        "M252-E001 semantic-closure gate anchor: lane-E treats this sema-owned",
    ),
)
FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-ART-01",
        "M252-E001 semantic-closure gate anchor: lane-E freezes the",
    ),
)
PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M252-E001-PKG-01",
        '"check:objc3c:m252-e001-metadata-semantic-closure-gate": "python scripts/check_m252_e001_metadata_semantic_closure_gate.py"',
    ),
    SnippetCheck(
        "M252-E001-PKG-02",
        '"test:tooling:m252-e001-metadata-semantic-closure-gate": "python -m pytest tests/tooling/test_check_m252_e001_metadata_semantic_closure_gate.py -q"',
    ),
    SnippetCheck(
        "M252-E001-PKG-03",
        '"check:objc3c:m252-e001-lane-e-readiness": "npm run check:objc3c:m252-a003-lane-a-readiness && npm run check:objc3c:m252-b004-lane-b-readiness && npm run check:objc3c:m252-c003-lane-c-readiness && npm run check:objc3c:m252-d002-lane-d-readiness && npm run check:objc3c:m252-e001-metadata-semantic-closure-gate && npm run test:tooling:m252-e001-metadata-semantic-closure-gate"',
    ),
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
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a003-summary", type=Path, default=DEFAULT_A003_SUMMARY)
    parser.add_argument("--b004-summary", type=Path, default=DEFAULT_B004_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d002-summary", type=Path, default=DEFAULT_D002_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
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


def load_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def validate_a003(path: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(path.exists(), display_path(path), "M252-E001-A003-EXISTS", "A003 summary is missing", failures)
    if failures:
        return checks_total, failures, {}

    payload = load_json_object(path)
    checks_total += require(payload.get("ok") is True, display_path(path), "M252-E001-A003-OK", "A003 summary must report ok: true", failures)
    checks_total += require(payload.get("runner_probes_executed") is True, display_path(path), "M252-E001-A003-PROBES", "A003 runner probes must be executed", failures)
    runner_cases = payload.get("runner_cases")
    checks_total += require(isinstance(runner_cases, dict), display_path(path), "M252-E001-A003-CASES", "A003 runner_cases payload must be an object", failures)
    if failures:
        return checks_total, failures, {}

    class_case = runner_cases.get("class_fixture")
    category_case = runner_cases.get("category_fixture")
    checks_total += require(isinstance(class_case, dict), display_path(path), "M252-E001-A003-CLASS-CASE", "A003 class_fixture case is missing", failures)
    checks_total += require(isinstance(category_case, dict), display_path(path), "M252-E001-A003-CATEGORY-CASE", "A003 category_fixture case is missing", failures)
    if failures:
        return checks_total, failures, {}

    class_graph = class_case.get("graph")
    category_graph = category_case.get("graph")
    checks_total += require(isinstance(class_graph, dict), display_path(path), "M252-E001-A003-CLASS-GRAPH", "A003 class fixture graph is missing", failures)
    checks_total += require(isinstance(category_graph, dict), display_path(path), "M252-E001-A003-CATEGORY-GRAPH", "A003 category fixture graph is missing", failures)
    if failures:
        return checks_total, failures, {}

    checks_total += require(class_graph.get("source_graph_complete") is True, display_path(path), "M252-E001-A003-CLASS-COMPLETE", "A003 class fixture must keep source_graph_complete == true", failures)
    checks_total += require(class_graph.get("ready_for_lowering") is False, display_path(path), "M252-E001-A003-CLASS-NOT-LOWERING", "A003 class fixture must remain not-ready-for-lowering in the lane-A-only proof", failures)
    checks_total += require(class_graph.get("protocol_nodes", 0) >= 2, display_path(path), "M252-E001-A003-CLASS-PROTOCOLS", "A003 class fixture must preserve protocol graph nodes", failures)
    checks_total += require(class_graph.get("property_nodes", 0) >= 3, display_path(path), "M252-E001-A003-CLASS-PROPERTIES", "A003 class fixture must preserve property graph nodes", failures)
    checks_total += require(class_graph.get("ivar_nodes", 0) >= 1, display_path(path), "M252-E001-A003-CLASS-IVARS", "A003 class fixture must preserve ivar graph nodes", failures)
    checks_total += require(class_graph.get("method_nodes", 0) >= 5, display_path(path), "M252-E001-A003-CLASS-METHODS", "A003 class fixture must preserve method graph nodes", failures)
    checks_total += require(category_graph.get("source_graph_complete") is True, display_path(path), "M252-E001-A003-CATEGORY-COMPLETE", "A003 category fixture must keep source_graph_complete == true", failures)
    checks_total += require(category_graph.get("ready_for_lowering") is False, display_path(path), "M252-E001-A003-CATEGORY-NOT-LOWERING", "A003 category fixture must remain not-ready-for-lowering in the lane-A-only proof", failures)
    checks_total += require(category_graph.get("category_nodes", 0) >= 1, display_path(path), "M252-E001-A003-CATEGORY-NODES", "A003 category fixture must preserve category graph nodes", failures)
    checks_total += require(category_graph.get("property_nodes", 0) >= 3, display_path(path), "M252-E001-A003-CATEGORY-PROPERTIES", "A003 category fixture must preserve property graph nodes", failures)
    checks_total += require(category_graph.get("ivar_nodes", 0) >= 1, display_path(path), "M252-E001-A003-CATEGORY-IVARS", "A003 category fixture must preserve ivar graph nodes", failures)

    facts = {
        "source_graph_cases_ready": 2,
        "class_fixture_protocol_nodes": class_graph.get("protocol_nodes", 0),
        "category_fixture_category_nodes": category_graph.get("category_nodes", 0),
    }
    return checks_total, failures, facts


def validate_b004(path: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(path.exists(), display_path(path), "M252-E001-B004-EXISTS", "B004 summary is missing", failures)
    if failures:
        return checks_total, failures, {}

    payload = load_json_object(path)
    checks_total += require(payload.get("ok") is True, display_path(path), "M252-E001-B004-OK", "B004 summary must report ok: true", failures)
    checks_total += require(payload.get("runner_probes_executed") is True, display_path(path), "M252-E001-B004-PROBES", "B004 runner probes must be executed", failures)
    runner_cases = payload.get("runner_cases")
    checks_total += require(isinstance(runner_cases, dict), display_path(path), "M252-E001-B004-CASES", "B004 runner_cases payload must be an object", failures)
    if failures:
        return checks_total, failures, {}

    class_case = runner_cases.get("class_property_synthesis_ready")
    category_case = runner_cases.get("category_property_export_only")
    missing_case = runner_cases.get("missing_interface_property")
    incompatible_case = runner_cases.get("incompatible_property_signature")
    checks_total += require(isinstance(class_case, dict), display_path(path), "M252-E001-B004-CLASS-CASE", "B004 class_property_synthesis_ready case is missing", failures)
    checks_total += require(isinstance(category_case, dict), display_path(path), "M252-E001-B004-CATEGORY-CASE", "B004 category_property_export_only case is missing", failures)
    checks_total += require(isinstance(missing_case, dict), display_path(path), "M252-E001-B004-MISSING-CASE", "B004 missing_interface_property case is missing", failures)
    checks_total += require(isinstance(incompatible_case, dict), display_path(path), "M252-E001-B004-INCOMPATIBLE-CASE", "B004 incompatible_property_signature case is missing", failures)
    if failures:
        return checks_total, failures, {}

    class_counts = class_case.get("expected_counts") if isinstance(class_case.get("expected_counts"), dict) else {}
    category_counts = category_case.get("expected_counts") if isinstance(category_case.get("expected_counts"), dict) else {}
    checks_total += require(class_case.get("observed_codes") == [], display_path(path), "M252-E001-B004-CLASS-CODES", "B004 class_property_synthesis_ready must stay diagnostic-clean", failures)
    checks_total += require(class_counts.get("property_synthesis_sites") == 1, display_path(path), "M252-E001-B004-CLASS-SYNTHESIS", "B004 class_property_synthesis_ready must preserve one synthesis site", failures)
    checks_total += require(class_counts.get("ivar_binding_resolved") == 1, display_path(path), "M252-E001-B004-CLASS-IVAR-RESOLVED", "B004 class_property_synthesis_ready must preserve one resolved ivar binding", failures)
    checks_total += require(class_counts.get("ivar_binding_missing") == 0, display_path(path), "M252-E001-B004-CLASS-IVAR-MISSING", "B004 class_property_synthesis_ready must not introduce missing ivar bindings", failures)
    checks_total += require(class_counts.get("ivar_binding_conflicts") == 0, display_path(path), "M252-E001-B004-CLASS-IVAR-CONFLICTS", "B004 class_property_synthesis_ready must not introduce ivar binding conflicts", failures)
    checks_total += require("deterministic=true" in str(class_case.get("observed_replay_key", "")), display_path(path), "M252-E001-B004-CLASS-REPLAY", "B004 class_property_synthesis_ready replay key must remain deterministic", failures)
    checks_total += require(category_case.get("observed_codes") == [], display_path(path), "M252-E001-B004-CATEGORY-CODES", "B004 category_property_export_only must stay diagnostic-clean", failures)
    checks_total += require(category_counts.get("property_synthesis_sites") == 0, display_path(path), "M252-E001-B004-CATEGORY-SYNTHESIS", "B004 category_property_export_only must keep zero synthesis sites", failures)
    checks_total += require(category_counts.get("ivar_binding_sites") == 0, display_path(path), "M252-E001-B004-CATEGORY-IVAR-SITES", "B004 category_property_export_only must keep zero ivar binding sites", failures)
    checks_total += require(missing_case.get("observed_codes") == ["O3S206"], display_path(path), "M252-E001-B004-MISSING-CODE", "B004 missing_interface_property must keep deterministic O3S206 diagnostics", failures)
    checks_total += require(incompatible_case.get("observed_codes") == ["O3S206"], display_path(path), "M252-E001-B004-INCOMPATIBLE-CODE", "B004 incompatible_property_signature must keep deterministic O3S206 diagnostics", failures)

    facts = {
        "diagnostic_negative_case_count": 2,
        "property_synthesis_sites": class_counts.get("property_synthesis_sites", 0),
        "property_ivar_conflicts": class_counts.get("ivar_binding_conflicts", 0),
    }
    return checks_total, failures, facts


def validate_c003(path: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(path.exists(), display_path(path), "M252-E001-C003-EXISTS", "C003 summary is missing", failures)
    if failures:
        return checks_total, failures, {}

    payload = load_json_object(path)
    checks_total += require(payload.get("ok") is True, display_path(path), "M252-E001-C003-OK", "C003 summary must report ok: true", failures)
    checks_total += require(payload.get("runner_probes_executed") is True, display_path(path), "M252-E001-C003-PROBES", "C003 runner probes must be executed", failures)
    runner_cases = payload.get("runner_cases")
    checks_total += require(isinstance(runner_cases, dict), display_path(path), "M252-E001-C003-CASES", "C003 runner_cases payload must be an object", failures)
    if failures:
        return checks_total, failures, {}

    class_case = runner_cases.get("class_protocol_property_ivar")
    category_case = runner_cases.get("category_protocol_property")
    hello_case = runner_cases.get("hello_ir_anchor")
    checks_total += require(isinstance(class_case, dict), display_path(path), "M252-E001-C003-CLASS-CASE", "C003 class_protocol_property_ivar case is missing", failures)
    checks_total += require(isinstance(category_case, dict), display_path(path), "M252-E001-C003-CATEGORY-CASE", "C003 category_protocol_property case is missing", failures)
    checks_total += require(isinstance(hello_case, dict), display_path(path), "M252-E001-C003-HELLO-CASE", "C003 hello_ir_anchor case is missing", failures)
    if failures:
        return checks_total, failures, {}

    checks_total += require(class_case.get("row_key") == "class-protocol-property-ivar-manifest-projection", display_path(path), "M252-E001-C003-CLASS-ROW", "C003 class row key drifted", failures)
    checks_total += require(category_case.get("row_key") == "category-protocol-property-manifest-projection", display_path(path), "M252-E001-C003-CATEGORY-ROW", "C003 category row key drifted", failures)
    checks_total += require(hello_case.get("row_key") == "hello-ir-named-metadata-anchor", display_path(path), "M252-E001-C003-HELLO-ROW", "C003 hello row key drifted", failures)
    checks_total += require(bool(class_case.get("active_typed_handoff_replay_key")), display_path(path), "M252-E001-C003-CLASS-TYPED", "C003 class case must carry the active typed handoff replay key", failures)
    checks_total += require(bool(category_case.get("active_typed_handoff_replay_key")), display_path(path), "M252-E001-C003-CATEGORY-TYPED", "C003 category case must carry the active typed handoff replay key", failures)
    checks_total += require("hello-ir-named-metadata-anchor" in str(hello_case.get("debug_replay_key", "")), display_path(path), "M252-E001-C003-HELLO-REPLAY", "C003 hello case debug replay key must preserve the IR anchor row", failures)

    facts = {
        "debug_projection_row_count": 3,
        "debug_projection_named_metadata_row": hello_case.get("row_key", ""),
    }
    return checks_total, failures, facts


def validate_d002(path: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(path.exists(), display_path(path), "M252-E001-D002-EXISTS", "D002 summary is missing", failures)
    if failures:
        return checks_total, failures, {}

    payload = load_json_object(path)
    checks_total += require(payload.get("ok") is True, display_path(path), "M252-E001-D002-OK", "D002 summary must report ok: true", failures)
    checks_total += require(payload.get("dynamic_probes_executed") is True, display_path(path), "M252-E001-D002-PROBES", "D002 dynamic probes must be executed", failures)
    dynamic_cases = payload.get("dynamic_cases")
    checks_total += require(isinstance(dynamic_cases, list), display_path(path), "M252-E001-D002-CASES", "D002 dynamic_cases payload must be a list", failures)
    if failures:
        return checks_total, failures, {}

    checks_total += require(len(dynamic_cases) == 1, display_path(path), "M252-E001-D002-CASE-COUNT", "D002 must preserve exactly one binary-boundary dynamic case", failures)
    if len(dynamic_cases) != 1:
        return checks_total, failures, {}

    case = dynamic_cases[0]
    checks_total += require(case.get("fixture") == "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3", display_path(path), "M252-E001-D002-FIXTURE", "D002 dynamic case fixture drifted", failures)
    checks_total += require(str(case.get("runtime_metadata_binary_path", "")).endswith("module.runtime-metadata.bin"), display_path(path), "M252-E001-D002-ARTIFACT", "D002 runtime metadata binary artifact path drifted", failures)
    checks_total += require(case.get("chunk_names") == ["runtime_ingest_packaging_contract", "typed_lowering_handoff", "debug_projection"], display_path(path), "M252-E001-D002-CHUNKS", "D002 chunk ordering drifted", failures)
    checks_total += require(int(case.get("payload_bytes", 0)) > 32, display_path(path), "M252-E001-D002-PAYLOAD-BYTES", "D002 binary payload must remain non-trivial", failures)
    checks_total += require(bool(case.get("packaging_contract_replay_key")), display_path(path), "M252-E001-D002-PACKAGING-REPLAY", "D002 packaging replay key is missing", failures)
    checks_total += require(bool(case.get("typed_handoff_replay_key")), display_path(path), "M252-E001-D002-TYPED-REPLAY", "D002 typed handoff replay key is missing", failures)
    checks_total += require(bool(case.get("debug_projection_replay_key")), display_path(path), "M252-E001-D002-DEBUG-REPLAY", "D002 debug projection replay key is missing", failures)
    checks_total += require("artifact_relative_path=module.runtime-metadata.bin" in str(case.get("replay_key", "")), display_path(path), "M252-E001-D002-REPLAY-ARTIFACT", "D002 aggregate replay key must preserve the runtime metadata binary artifact path", failures)

    facts = {
        "runtime_metadata_chunk_count": len(case.get("chunk_names", [])),
        "runtime_metadata_payload_bytes": int(case.get("payload_bytes", 0)),
    }
    return checks_total, failures, facts


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0

    doc_checks = (
        (args.expectations_doc, "M252-E001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-E001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-E001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-E001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-E001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-E001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-E001-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-E001-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-E001-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_artifacts, "M252-E001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, "M252-E001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        doc_total, doc_failures = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += doc_total
        failures.extend(doc_failures)

    a003_total, a003_failures, a003_facts = validate_a003(args.a003_summary)
    b004_total, b004_failures, b004_facts = validate_b004(args.b004_summary)
    c003_total, c003_failures, c003_facts = validate_c003(args.c003_summary)
    d002_total, d002_failures, d002_facts = validate_d002(args.d002_summary)
    checks_total += a003_total + b004_total + c003_total + d002_total
    failures.extend(a003_failures)
    failures.extend(b004_failures)
    failures.extend(c003_failures)
    failures.extend(d002_failures)

    facts = {
        **a003_facts,
        **b004_facts,
        **c003_facts,
        **d002_facts,
    }
    checks_total += require(
        facts.get("source_graph_cases_ready") == 2
        and facts.get("diagnostic_negative_case_count") == 2
        and facts.get("debug_projection_row_count") == 3
        and facts.get("runtime_metadata_chunk_count") == 3,
        "aggregate",
        "M252-E001-AGGREGATE-CLOSURE",
        "aggregate semantic-closure prerequisites are incomplete",
        failures,
    )

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "upstream_summaries": {
            "a003": display_path(args.a003_summary),
            "b004": display_path(args.b004_summary),
            "c003": display_path(args.c003_summary),
            "d002": display_path(args.d002_summary),
        },
        "next_implementation_issue": "M253-A001",
        "aggregate_semantic_closure_ready_for_section_emission": not failures,
        **facts,
    }

    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[FAIL] M252-E001 metadata semantic-closure gate drift detected; summary: {display_path(summary_out)}")
        return 1

    print(f"[PASS] M252-E001 metadata semantic-closure gate preserved; summary: {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
