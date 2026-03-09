#!/usr/bin/env python3
"""Validate M263-B003 bootstrap failure/restart semantics."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable, Mapping, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-b003-bootstrap-failure-restart-semantics-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1"
LEGality_CONTRACT_ID = "objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1"
BOOTSTRAP_SEMANTICS_CONTRACT_ID = "objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1"
BOOTSTRAP_RESET_CONTRACT_ID = "objc3c-runtime-bootstrap-reset-replay/m254-d003-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_failure_restart_semantics"
FAILURE_MODE = "abort-before-user-main-no-partial-registration-commit"
RESTART_LIFECYCLE_MODEL = "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells"
REPLAY_ORDER_MODEL = "replay-re-registers-retained-images-in-original-registration-order"
IMAGE_LOCAL_INIT_RESET_MODEL = "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay"
CATALOG_RETENTION_MODEL = "bootstrap-catalog-retained-across-reset-for-deterministic-replay"
UNSUPPORTED_TOPOLOGY_MODEL = "replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"
REPLAY_SYMBOL = "objc3_runtime_replay_registered_images_for_testing"
RESET_REPLAY_SNAPSHOT_SYMBOL = "objc3_runtime_copy_reset_replay_state_for_testing"
RUNTIME_STATE_SNAPSHOT_SYMBOL = "objc3_runtime_copy_registration_state_for_testing"
INVALID_DESCRIPTOR_STATUS_CODE = -1

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PROBE_SOURCE = ROOT / "tests" / "tooling" / "runtime" / "m263_b003_bootstrap_failure_restart_probe.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_failure_restart_explicit.objc3"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_failure_restart_default.objc3"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-B003" / "bootstrap_failure_restart_semantics_summary.json"
PROBE_ROOT = ROOT / "tmp" / "reports" / "m263" / "M263-B003" / "dynamic"
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


STATIC_SNIPPETS: Mapping[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M263-B003-DOC-EXP-01", CONTRACT_ID),
        SnippetCheck("M263-B003-DOC-EXP-02", "`objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`"),
        SnippetCheck("M263-B003-DOC-EXP-03", "`objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`"),
        SnippetCheck("M263-B003-DOC-EXP-04", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-B003-DOC-EXP-05", "`replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M263-B003-PACKET-01", "Dependencies: `M263-B002`, `M254-B002`, `M254-D003`"),
        SnippetCheck("M263-B003-PACKET-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-B003-PACKET-03", f"`{REPLAY_SYMBOL}`"),
        SnippetCheck("M263-B003-PACKET-04", f"`{RESET_REPLAY_SNAPSHOT_SYMBOL}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M263-B003-NDOC-01", "## Bootstrap failure-mode and restart semantics (M263-B003)"),
        SnippetCheck("M263-B003-NDOC-02", CONTRACT_ID),
        SnippetCheck("M263-B003-NDOC-03", "replay without reset fails closed"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M263-B003-LSPEC-01", "## M263 bootstrap failure-mode and restart semantics (B003)"),
        SnippetCheck("M263-B003-LSPEC-02", CONTRACT_ID),
        SnippetCheck("M263-B003-LSPEC-03", "replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M263-B003-MSPEC-01", "## M263 bootstrap failure-mode and restart semantics metadata anchors (B003)"),
        SnippetCheck("M263-B003-MSPEC-02", CONTRACT_ID),
        SnippetCheck("M263-B003-MSPEC-03", "reset_replay_state_snapshot_symbol"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M263-B003-ARCH-01", "## M263 bootstrap failure-mode and restart semantics (B003)"),
        SnippetCheck("M263-B003-ARCH-02", "reset plus replay restores canonical registration order deterministically"),
        SnippetCheck("M263-B003-ARCH-03", "check:objc3c:m263-b003-lane-b-readiness"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M263-B003-PKG-01", '"check:objc3c:m263-b003-bootstrap-failure-mode-and-restart-semantics"'),
        SnippetCheck("M263-B003-PKG-02", '"test:tooling:m263-b003-bootstrap-failure-mode-and-restart-semantics"'),
        SnippetCheck("M263-B003-PKG-03", '"check:objc3c:m263-b003-lane-b-readiness"'),
    ),
    SEMA_CONTRACT: (
        SnippetCheck("M263-B003-SEMA-01", "kObjc3BootstrapFailureRestartSemanticsContractId"),
        SnippetCheck("M263-B003-SEMA-02", "kObjc3BootstrapFailureRestartUnsupportedTopologyModel"),
        SnippetCheck("M263-B003-SEMA-03", "struct Objc3BootstrapFailureRestartSemanticsSummary"),
        SnippetCheck("M263-B003-SEMA-04", "bootstrap_failure_restart_semantics_summary;"),
    ),
    SEMANTIC_PASSES: (
        SnippetCheck("M263-B003-PASS-01", "BuildBootstrapFailureRestartSemanticsReplayKey"),
        SnippetCheck("M263-B003-PASS-02", "BuildBootstrapFailureRestartSemanticsSummaryFromIntegrationSurface"),
        SnippetCheck("M263-B003-PASS-03", "surface.bootstrap_failure_restart_semantics_summary ="),
    ),
    SEMA_PASS_MANAGER_CONTRACT: (
        SnippetCheck("M263-B003-SPMC-01", "bootstrap_failure_restart_semantics_summary;"),
        SnippetCheck("M263-B003-SPMC-02", "IsReadyObjc3BootstrapFailureRestartSemanticsSummary("),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M263-B003-SPM-01", "result.parity_surface.bootstrap_failure_restart_semantics_summary ="),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M263-B003-TYPES-01", "struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary"),
        SnippetCheck("M263-B003-TYPES-02", "replay_registered_images_symbol"),
        SnippetCheck("M263-B003-TYPES-03", "unsupported_topology_semantics_landed"),
    ),
    FRONTEND_ARTIFACTS_H: (
        SnippetCheck("M263-B003-ARTH-01", "runtime_bootstrap_failure_restart_semantics_summary;"),
    ),
    FRONTEND_ARTIFACTS_CPP: (
        SnippetCheck("M263-B003-ART-01", "BuildRuntimeBootstrapFailureRestartSemanticsSummary("),
        SnippetCheck("M263-B003-ART-02", "BuildRuntimeBootstrapFailureRestartSemanticsSummaryJson("),
        SnippetCheck("M263-B003-ART-03", "objc_runtime_bootstrap_failure_restart_semantics"),
        SnippetCheck("M263-B003-ART-04", "runtime_bootstrap_failure_restart_semantics_contract_id"),
    ),
    PROBE_SOURCE: (
        SnippetCheck("M263-B003-PROBE-01", "objc3_runtime_replay_registered_images_for_testing"),
        SnippetCheck("M263-B003-PROBE-02", "unsupported_replay_status"),
        SnippetCheck("M263-B003-PROBE-03", "second_restart_status"),
    ),
}


def display_path(path: Path | str) -> str:
    if isinstance(path, str):
        return path
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except Exception:
        return str(path.resolve()).replace("\\", "/")


def canonical_json(payload: Mapping[str, Any]) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if not condition:
        failures.append(Finding(artifact, check_id, detail))
        return 0
    return 1


def ensure_snippets(path: Path, snippets: Iterable[SnippetCheck], failures: list[Finding]) -> int:
    text = path.read_text(encoding="utf-8")
    passed = 1
    passed += require(path.exists(), display_path(path), f"{path.name}-EXISTS", "artifact missing", failures)
    for snippet in snippets:
        passed += require(snippet.snippet in text, display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}", failures)
    return passed


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


def resolve_tool(*names: str) -> str | None:
    for name in names:
        resolved = shutil.which(name)
        if resolved:
            return resolved
    return None


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks = 1 + len(snippets)
    if not path.exists():
        failures.append(Finding(display_path(path), f"{path.name}-EXISTS", "artifact missing"))
        return checks, failures
    ensure_snippets(path, snippets, failures)
    return checks, failures


def extract_semantic_surface(manifest_payload: dict[str, Any]) -> tuple[dict[str, Any] | None, dict[str, Any] | None]:
    frontend = manifest_payload.get("frontend")
    if not isinstance(frontend, dict):
        return None, None
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        return None, None
    semantic_surface = pipeline.get("semantic_surface")
    sema_pass_manager = pipeline.get("sema_pass_manager")
    return (
        semantic_surface if isinstance(semantic_surface, dict) else None,
        sema_pass_manager if isinstance(sema_pass_manager, dict) else None,
    )


def check_expected_container(
    *,
    container: dict[str, Any],
    label: str,
    failures: list[Finding],
    checks_total_ref: list[int],
    expected_identity_key: str,
    expected_ordinal: int,
    require_ready_field: bool,
    prefix: str = "",
) -> None:
    def key(name: str) -> str:
        return f"{prefix}{name}" if prefix else name

    def add(condition: bool, check_id: str, detail: str) -> None:
        checks_total_ref[0] += 1
        require(condition, label, check_id, detail, failures)

    add(container.get(key("contract_id")) == CONTRACT_ID, "M263-B003-CONTRACT", "contract id mismatch")
    add(container.get(key("bootstrap_legality_semantics_contract_id")) == LEGality_CONTRACT_ID, "M263-B003-UPSTREAM-LEGALITY", "bootstrap legality semantics contract id mismatch")
    add(container.get(key("bootstrap_reset_contract_id")) == BOOTSTRAP_RESET_CONTRACT_ID, "M263-B003-UPSTREAM-RESET", "bootstrap reset contract id mismatch")
    add(container.get(key("bootstrap_semantics_contract_id")) == BOOTSTRAP_SEMANTICS_CONTRACT_ID, "M263-B003-UPSTREAM-BOOTSTRAP", "bootstrap semantics contract id mismatch")
    add(container.get(key("frontend_surface_path")) == SURFACE_PATH, "M263-B003-SURFACE-PATH", "surface path mismatch")
    add(container.get(key("failure_mode")) == FAILURE_MODE, "M263-B003-FAILURE-MODE", "failure mode mismatch")
    add(container.get(key("restart_lifecycle_model")) == RESTART_LIFECYCLE_MODEL, "M263-B003-RESTART-MODEL", "restart lifecycle model mismatch")
    add(container.get(key("replay_order_model")) == REPLAY_ORDER_MODEL, "M263-B003-REPLAY-MODEL", "replay order model mismatch")
    add(container.get(key("image_local_init_reset_model")) == IMAGE_LOCAL_INIT_RESET_MODEL, "M263-B003-INIT-RESET", "image-local init reset model mismatch")
    add(container.get(key("catalog_retention_model")) == CATALOG_RETENTION_MODEL, "M263-B003-CATALOG", "catalog retention model mismatch")
    add(container.get(key("unsupported_topology_model")) == UNSUPPORTED_TOPOLOGY_MODEL, "M263-B003-TOPOLOGY", "unsupported topology model mismatch")
    add(container.get(key("translation_unit_identity_model")) == TRANSLATION_UNIT_IDENTITY_MODEL, "M263-B003-IDENTITY-MODEL", "identity model mismatch")
    add(container.get(key("translation_unit_identity_key")) == expected_identity_key, "M263-B003-IDENTITY-KEY", "identity key mismatch")
    add(container.get(key("runtime_state_snapshot_symbol")) == RUNTIME_STATE_SNAPSHOT_SYMBOL, "M263-B003-SNAPSHOT", "runtime state snapshot symbol mismatch")
    add(container.get(key("replay_registered_images_symbol")) == REPLAY_SYMBOL, "M263-B003-REPLAY-SYMBOL", "replay symbol mismatch")
    add(container.get(key("reset_replay_state_snapshot_symbol")) == RESET_REPLAY_SNAPSHOT_SYMBOL, "M263-B003-RESET-SNAPSHOT", "reset replay snapshot symbol mismatch")
    add(container.get(key("invalid_descriptor_status_code")) == INVALID_DESCRIPTOR_STATUS_CODE, "M263-B003-INVALID-STATUS", "invalid descriptor status code mismatch")
    add(container.get(key("translation_unit_registration_order_ordinal")) == expected_ordinal, "M263-B003-ORDINAL", "translation unit registration ordinal mismatch")
    if require_ready_field:
        add(container.get(key("ready")) is True, "M263-B003-READY", "ready must be true")
    for field_name, check_id in (
        ("fail_closed", "M263-B003-FAIL-CLOSED"),
        ("semantic_boundary_ready", "M263-B003-SEMA-READY"),
        ("bootstrap_legality_semantics_contract_ready", "M263-B003-LEGALITY-READY"),
        ("bootstrap_semantics_contract_ready", "M263-B003-BOOTSTRAP-READY"),
        ("bootstrap_reset_contract_ready", "M263-B003-RESET-READY"),
        ("failure_mode_semantics_landed", "M263-B003-FAILURE-LANDED"),
        ("restart_semantics_landed", "M263-B003-RESTART-LANDED"),
        ("replay_semantics_landed", "M263-B003-REPLAY-LANDED"),
        ("unsupported_topology_semantics_landed", "M263-B003-TOPOLOGY-LANDED"),
        ("deterministic_recovery_semantics_landed", "M263-B003-RECOVERY-LANDED"),
        ("runtime_restart_probe_required", "M263-B003-PROBE-REQUIRED"),
        ("ready_for_lowering_and_runtime", "M263-B003-LOWERING-READY"),
    ):
        add(container.get(key(field_name)) is True, check_id, f"{field_name} must be true")
    for field_name, check_id in (
        ("semantic_boundary_replay_key", "M263-B003-SEMA-REPLAY"),
        ("bootstrap_legality_semantics_replay_key", "M263-B003-LEGALITY-REPLAY"),
        ("bootstrap_semantics_replay_key", "M263-B003-BOOTSTRAP-REPLAY"),
        ("replay_key", "M263-B003-REPLAY"),
    ):
        add(bool(container.get(key(field_name))), check_id, f"{field_name} must be non-empty")
    add(container.get(key("failure_reason")) == "", "M263-B003-FAILURE-REASON", "failure reason must be empty")


def compile_fixture(
    *,
    fixture: Path,
    out_dir: Path,
    expected_registration_descriptor: str,
    expected_image_root: str,
    expected_registration_source: str,
    expected_image_root_source: str,
    clangxx: str,
    failures: list[Finding],
) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE.resolve()), str(fixture.resolve()), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    llc = resolve_tool("llc.exe", "llc")
    if llc is not None:
        command.extend(["--llc", llc])
    completed = run_command(command)

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    registration_descriptor_path = out_dir / "module.runtime-registration-descriptor.json"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += 1
    require(completed.returncode == 0, display_path(fixture), "M263-B003-COMPILE", f"native compile exited with {completed.returncode}", failures)
    for artifact_path, check_id, detail in (
        (manifest_path, "M263-B003-MANIFEST", "module manifest is missing"),
        (registration_manifest_path, "M263-B003-REG-MANIFEST", "runtime registration manifest is missing"),
        (registration_descriptor_path, "M263-B003-REG-DESCRIPTOR", "runtime registration descriptor is missing"),
        (obj_path, "M263-B003-OBJ", "object file is missing"),
    ):
        checks_total += 1
        require(artifact_path.exists(), display_path(artifact_path), check_id, detail, failures)
    if completed.returncode != 0 or failures:
        return checks_total, {
            "fixture": display_path(fixture),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    manifest = load_json(manifest_path)
    registration_manifest = load_json(registration_manifest_path)
    registration_descriptor = load_json(registration_descriptor_path)
    semantic_surface, flattened = extract_semantic_surface(manifest)
    surface = semantic_surface.get("objc_runtime_bootstrap_failure_restart_semantics") if isinstance(semantic_surface, dict) else None
    legality_surface = semantic_surface.get("objc_runtime_bootstrap_legality_semantics") if isinstance(semantic_surface, dict) else None
    bootstrap_semantics = semantic_surface.get("objc_runtime_startup_bootstrap_semantics") if isinstance(semantic_surface, dict) else None
    reset_contract = semantic_surface.get("objc_runtime_bootstrap_reset_contract") if isinstance(semantic_surface, dict) else None

    for value, check_id, detail in (
        (isinstance(surface, dict), "M263-B003-SURFACE", "bootstrap failure/restart semantics surface missing"),
        (isinstance(flattened, dict), "M263-B003-FLAT", "flattened sema pass manager surface missing"),
        (isinstance(legality_surface, dict), "M263-B003-LEGALITY-SURFACE", "bootstrap legality semantics surface missing"),
        (isinstance(bootstrap_semantics, dict), "M263-B003-BOOTSTRAP-SURFACE", "bootstrap semantics surface missing"),
        (isinstance(reset_contract, dict), "M263-B003-RESET-SURFACE", "bootstrap reset contract surface missing"),
    ):
        checks_total += 1
        require(value, display_path(manifest_path), check_id, detail, failures)
    if failures:
        return checks_total, {"fixture": display_path(fixture)}

    expected_identity_key = str(registration_manifest.get("translation_unit_identity_key", ""))
    expected_ordinal = int(registration_manifest.get("translation_unit_registration_order_ordinal", 0))
    counter = [0]
    check_expected_container(
        container=surface,
        label=display_path(manifest_path),
        failures=failures,
        checks_total_ref=counter,
        expected_identity_key=expected_identity_key,
        expected_ordinal=expected_ordinal,
        require_ready_field=True,
    )
    check_expected_container(
        container=flattened,
        label=display_path(manifest_path),
        failures=failures,
        checks_total_ref=counter,
        expected_identity_key=expected_identity_key,
        expected_ordinal=expected_ordinal,
        require_ready_field=False,
        prefix="runtime_bootstrap_failure_restart_semantics_",
    )
    checks_total += counter[0]

    for condition, check_id, detail in (
        (surface.get("translation_unit_identity_key") == registration_descriptor.get("translation_unit_identity_key"), "M263-B003-DESCRIPTOR-IDENTITY", "surface identity key must match registration descriptor"),
        (surface.get("translation_unit_identity_model") == registration_manifest.get("translation_unit_identity_model"), "M263-B003-MANIFEST-IDENTITY-MODEL", "surface identity model must match registration manifest"),
        (surface.get("translation_unit_identity_model") == registration_descriptor.get("translation_unit_identity_model"), "M263-B003-DESCRIPTOR-IDENTITY-MODEL", "surface identity model must match registration descriptor"),
        (surface.get("translation_unit_registration_order_ordinal") == registration_descriptor.get("translation_unit_registration_order_ordinal"), "M263-B003-DESCRIPTOR-ORDINAL", "surface ordinal must match registration descriptor"),
        (surface.get("failure_mode") == bootstrap_semantics.get("failure_mode"), "M263-B003-BOOTSTRAP-FAILURE", "failure mode must match bootstrap semantics"),
        (surface.get("translation_unit_identity_model") == legality_surface.get("translation_unit_identity_model"), "M263-B003-LEGALITY-IDENTITY-MODEL", "identity model must match legality surface"),
        (surface.get("translation_unit_identity_key") == legality_surface.get("translation_unit_identity_key"), "M263-B003-LEGALITY-IDENTITY-KEY", "identity key must match legality surface"),
        (reset_contract.get("contract_id") == BOOTSTRAP_RESET_CONTRACT_ID, "M263-B003-RESET-ID", "bootstrap reset contract id mismatch"),
        (reset_contract.get("replay_registered_images_symbol") == REPLAY_SYMBOL, "M263-B003-RESET-REPLAY-SYMBOL", "bootstrap reset replay symbol mismatch"),
        (reset_contract.get("reset_replay_state_snapshot_symbol") == RESET_REPLAY_SNAPSHOT_SYMBOL, "M263-B003-RESET-SNAPSHOT-SYMBOL", "bootstrap reset snapshot symbol mismatch"),
        (reset_contract.get("ready") is True, "M263-B003-RESET-READY", "bootstrap reset contract must be ready"),
        (backend_path.read_text(encoding="utf-8").strip() == "llvm-direct", "M263-B003-BACKEND", "object backend must remain llvm-direct"),
        (registration_descriptor.get("registration_descriptor_identifier") == expected_registration_descriptor, "M263-B003-REG-ID", "registration descriptor identifier mismatch"),
        (registration_descriptor.get("image_root_identifier") == expected_image_root, "M263-B003-ROOT-ID", "image root identifier mismatch"),
        (registration_descriptor.get("registration_descriptor_identity_source") == expected_registration_source, "M263-B003-REG-SOURCE", "registration descriptor identity source mismatch"),
        (registration_descriptor.get("image_root_identity_source") == expected_image_root_source, "M263-B003-ROOT-SOURCE", "image root identity source mismatch"),
    ):
        checks_total += 1
        require(condition, display_path(manifest_path), check_id, detail, failures)
    if failures:
        return checks_total, {"fixture": display_path(fixture)}

    probe_out_dir = PROBE_ROOT / out_dir.name
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / f"{out_dir.name}-bootstrap-failure-restart-probe.exe"
    probe_compile_command = [
        clangxx,
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        f"-I{RUNTIME_INCLUDE_ROOT}",
        str(PROBE_SOURCE),
        str(obj_path),
        str(RUNTIME_LIBRARY),
        "-o",
        str(probe_exe),
    ]
    probe_compile = run_command(probe_compile_command)
    checks_total += 1
    require(probe_compile.returncode == 0, display_path(PROBE_SOURCE), "M263-B003-PROBE-COMPILE", f"probe compile exited with {probe_compile.returncode}", failures)
    checks_total += 1
    require(probe_exe.exists(), display_path(probe_exe), "M263-B003-PROBE-EXE", "probe executable is missing", failures)
    if failures:
        return checks_total, {"fixture": display_path(fixture)}

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    require(probe_run.returncode == 0, display_path(probe_exe), "M263-B003-PROBE-RUN", f"probe exited with {probe_run.returncode}", failures)
    if probe_run.returncode != 0:
        return checks_total, {"fixture": display_path(fixture), "probe_stdout": probe_run.stdout, "probe_stderr": probe_run.stderr}
    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        checks_total += 1
        require(False, display_path(probe_exe), "M263-B003-PROBE-JSON", f"invalid probe JSON: {exc}", failures)
        return checks_total, {"fixture": display_path(fixture), "probe_stdout": probe_run.stdout}

    for condition, check_id, detail in (
        (probe_payload.get("startup_registration_copy_status") == 0, "M263-B003-PROBE-START-REG", "startup registration snapshot copy must succeed"),
        (probe_payload.get("startup_reset_replay_copy_status") == 0, "M263-B003-PROBE-START-RESET", "startup reset snapshot copy must succeed"),
        (probe_payload.get("startup_registered_image_count") == 1, "M263-B003-PROBE-START-COUNT", "startup registered image count mismatch"),
        (probe_payload.get("startup_last_registered_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-START-ID", "startup identity mismatch"),
        (probe_payload.get("unsupported_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, "M263-B003-PROBE-UNSUPPORTED-STATUS", "unsupported replay must fail closed"),
        (probe_payload.get("unsupported_replay_registration_copy_status") == 0, "M263-B003-PROBE-UNSUPPORTED-REG", "unsupported replay registration snapshot copy must succeed"),
        (probe_payload.get("unsupported_replay_reset_replay_copy_status") == 0, "M263-B003-PROBE-UNSUPPORTED-RESET", "unsupported replay reset snapshot copy must succeed"),
        (probe_payload.get("unsupported_replay_registered_image_count") == 1, "M263-B003-PROBE-UNSUPPORTED-COUNT", "unsupported replay must not clear live state"),
        (probe_payload.get("unsupported_replay_last_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, "M263-B003-PROBE-UNSUPPORTED-LAST-STATUS", "unsupported replay status snapshot mismatch"),
        (probe_payload.get("unsupported_replay_last_replayed_image_count") == 0, "M263-B003-PROBE-UNSUPPORTED-REPLAYED", "unsupported replay must not replay any image"),
        (probe_payload.get("post_reset_registration_copy_status") == 0, "M263-B003-PROBE-RESET-REG", "post-reset registration snapshot copy must succeed"),
        (probe_payload.get("post_reset_reset_replay_copy_status") == 0, "M263-B003-PROBE-RESET-SNAPSHOT", "post-reset reset snapshot copy must succeed"),
        (probe_payload.get("post_reset_registered_image_count") == 0, "M263-B003-PROBE-RESET-COUNT", "post-reset registered image count must be zero"),
        (probe_payload.get("post_reset_next_expected_registration_order_ordinal") == 1, "M263-B003-PROBE-RESET-ORDINAL", "post-reset next ordinal must be 1"),
        (probe_payload.get("post_reset_retained_bootstrap_image_count") == 1, "M263-B003-PROBE-RESET-RETAINED", "post-reset retained image count mismatch"),
        (probe_payload.get("post_reset_last_reset_cleared_image_local_init_state_count", 0) >= 1, "M263-B003-PROBE-RESET-CLEARED", "reset must clear at least one image-local init cell"),
        (probe_payload.get("post_reset_reset_generation", 0) >= 1, "M263-B003-PROBE-RESET-GEN", "reset generation must advance"),
        (probe_payload.get("first_restart_status") == 0, "M263-B003-PROBE-FIRST-RESTART-STATUS", "first restart replay must succeed"),
        (probe_payload.get("first_restart_registration_copy_status") == 0, "M263-B003-PROBE-FIRST-RESTART-REG", "first restart registration snapshot copy must succeed"),
        (probe_payload.get("first_restart_image_walk_copy_status") == 0, "M263-B003-PROBE-FIRST-RESTART-WALK", "first restart image-walk snapshot copy must succeed"),
        (probe_payload.get("first_restart_reset_replay_copy_status") == 0, "M263-B003-PROBE-FIRST-RESTART-RESET", "first restart reset snapshot copy must succeed"),
        (probe_payload.get("first_restart_registered_image_count") == 1, "M263-B003-PROBE-FIRST-RESTART-COUNT", "first restart must restore one image"),
        (probe_payload.get("first_restart_last_registration_status") == 0, "M263-B003-PROBE-FIRST-RESTART-LAST-STATUS", "first restart registration status mismatch"),
        (probe_payload.get("first_restart_last_replayed_image_count") == 1, "M263-B003-PROBE-FIRST-RESTART-REPLAYED", "first restart replayed image count mismatch"),
        (probe_payload.get("first_restart_replay_generation", 0) >= 1, "M263-B003-PROBE-FIRST-RESTART-GEN", "first restart replay generation must advance"),
        (probe_payload.get("first_restart_last_registered_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-FIRST-RESTART-REG-ID", "first restart registered identity mismatch"),
        (probe_payload.get("first_restart_last_replayed_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-FIRST-RESTART-REPLAY-ID", "first restart replayed identity mismatch"),
        (probe_payload.get("first_restart_last_walked_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-FIRST-RESTART-WALK-ID", "first restart walked identity mismatch"),
        (probe_payload.get("second_unsupported_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, "M263-B003-PROBE-SECOND-UNSUPPORTED-STATUS", "second unsupported replay must fail closed"),
        (probe_payload.get("second_unsupported_replay_registration_copy_status") == 0, "M263-B003-PROBE-SECOND-UNSUPPORTED-REG", "second unsupported replay registration snapshot copy must succeed"),
        (probe_payload.get("second_unsupported_replay_reset_replay_copy_status") == 0, "M263-B003-PROBE-SECOND-UNSUPPORTED-RESET", "second unsupported replay reset snapshot copy must succeed"),
        (probe_payload.get("second_unsupported_replay_registered_image_count") == 1, "M263-B003-PROBE-SECOND-UNSUPPORTED-COUNT", "second unsupported replay must not clear live state"),
        (probe_payload.get("second_unsupported_replay_last_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, "M263-B003-PROBE-SECOND-UNSUPPORTED-LAST-STATUS", "second unsupported replay status snapshot mismatch"),
        (probe_payload.get("second_reset_registration_copy_status") == 0, "M263-B003-PROBE-SECOND-RESET-REG", "second reset registration snapshot copy must succeed"),
        (probe_payload.get("second_reset_reset_replay_copy_status") == 0, "M263-B003-PROBE-SECOND-RESET-SNAPSHOT", "second reset reset snapshot copy must succeed"),
        (probe_payload.get("second_reset_registered_image_count") == 0, "M263-B003-PROBE-SECOND-RESET-COUNT", "second reset registered image count must be zero"),
        (probe_payload.get("second_reset_next_expected_registration_order_ordinal") == 1, "M263-B003-PROBE-SECOND-RESET-ORDINAL", "second reset next ordinal must be 1"),
        (probe_payload.get("second_reset_retained_bootstrap_image_count") == 1, "M263-B003-PROBE-SECOND-RESET-RETAINED", "second reset retained image count mismatch"),
        (probe_payload.get("second_reset_last_reset_cleared_image_local_init_state_count", 0) >= 1, "M263-B003-PROBE-SECOND-RESET-CLEARED", "second reset must clear at least one image-local init cell"),
        (probe_payload.get("second_reset_reset_generation", 0) > probe_payload.get("post_reset_reset_generation", 0), "M263-B003-PROBE-SECOND-RESET-GEN", "second reset generation must advance again"),
        (probe_payload.get("second_restart_status") == 0, "M263-B003-PROBE-SECOND-RESTART-STATUS", "second restart replay must succeed"),
        (probe_payload.get("second_restart_registration_copy_status") == 0, "M263-B003-PROBE-SECOND-RESTART-REG", "second restart registration snapshot copy must succeed"),
        (probe_payload.get("second_restart_image_walk_copy_status") == 0, "M263-B003-PROBE-SECOND-RESTART-WALK", "second restart image-walk snapshot copy must succeed"),
        (probe_payload.get("second_restart_reset_replay_copy_status") == 0, "M263-B003-PROBE-SECOND-RESTART-RESET", "second restart reset snapshot copy must succeed"),
        (probe_payload.get("second_restart_registered_image_count") == 1, "M263-B003-PROBE-SECOND-RESTART-COUNT", "second restart must restore one image"),
        (probe_payload.get("second_restart_last_registration_status") == 0, "M263-B003-PROBE-SECOND-RESTART-LAST-STATUS", "second restart registration status mismatch"),
        (probe_payload.get("second_restart_last_replayed_image_count") == 1, "M263-B003-PROBE-SECOND-RESTART-REPLAYED", "second restart replayed image count mismatch"),
        (probe_payload.get("second_restart_replay_generation", 0) > probe_payload.get("first_restart_replay_generation", 0), "M263-B003-PROBE-SECOND-RESTART-GEN", "second restart replay generation must advance again"),
        (probe_payload.get("second_restart_last_registered_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-SECOND-RESTART-REG-ID", "second restart registered identity mismatch"),
        (probe_payload.get("second_restart_last_replayed_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-SECOND-RESTART-REPLAY-ID", "second restart replayed identity mismatch"),
        (probe_payload.get("second_restart_last_walked_translation_unit_identity_key") == expected_identity_key, "M263-B003-PROBE-SECOND-RESTART-WALK-ID", "second restart walked identity mismatch"),
    ):
        checks_total += 1
        require(condition, display_path(probe_exe), check_id, detail, failures)

    summary = {
        "fixture": display_path(fixture),
        "translation_unit_identity_key": expected_identity_key,
        "translation_unit_registration_order_ordinal": expected_ordinal,
        "registration_descriptor_identifier": registration_descriptor.get("registration_descriptor_identifier"),
        "image_root_identifier": registration_descriptor.get("image_root_identifier"),
        "registration_descriptor_identity_source": registration_descriptor.get("registration_descriptor_identity_source"),
        "image_root_identity_source": registration_descriptor.get("image_root_identity_source"),
    }
    summary.update(probe_payload)
    summary["probe_compile_command"] = probe_compile_command
    summary["probe_run_command"] = [str(probe_exe)]
    return checks_total, summary


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, Any], list[Finding], int]:
    failures: list[Finding] = []
    checks_total = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_checks, path_failures = check_static_contract(path, snippets)
        checks_total += path_checks
        failures.extend(path_failures)
        static_summary[display_path(path)] = {
            "checks": path_checks,
            "ok": not path_failures,
        }

    dynamic_summary: dict[str, object] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        if not NATIVE_EXE.exists():
            failures.append(Finding(display_path(NATIVE_EXE), "M263-B003-NATIVE-EXE", "native frontend executable is missing"))
        if not RUNTIME_LIBRARY.exists():
            failures.append(Finding(display_path(RUNTIME_LIBRARY), "M263-B003-RUNTIME-LIB", "runtime support library is missing"))
        clangxx = resolve_tool("clang++.exe", "clang++")
        if clangxx is None:
            failures.append(Finding("toolchain", "M263-B003-CLANGXX", "unable to resolve clang++"))
        if not failures and clangxx is not None:
            probe_specs = (
                (
                    "explicit",
                    EXPLICIT_FIXTURE,
                    PROBE_ROOT / "explicit",
                    "BootstrapFailureRestartDescriptor",
                    "BootstrapFailureRestartImageRoot",
                    "source-pragma",
                    "source-pragma",
                ),
                (
                    "default",
                    DEFAULT_FIXTURE,
                    PROBE_ROOT / "default",
                    "BootstrapFailureRestartDefault_registration_descriptor",
                    "BootstrapFailureRestartDefault_image_root",
                    "module-derived-default",
                    "module-derived-default",
                ),
            )
            for key, fixture, out_dir, reg_id, root_id, reg_source, root_source in probe_specs:
                probe_checks, probe_summary = compile_fixture(
                    fixture=fixture,
                    out_dir=out_dir,
                    expected_registration_descriptor=reg_id,
                    expected_image_root=root_id,
                    expected_registration_source=reg_source,
                    expected_image_root_source=root_source,
                    clangxx=clangxx,
                    failures=failures,
                )
                checks_total += probe_checks
                dynamic_summary[key] = probe_summary

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "static_checks": static_summary,
        "dynamic_probes": dynamic_summary,
        "dynamic_probes_executed": not skip_dynamic_probes,
        "next_implementation_issue": "M263-C001",
        "failures": [
            {
                "artifact": failure.artifact,
                "check_id": failure.check_id,
                "detail": failure.detail,
            }
            for failure in failures
        ],
    }
    return payload, failures, checks_total


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(tuple(argv or ()))
    payload, failures, _ = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.check_id} {failure.artifact}: {failure.detail}", file=sys.stderr)
        print(f"[error] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
