#!/usr/bin/env python3
"""Fail-closed contract checker for M253-B003 host-format metadata policy expansion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-runtime-metadata-object-format-policy/m253-b003-v1"
LAYOUT_POLICY_COMMENT_PREFIX = "; runtime_metadata_layout_policy = contract=objc3c-runtime-metadata-layout-policy/m253-b002-v1"
POLICY_METADATA_NAME = "!objc3.objc_runtime_metadata_layout_policy = !{!55}"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_b003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_packet.md"
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
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "object-format-policy-surface"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-B003/coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json")


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
    SnippetCheck("M253-B003-DOC-EXP-01", "# M253 COFF, ELF, and Mach-O Metadata Policy Surface Core Feature Expansion Expectations (B003)"),
    SnippetCheck("M253-B003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-B003-DOC-EXP-03", "`coff`"),
    SnippetCheck("M253-B003-DOC-EXP-04", "`elf`"),
    SnippetCheck("M253-B003-DOC-EXP-05", "`mach-o`"),
    SnippetCheck("M253-B003-DOC-EXP-06", "`tmp/reports/m253/M253-B003/coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-DOC-PKT-01", "# M253-B003 COFF, ELF, and Mach-O Metadata Policy Surface Core Feature Expansion Packet"),
    SnippetCheck("M253-B003-DOC-PKT-02", "Packet: `M253-B003`"),
    SnippetCheck("M253-B003-DOC-PKT-03", "Dependencies: `M253-B002`"),
    SnippetCheck("M253-B003-DOC-PKT-04", "`hello.objc3`"),
    SnippetCheck("M253-B003-DOC-PKT-05", "`module.obj`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-ARCH-01", "M253 lane-B B003 object-format metadata policy anchors explicit"),
    SnippetCheck("M253-B003-ARCH-02", "m253_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_b003_expectations.md"),
    SnippetCheck("M253-B003-ARCH-03", "host-format section spellings"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-NDOC-01", "## COFF, ELF, and Mach-O metadata policy surface (M253-B003)"),
    SnippetCheck("M253-B003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-B003-NDOC-03", "`coff-logical-section-spellings`"),
    SnippetCheck("M253-B003-NDOC-04", "`check:objc3c:m253-b003-lane-b-readiness`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-SPC-01", "## M253 COFF, ELF, and Mach-O metadata policy surface (B003)"),
    SnippetCheck("M253-B003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-B003-SPC-03", "explicit host-format mapping surface"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-META-01", "## M253 object-format metadata policy anchors (B003)"),
    SnippetCheck("M253-B003-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-B003-META-03", "`mach-o-data-segment-comma-section-spellings`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-LHDR-01", "kObjc3RuntimeMetadataObjectFormatSurfaceContractId"),
    SnippetCheck("M253-B003-LHDR-02", "kObjc3RuntimeMetadataObjectFormatMachO"),
    SnippetCheck("M253-B003-LHDR-03", "std::string emitted_section_name;"),
    SnippetCheck("M253-B003-LHDR-04", "std::string retention_anchor_model;"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-LCPP-01", "HostRuntimeMetadataObjectFormat()"),
    SnippetCheck("M253-B003-LCPP-02", "MapRuntimeMetadataSectionForObjectFormat("),
    SnippetCheck("M253-B003-LCPP-03", "object_format_contract="),
    SnippetCheck("M253-B003-LCPP-04", "M253-B003 object-format policy expansion anchor"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-IR-01", "runtime_metadata_layout_policy.object_format_surface_contract_id"),
    SnippetCheck("M253-B003-IR-02", "layout_policy.emitted_image_info_section"),
    SnippetCheck("M253-B003-IR-03", "family.emitted_section_name"),
    SnippetCheck("M253-B003-IR-04", "M253-B003 object-format policy expansion anchor"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B003-PROC-01", "enum class ProducedObjectFormat"),
    SnippetCheck("M253-B003-PROC-02", "DetectProducedObjectFormat("),
    SnippetCheck("M253-B003-PROC-03", "NormalizeObjectDeterminism("),
    SnippetCheck("M253-B003-PROC-04", "M253-B003 object-format policy expansion anchor"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-B003-PKG-01",
        '"check:objc3c:m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion": "python scripts/check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py"',
    ),
    SnippetCheck(
        "M253-B003-PKG-02",
        '"test:tooling:m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion": "python -m pytest tests/tooling/test_check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py -q"',
    ),
    SnippetCheck(
        "M253-B003-PKG-03",
        '"check:objc3c:m253-b003-lane-b-readiness": "npm run check:objc3c:m253-b002-lane-b-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion && npm run test:tooling:m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion"',
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


def host_policy() -> dict[str, str]:
    if sys.platform.startswith("win"):
        return {
            "object_format": "coff",
            "section_spelling_model": "coff-logical-section-spellings",
            "retention_anchor_model": "llvm.used-appending-global+coff-timestamp-normalization",
            "image_info_section": "objc3.runtime.image_info",
            "class_section": "objc3.runtime.class_descriptors",
        }
    if sys.platform == "darwin":
        return {
            "object_format": "mach-o",
            "section_spelling_model": "mach-o-data-segment-comma-section-spellings",
            "retention_anchor_model": "llvm.used-appending-global+mach-o-data-segment-sections",
            "image_info_section": "__DATA,__objc3_image_info",
            "class_section": "__DATA,__objc3_class_descriptors",
        }
    return {
        "object_format": "elf",
        "section_spelling_model": "elf-logical-section-spellings",
        "retention_anchor_model": "llvm.used-appending-global+elf-stable-sections",
        "image_info_section": "objc3.runtime.image_info",
        "class_section": "objc3.runtime.class_descriptors",
    }


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


def detect_object_format(object_path: Path) -> str:
    if not object_path.exists():
        return "missing"
    header = object_path.read_bytes()[:8]
    if len(header) >= 4 and header[:4] == b"\x7fELF":
        return "elf"
    if len(header) >= 4:
        magic = int.from_bytes(header[:4], "little")
        if magic in {0xfeedface, 0xcefaedfe, 0xfeedfacf, 0xcffaedfe, 0xcafebabe, 0xbebafeca}:
            return "mach-o"
    if len(header) >= 2:
        machine = int.from_bytes(header[:2], "little")
        if machine in {0x014C, 0x8664, 0x01C0, 0xAA64}:
            return "coff"
    return "unknown"


def run_native_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    policy = host_policy()
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
        "case_id": "M253-B003-CASE-NATIVE",
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
        "expected_object_format": policy["object_format"],
        "expected_section_spelling_model": policy["section_spelling_model"],
        "expected_retention_anchor_model": policy["retention_anchor_model"],
        "expected_image_info_section": policy["image_info_section"],
        "expected_class_section": policy["class_section"],
        "detected_object_format": detect_object_format(obj_path),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M253-B003-NATIVE-EXISTS", "objc3c-native.exe is missing", failures)
    checks_total += 1
    checks_passed += require(args.hello_fixture.exists(), display_path(args.hello_fixture), "M253-B003-NATIVE-FIXTURE", "hello fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(ir_path), "M253-B003-NATIVE-STATUS", "native probe must exit 0", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M253-B003-NATIVE-IR", "native probe must emit module.ll", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M253-B003-NATIVE-OBJ", "native probe must emit module.obj", failures)

    if not ir_path.exists():
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    policy_line = next((line for line in ir_text.splitlines() if line.startswith(LAYOUT_POLICY_COMMENT_PREFIX)), "")
    case["policy_line"] = policy_line
    case["llvm_used_line"] = next((line for line in ir_text.splitlines() if line.startswith("@llvm.used = appending global [")), "")

    checks_total += 1
    checks_passed += require(POLICY_METADATA_NAME in ir_text, display_path(ir_path), "M253-B003-NATIVE-NAMED-METADATA", "IR must publish the named metadata anchor", failures)
    checks_total += 1
    checks_passed += require(CONTRACT_ID in ir_text, display_path(ir_path), "M253-B003-NATIVE-CONTRACT", "IR must publish the B003 object-format contract id", failures)
    checks_total += 1
    checks_passed += require(bool(policy_line), display_path(ir_path), "M253-B003-NATIVE-POLICY", "IR must publish the runtime metadata layout replay line", failures)
    checks_total += 1
    checks_passed += require(f"object_format_contract={CONTRACT_ID}" in policy_line, display_path(ir_path), "M253-B003-NATIVE-POLICY-CONTRACT", "policy line must include the B003 object-format contract id", failures)
    checks_total += 1
    checks_passed += require(f"object_format={policy['object_format']}" in policy_line, display_path(ir_path), "M253-B003-NATIVE-OBJECT-FORMAT", "policy line must include the host object format", failures)
    checks_total += 1
    checks_passed += require(f"section_spelling_model={policy['section_spelling_model']}" in policy_line, display_path(ir_path), "M253-B003-NATIVE-SECTION-MODEL", "policy line must include the host section spelling model", failures)
    checks_total += 1
    checks_passed += require(f"retention_anchor_model={policy['retention_anchor_model']}" in policy_line, display_path(ir_path), "M253-B003-NATIVE-RETENTION-MODEL", "policy line must include the host retention-anchor model", failures)
    checks_total += 1
    checks_passed += require(f"image_info_emitted=__objc3_image_info@{policy['image_info_section']}" in policy_line, display_path(ir_path), "M253-B003-NATIVE-IMAGE-INFO-SECTION", "policy line must include the emitted image-info section", failures)
    checks_total += 1
    checks_passed += require(f"family_emitted=class|{policy['class_section']}|__objc3_sec_class_descriptors|0" in policy_line, display_path(ir_path), "M253-B003-NATIVE-CLASS-SECTION", "policy line must include the emitted class section", failures)
    checks_total += 1
    checks_passed += require(f'section "{policy["image_info_section"]}"' in ir_text, display_path(ir_path), "M253-B003-NATIVE-IMAGE-INFO-LINE", "IR must emit the expected host image-info section", failures)
    checks_total += 1
    checks_passed += require(f'section "{policy["class_section"]}"' in ir_text, display_path(ir_path), "M253-B003-NATIVE-CLASS-LINE", "IR must emit the expected host class section", failures)
    checks_total += 1
    checks_passed += require(case["llvm_used_line"].startswith("@llvm.used = appending global ["), display_path(ir_path), "M253-B003-NATIVE-LLVM-USED", "IR must contain @llvm.used retention root", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M253-B003-NATIVE-OBJ-SIZE", "module.obj must be non-empty", failures)
    checks_total += 1
    checks_passed += require(case["detected_object_format"] == policy["object_format"], display_path(obj_path), "M253-B003-NATIVE-OBJECT-MAGIC", "object file magic must match the host object format", failures)

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
        "host_policy": host_policy(),
        "evidence_path": str(args.summary_out).replace("\\", "/"),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
