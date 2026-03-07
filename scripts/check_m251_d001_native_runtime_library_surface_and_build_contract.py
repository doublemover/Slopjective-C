#!/usr/bin/env python3
"""Fail-closed validator for M251-D001 native runtime-library surface freeze."""

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
MODE = "m251-d001-native-runtime-library-surface-build-contract-v1"
CONTRACT_ID = "objc3c-runtime-support-library-surface-build-contract/m251-d001-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_native_runtime_library_surface_and_build_contract_d001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_d001_native_runtime_library_surface_and_build_contract_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_CMAKE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
DEFAULT_RUNTIME_SURFACE_DOC = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = (
    ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-support-library-contract"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-D001/runtime_support_library_contract_summary.json"
)

TARGET_NAME = "objc3_runtime"
SOURCE_ROOT = "native/objc3c/src/runtime"
PUBLIC_HEADER = "native/objc3c/src/runtime/objc3_runtime.h"
LIBRARY_KIND = "static"
ARCHIVE_BASENAME = "objc3_runtime"
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
        "M251-D001-DOC-EXP-01",
        "# M251 Native Runtime Library Surface and Build Contract Expectations (D001)",
    ),
    SnippetCheck(
        "M251-D001-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-support-library-surface-build-contract/m251-d001-v1`",
    ),
    SnippetCheck("M251-D001-DOC-EXP-03", "`Objc3RuntimeSupportLibraryContractSummary` remains the canonical lane-D"),
    SnippetCheck("M251-D001-DOC-EXP-04", "target `objc3_runtime`"),
    SnippetCheck("M251-D001-DOC-EXP-05", "`objc3_runtime_dispatch_i32`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-PKT-01", "# M251-D001 Native Runtime Library Surface and Build Contract Packet"),
    SnippetCheck("M251-D001-PKT-02", "Packet: `M251-D001`"),
    SnippetCheck("M251-D001-PKT-03", "driver-link mode as `not-linked-until-m251-d003`"),
    SnippetCheck("M251-D001-PKT-04", "The checker runs one deterministic dynamic probe:"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-ARCH-01", "M251 lane-D D001 native runtime-library surface and build contract freeze"),
    SnippetCheck("M251-D001-ARCH-02", "m251_native_runtime_library_surface_and_build_contract_d001_expectations.md"),
    SnippetCheck("M251-D001-ARCH-03", "exported registration/lookup/dispatch/reset entrypoints"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-NDOC-01", "## Native runtime-library surface and build contract (M251-D001)"),
    SnippetCheck("M251-D001-NDOC-02", "`Objc3RuntimeSupportLibraryContractSummary`"),
    SnippetCheck("M251-D001-NDOC-03", "`!objc3.objc_runtime_support_library`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-SPC-01", "## M251 native runtime-library surface and build contract (D001)"),
    SnippetCheck("M251-D001-SPC-02", "`Objc3RuntimeSupportLibraryContractSummary` to remain the single lane-D"),
    SnippetCheck("M251-D001-SPC-03", "`not-linked-until-m251-d003`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-META-01", "## M251 native runtime-library surface and build contract metadata anchors (D001)"),
    SnippetCheck("M251-D001-META-02", "target name `objc3_runtime`"),
    SnippetCheck("M251-D001-META-03", "named LLVM IR metadata `!objc3.objc_runtime_support_library`."),
)

CMAKE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-CMAKE-01", "set(OBJC3_RUNTIME_LIBRARY_TARGET objc3_runtime)"),
    SnippetCheck("M251-D001-CMAKE-02", "set(OBJC3_RUNTIME_LIBRARY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime)"),
    SnippetCheck("M251-D001-CMAKE-03", "set(OBJC3_RUNTIME_LIBRARY_PUBLIC_HEADER ${OBJC3_RUNTIME_LIBRARY_SOURCE_DIR}/objc3_runtime.h)"),
    SnippetCheck("M251-D001-CMAKE-04", "set(OBJC3_RUNTIME_LIBRARY_KIND STATIC)"),
)

