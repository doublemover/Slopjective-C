#!/usr/bin/env python3
"""Fail-closed validator for M251-D003 runtime-library link wiring."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-d003-runtime-library-link-wiring-v1"
CONTRACT_ID = "objc3c-runtime-support-library-link-wiring/m251-d003-v1"
CORE_FEATURE_CONTRACT_ID = "objc3c-runtime-support-library-core-feature/m251-d002-v1"
ARCHIVE_RELATIVE_PATH = "artifacts/lib/objc3_runtime.lib"
COMPATIBILITY_DISPATCH_SYMBOL = "objc3_msgsend_i32"
RUNTIME_DISPATCH_SYMBOL = "objc3_runtime_dispatch_i32"
SMOKE_SCRIPT_RELATIVE_PATH = "scripts/check_objc3c_native_execution_smoke.ps1"
LINK_MODE = "emitted-object-links-against-objc3_runtime-lib"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-D003/runtime_support_library_link_wiring_summary.json"
)
DEFAULT_EXPECTATIONS_DOC = (
    ROOT / "docs" / "contracts" / "m251_native_runtime_library_link_wiring_d003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_d003_driver_cmake_and_link_wiring_for_runtime_library_consumption_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_CMAKE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_RUNTIME_SURFACE_DOC = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / ARCHIVE_RELATIVE_PATH


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
    SnippetCheck("M251-D003-DOC-EXP-01", "# M251 Native Runtime Library Link Wiring Expectations (D003)"),
    SnippetCheck("M251-D003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M251-D003-DOC-EXP-03", "Objc3RuntimeSupportLibraryLinkWiringSummary"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M251-D003-PKT-01", "# M251-D003 Driver, CMake, and Link Wiring for Runtime Library Consumption Packet"),
    SnippetCheck("M251-D003-PKT-02", f"Packet: `M251-D003`"),
    SnippetCheck("M251-D003-PKT-03", "runtime_support_library_link_wiring_contract_id"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M251-D003-ARCH-01", "M251 lane-D D003 native runtime-library link wiring anchors explicit"),
    SnippetCheck("M251-D003-ARCH-02", "m251_native_runtime_library_link_wiring_d003_expectations.md"),
    SnippetCheck("M251-D003-ARCH-03", "check_objc3c_native_execution_smoke.ps1"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M251-D003-NDOC-01", "## Native runtime-library link wiring (M251-D003)"),
    SnippetCheck("M251-D003-NDOC-02", "!objc3.objc_runtime_support_library_link_wiring"),
    SnippetCheck("M251-D003-NDOC-03", LINK_MODE),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M251-D003-SPC-01", "## M251 native runtime-library link wiring (D003)"),
    SnippetCheck("M251-D003-SPC-02", "Objc3RuntimeSupportLibraryLinkWiringSummary"),
    SnippetCheck("M251-D003-SPC-03", LINK_MODE),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M251-D003-META-01", "## M251 native runtime-library link wiring metadata anchors (D003)"),
    SnippetCheck("M251-D003-META-02", "!objc3.objc_runtime_support_library_link_wiring"),
    SnippetCheck("M251-D003-META-03", COMPATIBILITY_DISPATCH_SYMBOL),
)
CMAKE_SNIPPETS = (
    SnippetCheck("M251-D003-CMAKE-01", "add_library(objc3_runtime STATIC"),
    SnippetCheck("M251-D003-CMAKE-02", "add_library(objc3c::runtime ALIAS objc3_runtime)"),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M251-D003-RTS-01", 'extern "C" int objc3_msgsend_i32('),
    SnippetCheck("M251-D003-RTS-02", "return objc3_runtime_dispatch_i32(receiver, selector, a0, a1, a2, a3);"),
)
RUNTIME_SURFACE_SNIPPETS = (
    SnippetCheck("M251-D003-RTDOC-01", "`M251-D003` now wires emitted-object consumers to the real archive"),
    SnippetCheck("M251-D003-RTDOC-02", "`objc3_msgsend_i32`"),
)
AST_SNIPPETS = (
    SnippetCheck("M251-D003-AST-01", "kObjc3RuntimeSupportLibraryLinkWiringContractId"),
    SnippetCheck("M251-D003-AST-02", "kObjc3RuntimeSupportLibraryCompatibilityDispatchSymbol"),
    SnippetCheck("M251-D003-AST-03", "kObjc3RuntimeSupportLibraryLinkWiringMode"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M251-D003-TYP-01", "struct Objc3RuntimeSupportLibraryLinkWiringSummary {"),
    SnippetCheck("M251-D003-TYP-02", "bool compatibility_dispatch_alias_exported = false;"),
    SnippetCheck("M251-D003-TYP-03", "inline bool IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary("),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M251-D003-ART-01", "BuildRuntimeSupportLibraryLinkWiringSummary("),
    SnippetCheck("M251-D003-ART-02", "runtime_support_library_link_wiring_contract_id"),
    SnippetCheck("M251-D003-ART-03", "runtime_support_library_link_wiring_archive_relative_path"),
)
IR_EMITTER_H_SNIPPETS = (
    SnippetCheck("M251-D003-IRH-01", "std::string runtime_support_library_link_wiring_contract_id;"),
    SnippetCheck("M251-D003-IRH-02", "bool runtime_support_library_link_wiring_compatibility_dispatch_alias_exported ="),
    SnippetCheck("M251-D003-IRH-03", "std::string runtime_support_library_link_wiring_driver_link_mode;"),
)
IR_EMITTER_CPP_SNIPPETS = (
    SnippetCheck("M251-D003-IRC-01", "; runtime_support_library_link_wiring = "),
    SnippetCheck("M251-D003-IRC-02", "!objc3.objc_runtime_support_library_link_wiring = !{!53}"),
    SnippetCheck("M251-D003-IRC-03", 'out << "!53 = !{!\\""'),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M251-D003-DRV-01", "runtime-archive handoff"),
    SnippetCheck("M251-D003-DRV-02", "this driver tranche still stops at deterministic object emission"),
)
SMOKE_SCRIPT_SNIPPETS = (
    SnippetCheck("M251-D003-SMK-01", "function Resolve-RuntimeLibraryForLink {"),
    SnippetCheck("M251-D003-SMK-02", "runtime_support_library_link_wiring_archive_relative_path"),
    SnippetCheck("M251-D003-SMK-03", "runtime_library = if (Test-Path -LiteralPath $defaultRuntimeLibrary -PathType Leaf)"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M251-D003-RTR-01", "`M251-D003` now makes that linkage real for emitted-object execution paths:"),
    SnippetCheck("M251-D003-RTR-02", "objc3_runtime.cpp` exports"),
)
RUNTIME_SHIM_SNIPPETS = (
    SnippetCheck("M251-D003-SHIM-01", "M251-D003 link wiring: emitted objects now link against"),
    SnippetCheck("M251-D003-SHIM-02", "negative unresolved-symbol coverage"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M251-D003-PKG-01", '"check:objc3c:m251-d003-driver-cmake-and-link-wiring-for-runtime-library-consumption"'),
    SnippetCheck("M251-D003-PKG-02", '"test:tooling:m251-d003-driver-cmake-and-link-wiring-for-runtime-library-consumption"'),
    SnippetCheck("M251-D003-PKG-03", '"check:objc3c:m251-d003-lane-d-readiness"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def load_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def resolve_tool(*names: str) -> Path | None:
    for name in names:
        located = shutil.which(name)
        if located:
            return Path(located)
    for candidate in (
        Path("C:/Program Files/PowerShell/7/pwsh.exe"),
        Path("C:/Program Files/LLVM/bin/llc.exe"),
    ):
        if candidate.name.lower() in {name.lower() for name in names} and candidate.exists():
            return candidate
    return None


def record_snippet_failures(
    artifact_label: str,
    path: Path,
    checks: Sequence[SnippetCheck],
    failures: list[Finding],
) -> int:
    text = load_text(path)
    passed = 0
    for check in checks:
        if check.snippet in text:
            passed += 1
        else:
            failures.append(Finding(artifact_label, check.check_id, f"missing snippet in {path.as_posix()}: {check.snippet}"))
    return passed


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--cmake-file", type=Path, default=DEFAULT_CMAKE)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--runtime-surface-doc", type=Path, default=DEFAULT_RUNTIME_SURFACE_DOC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--smoke-script", type=Path, default=DEFAULT_SMOKE_SCRIPT)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--llc", type=Path)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(list(argv))


def run_smoke(args: argparse.Namespace, failures: list[Finding]) -> list[dict[str, object]]:
    dynamic_cases: list[dict[str, object]] = []
    shell = resolve_tool("pwsh.exe", "pwsh", "powershell.exe", "powershell")
    if shell is None:
        failures.append(Finding("dynamic", "M251-D003-DYN-01", "PowerShell executable not found for smoke replay"))
        return dynamic_cases
    llc_path = args.llc or resolve_tool("llc.exe", "llc")
    if llc_path is None:
        failures.append(Finding("dynamic", "M251-D003-DYN-02", "llc executable not found for smoke replay"))
        return dynamic_cases
    if not args.native_exe.exists():
        failures.append(Finding("dynamic", "M251-D003-DYN-03", f"missing native executable: {args.native_exe.as_posix()}"))
        return dynamic_cases
    if not args.runtime_library.exists():
        failures.append(Finding("dynamic", "M251-D003-DYN-04", f"missing runtime archive: {args.runtime_library.as_posix()}"))
        return dynamic_cases
    run_id = "m251_d003_runtime_library_link_wiring"
    env = os.environ.copy()
    env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = run_id
    env["OBJC3C_NATIVE_EXECUTION_LLC_PATH"] = str(llc_path)
    smoke_summary = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke" / run_id / "summary.json"
    command = [
        str(shell),
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(args.smoke_script),
    ]
    completed = subprocess.run(
        command,
        cwd=ROOT,
        env=env,
        capture_output=True,
        text=True,
        check=False,
    )
    case: dict[str, object] = {
        "case_id": "M251-D003-CASE-SMOKE",
        "command": command,
        "exit_code": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
        "status": 1,
        "success": False,
    }
    if completed.returncode != 0:
        failures.append(Finding("dynamic", "M251-D003-DYN-05", f"execution smoke failed with exit {completed.returncode}"))
        dynamic_cases.append(case)
        return dynamic_cases
    if not smoke_summary.exists():
        failures.append(Finding("dynamic", "M251-D003-DYN-06", f"missing smoke summary: {smoke_summary.as_posix()}"))
        dynamic_cases.append(case)
        return dynamic_cases
    payload = json.loads(smoke_summary.read_text(encoding="utf-8"))
    case["summary_path"] = smoke_summary.relative_to(ROOT).as_posix()
    case["summary_status"] = payload.get("status")
    case["runtime_library"] = payload.get("runtime_library", "")
    case["runtime_shim"] = payload.get("runtime_shim", "")
    results = payload.get("results", [])
    positive_runtime_case = next(
        (
            result
            for result in results
            if result.get("kind") == "positive"
            and result.get("requires_runtime_shim") is True
            and result.get("runtime_library") == ARCHIVE_RELATIVE_PATH
            and result.get("passed") is True
        ),
        None,
    )
    negative_unresolved_case = next(
        (
            result
            for result in results
            if result.get("kind") == "negative"
            and result.get("stage") == "link"
            and result.get("fixture", "").endswith("runtime_dispatch_unresolved_symbol.objc3")
            and result.get("passed") is True
        ),
        None,
    )
    if payload.get("status") != "PASS":
        failures.append(Finding("dynamic", "M251-D003-DYN-07", "execution smoke summary did not report PASS"))
    if payload.get("runtime_library") != ARCHIVE_RELATIVE_PATH:
        failures.append(Finding("dynamic", "M251-D003-DYN-08", "execution smoke summary did not publish the runtime archive path"))
    if positive_runtime_case is None:
        failures.append(Finding("dynamic", "M251-D003-DYN-09", "execution smoke did not prove a positive runtime-linked fixture"))
    if negative_unresolved_case is None:
        failures.append(Finding("dynamic", "M251-D003-DYN-10", "execution smoke did not preserve unresolved-symbol negative coverage"))
    case["positive_runtime_fixture"] = positive_runtime_case.get("fixture") if positive_runtime_case else ""
    case["negative_unresolved_fixture"] = negative_unresolved_case.get("fixture") if negative_unresolved_case else ""
    case["status"] = 0 if not any(f.check_id.startswith("M251-D003-DYN-") for f in failures) else 1
    case["success"] = case["status"] == 0
    dynamic_cases.append(case)
    return dynamic_cases


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_groups: tuple[tuple[str, Path, Sequence[SnippetCheck]], ...] = (
        ("expectations", args.expectations_doc, EXPECTATIONS_SNIPPETS),
        ("packet", args.packet_doc, PACKET_SNIPPETS),
        ("architecture", args.architecture_doc, ARCHITECTURE_SNIPPETS),
        ("native_doc", args.native_doc, NATIVE_DOC_SNIPPETS),
        ("lowering_spec", args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, METADATA_SPEC_SNIPPETS),
        ("cmake", args.cmake_file, CMAKE_SNIPPETS),
        ("runtime_source", args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        ("runtime_surface_doc", args.runtime_surface_doc, RUNTIME_SURFACE_SNIPPETS),
        ("ast", args.ast_header, AST_SNIPPETS),
        ("frontend_types", args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        ("frontend_artifacts", args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        ("ir_emitter_h", args.ir_emitter_h, IR_EMITTER_H_SNIPPETS),
        ("ir_emitter_cpp", args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        ("driver", args.driver_cpp, DRIVER_SNIPPETS),
        ("smoke_script", args.smoke_script, SMOKE_SCRIPT_SNIPPETS),
        ("runtime_readme", args.runtime_readme, RUNTIME_README_SNIPPETS),
        ("runtime_shim", args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        ("package_json", args.package_json, PACKAGE_SNIPPETS),
    )
    for label, path, snippets in static_groups:
        checks_total += len(snippets)
        checks_passed += record_snippet_failures(label, path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        dynamic_cases = run_smoke(args, failures)
    dynamic_executed = not args.skip_dynamic_probes

    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "dynamic_cases": dynamic_cases,
        "failures": [asdict(finding) for finding in failures],
    }
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
