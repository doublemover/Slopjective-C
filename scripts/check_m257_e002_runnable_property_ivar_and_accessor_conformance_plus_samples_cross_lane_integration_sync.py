#!/usr/bin/env python3
"""Validate M257-E002 runnable property/ivar/accessor execution matrix."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-e002-runnable-property-ivar-accessor-conformance-plus-samples-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-runnable-property-ivar-accessor-execution-matrix/m257-e002-v1"
EVIDENCE_MODEL = "a002-b003-c003-d003-e001-summary-chain-plus-live-property-runtime-execution"
EXECUTION_MATRIX_MODEL = (
    "runnable-property-ivar-matrix-composes-upstream-summaries-with-live-storage-accessor-and-reflection-proof"
)
FAILURE_MODEL = "fail-closed-on-runnable-property-ivar-matrix-drift-or-missing-live-runtime-proof"
NEXT_ISSUE = "M258-A001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m257" / "M257-E002" / "runnable_property_ivar_execution_matrix_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
CLANGXX_CANDIDATES = ("clang++", "clang++-21")
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "e002-runnable-property-matrix"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_execution_matrix_positive.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m257_e002_property_ivar_execution_matrix_probe.cpp"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-A002" / "property_ivar_source_model_completion_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-B003" / "accessor_legality_attribute_interactions_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-C003" / "synthesized_accessor_property_lowering_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-D003" / "property_metadata_reflection_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-E001" / "property_ivar_execution_gate_summary.json"

A002_CONTRACT_ID = "objc3c-executable-property-ivar-source-model-completion/m257-a002-v1"
B003_CONTRACT_ID = "objc3c-property-accessor-attribute-interactions/m257-b003-v1"
C003_CONTRACT_ID = "objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1"
D003_CONTRACT_ID = "objc3c-runtime-property-metadata-reflection/m257-d003-v1"
E001_CONTRACT_ID = "objc3c-executable-property-ivar-execution-gate/m257-e001-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m257_e002_lane_e_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py"

C003_BOUNDARY_PREFIX = "; executable_synthesized_accessor_property_lowering = "
D003_BOUNDARY_PREFIX = "; runtime_property_metadata_reflection = "


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M257-E002-DOC-EXP-01", "# M257 Runnable Property, Ivar, And Accessor Conformance Plus Samples Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M257-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M257-E002-DOC-EXP-03", "`count` stores and reloads `37`"),
        SnippetCheck("M257-E002-DOC-EXP-04", "The matrix must explicitly hand off to `M258-A001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M257-E002-DOC-PKT-01", "# M257-E002 Runnable Property, Ivar, And Accessor Conformance Plus Samples Cross-Lane Integration Sync Packet"),
        SnippetCheck("M257-E002-DOC-PKT-02", "Packet: `M257-E002`"),
        SnippetCheck("M257-E002-DOC-PKT-03", "Issue: `#7157`"),
        SnippetCheck("M257-E002-DOC-PKT-04", "- `M257-E001`"),
        SnippetCheck("M257-E002-DOC-PKT-05", "`M258-A001` is the explicit next handoff after this matrix closes."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M257-E002-NDOC-01", "## Runnable property, ivar, and accessor execution matrix (M257-E002)"),
        SnippetCheck("M257-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-E002-NDOC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M257-E002-NDOC-04", "tmp/reports/m257/M257-E002/runnable_property_ivar_execution_matrix_summary.json"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M257-E002-SPC-01", "## M257 runnable property/ivar/accessor execution matrix (E002)"),
        SnippetCheck("M257-E002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-E002-SPC-03", f"`{EXECUTION_MATRIX_MODEL}`"),
        SnippetCheck("M257-E002-SPC-04", "`M258-A001`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M257-E002-META-01", "## M257 runnable property/ivar/accessor execution matrix metadata anchors (E002)"),
        SnippetCheck("M257-E002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M257-E002-META-03", "`tmp/reports/m257/M257-E002/runnable_property_ivar_execution_matrix_summary.json`"),
        SnippetCheck("M257-E002-META-04", "`tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M257-E002-ARCH-01", "## M257 runnable property and ivar execution matrix (E002)"),
        SnippetCheck("M257-E002-ARCH-02", "`M257-E002` broadens the frozen `M257-E001` gate into one live runnable property matrix:"),
        SnippetCheck("M257-E002-ARCH-03", "check:objc3c:m257-e002-lane-e-readiness"),
    ),
    AST_HEADER: (
        SnippetCheck("M257-E002-AST-01", "M257-E002 runnable property-ivar execution-matrix anchor"),
    ),
    SEMA_CPP: (
        SnippetCheck("M257-E002-SEMA-01", "M257-E002 runnable property-ivar execution-matrix anchor"),
    ),
    IR_CPP: (
        SnippetCheck("M257-E002-IR-01", "M257-E002 runnable property-ivar execution-matrix anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M257-E002-PKG-01", '"check:objc3c:m257-e002-runnable-property-ivar-and-accessor-conformance-plus-samples": "python scripts/check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py"'),
        SnippetCheck("M257-E002-PKG-02", '"test:tooling:m257-e002-runnable-property-ivar-and-accessor-conformance-plus-samples": "python -m pytest tests/tooling/test_check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py -q"'),
        SnippetCheck("M257-E002-PKG-03", '"check:objc3c:m257-e002-lane-e-readiness": "python scripts/run_m257_e002_lane_e_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M257-E002-RUN-01", "check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py"),
        SnippetCheck("M257-E002-RUN-02", "check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py"),
        SnippetCheck("M257-E002-RUN-03", "check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py"),
        SnippetCheck("M257-E002-RUN-04", "check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py"),
        SnippetCheck("M257-E002-RUN-05", "check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M257-E002-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M257-E002-TEST-02", CONTRACT_ID),
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


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def resolve_tool(candidates: Sequence[str]) -> str | None:
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved is not None:
            return resolved
    return None


def boundary_line(ir_text: str, prefix: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(prefix):
            return line.strip()
    return ""


def validate_a002(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(A002_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M257-E002-A002-01", "A002 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == A002_CONTRACT_ID, artifact, "M257-E002-A002-02", "A002 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E002-A002-03", "A002 summary must report full coverage", failures)
    dynamic_probe = payload.get("dynamic_probe", {})
    checks_total += require(isinstance(dynamic_probe, dict), artifact, "M257-E002-A002-04", "A002 dynamic probe payload must remain a dictionary", failures)
    return checks_total, {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
    }


def validate_b003(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(B003_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("contract_id") == B003_CONTRACT_ID, artifact, "M257-E002-B003-01", "B003 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E002-B003-02", "B003 summary must report full coverage", failures)
    checks_total += require(payload.get("findings") == [], artifact, "M257-E002-B003-03", "B003 findings must remain empty", failures)
    return checks_total, {
        "contract_id": payload.get("contract_id"),
        "checks_passed": payload.get("checks_passed"),
        "checks_total": payload.get("checks_total"),
    }


def validate_c003(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(C003_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M257-E002-C003-01", "C003 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == C003_CONTRACT_ID, artifact, "M257-E002-C003-02", "C003 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E002-C003-03", "C003 summary must report full coverage", failures)
    return checks_total, {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
    }


def validate_d003(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(D003_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M257-E002-D003-01", "D003 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == D003_CONTRACT_ID, artifact, "M257-E002-D003-02", "D003 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E002-D003-03", "D003 summary must report full coverage", failures)
    return checks_total, {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
    }


def validate_e001(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(E001_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M257-E002-E001-01", "E001 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == E001_CONTRACT_ID, artifact, "M257-E002-E001-02", "E001 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E002-E001-03", "E001 summary must report full coverage", failures)
    return checks_total, {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "upstream_summaries": payload.get("upstream_summaries"),
    }


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    artifact = "dynamic_probe_payload"
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(isinstance(payload, dict), "M257-E002-DYN-PAY-01", "probe payload must be a JSON object")
    if not isinstance(payload, dict):
        return checks_passed, checks_total

    check(isinstance(payload.get("widget_instance"), int) and payload["widget_instance"] > 0, "M257-E002-DYN-PAY-02", "widget instance must be a non-zero runtime identity")
    check(payload.get("set_count_result") == 0, "M257-E002-DYN-PAY-03", "setCount: must report runtime success")
    check(payload.get("count_value") == 37, "M257-E002-DYN-PAY-04", "count getter must reload the stored value 37")
    check(payload.get("set_enabled_result") == 0, "M257-E002-DYN-PAY-05", "setEnabled: must report runtime success")
    check(payload.get("enabled_value") == 1, "M257-E002-DYN-PAY-06", "enabled getter must reload the stored value 1")
    check(payload.get("set_value_result") == 0, "M257-E002-DYN-PAY-07", "setCurrentValue: must report runtime success")
    check(payload.get("value_result") == 55, "M257-E002-DYN-PAY-08", "currentValue getter must reload the stored value 55")
    check(payload.get("token_value") == 0, "M257-E002-DYN-PAY-09", "tokenValue must remain the default zero value")

    widget_entry = payload.get("widget_entry", {})
    check(widget_entry.get("found") == 1, "M257-E002-DYN-PAY-10", "Widget realized class entry must resolve")
    check(widget_entry.get("base_identity") == 1024, "M257-E002-DYN-PAY-11", "Widget base identity must remain 1024")
    check(widget_entry.get("runtime_property_accessor_count") == 4, "M257-E002-DYN-PAY-12", "Widget must publish four runtime property accessors")
    check(widget_entry.get("runtime_instance_size_bytes") == 24, "M257-E002-DYN-PAY-13", "Widget instance size must remain 24 bytes")
    check(widget_entry.get("class_name") == "Widget", "M257-E002-DYN-PAY-14", "Widget class entry name drifted")
    check(widget_entry.get("class_owner_identity") == "class:Widget", "M257-E002-DYN-PAY-15", "Widget class owner identity drifted")

    registry_state = payload.get("registry_state", {})
    check(registry_state.get("layout_ready_class_count") == 1, "M257-E002-DYN-PAY-16", "registry must publish one layout-ready class")
    check(registry_state.get("reflectable_property_count") == 4, "M257-E002-DYN-PAY-17", "registry must publish four reflectable properties")
    check(registry_state.get("writable_property_count") == 3, "M257-E002-DYN-PAY-18", "registry must publish three writable properties")
    check(registry_state.get("slot_backed_property_count") == 4, "M257-E002-DYN-PAY-19", "registry must publish four slot-backed properties")
    check(registry_state.get("last_query_found") == 1, "M257-E002-DYN-PAY-20", "registry must preserve the final successful property lookup")
    check(registry_state.get("last_queried_class_name") == "Widget", "M257-E002-DYN-PAY-21", "registry queried class name drifted")
    check(registry_state.get("last_queried_property_name") == "token", "M257-E002-DYN-PAY-22", "registry final queried property drifted")
    check(registry_state.get("last_resolved_class_name") == "Widget", "M257-E002-DYN-PAY-23", "registry resolved class drifted")
    check(registry_state.get("last_resolved_owner_identity") == "implementation:Widget", "M257-E002-DYN-PAY-24", "registry resolved owner drifted")

    count_property = payload.get("count_property", {})
    enabled_property = payload.get("enabled_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})

    check(count_property.get("found") == 1 and count_property.get("setter_available") == 1, "M257-E002-DYN-PAY-25", "count property must resolve as writable")
    check(count_property.get("effective_getter_selector") == "count", "M257-E002-DYN-PAY-26", "count getter selector drifted")
    check(count_property.get("effective_setter_selector") == "setCount:", "M257-E002-DYN-PAY-27", "count setter selector drifted")
    check(count_property.get("slot_index") == 0 and count_property.get("offset_bytes") == 0, "M257-E002-DYN-PAY-28", "count layout slot drifted")
    check(count_property.get("size_bytes") == 4 and count_property.get("alignment_bytes") == 4, "M257-E002-DYN-PAY-29", "count layout size/alignment drifted")
    check(count_property.get("getter_owner_identity") == "implementation:Widget::instance_method:count", "M257-E002-DYN-PAY-30", "count getter owner drifted")
    check(count_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCount:", "M257-E002-DYN-PAY-31", "count setter owner drifted")

    check(enabled_property.get("found") == 1 and enabled_property.get("setter_available") == 1, "M257-E002-DYN-PAY-32", "enabled property must resolve as writable")
    check(enabled_property.get("effective_getter_selector") == "enabled", "M257-E002-DYN-PAY-33", "enabled getter selector drifted")
    check(enabled_property.get("effective_setter_selector") == "setEnabled:", "M257-E002-DYN-PAY-34", "enabled setter selector drifted")
    check(enabled_property.get("slot_index") == 1 and enabled_property.get("offset_bytes") == 4, "M257-E002-DYN-PAY-35", "enabled layout slot drifted")
    check(enabled_property.get("size_bytes") == 1 and enabled_property.get("alignment_bytes") == 1, "M257-E002-DYN-PAY-36", "enabled layout size/alignment drifted")
    check(enabled_property.get("getter_owner_identity") == "implementation:Widget::instance_method:enabled", "M257-E002-DYN-PAY-37", "enabled getter owner drifted")
    check(enabled_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setEnabled:", "M257-E002-DYN-PAY-38", "enabled setter owner drifted")

    check(value_property.get("found") == 1 and value_property.get("setter_available") == 1, "M257-E002-DYN-PAY-39", "value property must resolve as writable")
    check(value_property.get("effective_getter_selector") == "currentValue", "M257-E002-DYN-PAY-40", "value getter selector drifted")
    check(value_property.get("effective_setter_selector") == "setCurrentValue:", "M257-E002-DYN-PAY-41", "value setter selector drifted")
    check(value_property.get("slot_index") == 2 and value_property.get("offset_bytes") == 8, "M257-E002-DYN-PAY-42", "value layout slot drifted")
    check(value_property.get("size_bytes") == 8 and value_property.get("alignment_bytes") == 8, "M257-E002-DYN-PAY-43", "value layout size/alignment drifted")
    check(value_property.get("getter_owner_identity") == "implementation:Widget::instance_method:currentValue", "M257-E002-DYN-PAY-44", "value getter owner drifted")
    check(value_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCurrentValue:", "M257-E002-DYN-PAY-45", "value setter owner drifted")

    check(token_property.get("found") == 1 and token_property.get("setter_available") == 0, "M257-E002-DYN-PAY-46", "token property must resolve as readonly")
    check(token_property.get("effective_getter_selector") == "tokenValue", "M257-E002-DYN-PAY-47", "token getter selector drifted")
    check(token_property.get("effective_setter_selector") is None, "M257-E002-DYN-PAY-48", "token must not publish a setter selector")
    check(token_property.get("slot_index") == 3 and token_property.get("offset_bytes") == 16, "M257-E002-DYN-PAY-49", "token layout slot drifted")
    check(token_property.get("size_bytes") == 8 and token_property.get("alignment_bytes") == 8, "M257-E002-DYN-PAY-50", "token layout size/alignment drifted")
    check(token_property.get("getter_owner_identity") == "implementation:Widget::instance_method:tokenValue", "M257-E002-DYN-PAY-51", "token getter owner drifted")
    check(token_property.get("setter_owner_identity") is None, "M257-E002-DYN-PAY-52", "token must not publish a setter owner")

    count_method = payload.get("count_method", {})
    enabled_method = payload.get("enabled_method", {})
    value_method = payload.get("value_method", {})
    token_method = payload.get("token_method", {})
    check(count_method.get("found") == 1 and count_method.get("resolved") == 1, "M257-E002-DYN-PAY-53", "count method cache entry must resolve")
    check(count_method.get("resolved_owner_identity") == "implementation:Widget::instance_method:count", "M257-E002-DYN-PAY-54", "count method owner drifted")
    check(enabled_method.get("resolved_owner_identity") == "implementation:Widget::instance_method:enabled", "M257-E002-DYN-PAY-55", "enabled method owner drifted")
    check(value_method.get("selector") == "currentValue" and value_method.get("resolved_owner_identity") == "implementation:Widget::instance_method:currentValue", "M257-E002-DYN-PAY-56", "value method cache entry drifted")
    check(token_method.get("selector") == "tokenValue" and token_method.get("resolved_owner_identity") == "implementation:Widget::instance_method:tokenValue", "M257-E002-DYN-PAY-57", "token method cache entry drifted")
    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M257-E002-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M257-E002-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M257-E002-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M257-E002-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M257-E002-DYN-05", f"unable to resolve any clang++ candidate from {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    probe_dir = args.probe_root.resolve() / f"probe-{uuid.uuid4().hex}"
    probe_dir.mkdir(parents=True, exist_ok=True)

    compile_result = run_command(
        [
            str(args.native_exe),
            str(args.fixture),
            "--out-dir",
            str(probe_dir),
            "--emit-prefix",
            "module",
        ],
        ROOT,
    )
    check(compile_result.returncode == 0, "M257-E002-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    module_manifest = probe_dir / "module.manifest.json"
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    module_backend = probe_dir / "module.object-backend.txt"
    check(module_manifest.exists(), "M257-E002-DYN-07", f"missing emitted manifest: {display_path(module_manifest)}")
    check(module_ir.exists(), "M257-E002-DYN-08", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M257-E002-DYN-09", f"missing emitted object: {display_path(module_obj)}")
    check(module_backend.exists(), "M257-E002-DYN-10", f"missing backend marker: {display_path(module_backend)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists() or not module_backend.exists():
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    backend_text = module_backend.read_text(encoding="utf-8").strip()
    check(backend_text == "llvm-direct", "M257-E002-DYN-11", "fixture must preserve llvm-direct object emission")

    ir_text = module_ir.read_text(encoding="utf-8")
    c003_boundary = boundary_line(ir_text, C003_BOUNDARY_PREFIX)
    d003_boundary = boundary_line(ir_text, D003_BOUNDARY_PREFIX)
    check(bool(c003_boundary), "M257-E002-DYN-12", "IR must publish the C003 synthesized accessor lowering summary")
    check("synthesized_accessor_entries=7" in c003_boundary, "M257-E002-DYN-13", "C003 boundary must publish seven synthesized accessors")
    check("synthesized_storage_globals=4" in c003_boundary, "M257-E002-DYN-14", "C003 boundary must publish four synthesized storage globals")
    check(bool(d003_boundary), "M257-E002-DYN-15", "IR must publish the D003 runtime property reflection summary")
    check("reflectable_property_entries=8" in d003_boundary, "M257-E002-DYN-16", "D003 boundary must publish the eight-record property descriptor inventory")
    check("writable_property_entries=3" in d003_boundary, "M257-E002-DYN-17", "D003 boundary must publish three writable property entries")
    check("synthesized_accessor_entries=7" in d003_boundary, "M257-E002-DYN-18", "D003 boundary must preserve seven synthesized accessors")

    probe_exe = probe_dir / "m257_e002_property_ivar_execution_matrix_probe.exe"
    probe_compile = run_command(
        [
            str(clangxx),
            "-std=c++20",
            "-I",
            str(args.runtime_include_root),
            str(args.runtime_probe),
            str(module_obj),
            str(args.runtime_library),
            "-o",
            str(probe_exe),
        ],
        ROOT,
    )
    check(probe_compile.returncode == 0, "M257-E002-DYN-19", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M257-E002-DYN-20", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M257-E002-DYN-21", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = validate_probe_payload(payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_manifest": display_path(module_manifest),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "module_backend": backend_text,
        "probe_exe": display_path(probe_exe),
        "c003_boundary": c003_boundary,
        "d003_boundary": d003_boundary,
        "payload": payload,
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--probe-root", type=Path, default=PROBE_ROOT)
    parser.add_argument("--fixture", type=Path, default=FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=RUNTIME_PROBE)
    parser.add_argument("--clangxx", nargs="+", default=list(CLANGXX_CANDIDATES))
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        static_total, static_findings = check_static_contract(path, snippets)
        checks_total += static_total
        checks_passed += static_total - len(static_findings)
        failures.extend(static_findings)

    a002_checks, a002_distilled = validate_a002(load_json(A002_SUMMARY), failures)
    b003_checks, b003_distilled = validate_b003(load_json(B003_SUMMARY), failures)
    c003_checks, c003_distilled = validate_c003(load_json(C003_SUMMARY), failures)
    d003_checks, d003_distilled = validate_d003(load_json(D003_SUMMARY), failures)
    e001_checks, e001_distilled = validate_e001(load_json(E001_SUMMARY), failures)
    checks_total += a002_checks + b003_checks + c003_checks + d003_checks + e001_checks
    checks_passed += a002_checks + b003_checks + c003_checks + d003_checks + e001_checks

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_passed, dynamic_total, dynamic_payload = run_dynamic_case(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    checks_passed = checks_total - len(failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "evidence_model": EVIDENCE_MODEL,
        "execution_matrix_model": EXECUTION_MATRIX_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "upstream_summaries": {
            "M257-A002": a002_distilled,
            "M257-B003": b003_distilled,
            "M257-C003": c003_distilled,
            "M257-D003": d003_distilled,
            "M257-E001": e001_distilled,
        },
        "dynamic_probe": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)")
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}")
        print(f"[info] summary: {display_path(args.summary_out)}")
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
