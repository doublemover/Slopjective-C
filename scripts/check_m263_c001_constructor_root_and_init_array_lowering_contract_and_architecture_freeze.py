#!/usr/bin/env python3
"""Fail-closed checker for M263-C001 constructor-root/init-array lowering freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-c001-constructor-root-and-init-array-lowering-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-constructor-root-init-array-lowering/m263-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract"
DESCRIPTOR_CONTRACT_ID = "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"
DESCRIPTOR_ARTIFACT = "module.runtime-registration-descriptor.json"
BOUNDARY_MODEL = (
    "registration-descriptor-and-registration-manifest-drive-constructor-root-"
    "init-stub-registration-table-and-platform-init-array-lowering"
)
DESCRIPTOR_HANDOFF_MODEL = (
    "registration-descriptor-artifact-and-registration-manifest-are-authoritative-"
    "lowering-inputs"
)
CTOR_SYMBOL = "__objc3_runtime_register_image_ctor"
INIT_STUB_PREFIX = "__objc3_runtime_register_image_init_stub_"
REGISTRATION_TABLE_PREFIX = "__objc3_runtime_registration_table_"
IMAGE_LOCAL_INIT_PREFIX = "__objc3_runtime_image_local_init_state_"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
GLOBAL_CTOR_MODEL = "llvm.global_ctors-single-root-priority-65535"
CTOR_STATE = "materialized-before-user-main-via-llvm-global-ctors-single-root"
INIT_STUB_STATE = "materialized-before-user-main-via-derived-init-stub"
REGISTRATION_TABLE_STATE = "materialized-in-native-object-artifact"
NON_GOALS = (
    "no-multi-image-root-fanout-no-runtime-replay-partitioning-no-late-linker-synthesis"
)
RUNTIME_CTOR_INIT_EMISSION_CONTRACT_ID = "objc3c-runtime-constructor-init-stub-emission/m254-c002-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_constructor_root_and_init_array_lowering_contract_and_architecture_freeze_c001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_c001_constructor_root_and_init_array_lowering_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_failure_restart_default.objc3"
DEFAULT_EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_failure_restart_explicit.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "reports" / "m263" / "M263-C001" / "dynamic"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-C001" / "constructor_root_and_init_array_lowering_contract_summary.json"


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


DYNAMIC_CASES = (
    DynamicCase("default", DEFAULT_DEFAULT_FIXTURE),
    DynamicCase("explicit", DEFAULT_EXPLICIT_FIXTURE),
)

EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M263-C001-DOC-EXP-01", "# M263 Constructor-Root and Init-Array Lowering Contract and Architecture Freeze Expectations (C001)"),
    SnippetCheck("M263-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-C001-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M263-C001-DOC-EXP-04", f"`{DESCRIPTOR_CONTRACT_ID}`"),
    SnippetCheck("M263-C001-DOC-EXP-05", f"`{DESCRIPTOR_ARTIFACT}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M263-C001-PKT-01", "# M263-C001 Constructor-Root and Init-Array Lowering Contract and Architecture Freeze Packet"),
    SnippetCheck("M263-C001-PKT-02", "Packet: `M263-C001`"),
    SnippetCheck("M263-C001-PKT-03", f"contract id `{CONTRACT_ID}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M263-C001-ARCH-01", "## M263 constructor-root and init-array lowering contract (C001)"),
    SnippetCheck("M263-C001-ARCH-02", "module.runtime-registration-descriptor.json"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M263-C001-NDOC-01", "## Constructor-root and init-array lowering contract (M263-C001)"),
    SnippetCheck("M263-C001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-C001-NDOC-03", f"`{DESCRIPTOR_ARTIFACT}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M263-C001-SPC-01", "## M263 constructor-root and init-array lowering contract (C001)"),
    SnippetCheck("M263-C001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-C001-SPC-03", f"`{BOUNDARY_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M263-C001-META-01", "## M263 constructor-root and init-array lowering metadata anchors (C001)"),
    SnippetCheck("M263-C001-META-02", "`registration_descriptor_frontend_closure_contract_id`"),
    SnippetCheck("M263-C001-META-03", "`module.runtime-registration-descriptor.json`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M263-C001-LHDR-01", "kObjc3RuntimeBootstrapLoweringContractId"),
    SnippetCheck("M263-C001-LHDR-02", CONTRACT_ID),
    SnippetCheck("M263-C001-LHDR-03", "kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel"),
    SnippetCheck("M263-C001-LHDR-04", DESCRIPTOR_HANDOFF_MODEL),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M263-C001-LCPP-01", "M263-C001 constructor-root/init-array lowering freeze anchor"),
    SnippetCheck("M263-C001-LCPP-02", "registration_descriptor_handoff_contract_id"),
    SnippetCheck("M263-C001-LCPP-03", NON_GOALS),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M263-C001-IR-01", "M263-C001 freezes this emitted boundary against the live ctor-root and"),
    SnippetCheck("M263-C001-IR-02", "runtime_bootstrap_lowering_boundary"),
    SnippetCheck("M263-C001-IR-03", "runtime_bootstrap_ctor_init_emission"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M263-C001-TYPES-01", "registration_descriptor_frontend_closure_contract_id"),
    SnippetCheck("M263-C001-TYPES-02", "registration_descriptor_artifact"),
    SnippetCheck("M263-C001-TYPES-03", "registration_descriptor_handoff_model"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M263-C001-ART-01", "M263-C001 constructor-root/init-array lowering anchor"),
    SnippetCheck("M263-C001-ART-02", "registration_descriptor_frontend_closure_contract_ready"),
    SnippetCheck("M263-C001-ART-03", "runtime_bootstrap_lowering_registration_descriptor_artifact"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M263-C001-PKG-01", "check:objc3c:m263-c001-constructor-root-and-init-array-lowering-contract"),
    SnippetCheck("M263-C001-PKG-02", "test:tooling:m263-c001-constructor-root-and-init-array-lowering-contract"),
    SnippetCheck("M263-C001-PKG-03", "check:objc3c:m263-c001-lane-c-readiness"),
)


def display_path(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace('\\', '/') if path.is_absolute() and ROOT in path.parents else str(path).replace('\\', '/')


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> Any:
    return json.loads(read_text(path))


def canonical_json(payload: Any) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if not condition:
        failures.append(Finding(artifact=artifact, check_id=check_id, detail=detail))
        return 0
    return 1


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        passed += require(
            snippet.snippet in text,
            display_path(path),
            snippet.check_id,
            f"missing snippet: {snippet.snippet}",
            failures,
        )
    return passed


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser()
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
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    return parser.parse_args(argv)


def run_dynamic_case(args: argparse.Namespace, case_input: DynamicCase, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root / case_input.case_id
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(args.native_exe), str(case_input.fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    case: dict[str, Any] = {
        "case_id": case_input.case_id,
        "fixture": display_path(case_input.fixture),
        "command": command,
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(case_input.fixture), f"M263-C001-{case_input.case_id}-COMPILE", "native compile probe failed", failures)
    if completed.returncode != 0:
        return checks_total, checks_passed, case

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    descriptor_path = out_dir / DESCRIPTOR_ARTIFACT
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    for path, check_id in (
        (manifest_path, "MANIFEST"),
        (registration_manifest_path, "REG-MANIFEST"),
        (descriptor_path, "DESCRIPTOR"),
        (ir_path, "IR"),
        (obj_path, "OBJ"),
        (backend_path, "BACKEND"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M263-C001-{case_input.case_id}-{check_id}", f"missing artifact {path.name}", failures)
    if not all(path.exists() for path in (manifest_path, registration_manifest_path, descriptor_path, ir_path, obj_path, backend_path)):
        return checks_total, checks_passed, case

    manifest = read_json(manifest_path)
    registration_manifest = read_json(registration_manifest_path)
    descriptor = read_json(descriptor_path)
    ir_text = read_text(ir_path)
    backend = read_text(backend_path).strip()
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_runtime_bootstrap_lowering_contract"]

    case["surface_replay_key"] = surface.get("replay_key")
    case["registration_descriptor_identifier"] = descriptor.get("registration_descriptor_identifier")
    case["image_root_identifier"] = descriptor.get("image_root_identifier")

    checks_total += 1
    checks_passed += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-CONTRACT", "surface contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("bootstrap_surface_path") == SURFACE_PATH, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-PATH", "surface path mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("registration_descriptor_frontend_closure_contract_id") == DESCRIPTOR_CONTRACT_ID, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-DESC-ID", "descriptor contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("registration_descriptor_artifact") == DESCRIPTOR_ARTIFACT, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-DESC-ART", "descriptor artifact mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("registration_descriptor_handoff_model") == DESCRIPTOR_HANDOFF_MODEL, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-HANDOFF", "descriptor handoff model mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("lowering_boundary_model") == BOUNDARY_MODEL, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-MODEL", "boundary model mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("constructor_root_symbol") == CTOR_SYMBOL, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-CTOR", "constructor root symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("constructor_root_emission_state") == CTOR_STATE, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-CTOR-STATE", "constructor root emission state mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("init_stub_emission_state") == INIT_STUB_STATE, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-INIT-STATE", "init stub emission state mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("registration_table_emission_state") == REGISTRATION_TABLE_STATE, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-TABLE-STATE", "registration table emission state mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("registration_descriptor_frontend_closure_contract_ready") is True, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-DESC-READY", "descriptor closure readiness mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("bootstrap_ir_materialization_landed") is True, display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-MATERIALIZED", "bootstrap IR materialization must be true", failures)
    checks_total += 1
    checks_passed += require(bool(surface.get("replay_key")), display_path(manifest_path), f"M263-C001-{case_input.case_id}-SURFACE-REPLAY", "surface replay key must be non-empty", failures)

    checks_total += 1
    checks_passed += require(registration_manifest.get("bootstrap_lowering_contract_id") == CONTRACT_ID, display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-CONTRACT", "registration manifest lowering contract mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("bootstrap_lowering_boundary_model") == BOUNDARY_MODEL, display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-MODEL", "registration manifest lowering model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("constructor_root_symbol") == CTOR_SYMBOL, display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-CTOR", "registration manifest constructor root mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("bootstrap_registration_table_symbol", "").startswith(REGISTRATION_TABLE_PREFIX), display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-TABLE", "registration manifest table symbol prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("bootstrap_image_local_init_state_symbol", "").startswith(IMAGE_LOCAL_INIT_PREFIX), display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-INIT-CELL", "registration manifest image-local init symbol prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("constructor_init_stub_symbol", "").startswith(INIT_STUB_PREFIX), display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-INIT-STUB", "registration manifest init stub prefix mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("bootstrap_global_ctor_list_model") == GLOBAL_CTOR_MODEL, display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-GCTORS", "registration manifest global ctor model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest.get("ready_for_bootstrap_lowering_materialization") is True, display_path(registration_manifest_path), f"M263-C001-{case_input.case_id}-REG-READY", "registration manifest lowering readiness mismatch", failures)

    checks_total += 1
    checks_passed += require(descriptor.get("contract_id") == DESCRIPTOR_CONTRACT_ID, display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-CONTRACT", "descriptor contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("artifact") == DESCRIPTOR_ARTIFACT, display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-ARTIFACT", "descriptor artifact mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("constructor_root_symbol") == registration_manifest.get("constructor_root_symbol"), display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-CTOR", "descriptor/manifest constructor root mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("constructor_init_stub_symbol") == registration_manifest.get("constructor_init_stub_symbol"), display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-INIT-STUB", "descriptor/manifest init stub mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("bootstrap_registration_table_symbol") == registration_manifest.get("bootstrap_registration_table_symbol"), display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-TABLE", "descriptor/manifest table symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("bootstrap_image_local_init_state_symbol") == registration_manifest.get("bootstrap_image_local_init_state_symbol"), display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-INIT-CELL", "descriptor/manifest image-local init symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-ENTRY", "descriptor entrypoint mismatch", failures)
    checks_total += 1
    checks_passed += require(descriptor.get("ready_for_registration_descriptor_lowering") is True, display_path(descriptor_path), f"M263-C001-{case_input.case_id}-DESC-READY", "descriptor lowering readiness mismatch", failures)

    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(f"; runtime_bootstrap_lowering_boundary = contract={CONTRACT_ID}")), "")
    case["boundary_line"] = boundary_line
    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-BOUNDARY", "IR must publish the M263-C001 lowering boundary", failures)
    checks_total += 1
    checks_passed += require(f"boundary_model={BOUNDARY_MODEL}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-MODEL", "IR lowering boundary model mismatch", failures)
    checks_total += 1
    checks_passed += require(f"registration_descriptor_handoff_contract_id={DESCRIPTOR_CONTRACT_ID}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-DESC-ID", "IR descriptor handoff contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(f"registration_descriptor_artifact={DESCRIPTOR_ARTIFACT}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-DESC-ART", "IR descriptor artifact mismatch", failures)
    checks_total += 1
    checks_passed += require(f"registration_descriptor_handoff_model={DESCRIPTOR_HANDOFF_MODEL}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-DESC-MODEL", "IR descriptor handoff model mismatch", failures)
    checks_total += 1
    checks_passed += require(f"constructor_root_emission_state={CTOR_STATE}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-CTOR-STATE", "IR constructor root state mismatch", failures)
    checks_total += 1
    checks_passed += require(f"init_stub_emission_state={INIT_STUB_STATE}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-INIT-STATE", "IR init stub state mismatch", failures)
    checks_total += 1
    checks_passed += require(f"registration_table_emission_state={REGISTRATION_TABLE_STATE}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-TABLE-STATE", "IR registration table state mismatch", failures)
    checks_total += 1
    checks_passed += require(f"non_goals={NON_GOALS}" in boundary_line, display_path(ir_path), f"M263-C001-{case_input.case_id}-IR-NONGOALS", "IR non-goals mismatch", failures)

    for needle, suffix in (
        (f"; runtime_bootstrap_ctor_init_emission = contract={RUNTIME_CTOR_INIT_EMISSION_CONTRACT_ID}", "IR-CTOR-EMISSION"),
        ("@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }]", "IR-GCTORS"),
        (f"@{CTOR_SYMBOL}", "IR-CTOR-SYMBOL"),
        (f"@{INIT_STUB_PREFIX}", "IR-INIT-STUB"),
        (f"@{REGISTRATION_TABLE_PREFIX}", "IR-TABLE"),
        (f"@{IMAGE_LOCAL_INIT_PREFIX}", "IR-INIT-CELL"),
        ("call void @objc3_runtime_stage_registration_table_for_bootstrap(", "IR-STAGE"),
        (f"call i32 @{REGISTRATION_ENTRYPOINT_SYMBOL}(", "IR-REGISTER"),
    ):
        checks_total += 1
        checks_passed += require(needle in ir_text, display_path(ir_path), f"M263-C001-{case_input.case_id}-{suffix}", f"missing IR proof: {needle}", failures)

    checks_total += 1
    checks_passed += require(backend == "llvm-direct", display_path(backend_path), f"M263-C001-{case_input.case_id}-BACKEND", "object backend must remain llvm-direct", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0, display_path(obj_path), f"M263-C001-{case_input.case_id}-OBJ-SIZE", "object output must be non-empty", failures)
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
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        checks_total += 1
        checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M263-C001-NATIVE-EXE", "native executable is required", failures)
        if args.native_exe.exists():
            for case_input in DYNAMIC_CASES:
                case_total, case_passed, case_payload = run_dynamic_case(args, case_input, failures)
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
