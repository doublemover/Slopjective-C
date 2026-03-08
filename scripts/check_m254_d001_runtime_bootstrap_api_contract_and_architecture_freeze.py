#!/usr/bin/env python3
"""Fail-closed validator for M254-D001 runtime bootstrap API freeze."""

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
MODE = "m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-api-freeze/m254-d001-v1"
SUPPORT_LIBRARY_CORE_FEATURE_CONTRACT_ID = (
    "objc3c-runtime-support-library-core-feature/m251-d002-v1"
)
SUPPORT_LIBRARY_LINK_WIRING_CONTRACT_ID = (
    "objc3c-runtime-support-library-link-wiring/m251-d003-v1"
)
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract"
PUBLIC_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime.h"
ARCHIVE_RELATIVE_PATH = "artifacts/lib/objc3_runtime.lib"
REGISTRATION_STATUS_ENUM_TYPE = "objc3_runtime_registration_status_code"
IMAGE_DESCRIPTOR_TYPE = "objc3_runtime_image_descriptor"
SELECTOR_HANDLE_TYPE = "objc3_runtime_selector_handle"
REGISTRATION_SNAPSHOT_TYPE = "objc3_runtime_registration_state_snapshot"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
SELECTOR_LOOKUP_SYMBOL = "objc3_runtime_lookup_selector"
DISPATCH_ENTRYPOINT_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_DISPATCH_SYMBOL = "objc3_msgsend_i32"
STATE_SNAPSHOT_SYMBOL = "objc3_runtime_copy_registration_state_for_testing"
RESET_FOR_TESTING_SYMBOL = "objc3_runtime_reset_for_testing"
REGISTRATION_RESULT_MODEL = "zero-success-negative-fail-closed"
REGISTRATION_ORDER_ORDINAL_MODEL = (
    "strictly-monotonic-positive-registration-order-ordinal"
)
RUNTIME_STATE_LOCKING_MODEL = "process-global-mutex-serialized-runtime-state"
STARTUP_INVOCATION_MODEL = "generated-init-stub-calls-runtime-register-image"
IMAGE_WALK_LIFECYCLE_MODEL = "deferred-until-m254-d002"
DETERMINISTIC_RESET_LIFECYCLE_MODEL = "deferred-until-m254-d003"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
HELLO_FIXTURE_CASE_ID = "hello-manifest"
PROBE_CASE_ID = "runtime-probe"
DISPATCH_MODULUS = 2147483629

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m254_runtime_bootstrap_api_contract_and_architecture_freeze_d001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m254"
    / "m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_RUNTIME_SURFACE_DOC = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
)
DEFAULT_FRONTEND_ARTIFACTS_H = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
)
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = (
    ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
)
DEFAULT_TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_RUNTIME_PROBE = (
    ROOT / "tests" / "tooling" / "runtime" / "m254_d001_runtime_bootstrap_api_probe.cpp"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = (
    ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "d001-runtime-bootstrap-api"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m254/M254-D001/runtime_bootstrap_api_contract_summary.json"
)


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
    SnippetCheck(
        "M254-D001-DOC-EXP-01",
        "# M254 Runtime Bootstrap API Contract and Architecture Freeze Expectations (D001)",
    ),
    SnippetCheck("M254-D001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-D001-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-D001-DOC-EXP-04", f"`{PUBLIC_HEADER_PATH}`"),
    SnippetCheck("M254-D001-DOC-EXP-05", f"`{ARCHIVE_RELATIVE_PATH}`"),
    SnippetCheck("M254-D001-DOC-EXP-06", f"`{IMAGE_WALK_LIFECYCLE_MODEL}`"),
    SnippetCheck(
        "M254-D001-DOC-EXP-07",
        "`tests/tooling/runtime/m254_d001_runtime_bootstrap_api_probe.cpp`",
    ),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M254-D001-DOC-PKT-01",
        "# M254-D001 Runtime Bootstrap API Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M254-D001-DOC-PKT-02", "Packet: `M254-D001`"),
    SnippetCheck("M254-D001-DOC-PKT-03", f"Contract id `{CONTRACT_ID}`"),
    SnippetCheck("M254-D001-DOC-PKT-04", f"`{PUBLIC_HEADER_PATH}`"),
    SnippetCheck("M254-D001-DOC-PKT-05", f"`{STATE_SNAPSHOT_SYMBOL}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck(
        "M254-D001-ARCH-01",
        "M254 lane-D D001 freezes the runtime-owned bootstrap API surface in",
    ),
    SnippetCheck(
        "M254-D001-ARCH-02",
        "semantic surface and registration manifest",
    ),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M254-D001-NDOC-01",
        "## Runtime bootstrap API freeze (M254-D001)",
    ),
    SnippetCheck("M254-D001-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-D001-NDOC-03", f"`{SELECTOR_LOOKUP_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-D001-SPC-01",
        "## M254 runtime bootstrap API freeze (D001)",
    ),
    SnippetCheck("M254-D001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-D001-SPC-03", f"`{STARTUP_INVOCATION_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-D001-META-01",
        "## M254 runtime bootstrap API metadata anchors (D001)",
    ),
    SnippetCheck("M254-D001-META-02", f"`{STATE_SNAPSHOT_SYMBOL}`"),
    SnippetCheck("M254-D001-META-03", f"`{RESET_FOR_TESTING_SYMBOL}`"),
)
AST_SNIPPETS = (
    SnippetCheck("M254-D001-AST-01", "kObjc3RuntimeBootstrapApiContractId"),
    SnippetCheck("M254-D001-AST-02", "kObjc3RuntimeBootstrapApiSurfacePath"),
    SnippetCheck("M254-D001-AST-03", "kObjc3RuntimeBootstrapApiImageWalkLifecycleModel"),
    SnippetCheck(
        "M254-D001-AST-04",
        "kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel",
    ),
)
RUNTIME_HEADER_SNIPPETS = (
    SnippetCheck("M254-D001-RTH-01", f"typedef enum {REGISTRATION_STATUS_ENUM_TYPE} {{"),
    SnippetCheck("M254-D001-RTH-02", f"typedef struct {IMAGE_DESCRIPTOR_TYPE} {{"),
    SnippetCheck("M254-D001-RTH-03", f"typedef struct {SELECTOR_HANDLE_TYPE} {{"),
    SnippetCheck(
        "M254-D001-RTH-04",
        f"typedef struct {REGISTRATION_SNAPSHOT_TYPE} {{",
    ),
    SnippetCheck("M254-D001-RTH-05", REGISTRATION_ENTRYPOINT_SYMBOL),
    SnippetCheck("M254-D001-RTH-06", SELECTOR_LOOKUP_SYMBOL),
    SnippetCheck("M254-D001-RTH-07", DISPATCH_ENTRYPOINT_SYMBOL),
    SnippetCheck("M254-D001-RTH-08", STATE_SNAPSHOT_SYMBOL),
    SnippetCheck("M254-D001-RTH-09", RESET_FOR_TESTING_SYMBOL),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M254-D001-RTS-01", "M254-D001 runtime-bootstrap-api anchor"),
    SnippetCheck("M254-D001-RTS-02", REGISTRATION_ENTRYPOINT_SYMBOL),
    SnippetCheck("M254-D001-RTS-03", SELECTOR_LOOKUP_SYMBOL),
    SnippetCheck("M254-D001-RTS-04", DISPATCH_ENTRYPOINT_SYMBOL),
    SnippetCheck("M254-D001-RTS-05", STATE_SNAPSHOT_SYMBOL),
    SnippetCheck("M254-D001-RTS-06", RESET_FOR_TESTING_SYMBOL),
    SnippetCheck("M254-D001-RTS-07", COMPATIBILITY_DISPATCH_SYMBOL),
)
RUNTIME_SURFACE_DOC_SNIPPETS = (
    SnippetCheck("M254-D001-RTDOC-01", "`M254-D001` freezes the runtime-owned bootstrap API surface"),
    SnippetCheck("M254-D001-RTDOC-02", f"- image walk remains deferred to `M254-D002`"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M254-D001-TYPES-01", "struct Objc3RuntimeBootstrapApiSummary {"),
    SnippetCheck("M254-D001-TYPES-02", "std::string selector_handle_type ="),
    SnippetCheck("M254-D001-TYPES-03", "std::string startup_invocation_model ="),
    SnippetCheck("M254-D001-TYPES-04", "bool runtime_probe_required = false;"),
    SnippetCheck(
        "M254-D001-TYPES-05",
        "inline bool IsReadyObjc3RuntimeBootstrapApiSummary(",
    ),
)
FRONTEND_ARTIFACTS_H_SNIPPETS = (
    SnippetCheck("M254-D001-ARTH-01", "Objc3RuntimeBootstrapApiSummary runtime_bootstrap_api_summary;"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-D001-ART-01", "BuildRuntimeBootstrapApiSummary("),
    SnippetCheck("M254-D001-ART-02", "BuildRuntimeBootstrapApiSummaryJson("),
    SnippetCheck("M254-D001-ART-03", "objc_runtime_bootstrap_api_contract"),
    SnippetCheck("M254-D001-ART-04", "runtime_bootstrap_api_selector_handle_type"),
    SnippetCheck("M254-D001-ART-05", "runtime_bootstrap_api_selector_lookup_symbol"),
    SnippetCheck("M254-D001-ART-06", "runtime_bootstrap_api_dispatch_entrypoint_symbol"),
    SnippetCheck("M254-D001-ART-07", "runtime_bootstrap_api_runtime_state_locking_model"),
    SnippetCheck("M254-D001-ART-08", "runtime_bootstrap_api_image_walk_lifecycle_model"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-D001-DRV-01", "M254-D001 runtime-bootstrap-api anchor"),
    SnippetCheck("M254-D001-DRV-02", "bootstrap_runtime_api_selector_lookup_symbol"),
    SnippetCheck("M254-D001-DRV-03", "bootstrap_runtime_api_dispatch_entrypoint_symbol"),
)
PROCESS_H_SNIPPETS = (
    SnippetCheck("M254-D001-PROCH-01", "bootstrap_runtime_api_selector_handle_type"),
    SnippetCheck("M254-D001-PROCH-02", "bootstrap_runtime_api_selector_lookup_symbol"),
    SnippetCheck("M254-D001-PROCH-03", "bootstrap_runtime_api_dispatch_entrypoint_symbol"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-D001-PROC-01", "M254-D001 runtime-bootstrap-api anchor"),
    SnippetCheck("M254-D001-PROC-02", "bootstrap_runtime_api_selector_handle_type"),
    SnippetCheck("M254-D001-PROC-03", "bootstrap_runtime_api_selector_lookup_symbol"),
    SnippetCheck("M254-D001-PROC-04", "bootstrap_runtime_api_dispatch_entrypoint_symbol"),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M254-D001-FRONT-01", "runtime_bootstrap_api_summary"),
    SnippetCheck("M254-D001-FRONT-02", "bootstrap_runtime_api_selector_lookup_symbol"),
    SnippetCheck("M254-D001-FRONT-03", "bootstrap_runtime_api_dispatch_entrypoint_symbol"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-D001-TRTDOC-01", "`M254-D001` freezes the runtime-owned bootstrap API surface"),
    SnippetCheck("M254-D001-TRTDOC-02", f"- image walk remains deferred to `M254-D002`"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M254-D001-PRB-01", '#include "runtime/objc3_runtime.h"'),
    SnippetCheck("M254-D001-PRB-02", "objc3_runtime_copy_registration_state_for_testing"),
    SnippetCheck("M254-D001-PRB-03", '"runtimeBootstrapApiProbe"'),
    SnippetCheck("M254-D001-PRB-04", '"bootstrap:ready:"'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M254-D001-PKG-01",
        '"check:objc3c:m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze": "python scripts/check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M254-D001-PKG-02",
        '"test:tooling:m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze": "python -m pytest tests/tooling/test_check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M254-D001-PKG-03",
        '"check:objc3c:m254-d001-lane-d-readiness": "python scripts/run_m254_d001_lane_d_readiness.py"',
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--runtime-header", type=Path, default=DEFAULT_RUNTIME_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--runtime-surface-doc", type=Path, default=DEFAULT_RUNTIME_SURFACE_DOC)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--frontend-artifacts-h", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_H)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-h", type=Path, default=DEFAULT_PROCESS_H)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--tooling-runtime-readme", type=Path, default=DEFAULT_TOOLING_RUNTIME_README)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}")
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}")
        )
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}")
            )
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


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


def stale_inputs(target: Path, inputs: Sequence[Path]) -> list[str]:
    if not target.exists():
        return [display_path(path) for path in inputs]
    target_mtime = target.stat().st_mtime_ns
    stale: list[str] = []
    for path in inputs:
        if path.exists() and path.stat().st_mtime_ns > target_mtime:
            stale.append(display_path(path))
    return stale


def find_objc_runtime_bootstrap_surface(manifest_payload: dict[str, Any]) -> tuple[dict[str, Any] | None, dict[str, Any] | None]:
    frontend = manifest_payload.get("frontend")
    if not isinstance(frontend, dict):
        return None, None
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        return None, None
    semantic_surface = pipeline.get("semantic_surface")
    sema_pass_manager = pipeline.get("sema_pass_manager")
    if not isinstance(semantic_surface, dict) or not isinstance(sema_pass_manager, dict):
        return None, None
    packet = semantic_surface.get("objc_runtime_bootstrap_api_contract")
    return packet if isinstance(packet, dict) else None, sema_pass_manager


def selector_score(selector: str) -> int:
    total = 0
    for index, byte in enumerate(selector.encode("utf-8"), start=1):
        total = (total + (byte * index)) % DISPATCH_MODULUS
    return total


def expected_dispatch(receiver: int, selector: str, a0: int, a1: int, a2: int, a3: int) -> int:
    value = 41
    value += receiver * 97
    value += a0 * 7
    value += a1 * 11
    value += a2 * 13
    value += a3 * 17
    value += selector_score(selector) * 19
    value %= DISPATCH_MODULUS
    if value < 0:
        value += DISPATCH_MODULUS
    return value


def run_manifest_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, list[dict[str, object]], int]:
    checks_total = 0
    checks_passed = 0
    dynamic_cases: list[dict[str, object]] = []

    native_stale = stale_inputs(
        args.native_exe.resolve(),
        (
            args.frontend_artifacts.resolve(),
            args.frontend_artifacts_h.resolve(),
            args.frontend_types.resolve(),
            args.driver_cpp.resolve(),
            args.process_h.resolve(),
            args.process_cpp.resolve(),
            args.runtime_header.resolve(),
            args.runtime_source.resolve(),
            args.runtime_probe.resolve(),
        ),
    )
    checks_total += 1
    if not native_stale:
        checks_passed += 1
    else:
        failures.append(
            Finding(
                display_path(args.native_exe),
                "M254-D001-DYN-STALE-NATIVE",
                "native executable is older than D001 inputs: " + ", ".join(native_stale),
            )
        )
        return checks_total, dynamic_cases, checks_passed

    out_dir = args.probe_root.resolve() / "hello"
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        command.extend(["--llc", str(llc)])

    completed = run_command(command, ROOT)
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    case_payload: dict[str, object] = {
        "case_id": HELLO_FIXTURE_CASE_ID,
        "command": command,
        "process_exit_code": completed.returncode,
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
    }

    checks_total += require(completed.returncode == 0, display_path(out_dir), "M254-D001-DYN-01", "native compile must succeed", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M254-D001-DYN-02", "manifest is missing", failures)
    checks_total += require(
        registration_manifest_path.exists(),
        display_path(registration_manifest_path),
        "M254-D001-DYN-03",
        "registration manifest is missing",
        failures,
    )
    if failures:
        dynamic_cases.append(case_payload)
        return checks_total, dynamic_cases, checks_passed + max(0, 3 - len([f for f in failures if f.check_id in {"M254-D001-DYN-01", "M254-D001-DYN-02", "M254-D001-DYN-03"}]))

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    packet, flat = find_objc_runtime_bootstrap_surface(manifest_payload)

    checks_total += require(packet is not None, display_path(manifest_path), "M254-D001-DYN-04", "runtime bootstrap API semantic packet missing", failures)
    checks_total += require(flat is not None, display_path(manifest_path), "M254-D001-DYN-05", "flattened sema-pass-manager summary missing", failures)
    if packet is None or flat is None:
        dynamic_cases.append(case_payload)
        return checks_total, dynamic_cases, checks_passed

    packet_expectations = {
        "contract_id": CONTRACT_ID,
        "support_library_core_feature_contract_id": SUPPORT_LIBRARY_CORE_FEATURE_CONTRACT_ID,
        "support_library_link_wiring_contract_id": SUPPORT_LIBRARY_LINK_WIRING_CONTRACT_ID,
        "bootstrap_surface_path": SURFACE_PATH,
        "public_header_path": PUBLIC_HEADER_PATH,
        "archive_relative_path": ARCHIVE_RELATIVE_PATH,
        "registration_status_enum_type": REGISTRATION_STATUS_ENUM_TYPE,
        "image_descriptor_type": IMAGE_DESCRIPTOR_TYPE,
        "selector_handle_type": SELECTOR_HANDLE_TYPE,
        "registration_snapshot_type": REGISTRATION_SNAPSHOT_TYPE,
        "registration_entrypoint_symbol": REGISTRATION_ENTRYPOINT_SYMBOL,
        "selector_lookup_symbol": SELECTOR_LOOKUP_SYMBOL,
        "dispatch_entrypoint_symbol": DISPATCH_ENTRYPOINT_SYMBOL,
        "compatibility_dispatch_symbol": COMPATIBILITY_DISPATCH_SYMBOL,
        "state_snapshot_symbol": STATE_SNAPSHOT_SYMBOL,
        "reset_for_testing_symbol": RESET_FOR_TESTING_SYMBOL,
        "registration_result_model": REGISTRATION_RESULT_MODEL,
        "registration_order_ordinal_model": REGISTRATION_ORDER_ORDINAL_MODEL,
        "runtime_state_locking_model": RUNTIME_STATE_LOCKING_MODEL,
        "startup_invocation_model": STARTUP_INVOCATION_MODEL,
        "image_walk_lifecycle_model": IMAGE_WALK_LIFECYCLE_MODEL,
        "deterministic_reset_lifecycle_model": DETERMINISTIC_RESET_LIFECYCLE_MODEL,
    }
    for key, expected in packet_expectations.items():
        checks_total += require(
            packet.get(key) == expected,
            display_path(manifest_path),
            f"M254-D001-DYN-PKT-{key}",
            f"semantic packet field {key} mismatch: expected {expected!r}, got {packet.get(key)!r}",
            failures,
        )
    for key in (
        "ready",
        "fail_closed",
        "support_library_core_feature_contract_ready",
        "support_library_link_wiring_contract_ready",
        "api_surface_frozen",
        "registration_entrypoint_frozen",
        "selector_lookup_and_dispatch_frozen",
        "reset_and_snapshot_hooks_frozen",
        "runtime_probe_required",
        "image_walk_not_yet_landed",
        "deterministic_reset_expansion_not_yet_landed",
        "ready_for_registrar_implementation",
    ):
        checks_total += require(
            packet.get(key) is True,
            display_path(manifest_path),
            f"M254-D001-DYN-PKT-{key}",
            f"semantic packet flag {key} must be true",
            failures,
        )
    for key in (
        "support_library_core_feature_replay_key",
        "support_library_link_wiring_replay_key",
        "replay_key",
    ):
        checks_total += require(
            isinstance(packet.get(key), str) and bool(packet.get(key)),
            display_path(manifest_path),
            f"M254-D001-DYN-PKT-{key}",
            f"semantic packet field {key} must be a non-empty string",
            failures,
        )
    checks_total += require(
        packet.get("failure_reason") == "",
        display_path(manifest_path),
        "M254-D001-DYN-PKT-failure_reason",
        "semantic packet failure_reason must be empty",
        failures,
    )

    flat_expectations = {
        "runtime_bootstrap_api_contract_id": CONTRACT_ID,
        "runtime_bootstrap_api_public_header_path": PUBLIC_HEADER_PATH,
        "runtime_bootstrap_api_archive_relative_path": ARCHIVE_RELATIVE_PATH,
        "runtime_bootstrap_api_registration_status_enum_type": REGISTRATION_STATUS_ENUM_TYPE,
        "runtime_bootstrap_api_image_descriptor_type": IMAGE_DESCRIPTOR_TYPE,
        "runtime_bootstrap_api_selector_handle_type": SELECTOR_HANDLE_TYPE,
        "runtime_bootstrap_api_registration_snapshot_type": REGISTRATION_SNAPSHOT_TYPE,
        "runtime_bootstrap_api_registration_entrypoint_symbol": REGISTRATION_ENTRYPOINT_SYMBOL,
        "runtime_bootstrap_api_selector_lookup_symbol": SELECTOR_LOOKUP_SYMBOL,
        "runtime_bootstrap_api_dispatch_entrypoint_symbol": DISPATCH_ENTRYPOINT_SYMBOL,
        "runtime_bootstrap_api_compatibility_dispatch_symbol": COMPATIBILITY_DISPATCH_SYMBOL,
        "runtime_bootstrap_api_state_snapshot_symbol": STATE_SNAPSHOT_SYMBOL,
        "runtime_bootstrap_api_reset_for_testing_symbol": RESET_FOR_TESTING_SYMBOL,
        "runtime_bootstrap_api_registration_result_model": REGISTRATION_RESULT_MODEL,
        "runtime_bootstrap_api_registration_order_ordinal_model": REGISTRATION_ORDER_ORDINAL_MODEL,
        "runtime_bootstrap_api_runtime_state_locking_model": RUNTIME_STATE_LOCKING_MODEL,
        "runtime_bootstrap_api_startup_invocation_model": STARTUP_INVOCATION_MODEL,
        "runtime_bootstrap_api_image_walk_lifecycle_model": IMAGE_WALK_LIFECYCLE_MODEL,
        "runtime_bootstrap_api_deterministic_reset_lifecycle_model": DETERMINISTIC_RESET_LIFECYCLE_MODEL,
    }
    for key, expected in flat_expectations.items():
        checks_total += require(
            flat.get(key) == expected,
            display_path(manifest_path),
            f"M254-D001-DYN-FLAT-{key}",
            f"flattened field {key} mismatch: expected {expected!r}, got {flat.get(key)!r}",
            failures,
        )
    checks_total += require(
        flat.get("runtime_bootstrap_api_ready_for_registrar_implementation") is True,
        display_path(manifest_path),
        "M254-D001-DYN-FLAT-ready",
        "flattened ready_for_registrar_implementation must be true",
        failures,
    )
    for key in (
        "runtime_bootstrap_api_support_library_core_feature_replay_key",
        "runtime_bootstrap_api_support_library_link_wiring_replay_key",
        "runtime_bootstrap_api_replay_key",
    ):
        checks_total += require(
            isinstance(flat.get(key), str) and bool(flat.get(key)),
            display_path(manifest_path),
            f"M254-D001-DYN-FLAT-{key}",
            f"flattened field {key} must be a non-empty string",
            failures,
        )
    checks_total += require(
        flat.get("runtime_bootstrap_api_failure_reason") == "",
        display_path(manifest_path),
        "M254-D001-DYN-FLAT-failure_reason",
        "flattened failure reason must be empty",
        failures,
    )

    registration_expectations = {
        "bootstrap_runtime_api_contract_id": CONTRACT_ID,
        "bootstrap_runtime_api_public_header_path": PUBLIC_HEADER_PATH,
        "bootstrap_runtime_api_archive_relative_path": ARCHIVE_RELATIVE_PATH,
        "bootstrap_runtime_api_registration_status_enum_type": REGISTRATION_STATUS_ENUM_TYPE,
        "bootstrap_runtime_api_image_descriptor_type": IMAGE_DESCRIPTOR_TYPE,
        "bootstrap_runtime_api_selector_handle_type": SELECTOR_HANDLE_TYPE,
        "bootstrap_runtime_api_registration_snapshot_type": REGISTRATION_SNAPSHOT_TYPE,
        "bootstrap_runtime_api_registration_entrypoint_symbol": REGISTRATION_ENTRYPOINT_SYMBOL,
        "bootstrap_runtime_api_selector_lookup_symbol": SELECTOR_LOOKUP_SYMBOL,
        "bootstrap_runtime_api_dispatch_entrypoint_symbol": DISPATCH_ENTRYPOINT_SYMBOL,
        "bootstrap_runtime_api_state_snapshot_symbol": STATE_SNAPSHOT_SYMBOL,
        "bootstrap_runtime_api_reset_for_testing_symbol": RESET_FOR_TESTING_SYMBOL,
    }
    for key, expected in registration_expectations.items():
        checks_total += require(
            registration_manifest_payload.get(key) == expected,
            display_path(registration_manifest_path),
            f"M254-D001-DYN-REG-{key}",
            f"registration manifest field {key} mismatch: expected {expected!r}, got {registration_manifest_payload.get(key)!r}",
            failures,
        )

    case_payload.update(
        {
            "packet_contract_id": packet.get("contract_id"),
            "packet_replay_key": packet.get("replay_key"),
            "flat_replay_key": flat.get("runtime_bootstrap_api_replay_key"),
            "registration_manifest_contract_id": registration_manifest_payload.get("bootstrap_runtime_api_contract_id"),
        }
    )
    dynamic_cases.append(case_payload)
    checks_passed = checks_total - len([finding for finding in failures if finding.artifact in {display_path(manifest_path), display_path(registration_manifest_path), display_path(out_dir), display_path(args.native_exe)}])
    return checks_total, dynamic_cases, checks_passed


def run_probe_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, list[dict[str, object]], int]:
    checks_total = 0
    checks_passed = 0
    dynamic_cases: list[dict[str, object]] = []

    runtime_lib_stale = stale_inputs(
        args.runtime_library.resolve(),
        (args.runtime_header.resolve(), args.runtime_source.resolve()),
    )
    checks_total += 1
    if not runtime_lib_stale:
        checks_passed += 1
    else:
        failures.append(
            Finding(
                display_path(args.runtime_library),
                "M254-D001-DYN-STALE-RUNTIME",
                "runtime library is older than runtime sources: " + ", ".join(runtime_lib_stale),
            )
        )
        return checks_total, dynamic_cases, checks_passed

    clangxx = resolve_tool("clang++.exe") or resolve_tool("clang++")
    checks_total += require(clangxx is not None, "dynamic", "M254-D001-DYN-100", "clang++ is required for the runtime probe", failures)
    checks_total += require(args.runtime_library.exists(), display_path(args.runtime_library), "M254-D001-DYN-101", "runtime library is missing", failures)
    if clangxx is None or not args.runtime_library.exists():
        return checks_total, dynamic_cases, checks_passed

    probe_dir = args.probe_root.resolve() / "probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m254_d001_runtime_bootstrap_api_probe.exe"
    compile_command = [
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
    compile_result = run_command(compile_command, ROOT)
    case_payload: dict[str, object] = {
        "case_id": PROBE_CASE_ID,
        "compile_command": compile_command,
        "compile_exit_code": compile_result.returncode,
    }

    checks_total += require(compile_result.returncode == 0, display_path(args.runtime_probe), "M254-D001-DYN-102", "runtime probe compile must succeed", failures)
    checks_total += require(probe_exe.exists(), display_path(probe_exe), "M254-D001-DYN-103", "runtime probe executable missing", failures)
    if compile_result.returncode != 0 or not probe_exe.exists():
        dynamic_cases.append(case_payload)
        return checks_total, dynamic_cases, checks_passed

    run_result = run_command([str(probe_exe)], ROOT)
    case_payload["run_exit_code"] = run_result.returncode
    checks_total += require(run_result.returncode == 0, display_path(probe_exe), "M254-D001-DYN-104", "runtime probe must exit cleanly", failures)
    if run_result.returncode != 0:
        dynamic_cases.append(case_payload)
        return checks_total, dynamic_cases, checks_passed

    try:
        probe_payload = json.loads(run_result.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), "M254-D001-DYN-105", f"runtime probe stdout is not JSON: {exc}"))
        dynamic_cases.append(case_payload)
        return checks_total + 1, dynamic_cases, checks_passed
    checks_total += 1

    expected = expected_dispatch(5, "bootstrap:ready:", 1, 2, 3, 4)
    value_expectations = {
        "initial_copy_status": 0,
        "register_status": 0,
        "post_register_copy_status": 0,
        "selector_stable_id": 1,
        "dispatch_result": expected,
        "expected_dispatch_result": expected,
        "post_register_registered_image_count": 1,
        "post_register_registered_descriptor_total": 5,
        "post_register_next_expected_registration_order_ordinal": 2,
        "post_register_last_registration_status": 0,
        "post_register_last_registered_module_name": "runtimeBootstrapApiProbe",
        "post_register_last_registered_translation_unit_identity_key": "runtime-bootstrap-api-probe::translation-unit",
        "post_register_last_successful_registration_order_ordinal": 1,
        "post_reset_copy_status": 0,
        "post_reset_registered_image_count": 0,
        "post_reset_registered_descriptor_total": 0,
        "post_reset_next_expected_registration_order_ordinal": 1,
        "post_reset_last_registration_status": 0,
        "post_reset_last_registered_module_name": None,
        "post_reset_last_registered_translation_unit_identity_key": None,
        "post_reset_last_successful_registration_order_ordinal": 0,
        "selector_after_reset_stable_id": 1,
    }
    for key, expected_value in value_expectations.items():
        checks_total += require(
            probe_payload.get(key) == expected_value,
            display_path(probe_exe),
            f"M254-D001-DYN-PROBE-{key}",
            f"probe field {key} mismatch: expected {expected_value!r}, got {probe_payload.get(key)!r}",
            failures,
        )

    case_payload["probe_payload"] = probe_payload
    dynamic_cases.append(case_payload)
    checks_passed = checks_total - len([finding for finding in failures if finding.artifact in {display_path(probe_exe), display_path(args.runtime_probe), display_path(args.runtime_library)}])
    return checks_total, dynamic_cases, checks_passed


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0

    artifacts: tuple[tuple[Path, str, tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, "M254-D001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M254-D001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M254-D001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M254-D001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M254-D001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M254-D001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M254-D001-AST-EXISTS", AST_SNIPPETS),
        (args.runtime_header, "M254-D001-RTH-EXISTS", RUNTIME_HEADER_SNIPPETS),
        (args.runtime_source, "M254-D001-RTS-EXISTS", RUNTIME_SOURCE_SNIPPETS),
        (args.runtime_surface_doc, "M254-D001-RTDOC-EXISTS", RUNTIME_SURFACE_DOC_SNIPPETS),
        (args.frontend_types, "M254-D001-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, "M254-D001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.frontend_artifacts_h, "M254-D001-ARTH-EXISTS", FRONTEND_ARTIFACTS_H_SNIPPETS),
        (args.driver_cpp, "M254-D001-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.process_h, "M254-D001-PROCH-EXISTS", PROCESS_H_SNIPPETS),
        (args.process_cpp, "M254-D001-PROC-EXISTS", PROCESS_CPP_SNIPPETS),
        (args.frontend_anchor_cpp, "M254-D001-FRONT-EXISTS", FRONTEND_ANCHOR_SNIPPETS),
        (args.tooling_runtime_readme, "M254-D001-TRTDOC-EXISTS", TOOLING_RUNTIME_README_SNIPPETS),
        (args.runtime_probe, "M254-D001-PRB-EXISTS", PROBE_SNIPPETS),
        (args.package_json, "M254-D001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )

    for path, exists_check_id, snippets in artifacts:
        artifact_checks, artifact_findings = check_doc_contract(
            path=path, exists_check_id=exists_check_id, snippets=snippets
        )
        checks_total += artifact_checks
        failures.extend(artifact_findings)

    dynamic_cases: list[dict[str, object]] = []
    checks_passed = checks_total - len(failures)
    if not args.skip_dynamic_probes:
        manifest_checks, manifest_cases, manifest_passed = run_manifest_case(
            args=args, failures=failures
        )
        checks_total += manifest_checks
        dynamic_cases.extend(manifest_cases)
        checks_passed += manifest_passed

        probe_checks, probe_cases, probe_passed = run_probe_case(
            args=args, failures=failures
        )
        checks_total += probe_checks
        dynamic_cases.extend(probe_cases)
        checks_passed += probe_passed

    checks_passed = checks_total - len(failures)
    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }
    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run())
