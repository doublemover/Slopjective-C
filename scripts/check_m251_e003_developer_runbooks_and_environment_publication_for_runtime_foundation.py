#!/usr/bin/env python3
"""Fail-closed checker for M251-E003 runtime-foundation developer runbooks."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-e003-runtime-foundation-developer-runbook-environment-publication-v1"
CONTRACT_ID = "objc3c-runtime-foundation-developer-runbook-environment-publication/m251-e003-v1"
SMOKE_RUN_ID = "m251_e003_runtime_foundation_runbook_smoke"
CANONICAL_RUNTIME_LIBRARY = "artifacts/lib/objc3_runtime.lib"
RUNBOOK_RELATIVE_PATH = "docs/runbooks/m251_runtime_foundation_developer_runbook.md"
OBJECT_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runbook" / "object_inspection"
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

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_developer_runbooks_and_environment_publication_for_runtime_foundation_e003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation_packet.md"
)
DEFAULT_RUNBOOK_DOC = ROOT / "docs" / "runbooks" / "m251_runtime_foundation_developer_runbook.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_E002_SUMMARY = ROOT / "tmp" / "reports" / "m251" / "M251-E002" / "cross_lane_runtime_foundation_gate_summary.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_OBJECT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
)
DEFAULT_SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m251/M251-E003/runtime_foundation_developer_runbook_summary.json")


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
        "M251-E003-DEP-E002-01",
        Path("docs/contracts/m251_cross_lane_runtime_foundation_gate_and_bootstrap_proof_e002_expectations.md"),
    ),
    AssetCheck(
        "M251-E003-DEP-E002-02",
        Path("spec/planning/compiler/m251/m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof_packet.md"),
    ),
    AssetCheck(
        "M251-E003-DEP-E002-03",
        Path("scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py"),
    ),
    AssetCheck(
        "M251-E003-DEP-E002-04",
        Path("tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-DOC-EXP-01",
        "# M251 Developer Runbooks and Environment Publication for Runtime Foundation Expectations (E003)",
    ),
    SnippetCheck(
        "M251-E003-DOC-EXP-02",
        f"Contract ID: `{CONTRACT_ID}`",
    ),
    SnippetCheck("M251-E003-DOC-EXP-03", "Issue: `#7070`"),
    SnippetCheck("M251-E003-DOC-EXP-04", "Dependencies: `M251-E002`"),
    SnippetCheck("M251-E003-DOC-EXP-05", f"`{RUNBOOK_RELATIVE_PATH}`"),
    SnippetCheck("M251-E003-DOC-EXP-06", "`npm run build:objc3c-native`"),
    SnippetCheck("M251-E003-DOC-EXP-07", "`C:\\Program Files\\LLVM\\bin\\llc.exe`"),
    SnippetCheck("M251-E003-DOC-EXP-08", "`C:\\Program Files\\LLVM\\bin\\llvm-readobj.exe`"),
    SnippetCheck("M251-E003-DOC-EXP-09", "`C:\\Program Files\\LLVM\\bin\\llvm-objdump.exe`"),
    SnippetCheck(
        "M251-E003-DOC-EXP-10",
        "`tmp/reports/m251/M251-E003/runtime_foundation_developer_runbook_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-DOC-PKT-01",
        "# M251-E003 Developer Runbooks and Environment Publication for Runtime Foundation Packet",
    ),
    SnippetCheck("M251-E003-DOC-PKT-02", "Packet: `M251-E003`"),
    SnippetCheck("M251-E003-DOC-PKT-03", "Issue: `#7070`"),
    SnippetCheck("M251-E003-DOC-PKT-04", "Dependencies: `M251-E002`"),
    SnippetCheck("M251-E003-DOC-PKT-05", f"`{RUNBOOK_RELATIVE_PATH}`"),
    SnippetCheck(
        "M251-E003-DOC-PKT-06",
        "`scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`",
    ),
    SnippetCheck(
        "M251-E003-DOC-PKT-07",
        "`tests/tooling/test_check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`",
    ),
    SnippetCheck(
        "M251-E003-DOC-PKT-08",
        "`tmp/reports/m251/M251-E003/runtime_foundation_developer_runbook_summary.json`",
    ),
)

RUNBOOK_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-E003-RUN-01", "# M251 Runtime Foundation Developer Runbook"),
    SnippetCheck("M251-E003-RUN-02", "Run every command from the repository root in PowerShell 7."),
    SnippetCheck("M251-E003-RUN-03", "`C:\\Program Files\\PowerShell\\7\\pwsh.exe`"),
    SnippetCheck("M251-E003-RUN-04", "`C:\\Program Files\\LLVM\\bin\\llc.exe`"),
    SnippetCheck("M251-E003-RUN-05", "`C:\\Program Files\\LLVM\\bin\\llvm-readobj.exe`"),
    SnippetCheck("M251-E003-RUN-06", "`C:\\Program Files\\LLVM\\bin\\llvm-objdump.exe`"),
    SnippetCheck("M251-E003-RUN-07", "npm run build:objc3c-native"),
    SnippetCheck(
        "M251-E003-RUN-08",
        '& .\\artifacts\\bin\\objc3c-native.exe .\\tests\\tooling\\fixtures\\native\\m251_runtime_metadata_object_inspection_zero_descriptor.objc3 --out-dir .\\tmp\\artifacts\\compilation\\objc3c-native\\m251\\runbook\\object_inspection --emit-prefix module --llc "C:\\Program Files\\LLVM\\bin\\llc.exe"',
    ),
    SnippetCheck(
        "M251-E003-RUN-09",
        '& "C:\\Program Files\\LLVM\\bin\\llvm-readobj.exe" --sections .\\tmp\\artifacts\\compilation\\objc3c-native\\m251\\runbook\\object_inspection\\module.obj',
    ),
    SnippetCheck(
        "M251-E003-RUN-10",
        '& "C:\\Program Files\\LLVM\\bin\\llvm-objdump.exe" --syms .\\tmp\\artifacts\\compilation\\objc3c-native\\m251\\runbook\\object_inspection\\module.obj',
    ),
    SnippetCheck(
        "M251-E003-RUN-11",
        '$env:OBJC3C_NATIVE_EXECUTION_RUN_ID=\'m251_e003_runtime_foundation_runbook_smoke\'; $env:OBJC3C_NATIVE_EXECUTION_LLC_PATH=\'C:\\Program Files\\LLVM\\bin\\llc.exe\'; & "C:\\Program Files\\PowerShell\\7\\pwsh.exe" -NoProfile -ExecutionPolicy Bypass -File .\\scripts\\check_objc3c_native_execution_smoke.ps1',
    ),
    SnippetCheck("M251-E003-RUN-12", "tmp/artifacts/objc3c-native/execution-smoke/m251_e003_runtime_foundation_runbook_smoke/summary.json"),
    SnippetCheck("M251-E003-RUN-13", "npm run check:objc3c:m251-e003-lane-e-readiness"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-NDOC-01",
        "## Developer runbooks and environment publication for runtime foundation (M251-E003)",
    ),
    SnippetCheck("M251-E003-NDOC-02", "`docs/runbooks/m251_runtime_foundation_developer_runbook.md`"),
    SnippetCheck("M251-E003-NDOC-03", "`m251_e003_runtime_foundation_runbook_smoke`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-SPC-01",
        "## M251 developer runbooks and environment publication for runtime foundation (E003)",
    ),
    SnippetCheck("M251-E003-SPC-02", "`docs/runbooks/m251_runtime_foundation_developer_runbook.md`"),
    SnippetCheck("M251-E003-SPC-03", "`npm run build:objc3c-native`"),
    SnippetCheck("M251-E003-SPC-04", "`m251_e003_runtime_foundation_runbook_smoke`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-META-01",
        "## M251 developer runbook and environment publication metadata anchors (E003)",
    ),
    SnippetCheck("M251-E003-META-02", f"contract id `{CONTRACT_ID}`"),
    SnippetCheck("M251-E003-META-03", f"`{RUNBOOK_RELATIVE_PATH}`"),
    SnippetCheck("M251-E003-META-04", "`tmp/artifacts/compilation/objc3c-native/m251/runbook/object_inspection`"),
    SnippetCheck("M251-E003-META-05", "`tmp/artifacts/objc3c-native/execution-smoke/m251_e003_runtime_foundation_runbook_smoke/summary.json`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-ARCH-01",
        "M251 lane-E E003 runtime-foundation developer runbook anchors explicit",
    ),
    SnippetCheck("M251-E003-ARCH-02", "`docs/runbooks/m251_runtime_foundation_developer_runbook.md`"),
    SnippetCheck("M251-E003-ARCH-03", "runbook-verified build, object inspection, and execution smoke"),
)

RUNTIME_README_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-E003-RTDOC-01", "`M251-E003` publishes the canonical operator runbook"),
    SnippetCheck("M251-E003-RTDOC-02", "`docs/runbooks/m251_runtime_foundation_developer_runbook.md`"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-E003-PKG-01",
        '"check:objc3c:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation": "python scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py"',
    ),
    SnippetCheck(
        "M251-E003-PKG-02",
        '"test:tooling:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation": "python -m pytest tests/tooling/test_check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py -q"',
    ),
    SnippetCheck(
        "M251-E003-PKG-03",
        '"check:objc3c:m251-e003-lane-e-readiness": "npm run check:objc3c:m251-e002-lane-e-readiness && npm run check:objc3c:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation && npm run test:tooling:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation"',
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
    parser.add_argument("--runbook-doc", type=Path, default=DEFAULT_RUNBOOK_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--e002-summary", type=Path, default=DEFAULT_E002_SUMMARY)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--object-fixture", type=Path, default=DEFAULT_OBJECT_FIXTURE)
    parser.add_argument("--smoke-script", type=Path, default=DEFAULT_SMOKE_SCRIPT)
    parser.add_argument("--llvm-readobj", type=Path, default=None)
    parser.add_argument("--llvm-objdump", type=Path, default=None)
    parser.add_argument("--llc", type=Path, default=None)
    parser.add_argument("--pwsh", type=Path, default=None)
    parser.add_argument("--npm", type=Path, default=None)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
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
    if env_bin and tool_name.lower().startswith("llvm"):
        candidate = Path(env_bin) / tool_name
        if candidate.exists():
            return candidate.resolve()
    for candidate in (
        Path("C:/Program Files/PowerShell/7/pwsh.exe"),
        Path("C:/Program Files/LLVM/bin/llc.exe"),
        Path("C:/Program Files/LLVM/bin/llvm-readobj.exe"),
        Path("C:/Program Files/LLVM/bin/llvm-objdump.exe"),
    ):
        if candidate.name.lower() == tool_name.lower() and candidate.exists():
            return candidate.resolve()
    for name in (tool_name, tool_name.replace(".exe", ""), tool_name.replace(".cmd", "")):
        resolved = shutil.which(name)
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


def extract_metadata_sections(readobj_stdout: str) -> list[str]:
    import re

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

    llvm_readobj = resolve_tool("llvm-readobj.exe", args.llvm_readobj)
    llvm_objdump = resolve_tool("llvm-objdump.exe", args.llvm_objdump)
    llc = resolve_tool("llc.exe", args.llc)
    pwsh = resolve_tool("pwsh.exe", args.pwsh)
    npm_tool = resolve_tool("npm.cmd", args.npm)
    if llvm_readobj is not None:
        tool_paths["llvm_readobj"] = display_path(llvm_readobj)
    if llvm_objdump is not None:
        tool_paths["llvm_objdump"] = display_path(llvm_objdump)
    if llc is not None:
        tool_paths["llc"] = display_path(llc)
    if pwsh is not None:
        tool_paths["pwsh"] = display_path(pwsh)
    if npm_tool is not None:
        tool_paths["npm"] = display_path(npm_tool)

    build_case: dict[str, object] = {
        "case_id": "M251-E003-CASE-BUILD",
        "status": 1,
        "success": False,
    }
    checks_total += 3
    if npm_tool is None:
        failures.append(Finding("dynamic", "M251-E003-DYN-BUILD-TOOL", "npm tool not found for documented build command"))
    else:
        build_cmd = [str(npm_tool), "run", "build:objc3c-native"]
        build_result = run_command(build_cmd, ROOT)
        build_case["command"] = build_cmd
        build_case["exit_code"] = build_result.returncode
        build_case["stdout"] = build_result.stdout
        build_case["stderr"] = build_result.stderr
        build_case["native_exe"] = display_path(args.native_exe)
        build_case["runtime_library"] = display_path(args.runtime_library)
        build_success = (
            build_result.returncode == 0
            and args.native_exe.exists()
            and args.runtime_library.exists()
        )
        build_case["status"] = 0 if build_success else 1
        build_case["success"] = build_success
        if build_success:
            checks_passed += 3
        else:
            failures.append(Finding("dynamic", "M251-E003-DYN-BUILD", "documented build command did not reproduce runtime-foundation artifacts"))
    cases.append(build_case)

    object_case: dict[str, object] = {
        "case_id": "M251-E003-CASE-OBJECT",
        "status": 1,
        "success": False,
    }
    checks_total += 5
    if llvm_readobj is None or llvm_objdump is None or llc is None:
        failures.append(Finding("dynamic", "M251-E003-DYN-OBJECT-TOOLS", "llc.exe, llvm-readobj.exe, or llvm-objdump.exe not found for documented object-inspection commands"))
    elif not args.native_exe.exists():
        failures.append(Finding("dynamic", "M251-E003-DYN-OBJECT-EXE", f"missing native executable: {display_path(args.native_exe)}"))
    else:
        OBJECT_OUT_DIR.mkdir(parents=True, exist_ok=True)
        object_cmd = [
            str(args.native_exe.resolve()),
            str(args.object_fixture.resolve()),
            "--out-dir",
            str(OBJECT_OUT_DIR),
            "--emit-prefix",
            "module",
            "--llc",
            str(llc),
        ]
        object_result = run_command(object_cmd, ROOT)
        object_path = OBJECT_OUT_DIR / "module.obj"
        manifest_path = OBJECT_OUT_DIR / "module.manifest.json"
        object_case["command"] = object_cmd
        object_case["compile_exit_code"] = object_result.returncode
        object_case["stdout"] = object_result.stdout
        object_case["stderr"] = object_result.stderr
        object_case["manifest_path"] = display_path(manifest_path)
        object_case["object_path"] = display_path(object_path)
        if object_result.returncode == 0 and object_path.exists() and manifest_path.exists():
            readobj_result = run_command([str(llvm_readobj), "--sections", str(object_path)], ROOT)
            objdump_result = run_command([str(llvm_objdump), "--syms", str(object_path)], ROOT)
            sections = extract_metadata_sections(readobj_result.stdout)
            symbols = extract_retained_symbols(objdump_result.stdout)
            object_case["metadata_sections"] = sections
            object_case["retained_symbols"] = symbols
            object_success = (
                readobj_result.returncode == 0
                and objdump_result.returncode == 0
                and set(EXPECTED_SECTIONS).issubset(set(sections))
                and set(EXPECTED_SYMBOLS).issubset(set(symbols))
            )
            object_case["status"] = 0 if object_success else 1
            object_case["success"] = object_success
            if object_success:
                checks_passed += 5
            else:
                failures.append(Finding("dynamic", "M251-E003-DYN-OBJECT", "documented object-inspection commands drifted from emitted metadata sections or retained symbols"))
        else:
            failures.append(Finding("dynamic", "M251-E003-DYN-OBJECT", "documented object-emission command did not produce manifest/object artifacts"))
    cases.append(object_case)

    smoke_case: dict[str, object] = {
        "case_id": "M251-E003-CASE-SMOKE",
        "status": 1,
        "success": False,
    }
    checks_total += 4
    if pwsh is None or llc is None:
        failures.append(Finding("dynamic", "M251-E003-DYN-SMOKE-TOOLS", "pwsh.exe or llc.exe not found for documented smoke command"))
    else:
        smoke_env = os.environ.copy()
        smoke_env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = SMOKE_RUN_ID
        smoke_env["OBJC3C_NATIVE_EXECUTION_LLC_PATH"] = str(llc)
        smoke_summary_path = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke" / SMOKE_RUN_ID / "summary.json"
        smoke_cmd = [str(pwsh), "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(args.smoke_script.resolve())]
        smoke_result = run_command(smoke_cmd, ROOT, env=smoke_env)
        smoke_case["command"] = smoke_cmd
        smoke_case["exit_code"] = smoke_result.returncode
        smoke_case["stdout"] = smoke_result.stdout
        smoke_case["stderr"] = smoke_result.stderr
        smoke_case["summary_path"] = display_path(smoke_summary_path)
        if smoke_result.returncode == 0 and smoke_summary_path.exists():
            smoke_payload = json.loads(smoke_summary_path.read_text(encoding="utf-8"))
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
                and len(positive_runtime_results) > 0
                and smoke_payload.get("failed") == 0
            )
            smoke_case["summary_status"] = smoke_payload.get("status")
            smoke_case["runtime_library"] = smoke_payload.get("runtime_library")
            smoke_case["positive_runtime_fixture_count"] = len(positive_runtime_results)
            smoke_case["status"] = 0 if smoke_success else 1
            smoke_case["success"] = smoke_success
            if smoke_success:
                checks_passed += 4
            else:
                failures.append(Finding("dynamic", "M251-E003-DYN-SMOKE", "documented smoke command drifted from the runtime-linked PASS path"))
        else:
            failures.append(Finding("dynamic", "M251-E003-DYN-SMOKE", "documented smoke command failed"))
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
        (args.expectations_doc, "M251-E003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M251-E003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.runbook_doc, "M251-E003-RUN-EXISTS", RUNBOOK_SNIPPETS),
        (args.native_doc, "M251-E003-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M251-E003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M251-E003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, "M251-E003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.runtime_readme, "M251-E003-RTDOC-EXISTS", RUNTIME_README_SNIPPETS),
        (args.package_json, "M251-E003-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += check_count
        checks_passed += check_count - len(findings)
        failures.extend(findings)

    e002_checks, e002_findings, e002_payload = load_json_payload(
        args.e002_summary,
        exists_check_id="M251-E003-E002-SUM-EXISTS",
        parse_check_id="M251-E003-E002-SUM-PARSE",
    )
    checks_total += e002_checks
    checks_passed += e002_checks - len(e002_findings)
    failures.extend(e002_findings)
    if e002_payload is not None:
        checks_total += 2
        e002_ok = e002_payload.get("ok") is True
        e002_smoke = any(case.get("case_id") == "M251-E002-CASE-SMOKE" and case.get("success") is True for case in e002_payload.get("dynamic_cases", []))
        if e002_ok:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(args.e002_summary), "M251-E003-E002-OK", "E002 summary did not report ok=true"))
        if e002_smoke:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(args.e002_summary), "M251-E003-E002-SMOKE", "E002 summary no longer reports a successful smoke case"))

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
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "tool_paths": tool_paths,
        "dependency_summary": display_path(args.e002_summary),
        "dynamic_cases": dynamic_cases,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if ok:
        print(f"[PASS] M251-E003 runtime-foundation runbook preserved; summary: {display_path(summary_path)}")
        return 0

    print(f"[FAIL] M251-E003 runtime-foundation runbook drift detected; summary: {display_path(summary_path)}", file=sys.stderr)
    for finding in failures:
        print(f" - {finding.check_id} :: {finding.detail} [{finding.artifact}]", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
