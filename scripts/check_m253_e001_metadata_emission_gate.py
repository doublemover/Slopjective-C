#!/usr/bin/env python3
"""Fail-closed contract checker for M253-E001 metadata emission gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-e001-metadata-emission-gate-v1"
CONTRACT_ID = "objc3c-runtime-metadata-emission-gate/m253-e001-v1"
EVIDENCE_MODEL = "a002-b003-c006-d003-summary-chain"
FAILURE_MODEL = "fail-closed-on-upstream-summary-drift"
NEXT_CLOSEOUT_ISSUE = "M253-E002"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m253_metadata_emission_gate_contract_and_architecture_freeze_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m253"
    / "m253_e001_metadata_emission_gate_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m253_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A002_SUMMARY = ROOT / "tmp" / "reports" / "m253" / "M253-A002" / "source_to_section_mapping_completeness_matrix_summary.json"
DEFAULT_B003_SUMMARY = ROOT / "tmp" / "reports" / "m253" / "M253-B003" / "coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json"
DEFAULT_C006_SUMMARY = ROOT / "tmp" / "reports" / "m253" / "M253-C006" / "binary_inspection_harness_summary.json"
DEFAULT_D003_SUMMARY = ROOT / "tmp" / "reports" / "m253" / "M253-D003" / "archive_and_static_link_metadata_discovery_behavior_summary.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m253" / "M253-E001" / "metadata_emission_gate_summary.json"

EXPECTED_RETAINED_SYMBOLS = [
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
]
EXPECTED_BASE_SECTION_NAMES = [
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
]
EXPECTED_A002_SECTION_NAMES = EXPECTED_BASE_SECTION_NAMES + [
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
]


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
    SnippetCheck("M253-E001-DOC-EXP-01", "# M253 Metadata Emission Gate Contract And Architecture Freeze Expectations (E001)"),
    SnippetCheck("M253-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-E001-DOC-EXP-03", "tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json"),
    SnippetCheck("M253-E001-DOC-EXP-04", "tmp/reports/m253/M253-D003/archive_and_static_link_metadata_discovery_behavior_summary.json"),
    SnippetCheck("M253-E001-DOC-EXP-05", "The gate must explicitly hand off to `M253-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck("M253-E001-DOC-PKT-01", "# M253-E001 Metadata Emission Gate Contract And Architecture Freeze Packet"),
    SnippetCheck("M253-E001-DOC-PKT-02", "Packet: `M253-E001`"),
    SnippetCheck("M253-E001-DOC-PKT-03", "- `M253-C006`"),
    SnippetCheck("M253-E001-DOC-PKT-04", "- `M253-D003`"),
    SnippetCheck("M253-E001-DOC-PKT-05", "`check:objc3c:m253-e001-lane-e-readiness`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M253-E001-ARCH-01", "M253 lane-E E001 metadata emission gate anchors explicit aggregate evidence"),
    SnippetCheck("M253-E001-ARCH-02", "A002/B003/C006/D003 metadata emission boundary"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M253-E001-NDOC-01", "## Metadata emission gate (M253-E001)"),
    SnippetCheck("M253-E001-NDOC-02", "Objc3RuntimeMetadataEmissionGateSummary"),
    SnippetCheck("M253-E001-NDOC-03", "!objc3.objc_runtime_metadata_emission_gate"),
    SnippetCheck("M253-E001-NDOC-04", "tmp/reports/m253/M253-E001/metadata_emission_gate_summary.json"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M253-E001-SPC-01", "## M253 metadata emission gate (E001)"),
    SnippetCheck("M253-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-E001-SPC-03", "fail-closed-on-upstream-summary-drift"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M253-E001-META-01", "## M253 metadata emission gate (E001)"),
    SnippetCheck("M253-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-E001-META-03", "next-closeout handoff `M253-E002`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M253-E001-LHDR-01", "kObjc3RuntimeMetadataEmissionGateContractId"),
    SnippetCheck("M253-E001-LHDR-02", "kObjc3RuntimeMetadataEmissionGateEvidenceModel"),
    SnippetCheck("M253-E001-LHDR-03", "kObjc3RuntimeMetadataEmissionGateFailureModel"),
    SnippetCheck("M253-E001-LHDR-04", "Objc3RuntimeMetadataEmissionGateSummary();"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M253-E001-LCPP-01", "Objc3RuntimeMetadataEmissionGateSummary()"),
    SnippetCheck("M253-E001-LCPP-02", "evidence chain over the implemented A002/B003/C006/D003 summaries"),
    SnippetCheck("M253-E001-LCPP-03", "non_goals=no-new-emission-families-or-runtime-registration"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M253-E001-IR-01", '!objc3.objc_runtime_metadata_emission_gate = !{!64}'),
    SnippetCheck("M253-E001-IR-02", '; runtime_metadata_emission_gate = '),
    SnippetCheck("M253-E001-IR-03", 'source_to_section_matrix_issue=M253-A002'),
    SnippetCheck("M253-E001-IR-04", 'archive_static_link_issue=M253-D003'),
)
PROCESS_SNIPPETS = (
    SnippetCheck("M253-E001-PROC-01", "M253-E001 metadata-emission gate anchor"),
    SnippetCheck("M253-E001-PROC-02", "Any drift here must fail closed before later cross-lane closeout runs."),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M253-E001-RUN-01", "check:objc3c:m253-a002-lane-a-readiness"),
    SnippetCheck("M253-E001-RUN-02", "check:objc3c:m253-b003-lane-b-readiness"),
    SnippetCheck("M253-E001-RUN-03", "check:objc3c:m253-c006-lane-c-readiness"),
    SnippetCheck("M253-E001-RUN-04", "check:objc3c:m253-d003-lane-d-readiness"),
    SnippetCheck("M253-E001-RUN-05", "tests/tooling/test_check_m253_e001_metadata_emission_gate.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M253-E001-PKG-01",
        '"check:objc3c:m253-e001-metadata-emission-gate": "python scripts/check_m253_e001_metadata_emission_gate.py"',
    ),
    SnippetCheck(
        "M253-E001-PKG-02",
        '"test:tooling:m253-e001-metadata-emission-gate": "python -m pytest tests/tooling/test_check_m253_e001_metadata_emission_gate.py -q"',
    ),
    SnippetCheck(
        "M253-E001-PKG-03",
        '"check:objc3c:m253-e001-lane-e-readiness": "python scripts/run_m253_e001_lane_e_readiness.py"',
    ),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=DEFAULT_B003_SUMMARY)
    parser.add_argument("--c006-summary", type=Path, default=DEFAULT_C006_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def get_case_map(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    cases = payload.get("dynamic_cases", [])
    if isinstance(cases, list):
        return {str(case.get("case_id")): case for case in cases if isinstance(case, dict)}
    return {}


def get_section(case: dict[str, Any], name: str) -> dict[str, Any] | None:
    for section in case.get("sections", []):
        if isinstance(section, dict) and section.get("name") == name:
            return section
    return None


def validate_a002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M253-E001-A002-01", "A002 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M253-E001-A002-02", "A002 summary must report full check pass coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M253-E001-A002-03", "A002 summary must report dynamic probes executed", failures)
    checks_total += 1
    checks_passed += require(payload.get("expected_row_count") == 9, artifact, "M253-E001-A002-04", "A002 summary must freeze expected_row_count=9", failures)

    dynamic_cases = payload.get("dynamic_cases", {})
    inspection = dynamic_cases.get("inspection_fixture", {}) if isinstance(dynamic_cases, dict) else {}
    matrix = inspection.get("matrix", {}) if isinstance(inspection, dict) else {}

    checks_total += 1
    checks_passed += require(isinstance(inspection, dict) and bool(inspection), artifact, "M253-E001-A002-05", "A002 summary must publish inspection_fixture evidence", failures)
    checks_total += 1
    checks_passed += require(inspection.get("compile_exit_code") == 0, artifact, "M253-E001-A002-06", "A002 inspection fixture must compile successfully", failures)
    checks_total += 1
    checks_passed += require(inspection.get("manifest_exists") is True and inspection.get("object_exists") is True, artifact, "M253-E001-A002-07", "A002 inspection fixture must emit manifest and object outputs", failures)
    checks_total += 1
    checks_passed += require(inspection.get("object_backend") == "llvm-direct", artifact, "M253-E001-A002-08", "A002 inspection fixture must preserve llvm-direct object emission", failures)
    checks_total += 1
    checks_passed += require(inspection.get("readobj_exit_code") == 0 and inspection.get("objdump_exit_code") == 0, artifact, "M253-E001-A002-09", "A002 inspection fixture must remain object-inspectable", failures)
    checks_total += 1
    checks_passed += require(matrix.get("ready") is True and matrix.get("fail_closed") is True, artifact, "M253-E001-A002-10", "A002 matrix must remain ready and fail-closed", failures)
    checks_total += 1
    checks_passed += require(matrix.get("matrix_published") is True and matrix.get("row_ordering_frozen") is True, artifact, "M253-E001-A002-11", "A002 matrix publication and ordering freeze must remain true", failures)
    checks_total += 1
    checks_passed += require(matrix.get("matrix_row_count") == 9 and len(matrix.get("rows", [])) == 9, artifact, "M253-E001-A002-12", "A002 matrix must retain exactly nine rows", failures)
    checks_total += 1
    checks_passed += require(matrix.get("object_inspection_ready") is True and matrix.get("supported_node_coverage_complete") is True, artifact, "M253-E001-A002-13", "A002 matrix must keep object inspection and supported coverage ready", failures)
    checks_total += 1
    checks_passed += require(inspection.get("retained_symbols") == EXPECTED_RETAINED_SYMBOLS, artifact, "M253-E001-A002-14", "A002 retained symbol inventory must remain frozen", failures)
    checks_total += 1
    section_names = [section.get("name") for section in inspection.get("metadata_sections", []) if isinstance(section, dict)]
    checks_passed += require(section_names == EXPECTED_A002_SECTION_NAMES, artifact, "M253-E001-A002-15", "A002 metadata section inventory must retain the base matrix families plus D-lane packaging sections", failures)

    return checks_total, checks_passed, {
        "path": artifact,
        "ok": payload.get("ok") is True,
        "dynamic_probes_executed": payload.get("dynamic_probes_executed") is True,
        "inspection_backend": inspection.get("object_backend"),
        "matrix_row_count": matrix.get("matrix_row_count"),
    }


def validate_b003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M253-E001-B003-01", "B003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == "objc3c-runtime-metadata-object-format-policy/m253-b003-v1", artifact, "M253-E001-B003-02", "B003 summary must preserve the object-format policy contract id", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M253-E001-B003-03", "B003 summary must report dynamic probes executed", failures)

    host_policy = payload.get("host_policy", {}) if isinstance(payload.get("host_policy"), dict) else {}
    checks_total += 1
    checks_passed += require(host_policy.get("object_format") == "coff", artifact, "M253-E001-B003-04", "B003 host policy must remain coff on this host boundary", failures)
    checks_total += 1
    checks_passed += require(host_policy.get("section_spelling_model") == "coff-logical-section-spellings", artifact, "M253-E001-B003-05", "B003 host policy must preserve the COFF section spelling model", failures)
    checks_total += 1
    checks_passed += require(host_policy.get("retention_anchor_model") == "llvm.used-appending-global+coff-timestamp-normalization", artifact, "M253-E001-B003-06", "B003 host policy must preserve the COFF retention anchor model", failures)
    checks_total += 1
    checks_passed += require(host_policy.get("image_info_section") == "objc3.runtime.image_info" and host_policy.get("class_section") == "objc3.runtime.class_descriptors", artifact, "M253-E001-B003-07", "B003 host policy must preserve image/class section spellings", failures)

    case_map = get_case_map(payload)
    native_case = case_map.get("M253-B003-CASE-NATIVE", {})
    checks_total += 1
    checks_passed += require(bool(native_case), artifact, "M253-E001-B003-08", "B003 native dynamic case must remain present", failures)
    checks_total += 1
    checks_passed += require(native_case.get("process_exit_code") == 0, artifact, "M253-E001-B003-09", "B003 native case must compile successfully", failures)
    checks_total += 1
    checks_passed += require(native_case.get("detected_object_format") == "coff", artifact, "M253-E001-B003-10", "B003 native case must still detect COFF output", failures)
    checks_total += 1
    checks_passed += require("object_format_contract=objc3c-runtime-metadata-object-format-policy/m253-b003-v1" in str(native_case.get("policy_line", "")), artifact, "M253-E001-B003-11", "B003 policy line must preserve the object-format contract token", failures)
    checks_total += 1
    checks_passed += require("@llvm.used = appending global" in str(native_case.get("llvm_used_line", "")), artifact, "M253-E001-B003-12", "B003 native case must preserve llvm.used retention", failures)

    return checks_total, checks_passed, {
        "path": artifact,
        "ok": payload.get("ok") is True,
        "dynamic_probes_executed": payload.get("dynamic_probes_executed") is True,
        "host_object_format": host_policy.get("object_format"),
        "native_case_exit_code": native_case.get("process_exit_code"),
    }


def validate_c006(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M253-E001-C006-01", "C006 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == "objc3c-runtime-binary-inspection-harness/m253-c006-v1", artifact, "M253-E001-C006-02", "C006 summary must preserve the binary inspection contract id", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M253-E001-C006-03", "C006 summary must report dynamic probes executed", failures)

    case_map = get_case_map(payload)
    expected_case_ids = {
        "M253-C006-CASE-ZERO-DESCRIPTOR",
        "M253-C006-CASE-CLASS-PROTOCOL-PROPERTY-IVAR",
        "M253-C006-CASE-CATEGORY-PROTOCOL-PROPERTY",
        "M253-C006-CASE-MESSAGE-SEND",
        "M253-C006-CASE-NEGATIVE-MISSING-INTERFACE-PROPERTY",
    }
    checks_total += 1
    checks_passed += require(set(case_map) == expected_case_ids, artifact, "M253-E001-C006-04", "C006 must preserve the full five-case binary inspection corpus", failures)

    zero_case = case_map.get("M253-C006-CASE-ZERO-DESCRIPTOR", {})
    zero_names = [section.get("name") for section in zero_case.get("sections", []) if isinstance(section, dict)]
    checks_total += 1
    checks_passed += require(zero_case.get("process_exit_code") == 0 and zero_case.get("backend") == "llvm-direct", artifact, "M253-E001-C006-05", "C006 zero-descriptor case must keep llvm-direct success", failures)
    checks_total += 1
    checks_passed += require(zero_case.get("named_metadata_present") is True, artifact, "M253-E001-C006-06", "C006 zero-descriptor case must preserve named metadata", failures)
    checks_total += 1
    checks_passed += require(zero_names == [
        "objc3.runtime.category_descriptors",
        "objc3.runtime.class_descriptors",
        "objc3.runtime.discovery_root",
        "objc3.runtime.image_info",
        "objc3.runtime.ivar_descriptors",
        "objc3.runtime.linker_anchor",
        "objc3.runtime.property_descriptors",
        "objc3.runtime.protocol_descriptors",
    ], artifact, "M253-E001-C006-07", "C006 zero-descriptor case must preserve the zero-descriptor base section inventory", failures)

    class_case = case_map.get("M253-C006-CASE-CLASS-PROTOCOL-PROPERTY-IVAR", {})
    class_selector = get_section(class_case, "objc3.runtime.selector_pool")
    class_string = get_section(class_case, "objc3.runtime.string_pool")
    class_class = get_section(class_case, "objc3.runtime.class_descriptors")
    class_protocol = get_section(class_case, "objc3.runtime.protocol_descriptors")
    class_property = get_section(class_case, "objc3.runtime.property_descriptors")
    class_ivar = get_section(class_case, "objc3.runtime.ivar_descriptors")
    checks_total += 1
    checks_passed += require(class_case.get("process_exit_code") == 0 and class_case.get("backend") == "llvm-direct", artifact, "M253-E001-C006-08", "C006 class/protocol/property/ivar case must keep llvm-direct success", failures)
    checks_total += 1
    checks_passed += require(class_selector is not None and int(class_selector.get("raw_data_size", 0)) > 0 and class_string is not None and int(class_string.get("raw_data_size", 0)) > 0, artifact, "M253-E001-C006-09", "C006 class/protocol/property/ivar case must preserve selector and string pools", failures)
    checks_total += 1
    checks_passed += require(class_class is not None and int(class_class.get("raw_data_size", 0)) > 8 and class_protocol is not None and int(class_protocol.get("raw_data_size", 0)) > 8 and class_property is not None and int(class_property.get("raw_data_size", 0)) > 8 and class_ivar is not None and int(class_ivar.get("raw_data_size", 0)) > 8, artifact, "M253-E001-C006-10", "C006 class/protocol/property/ivar case must preserve non-trivial metadata payload sections", failures)

    category_case = case_map.get("M253-C006-CASE-CATEGORY-PROTOCOL-PROPERTY", {})
    category_category = get_section(category_case, "objc3.runtime.category_descriptors")
    category_protocol = get_section(category_case, "objc3.runtime.protocol_descriptors")
    category_property = get_section(category_case, "objc3.runtime.property_descriptors")
    checks_total += 1
    checks_passed += require(category_case.get("process_exit_code") == 0 and category_case.get("backend") == "llvm-direct", artifact, "M253-E001-C006-11", "C006 category/protocol/property case must keep llvm-direct success", failures)
    checks_total += 1
    checks_passed += require(category_category is not None and int(category_category.get("raw_data_size", 0)) > 8 and category_protocol is not None and int(category_protocol.get("raw_data_size", 0)) > 8 and category_property is not None and int(category_property.get("raw_data_size", 0)) > 8, artifact, "M253-E001-C006-12", "C006 category/protocol/property case must preserve non-trivial emitted category/protocol/property payloads", failures)

    message_case = case_map.get("M253-C006-CASE-MESSAGE-SEND", {})
    message_selector = get_section(message_case, "objc3.runtime.selector_pool")
    message_string = get_section(message_case, "objc3.runtime.string_pool")
    checks_total += 1
    checks_passed += require(message_case.get("process_exit_code") == 0 and message_case.get("backend") == "llvm-direct", artifact, "M253-E001-C006-13", "C006 message-send case must keep llvm-direct success", failures)
    checks_total += 1
    checks_passed += require(message_selector is not None and int(message_selector.get("raw_data_size", 0)) > 0 and message_string is not None, artifact, "M253-E001-C006-14", "C006 message-send case must preserve pooled selector/string sections", failures)

    negative_case = case_map.get("M253-C006-CASE-NEGATIVE-MISSING-INTERFACE-PROPERTY", {})
    checks_total += 1
    checks_passed += require(negative_case.get("process_exit_code") == 1, artifact, "M253-E001-C006-15", "C006 negative case must still fail compilation", failures)
    checks_total += 1
    checks_passed += require(negative_case.get("inspection_blocked") is True and negative_case.get("manifest_exists") is False and negative_case.get("object_exists") is False, artifact, "M253-E001-C006-16", "C006 negative case must keep object inspection fail-closed", failures)

    return checks_total, checks_passed, {
        "path": artifact,
        "ok": payload.get("ok") is True,
        "dynamic_probes_executed": payload.get("dynamic_probes_executed") is True,
        "case_ids": sorted(case_map),
    }


def validate_d003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M253-E001-D003-01", "D003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M253-E001-D003-02", "D003 summary must report dynamic probes executed", failures)

    case_map = get_case_map(payload)
    expected_case_ids = {
        "M253-D003-CASE-COLLISION-PROVENANCE",
        "M253-D003-CASE-MULTI-ARCHIVE-FANIN",
        "M253-D003-CASE-NEGATIVE-COLLIDING-DISCOVERY-INPUTS",
    }
    checks_total += 1
    checks_passed += require(set(case_map) == expected_case_ids, artifact, "M253-E001-D003-03", "D003 must preserve the full three-case discovery corpus", failures)

    collision_case = case_map.get("M253-D003-CASE-COLLISION-PROVENANCE", {})
    anchor_symbols = collision_case.get("anchor_symbols", [])
    discovery_symbols = collision_case.get("discovery_symbols", [])
    tu_keys = collision_case.get("translation_unit_identity_keys", [])
    checks_total += 1
    checks_passed += require(collision_case.get("compile_exit_codes") == [0, 0], artifact, "M253-E001-D003-04", "D003 collision case must keep both compilation passes green", failures)
    checks_total += 1
    checks_passed += require(len(set(anchor_symbols)) == 2 and len(set(discovery_symbols)) == 2 and len(set(tu_keys)) == 2, artifact, "M253-E001-D003-05", "D003 collision case must preserve distinct anchor/discovery/translation-unit identities", failures)

    fanin_case = case_map.get("M253-D003-CASE-MULTI-ARCHIVE-FANIN", {})
    checks_total += 1
    checks_passed += require(fanin_case.get("object_format") == "coff", artifact, "M253-E001-D003-06", "D003 fan-in case must remain on the COFF host boundary", failures)
    checks_total += 1
    checks_passed += require(fanin_case.get("merge_exit_code") == 0 and fanin_case.get("plain_link_exit_code") == 0 and fanin_case.get("single_link_exit_code") == 0 and fanin_case.get("merged_link_exit_code") == 0, artifact, "M253-E001-D003-07", "D003 fan-in case must preserve the full successful archive merge/link chain", failures)
    checks_total += 1
    checks_passed += require(int(fanin_case.get("merged_retained_metadata_raw_size", 0)) > int(fanin_case.get("single_retained_metadata_raw_size", 0)) > 0, artifact, "M253-E001-D003-08", "D003 fan-in case must preserve a strictly larger merged retained metadata footprint", failures)
    checks_total += 1
    checks_passed += require(len(fanin_case.get("merged_driver_linker_flags", [])) == 2, artifact, "M253-E001-D003-09", "D003 fan-in case must retain one linker flag per archive input", failures)

    negative_case = case_map.get("M253-D003-CASE-NEGATIVE-COLLIDING-DISCOVERY-INPUTS", {})
    checks_total += 1
    checks_passed += require(negative_case.get("compile_exit_code") == 0, artifact, "M253-E001-D003-10", "D003 negative case source must still compile before merge failure", failures)
    checks_total += 1
    checks_passed += require(int(negative_case.get("merge_exit_code", 0)) != 0 and negative_case.get("merged_response_exists") is False and negative_case.get("merged_discovery_exists") is False, artifact, "M253-E001-D003-11", "D003 negative case must remain fail-closed with no merged artifacts", failures)

    return checks_total, checks_passed, {
        "path": artifact,
        "ok": payload.get("ok") is True,
        "dynamic_probes_executed": payload.get("dynamic_probes_executed") is True,
        "case_ids": sorted(case_map),
    }


def serialize_findings(findings: Sequence[Finding]) -> list[dict[str, str]]:
    return [
        {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
        for finding in findings
    ]


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_groups = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    a002_total, a002_passed, a002_details = validate_a002(args.a002_summary, failures)
    checks_total += a002_total
    checks_passed += a002_passed

    b003_total, b003_passed, b003_details = validate_b003(args.b003_summary, failures)
    checks_total += b003_total
    checks_passed += b003_passed

    c006_total, c006_passed, c006_details = validate_c006(args.c006_summary, failures)
    checks_total += c006_total
    checks_passed += c006_passed

    d003_total, d003_passed, d003_details = validate_d003(args.d003_summary, failures)
    checks_total += d003_total
    checks_passed += d003_passed

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": True,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "upstream_evidence": {
            "M253-A002": a002_details,
            "M253-B003": b003_details,
            "M253-C006": c006_details,
            "M253-D003": d003_details,
        },
        "failures": serialize_findings(failures),
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[m253-e001] FAIL {checks_passed}/{checks_total} -> {display_path(args.summary_out)}", file=sys.stderr)
        for failure in failures:
            print(f"  - {failure.check_id} [{failure.artifact}] {failure.detail}", file=sys.stderr)
        return 1

    print(f"[m253-e001] PASS {checks_passed}/{checks_total} -> {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
