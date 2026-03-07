#!/usr/bin/env python3
"""Fail-closed checker for M251-E002 cross-lane runtime foundation gate."""

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
MODE = "m251-e002-cross-lane-runtime-foundation-gate-bootstrap-proof-v1"
CONTRACT_ID = "objc3c-cross-lane-runtime-foundation-gate-bootstrap-proof/m251-e002-v1"
CANONICAL_RUNTIME_LIBRARY = "artifacts/lib/objc3_runtime.lib"
SMOKE_RUN_ID = "m251_e002_cross_lane_runtime_foundation_gate"
EXPECTED_SECTIONS = (
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
)
EXPECTED_SYMBOLS = (
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
)
MISSING = object()

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_cross_lane_runtime_foundation_gate_and_bootstrap_proof_e002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_E001_SUMMARY = ROOT / "tmp" / "reports" / "m251" / "M251-E001" / "runnable_runtime_foundation_gate_contract_summary.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
DEFAULT_METADATA_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_INCOMPLETE_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_export_diagnostics_incomplete_interface.objc3"
)
DEFAULT_OBJECT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "cross-lane-runtime-foundation-gate"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m251/M251-E002/cross_lane_runtime_foundation_gate_summary.json")


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M251-E002-DEP-E001-01",
        Path("docs/contracts/m251_runnable_runtime_foundation_gate_and_evidence_contract_e001_expectations.md"),
    ),
    AssetCheck(
        "M251-E002-DEP-E001-02",
        Path("spec/planning/compiler/m251/m251_e001_runnable_runtime_foundation_gate_and_evidence_contract_packet.md"),
    ),
    AssetCheck(
        "M251-E002-DEP-E001-03",
        Path("scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py"),
    ),
    AssetCheck(
        "M251-E002-DEP-E001-04",
        Path("tests/tooling/test_check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-DOC-EXP-01",
        "# M251 Lane E Cross-Lane Runtime-Foundation Gate and Bootstrap Proof Expectations (E002)",
    ),
    SnippetCheck(
        "M251-E002-DOC-EXP-02",
        "Contract ID: `objc3c-cross-lane-runtime-foundation-gate-bootstrap-proof/m251-e002-v1`",
    ),
    SnippetCheck("M251-E002-DOC-EXP-03", "`M251-E001`"),
    SnippetCheck("M251-E002-DOC-EXP-04", "`M251-A003`"),
    SnippetCheck("M251-E002-DOC-EXP-05", "`M251-B003`"),
    SnippetCheck("M251-E002-DOC-EXP-06", "`M251-C003`"),
    SnippetCheck("M251-E002-DOC-EXP-07", "`M251-D003`"),
    SnippetCheck("M251-E002-DOC-EXP-08", "`artifacts/bin/objc3c-native.exe`"),
    SnippetCheck("M251-E002-DOC-EXP-09", "`llvm-readobj --sections`"),
    SnippetCheck("M251-E002-DOC-EXP-10", "`m251_e002_cross_lane_runtime_foundation_gate`"),
    SnippetCheck(
        "M251-E002-DOC-EXP-11",
        "`tmp/reports/m251/M251-E002/cross_lane_runtime_foundation_gate_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-DOC-PKT-01",
        "# M251-E002 Cross-Lane Runtime-Foundation Gate and Bootstrap Proof Packet",
    ),
    SnippetCheck("M251-E002-DOC-PKT-02", "Packet: `M251-E002`"),
    SnippetCheck(
        "M251-E002-DOC-PKT-03",
        "- `M251-A003`",
    ),
    SnippetCheck("M251-E002-DOC-PKT-04", "- `M251-B003`"),
    SnippetCheck("M251-E002-DOC-PKT-05", "- `M251-C003`"),
    SnippetCheck("M251-E002-DOC-PKT-06", "- `M251-D003`"),
    SnippetCheck("M251-E002-DOC-PKT-07", "- `M251-E001`"),
    SnippetCheck(
        "M251-E002-DOC-PKT-08",
        "`scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`",
    ),
    SnippetCheck(
        "M251-E002-DOC-PKT-09",
        "`tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`",
    ),
    SnippetCheck("M251-E002-DOC-PKT-10", "`m251_e002_cross_lane_runtime_foundation_gate`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-ARCH-01",
        "M251 lane-E E002 cross-lane runtime-foundation gate anchors explicit",
    ),
    SnippetCheck("M251-E002-ARCH-02", "`objc3c-native` probes for manifest preservation, precise export diagnostics"),
    SnippetCheck("M251-E002-ARCH-03", "before operator runbooks land in E003"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-NDOC-01",
        "## Cross-lane runtime-foundation gate and bootstrap proof (M251-E002)",
    ),
    SnippetCheck("M251-E002-NDOC-02", "`M251-A003`, `M251-B003`, `M251-C003`, `M251-D003`, and `M251-E001`"),
    SnippetCheck("M251-E002-NDOC-03", "`llvm-readobj --sections` and `llvm-objdump --syms`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-SPC-01",
        "## M251 cross-lane runtime-foundation gate and bootstrap proof (E002)",
    ),
    SnippetCheck("M251-E002-SPC-02", "`artifacts/bin/objc3c-native.exe`"),
    SnippetCheck("M251-E002-SPC-03", "`m251_e002_cross_lane_runtime_foundation_gate`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-META-01",
        "## M251 cross-lane runtime-foundation gate metadata anchors (E002)",
    ),
    SnippetCheck(
        "M251-E002-META-02",
        "objc3c-cross-lane-runtime-foundation-gate-bootstrap-proof/m251-e002-v1",
    ),
    SnippetCheck("M251-E002-META-03", "`tmp/reports/m251/M251-E001/runnable_runtime_foundation_gate_contract_summary.json`"),
    SnippetCheck("M251-E002-META-04", "`tmp/artifacts/objc3c-native/execution-smoke/m251_e002_cross_lane_runtime_foundation_gate/summary.json`"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E002-PKG-01",
        '"check:objc3c:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof": '
        '"python scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py"',
    ),
    SnippetCheck(
        "M251-E002-PKG-02",
        '"test:tooling:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof": '
        '"python -m pytest tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py -q"',
    ),
    SnippetCheck(
        "M251-E002-PKG-03",
        '"check:objc3c:m251-e002-lane-e-readiness": '
        '"npm run check:objc3c:m251-e001-lane-e-readiness '
        '&& npm run check:objc3c:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof '
        '&& npm run test:tooling:m251-e002-cross-lane-runtime-foundation-gate-and-bootstrap-proof"',
    ),
    SnippetCheck("M251-E002-PKG-04", '"check:objc3c:m251-e001-lane-e-readiness": '),
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--e001-summary", type=Path, default=DEFAULT_E001_SUMMARY)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--smoke-script", type=Path, default=DEFAULT_SMOKE_SCRIPT)
    parser.add_argument("--metadata-fixture", type=Path, default=DEFAULT_METADATA_FIXTURE)
    parser.add_argument("--incomplete-fixture", type=Path, default=DEFAULT_INCOMPLETE_FIXTURE)
    parser.add_argument("--object-fixture", type=Path, default=DEFAULT_OBJECT_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-readobj", type=Path, default=None)
    parser.add_argument("--llvm-objdump", type=Path, default=None)
    parser.add_argument("--pwsh", type=Path, default=None)
    parser.add_argument("--llc", type=Path, default=None)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
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


def load_json_payload(path: Path, *, exists_check_id: str, parse_check_id: str) -> tuple[int, list[Finding], dict[str, Any] | None]:
    checks_total = 2
    findings: list[Finding] = []
    payload: dict[str, Any] | None = None
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required JSON payload is missing: {display_path(path)}"))
        return checks_total, findings, None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(path), parse_check_id, f"invalid JSON: {exc}"))
    return checks_total, findings, payload


