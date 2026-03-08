#!/usr/bin/env python3
"""Fail-closed contract checker for M253-C001 metadata section emission freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-c001-metadata-section-emission-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1"
BOUNDARY_COMMENT_PREFIX = "; runtime_metadata_section_emission_boundary = contract=objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_metadata_section_emission_contract_and_architecture_freeze_c001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_c001_metadata_section_emission_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "metadata-section-emission-freeze"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-C001/metadata_section_emission_contract_summary.json")


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
    SnippetCheck("M253-C001-DOC-EXP-01", "# M253 Metadata Section Emission Contract and Architecture Freeze Expectations (C001)"),
    SnippetCheck("M253-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-C001-DOC-EXP-03", "`scaffold-placeholder-payloads-until-m253-c002`"),
    SnippetCheck("M253-C001-DOC-EXP-04", "`private-[1xi8]-zeroinitializer-per-descriptor`"),
    SnippetCheck("M253-C001-DOC-EXP-05", "`tmp/reports/m253/M253-C001/metadata_section_emission_contract_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-DOC-PKT-01", "# M253-C001 Metadata Section Emission Contract and Architecture Freeze Packet"),
    SnippetCheck("M253-C001-DOC-PKT-02", "Packet: `M253-C001`"),
    SnippetCheck("M253-C001-DOC-PKT-03", "Dependencies: None"),
    SnippetCheck("M253-C001-DOC-PKT-04", "`hello.objc3`"),
    SnippetCheck("M253-C001-DOC-PKT-05", "`module.obj`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-ARCH-01", "M253 lane-C C001 metadata section emission freeze anchors explicit"),
    SnippetCheck("M253-C001-ARCH-02", "m253_metadata_section_emission_contract_and_architecture_freeze_c001_expectations.md"),
    SnippetCheck("M253-C001-ARCH-03", "placeholder payload models"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-NDOC-01", "## Metadata section emission freeze (M253-C001)"),
    SnippetCheck("M253-C001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C001-NDOC-03", "`scaffold-placeholder-payloads-until-m253-c002`"),
    SnippetCheck("M253-C001-NDOC-04", "`check:objc3c:m253-c001-lane-c-readiness`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-SPC-01", "## M253 metadata section emission freeze (C001)"),
    SnippetCheck("M253-C001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C001-SPC-03", "placeholder bytes without reopening the"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-META-01", "## M253 metadata section emission anchors (C001)"),
    SnippetCheck("M253-C001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C001-META-03", "`i64-count-plus-pointer-vector-aggregates`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-LHDR-01", "kObjc3RuntimeMetadataSectionEmissionContractId"),
    SnippetCheck("M253-C001-LHDR-02", "kObjc3RuntimeMetadataSectionEmissionPayloadModel"),
    SnippetCheck("M253-C001-LHDR-03", "kObjc3RuntimeMetadataSectionEmissionDescriptorPayloadModel"),
    SnippetCheck("M253-C001-LHDR-04", "Objc3RuntimeMetadataSectionEmissionBoundarySummary"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-LCPP-01", "Objc3RuntimeMetadataSectionEmissionBoundarySummary()"),
    SnippetCheck("M253-C001-LCPP-02", "M253-C001 metadata section emission freeze anchor"),
    SnippetCheck("M253-C001-LCPP-03", "non_goals=no-method-selector-string-pool-payloads"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-IR-01", 'out << "; runtime_metadata_section_emission_boundary = "'),
    SnippetCheck("M253-C001-IR-02", "runtime metadata section scaffold globals"),
    SnippetCheck("M253-C001-IR-03", "global [1 x i8] zeroinitializer"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C001-PROC-01", "M253-C001 metadata section emission freeze anchor"),
    SnippetCheck("M253-C001-PROC-02", "placeholder-emission boundary"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-C001-PKG-01",
        '"check:objc3c:m253-c001-metadata-section-emission-contract": "python scripts/check_m253_c001_metadata_section_emission_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M253-C001-PKG-02",
        '"test:tooling:m253-c001-metadata-section-emission-contract": "python -m pytest tests/tooling/test_check_m253_c001_metadata_section_emission_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M253-C001-PKG-03",
        '"check:objc3c:m253-c001-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m253-c001-metadata-section-emission-contract && npm run test:tooling:m253-c001-metadata-section-emission-contract"',
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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--hello-fixture", type=Path, default=DEFAULT_HELLO_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


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


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_native_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / "native-hello"
    command = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(probe_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)

    ir_path = probe_dir / "module.ll"
    obj_path = probe_dir / "module.obj"
    case: dict[str, Any] = {
        "case_id": "M253-C001-CASE-NATIVE",
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M253-C001-NATIVE-EXISTS", "objc3c-native.exe is missing", failures)
    checks_total += 1
    checks_passed += require(args.hello_fixture.exists(), display_path(args.hello_fixture), "M253-C001-NATIVE-FIXTURE", "hello fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(ir_path), "M253-C001-NATIVE-STATUS", "native probe must exit 0", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M253-C001-NATIVE-IR", "native probe must emit module.ll", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M253-C001-NATIVE-OBJ", "native probe must emit module.obj", failures)

    if not ir_path.exists():
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    llvm_used_line = next((line for line in ir_text.splitlines() if line.startswith("@llvm.used = appending global [")), "")
    case["boundary_line"] = boundary_line
    case["llvm_used_line"] = llvm_used_line

    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), "M253-C001-NATIVE-BOUNDARY", "IR must publish the section-emission boundary line", failures)
    checks_total += 1
    checks_passed += require("payload_model=scaffold-placeholder-payloads-until-m253-c002" in boundary_line, display_path(ir_path), "M253-C001-NATIVE-PAYLOAD-MODEL", "boundary line must carry the placeholder payload model", failures)
    checks_total += 1
    checks_passed += require("inventory_model=image-info-plus-class-protocol-category-property-ivar-sections" in boundary_line, display_path(ir_path), "M253-C001-NATIVE-INVENTORY", "boundary line must carry the emitted inventory model", failures)
    checks_total += 1
    checks_passed += require("descriptor_payload_model=private-[1xi8]-zeroinitializer-per-descriptor" in boundary_line, display_path(ir_path), "M253-C001-NATIVE-DESCRIPTOR-MODEL", "boundary line must carry the descriptor placeholder model", failures)
    checks_total += 1
    checks_passed += require("aggregate_payload_model=i64-count-plus-pointer-vector-aggregates" in boundary_line, display_path(ir_path), "M253-C001-NATIVE-AGGREGATE-MODEL", "boundary line must carry the aggregate payload model", failures)
    checks_total += 1
    checks_passed += require("non_goals=no-method-selector-string-pool-payloads" in boundary_line, display_path(ir_path), "M253-C001-NATIVE-NON-GOALS", "boundary line must carry explicit non-goals", failures)
    checks_total += 1
    checks_passed += require('@__objc3_image_info = internal global { i32, i32 } zeroinitializer' in ir_text, display_path(ir_path), "M253-C001-NATIVE-IMAGE-INFO", "IR must keep zeroinitializer image-info payloads", failures)
    checks_total += 1
    checks_passed += require('@__objc3_sec_class_descriptors = internal global { i64 } { i64 0 }' in ir_text, display_path(ir_path), "M253-C001-NATIVE-CLASS-AGGREGATE", "IR must keep zero-sentinel class aggregates on the hello probe", failures)
    checks_total += 1
    checks_passed += require(bool(llvm_used_line), display_path(ir_path), "M253-C001-NATIVE-LLVM-USED", "IR must retain metadata through @llvm.used", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M253-C001-NATIVE-OBJ-SIZE", "module.obj must be non-empty", failures)

    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
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
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        probe_total, probe_passed, case = run_native_probe(args, failures)
        checks_total += probe_total
        checks_passed += probe_passed
        dynamic_cases.append(case)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "evidence_path": str(args.summary_out).replace("\\", "/"),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
