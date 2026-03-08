#!/usr/bin/env python3
"""Fail-closed checker for M253-E002 cross-lane object-emission closeout."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-e002-cross-lane-object-emission-gate-and-closeout-v1"
CONTRACT_ID = "objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1"
EVIDENCE_MODEL = "e001-summary-plus-integrated-native-object-emission-probes"
FAILURE_MODEL = "fail-closed-on-summary-or-integrated-probe-drift"
DISCOVERY_CONTRACT_ID = (
    "objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1"
)
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m253_cross_lane_object_emission_gate_and_closeout_e002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m253"
    / "m253_e002_cross_lane_object_emission_gate_and_closeout_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_RUNNER_SCRIPT = ROOT / "scripts" / "run_m253_e002_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_A002_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-A002"
    / "source_to_section_mapping_completeness_matrix_summary.json"
)
DEFAULT_B003_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-B003"
    / "coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json"
)
DEFAULT_C006_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-C006"
    / "binary_inspection_harness_summary.json"
)
DEFAULT_D003_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-D003"
    / "archive_and_static_link_metadata_discovery_behavior_summary.json"
)
DEFAULT_E001_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-E001"
    / "metadata_emission_gate_summary.json"
)
DEFAULT_PROBE_ROOT = (
    ROOT
    / "tmp"
    / "artifacts"
    / "compilation"
    / "objc3c-native"
    / "m253"
    / "e002-object-emission-closeout"
)
DEFAULT_SUMMARY_OUT = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-E002"
    / "cross_lane_object_emission_gate_and_closeout_summary.json"
)
DEFAULT_CLASS_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_CATEGORY_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
)
DEFAULT_MESSAGE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "execution"
    / "positive"
    / "message_send_runtime_shim.objc3"
)
DEFAULT_NEGATIVE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m252_b004_missing_interface_property.objc3"
)

IR_REQUIRED_SNIPPETS = (
    "!objc3.objc_runtime_metadata_emission_gate",
    "!objc3.objc_runtime_metadata_object_emission_closeout",
    "; runtime_metadata_emission_gate = ",
    "; runtime_metadata_object_emission_closeout = ",
    "source_to_section_matrix_issue=M253-A002",
    "object_format_policy_issue=M253-B003",
    "binary_inspection_issue=M253-C006",
    "archive_static_link_issue=M253-D003",
    "dependency_gate_issue=M253-E001",
    "class_object_case_issue=M253-A002",
    "binary_object_case_issue=M253-C006",
    "linker_fanin_issue=M253-D003",
)

COMMON_RETAINED_SYMBOLS = (
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
)


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
    SnippetCheck(
        "M253-E002-DOC-EXP-01",
        "# M253 Cross-Lane Object Emission Gate And Closeout Expectations (E002)",
    ),
    SnippetCheck("M253-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck(
        "M253-E002-DOC-EXP-03",
        "`class-protocol-property-ivar-object-closeout`",
    ),
    SnippetCheck(
        "M253-E002-DOC-EXP-04",
        "`fanin-distinct-linker-discovery-closeout`",
    ),
    SnippetCheck(
        "M253-E002-DOC-EXP-05",
        "`scripts/run_m253_e002_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M253-E002-DOC-EXP-06",
        "`tmp/reports/m253/M253-E002/cross_lane_object_emission_gate_and_closeout_summary.json`",
    ),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M253-E002-DOC-PKT-01",
        "# M253-E002 Cross-Lane Object Emission Gate And Closeout Packet",
    ),
    SnippetCheck("M253-E002-DOC-PKT-02", "Packet: `M253-E002`"),
    SnippetCheck("M253-E002-DOC-PKT-03", "Issue: `#7100`"),
    SnippetCheck("M253-E002-DOC-PKT-04", "- `M253-A002`"),
    SnippetCheck("M253-E002-DOC-PKT-05", "- `M253-D003`"),
    SnippetCheck("M253-E002-DOC-PKT-06", "- `M253-E001`"),
    SnippetCheck(
        "M253-E002-DOC-PKT-07",
        "`scripts/check_m253_e002_cross_lane_object_emission_gate_and_closeout.py`",
    ),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck(
        "M253-E002-ARCH-01",
        "M253 lane-E E002 cross-lane object-emission closeout anchors explicit",
    ),
    SnippetCheck(
        "M253-E002-ARCH-02",
        "same class/category/message-send native objects satisfy `M253-A002`,",
    ),
    SnippetCheck("M253-E002-ARCH-03", "startup-registration work begins."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M253-E002-NDOC-01",
        "## Cross-lane object-emission gate and closeout (M253-E002)",
    ),
    SnippetCheck(
        "M253-E002-NDOC-02",
        "`Objc3RuntimeMetadataObjectEmissionCloseoutSummary`",
    ),
    SnippetCheck(
        "M253-E002-NDOC-03",
        "`fanin-distinct-linker-discovery-closeout`",
    ),
    SnippetCheck(
        "M253-E002-NDOC-04",
        "`scripts/run_m253_e002_lane_e_readiness.py`",
    ),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M253-E002-SPC-01",
        "## M253 cross-lane object-emission gate and closeout (E002)",
    ),
    SnippetCheck("M253-E002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M253-E002-SPC-03",
        "`!objc3.objc_runtime_metadata_object_emission_closeout`",
    ),
    SnippetCheck(
        "M253-E002-SPC-04",
        "blocks object/discovery emission.",
    ),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M253-E002-META-01",
        "## M253 cross-lane object-emission closeout metadata anchors (E002)",
    ),
    SnippetCheck("M253-E002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M253-E002-META-03",
        "`!objc3.objc_runtime_metadata_object_emission_closeout`",
    ),
    SnippetCheck(
        "M253-E002-META-04",
        "`tmp/reports/m253/M253-E002/cross_lane_object_emission_gate_and_closeout_summary.json`",
    ),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck(
        "M253-E002-LHDR-01",
        "kObjc3RuntimeMetadataObjectEmissionCloseoutContractId",
    ),
    SnippetCheck(
        "M253-E002-LHDR-02",
        "kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel",
    ),
    SnippetCheck(
        "M253-E002-LHDR-03",
        "kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel",
    ),
    SnippetCheck(
        "M253-E002-LHDR-04",
        "Objc3RuntimeMetadataObjectEmissionCloseoutSummary();",
    ),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck(
        "M253-E002-LCPP-01",
        "std::string Objc3RuntimeMetadataObjectEmissionCloseoutSummary() {",
    ),
    SnippetCheck(
        "M253-E002-LCPP-02",
        "fresh integrated native object probes",
    ),
    SnippetCheck(
        "M253-E002-LCPP-03",
        "non_goals=no-startup-registration-or-runtime-bootstrap",
    ),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck(
        "M253-E002-IR-01",
        '!objc3.objc_runtime_metadata_object_emission_closeout = !{!65}',
    ),
    SnippetCheck(
        "M253-E002-IR-02",
        "; runtime_metadata_object_emission_closeout = ",
    ),
    SnippetCheck("M253-E002-IR-03", "dependency_gate_issue=M253-E001"),
    SnippetCheck("M253-E002-IR-04", "linker_fanin_issue=M253-D003"),
)
PROCESS_SNIPPETS = (
    SnippetCheck(
        "M253-E002-PROC-01",
        "M253-E002 cross-lane object-emission closeout anchor",
    ),
    SnippetCheck(
        "M253-E002-PROC-02",
        "later startup-registration work to trust the produced objects",
    ),
)
RUNNER_SNIPPETS = (
    SnippetCheck(
        "M253-E002-RUN-01",
        '"build:objc3c-native"',
    ),
    SnippetCheck(
        "M253-E002-RUN-02",
        '"check:objc3c:m253-a002-lane-a-readiness"',
    ),
    SnippetCheck(
        "M253-E002-RUN-03",
        '"check:objc3c:m253-d003-lane-d-readiness"',
    ),
    SnippetCheck(
        "M253-E002-RUN-04",
        '"scripts/check_m253_e001_metadata_emission_gate.py"',
    ),
    SnippetCheck(
        "M253-E002-RUN-05",
        '"scripts/check_m253_e002_cross_lane_object_emission_gate_and_closeout.py"',
    ),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M253-E002-PKG-01",
        '"check:objc3c:m253-e002-cross-lane-object-emission-gate-and-closeout": "python scripts/check_m253_e002_cross_lane_object_emission_gate_and_closeout.py"',
    ),
    SnippetCheck(
        "M253-E002-PKG-02",
        '"test:tooling:m253-e002-cross-lane-object-emission-gate-and-closeout": "python -m pytest tests/tooling/test_check_m253_e002_cross_lane_object_emission_gate_and_closeout.py -q"',
    ),
    SnippetCheck(
        "M253-E002-PKG-03",
        '"check:objc3c:m253-e002-lane-e-readiness": "python scripts/run_m253_e002_lane_e_readiness.py"',
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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(
    condition: bool,
    artifact: str,
    check_id: str,
    detail: str,
    failures: list[Finding],
) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_doc_contract(
    *, path: Path, exists_check_id: str, snippets: Sequence[SnippetCheck]
) -> tuple[int, list[Finding]]:
    checks_total = 1
    failures: list[Finding] = []
    if not path.exists():
        failures.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required artifact is missing: {display_path(path)}",
            )
        )
        return checks_total, failures
    text = read_text(path)
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            failures.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, failures


def load_json_payload(
    path: Path, *, exists_check_id: str, parse_check_id: str
) -> tuple[int, list[Finding], dict[str, Any] | None]:
    checks_total = 2
    failures: list[Finding] = []
    if not path.exists():
        failures.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required JSON payload is missing: {display_path(path)}",
            )
        )
        return checks_total, failures, None
    try:
        payload = json.loads(read_text(path))
    except json.JSONDecodeError as exc:
        failures.append(
            Finding(display_path(path), parse_check_id, f"invalid JSON: {exc}")
        )
        return checks_total, failures, None
    if not isinstance(payload, dict):
        failures.append(
            Finding(
                display_path(path),
                parse_check_id,
                "JSON payload must be an object",
            )
        )
        return checks_total, failures, None
    return checks_total, failures, payload


def resolve_tool(tool_name: str) -> Path | None:
    env_bin = os.environ.get("LLVM_BIN_DIR")
    if env_bin:
        candidate = Path(env_bin) / tool_name
        if candidate.exists():
            return candidate.resolve()
    program_files = os.environ.get("ProgramFiles")
    if program_files:
        candidate = Path(program_files) / "LLVM" / "bin" / tool_name
        if candidate.exists():
            return candidate.resolve()
    resolved = shutil.which(tool_name)
    if resolved:
        return Path(resolved).resolve()
    return None


def run_command(
    command: Sequence[str], cwd: Path
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def find_first_key(payload: object, key: str) -> object:
    if isinstance(payload, dict):
        if key in payload:
            return payload[key]
        for value in payload.values():
            found = find_first_key(value, key)
            if found is not None:
                return found
    elif isinstance(payload, list):
        for value in payload:
            found = find_first_key(value, key)
            if found is not None:
                return found
    return None


def load_diagnostics(path: Path) -> list[dict[str, Any]]:
    if not path.exists():
        return []
    payload = json.loads(read_text(path))
    if isinstance(payload, list):
        return [entry for entry in payload if isinstance(entry, dict)]
    diagnostics = payload.get("diagnostics") if isinstance(payload, dict) else None
    if isinstance(diagnostics, list):
        return [entry for entry in diagnostics if isinstance(entry, dict)]
    raise TypeError(f"expected diagnostics payload in {display_path(path)}")


def extract_sections(readobj_stdout: str) -> list[dict[str, int | str]]:
    sections: list[dict[str, int | str]] = []
    for block in readobj_stdout.split("Section {"):
        name_match = re.search(r"Name: ([^\s(]+)", block)
        if not name_match:
            continue
        size_match = re.search(r"RawDataSize: (\d+)", block)
        reloc_match = re.search(r"RelocationCount: (\d+)", block)
        sections.append(
            {
                "name": name_match.group(1),
                "raw_data_size": int(size_match.group(1)) if size_match else 0,
                "relocation_count": int(reloc_match.group(1)) if reloc_match else 0,
            }
        )
    return sections


def sections_by_name(
    sections: Sequence[dict[str, Any]]
) -> dict[str, dict[str, Any]]:
    return {
        str(section["name"]): section
        for section in sections
        if isinstance(section, dict) and isinstance(section.get("name"), str)
    }


def case_map(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    dynamic_cases = payload.get("dynamic_cases", [])
    if isinstance(dynamic_cases, list):
        return {
            str(case.get("case_id")): case
            for case in dynamic_cases
            if isinstance(case, dict) and isinstance(case.get("case_id"), str)
        }
    return {}


def metadata_sections(
    sections: Sequence[dict[str, Any]]
) -> list[dict[str, Any]]:
    return [
        section
        for section in sections
        if isinstance(section, dict)
        and isinstance(section.get("name"), str)
        and str(section["name"]).startswith("objc3.runtime.")
    ]


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
    parser.add_argument("--runner-script", type=Path, default=DEFAULT_RUNNER_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=DEFAULT_B003_SUMMARY)
    parser.add_argument("--c006-summary", type=Path, default=DEFAULT_C006_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--e001-summary", type=Path, default=DEFAULT_E001_SUMMARY)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--message-fixture", type=Path, default=DEFAULT_MESSAGE_FIXTURE)
    parser.add_argument("--negative-fixture", type=Path, default=DEFAULT_NEGATIVE_FIXTURE)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def validate_dependencies(
    args: argparse.Namespace, failures: list[Finding]
) -> tuple[int, int, dict[str, dict[str, Any]]]:
    checks_total = 0
    checks_passed = 0
    loaded: dict[str, dict[str, Any]] = {}
    for label, path in (
        ("M253-A002", args.a002_summary),
        ("M253-B003", args.b003_summary),
        ("M253-C006", args.c006_summary),
        ("M253-D003", args.d003_summary),
        ("M253-E001", args.e001_summary),
    ):
        count, load_failures, payload = load_json_payload(
            path,
            exists_check_id=f"{label}-SUM-EXISTS",
            parse_check_id=f"{label}-SUM-PARSE",
        )
        checks_total += count
        checks_passed += count - len(load_failures)
        failures.extend(load_failures)
        if payload is not None:
            loaded[label] = payload

    a002 = loaded.get("M253-A002")
    if a002 is not None:
        checks_total += 4
        checks_passed += require(
            a002.get("ok") is True,
            display_path(args.a002_summary),
            "M253-E002-A002-OK",
            "A002 summary must report ok=true",
            failures,
        )
        checks_passed += require(
            a002.get("dynamic_probes_executed") is True,
            display_path(args.a002_summary),
            "M253-E002-A002-DYN",
            "A002 summary must report dynamic probes executed",
            failures,
        )
        dynamic_cases = a002.get("dynamic_cases", {})
        class_graph = (
            dynamic_cases.get("class_fixture", {}).get("graph", {})
            if isinstance(dynamic_cases, dict)
            else {}
        )
        category_graph = (
            dynamic_cases.get("category_fixture", {}).get("graph", {})
            if isinstance(dynamic_cases, dict)
            else {}
        )
        checks_passed += require(
            class_graph.get("source_graph_complete") is True
            and int(class_graph.get("class_nodes", 0)) >= 1,
            display_path(args.a002_summary),
            "M253-E002-A002-CLASS",
            "A002 class graph expectations drifted",
            failures,
        )
        checks_passed += require(
            category_graph.get("source_graph_complete") is True
            and int(category_graph.get("category_nodes", 0)) >= 1,
            display_path(args.a002_summary),
            "M253-E002-A002-CATEGORY",
            "A002 category graph expectations drifted",
            failures,
        )

    b003 = loaded.get("M253-B003")
    if b003 is not None:
        checks_total += 4
        host_policy = b003.get("host_policy", {})
        first_case = case_map(b003).get("M253-B003-CASE-NATIVE", {})
        checks_passed += require(
            b003.get("ok") is True,
            display_path(args.b003_summary),
            "M253-E002-B003-OK",
            "B003 summary must report ok=true",
            failures,
        )
        checks_passed += require(
            b003.get("dynamic_probes_executed") is True,
            display_path(args.b003_summary),
            "M253-E002-B003-DYN",
            "B003 summary must report dynamic probes executed",
            failures,
        )
        checks_passed += require(
            isinstance(host_policy, dict)
            and str(host_policy.get("object_format")) != "",
            display_path(args.b003_summary),
            "M253-E002-B003-POLICY",
            "B003 host policy must publish an object format",
            failures,
        )
        checks_passed += require(
            first_case.get("detected_object_format") == host_policy.get("object_format"),
            display_path(args.b003_summary),
            "M253-E002-B003-FORMAT",
            "B003 detected object format must match host policy",
            failures,
        )

    c006 = loaded.get("M253-C006")
    if c006 is not None:
        checks_total += 4
        c006_cases = case_map(c006)
        checks_passed += require(
            c006.get("ok") is True,
            display_path(args.c006_summary),
            "M253-E002-C006-OK",
            "C006 summary must report ok=true",
            failures,
        )
        checks_passed += require(
            c006.get("dynamic_probes_executed") is True,
            display_path(args.c006_summary),
            "M253-E002-C006-DYN",
            "C006 summary must report dynamic probes executed",
            failures,
        )
        checks_passed += require(
            "M253-C006-CASE-CLASS-PROTOCOL-PROPERTY-IVAR" in c006_cases,
            display_path(args.c006_summary),
            "M253-E002-C006-CLASS",
            "C006 class inspection case is missing",
            failures,
        )
        checks_passed += require(
            "M253-C006-CASE-NEGATIVE-MISSING-INTERFACE-PROPERTY" in c006_cases,
            display_path(args.c006_summary),
            "M253-E002-C006-NEGATIVE",
            "C006 negative inspection case is missing",
            failures,
        )

    d003 = loaded.get("M253-D003")
    if d003 is not None:
        checks_total += 4
        d003_cases = case_map(d003)
        checks_passed += require(
            d003.get("ok") is True,
            display_path(args.d003_summary),
            "M253-E002-D003-OK",
            "D003 summary must report ok=true",
            failures,
        )
        checks_passed += require(
            d003.get("dynamic_probes_executed") is True,
            display_path(args.d003_summary),
            "M253-E002-D003-DYN",
            "D003 summary must report dynamic probes executed",
            failures,
        )
        checks_passed += require(
            "M253-D003-CASE-MULTI-ARCHIVE-FANIN" in d003_cases,
            display_path(args.d003_summary),
            "M253-E002-D003-FANIN",
            "D003 multi-archive fan-in case is missing",
            failures,
        )
        checks_passed += require(
            "M253-D003-CASE-COLLISION-PROVENANCE" in d003_cases,
            display_path(args.d003_summary),
            "M253-E002-D003-COLLISION",
            "D003 collision provenance case is missing",
            failures,
        )

    e001 = loaded.get("M253-E001")
    if e001 is not None:
        checks_total += 4
        checks_passed += require(
            e001.get("ok") is True,
            display_path(args.e001_summary),
            "M253-E002-E001-OK",
            "E001 summary must report ok=true",
            failures,
        )
        checks_passed += require(
            e001.get("dynamic_probes_executed") is True,
            display_path(args.e001_summary),
            "M253-E002-E001-DYN",
            "E001 summary must report dynamic probes executed",
            failures,
        )
        checks_passed += require(
            e001.get("next_closeout_issue") == "M253-E002",
            display_path(args.e001_summary),
            "M253-E002-E001-NEXT",
            "E001 summary must hand off to M253-E002",
            failures,
        )
        checks_passed += require(
            e001.get("evidence_model") == "a002-b003-c006-d003-summary-chain",
            display_path(args.e001_summary),
            "M253-E002-E001-MODEL",
            "E001 summary evidence model drifted",
            failures,
        )

    return checks_total, checks_passed, loaded


def run_positive_case(
    *,
    native_exe: Path,
    fixture: Path,
    out_dir: Path,
    llvm_readobj: Path,
    llvm_objdump: Path,
    case_id: str,
    graph_expectations: dict[str, Any] | None,
    c006_case: dict[str, Any],
    expected_object_format: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = display_path(fixture)
    out_dir.mkdir(parents=True, exist_ok=True)

    command = [
        str(native_exe.resolve()),
        str(fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    object_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    binary_path = out_dir / "module.runtime-metadata.bin"
    discovery_path = out_dir / "module.runtime-metadata-discovery.json"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    diagnostics_path = out_dir / "module.diagnostics.json"

    for check_id, path in (
        (f"{case_id}-MANIFEST", manifest_path),
        (f"{case_id}-IR", ir_path),
        (f"{case_id}-OBJECT", object_path),
        (f"{case_id}-BACKEND", backend_path),
        (f"{case_id}-BINARY", binary_path),
        (f"{case_id}-DISCOVERY", discovery_path),
        (f"{case_id}-RSP", rsp_path),
    ):
        checks_total += 1
        checks_passed += require(
            path.exists(),
            artifact,
            check_id,
            f"required output is missing: {display_path(path)}",
            failures,
        )

    checks_total += 1
    checks_passed += require(
        result.returncode == 0,
        artifact,
        f"{case_id}-EXIT",
        f"native compile exited with {result.returncode}",
        failures,
    )

    if any(
        not path.exists()
        for path in (
            manifest_path,
            ir_path,
            object_path,
            backend_path,
            discovery_path,
            rsp_path,
        )
    ):
        return checks_total, checks_passed, {
            "case_id": case_id,
            "fixture": display_path(fixture),
            "status": 1,
            "success": False,
            "command": command,
            "stdout": result.stdout,
            "stderr": result.stderr,
        }

    manifest = json.loads(read_text(manifest_path))
    ir_text = read_text(ir_path)
    backend = read_text(backend_path).strip()
    diagnostics = load_diagnostics(diagnostics_path)
    discovery = json.loads(read_text(discovery_path))
    rsp_line = read_text(rsp_path).strip()

    checks_total += 4
    checks_passed += require(
        backend == "llvm-direct",
        display_path(backend_path),
        f"{case_id}-LLVM-DIRECT",
        f"expected llvm-direct backend, observed {backend!r}",
        failures,
    )
    checks_passed += require(
        diagnostics == [],
        display_path(diagnostics_path),
        f"{case_id}-DIAGNOSTICS",
        "positive case must remain diagnostic-clean",
        failures,
    )
    checks_passed += require(
        isinstance(discovery, dict),
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-JSON",
        "discovery sidecar must be a JSON object",
        failures,
    )
    checks_passed += require(
        rsp_line != "",
        display_path(rsp_path),
        f"{case_id}-RSP-NONEMPTY",
        "linker response sidecar must not be empty",
        failures,
    )

    checks_total += len(IR_REQUIRED_SNIPPETS)
    for index, snippet in enumerate(IR_REQUIRED_SNIPPETS, start=1):
        checks_passed += require(
            snippet in ir_text,
            display_path(ir_path),
            f"{case_id}-IR-{index:02d}",
            f"IR is missing required snippet: {snippet}",
            failures,
        )

    checks_total += 7
    checks_passed += require(
        discovery.get("contract_id") == DISCOVERY_CONTRACT_ID,
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-CONTRACT",
        "discovery contract id drifted",
        failures,
    )
    checks_passed += require(
        discovery.get("object_format") == expected_object_format,
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-FORMAT",
        "discovery object format drifted",
        failures,
    )
    checks_passed += require(
        discovery.get("object_artifact") == "module.obj",
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-ARTIFACT",
        "discovery object artifact drifted",
        failures,
    )
    checks_passed += require(
        discovery.get("linker_anchor_emitted_section") == "objc3.runtime.linker_anchor"
        and discovery.get("discovery_root_emitted_section")
        == "objc3.runtime.discovery_root",
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-SECTIONS",
        "discovery emitted section names drifted",
        failures,
    )
    checks_passed += require(
        discovery.get("translation_unit_identity_model")
        == TRANSLATION_UNIT_IDENTITY_MODEL,
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-IDENTITY-MODEL",
        "translation-unit identity model drifted",
        failures,
    )
    driver_flags = discovery.get("driver_linker_flags")
    checks_passed += require(
        isinstance(driver_flags, list) and len(driver_flags) == 1,
        display_path(discovery_path),
        f"{case_id}-DISCOVERY-FLAGS",
        "discovery driver linker flags must publish exactly one flag",
        failures,
    )
    checks_passed += require(
        isinstance(driver_flags, list) and driver_flags and driver_flags[0] == rsp_line,
        display_path(rsp_path),
        f"{case_id}-RSP-MATCH",
        "linker response payload must match discovery driver flag",
        failures,
    )

    readobj_result = run_command(
        [str(llvm_readobj), "--sections", str(object_path)], ROOT
    )
    objdump_result = run_command([str(llvm_objdump), "--syms", str(object_path)], ROOT)
    actual_sections = extract_sections(readobj_result.stdout)
    actual_metadata_sections = metadata_sections(actual_sections)
    expected_sections = [
        {
            "name": str(section.get("name")),
            "raw_data_size": int(section.get("raw_data_size", 0)),
            "relocation_count": int(section.get("relocation_count", 0)),
        }
        for section in c006_case.get("sections", [])
        if isinstance(section, dict)
    ]
    actual_section_map = sections_by_name(actual_metadata_sections)
    expected_section_map = sections_by_name(expected_sections)

    checks_total += 4
    checks_passed += require(
        readobj_result.returncode == 0,
        display_path(object_path),
        f"{case_id}-READOBJ",
        "llvm-readobj failed for integrated object output",
        failures,
    )
    checks_passed += require(
        objdump_result.returncode == 0,
        display_path(object_path),
        f"{case_id}-OBJDUMP",
        "llvm-objdump failed for integrated object output",
        failures,
    )
    checks_passed += require(
        actual_section_map == expected_section_map,
        display_path(object_path),
        f"{case_id}-SECTIONS",
        "section inventory drifted from C006 evidence",
        failures,
    )
    checks_passed += require(
        all(symbol in objdump_result.stdout for symbol in COMMON_RETAINED_SYMBOLS),
        display_path(object_path),
        f"{case_id}-COMMON-SYMBOLS",
        "common retained metadata symbols drifted from object inventory",
        failures,
    )

    tracked_symbol_offsets = (
        c006_case.get("tracked_symbol_offsets", {})
        if isinstance(c006_case.get("tracked_symbol_offsets"), dict)
        else {}
    )
    checks_total += len(tracked_symbol_offsets) + 2
    for symbol_name in tracked_symbol_offsets:
        checks_passed += require(
            symbol_name in objdump_result.stdout,
            display_path(object_path),
            f"{case_id}-TRACKED-{symbol_name}",
            f"tracked symbol missing from objdump output: {symbol_name}",
            failures,
        )
    checks_passed += require(
        str(discovery.get("linker_anchor_symbol", "")) in objdump_result.stdout,
        display_path(object_path),
        f"{case_id}-ANCHOR-SYMBOL",
        "linker anchor symbol missing from objdump output",
        failures,
    )
    checks_passed += require(
        str(discovery.get("discovery_root_symbol", "")) in objdump_result.stdout,
        display_path(object_path),
        f"{case_id}-DISCOVERY-SYMBOL",
        "discovery root symbol missing from objdump output",
        failures,
    )

    if graph_expectations is not None:
        checks_total += 6
        checks_passed += require(
            find_first_key(manifest, "source_graph_complete") is True,
            display_path(manifest_path),
            f"{case_id}-GRAPH-COMPLETE",
            "manifest source graph is no longer complete",
            failures,
        )
        for graph_key in (
            "class_nodes",
            "category_nodes",
            "metaclass_nodes",
            "protocol_nodes",
            "property_nodes",
            "ivar_nodes",
        ):
            expected_value = int(graph_expectations.get(graph_key, 0))
            actual_value = int(find_first_key(manifest, graph_key) or 0)
            checks_passed += require(
                actual_value == expected_value,
                display_path(manifest_path),
                f"{case_id}-GRAPH-{graph_key.upper()}",
                f"manifest {graph_key} drifted: expected {expected_value}, observed {actual_value}",
                failures,
            )

    return checks_total, checks_passed, {
        "case_id": case_id,
        "fixture": display_path(fixture),
        "command": command,
        "process_exit_code": result.returncode,
        "backend": backend,
        "sections": actual_metadata_sections,
        "tracked_symbols": sorted(tracked_symbol_offsets.keys()),
        "linker_anchor_symbol": discovery.get("linker_anchor_symbol"),
        "discovery_root_symbol": discovery.get("discovery_root_symbol"),
        "driver_linker_flag": rsp_line,
        "translation_unit_identity_key": discovery.get("translation_unit_identity_key"),
        "status": 0 if result.returncode == 0 and not diagnostics else 1,
        "success": result.returncode == 0 and diagnostics == [],
    }


def run_negative_case(
    *,
    native_exe: Path,
    fixture: Path,
    out_dir: Path,
    c006_case: dict[str, Any],
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = display_path(fixture)
    out_dir.mkdir(parents=True, exist_ok=True)

    command = [
        str(native_exe.resolve()),
        str(fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    object_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    binary_path = out_dir / "module.runtime-metadata.bin"
    discovery_path = out_dir / "module.runtime-metadata-discovery.json"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    diagnostics_path = out_dir / "module.diagnostics.json"
    diagnostics_txt_path = out_dir / "module.diagnostics.txt"

    diagnostics = load_diagnostics(diagnostics_path)
    observed_codes = [
        entry.get("code") for entry in diagnostics if isinstance(entry.get("code"), str)
    ]

    checks_total += 11
    checks_passed += require(
        result.returncode == 1,
        artifact,
        "M253-E002-NEGATIVE-EXIT",
        f"negative compile must exit 1, observed {result.returncode}",
        failures,
    )
    checks_passed += require(
        diagnostics_txt_path.exists(),
        artifact,
        "M253-E002-NEGATIVE-DIAG-TXT",
        "negative compile must publish diagnostics text",
        failures,
    )
    checks_passed += require(
        diagnostics_path.exists(),
        artifact,
        "M253-E002-NEGATIVE-DIAG-JSON",
        "negative compile must publish diagnostics json",
        failures,
    )
    checks_passed += require(
        observed_codes == ["O3S206"],
        display_path(diagnostics_path),
        "M253-E002-NEGATIVE-O3S206",
        f"expected ['O3S206'], observed {observed_codes}",
        failures,
    )
    for check_id, path in (
        ("M253-E002-NEGATIVE-MANIFEST", manifest_path),
        ("M253-E002-NEGATIVE-IR", ir_path),
        ("M253-E002-NEGATIVE-OBJECT", object_path),
        ("M253-E002-NEGATIVE-BACKEND", backend_path),
        ("M253-E002-NEGATIVE-BINARY", binary_path),
        ("M253-E002-NEGATIVE-DISCOVERY", discovery_path),
        ("M253-E002-NEGATIVE-RSP", rsp_path),
    ):
        checks_passed += require(
            not path.exists(),
            artifact,
            check_id,
            f"negative compile must not emit {display_path(path)}",
            failures,
        )

    checks_total += 2
    checks_passed += require(
        c006_case.get("inspection_blocked") is True,
        display_path(DEFAULT_C006_SUMMARY),
        "M253-E002-NEGATIVE-C006-BLOCKED",
        "C006 negative case must remain inspection-blocked",
        failures,
    )
    checks_passed += require(
        c006_case.get("inspection_block_reason") == "compile-failed-before-object-emission",
        display_path(DEFAULT_C006_SUMMARY),
        "M253-E002-NEGATIVE-C006-REASON",
        "C006 negative case block reason drifted",
        failures,
    )

    return checks_total, checks_passed, {
        "case_id": "negative-missing-interface-property-closeout",
        "fixture": display_path(fixture),
        "command": command,
        "process_exit_code": result.returncode,
        "observed_codes": observed_codes,
        "status": 0 if result.returncode == 1 and observed_codes == ["O3S206"] else 1,
        "success": result.returncode == 1 and observed_codes == ["O3S206"],
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M253-E002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M253-E002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M253-E002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M253-E002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M253-E002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M253-E002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.lowering_header, "M253-E002-LHDR-EXISTS", LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, "M253-E002-LCPP-EXISTS", LOWERING_CPP_SNIPPETS),
        (args.ir_emitter, "M253-E002-IR-EXISTS", IR_EMITTER_SNIPPETS),
        (args.process_cpp, "M253-E002-PROC-EXISTS", PROCESS_SNIPPETS),
        (args.runner_script, "M253-E002-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.package_json, "M253-E002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, doc_failures = check_doc_contract(
            path=path, exists_check_id=exists_check_id, snippets=snippets
        )
        checks_total += count
        checks_passed += count - len(doc_failures)
        failures.extend(doc_failures)

    dependency_total, dependency_passed, loaded = validate_dependencies(args, failures)
    checks_total += dependency_total
    checks_passed += dependency_passed

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_tools: dict[str, str] = {}
    dynamic_probes_executed = not args.skip_dynamic_probes
    closeout_ready_for_startup_registration = False

    if not args.skip_dynamic_probes:
        llvm_readobj = resolve_tool("llvm-readobj.exe")
        llvm_objdump = resolve_tool("llvm-objdump.exe")

        checks_total += 3
        checks_passed += require(
            args.native_exe.exists(),
            display_path(args.native_exe),
            "M253-E002-DYN-NATIVE",
            "native executable is missing",
            failures,
        )
        checks_passed += require(
            llvm_readobj is not None,
            "dynamic",
            "M253-E002-DYN-READOBJ",
            "llvm-readobj.exe not found",
            failures,
        )
        checks_passed += require(
            llvm_objdump is not None,
            "dynamic",
            "M253-E002-DYN-OBJDUMP",
            "llvm-objdump.exe not found",
            failures,
        )

        if (
            args.native_exe.exists()
            and llvm_readobj is not None
            and llvm_objdump is not None
            and {"M253-A002", "M253-B003", "M253-C006", "M253-D003", "M253-E001"}
            <= loaded.keys()
        ):
            dynamic_tools = {
                "llvm_readobj": display_path(llvm_readobj),
                "llvm_objdump": display_path(llvm_objdump),
            }
            a002_dynamic = loaded["M253-A002"].get("dynamic_cases", {})
            a002_class_graph = (
                a002_dynamic.get("class_fixture", {}).get("graph", {})
                if isinstance(a002_dynamic, dict)
                else {}
            )
            a002_category_graph = (
                a002_dynamic.get("category_fixture", {}).get("graph", {})
                if isinstance(a002_dynamic, dict)
                else {}
            )
            b003_host_policy = loaded["M253-B003"].get("host_policy", {})
            expected_object_format = str(b003_host_policy.get("object_format", ""))
            c006_cases = case_map(loaded["M253-C006"])
            d003_cases = case_map(loaded["M253-D003"])

            class_total, class_passed, class_case = run_positive_case(
                native_exe=args.native_exe,
                fixture=args.class_fixture,
                out_dir=args.probe_root / "class-object-closeout",
                llvm_readobj=llvm_readobj,
                llvm_objdump=llvm_objdump,
                case_id="class-protocol-property-ivar-object-closeout",
                graph_expectations=a002_class_graph,
                c006_case=c006_cases["M253-C006-CASE-CLASS-PROTOCOL-PROPERTY-IVAR"],
                expected_object_format=expected_object_format,
                failures=failures,
            )
            checks_total += class_total
            checks_passed += class_passed
            dynamic_cases.append(class_case)

            category_total, category_passed, category_case = run_positive_case(
                native_exe=args.native_exe,
                fixture=args.category_fixture,
                out_dir=args.probe_root / "category-object-closeout",
                llvm_readobj=llvm_readobj,
                llvm_objdump=llvm_objdump,
                case_id="category-protocol-property-object-closeout",
                graph_expectations=a002_category_graph,
                c006_case=c006_cases["M253-C006-CASE-CATEGORY-PROTOCOL-PROPERTY"],
                expected_object_format=expected_object_format,
                failures=failures,
            )
            checks_total += category_total
            checks_passed += category_passed
            dynamic_cases.append(category_case)

            message_total, message_passed, message_case = run_positive_case(
                native_exe=args.native_exe,
                fixture=args.message_fixture,
                out_dir=args.probe_root / "message-send-object-closeout",
                llvm_readobj=llvm_readobj,
                llvm_objdump=llvm_objdump,
                case_id="message-send-object-closeout",
                graph_expectations=None,
                c006_case=c006_cases["M253-C006-CASE-MESSAGE-SEND"],
                expected_object_format=expected_object_format,
                failures=failures,
            )
            checks_total += message_total
            checks_passed += message_passed
            dynamic_cases.append(message_case)

            negative_total, negative_passed, negative_case = run_negative_case(
                native_exe=args.native_exe,
                fixture=args.negative_fixture,
                out_dir=args.probe_root / "negative-missing-interface-property-closeout",
                c006_case=c006_cases["M253-C006-CASE-NEGATIVE-MISSING-INTERFACE-PROPERTY"],
                failures=failures,
            )
            checks_total += negative_total
            checks_passed += negative_passed
            dynamic_cases.append(negative_case)

            checks_total += 6
            class_symbol = str(class_case.get("linker_anchor_symbol", ""))
            category_symbol = str(category_case.get("linker_anchor_symbol", ""))
            class_root = str(class_case.get("discovery_root_symbol", ""))
            category_root = str(category_case.get("discovery_root_symbol", ""))
            class_flag = str(class_case.get("driver_linker_flag", ""))
            category_flag = str(category_case.get("driver_linker_flag", ""))
            class_identity = str(class_case.get("translation_unit_identity_key", ""))
            category_identity = str(
                category_case.get("translation_unit_identity_key", "")
            )
            d003_fanin = d003_cases.get("M253-D003-CASE-MULTI-ARCHIVE-FANIN", {})

            checks_passed += require(
                class_symbol != "" and class_symbol != category_symbol,
                "dynamic",
                "M253-E002-FANIN-ANCHOR",
                "positive probes must publish distinct linker anchor symbols",
                failures,
            )
            checks_passed += require(
                class_root != "" and class_root != category_root,
                "dynamic",
                "M253-E002-FANIN-ROOT",
                "positive probes must publish distinct discovery root symbols",
                failures,
            )
            checks_passed += require(
                class_flag != "" and class_flag != category_flag,
                "dynamic",
                "M253-E002-FANIN-FLAG",
                "positive probes must publish distinct linker flags",
                failures,
            )
            checks_passed += require(
                class_identity != "" and class_identity != category_identity,
                "dynamic",
                "M253-E002-FANIN-IDENTITY",
                "positive probes must publish distinct translation-unit identity keys",
                failures,
            )
            checks_passed += require(
                d003_fanin.get("object_format") == expected_object_format,
                display_path(args.d003_summary),
                "M253-E002-FANIN-FORMAT",
                "D003 fan-in object format must match integrated closeout object format",
                failures,
            )
            checks_passed += require(
                class_case.get("success") is True
                and category_case.get("success") is True
                and message_case.get("success") is True
                and negative_case.get("success") is True,
                "dynamic",
                "M253-E002-FANIN-UPSTREAM",
                "integrated cases must succeed before fan-in closeout passes",
                failures,
            )
            dynamic_cases.append(
                {
                    "case_id": "fanin-distinct-linker-discovery-closeout",
                    "anchor_symbols": [class_symbol, category_symbol],
                    "discovery_symbols": [class_root, category_root],
                    "driver_linker_flags": [class_flag, category_flag],
                    "translation_unit_identity_keys": [
                        class_identity,
                        category_identity,
                    ],
                    "status": 0
                    if all(
                        (
                            class_symbol and class_symbol != category_symbol,
                            class_root and class_root != category_root,
                            class_flag and class_flag != category_flag,
                            class_identity and class_identity != category_identity,
                        )
                    )
                    else 1,
                    "success": all(
                        (
                            class_symbol and class_symbol != category_symbol,
                            class_root and class_root != category_root,
                            class_flag and class_flag != category_flag,
                            class_identity and class_identity != category_identity,
                        )
                    ),
                }
            )

            closeout_ready_for_startup_registration = not failures

    ok = not failures
    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "closeout_ready_for_startup_registration": closeout_ready_for_startup_registration
        and ok,
        "dependency_summaries": {
            "M253-A002": display_path(args.a002_summary),
            "M253-B003": display_path(args.b003_summary),
            "M253-C006": display_path(args.c006_summary),
            "M253-D003": display_path(args.d003_summary),
            "M253-E001": display_path(args.e001_summary),
        },
        "dynamic_tools": dynamic_tools,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if ok:
        print(
            "[PASS] M253-E002 cross-lane object-emission closeout preserved; "
            f"summary: {display_path(args.summary_out)}"
        )
        return 0

    print(
        "[FAIL] M253-E002 cross-lane object-emission closeout drift detected; "
        f"summary: {display_path(args.summary_out)}",
        file=sys.stderr,
    )
    for finding in failures:
        print(
            f" - {finding.check_id} :: {finding.detail} [{finding.artifact}]",
            file=sys.stderr,
        )
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