def resolve_tool(tool_name: str, explicit_path: Path | None) -> Path | None:
    if explicit_path is not None:
        candidate = explicit_path.resolve()
        return candidate if candidate.exists() else None
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
        if tool_name.lower().startswith("pwsh"):
            candidate = Path(program_files) / "PowerShell" / "7" / tool_name
            if candidate.exists():
                return candidate.resolve()
    resolved = shutil.which(tool_name)
    if resolved:
        return Path(resolved).resolve()
    return None


def run_command(command: Sequence[str], cwd: Path, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        env=env,
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
            if found is not MISSING:
                return found
    elif isinstance(payload, list):
        for value in payload:
            found = find_first_key(value, key)
            if found is not MISSING:
                return found
    return MISSING


def manifest_value(payload: object, key: str, default: object) -> object:
    found = find_first_key(payload, key)
    return default if found is MISSING else found


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def load_diagnostics(path: Path) -> list[dict[str, object]]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    if not isinstance(diagnostics, list):
        raise ValueError(f"diagnostics payload missing list at {path}")
    return diagnostics


def has_diagnostic(diagnostics: Sequence[dict[str, object]], code: str, message_substring: str) -> bool:
    for diagnostic in diagnostics:
        if diagnostic.get("code") != code:
            continue
        message = diagnostic.get("message", "")
        if isinstance(message, str) and message_substring in message:
            return True
    return False


def extract_metadata_sections(readobj_stdout: str) -> list[str]:
    names: list[str] = []
    for block in readobj_stdout.split("Section {"):
        match = re.search(r"Name: ([^\s(]+)", block)
        if match:
            names.append(match.group(1))
    return names


def extract_retained_symbols(objdump_stdout: str) -> list[str]:
    return [symbol for symbol in EXPECTED_SYMBOLS if symbol in objdump_stdout]


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[list[dict[str, object]], int, int, dict[str, str]]:
    cases: list[dict[str, object]] = []
    checks_total = 0
    checks_passed = 0
    tool_paths: dict[str, str] = {}
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)

    if not args.native_exe.exists():
        failures.append(Finding("dynamic", "M251-E002-DYN-EXE", f"missing native executable: {display_path(args.native_exe)}"))
        return cases, checks_total, checks_passed, tool_paths
    if not args.runtime_library.exists():
        failures.append(Finding("dynamic", "M251-E002-DYN-LIB", f"missing runtime archive: {display_path(args.runtime_library)}"))
        return cases, checks_total, checks_passed, tool_paths

    llvm_readobj = resolve_tool("llvm-readobj.exe", args.llvm_readobj)
    llvm_objdump = resolve_tool("llvm-objdump.exe", args.llvm_objdump)
    pwsh = resolve_tool("pwsh.exe", args.pwsh)
    llc = resolve_tool("llc.exe", args.llc)
    if llvm_readobj is not None:
        tool_paths["llvm_readobj"] = display_path(llvm_readobj)
    if llvm_objdump is not None:
        tool_paths["llvm_objdump"] = display_path(llvm_objdump)
    if pwsh is not None:
        tool_paths["pwsh"] = display_path(pwsh)
    if llc is not None:
        tool_paths["llc"] = display_path(llc)

    metadata_out = probe_root / "manifest_preserved"
    metadata_out.mkdir(parents=True, exist_ok=True)
    metadata_cmd = [
        str(args.native_exe.resolve()),
        str(args.metadata_fixture.resolve()),
        "--out-dir",
        str(metadata_out),
        "--emit-prefix",
        "module",
    ]
    metadata_result = run_command(metadata_cmd, ROOT)
    metadata_manifest_path = metadata_out / "module.manifest.json"
    metadata_case: dict[str, object] = {
        "case_id": "M251-E002-CASE-MANIFEST",
        "command": metadata_cmd,
        "compile_exit_code": metadata_result.returncode,
        "manifest_path": display_path(metadata_manifest_path),
        "status": 1,
        "success": False,
    }
    checks_total += 4
    if metadata_manifest_path.exists() and metadata_result.returncode in (0, 1):
        manifest_payload = load_json(metadata_manifest_path)
        records = manifest_payload.get("runtime_metadata_source_records", {}) if isinstance(manifest_payload, dict) else {}
        interfaces = manifest_payload.get("interfaces", []) if isinstance(manifest_payload, dict) else []
        implementations = manifest_payload.get("implementations", []) if isinstance(manifest_payload, dict) else []
        metadata_case.update({
            "runtime_metadata_source_records_present": isinstance(records, dict),
            "property_records": len(records.get("properties", [])) if isinstance(records, dict) else 0,
            "method_records": len(records.get("methods", [])) if isinstance(records, dict) else 0,
            "ivar_records": len(records.get("ivars", [])) if isinstance(records, dict) else 0,
            "interface_count": len(interfaces) if isinstance(interfaces, list) else 0,
            "implementation_count": len(implementations) if isinstance(implementations, list) else 0,
            "runtime_export_ready_for_runtime_export": manifest_value(manifest_payload, "runtime_export_ready_for_runtime_export", False),
        })
        manifest_success = (
            metadata_case["runtime_metadata_source_records_present"] is True
            and metadata_case["property_records"] > 0
            and metadata_case["method_records"] > 0
            and metadata_case["ivar_records"] > 0
            and metadata_case["interface_count"] > 0
            and metadata_case["implementation_count"] > 0
            and metadata_case["runtime_export_ready_for_runtime_export"] is True
        )
        metadata_case["status"] = 0 if manifest_success else 1
        metadata_case["success"] = manifest_success
        if manifest_success:
            checks_passed += 4
        else:
            failures.append(Finding("dynamic", "M251-E002-DYN-MANIFEST", "integrated metadata manifest probe drifted"))
    else:
        failures.append(Finding("dynamic", "M251-E002-DYN-MANIFEST", "metadata manifest probe failed to preserve manifest output"))
    cases.append(metadata_case)

    diag_out = probe_root / "incomplete_interface"
    diag_out.mkdir(parents=True, exist_ok=True)
    diag_cmd = [
        str(args.native_exe.resolve()),
        str(args.incomplete_fixture.resolve()),
        "--out-dir",
        str(diag_out),
        "--emit-prefix",
        "module",
    ]
    diag_result = run_command(diag_cmd, ROOT)
    diag_path = diag_out / "module.diagnostics.json"
    diag_case: dict[str, object] = {
        "case_id": "M251-E002-CASE-DIAGNOSTIC",
        "command": diag_cmd,
        "compile_exit_code": diag_result.returncode,
        "diagnostics_path": display_path(diag_path),
        "status": 1,
        "success": False,
    }
    checks_total += 3
    if diag_result.returncode == 1 and diag_path.exists():
        diagnostics = load_diagnostics(diag_path)
        diag_case["diagnostic_codes"] = [d.get("code") for d in diagnostics]
        diag_case["success"] = has_diagnostic(
            diagnostics,
            "O3S260",
            "interface 'Widget' is missing a matching @implementation",
        )
        diag_case["status"] = 0 if diag_case["success"] else 1
        if diag_case["success"]:
            checks_passed += 3
        else:
            failures.append(Finding("dynamic", "M251-E002-DYN-DIAG", "integrated diagnostic probe lost precise O3S260 coverage"))
    else:
        failures.append(Finding("dynamic", "M251-E002-DYN-DIAG", "integrated diagnostic probe did not fail closed as expected"))
    cases.append(diag_case)

    object_out = probe_root / "zero_descriptor"
    object_out.mkdir(parents=True, exist_ok=True)
    object_cmd = [
        str(args.native_exe.resolve()),
        str(args.object_fixture.resolve()),
        "--out-dir",
        str(object_out),
        "--emit-prefix",
        "module",
    ]
    object_result = run_command(object_cmd, ROOT)
    object_path = object_out / "module.obj"
    object_case: dict[str, object] = {
        "case_id": "M251-E002-CASE-OBJECT",
        "command": object_cmd,
        "compile_exit_code": object_result.returncode,
        "object_path": display_path(object_path),
        "status": 1,
        "success": False,
    }
    checks_total += 4
    if object_result.returncode == 0 and object_path.exists() and llvm_readobj is not None and llvm_objdump is not None:
        readobj_result = run_command([str(llvm_readobj), "--sections", str(object_path)], ROOT)
        objdump_result = run_command([str(llvm_objdump), "--syms", str(object_path)], ROOT)
        section_names = extract_metadata_sections(readobj_result.stdout)
        retained_symbols = extract_retained_symbols(objdump_result.stdout)
        object_case["metadata_sections"] = section_names
        object_case["retained_symbols"] = retained_symbols
        object_success = (
            readobj_result.returncode == 0
            and objdump_result.returncode == 0
            and set(EXPECTED_SECTIONS).issubset(set(section_names))
            and set(EXPECTED_SYMBOLS).issubset(set(retained_symbols))
        )
        object_case["status"] = 0 if object_success else 1
        object_case["success"] = object_success
        if object_success:
            checks_passed += 4
        else:
            failures.append(Finding("dynamic", "M251-E002-DYN-OBJECT", "integrated object inspection probe drifted"))
    else:
        if llvm_readobj is None or llvm_objdump is None:
            failures.append(Finding("dynamic", "M251-E002-DYN-TOOLS", "llvm-readobj.exe or llvm-objdump.exe not found"))
        else:
            failures.append(Finding("dynamic", "M251-E002-DYN-OBJECT", "integrated object probe did not emit an inspectable object"))
    cases.append(object_case)

    smoke_case: dict[str, object] = {
        "case_id": "M251-E002-CASE-SMOKE",
        "status": 1,
        "success": False,
    }
    checks_total += 4
    if pwsh is None or llc is None:
        failures.append(Finding("dynamic", "M251-E002-DYN-SMOKE-TOOL", "pwsh.exe or llc.exe not found for integrated smoke replay"))
    else:
        smoke_env = os.environ.copy()
        smoke_env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = SMOKE_RUN_ID
        smoke_env["OBJC3C_NATIVE_EXECUTION_LLC_PATH"] = str(llc)
        smoke_summary_path = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke" / SMOKE_RUN_ID / "summary.json"
        smoke_cmd = [str(pwsh), "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(args.smoke_script.resolve())]
        smoke_result = run_command(smoke_cmd, ROOT, env=smoke_env)
        smoke_case["command"] = smoke_cmd
        smoke_case["exit_code"] = smoke_result.returncode
        smoke_case["summary_path"] = display_path(smoke_summary_path)
        smoke_case["stdout"] = smoke_result.stdout
        smoke_case["stderr"] = smoke_result.stderr
        if smoke_result.returncode == 0 and smoke_summary_path.exists():
            smoke_payload = load_json(smoke_summary_path)
            positive_runtime_results = [
                result
                for result in smoke_payload.get("results", [])
                if result.get("kind") == "positive"
                and result.get("requires_runtime_shim") is True
                and result.get("runtime_library") == CANONICAL_RUNTIME_LIBRARY
                and result.get("passed") is True
            ]
            smoke_success = (
                smoke_payload.get("status") == "PASS"
                and smoke_payload.get("runtime_library") == CANONICAL_RUNTIME_LIBRARY
                and smoke_payload.get("failed") == 0
                and len(positive_runtime_results) > 0
            )
            smoke_case["summary_status"] = smoke_payload.get("status")
            smoke_case["runtime_library"] = smoke_payload.get("runtime_library")
            smoke_case["positive_runtime_fixture_count"] = len(positive_runtime_results)
            smoke_case["status"] = 0 if smoke_success else 1
            smoke_case["success"] = smoke_success
            if smoke_success:
                checks_passed += 4
            else:
                failures.append(Finding("dynamic", "M251-E002-DYN-SMOKE", "integrated smoke replay drifted from the native runtime archive path"))
        else:
            failures.append(Finding("dynamic", "M251-E002-DYN-SMOKE", "integrated smoke replay failed"))
    cases.append(smoke_case)

    return cases, checks_total, checks_passed, tool_paths


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    checks_passed = 0
    failures: list[Finding] = []

    prereq_checks, prereq_findings = check_prerequisite_assets()
    checks_total += prereq_checks
    checks_passed += prereq_checks - len(prereq_findings)
    failures.extend(prereq_findings)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M251-E002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M251-E002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M251-E002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M251-E002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M251-E002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M251-E002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M251-E002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += check_count
        checks_passed += check_count - len(findings)
        failures.extend(findings)

    e001_checks, e001_findings, e001_payload = load_json_payload(
        args.e001_summary,
        exists_check_id="M251-E002-E001-SUM-EXISTS",
        parse_check_id="M251-E002-E001-SUM-PARSE",
    )
    checks_total += e001_checks
    checks_passed += e001_checks - len(e001_findings)
    failures.extend(e001_findings)
    if e001_payload is not None:
        checks_total += 2
        e001_ok = e001_payload.get("ok") is True
        e001_smoke = int(e001_payload.get("smoke_positive_runtime_fixture_count", 0)) > 0
        if e001_ok:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(args.e001_summary), "M251-E002-E001-OK", "E001 summary did not report ok=true"))
        if e001_smoke:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(args.e001_summary), "M251-E002-E001-SMOKE", "E001 summary no longer reports positive runtime smoke coverage"))

    tool_paths: dict[str, str] = {}
    dynamic_cases: list[dict[str, object]] = []
    dynamic_probes_executed = not args.skip_dynamic_probes
    if not args.skip_dynamic_probes:
        dyn_cases, dyn_checks, dyn_passed, dyn_tools = run_dynamic_probes(args, failures)
        dynamic_cases = dyn_cases
        checks_total += dyn_checks
        checks_passed += dyn_passed
        tool_paths = dyn_tools

    ok = not failures
    summary_payload = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "tool_paths": tool_paths,
        "dependency_summary": display_path(args.e001_summary),
        "dynamic_cases": dynamic_cases,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if ok:
        print(f"[PASS] M251-E002 cross-lane runtime foundation gate preserved; summary: {display_path(args.summary_out)}")
        return 0

    print(f"[FAIL] M251-E002 cross-lane runtime foundation gate drift detected; summary: {display_path(args.summary_out)}", file=sys.stderr)
    for finding in failures:
        print(f" - {finding.check_id} :: {finding.detail} [{finding.artifact}]", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
