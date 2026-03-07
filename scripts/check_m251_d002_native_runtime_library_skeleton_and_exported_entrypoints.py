#!/usr/bin/env python3
"""Fail-closed validator for M251-D002 native runtime library core feature."""

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
MODE = "m251-d002-native-runtime-library-core-feature-v1"
CONTRACT_ID = "objc3c-runtime-support-library-core-feature/m251-d002-v1"
SUPPORT_LIBRARY_CONTRACT_ID = "objc3c-runtime-support-library-surface-build-contract/m251-d001-v1"
METADATA_SCAFFOLD_CONTRACT_ID = "objc3c-runtime-metadata-section-scaffold/m251-c002-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_native_runtime_library_core_feature_d002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_d002_native_runtime_library_skeleton_and_exported_entrypoints_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_CMAKE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
DEFAULT_BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
DEFAULT_RUNTIME_SURFACE_DOC = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m251_d002_runtime_library_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-core-feature"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m251/M251-D002/runtime_support_library_core_feature_summary.json")

TARGET_NAME = "objc3_runtime"
SOURCE_ROOT = "native/objc3c/src/runtime"
PUBLIC_HEADER = "native/objc3c/src/runtime/objc3_runtime.h"
IMPLEMENTATION_SOURCE = "native/objc3c/src/runtime/objc3_runtime.cpp"
LIBRARY_KIND = "static"
ARCHIVE_BASENAME = "objc3_runtime"
ARCHIVE_RELATIVE_PATH = "artifacts/lib/objc3_runtime.lib"
PROBE_SOURCE_PATH = "tests/tooling/runtime/m251_d002_runtime_library_probe.cpp"
REGISTER_IMAGE = "objc3_runtime_register_image"
LOOKUP_SELECTOR = "objc3_runtime_lookup_selector"
DISPATCH_I32 = "objc3_runtime_dispatch_i32"
RESET_FOR_TESTING = "objc3_runtime_reset_for_testing"
LINK_MODE = "not-linked-until-m251-d003"


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
    SnippetCheck(
        "M251-D002-DOC-EXP-01",
        "# M251 Native Runtime Library Core Feature Expectations (D002)",
    ),
    SnippetCheck(
        "M251-D002-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-support-library-core-feature/m251-d002-v1`",
    ),
    SnippetCheck("M251-D002-DOC-EXP-03", "`Objc3RuntimeSupportLibraryCoreFeatureSummary`"),
    SnippetCheck("M251-D002-DOC-EXP-04", "`artifacts/lib/objc3_runtime.lib`"),
    SnippetCheck("M251-D002-DOC-EXP-05", "`tests/tooling/runtime/m251_d002_runtime_library_probe.cpp`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-PKT-01", "# M251-D002 Native Runtime Library Skeleton and Exported Entrypoints Packet"),
    SnippetCheck("M251-D002-PKT-02", "Packet: `M251-D002`"),
    SnippetCheck("M251-D002-PKT-03", "build `artifacts/lib/objc3_runtime.lib`"),
    SnippetCheck("M251-D002-PKT-04", "runtime_support_library_core_feature_contract_id"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-ARCH-01", "M251 lane-D D002 native runtime-library core feature implementation anchors"),
    SnippetCheck("M251-D002-ARCH-02", "m251_native_runtime_library_core_feature_d002_expectations.md"),
    SnippetCheck("M251-D002-ARCH-03", "tests/tooling/runtime/m251_d002_runtime_library_probe.cpp"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-NDOC-01", "## Native runtime-library core feature (M251-D002)"),
    SnippetCheck("M251-D002-NDOC-02", "`Objc3RuntimeSupportLibraryCoreFeatureSummary`"),
    SnippetCheck("M251-D002-NDOC-03", "`!objc3.objc_runtime_support_library_core_feature`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-SPC-01", "## M251 native runtime-library core feature (D002)"),
    SnippetCheck("M251-D002-SPC-02", "`Objc3RuntimeSupportLibraryCoreFeatureSummary`"),
    SnippetCheck("M251-D002-SPC-03", "`artifacts/lib/objc3_runtime.lib`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-META-01", "## M251 native runtime-library core feature metadata anchors (D002)"),
    SnippetCheck("M251-D002-META-02", "`!objc3.objc_runtime_support_library_core_feature`"),
    SnippetCheck("M251-D002-META-03", "`artifacts/lib/objc3_runtime.lib`"),
)

CMAKE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-CMAKE-01", "add_library(objc3_runtime STATIC"),
    SnippetCheck("M251-D002-CMAKE-02", "src/runtime/objc3_runtime.cpp"),
    SnippetCheck("M251-D002-CMAKE-03", "OUTPUT_NAME ${OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME}"),
)

BUILD_SCRIPT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-BLD-01", "$outRuntimeLib = Join-Path $outLibDir \"objc3_runtime.lib\""),
    SnippetCheck("M251-D002-BLD-02", "$stagedRuntimeObj = Join-Path $tmpOutDir (\"objc3_runtime.{0}.obj\" -f $runSuffix)"),
    SnippetCheck("M251-D002-BLD-03", "Write-BuildStep (\"archive_start=objc3_runtime -> \""),
    SnippetCheck("M251-D002-BLD-04", "$llvmLibTool"),
)

