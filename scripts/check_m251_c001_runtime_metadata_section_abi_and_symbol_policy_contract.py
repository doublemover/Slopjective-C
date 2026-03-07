#!/usr/bin/env python3
"""Fail-closed contract checker for M251-C001 runtime metadata section ABI freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-c001-runtime-metadata-section-abi-symbol-policy-contract-v1"
CONTRACT_ID = "objc3c-runtime-metadata-section-abi-symbol-policy-freeze/m251-c001-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_runtime_metadata_section_abi_and_symbol_policy_contract_and_architecture_freeze_c001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-metadata-section-abi"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-C001/runtime_metadata_section_abi_and_symbol_policy_contract_summary.json"
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


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-DOC-EXP-01", "# M251 Runtime Metadata Section ABI and Symbol Policy Contract and Architecture Freeze Expectations (C001)"),
    SnippetCheck("M251-C001-DOC-EXP-02", "Contract ID: `objc3c-runtime-metadata-section-abi-symbol-policy-freeze/m251-c001-v1`"),
    SnippetCheck("M251-C001-DOC-EXP-03", "`Objc3RuntimeMetadataSectionAbiFreezeSummary` remains the canonical lane-C freeze packet"),
    SnippetCheck("M251-C001-DOC-EXP-04", "descriptor prefix `__objc3_meta_`, aggregate prefix `__objc3_sec_`, and image info symbol `__objc3_image_info` remain canonical."),
    SnippetCheck("M251-C001-DOC-EXP-05", "`tmp/reports/m251/M251-C001/runtime_metadata_section_abi_and_symbol_policy_contract_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-DOC-PKT-01", "# M251-C001 Runtime Metadata Section ABI and Symbol Policy Contract and Architecture Freeze Packet"),
    SnippetCheck("M251-C001-DOC-PKT-02", "Packet: `M251-C001`"),
    SnippetCheck("M251-C001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M251-C001-DOC-PKT-04", "The checker runs two deterministic probes:"),
    SnippetCheck("M251-C001-DOC-PKT-05", "C001 freezes logical section identifiers rather than physical COFF/ELF/Mach-O spellings."),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-ARCH-01", "M251 lane-C C001 runtime metadata section ABI and symbol policy freeze"),
    SnippetCheck("M251-C001-ARCH-02", "m251_runtime_metadata_section_abi_and_symbol_policy_contract_and_architecture_freeze_c001_expectations.md"),
    SnippetCheck("M251-C001-ARCH-03", "before `M251-C002` begins reserving LLVM globals and physical object sections."),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-NDOC-01", "## Runtime metadata section ABI and symbol policy freeze (M251-C001)"),
    SnippetCheck("M251-C001-NDOC-02", "`Objc3RuntimeMetadataSectionAbiFreezeSummary`"),
    SnippetCheck("M251-C001-NDOC-03", "`!objc3.objc_runtime_metadata_section_abi`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-SPC-01", "## M251 runtime metadata section ABI and symbol policy freeze (C001)"),
    SnippetCheck("M251-C001-SPC-02", "`Objc3RuntimeMetadataSectionAbiFreezeSummary` to remain the single lane-C"),
    SnippetCheck("M251-C001-SPC-03", "deterministic and fail closed before `M251-C002` starts reserving LLVM globals"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-META-01", "## M251 runtime metadata section ABI and symbol policy metadata anchors (C001)"),
    SnippetCheck("M251-C001-META-02", "`objc3.runtime.image_info`"),
    SnippetCheck("M251-C001-META-03", "named LLVM IR metadata `!objc3.objc_runtime_metadata_section_abi`."),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-AST-01", "kObjc3RuntimeMetadataSectionAbiContractId"),
    SnippetCheck("M251-C001-AST-02", '"objc3.runtime.class_descriptors"'),
    SnippetCheck("M251-C001-AST-03", '"__objc3_meta_"'),
    SnippetCheck("M251-C001-AST-04", '"llvm.used"'),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-TYP-01", "struct Objc3RuntimeMetadataSectionAbiFreezeSummary {"),
    SnippetCheck("M251-C001-TYP-02", "bool object_file_section_inventory_frozen = false;"),
    SnippetCheck("M251-C001-TYP-03", "std::string logical_image_info_section ="),
    SnippetCheck("M251-C001-TYP-04", "std::string descriptor_linkage ="),
    SnippetCheck("M251-C001-TYP-05", "inline bool IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary("),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-ART-01", "BuildRuntimeMetadataSectionAbiFreezeSummary("),
    SnippetCheck("M251-C001-ART-02", "runtime_metadata_section_abi_contract_id"),
    SnippetCheck("M251-C001-ART-03", "runtime_metadata_section_logical_class_descriptor_section"),
    SnippetCheck("M251-C001-ART-04", "runtime_metadata_section_retention_root"),
    SnippetCheck("M251-C001-ART-05", "ir_frontend_metadata.runtime_metadata_section_abi_contract_id ="),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-IRH-01", "std::string runtime_metadata_section_abi_contract_id;"),
    SnippetCheck("M251-C001-IRH-02", "bool runtime_metadata_section_ready_for_scaffold = false;"),
    SnippetCheck("M251-C001-IRH-03", "std::string runtime_metadata_section_visibility;"),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-IRC-01", "; runtime_metadata_section_abi = "),
    SnippetCheck("M251-C001-IRC-02", "!objc3.objc_runtime_metadata_section_abi = !{!48}"),
    SnippetCheck("M251-C001-IRC-03", 'out << "!48 = !{!\\"'),
    SnippetCheck("M251-C001-IRC-04", "runtime_metadata_section_retention_root"),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-DRV-01", "metadata-section ABI surface until later object-section emission lands."),
    SnippetCheck("M251-C001-DRV-02", "WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);"),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-SHIM-01", "M251-C001 freeze: this shim does not define metadata section inventory,"),
    SnippetCheck("M251-C001-SHIM-02", "symbol retention roots, or native object-file symbol policy."),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-C001-PKG-01", '"check:objc3c:m251-c001-runtime-metadata-section-abi-and-symbol-policy-contract": "python scripts/check_m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract.py"'),
    SnippetCheck("M251-C001-PKG-02", '"test:tooling:m251-c001-runtime-metadata-section-abi-and-symbol-policy-contract": "python -m pytest tests/tooling/test_check_m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract.py -q"'),
    SnippetCheck("M251-C001-PKG-03", '"check:objc3c:m251-c001-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m251-c001-runtime-metadata-section-abi-and-symbol-policy-contract && npm run test:tooling:m251-c001-runtime-metadata-section-abi-and-symbol-policy-contract"'),
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
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
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
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[list[dict[str, object]], int, int]:
    cases: list[dict[str, object]] = []
    checks_total = 0
    checks_passed = 0
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)

    class_out = probe_root / "class_manifest_only"
    class_out.mkdir(parents=True, exist_ok=True)
    class_cmd = [
        str(args.runner_exe.resolve()),
        str(args.class_fixture.resolve()),
        "--out-dir",
        str(class_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    class_result = run_command(class_cmd, ROOT)
    class_summary_path = class_out / "module.c_api_summary.json"
    class_manifest_path = class_out / "module.manifest.json"
    class_case: dict[str, object] = {
        "case_id": "M251-C001-CASE-CLASS",
        "command": class_cmd,
        "process_exit_code": class_result.returncode,
        "summary_path": display_path(class_summary_path),
        "manifest_path": display_path(class_manifest_path),
    }
    checks_total += 5
    if class_result.returncode == 0 and class_summary_path.exists() and class_manifest_path.exists():
        class_summary = load_json(class_summary_path)
        class_manifest = load_json(class_manifest_path)
        sema = class_manifest["frontend"]["pipeline"]["sema_pass_manager"]
        class_case["status"] = class_summary["status"]
        class_case["success"] = class_summary["success"]
        class_case["runtime_metadata_section_abi_contract_id"] = sema["runtime_metadata_section_abi_contract_id"]
        class_case["runtime_metadata_section_ready_for_scaffold"] = sema["runtime_metadata_section_ready_for_scaffold"]
        class_case["runtime_metadata_section_logical_class_descriptor_section"] = sema["runtime_metadata_section_logical_class_descriptor_section"]
        class_case["runtime_metadata_section_retention_root"] = sema["runtime_metadata_section_retention_root"]
        if class_summary["status"] == 0 and class_summary["success"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-CLASS-STATUS", "class manifest probe did not succeed"))
        if sema["runtime_metadata_section_abi_contract_id"] == CONTRACT_ID:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-CLASS-CONTRACT", "class manifest probe contract id mismatch"))
        if sema["runtime_metadata_section_ready_for_scaffold"] is True:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-CLASS-READY", "class manifest probe was not ready for scaffold"))
        if sema["runtime_metadata_section_logical_class_descriptor_section"] == "objc3.runtime.class_descriptors":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-CLASS-SECTION", "class manifest probe logical class section drifted"))
        if sema["runtime_metadata_section_retention_root"] == "llvm.used":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-CLASS-RETENTION", "class manifest probe retention root drifted"))
    else:
        failures.append(Finding("dynamic", "M251-C001-CASE-CLASS-EXIT", f"runner exited {class_result.returncode} or artifacts were missing"))
    cases.append(class_case)

    ir_out = probe_root / "hello_ir"
    ir_out.mkdir(parents=True, exist_ok=True)
    ir_cmd = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(ir_out),
        "--emit-prefix",
        "module",
    ]
    ir_result = run_command(ir_cmd, ROOT)
    ir_path = ir_out / "module.ll"
    ir_case: dict[str, object] = {
        "case_id": "M251-C001-CASE-IR",
        "command": ir_cmd,
        "process_exit_code": ir_result.returncode,
        "ir_path": display_path(ir_path),
    }
    checks_total += 3
    if ir_result.returncode == 0 and ir_path.exists():
        ir_text = read_text(ir_path)
        ir_case["ir_contains_contract_comment"] = f"; runtime_metadata_section_abi = {CONTRACT_ID}" in ir_text
        ir_case["ir_contains_named_metadata"] = "!objc3.objc_runtime_metadata_section_abi = !{!48}" in ir_text
        if ir_result.returncode == 0:
            checks_passed += 1
        if ir_case["ir_contains_contract_comment"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-IR-COMMENT", "IR did not preserve the runtime metadata section ABI contract comment"))
        if ir_case["ir_contains_named_metadata"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-C001-CASE-IR-METADATA", "IR did not preserve the runtime metadata section ABI named metadata node"))
    else:
        failures.append(Finding("dynamic", "M251-C001-CASE-IR-EXIT", f"native driver exited {ir_result.returncode} or IR was missing"))
    cases.append(ir_case)

    return cases, checks_total, checks_passed


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_h, IR_EMITTER_H_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        dynamic_cases, dynamic_total, dynamic_passed = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [failure.__dict__ for failure in failures],
    }
    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
        return 0

    for failure in failures:
        print(f"[fail] {failure.check_id} ({failure.artifact}): {failure.detail}")
    return 1


if __name__ == "__main__":
    raise SystemExit(run())
