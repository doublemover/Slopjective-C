#!/usr/bin/env python3
"""Fail-closed checker for M263-C002 registration-descriptor/image-root lowering."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-c002-registration-descriptor-lowering-and-multi-image-root-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1"
BOUNDARY_PREFIX = "; runtime_registration_descriptor_image_root_lowering = contract=objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1"
LOWERING_MODEL = "frontend-identifiers-drive-emitted-registration-descriptor-and-image-root-globals"
REG_DESCRIPTOR_SECTION = "objc3.runtime.registration_descriptor"
IMAGE_ROOT_SECTION = "objc3.runtime.image_root"
REG_DESCRIPTOR_SYMBOL_PREFIX = "__objc3_runtime_registration_descriptor_"
IMAGE_ROOT_SYMBOL_PREFIX = "__objc3_runtime_image_root_"
REG_DESCRIPTOR_PAYLOAD_MODEL = "registration-descriptor-record-points-at-image-root-image-descriptor-registration-table-linker-anchor-and-init-state"
IMAGE_ROOT_PAYLOAD_MODEL = "image-root-record-points-at-module-name-image-descriptor-registration-table-and-discovery-root"
NON_GOALS = "no-cross-translation-unit-root-deduplication-or-runtime-fanout-merge"
FRONTEND_CLOSURE_CONTRACT = "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_LLVM_READOBJ = "llvm-readobj"
DEFAULT_LLVM_OBJDUMP = "llvm-objdump"
DEFAULT_DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_registration_descriptor_image_root_default.objc3"
DEFAULT_EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_registration_descriptor_image_root_explicit.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "reports" / "m263" / "M263-C002" / "dynamic"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-C002" / "registration_descriptor_lowering_and_multi_image_root_emission_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class DynamicCase:
    case_id: str
    fixture: Path
    expected_registration_descriptor: str
    expected_image_root: str


DYNAMIC_CASES = (
    DynamicCase(
        "default",
        DEFAULT_DEFAULT_FIXTURE,
        "AutoBootstrap_registration_descriptor",
        "AutoBootstrap_image_root",
    ),
    DynamicCase(
        "explicit",
        DEFAULT_EXPLICIT_FIXTURE,
        "DemoRegistrationDescriptor",
        "DemoImageRoot",
    ),
)

EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M263-C002-DOC-EXP-01", "# M263 Registration-Descriptor Lowering And Multi-Image Root Emission Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M263-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-C002-DOC-EXP-03", f"`{REG_DESCRIPTOR_SECTION}`"),
    SnippetCheck("M263-C002-DOC-EXP-04", f"`{IMAGE_ROOT_SECTION}`"),
    SnippetCheck("M263-C002-DOC-EXP-05", "`AutoBootstrap_registration_descriptor`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M263-C002-PKT-01", "# M263-C002 Registration-Descriptor Lowering And Multi-Image Root Emission Core Feature Implementation Packet"),
    SnippetCheck("M263-C002-PKT-02", "Packet: `M263-C002`"),
    SnippetCheck("M263-C002-PKT-03", "Dependencies: `M263-A002`, `M263-B003`, `M263-C001`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M263-C002-ARCH-01", "## M263 registration-descriptor lowering and multi-image root emission (C002)"),
    SnippetCheck("M263-C002-ARCH-02", f"`{REG_DESCRIPTOR_SYMBOL_PREFIX}`"),
    SnippetCheck("M263-C002-ARCH-03", f"`{IMAGE_ROOT_SYMBOL_PREFIX}`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M263-C002-NDOC-01", "## Registration-descriptor lowering and multi-image root emission (M263-C002)"),
    SnippetCheck("M263-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-C002-NDOC-03", "`DemoRegistrationDescriptor`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M263-C002-SPC-01", "## M263 registration-descriptor lowering and multi-image root emission (C002)"),
    SnippetCheck("M263-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-C002-SPC-03", f"`{LOWERING_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M263-C002-META-01", "## M263 registration-descriptor and image-root emitted metadata anchors (C002)"),
    SnippetCheck("M263-C002-META-02", f"`{REG_DESCRIPTOR_SECTION}`"),
    SnippetCheck("M263-C002-META-03", f"`{IMAGE_ROOT_SECTION}`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M263-C002-LHDR-01", "kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId"),
    SnippetCheck("M263-C002-LHDR-02", CONTRACT_ID),
    SnippetCheck("M263-C002-LHDR-03", "kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection"),
    SnippetCheck("M263-C002-LHDR-04", "kObjc3RuntimeBootstrapImageRootLogicalSection"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M263-C002-LCPP-01", "Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary()"),
    SnippetCheck("M263-C002-LCPP-02", "M263-C002 registration-descriptor/image-root lowering anchor"),
    SnippetCheck("M263-C002-LCPP-03", NON_GOALS),
)
IR_HEADER_SNIPPETS = (
    SnippetCheck("M263-C002-IHDR-01", "runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id"),
    SnippetCheck("M263-C002-IHDR-02", "runtime_bootstrap_registration_descriptor_identifier"),
    SnippetCheck("M263-C002-IHDR-03", "runtime_bootstrap_image_root_identifier"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M263-C002-IR-01", "runtime_registration_descriptor_image_root_lowering = "),
    SnippetCheck("M263-C002-IR-02", "M263-C002 registration-descriptor/image-root lowering anchor"),
    SnippetCheck("M263-C002-IR-03", "M263-C002 emits first-class image-root/registration-descriptor"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M263-C002-ART-01", "runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id"),
    SnippetCheck("M263-C002-ART-02", "runtime_bootstrap_registration_descriptor_identifier"),
    SnippetCheck("M263-C002-ART-03", "runtime_bootstrap_image_root_identifier"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M263-C002-PKG-01", '"check:objc3c:m263-c002-registration-descriptor-and-image-root-lowering": "python scripts/check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py"'),
    SnippetCheck("M263-C002-PKG-02", '"test:tooling:m263-c002-registration-descriptor-and-image-root-lowering": "python -m pytest tests/tooling/test_check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py -q"'),
    SnippetCheck("M263-C002-PKG-03", '"check:objc3c:m263-c002-lane-c-readiness": "python scripts/run_m263_c002_lane_c_readiness.py"'),
)


def canonical_json(payload: Any) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> Any:
    return json.loads(read_text(path))


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


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def resolve_command(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    if not name.endswith(".exe"):
        return shutil.which(name + ".exe")
    return None


def make_identifier_safe_suffix(text: str) -> str:
    safe = []
    previous_underscore = False
    for ch in text:
        keep = ch.isalnum()
        emitted = ch if keep else "_"
        if emitted == "_":
            if previous_underscore:
                continue
            previous_underscore = True
        else:
            previous_underscore = False
        safe.append(emitted)
    value = "".join(safe).strip("_")
    return value or "symbol"


def parse_readobj_sections(text: str) -> dict[str, dict[str, int]]:
    sections: dict[str, dict[str, int]] = {}
    current_name: str | None = None
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("Name: "):
            current_name = stripped.split(": ", 1)[1].split(" ", 1)[0]
            sections.setdefault(current_name, {})
        elif current_name and stripped.startswith("RawDataSize:"):
            sections[current_name]["raw_data_size"] = int(stripped.split(":", 1)[1].strip(), 0)
        elif current_name and stripped.startswith("RelocationCount:"):
            sections[current_name]["relocation_count"] = int(stripped.split(":", 1)[1].strip(), 0)
    return sections


def parse_objdump_symbols(text: str) -> set[str]:
    symbols: set[str] = set()
    for line in text.splitlines():
        if not line.startswith("["):
            continue
        if ")" not in line:
            continue
        name = line.rsplit(" ", 1)[-1].strip()
        if name and not name.startswith("."):
            symbols.add(name)
    return symbols


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-header", type=Path, default=DEFAULT_IR_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--llvm-readobj", default=DEFAULT_LLVM_READOBJ)
    parser.add_argument("--llvm-objdump", default=DEFAULT_LLVM_OBJDUMP)
    return parser.parse_args(argv)


def run_dynamic_case(args: argparse.Namespace, case_input: DynamicCase, llvm_readobj: str, llvm_objdump: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root / case_input.case_id
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(args.native_exe.resolve()), str(case_input.fixture.resolve()), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    completed = run_command(command, ROOT)
    case: dict[str, Any] = {
        "case_id": case_input.case_id,
        "fixture": display_path(case_input.fixture),
        "command": command,
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }

    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(case_input.fixture), f"M263-C002-{case_input.case_id}-COMPILE", "native compile probe failed", failures)
    if completed.returncode != 0:
        return checks_total, checks_passed, case

    manifest_path = out_dir / "module.manifest.json"
    descriptor_path = out_dir / "module.runtime-registration-descriptor.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    for path, check_id in (
        (manifest_path, "MANIFEST"),
        (descriptor_path, "DESCRIPTOR"),
        (ir_path, "IR"),
        (obj_path, "OBJ"),
        (backend_path, "BACKEND"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M263-C002-{case_input.case_id}-{check_id}", f"missing artifact {path.name}", failures)
    if not all(path.exists() for path in (manifest_path, descriptor_path, ir_path, obj_path, backend_path)):
        return checks_total, checks_passed, case

    manifest = read_json(manifest_path)
    descriptor = read_json(descriptor_path)
    ir_text = read_text(ir_path)
    backend = read_text(backend_path).strip()
    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    closure = semantic_surface.get("objc_runtime_registration_descriptor_frontend_closure", {})
    expected_reg = case_input.expected_registration_descriptor
    expected_root = case_input.expected_image_root
    safe_reg = make_identifier_safe_suffix(expected_reg)
    safe_root = make_identifier_safe_suffix(expected_root)
    expected_reg_symbol = f"{REG_DESCRIPTOR_SYMBOL_PREFIX}{safe_reg}"
    expected_root_symbol = f"{IMAGE_ROOT_SYMBOL_PREFIX}{safe_root}"
    case["registration_descriptor_identifier"] = descriptor.get("registration_descriptor_identifier")
    case["image_root_identifier"] = descriptor.get("image_root_identifier")
    case["registration_descriptor_symbol"] = expected_reg_symbol
    case["image_root_symbol"] = expected_root_symbol

    checks_total += 1
    checks_passed += require(closure.get("contract_id") == FRONTEND_CLOSURE_CONTRACT, display_path(manifest_path), f"M263-C002-{case_input.case_id}-CLOSURE-CONTRACT", "frontend closure contract mismatch", failures)
    checks_total += 1
    checks_passed += require(closure.get("registration_descriptor_identifier") == expected_reg, display_path(manifest_path), f"M263-C002-{case_input.case_id}-CLOSURE-REG-ID", "frontend closure registration descriptor mismatch", failures)
    checks_total += 1
    checks_passed += require(closure.get("image_root_identifier") == expected_root, display_path(manifest_path), f"M263-C002-{case_input.case_id}-CLOSURE-ROOT-ID", "frontend closure image root mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("registration_descriptor_identifier") == expected_reg, display_path(descriptor_path), f"M263-C002-{case_input.case_id}-DESC-REG-ID", "descriptor registration descriptor mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("image_root_identifier") == expected_root, display_path(descriptor_path), f"M263-C002-{case_input.case_id}-DESC-ROOT-ID", "descriptor image root mismatch", failures)

    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_PREFIX)), "")
    case["boundary_line"] = boundary_line
    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), f"M263-C002-{case_input.case_id}-IR-BOUNDARY", "IR boundary line missing", failures)
    for token, suffix in (
        (f"lowering_model={LOWERING_MODEL}", "MODEL"),
        (f"registration_descriptor_logical_section={REG_DESCRIPTOR_SECTION}", "REG-SECTION"),
        (f"image_root_logical_section={IMAGE_ROOT_SECTION}", "ROOT-SECTION"),
        (f"registration_descriptor_symbol_prefix={REG_DESCRIPTOR_SYMBOL_PREFIX}", "REG-PREFIX"),
        (f"image_root_symbol_prefix={IMAGE_ROOT_SYMBOL_PREFIX}", "ROOT-PREFIX"),
        (f"registration_descriptor_payload_model={REG_DESCRIPTOR_PAYLOAD_MODEL}", "REG-PAYLOAD"),
        (f"image_root_payload_model={IMAGE_ROOT_PAYLOAD_MODEL}", "ROOT-PAYLOAD"),
        (f"non_goals={NON_GOALS}", "NONGOALS"),
        (f"registration_descriptor_identifier={expected_reg}", "REG-ID"),
        (f"image_root_identifier={expected_root}", "ROOT-ID"),
        (f"registration_descriptor_symbol={expected_reg_symbol}", "REG-SYMBOL"),
        (f"image_root_symbol={expected_root_symbol}", "ROOT-SYMBOL"),
    ):
        checks_total += 1
        checks_passed += require(token in boundary_line, display_path(ir_path), f"M263-C002-{case_input.case_id}-IR-{suffix}", f"IR boundary missing token: {token}", failures)

    for needle, suffix in (
        (f"@{expected_root_symbol} = internal constant", "ROOT-GLOBAL"),
        (f"@{expected_reg_symbol} = internal constant", "REG-GLOBAL"),
        (f'section "{IMAGE_ROOT_SECTION}"', "ROOT-SECTION-LIVE"),
        (f'section "{REG_DESCRIPTOR_SECTION}"', "REG-SECTION-LIVE"),
        (f"@llvm.used = appending global [", "LLVM-USED"),
        (f"ptr @{expected_root_symbol}", "LLVM-USED-ROOT"),
        (f"ptr @{expected_reg_symbol}", "LLVM-USED-REG"),
    ):
        checks_total += 1
        checks_passed += require(needle in ir_text, display_path(ir_path), f"M263-C002-{case_input.case_id}-IR-{suffix}", f"missing IR proof: {needle}", failures)

    checks_total += 1
    checks_passed += require(backend == "llvm-direct", display_path(backend_path), f"M263-C002-{case_input.case_id}-BACKEND", "object backend must remain llvm-direct", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0, display_path(obj_path), f"M263-C002-{case_input.case_id}-OBJ-SIZE", "object must be non-empty", failures)

    readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)], ROOT)
    objdump_result = run_command([llvm_objdump, "--syms", str(obj_path)], ROOT)
    case["readobj_returncode"] = readobj_result.returncode
    case["objdump_returncode"] = objdump_result.returncode
    checks_total += 1
    checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), f"M263-C002-{case_input.case_id}-READOBJ-STATUS", "llvm-readobj must exit 0", failures)
    checks_total += 1
    checks_passed += require(objdump_result.returncode == 0, display_path(obj_path), f"M263-C002-{case_input.case_id}-OBJDUMP-STATUS", "llvm-objdump must exit 0", failures)
    if readobj_result.returncode == 0:
        sections = parse_readobj_sections(readobj_result.stdout)
        case["section_inventory"] = sections
        checks_total += 1
        checks_passed += require(REG_DESCRIPTOR_SECTION in sections, display_path(obj_path), f"M263-C002-{case_input.case_id}-READOBJ-REG-SECTION", "registration descriptor section missing from object", failures)
        checks_total += 1
        checks_passed += require(IMAGE_ROOT_SECTION in sections, display_path(obj_path), f"M263-C002-{case_input.case_id}-READOBJ-ROOT-SECTION", "image root section missing from object", failures)
        checks_total += 1
        checks_passed += require(sections.get(REG_DESCRIPTOR_SECTION, {}).get("raw_data_size", 0) > 0, display_path(obj_path), f"M263-C002-{case_input.case_id}-READOBJ-REG-SIZE", "registration descriptor section must have payload bytes", failures)
        checks_total += 1
        checks_passed += require(sections.get(IMAGE_ROOT_SECTION, {}).get("raw_data_size", 0) > 0, display_path(obj_path), f"M263-C002-{case_input.case_id}-READOBJ-ROOT-SIZE", "image root section must have payload bytes", failures)
    if objdump_result.returncode == 0:
        symbols = parse_objdump_symbols(objdump_result.stdout)
        case["symbols"] = sorted(symbols)
        checks_total += 1
        checks_passed += require(expected_root_symbol in symbols, display_path(obj_path), f"M263-C002-{case_input.case_id}-OBJDUMP-ROOT", "image root symbol missing from object", failures)
        checks_total += 1
        checks_passed += require(expected_reg_symbol in symbols, display_path(obj_path), f"M263-C002-{case_input.case_id}-OBJDUMP-REG", "registration descriptor symbol missing from object", failures)

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
        (args.ir_header, IR_HEADER_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_probes_executed = not args.skip_dynamic_probes
    dynamic_cases: list[dict[str, Any]] = []
    llvm_readobj = resolve_command(args.llvm_readobj)
    llvm_objdump = resolve_command(args.llvm_objdump)
    if dynamic_probes_executed:
        checks_total += 1
        checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M263-C002-NATIVE-EXE", "native executable is required", failures)
        checks_total += 1
        checks_passed += require(llvm_readobj is not None, args.llvm_readobj, "M263-C002-LLVM-READOBJ", "llvm-readobj must be available", failures)
        checks_total += 1
        checks_passed += require(llvm_objdump is not None, args.llvm_objdump, "M263-C002-LLVM-OBJDUMP", "llvm-objdump must be available", failures)
        if args.native_exe.exists() and llvm_readobj and llvm_objdump:
            for case_input in DYNAMIC_CASES:
                case_total, case_passed, case_payload = run_dynamic_case(args, case_input, llvm_readobj, llvm_objdump, failures)
                checks_total += case_total
                checks_passed += case_passed
                dynamic_cases.append(case_payload)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