RUNTIME_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-RTDOC-01", "# ObjC3 Native Runtime Surface"),
    SnippetCheck("M251-D001-RTDOC-02", "`M251-D001` reserves `native/objc3c/src/runtime`"),
    SnippetCheck("M251-D001-RTDOC-03", "`objc3_runtime_lookup_selector`"),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-AST-01", "kObjc3RuntimeSupportLibraryContractId"),
    SnippetCheck("M251-D001-AST-02", '"objc3_runtime"'),
    SnippetCheck("M251-D001-AST-03", '"objc3_runtime_dispatch_i32"'),
    SnippetCheck("M251-D001-AST-04", '"not-linked-until-m251-d003"'),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-TYP-01", "struct Objc3RuntimeSupportLibraryContractSummary {"),
    SnippetCheck("M251-D001-TYP-02", "bool exported_entrypoints_frozen = false;"),
    SnippetCheck("M251-D001-TYP-03", "std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;"),
    SnippetCheck("M251-D001-TYP-04", "inline bool IsReadyObjc3RuntimeSupportLibraryContractSummary("),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-ART-01", "BuildRuntimeSupportLibraryContractSummary()"),
    SnippetCheck("M251-D001-ART-02", "runtime_support_library_contract_id"),
    SnippetCheck("M251-D001-ART-03", "runtime_support_library_dispatch_i32_symbol"),
    SnippetCheck("M251-D001-ART-04", "runtime_support_library_driver_link_mode"),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-IRH-01", "std::string runtime_support_library_contract_id;"),
    SnippetCheck("M251-D001-IRH-02", "bool runtime_support_library_ready_for_skeleton = false;"),
    SnippetCheck("M251-D001-IRH-03", "std::string runtime_support_library_dispatch_i32_symbol;"),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-IRC-01", "; runtime_support_library = "),
    SnippetCheck("M251-D001-IRC-02", "!objc3.objc_runtime_support_library = !{!51}"),
    SnippetCheck("M251-D001-IRC-03", 'out << "!51 = !{!\\"'),
    SnippetCheck("M251-D001-IRC-04", "runtime_support_library_driver_link_mode"),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-DRV-01", "M251-D001 freeze: manifest emission also publishes the reserved native"),
    SnippetCheck("M251-D001-DRV-02", "runtime support-library surface (target/header/entrypoints/link mode)"),
)

