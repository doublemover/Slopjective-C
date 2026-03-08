#!/usr/bin/env python3
"""Fail-closed checker for M254-C002 live constructor/init-stub emission."""

from __future__ import annotations

import argparse
import importlib.util
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-c002-constructor-init-stub-emission-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-constructor-init-stub-emission/m254-c002-v1"
BOOTSTRAP_LOWERING_CONTRACT_ID = "objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1"
REGISTRATION_MANIFEST_CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
BOOTSTRAP_SEMANTICS_CONTRACT_ID = "objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1"
CTOR_ROOT_SYMBOL = "__objc3_runtime_register_image_ctor"
INIT_STUB_PREFIX = "__objc3_runtime_register_image_init_stub_"
REGISTRATION_TABLE_PREFIX = "__objc3_runtime_registration_table_"
IMAGE_DESCRIPTOR_PREFIX = "__objc3_runtime_image_descriptor_"
GLOBAL_CTOR_MODEL = "llvm.global_ctors-single-root-priority-65535"
IR_COMMENT_PREFIX = "; runtime_bootstrap_ctor_init_emission = contract=objc3c-runtime-constructor-init-stub-emission/m254-c002-v1"
RUNTIME_REGISTRATION_CALL = "call i32 @objc3_runtime_register_image(ptr %bootstrap_image)"
COFF_STARTUP_SECTION = ".CRT$XCU"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_constructor_and_init_stub_emission_core_feature_implementation_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_c002_constructor_and_init_stub_emission_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c002_runtime_bootstrap_metadata_library.objc3"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m254_c002_constructor_startup_probe.cpp"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "c002-constructor-init-stub-emission"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-C002/constructor_init_stub_emission_summary.json")


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
    SnippetCheck("M254-C002-DOC-EXP-01", "# M254 Constructor and Init-Stub Emission Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M254-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-C002-DOC-EXP-03", "`@llvm.global_ctors`"),
    SnippetCheck("M254-C002-DOC-EXP-04", "`tests/tooling/runtime/m254_c002_constructor_startup_probe.cpp`"),
    SnippetCheck("M254-C002-DOC-EXP-05", "`tmp/reports/m254/M254-C002/constructor_init_stub_emission_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-C002-DOC-PKT-01", "# M254-C002 Constructor and Init-Stub Emission Core Feature Implementation Packet"),
    SnippetCheck("M254-C002-DOC-PKT-02", "Packet: `M254-C002`"),
    SnippetCheck("M254-C002-DOC-PKT-03", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C002-DOC-PKT-04", f"`{COFF_STARTUP_SECTION}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M254-C002-ARCH-01", "M254 lane-C C002 constructor/init-stub emission materializes that boundary"),
    SnippetCheck("M254-C002-ARCH-02", "one real ctor root, one derived init stub, one\n  derived registration table"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-C002-NDOC-01", "## Constructor and init-stub emission (M254-C002)"),
    SnippetCheck("M254-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C002-NDOC-03", f"`{COFF_STARTUP_SECTION}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-C002-SPC-01", "## M254 constructor and init-stub emission (C002)"),
    SnippetCheck("M254-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C002-SPC-03", "init stub calls `objc3_runtime_register_image`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-C002-META-01", "## M254 constructor and init-stub emission metadata anchors (C002)"),
    SnippetCheck("M254-C002-META-02", "`bootstrap_registration_table_symbol`"),
    SnippetCheck("M254-C002-META-03", f"`{COFF_STARTUP_SECTION}`"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M254-C002-IR-01", 'out << "; runtime_bootstrap_ctor_init_emission = "'),
    SnippetCheck("M254-C002-IR-02", "EncodeBoundaryTokenValueHex("),
    SnippetCheck("M254-C002-IR-03", "EmitRuntimeBootstrapLoweringFunctions("),
    SnippetCheck("M254-C002-IR-04", 'out << "@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] "'),
    SnippetCheck("M254-C002-IR-05", 'out << "  call void @abort()\\n";'),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-C002-ART-01", "M254-C002 bootstrap materialization anchor"),
    SnippetCheck("M254-C002-ART-02", "runtime_bootstrap_lowering_contract_id"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-C002-DRV-01", "M254-C002 constructor/init-stub emission anchor"),
    SnippetCheck("M254-C002-DRV-02", "manifest now publishes the exact derived registration-table"),
)
PROCESS_SNIPPETS = (
    SnippetCheck("M254-C002-PROC-01", "M254-C002 constructor/init-stub emission anchor"),
    SnippetCheck("M254-C002-PROC-02", '"translation_unit_identity_key_hex"'),
    SnippetCheck("M254-C002-PROC-03", '\\"bootstrap_registration_table_symbol\\": \\"'),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-C002-RUN-01", "`M254-C002` then turns that frozen boundary into a live emitted startup path"),
    SnippetCheck("M254-C002-RUN-02", "startup probes link a metadata-only `.objc3` object"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M254-C002-FIX-01", "module runtimeBootstrapLibrary;"),
    SnippetCheck("M254-C002-FIX-02", "@interface Widget<Worker>"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M254-C002-PRB-01", "objc3_runtime_copy_registration_state_for_testing"),
    SnippetCheck("M254-C002-PRB-02", '"\\\"registered_image_count\\\":"'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-C002-PKG-01", '"check:objc3c:m254-c002-constructor-and-init-stub-emission-core-feature-implementation": "python scripts/check_m254_c002_constructor_and_init_stub_emission_core_feature_implementation.py"'),
    SnippetCheck("M254-C002-PKG-02", '"test:tooling:m254-c002-constructor-and-init-stub-emission-core-feature-implementation": "python -m pytest tests/tooling/test_check_m254_c002_constructor_and_init_stub_emission_core_feature_implementation.py -q"'),
    SnippetCheck("M254-C002-PKG-03", '"check:objc3c:m254-c002-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m254-c001-bootstrap-lowering-contract-and-architecture-freeze && npm run test:tooling:m254-c001-bootstrap-lowering-contract-and-architecture-freeze && npm run check:objc3c:m254-c002-constructor-and-init-stub-emission-core-feature-implementation && npm run test:tooling:m254-c002-constructor-and-init-stub-emission-core-feature-implementation"'),
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
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--runtime-library", type=Path, default=RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-objdump", default="llvm-objdump")
    parser.add_argument("--llvm-readobj", default="llvm-readobj")
    parser.add_argument("--clangxx", default="clang++.exe")
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



def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0



def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload



def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )



def resolve_tool(name: str) -> str | None:
    if Path(name).is_file():
        return str(Path(name).resolve())
    return shutil.which(name)



def make_identifier_safe_suffix(text: str) -> str:
    out = "".join(ch if (ch.isalnum() or ch == "_") else "_" for ch in text)
    return out or "module"


def native_exe_is_stale(native_exe: Path, freshness_inputs: Sequence[Path]) -> bool:
    if not native_exe.exists():
        return True
    native_mtime = native_exe.stat().st_mtime
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)



def extract_section_names(readobj_stdout: str) -> list[str]:
    names: list[str] = []
    for line in readobj_stdout.splitlines():
        stripped = line.strip()
        if stripped.startswith("Name: "):
            names.append(stripped[len("Name: ") :].split(" (", 1)[0])
    return names



def collect_symbol_lines(objdump_stdout: str) -> list[str]:
    return [line.strip() for line in objdump_stdout.splitlines() if line.strip()]



def run_dynamic_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object] | None]:
    checks_total = 0
    checks_passed = 0

    native_exe = args.native_exe.resolve()
    fixture = args.fixture.resolve()
    runtime_probe = args.runtime_probe.resolve()
    runtime_library = args.runtime_library.resolve()
    runtime_include_root = args.runtime_include_root.resolve()
    out_dir = args.probe_root.resolve() / "startup-library"
    out_dir.mkdir(parents=True, exist_ok=True)

    compile_command = [
        str(native_exe),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_command)
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"

    case: dict[str, object] = {
        "fixture": display_path(fixture),
        "compile_command": compile_command,
        "compile_exit_code": compile_result.returncode,
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total += 1
    checks_passed += require(native_exe.exists(), display_path(native_exe), "M254-C002-NATIVE-EXE", "native executable is missing", failures)
    checks_total += 1
    checks_passed += require(fixture.exists(), display_path(fixture), "M254-C002-FIXTURE", "fixture is missing", failures)
    checks_total += 1
    checks_passed += require(runtime_probe.exists(), display_path(runtime_probe), "M254-C002-PROBE", "runtime probe source is missing", failures)
    checks_total += 1
    checks_passed += require(runtime_library.exists(), display_path(runtime_library), "M254-C002-RUNTIME-LIB", "runtime library is missing", failures)
    checks_total += 1
    checks_passed += require(
        not native_exe_is_stale(
            native_exe,
            (
                args.ir_emitter.resolve(),
                args.frontend_artifacts.resolve(),
                args.driver_cpp.resolve(),
                args.process_cpp.resolve(),
                fixture,
                runtime_probe,
            ),
        ),
        display_path(native_exe),
        "M254-C002-NATIVE-STALE",
        "native executable is older than the C002 implementation inputs; rebuild before running the dynamic probe",
        failures,
    )
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(fixture), "M254-C002-COMPILE", f"native compile exited with {compile_result.returncode}", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M254-C002-MANIFEST", "manifest artifact is missing", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M254-C002-REGFILE", "registration manifest is missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M254-C002-IR", "IR artifact is missing", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M254-C002-OBJ", "object artifact is missing", failures)
    if failures:
        return checks_total, checks_passed, case

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    lowering = (
        manifest_payload.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_runtime_bootstrap_lowering_contract")
    )
    checks_total += 1
    checks_passed += require(isinstance(lowering, dict), display_path(manifest_path), "M254-C002-LOWERING-SURFACE", "bootstrap lowering surface is missing", failures)
    if not isinstance(lowering, dict):
        return checks_total, checks_passed, case

    translation_unit_identity_key = registration_manifest_payload.get("translation_unit_identity_key")
    expected_suffix = make_identifier_safe_suffix(str(translation_unit_identity_key))
    expected_init_stub_symbol = INIT_STUB_PREFIX + expected_suffix
    expected_registration_table_symbol = REGISTRATION_TABLE_PREFIX + expected_suffix

    checks_total += 1
    checks_passed += require(lowering.get("contract_id") == BOOTSTRAP_LOWERING_CONTRACT_ID, display_path(manifest_path), "M254-C002-LOWERING-CONTRACT", "bootstrap lowering contract drifted", failures)
    checks_total += 1
    checks_passed += require(lowering.get("ready_for_bootstrap_materialization") is True, display_path(manifest_path), "M254-C002-LOWERING-READY", "bootstrap lowering readiness must stay true", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(registration_manifest_path), "M254-C002-REGFILE-CONTRACT", "registration manifest contract drifted", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_semantics_contract_id") == BOOTSTRAP_SEMANTICS_CONTRACT_ID, display_path(registration_manifest_path), "M254-C002-REGFILE-SEMANTICS", "bootstrap semantics contract drifted", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("constructor_root_symbol") == CTOR_ROOT_SYMBOL, display_path(registration_manifest_path), "M254-C002-REGFILE-CTOR", "constructor root symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("constructor_init_stub_symbol") == expected_init_stub_symbol, display_path(registration_manifest_path), "M254-C002-REGFILE-INIT", "registration manifest init stub symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_symbol") == expected_registration_table_symbol, display_path(registration_manifest_path), "M254-C002-REGFILE-TABLE", "registration manifest registration-table symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_symbol_prefix") == REGISTRATION_TABLE_PREFIX, display_path(registration_manifest_path), "M254-C002-REGFILE-TABLE-PREFIX", "registration-table prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("translation_unit_registration_order_ordinal") == 1, display_path(registration_manifest_path), "M254-C002-REGFILE-ORDINAL", "startup registration order must stay one for the first image", failures)

    ir_text = read_text(ir_path)
    case["ir_comment_present"] = IR_COMMENT_PREFIX in ir_text
    checks_total += 1
    checks_passed += require(IR_COMMENT_PREFIX in ir_text, display_path(ir_path), "M254-C002-IR-COMMENT", "IR bootstrap materialization comment is missing", failures)
    checks_total += 1
    checks_passed += require(f"constructor_root_symbol={CTOR_ROOT_SYMBOL}" in ir_text, display_path(ir_path), "M254-C002-IR-CTOR", "IR comment must publish ctor root symbol", failures)
    checks_total += 1
    checks_passed += require(f"constructor_init_stub_symbol={expected_init_stub_symbol}" in ir_text, display_path(ir_path), "M254-C002-IR-INIT", "IR comment must publish exact init stub symbol", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_symbol={expected_registration_table_symbol}" in ir_text, display_path(ir_path), "M254-C002-IR-TABLE", "IR comment must publish exact registration-table symbol", failures)
    checks_total += 1
    checks_passed += require("@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }]" in ir_text, display_path(ir_path), "M254-C002-IR-GLOBAL-CTORS", "IR must materialize llvm.global_ctors", failures)
    checks_total += 1
    checks_passed += require(f"ptr @{CTOR_ROOT_SYMBOL}, ptr @{expected_registration_table_symbol}" in ir_text, display_path(ir_path), "M254-C002-IR-GLOBAL-CTORS-PAYLOAD", "global ctor payload must reference ctor root and registration table", failures)
    checks_total += 1
    checks_passed += require(f"define internal void @{expected_init_stub_symbol}()" in ir_text, display_path(ir_path), "M254-C002-IR-INIT-DEF", "IR must define the derived init stub", failures)
    checks_total += 1
    checks_passed += require(f"define internal void @{CTOR_ROOT_SYMBOL}()" in ir_text, display_path(ir_path), "M254-C002-IR-CTOR-DEF", "IR must define the ctor root", failures)
    checks_total += 1
    checks_passed += require(RUNTIME_REGISTRATION_CALL in ir_text, display_path(ir_path), "M254-C002-IR-RUNTIME-CALL", "init stub must call objc3_runtime_register_image", failures)
    checks_total += 1
    checks_passed += require("call void @abort()" in ir_text, display_path(ir_path), "M254-C002-IR-ABORT", "init stub must abort on non-zero registration status", failures)

    llvm_objdump = resolve_tool(args.llvm_objdump)
    llvm_readobj = resolve_tool(args.llvm_readobj)
    clangxx = resolve_tool(args.clangxx)
    checks_total += 1
    checks_passed += require(llvm_objdump is not None, display_path(obj_path), "M254-C002-OBJDUMP", "llvm-objdump is required", failures)
    checks_total += 1
    checks_passed += require(llvm_readobj is not None, display_path(obj_path), "M254-C002-READOBJ", "llvm-readobj is required", failures)
    checks_total += 1
    checks_passed += require(clangxx is not None, display_path(runtime_probe), "M254-C002-CLANGXX", "clang++ is required", failures)
    if failures:
        return checks_total, checks_passed, case

    objdump_result = run_command([str(llvm_objdump), "--syms", str(obj_path)])
    readobj_result = run_command([str(llvm_readobj), "--sections", str(obj_path)])
    checks_total += 1
    checks_passed += require(objdump_result.returncode == 0, display_path(obj_path), "M254-C002-OBJDUMP-RUN", f"llvm-objdump exited with {objdump_result.returncode}", failures)
    checks_total += 1
    checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), "M254-C002-READOBJ-RUN", f"llvm-readobj exited with {readobj_result.returncode}", failures)
    if failures:
        return checks_total, checks_passed, case

    symbol_lines = collect_symbol_lines(objdump_result.stdout)
    sections = extract_section_names(readobj_result.stdout)
    case["symbol_count"] = len(symbol_lines)
    case["sections"] = sections
    checks_total += 1
    checks_passed += require(any(CTOR_ROOT_SYMBOL in line for line in symbol_lines), display_path(obj_path), "M254-C002-OBJ-CTOR", "object symbols must contain the ctor root", failures)
    checks_total += 1
    checks_passed += require(any(expected_init_stub_symbol in line for line in symbol_lines), display_path(obj_path), "M254-C002-OBJ-INIT", "object symbols must contain the derived init stub", failures)
    checks_total += 1
    checks_passed += require(any(expected_registration_table_symbol in line for line in symbol_lines), display_path(obj_path), "M254-C002-OBJ-TABLE", "object symbols must contain the derived registration table", failures)
    checks_total += 1
    checks_passed += require(COFF_STARTUP_SECTION in sections, display_path(obj_path), "M254-C002-OBJ-CRT", "COFF object must contain the startup ctor section", failures)

    probe_out_dir = args.probe_root.resolve() / "probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / "m254_c002_constructor_startup_probe.exe"
    probe_compile_command = [
        str(clangxx),
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        f"-I{runtime_include_root}",
        str(runtime_probe),
        str(obj_path),
        str(runtime_library),
        "-o",
        str(probe_exe),
    ]
    probe_compile = run_command(probe_compile_command)
    checks_total += 1
    checks_passed += require(probe_compile.returncode == 0, display_path(runtime_probe), "M254-C002-PROBE-COMPILE", f"probe compile exited with {probe_compile.returncode}", failures)
    checks_total += 1
    checks_passed += require(probe_exe.exists(), display_path(probe_exe), "M254-C002-PROBE-EXE", "probe executable is missing", failures)
    if failures:
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(probe_run.returncode == 0, display_path(probe_exe), "M254-C002-PROBE-RUN", f"probe exited with {probe_run.returncode}", failures)
    if probe_run.returncode != 0:
        return checks_total, checks_passed, case

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), "M254-C002-PROBE-JSON", f"invalid probe JSON: {exc}"))
        return checks_total + 1, checks_passed, case

    expected_total = registration_manifest_payload.get("total_descriptor_count")
    expected_order = registration_manifest_payload.get("translation_unit_registration_order_ordinal")
    expected_next = expected_order + 1 if isinstance(expected_order, int) else None
    checks_total += 1
    checks_passed += require(probe_payload.get("copy_status") == 0, display_path(probe_exe), "M254-C002-PROBE-COPY", "runtime snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("registered_image_count") == 1, display_path(probe_exe), "M254-C002-PROBE-IMAGE-COUNT", "startup probe must observe exactly one registered image", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("registered_descriptor_total") == expected_total, display_path(probe_exe), "M254-C002-PROBE-DESCRIPTOR-TOTAL", "startup probe descriptor total mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("next_expected_registration_order_ordinal") == expected_next, display_path(probe_exe), "M254-C002-PROBE-NEXT-ORDER", "startup probe next order mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("last_successful_registration_order_ordinal") == expected_order, display_path(probe_exe), "M254-C002-PROBE-LAST-ORDER", "startup probe last successful order mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("last_registration_status") == 0, display_path(probe_exe), "M254-C002-PROBE-STATUS", "startup probe registration status mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("last_registered_translation_unit_identity_key") == translation_unit_identity_key, display_path(probe_exe), "M254-C002-PROBE-TU", "startup probe identity key mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("last_rejected_registration_order_ordinal") == 0, display_path(probe_exe), "M254-C002-PROBE-REJECTED-ORDINAL", "happy path must not record a rejected registration", failures)

    case.update(
        {
            "constructor_init_stub_symbol": expected_init_stub_symbol,
            "bootstrap_registration_table_symbol": expected_registration_table_symbol,
            "translation_unit_identity_key": translation_unit_identity_key,
            "probe_compile_command": probe_compile_command,
            "probe_run_command": [str(probe_exe)],
            "probe_payload": probe_payload,
            "descriptor_total": expected_total,
        }
    )
    return checks_total, checks_passed, case



def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_inputs = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.fixture, FIXTURE_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_inputs:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        probe_total, probe_passed, case = run_dynamic_probe(args, failures)
        checks_total += probe_total
        checks_passed += probe_passed
        if case is not None:
            dynamic_cases.append(case)

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
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
