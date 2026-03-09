#!/usr/bin/env python3
"""Fail-closed checker for M263-D001 runtime bootstrap table consumption freeze."""

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
MODE = "m263-d001-runtime-bootstrap-table-consumption-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1"
REGISTRAR_CONTRACT_ID = "objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1"
LOWERING_CONTRACT_ID = "objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1"
TABLE_CONSUMPTION_MODEL = "next-public-register-call-consumes-staged-registration-table-once"
DEDUPLICATION_MODEL = "translation-unit-identity-key-rejection-before-registration-state-advance"
IMAGE_STATE_PUBLICATION_MODEL = "image-walk-snapshot-publishes-module-identity-root-counts-and-staged-table-usage"
INTERNAL_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
RUNTIME_SOURCE_PATH = "native/objc3c/src/runtime/objc3_runtime.cpp"
PROCESS_CPP_PATH = "native/objc3c/src/io/objc3_process.cpp"
STAGE_REGISTRATION_TABLE_SYMBOL = "objc3_runtime_stage_registration_table_for_bootstrap"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
IMAGE_WALK_SNAPSHOT_SYMBOL = "objc3_runtime_copy_image_walk_state_for_testing"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
REGISTRATION_DESCRIPTOR_ARTIFACT = "module.runtime-registration-descriptor.json"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_runtime_bootstrap_table_consumption_contract_and_architecture_freeze_d001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_d001_runtime_bootstrap_table_consumption_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_INTERNAL_HEADER = ROOT / INTERNAL_HEADER_PATH
DEFAULT_RUNTIME_SOURCE = ROOT / RUNTIME_SOURCE_PATH
DEFAULT_PROCESS_CPP = ROOT / PROCESS_CPP_PATH
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m263_d001_runtime_bootstrap_table_consumption_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PRIMARY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c002_runtime_bootstrap_metadata_library.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c003_runtime_bootstrap_category_library.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m263" / "d001-runtime-bootstrap-table-consumption"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m263/M263-D001/runtime_bootstrap_table_consumption_contract_summary.json")


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
    DynamicCase("metadata-library", DEFAULT_PRIMARY_FIXTURE),
    DynamicCase("category-library", DEFAULT_CATEGORY_FIXTURE),
)

EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M263-D001-DOC-EXP-01", "# M263 Runtime Bootstrap Table Consumption Contract And Architecture Freeze Expectations (D001)"),
    SnippetCheck("M263-D001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-D001-DOC-EXP-03", f"`{INTERNAL_HEADER_PATH}`"),
    SnippetCheck("M263-D001-DOC-EXP-04", f"`{RUNTIME_SOURCE_PATH}`"),
    SnippetCheck("M263-D001-DOC-EXP-05", f"`{PROCESS_CPP_PATH}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M263-D001-DOC-PKT-01", "# M263-D001 Runtime Bootstrap Table Consumption Contract And Architecture Freeze Packet"),
    SnippetCheck("M263-D001-DOC-PKT-02", "Packet: `M263-D001`"),
    SnippetCheck("M263-D001-DOC-PKT-03", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-D001-DOC-PKT-04", "duplicate bootstrap identities before state advance"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M263-D001-NDOC-01", "## Runtime bootstrap table consumption freeze (M263-D001)"),
    SnippetCheck("M263-D001-NDOC-02", f"`{TABLE_CONSUMPTION_MODEL}`"),
    SnippetCheck("M263-D001-NDOC-03", f"`{IMAGE_STATE_PUBLICATION_MODEL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M263-D001-SPC-01", "## M263 runtime bootstrap table consumption freeze (D001)"),
    SnippetCheck("M263-D001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-D001-SPC-03", f"`{DEDUPLICATION_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M263-D001-META-01", "## M263 runtime bootstrap table consumption anchors (D001)"),
    SnippetCheck("M263-D001-META-02", "`bootstrap_table_consumption_contract_id`"),
    SnippetCheck("M263-D001-META-03", "`bootstrap_table_image_walk_snapshot_symbol`"),
)
INTERNAL_HEADER_SNIPPETS = (
    SnippetCheck("M263-D001-INTH-01", "M263-D001 runtime-bootstrap-table-consumption anchor"),
    SnippetCheck("M263-D001-INTH-02", STAGE_REGISTRATION_TABLE_SYMBOL),
    SnippetCheck("M263-D001-INTH-03", IMAGE_WALK_SNAPSHOT_SYMBOL),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M263-D001-RTS-01", "M263-D001 runtime-bootstrap-table-consumption anchor"),
    SnippetCheck("M263-D001-RTS-02", "TryWalkRegistrationTableUnlocked"),
    SnippetCheck("M263-D001-RTS-03", "state.staged_registration_table = nullptr;"),
    SnippetCheck("M263-D001-RTS-04", "registration_order_by_identity_key.find("),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M263-D001-PROC-01", "kObjc3RuntimeBootstrapTableConsumptionContractId"),
    SnippetCheck("M263-D001-PROC-02", "bootstrap_table_consumption_contract_id"),
    SnippetCheck("M263-D001-PROC-03", "bootstrap_table_image_state_publication_model"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M263-D001-PRB-01", '#include "runtime/objc3_runtime_bootstrap_internal.h"'),
    SnippetCheck("M263-D001-PRB-02", "duplicate_registration_status"),
    SnippetCheck("M263-D001-PRB-03", "after_duplicate_last_rejected_registration_order_ordinal"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M263-D001-PKG-01", '"check:objc3c:m263-d001-runtime-bootstrap-table-consumption-freeze": "python scripts/check_m263_d001_runtime_bootstrap_table_consumption_contract_and_architecture_freeze.py"'),
    SnippetCheck("M263-D001-PKG-02", '"test:tooling:m263-d001-runtime-bootstrap-table-consumption-freeze": "python -m pytest tests/tooling/test_check_m263_d001_runtime_bootstrap_table_consumption_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M263-D001-PKG-03", '"check:objc3c:m263-d001-lane-d-readiness": "python scripts/run_m263_d001_lane_d_readiness.py"'),
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
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--internal-header", type=Path, default=DEFAULT_INTERNAL_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--primary-fixture", type=Path, default=DEFAULT_PRIMARY_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M263-D001-MISSING", f"required artifact is missing: {display_path(path)}"))
        return len(snippets)
    text = read_text(path)
    checks_passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return checks_passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(read_text(path))


def resolve_tool(name: str) -> Path | None:
    which = shutil.which(name)
    if which:
        return Path(which)
    return None


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd or ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_dynamic_case(*, args: argparse.Namespace, case_input: DynamicCase, clangxx: str, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    case: dict[str, object] = {"case_id": case_input.case_id, "fixture": display_path(case_input.fixture)}

    runtime_include_root = args.runtime_include_root.resolve()
    runtime_probe = args.runtime_probe.resolve()
    runtime_library = args.runtime_library.resolve()
    out_dir = args.probe_root.resolve() / case_input.case_id
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(case_input.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        command.extend(["--llc", str(llc)])
    compile_result = run_command(command)

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    registration_descriptor_path = out_dir / REGISTRATION_DESCRIPTOR_ARTIFACT
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(case_input.fixture), f"M263-D001-{case_input.case_id}-COMPILE", f"native compile exited with {compile_result.returncode}", failures)
    for artifact_path, suffix in ((manifest_path, "MANIFEST"), (registration_manifest_path, "REG-MANIFEST"), (registration_descriptor_path, "REG-DESC"), (ir_path, "IR"), (obj_path, "OBJ")):
        checks_total += 1
        checks_passed += require(artifact_path.exists(), display_path(artifact_path), f"M263-D001-{case_input.case_id}-{suffix}", f"required artifact is missing: {display_path(artifact_path)}", failures)
    if failures and any(f.check_id.startswith(f"M263-D001-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    registration_manifest_payload = load_json(registration_manifest_path)
    registration_descriptor_payload = load_json(registration_descriptor_path)
    ir_text = read_text(ir_path)
    backend_text = read_text(backend_path).strip() if backend_path.exists() else ""

    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_table_consumption_contract_id") == CONTRACT_ID, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-CONTRACT", "registration manifest consumption contract mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_table_consumption_model") == TABLE_CONSUMPTION_MODEL, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-MODEL", "registration manifest consumption model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_table_deduplication_model") == DEDUPLICATION_MODEL, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-DEDUP", "registration manifest dedup model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_table_image_state_publication_model") == IMAGE_STATE_PUBLICATION_MODEL, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-STATE", "registration manifest image-state model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_table_stage_registration_table_symbol") == STAGE_REGISTRATION_TABLE_SYMBOL, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-STAGE", "registration manifest stage-registration symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_table_image_walk_snapshot_symbol") == IMAGE_WALK_SNAPSHOT_SYMBOL, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-SNAPSHOT", "registration manifest image-walk snapshot symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registrar_contract_id") == REGISTRAR_CONTRACT_ID, display_path(registration_manifest_path), f"M263-D001-{case_input.case_id}-REG-UPSTREAM", "registration manifest upstream registrar contract mismatch", failures)
    checks_total += 1
    checks_passed += require(int(registration_descriptor_payload.get("translation_unit_registration_order_ordinal", 0)) > 0, display_path(registration_descriptor_path), f"M263-D001-{case_input.case_id}-DESC-ORDINAL", "registration descriptor ordinal must be positive", failures)
    checks_total += 1
    checks_passed += require(f"call void @{STAGE_REGISTRATION_TABLE_SYMBOL}" in ir_text, display_path(ir_path), f"M263-D001-{case_input.case_id}-IR-STAGE", "IR stage-registration call is missing", failures)
    checks_total += 1
    checks_passed += require(f"call i32 @{REGISTRATION_ENTRYPOINT_SYMBOL}(ptr %bootstrap_image)" in ir_text, display_path(ir_path), f"M263-D001-{case_input.case_id}-IR-REGISTER", "IR public registration call is missing", failures)
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), f"M263-D001-{case_input.case_id}-BACKEND", "object backend must remain llvm-direct", failures)
    if failures and any(f.check_id.startswith(f"M263-D001-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    probe_out_dir = out_dir / "probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / f"{case_input.case_id}-runtime-bootstrap-table-consumption-probe.exe"
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
    checks_passed += require(probe_compile.returncode == 0, display_path(runtime_probe), f"M263-D001-{case_input.case_id}-PROBE-COMPILE", f"probe compile exited with {probe_compile.returncode}", failures)
    checks_total += 1
    checks_passed += require(probe_exe.exists(), display_path(probe_exe), f"M263-D001-{case_input.case_id}-PROBE-EXE", "probe executable is missing", failures)
    if failures and any(f.check_id.startswith(f"M263-D001-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(probe_run.returncode == 0, display_path(probe_exe), f"M263-D001-{case_input.case_id}-PROBE-RUN", f"probe exited with {probe_run.returncode}", failures)
    if probe_run.returncode != 0:
        case.update({"probe_compile_command": probe_compile_command, "probe_run_stdout": probe_run.stdout, "probe_run_stderr": probe_run.stderr})
        return checks_total, checks_passed, case

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), f"M263-D001-{case_input.case_id}-PROBE-JSON", f"invalid probe JSON: {exc}"))
        return checks_total + 1, checks_passed, case

    expected_total = int(registration_manifest_payload.get("total_descriptor_count", 0))
    expected_class_count = int(registration_manifest_payload.get("class_descriptor_count", 0))
    expected_protocol_count = int(registration_manifest_payload.get("protocol_descriptor_count", 0))
    expected_category_count = int(registration_manifest_payload.get("category_descriptor_count", 0))
    expected_property_count = int(registration_manifest_payload.get("property_descriptor_count", 0))
    expected_ivar_count = int(registration_manifest_payload.get("ivar_descriptor_count", 0))
    expected_identity_key = str(registration_manifest_payload.get("translation_unit_identity_key", ""))
    expected_ordinal = int(registration_manifest_payload.get("translation_unit_registration_order_ordinal", 0))

    def add(condition: bool, suffix: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, display_path(probe_exe), f"M263-D001-{case_input.case_id}-{suffix}", detail, failures)

    add(probe_payload.get("startup_registration_copy_status") == 0, "PROBE-STARTUP-COPY", "startup registration snapshot copy must succeed")
    add(probe_payload.get("startup_image_walk_copy_status") == 0, "PROBE-STARTUP-WALK-COPY", "startup image-walk snapshot copy must succeed")
    add(probe_payload.get("startup_registered_image_count") == 1, "PROBE-STARTUP-COUNT", "startup registered image count mismatch")
    add(probe_payload.get("startup_registered_descriptor_total") == expected_total, "PROBE-STARTUP-TOTAL", "startup registered descriptor total mismatch")
    add(probe_payload.get("startup_next_expected_registration_order_ordinal") == expected_ordinal + 1, "PROBE-STARTUP-NEXT", "startup next expected ordinal mismatch")
    add(probe_payload.get("startup_last_successful_registration_order_ordinal") == expected_ordinal, "PROBE-STARTUP-ORDINAL", "startup successful ordinal mismatch")
    add(probe_payload.get("startup_last_registration_status") == 0, "PROBE-STARTUP-STATUS", "startup registration status mismatch")
    add(probe_payload.get("startup_last_registered_translation_unit_identity_key") == expected_identity_key, "PROBE-STARTUP-TU", "startup identity key mismatch")
    add(probe_payload.get("startup_walked_image_count") == 1, "PROBE-STARTUP-WALK-COUNT", "startup walked image count mismatch")
    add(probe_payload.get("startup_last_walked_class_descriptor_count") == expected_class_count, "PROBE-STARTUP-CLASS", "startup walked class count mismatch")
    add(probe_payload.get("startup_last_walked_protocol_descriptor_count") == expected_protocol_count, "PROBE-STARTUP-PROTOCOL", "startup walked protocol count mismatch")
    add(probe_payload.get("startup_last_walked_category_descriptor_count") == expected_category_count, "PROBE-STARTUP-CATEGORY", "startup walked category count mismatch")
    add(probe_payload.get("startup_last_walked_property_descriptor_count") == expected_property_count, "PROBE-STARTUP-PROPERTY", "startup walked property count mismatch")
    add(probe_payload.get("startup_last_walked_ivar_descriptor_count") == expected_ivar_count, "PROBE-STARTUP-IVAR", "startup walked ivar count mismatch")
    add(probe_payload.get("startup_last_linker_anchor_matches_discovery_root") == 1, "PROBE-STARTUP-LINKER", "startup linker anchor must match discovery root")
    add(probe_payload.get("startup_last_registration_used_staged_table") == 1, "PROBE-STARTUP-STAGED", "startup registration must consume staged table")
    add(int(probe_payload.get("startup_last_discovery_root_entry_count", 0)) >= 6, "PROBE-STARTUP-DISCOVERY", "startup discovery root entry count must be >= 6")
    add(probe_payload.get("duplicate_registration_status") == -2, "PROBE-DUP-STATUS", "duplicate registration must fail with duplicate-identity status")
    add(probe_payload.get("after_duplicate_registration_copy_status") == 0, "PROBE-DUP-COPY", "post-duplicate registration snapshot copy must succeed")
    add(probe_payload.get("after_duplicate_image_walk_copy_status") == 0, "PROBE-DUP-WALK-COPY", "post-duplicate image-walk snapshot copy must succeed")
    add(probe_payload.get("after_duplicate_registered_image_count") == 1, "PROBE-DUP-COUNT", "duplicate registration must not advance image count")
    add(probe_payload.get("after_duplicate_registered_descriptor_total") == expected_total, "PROBE-DUP-TOTAL", "duplicate registration must not advance descriptor total")
    add(probe_payload.get("after_duplicate_next_expected_registration_order_ordinal") == expected_ordinal + 1, "PROBE-DUP-NEXT", "duplicate registration must not advance next ordinal")
    add(probe_payload.get("after_duplicate_last_successful_registration_order_ordinal") == expected_ordinal, "PROBE-DUP-LAST-ORDINAL", "duplicate registration must preserve last successful ordinal")
    add(probe_payload.get("after_duplicate_last_registration_status") == -2, "PROBE-DUP-LAST-STATUS", "duplicate registration must publish duplicate status")
    add(probe_payload.get("after_duplicate_last_rejected_translation_unit_identity_key") == expected_identity_key, "PROBE-DUP-REJECTED-TU", "duplicate rejection must publish the rejected identity key")
    add(probe_payload.get("after_duplicate_last_rejected_registration_order_ordinal") == expected_ordinal, "PROBE-DUP-REJECTED-ORDINAL", "duplicate rejection must publish the rejected ordinal")
    add(probe_payload.get("after_duplicate_walked_image_count") == 1, "PROBE-DUP-WALK-COUNT", "duplicate rejection must preserve walked image count")
    add(probe_payload.get("after_duplicate_last_walked_class_descriptor_count") == expected_class_count, "PROBE-DUP-CLASS", "duplicate rejection must preserve walked class count")
    add(probe_payload.get("after_duplicate_last_walked_protocol_descriptor_count") == expected_protocol_count, "PROBE-DUP-PROTOCOL", "duplicate rejection must preserve walked protocol count")
    add(probe_payload.get("after_duplicate_last_walked_category_descriptor_count") == expected_category_count, "PROBE-DUP-CATEGORY", "duplicate rejection must preserve walked category count")
    add(probe_payload.get("after_duplicate_last_walked_property_descriptor_count") == expected_property_count, "PROBE-DUP-PROPERTY", "duplicate rejection must preserve walked property count")
    add(probe_payload.get("after_duplicate_last_walked_ivar_descriptor_count") == expected_ivar_count, "PROBE-DUP-IVAR", "duplicate rejection must preserve walked ivar count")
    add(probe_payload.get("after_duplicate_last_linker_anchor_matches_discovery_root") == 1, "PROBE-DUP-LINKER", "duplicate rejection must preserve linker/discovery proof")
    add(probe_payload.get("after_duplicate_last_registration_used_staged_table") == 1, "PROBE-DUP-STAGED", "duplicate rejection must preserve staged-table evidence")

    case.update(
        {
            "compile_command": command,
            "probe_compile_command": probe_compile_command,
            "expected_identity_key": expected_identity_key,
            "expected_registration_order_ordinal": expected_ordinal,
            "probe_payload": probe_payload,
        }
    )
    return checks_total, checks_passed, case


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.internal_header, INTERNAL_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        clangxx = resolve_tool(args.clangxx)
        checks_total += 1
        checks_passed += require(clangxx is not None, display_path(args.runtime_probe), "M263-D001-TOOL-CLANGXX", "clang++ is required", failures)
        checks_total += 1
        checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M263-D001-TOOL-NATIVE", "native executable is missing", failures)
        checks_total += 1
        checks_passed += require(args.runtime_library.exists(), display_path(args.runtime_library), "M263-D001-TOOL-RUNTIME", "runtime library is missing", failures)
        if clangxx is not None and args.native_exe.exists() and args.runtime_library.exists():
            for case_input in (
                DynamicCase("metadata-library", args.primary_fixture),
                DynamicCase("category-library", args.category_fixture),
            ):
                total, passed, case = run_dynamic_case(args=args, case_input=case_input, clangxx=str(clangxx), failures=failures)
                checks_total += total
                checks_passed += passed
                dynamic_cases.append(case)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [failure.__dict__ for failure in failures],
        "dynamic_cases": dynamic_cases,
    }
    summary_path = ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[fail] {failure.check_id} {failure.artifact}: {failure.detail}", file=sys.stderr)
        print(f"[summary] wrote failure summary to {display_path(summary_path)}", file=sys.stderr)
        return 1

    print(f"[ok] M263-D001 contract validated; summary: {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