RUNTIME_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-RTDOC-01", "# ObjC3 Native Runtime Surface"),
    SnippetCheck("M251-D002-RTDOC-02", "`M251-D002` now instantiates that reserved surface and lands:"),
    SnippetCheck("M251-D002-RTDOC-03", "`artifacts/lib/objc3_runtime.lib` via `npm run build:objc3c-native`"),
)

RUNTIME_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-RTH-01", "typedef struct objc3_runtime_image_descriptor {"),
    SnippetCheck("M251-D002-RTH-02", "typedef struct objc3_runtime_selector_handle {"),
    SnippetCheck("M251-D002-RTH-03", "int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,"),
)

RUNTIME_SOURCE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-RTS-01", "std::deque<SelectorSlot> selector_slots;"),
    SnippetCheck("M251-D002-RTS-02", "const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector)"),
    SnippetCheck("M251-D002-RTS-03", "return ComputeDispatchResult(receiver, selector, a0, a1, a2, a3);"),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-AST-01", "kObjc3RuntimeSupportLibraryCoreFeatureContractId"),
    SnippetCheck("M251-D002-AST-02", '"artifacts/lib/objc3_runtime.lib"'),
    SnippetCheck("M251-D002-AST-03", '"tests/tooling/runtime/m251_d002_runtime_library_probe.cpp"'),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-TYP-01", "struct Objc3RuntimeSupportLibraryCoreFeatureSummary {"),
    SnippetCheck("M251-D002-TYP-02", "bool native_runtime_library_archive_build_enabled = false;"),
    SnippetCheck("M251-D002-TYP-03", "std::string archive_relative_path ="),
    SnippetCheck("M251-D002-TYP-04", "inline bool IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary("),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-ART-01", "BuildRuntimeSupportLibraryCoreFeatureSummary("),
    SnippetCheck("M251-D002-ART-02", "runtime_support_library_core_feature_contract_id"),
    SnippetCheck("M251-D002-ART-03", "runtime_support_library_core_feature_archive_relative_path"),
    SnippetCheck("M251-D002-ART-04", "runtime_support_library_core_feature_dispatch_i32_symbol"),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-IRH-01", "std::string runtime_support_library_core_feature_contract_id;"),
    SnippetCheck("M251-D002-IRH-02", "bool runtime_support_library_core_feature_archive_build_enabled = false;"),
    SnippetCheck("M251-D002-IRH-03", "std::string runtime_support_library_core_feature_archive_relative_path;"),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-IRC-01", "; runtime_support_library_core_feature = "),
    SnippetCheck("M251-D002-IRC-02", "!objc3.objc_runtime_support_library_core_feature = !{!52}"),
    SnippetCheck("M251-D002-IRC-03", 'out << "!52 = !{!\\"'),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-DRV-01", "M251-D002 core feature: manifest emission also publishes the live"),
    SnippetCheck("M251-D002-DRV-02", "archive/header/source/probe surface stays synchronized with emitted IR"),
)

RUNTIME_README_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-RTR-01", "`M251-D002` now instantiates the in-tree native runtime library skeleton:"),
    SnippetCheck("M251-D002-RTR-02", "- `tests/tooling/runtime/m251_d002_runtime_library_probe.cpp`"),
    SnippetCheck("M251-D002-RTR-03", "`objc3_runtime_dispatch_i32` so the"),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-SHIM-01", "M251-D002 core feature: the in-tree native runtime library now exists"),
    SnippetCheck("M251-D002-SHIM-02", "driver/link path still targets this shim until"),
)