RUNTIME_README_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-RTR-01", "`M251-D001` reserves the real native runtime-library surface separately:"),
    SnippetCheck("M251-D001-RTR-02", "- `objc3_runtime`"),
    SnippetCheck("M251-D001-RTR-03", "This shim is not that runtime library and remains test-only evidence."),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-SHIM-01", "M251-D001 freeze: the canonical native runtime-library surface is reserved for"),
    SnippetCheck("M251-D001-SHIM-02", "`objc3_runtime_dispatch_i32` / `objc3_runtime_reset_for_testing`; this shim remains"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-D001-PKG-01", '"check:objc3c:m251-d001-native-runtime-library-surface-and-build-contract": "python scripts/check_m251_d001_native_runtime_library_surface_and_build_contract.py"'),
    SnippetCheck("M251-D001-PKG-02", '"test:tooling:m251-d001-native-runtime-library-surface-and-build-contract": "python -m pytest tests/tooling/test_check_m251_d001_native_runtime_library_surface_and_build_contract.py -q"'),
    SnippetCheck("M251-D001-PKG-03", '"check:objc3c:m251-d001-lane-d-readiness": "npm run build:objc3c-native && npm run check:objc3c:m251-d001-native-runtime-library-surface-and-build-contract && npm run test:tooling:m251-d001-native-runtime-library-surface-and-build-contract"'),
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
    parser.add_argument("--runtime-surface-doc", type=Path, default=DEFAULT_RUNTIME_SURFACE_DOC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
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


def run_dynamic_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[list[dict[str, object]], int, int]:
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)
    case_out = probe_root / "hello"
    case_out.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(case_out),
        "--emit-prefix",
        "module",
    ]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        command.extend(["--llc", str(llc)])
    result = run_command(command, ROOT)
    manifest_path = case_out / "module.manifest.json"
    ir_path = case_out / "module.ll"
    case: dict[str, object] = {
        "case_id": "M251-D001-CASE-HELLO",
        "command": command,
        "process_exit_code": result.returncode,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
    }
    checks_total = 4
    checks_passed = 0

    if result.returncode == 0:
        checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D001-DYN-01", f"native compile exited with {result.returncode}"))
    if manifest_path.exists():
        checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D001-DYN-02", f"missing manifest: {manifest_path.as_posix()}"))
    if ir_path.exists():
        checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M251-D001-DYN-03", f"missing IR artifact: {ir_path.as_posix()}"))

    if manifest_path.exists() and ir_path.exists():
        manifest = load_json(manifest_path)
        case["runtime_support_library_contract_id"] = find_first_key(manifest, "runtime_support_library_contract_id")
        case["runtime_support_library_target_name"] = find_first_key(manifest, "runtime_support_library_target_name")
        case["runtime_support_library_public_header_path"] = find_first_key(
            manifest, "runtime_support_library_public_header_path"
        )
        case["runtime_support_library_register_image_symbol"] = find_first_key(
            manifest, "runtime_support_library_register_image_symbol"
        )
        case["runtime_support_library_lookup_selector_symbol"] = find_first_key(
            manifest, "runtime_support_library_lookup_selector_symbol"
        )
        case["runtime_support_library_dispatch_i32_symbol"] = find_first_key(
            manifest, "runtime_support_library_dispatch_i32_symbol"
        )
        case["runtime_support_library_reset_for_testing_symbol"] = find_first_key(
            manifest, "runtime_support_library_reset_for_testing_symbol"
        )
        case["runtime_support_library_driver_link_mode"] = find_first_key(
            manifest, "runtime_support_library_driver_link_mode"
        )
        case["runtime_support_library_shim_remains_test_only"] = find_first_key(
            manifest, "runtime_support_library_shim_remains_test_only"
        )
        ir_text = read_text(ir_path)
        case["ir_contract_comment_present"] = f"; runtime_support_library = {CONTRACT_ID}" in ir_text
        case["ir_named_metadata_present"] = "!objc3.objc_runtime_support_library = !{!51}" in ir_text
        expected_manifest = (
            case["runtime_support_library_contract_id"] == CONTRACT_ID
            and case["runtime_support_library_target_name"] == TARGET_NAME
            and case["runtime_support_library_public_header_path"] == PUBLIC_HEADER
            and case["runtime_support_library_register_image_symbol"] == REGISTER_IMAGE
            and case["runtime_support_library_lookup_selector_symbol"] == LOOKUP_SELECTOR
            and case["runtime_support_library_dispatch_i32_symbol"] == DISPATCH_I32
            and case["runtime_support_library_reset_for_testing_symbol"] == RESET_FOR_TESTING
            and case["runtime_support_library_driver_link_mode"] == LINK_MODE
            and case["runtime_support_library_shim_remains_test_only"] is True
        )
        if expected_manifest and case["ir_contract_comment_present"] and case["ir_named_metadata_present"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-D001-DYN-04", "manifest/IR runtime-library contract drifted"))
    else:
        failures.append(Finding("dynamic", "M251-D001-DYN-04", "manifest/IR validation prerequisites missing"))

    case["status"] = 0 if checks_passed == checks_total and result.returncode == 0 else 1
    case["success"] = case["status"] == 0
    return [case], checks_total, checks_passed


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
        (args.runtime_surface_doc, RUNTIME_SURFACE_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_h, IR_EMITTER_H_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )

    for path, snippets in artifacts:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    dynamic_checks_total = 0
    dynamic_checks_passed = 0
    if not args.skip_dynamic_probes:
        dynamic_cases, dynamic_checks_total, dynamic_checks_passed = run_dynamic_probe(args, failures)
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
