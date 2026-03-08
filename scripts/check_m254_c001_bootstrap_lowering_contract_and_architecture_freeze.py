#!/usr/bin/env python3
"""Fail-closed contract checker for M254-C001 bootstrap lowering freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-c001-bootstrap-lowering-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract"
BOUNDARY_MODEL = "registration-manifest-driven-constructor-root-init-stub-and-registration-table-lowering"
CTOR_ROOT_SYMBOL = "__objc3_runtime_register_image_ctor"
INIT_STUB_SYMBOL_PREFIX = "__objc3_runtime_register_image_init_stub_"
REGISTRATION_TABLE_SYMBOL_PREFIX = "__objc3_runtime_registration_table_"
GLOBAL_CTOR_LIST_MODEL = "llvm.global_ctors-single-root-priority-65535"
DEFERRED_STATE = "deferred-until-m254-c002"
BOUNDARY_COMMENT_PREFIX = "; runtime_bootstrap_lowering_boundary = contract=objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_bootstrap_lowering_contract_and_architecture_freeze_c001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_c001_bootstrap_lowering_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_NATIVE_RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "bootstrap-lowering-freeze"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-C001/bootstrap_lowering_contract_summary.json")
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"


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
    SnippetCheck("M254-C001-DOC-EXP-01", "# M254 Bootstrap Lowering Contract and Architecture Freeze Expectations (C001)"),
    SnippetCheck("M254-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-C001-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-C001-DOC-EXP-04", f"`{REGISTRATION_TABLE_SYMBOL_PREFIX}`"),
    SnippetCheck("M254-C001-DOC-EXP-05", "`@llvm.global_ctors`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-C001-DOC-PKT-01", "# M254-C001 Bootstrap Lowering Contract and Architecture Freeze Packet"),
    SnippetCheck("M254-C001-DOC-PKT-02", "Packet: `M254-C001`"),
    SnippetCheck("M254-C001-DOC-PKT-03", "Dependencies"),
    SnippetCheck("M254-C001-DOC-PKT-04", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C001-DOC-PKT-05", f"`{GLOBAL_CTOR_LIST_MODEL}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M254-C001-ARCH-01", "M254 lane-C C001 bootstrap-lowering freeze"),
    SnippetCheck("M254-C001-ARCH-02", "ctor-root/init-stub/registration-table"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-C001-NDOC-01", "## Bootstrap lowering freeze (M254-C001)"),
    SnippetCheck("M254-C001-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-C001-NDOC-03", f"`{GLOBAL_CTOR_LIST_MODEL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-C001-SPC-01", "## M254 bootstrap lowering freeze (C001)"),
    SnippetCheck("M254-C001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-C001-SPC-03", "no emitted `@llvm.global_ctors` yet"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-C001-META-01", "## M254 bootstrap lowering metadata anchors (C001)"),
    SnippetCheck("M254-C001-META-02", f"`{REGISTRATION_TABLE_SYMBOL_PREFIX}`"),
    SnippetCheck("M254-C001-META-03", "module.runtime-registration-manifest.json"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M254-C001-LHDR-01", "kObjc3RuntimeBootstrapLoweringContractId"),
    SnippetCheck("M254-C001-LHDR-02", "kObjc3RuntimeBootstrapGlobalCtorListModel"),
    SnippetCheck("M254-C001-LHDR-03", "kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix"),
    SnippetCheck("M254-C001-LHDR-04", "Objc3RuntimeBootstrapLoweringBoundarySummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M254-C001-LCPP-01", "M254-C001 bootstrap lowering freeze anchor"),
    SnippetCheck("M254-C001-LCPP-02", "kObjc3RuntimeBootstrapLoweringContractId"),
    SnippetCheck("M254-C001-LCPP-03", "non_goals=no-ctor-root-no-init-stub-no-registration-table-materialized-yet"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M254-C001-IR-01", 'out << "; runtime_bootstrap_lowering_boundary = "'),
    SnippetCheck("M254-C001-IR-02", "Objc3RuntimeBootstrapLoweringBoundarySummary()"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M254-C001-TYPES-01", "struct Objc3RuntimeBootstrapLoweringSummary"),
    SnippetCheck("M254-C001-TYPES-02", f'bootstrap_surface_path =\n      "{SURFACE_PATH}"'),
    SnippetCheck("M254-C001-TYPES-03", "ready_for_bootstrap_materialization"),
)
FRONTEND_ARTIFACTS_HEADER_SNIPPETS = (
    SnippetCheck("M254-C001-ARTH-01", "Objc3RuntimeBootstrapLoweringSummary runtime_bootstrap_lowering_summary;"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-C001-ART-01", "BuildRuntimeBootstrapLoweringSummary("),
    SnippetCheck("M254-C001-ART-02", "BuildRuntimeBootstrapLoweringSummaryJson("),
    SnippetCheck("M254-C001-ART-03", '\",\\\"objc_runtime_bootstrap_lowering_contract\\\":'),
    SnippetCheck("M254-C001-ART-04", 'runtime_bootstrap_lowering_contract_id'),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-C001-DRV-01", "M254-C001 bootstrap-lowering anchor"),
    SnippetCheck("M254-C001-DRV-02", "bootstrap_lowering_contract_id"),
)
PROCESS_HEADER_SNIPPETS = (
    SnippetCheck("M254-C001-PROCH-01", "std::string bootstrap_lowering_contract_id;"),
    SnippetCheck("M254-C001-PROCH-02", "std::string bootstrap_registration_table_symbol_prefix;"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-C001-PROC-01", "M254-C001 bootstrap-lowering anchor"),
    SnippetCheck("M254-C001-PROC-02", '\\\"ready_for_bootstrap_lowering_materialization\\\": true'),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M254-C001-FRONT-01", "runtime_bootstrap_lowering_summary"),
    SnippetCheck("M254-C001-FRONT-02", "manifest_inputs.bootstrap_lowering_contract_id ="),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-C001-RUN-01", "`M254-C001` freezes the lowering-owned startup materialization boundary"),
    SnippetCheck("M254-C001-RUN-02", f"`{REGISTRATION_TABLE_SYMBOL_PREFIX}`"),
)
NATIVE_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-C001-NRTR-01", "`M254-C001` does not extend the runtime library API"),
    SnippetCheck("M254-C001-NRTR-02", f"`{INIT_STUB_SYMBOL_PREFIX}`"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-C001-PKG-01", '"check:objc3c:m254-c001-bootstrap-lowering-contract-and-architecture-freeze": "python scripts/check_m254_c001_bootstrap_lowering_contract_and_architecture_freeze.py"'),
    SnippetCheck("M254-C001-PKG-02", '"test:tooling:m254-c001-bootstrap-lowering-contract-and-architecture-freeze": "python -m pytest tests/tooling/test_check_m254_c001_bootstrap_lowering_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M254-C001-PKG-03", '"check:objc3c:m254-c001-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m254-c001-bootstrap-lowering-contract-and-architecture-freeze && npm run test:tooling:m254-c001-bootstrap-lowering-contract-and-architecture-freeze"'),
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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts-header", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_HEADER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--native-runtime-readme", type=Path, default=DEFAULT_NATIVE_RUNTIME_README)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
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
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def run_dynamic_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    out_dir = args.probe_root.resolve() / "native-hello"
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command)
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"

    case: dict[str, object] = {
        "case_id": "M254-C001-CASE-NATIVE",
        "command": command,
        "process_exit_code": result.returncode,
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M254-C001-NATIVE-EXE", "native executable is missing", failures)
    checks_total += 1
    checks_passed += require(args.fixture.exists(), display_path(args.fixture), "M254-C001-FIXTURE", "fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(out_dir), "M254-C001-NATIVE-EXIT", "native compile must succeed", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M254-C001-MANIFEST", "manifest is missing", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M254-C001-REG-MANIFEST", "registration manifest is missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M254-C001-IR", "IR output is missing", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M254-C001-OBJ", "object output is missing", failures)
    if result.returncode != 0 or not manifest_path.exists() or not registration_manifest_path.exists() or not ir_path.exists() or not obj_path.exists():
        return checks_total, checks_passed, case

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    sema_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    lowering = sema_surface.get("objc_runtime_bootstrap_lowering_contract") if isinstance(sema_surface, dict) else None
    checks_total += 1
    checks_passed += require(isinstance(lowering, dict), display_path(manifest_path), "M254-C001-SURFACE", "bootstrap lowering packet missing from semantic surface", failures)
    if not isinstance(lowering, dict):
        return checks_total, checks_passed, case

    checks_total += 1
    checks_passed += require(lowering.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-C001-CONTRACT", "contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("bootstrap_surface_path") == SURFACE_PATH, display_path(manifest_path), "M254-C001-SURFACE-PATH", "surface path mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("lowering_boundary_model") == BOUNDARY_MODEL, display_path(manifest_path), "M254-C001-BOUNDARY", "boundary model mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("constructor_root_symbol") == CTOR_ROOT_SYMBOL, display_path(manifest_path), "M254-C001-CTOR-SYMBOL", "constructor root symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("constructor_init_stub_symbol_prefix") == INIT_STUB_SYMBOL_PREFIX, display_path(manifest_path), "M254-C001-INIT-PREFIX", "init-stub prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("registration_table_symbol_prefix") == REGISTRATION_TABLE_SYMBOL_PREFIX, display_path(manifest_path), "M254-C001-TABLE-PREFIX", "registration-table prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("global_ctor_list_model") == GLOBAL_CTOR_LIST_MODEL, display_path(manifest_path), "M254-C001-GLOBAL-CTOR", "global ctor list model mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("constructor_root_emission_state") == DEFERRED_STATE, display_path(manifest_path), "M254-C001-CTOR-STATE", "constructor-root emission state mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("init_stub_emission_state") == DEFERRED_STATE, display_path(manifest_path), "M254-C001-INIT-STATE", "init-stub emission state mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("registration_table_emission_state") == DEFERRED_STATE, display_path(manifest_path), "M254-C001-TABLE-STATE", "registration-table emission state mismatch", failures)
    checks_total += 1
    checks_passed += require(lowering.get("ready_for_bootstrap_materialization") is True, display_path(manifest_path), "M254-C001-READY", "bootstrap lowering readiness mismatch", failures)

    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_lowering_contract_id") == CONTRACT_ID, display_path(registration_manifest_path), "M254-C001-REG-CONTRACT", "registration manifest lowering contract mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_lowering_boundary_model") == BOUNDARY_MODEL, display_path(registration_manifest_path), "M254-C001-REG-BOUNDARY", "registration manifest lowering boundary mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_global_ctor_list_model") == GLOBAL_CTOR_LIST_MODEL, display_path(registration_manifest_path), "M254-C001-REG-GLOBAL-CTOR", "registration manifest ctor list model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_symbol_prefix") == REGISTRATION_TABLE_SYMBOL_PREFIX, display_path(registration_manifest_path), "M254-C001-REG-TABLE-PREFIX", "registration manifest registration-table prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_constructor_root_emission_state") == DEFERRED_STATE, display_path(registration_manifest_path), "M254-C001-REG-CTOR-STATE", "registration manifest constructor-root state mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_init_stub_emission_state") == DEFERRED_STATE, display_path(registration_manifest_path), "M254-C001-REG-INIT-STATE", "registration manifest init-stub state mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registration_table_emission_state") == DEFERRED_STATE, display_path(registration_manifest_path), "M254-C001-REG-TABLE-STATE", "registration manifest registration-table state mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("ready_for_bootstrap_lowering_materialization") is True, display_path(registration_manifest_path), "M254-C001-REG-READY", "registration manifest bootstrap-lowering readiness mismatch", failures)

    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    case["boundary_line"] = boundary_line
    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), "M254-C001-IR-BOUNDARY", "IR must publish bootstrap lowering boundary", failures)
    checks_total += 1
    checks_passed += require(f"boundary_model={BOUNDARY_MODEL}" in boundary_line, display_path(ir_path), "M254-C001-IR-BOUNDARY-MODEL", "IR boundary model mismatch", failures)
    checks_total += 1
    checks_passed += require(f"constructor_root_symbol={CTOR_ROOT_SYMBOL}" in boundary_line, display_path(ir_path), "M254-C001-IR-CTOR-SYMBOL", "IR constructor-root symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(f"init_stub_symbol_prefix={INIT_STUB_SYMBOL_PREFIX}" in boundary_line, display_path(ir_path), "M254-C001-IR-INIT-PREFIX", "IR init-stub prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_symbol_prefix={REGISTRATION_TABLE_SYMBOL_PREFIX}" in boundary_line, display_path(ir_path), "M254-C001-IR-TABLE-PREFIX", "IR registration-table prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(f"global_ctor_list_model={GLOBAL_CTOR_LIST_MODEL}" in boundary_line, display_path(ir_path), "M254-C001-IR-GLOBAL-CTOR", "IR global ctor list model mismatch", failures)
    checks_total += 1
    checks_passed += require("no-ctor-root-no-init-stub-no-registration-table-materialized-yet" in boundary_line, display_path(ir_path), "M254-C001-IR-NONGOALS", "IR non-goal boundary mismatch", failures)
    bootstrap_globals_materialized = "@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }]" in ir_text
    case["bootstrap_globals_materialized"] = bootstrap_globals_materialized
    checks_total += 1
    if bootstrap_globals_materialized:
        checks_passed += require(f"ptr @{CTOR_ROOT_SYMBOL}" in ir_text, display_path(ir_path), "M254-C001-IR-CTOR-ROOT-MATERIALIZED", "materialized ctor root must preserve the frozen symbol name", failures)
    else:
        checks_passed += require("@llvm.global_ctors" not in ir_text, display_path(ir_path), "M254-C001-IR-NO-GLOBAL-CTORS", "IR must not materialize llvm.global_ctors before C002 lands", failures)
    checks_total += 1
    if bootstrap_globals_materialized:
        checks_passed += require(f"@{INIT_STUB_SYMBOL_PREFIX}" in ir_text, display_path(ir_path), "M254-C001-IR-INIT-STUB-MATERIALIZED", "materialized init stub must preserve the frozen symbol prefix", failures)
    else:
        checks_passed += require(f"@{CTOR_ROOT_SYMBOL}" not in ir_text, display_path(ir_path), "M254-C001-IR-NO-CTOR-ROOT", "IR must not materialize ctor root before C002 lands", failures)
    checks_total += 1
    if bootstrap_globals_materialized:
        checks_passed += require(f"@{REGISTRATION_TABLE_SYMBOL_PREFIX}" in ir_text, display_path(ir_path), "M254-C001-IR-TABLE-MATERIALIZED", "materialized registration table must preserve the frozen symbol prefix", failures)
    else:
        checks_passed += require(f"@{INIT_STUB_SYMBOL_PREFIX}" not in ir_text, display_path(ir_path), "M254-C001-IR-NO-INIT-STUB", "IR must not materialize init stubs before C002 lands", failures)
    checks_total += 1
    if bootstrap_globals_materialized:
        checks_passed += require("call i32 @objc3_runtime_register_image(ptr %bootstrap_image)" in ir_text, display_path(ir_path), "M254-C001-IR-RUNTIME-CALL-MATERIALIZED", "materialized bootstrap lowering must still call the frozen runtime registration entrypoint", failures)
    else:
        checks_passed += require(f"@{REGISTRATION_TABLE_SYMBOL_PREFIX}" not in ir_text, display_path(ir_path), "M254-C001-IR-NO-TABLE", "IR must not materialize registration tables before C002 lands", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0, display_path(obj_path), "M254-C001-OBJ-SIZE", "object output must be non-empty", failures)

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
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts_header, FRONTEND_ARTIFACTS_HEADER_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.process_header, PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.frontend_anchor_cpp, FRONTEND_ANCHOR_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.native_runtime_readme, NATIVE_RUNTIME_README_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        probe_total, probe_passed, case = run_dynamic_probe(args, failures)
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
        "evidence_path": str(args.summary_out).replace("\\", "/"),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