RUNTIME_PROBE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-PRB-01", "objc3_runtime_register_image(&image)"),
    SnippetCheck("M251-D002-PRB-02", "objc3_runtime_lookup_selector(\"alpha:beta:\")"),
    SnippetCheck("M251-D002-PRB-03", "objc3_runtime_dispatch_i32(5, \"alpha:beta:\", 1, 2, 3, 4)"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D002-PKG-01", '"check:objc3c:m251-d002-native-runtime-library-skeleton-and-exported-entrypoints": "python scripts/check_m251_d002_native_runtime_library_skeleton_and_exported_entrypoints.py"'),
    SnippetCheck("M251-D002-PKG-02", '"test:tooling:m251-d002-native-runtime-library-skeleton-and-exported-entrypoints": "python -m pytest tests/tooling/test_check_m251_d002_native_runtime_library_skeleton_and_exported_entrypoints.py -q"'),
    SnippetCheck("M251-D002-PKG-03", '"check:objc3c:m251-d002-lane-d-readiness": "npm run check:objc3c:m251-c002-lane-c-readiness && npm run check:objc3c:m251-d001-lane-d-readiness && npm run build:objc3c-native && npm run check:objc3c:m251-d002-native-runtime-library-skeleton-and-exported-entrypoints && npm run test:tooling:m251-d002-native-runtime-library-skeleton-and-exported-entrypoints"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--cmake-file", type=Path, default=DEFAULT_CMAKE)
    parser.add_argument("--build-script", type=Path, default=DEFAULT_BUILD_SCRIPT)
    parser.add_argument("--runtime-surface-doc", type=Path, default=DEFAULT_RUNTIME_SURFACE_DOC)
    parser.add_argument("--runtime-header", type=Path, default=DEFAULT_RUNTIME_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
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
            failures.append(Finding(path.as_posix(), snippet.check_id, f"missing snippet: {snippet.snippet}"))
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


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def find_first_key(payload: Any, target: str) -> Any:
    if isinstance(payload, dict):
        if target in payload:
            return payload[target]
        for value in payload.values():
            nested = find_first_key(value, target)
            if nested is not None:
                return nested
    elif isinstance(payload, list):
        for item in payload:
            nested = find_first_key(item, target)
            if nested is not None:
                return nested
    return None


def resolve_tool(executable: str) -> Path | None:
    env_bin = os.environ.get("LLVM_BIN_DIR")
    if env_bin:
        candidate = Path(env_bin) / executable
        if candidate.exists():
            return candidate
    default_candidate = Path("C:/Program Files/LLVM/bin") / executable
    if default_candidate.exists():
        return default_candidate
    which = shutil.which(executable)
    if which:
        return Path(which)
    return None


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[list[dict[str, object]], int, int]:
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)
    hello_out = probe_root / "hello"
    hello_out.mkdir(parents=True, exist_ok=True)
    runtime_probe_out = probe_root / "probe"
    runtime_probe_out.mkdir(parents=True, exist_ok=True)

    dynamic_cases: list[dict[str, object]] = []
    checks_total = 0
    checks_passed = 0

    hello_command = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(hello_out),
        "--emit-prefix",
        "module",
    ]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        hello_command.extend(["--llc", str(llc)])
    hello_result = run_command(hello_command, ROOT)
    manifest_path = hello_out / "module.manifest.json"
    ir_path = hello_out / "module.ll"
    hello_case: dict[str, object] = {
        "case_id": "M251-D002-CASE-HELLO",
        "command": hello_command,
        "process_exit_code": hello_result.returncode,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
    }
    case_checks_total = 7
    case_checks_passed = 0
    checks_total += case_checks_total

    if args.runtime_library.exists():
        case_checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D002-DYN-01", f"missing runtime archive: {args.runtime_library.as_posix()}"))
    if hello_result.returncode == 0:
        case_checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D002-DYN-02", f"native compile exited with {hello_result.returncode}"))
    if manifest_path.exists():
        case_checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D002-DYN-03", f"missing manifest: {manifest_path.as_posix()}"))
    if ir_path.exists():
        case_checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D002-DYN-04", f"missing IR artifact: {ir_path.as_posix()}"))

    if manifest_path.exists() and ir_path.exists():
        manifest = load_json(manifest_path)
        hello_case["runtime_support_library_core_feature_contract_id"] = find_first_key(
            manifest, "runtime_support_library_core_feature_contract_id"
        )
        hello_case["runtime_support_library_core_feature_archive_relative_path"] = find_first_key(
            manifest, "runtime_support_library_core_feature_archive_relative_path"
        )
        hello_case["runtime_support_library_core_feature_probe_source_path"] = find_first_key(
            manifest, "runtime_support_library_core_feature_probe_source_path"
        )
        hello_case["runtime_support_library_core_feature_dispatch_i32_symbol"] = find_first_key(
            manifest, "runtime_support_library_core_feature_dispatch_i32_symbol"
        )
        hello_case["runtime_support_library_core_feature_driver_link_mode"] = find_first_key(
            manifest, "runtime_support_library_core_feature_driver_link_mode"
        )
        hello_case["runtime_support_library_core_feature_entrypoints_implemented"] = find_first_key(
            manifest, "runtime_support_library_core_feature_entrypoints_implemented"
        )
        ir_text = read_text(ir_path)
        hello_case["ir_contract_comment_present"] = (
            f"; runtime_support_library_core_feature = {CONTRACT_ID}" in ir_text
        )
        hello_case["ir_named_metadata_present"] = (
            "!objc3.objc_runtime_support_library_core_feature = !{!52}" in ir_text
        )
        if (
            hello_case["runtime_support_library_core_feature_contract_id"] == CONTRACT_ID
            and hello_case["runtime_support_library_core_feature_archive_relative_path"] == ARCHIVE_RELATIVE_PATH
            and hello_case["runtime_support_library_core_feature_probe_source_path"] == PROBE_SOURCE_PATH
            and hello_case["runtime_support_library_core_feature_dispatch_i32_symbol"] == DISPATCH_I32
            and hello_case["runtime_support_library_core_feature_driver_link_mode"] == LINK_MODE
            and hello_case["runtime_support_library_core_feature_entrypoints_implemented"] is True
        ):
            case_checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-D002-DYN-05", "manifest core-feature contract drifted"))
        if hello_case["ir_contract_comment_present"]:
            case_checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-D002-DYN-06", "missing IR core-feature contract comment"))
        if hello_case["ir_named_metadata_present"]:
            case_checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-D002-DYN-07", "missing IR core-feature named metadata"))
    else:
        failures.append(Finding("dynamic", "M251-D002-DYN-05", "manifest/IR validation prerequisites missing"))
        failures.append(Finding("dynamic", "M251-D002-DYN-06", "manifest/IR validation prerequisites missing"))
        failures.append(Finding("dynamic", "M251-D002-DYN-07", "manifest/IR validation prerequisites missing"))

    hello_case["status"] = 0 if case_checks_passed == case_checks_total else 1
    hello_case["success"] = hello_case["status"] == 0
    dynamic_cases.append(hello_case)
    checks_passed += case_checks_passed

    clangxx = resolve_tool("clang++.exe") or resolve_tool("clang++")
    probe_exe = runtime_probe_out / "m251_d002_runtime_library_probe.exe"
    probe_case: dict[str, object] = {
        "case_id": "M251-D002-CASE-PROBE",
        "runtime_library_exists": args.runtime_library.exists(),
        "clangxx": str(clangxx) if clangxx is not None else None,
    }
    probe_checks_total = 5
    probe_checks_passed = 0
    checks_total += probe_checks_total

    if args.runtime_library.exists():
        probe_checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D002-DYN-08", f"missing runtime archive: {args.runtime_library.as_posix()}"))

    if clangxx is not None:
        probe_checks_passed += 1
        probe_compile_command = [
            str(clangxx),
            "-std=c++20",
            "-Wall",
            "-Wextra",
            "-pedantic",
            f"-I{args.runtime_include_root.resolve()}",
            str(args.runtime_probe.resolve()),
            str(args.runtime_library.resolve()),
            "-o",
            str(probe_exe),
        ]
        probe_case["compile_command"] = probe_compile_command
        probe_compile = run_command(probe_compile_command, ROOT)
        probe_case["compile_exit_code"] = probe_compile.returncode
        if probe_compile.returncode == 0:
            probe_checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-D002-DYN-09", f"probe compile exited with {probe_compile.returncode}"))
        if probe_exe.exists():
            probe_checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-D002-DYN-10", f"missing probe executable: {probe_exe.as_posix()}"))
        if probe_exe.exists():
            probe_run = run_command([str(probe_exe)], ROOT)
            probe_case["run_exit_code"] = probe_run.returncode
            if probe_run.returncode == 0:
                probe_checks_passed += 1
            else:
                failures.append(Finding("dynamic", "M251-D002-DYN-11", f"probe run exited with {probe_run.returncode}"))
        else:
            failures.append(Finding("dynamic", "M251-D002-DYN-11", "probe executable missing before run"))
    else:
        probe_case["compile_command"] = []
        failures.append(Finding("dynamic", "M251-D002-DYN-09", "clang++ not found for probe compilation"))
        failures.append(Finding("dynamic", "M251-D002-DYN-10", "clang++ not found for probe compilation"))
        failures.append(Finding("dynamic", "M251-D002-DYN-11", "clang++ not found for probe compilation"))

    probe_case["status"] = 0 if probe_checks_passed == probe_checks_total else 1
    probe_case["success"] = probe_case["status"] == 0
    dynamic_cases.append(probe_case)
    checks_passed += probe_checks_passed

    return dynamic_cases, checks_total, checks_passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifacts: tuple[tuple[Path, tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.cmake_file, CMAKE_SNIPPETS),
        (args.build_script, BUILD_SCRIPT_SNIPPETS),
        (args.runtime_surface_doc, RUNTIME_SURFACE_SNIPPETS),
        (args.runtime_header, RUNTIME_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_h, IR_EMITTER_H_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        (args.runtime_probe, RUNTIME_PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )

    for path, snippets in artifacts:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    dynamic_checks_total = 0
    dynamic_checks_passed = 0
    if not args.skip_dynamic_probes:
        dynamic_cases, dynamic_checks_total, dynamic_checks_passed = run_dynamic_probes(args, failures)
        checks_total += dynamic_checks_total
        checks_passed += dynamic_checks_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
