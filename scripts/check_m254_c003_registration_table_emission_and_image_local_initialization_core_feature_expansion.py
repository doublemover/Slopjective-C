#!/usr/bin/env python3
"""Fail-closed checker for M254-C003 registration-table/image-local-init expansion."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-c003-registration-table-image-local-initialization-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-runtime-registration-table-image-local-initialization/m254-c003-v1"
C002_CONTRACT_ID = "objc3c-runtime-constructor-init-stub-emission/m254-c002-v1"
LOWERING_CONTRACT_ID = "objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1"
REGISTRATION_MANIFEST_CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
REGISTRATION_TABLE_LAYOUT_MODEL = (
    "abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-image-local-init-state"
)
IMAGE_LOCAL_INITIALIZATION_MODEL = "guarded-once-per-image-local-state-cell"
REGISTRATION_TABLE_SYMBOL_PREFIX = "__objc3_runtime_registration_table_"
IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX = "__objc3_runtime_image_local_init_state_"
CTOR_ROOT_SYMBOL = "__objc3_runtime_register_image_ctor"
INIT_STUB_PREFIX = "__objc3_runtime_register_image_init_stub_"
TABLE_IR_COMMENT_PREFIX = (
    "; runtime_registration_table_image_local_initialization = contract=" + CONTRACT_ID
)
C002_IR_COMMENT_PREFIX = "; runtime_bootstrap_ctor_init_emission = contract=" + C002_CONTRACT_ID
REGISTRATION_TABLE_TYPE = "{ i64, i64, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr }"
REGISTRATION_TABLE_ABI_VERSION = 1
REGISTRATION_TABLE_POINTER_FIELD_COUNT = 11
COFF_STARTUP_SECTION = ".CRT$XCU"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_registration_table_emission_and_image_local_initialization_core_feature_expansion_c003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_IR_EMITTER_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PRIMARY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c002_runtime_bootstrap_metadata_library.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c003_runtime_bootstrap_category_library.objc3"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m254_c003_registration_table_startup_probe.cpp"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "c003-registration-table-image-local-init"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-C003/registration_table_image_local_initialization_summary.json")


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
    SnippetCheck("M254-C003-DOC-EXP-01", "# M254 Registration Table Emission and Image-Local Initialization Core Feature Expansion Expectations (C003)"),
    SnippetCheck("M254-C003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-C003-DOC-EXP-03", f"`{REGISTRATION_TABLE_LAYOUT_MODEL}`"),
    SnippetCheck("M254-C003-DOC-EXP-04", f"`{IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX}`"),
    SnippetCheck("M254-C003-DOC-EXP-05", "`tmp/reports/m254/M254-C003/registration_table_image_local_initialization_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-C003-DOC-PKT-01", "# M254-C003 Registration Table Emission and Image-Local Initialization Core Feature Expansion Packet"),
    SnippetCheck("M254-C003-DOC-PKT-02", "Packet: `M254-C003`"),
    SnippetCheck("M254-C003-DOC-PKT-03", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C003-DOC-PKT-04", "self-describing registration table"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M254-C003-ARCH-01", "M254 lane-C C003 registration-table/image-local-init expansion"),
    SnippetCheck("M254-C003-ARCH-02", "image-local init-state cell"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-C003-NDOC-01", "## Registration table emission and image-local initialization (M254-C003)"),
    SnippetCheck("M254-C003-NDOC-02", f"`{REGISTRATION_TABLE_LAYOUT_MODEL}`"),
    SnippetCheck("M254-C003-NDOC-03", f"`{IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-C003-SPC-01", "## M254 registration-table emission and image-local initialization (C003)"),
    SnippetCheck("M254-C003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C003-SPC-03", "image-local init-state cell"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-C003-META-01", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C003-META-02", f"`{IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX}`"),
    SnippetCheck("M254-C003-META-03", "bootstrap_image_local_init_state_symbol"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M254-C003-IR-01", 'out << "; runtime_registration_table_image_local_initialization = "'),
    SnippetCheck("M254-C003-IR-02", "RuntimeBootstrapImageLocalInitStateSymbol()"),
    SnippetCheck("M254-C003-IR-03", "guarded-once-before-runtime-registration"),
    SnippetCheck("M254-C003-IR-04", "internal global i8 0, align 1"),
    SnippetCheck("M254-C003-IR-05", "%bootstrap_already_initialized = icmp ne i8 %bootstrap_state, 0"),
)
IR_HEADER_SNIPPETS = (
    SnippetCheck("M254-C003-IRH-01", "runtime_bootstrap_lowering_image_local_init_state_symbol_prefix"),
    SnippetCheck("M254-C003-IRH-02", "runtime_bootstrap_lowering_registration_table_layout_model"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M254-C003-TYPES-01", "image_local_init_state_symbol_prefix"),
    SnippetCheck("M254-C003-TYPES-02", "registration_table_layout_model"),
    SnippetCheck("M254-C003-TYPES-03", "bootstrap_ir_materialization_landed"),
    SnippetCheck("M254-C003-TYPES-04", "image_local_initialization_landed"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-C003-ART-01", "M254-C003 registration-table/image-local-init anchor"),
    SnippetCheck("M254-C003-ART-02", "runtime_bootstrap_lowering_image_local_init_state_symbol_prefix"),
    SnippetCheck("M254-C003-ART-03", "registration_table_layout_model"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-C003-DRV-01", "M254-C003 registration-table/image-local-init anchor"),
    SnippetCheck("M254-C003-DRV-02", "bootstrap_image_local_init_state_symbol_prefix"),
)
PROCESS_HEADER_SNIPPETS = (
    SnippetCheck("M254-C003-PROCH-01", "std::string bootstrap_registration_table_layout_model;"),
    SnippetCheck("M254-C003-PROCH-02", "std::string bootstrap_image_local_init_state_symbol_prefix;"),
    SnippetCheck("M254-C003-PROCH-03", "std::uint64_t bootstrap_registration_table_pointer_field_count = 0;"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-C003-PROC-01", "M254-C003 registration-table/image-local-init anchor"),
    SnippetCheck("M254-C003-PROC-02", '\\"bootstrap_registration_table_layout_model\\": \\"'),
    SnippetCheck("M254-C003-PROC-03", '\\"bootstrap_image_local_init_state_symbol\\": \\"'),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M254-C003-FRONT-01", "bootstrap_registration_table_layout_model"),
    SnippetCheck("M254-C003-FRONT-02", "bootstrap_image_local_init_state_symbol_prefix"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-C003-RUN-01", "`M254-C003` expands the same emitted startup path"),
    SnippetCheck("M254-C003-RUN-02", "image-local init-state cell"),
)
PRIMARY_FIXTURE_SNIPPETS = (
    SnippetCheck("M254-C003-FIX1-01", "module runtimeBootstrapLibrary;"),
    SnippetCheck("M254-C003-FIX1-02", "@interface Widget<Worker>"),
)
CATEGORY_FIXTURE_SNIPPETS = (
    SnippetCheck("M254-C003-FIX2-01", "module runtimeBootstrapCategoryLibrary;"),
    SnippetCheck("M254-C003-FIX2-02", "@interface Widget (Debug)<Worker>"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M254-C003-PRB-01", "objc3_runtime_copy_registration_state_for_testing"),
    SnippetCheck("M254-C003-PRB-02", '"\\"registered_image_count\\":"'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-C003-PKG-01", '"check:objc3c:m254-c003-registration-table-emission-and-image-local-initialization-core-feature-expansion": "python scripts/check_m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion.py"'),
    SnippetCheck("M254-C003-PKG-02", '"test:tooling:m254-c003-registration-table-emission-and-image-local-initialization-core-feature-expansion": "python -m pytest tests/tooling/test_check_m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion.py -q"'),
    SnippetCheck("M254-C003-PKG-03", '"check:objc3c:m254-c003-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m254-c002-lane-c-readiness && npm run check:objc3c:m254-c003-registration-table-emission-and-image-local-initialization-core-feature-expansion && npm run test:tooling:m254-c003-registration-table-emission-and-image-local-initialization-core-feature-expansion"'),
)


@dataclass(frozen=True)
class DynamicCase:
    case_id: str
    fixture: Path
    expect_category_descriptors: bool


DYNAMIC_CASES = (
    DynamicCase("metadata-library", DEFAULT_PRIMARY_FIXTURE, False),
    DynamicCase("category-library", DEFAULT_CATEGORY_FIXTURE, True),
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
    parser.add_argument("--ir-emitter-header", type=Path, default=DEFAULT_IR_EMITTER_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--primary-fixture", type=Path, default=DEFAULT_PRIMARY_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
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



def dynamic_cases(args: argparse.Namespace) -> tuple[DynamicCase, ...]:
    return (
        DynamicCase("metadata-library", args.primary_fixture.resolve(), False),
        DynamicCase("category-library", args.category_fixture.resolve(), True),
    )



def run_dynamic_case(
    args: argparse.Namespace,
    case_input: DynamicCase,
    failures: list[Finding],
    llvm_objdump: str,
    llvm_readobj: str,
    clangxx: str,
) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    native_exe = args.native_exe.resolve()
    runtime_probe = args.runtime_probe.resolve()
    runtime_library = args.runtime_library.resolve()
    runtime_include_root = args.runtime_include_root.resolve()
    out_dir = args.probe_root.resolve() / case_input.case_id
    out_dir.mkdir(parents=True, exist_ok=True)

    compile_command = [
        str(native_exe),
        str(case_input.fixture),
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
        "case_id": case_input.case_id,
        "fixture": display_path(case_input.fixture),
        "compile_command": compile_command,
        "compile_exit_code": compile_result.returncode,
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total += 1
    checks_passed += require(case_input.fixture.exists(), display_path(case_input.fixture), f"M254-C003-{case_input.case_id}-FIXTURE", "fixture is missing", failures)
    checks_total += 1
    checks_passed += require(runtime_probe.exists(), display_path(runtime_probe), f"M254-C003-{case_input.case_id}-PROBE", "runtime probe is missing", failures)
    checks_total += 1
    checks_passed += require(runtime_library.exists(), display_path(runtime_library), f"M254-C003-{case_input.case_id}-RUNTIME-LIB", "runtime library is missing", failures)
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(case_input.fixture), f"M254-C003-{case_input.case_id}-COMPILE", f"native compile exited with {compile_result.returncode}", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), f"M254-C003-{case_input.case_id}-MANIFEST", "manifest artifact is missing", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_path.exists(), display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REGFILE", "registration manifest is missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), f"M254-C003-{case_input.case_id}-IR", "IR artifact is missing", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), f"M254-C003-{case_input.case_id}-OBJ", "object artifact is missing", failures)
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
    checks_passed += require(isinstance(lowering, dict), display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-SURFACE", "bootstrap lowering surface is missing", failures)
    if not isinstance(lowering, dict):
        return checks_total, checks_passed, case

    translation_unit_identity_key = registration_manifest_payload.get("translation_unit_identity_key")
    expected_suffix = make_identifier_safe_suffix(str(translation_unit_identity_key))
    expected_registration_table_symbol = REGISTRATION_TABLE_SYMBOL_PREFIX + expected_suffix
    expected_init_stub_symbol = INIT_STUB_PREFIX + expected_suffix
    expected_image_local_init_state_symbol = IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX + expected_suffix

    checks_total += 1
    checks_passed += require(lowering.get("contract_id") == LOWERING_CONTRACT_ID, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-CONTRACT", "bootstrap lowering contract drifted", failures)
    checks_total += 1
    checks_passed += require(lowering.get("registration_table_layout_model") == REGISTRATION_TABLE_LAYOUT_MODEL, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-LAYOUT", "registration-table layout model mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("image_local_initialization_model") == IMAGE_LOCAL_INITIALIZATION_MODEL, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-INIT-MODEL", "image-local initialization model mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("image_local_init_state_symbol_prefix") == IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-INIT-PREFIX", "image-local init-state prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("registration_table_abi_version") == REGISTRATION_TABLE_ABI_VERSION, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-ABI", "registration-table ABI version mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("registration_table_pointer_field_count") == REGISTRATION_TABLE_POINTER_FIELD_COUNT, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-FIELD-COUNT", "registration-table pointer-field count mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("bootstrap_ir_materialization_landed") is True, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-MATERIALIZED", "bootstrap materialization must be marked landed", failures)
    checks_total += 1
    checks_passed += require(lowering.get("image_local_initialization_landed") is True, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-IMAGE-LOCAL", "image-local initialization must be marked landed", failures)
    checks_total += 1
    checks_passed += require(lowering.get("no_bootstrap_ir_materialization_yet") is False, display_path(manifest_path), f"M254-C003-{case_input.case_id}-LOWERING-NONGOAL", "lowering surface must stop advertising no bootstrap materialization", failures)

    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-CONTRACT", "registration manifest contract drifted", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_layout_model") == REGISTRATION_TABLE_LAYOUT_MODEL, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-LAYOUT", "registration manifest layout model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_image_local_initialization_model") == IMAGE_LOCAL_INITIALIZATION_MODEL, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-INIT-MODEL", "registration manifest image-local initialization model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_image_local_init_state_symbol_prefix") == IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-INIT-PREFIX", "registration manifest init-state prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_image_local_init_state_symbol") == expected_image_local_init_state_symbol, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-INIT-SYMBOL", "registration manifest init-state symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_symbol") == expected_registration_table_symbol, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-TABLE-SYMBOL", "registration manifest registration-table symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("constructor_init_stub_symbol") == expected_init_stub_symbol, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-INIT-STUB", "registration manifest init-stub symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_abi_version") == REGISTRATION_TABLE_ABI_VERSION, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-ABI", "registration manifest ABI version mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_pointer_field_count") == REGISTRATION_TABLE_POINTER_FIELD_COUNT, display_path(registration_manifest_path), f"M254-C003-{case_input.case_id}-REG-FIELD-COUNT", "registration manifest pointer-field count mismatch", failures)

    category_count = registration_manifest_payload.get("category_descriptor_count")
    checks_total += 1
    checks_passed += require(
        (isinstance(category_count, int) and category_count > 0)
        if case_input.expect_category_descriptors
        else category_count == 0,
        display_path(registration_manifest_path),
        f"M254-C003-{case_input.case_id}-REG-CATEGORY-COUNT",
        "category descriptor count did not match the expected edge case",
        failures,
    )

    ir_text = read_text(ir_path)
    table_comment = next((line for line in ir_text.splitlines() if line.startswith(TABLE_IR_COMMENT_PREFIX)), "")
    c002_comment = next((line for line in ir_text.splitlines() if line.startswith(C002_IR_COMMENT_PREFIX)), "")
    case["table_comment_present"] = bool(table_comment)
    case["c002_comment_present"] = bool(c002_comment)
    checks_total += 1
    checks_passed += require(bool(table_comment), display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-TABLE-COMMENT", "IR must publish the C003 registration-table/image-local-init boundary", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_symbol={expected_registration_table_symbol}" in table_comment, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-TABLE-SYMBOL", "IR table comment must publish the exact registration-table symbol", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_layout_model={REGISTRATION_TABLE_LAYOUT_MODEL}" in table_comment, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-LAYOUT", "IR table comment must publish the layout model", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_abi_version={REGISTRATION_TABLE_ABI_VERSION}" in table_comment, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-ABI", "IR table comment must publish the ABI version", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_pointer_field_count={REGISTRATION_TABLE_POINTER_FIELD_COUNT}" in table_comment, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-FIELD-COUNT", "IR table comment must publish the pointer-field count", failures)
    checks_total += 1
    checks_passed += require(f"image_local_init_state_symbol={expected_image_local_init_state_symbol}" in table_comment, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-INIT-SYMBOL", "IR table comment must publish the exact init-state symbol", failures)
    checks_total += 1
    checks_passed += require(f"image_local_initialization_model={IMAGE_LOCAL_INITIALIZATION_MODEL}" in table_comment, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-INIT-MODEL", "IR table comment must publish the image-local initialization model", failures)
    checks_total += 1
    checks_passed += require(
        f"@{expected_image_local_init_state_symbol} = internal global i8 0, align 1" in ir_text,
        display_path(ir_path),
        f"M254-C003-{case_input.case_id}-IR-INIT-GLOBAL",
        "IR must emit the exact image-local init-state global",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        f"@{expected_registration_table_symbol} = internal constant {REGISTRATION_TABLE_TYPE} {{ i64 {REGISTRATION_TABLE_ABI_VERSION}, i64 {REGISTRATION_TABLE_POINTER_FIELD_COUNT}" in ir_text,
        display_path(ir_path),
        f"M254-C003-{case_input.case_id}-IR-TABLE-TYPE",
        "IR must emit the self-describing registration-table constant",
        failures,
    )
    checks_total += 1
    checks_passed += require("%bootstrap_state_slot = getelementptr inbounds " in ir_text, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-STATE-SLOT", "IR must address the image-local init-state slot", failures)
    checks_total += 1
    checks_passed += require("%bootstrap_state_cell = load ptr, ptr %bootstrap_state_slot, align 8" in ir_text, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-STATE-CELL", "IR must load the init-state cell pointer", failures)
    checks_total += 1
    checks_passed += require("%bootstrap_already_initialized = icmp ne i8 %bootstrap_state, 0" in ir_text, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-GUARD", "IR must guard runtime registration with the init-state cell", failures)
    checks_total += 1
    checks_passed += require("store i8 1, ptr %bootstrap_state_cell, align 1" in ir_text, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-STORE", "IR must commit success into the init-state cell", failures)
    checks_total += 1
    checks_passed += require("ptr @__objc3_sec_class_descriptors" in ir_text and "ptr @__objc3_sec_protocol_descriptors" in ir_text and "ptr @__objc3_sec_category_descriptors" in ir_text and "ptr @__objc3_sec_property_descriptors" in ir_text and "ptr @__objc3_sec_ivar_descriptors" in ir_text, display_path(ir_path), f"M254-C003-{case_input.case_id}-IR-ROOTS", "IR registration table must carry all canonical section-root pointers", failures)

    objdump_result = run_command([llvm_objdump, "--syms", str(obj_path)])
    readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)])
    checks_total += 1
    checks_passed += require(objdump_result.returncode == 0, display_path(obj_path), f"M254-C003-{case_input.case_id}-OBJDUMP", f"llvm-objdump exited with {objdump_result.returncode}", failures)
    checks_total += 1
    checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), f"M254-C003-{case_input.case_id}-READOBJ", f"llvm-readobj exited with {readobj_result.returncode}", failures)
    if failures:
        return checks_total, checks_passed, case

    symbol_lines = collect_symbol_lines(objdump_result.stdout)
    sections = extract_section_names(readobj_result.stdout)
    case["symbol_count"] = len(symbol_lines)
    case["sections"] = sections
    checks_total += 1
    checks_passed += require(any(expected_registration_table_symbol in line for line in symbol_lines), display_path(obj_path), f"M254-C003-{case_input.case_id}-OBJ-TABLE", "object symbols must contain the registration table", failures)
    checks_total += 1
    checks_passed += require(any(expected_image_local_init_state_symbol in line for line in symbol_lines), display_path(obj_path), f"M254-C003-{case_input.case_id}-OBJ-INIT", "object symbols must contain the image-local init-state symbol", failures)
    checks_total += 1
    checks_passed += require(COFF_STARTUP_SECTION in sections, display_path(obj_path), f"M254-C003-{case_input.case_id}-OBJ-CRT", "COFF object must contain the startup ctor section", failures)

    probe_out_dir = args.probe_root.resolve() / f"{case_input.case_id}-probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / f"{case_input.case_id}-startup-probe.exe"
    probe_compile_command = [
        clangxx,
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
    checks_passed += require(probe_compile.returncode == 0, display_path(runtime_probe), f"M254-C003-{case_input.case_id}-PROBE-COMPILE", f"probe compile exited with {probe_compile.returncode}", failures)
    checks_total += 1
    checks_passed += require(probe_exe.exists(), display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-EXE", "probe executable is missing", failures)
    if failures:
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(probe_run.returncode == 0, display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-RUN", f"probe exited with {probe_run.returncode}", failures)
    if probe_run.returncode != 0:
        return checks_total, checks_passed, case

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-JSON", f"invalid probe JSON: {exc}"))
        return checks_total + 1, checks_passed, case

    expected_total = registration_manifest_payload.get("total_descriptor_count")
    checks_total += 1
    checks_passed += require(probe_payload.get("copy_status") == 0, display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-COPY", "runtime snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("registered_image_count") == 1, display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-COUNT", "startup probe must observe exactly one registered image", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("registered_descriptor_total") == expected_total, display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-TOTAL", "startup probe descriptor total mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("last_registration_status") == 0, display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-STATUS", "startup probe registration status mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("last_registered_translation_unit_identity_key") == translation_unit_identity_key, display_path(probe_exe), f"M254-C003-{case_input.case_id}-PROBE-TU", "startup probe identity key mismatch", failures)

    case.update(
        {
            "registration_table_symbol": expected_registration_table_symbol,
            "image_local_init_state_symbol": expected_image_local_init_state_symbol,
            "translation_unit_identity_key": translation_unit_identity_key,
            "probe_compile_command": probe_compile_command,
            "probe_run_command": [str(probe_exe)],
            "probe_payload": probe_payload,
            "descriptor_total": expected_total,
            "category_descriptor_count": category_count,
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
        (args.ir_emitter_header, IR_HEADER_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.process_header, PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.frontend_anchor_cpp, FRONTEND_ANCHOR_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.primary_fixture, PRIMARY_FIXTURE_SNIPPETS),
        (args.category_fixture, CATEGORY_FIXTURE_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_inputs:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases_payload: list[dict[str, object]] = []
    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        llvm_objdump = resolve_tool(args.llvm_objdump)
        llvm_readobj = resolve_tool(args.llvm_readobj)
        clangxx = resolve_tool(args.clangxx)
        checks_total += 3
        checks_passed += require(llvm_objdump is not None, display_path(args.native_exe), "M254-C003-TOOL-OBJDUMP", "llvm-objdump is required", failures)
        checks_passed += require(llvm_readobj is not None, display_path(args.native_exe), "M254-C003-TOOL-READOBJ", "llvm-readobj is required", failures)
        checks_passed += require(clangxx is not None, display_path(args.runtime_probe), "M254-C003-TOOL-CLANGXX", "clang++ is required", failures)

        native_exe = args.native_exe.resolve()
        checks_total += 1
        checks_passed += require(
            not native_exe_is_stale(
                native_exe,
                (
                    args.ir_emitter.resolve(),
                    args.ir_emitter_header.resolve(),
                    args.frontend_types.resolve(),
                    args.frontend_artifacts.resolve(),
                    args.driver_cpp.resolve(),
                    args.process_header.resolve(),
                    args.process_cpp.resolve(),
                    args.frontend_anchor_cpp.resolve(),
                    args.primary_fixture.resolve(),
                    args.category_fixture.resolve(),
                    args.runtime_probe.resolve(),
                ),
            ),
            display_path(native_exe),
            "M254-C003-NATIVE-STALE",
            "native executable is older than the C003 implementation inputs; rebuild before running the dynamic probes",
            failures,
        )
        if not failures and llvm_objdump is not None and llvm_readobj is not None and clangxx is not None:
            for case_input in dynamic_cases(args):
                probe_total, probe_passed, case_payload = run_dynamic_case(
                    args,
                    case_input,
                    failures,
                    llvm_objdump,
                    llvm_readobj,
                    clangxx,
                )
                checks_total += probe_total
                checks_passed += probe_passed
                dynamic_cases_payload.append(case_payload)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases_payload,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
