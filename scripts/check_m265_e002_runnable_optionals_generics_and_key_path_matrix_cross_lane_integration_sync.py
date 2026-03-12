#!/usr/bin/env python3
"""Validate M265-E002 runnable Part 3 closeout evidence."""

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
MODE = "m265-e002-runnable-type-surface-closeout-v1"
CONTRACT_ID = "objc3c-runnable-type-surface-closeout/m265-e002-v1"
MATRIX_MODEL = (
    "closeout-matrix-consumes-a002-b003-c003-d003-and-e001-evidence-without-widening-the-supported-runnable-part3-slice"
)
SMOKE_MODEL = (
    "integrated-optional-send-binding-refinement-optional-member-access-and-validated-typed-keypath-runtime-rows-prove-the-supported-part3-slice-while-generics-stay-metadata-backed"
)
FAILURE_MODEL = "fail-closed-on-runnable-part3-closeout-drift-or-doc-mismatch"
NEXT_ISSUE = "M266-A001"

M265_A002_CONTRACT_ID = "objc3c-part3-type-source-closure/m265-a002-v1"
M265_B003_CONTRACT_ID = "objc3c-part3-type-semantic-model/m265-b001-v1"
M265_C003_CONTRACT_ID = "objc3c-part3-typed-keypath-artifact-emission/m265-c003-v1"
M265_D003_CONTRACT_ID = "objc3c-part3-cross-module-type-surface-preservation/m265-d003-v1"
M265_E001_CONTRACT_ID = "objc3c-type-surface-executable-conformance-gate/m265-e001-v1"
RUNTIME_HELPER_CONTRACT_ID = "objc3c-part3-optional-keypath-runtime-helper-contract/m265-d001-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_e002_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"

A002_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-A002" / "frontend_support_optional_binding_send_coalescing_keypath_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-B003" / "generic_erasure_keypath_legality_completion_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-C003" / "typed_keypath_artifact_emission_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-D003" / "cross_module_type_surface_preservation_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-E001" / "type_surface_executable_conformance_gate_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m265" / "M265-E002" / "runnable_type_surface_closeout_summary.json"

FIXTURE_ROWS = (
    ("optional-send-short-circuit", ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_send_argument_short_circuit_positive.objc3", 0),
    ("optional-binding-refinement", ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_flow_binding_refinement_positive.objc3", 36),
    ("optional-member-access", ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_runtime_positive.objc3", 9),
    ("typed-keypath-runtime", ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_runtime_positive.objc3", 11),
)
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "e002"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M265-E002-EXP-01", "# M265 Runnable Optionals, Generics, And Key-Path Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M265-E002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M265-E002-EXP-03", "preserved generic metadata and replay evidence"),
        SnippetCheck("M265-E002-EXP-04", "The closeout must explicitly hand off to `M266-A001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M265-E002-PKT-01", "# M265-E002 Runnable Optionals, Generics, And Key-Path Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M265-E002-PKT-02", "Packet: `M265-E002`"),
        SnippetCheck("M265-E002-PKT-03", "Issue: `#7256`"),
        SnippetCheck("M265-E002-PKT-04", "- generic metadata/replay preservation row"),
        SnippetCheck("M265-E002-PKT-05", "`M266-A001` is the next issue after `M265` closeout."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M265-E002-SRC-01", "## M265 runnable optionals, generics, and key-path matrix (M265-E002)"),
        SnippetCheck("M265-E002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E002-SRC-03", "pragmatic generic annotations remain erased at runtime"),
        SnippetCheck("M265-E002-SRC-04", "`M266-A001` is the next issue after `M265` closeout"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M265-E002-NDOC-01", "## M265 runnable optionals, generics, and key-path matrix (M265-E002)"),
        SnippetCheck("M265-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E002-NDOC-03", "pragmatic generic annotations remain erased at runtime"),
        SnippetCheck("M265-E002-NDOC-04", "`M266-A001` is the next issue after `M265` closeout"),
    ),
    SPEC_AM: (
        SnippetCheck("M265-E002-AM-01", "## M265 runnable type-surface closeout matrix (E002)"),
        SnippetCheck("M265-E002-AM-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E002-AM-03", "Pragmatic generic annotations remain represented through preserved metadata and replay evidence"),
        SnippetCheck("M265-E002-AM-04", "`M266-A001` is the\nnext issue after this closeout."),
    ),
    SPEC_ATTR: (
        SnippetCheck("M265-E002-ATTR-01", "## M265 runnable type-surface closeout matrix (E002)"),
        SnippetCheck("M265-E002-ATTR-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E002-ATTR-03", "`M265-E002` closes only the currently supported Part 3 slice."),
        SnippetCheck("M265-E002-ATTR-04", "`M266-A001` is the next issue after `M265` closeout."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M265-E002-ARCH-01", "## M265 Runnable Type-Surface Closeout Matrix (E002)"),
        SnippetCheck("M265-E002-ARCH-02", "runtime rows prove optional-send short-circuiting, optional binding/refinement,"),
        SnippetCheck("M265-E002-ARCH-03", "the next issue is `M266-A001`"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M265-E002-DRV-01", "M265-E002 runnable-type-surface closeout anchor"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M265-E002-MAN-01", "M265-E002 runnable-type-surface closeout anchor"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M265-E002-FA-01", "M265-E002 runnable-type-surface closeout anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M265-E002-PKG-01", '"check:objc3c:m265-e002-runnable-optionals-generics-and-key-path-matrix": "python scripts/check_m265_e002_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync.py"'),
        SnippetCheck("M265-E002-PKG-02", '"test:tooling:m265-e002-runnable-optionals-generics-and-key-path-matrix": "python -m pytest tests/tooling/test_check_m265_e002_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync.py -q"'),
        SnippetCheck("M265-E002-PKG-03", '"check:objc3c:m265-e002-lane-e-readiness": "python scripts/run_m265_e002_lane_e_readiness.py"'),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, int, list[Finding]]:
    failures: list[Finding] = []
    total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return total, 0, failures
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return total, passed, failures


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def resolve_clang() -> str:
    for candidate in (shutil.which("clang.exe"), shutil.which("clang"), r"C:\Program Files\LLVM\bin\clang.exe"):
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang"


def validate_summary_core(issue: str, path: Path, expected_contract: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{issue}-SUM-01", "checks_passed must equal checks_total", failures)
    total += 1
    passed += require(payload.get("checks_total", 0) > 0, artifact, f"{issue}-SUM-02", "summary must report at least one check", failures)
    if "ok" in payload:
        total += 1
        passed += require(payload.get("ok") is True, artifact, f"{issue}-SUM-03", "summary must report ok=true", failures)
    if expected_contract:
        total += 1
        passed += require(payload.get("contract_id") == expected_contract, artifact, f"{issue}-SUM-04", f"expected contract_id {expected_contract!r}", failures)
    return total, passed, payload


def validate_a002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-A002", path, M265_A002_CONTRACT_ID, failures)
    artifact = display_path(path)
    dynamic = payload.get("dynamic") if isinstance(payload.get("dynamic"), dict) else {}
    closure = dynamic.get("part3_type_source_closure") if isinstance(dynamic.get("part3_type_source_closure"), dict) else {}
    for check_id, condition, detail in (
        ("M265-A002-SUM-05", payload.get("dynamic_probes_executed") is True, "A002 must retain live frontend proof"),
        ("M265-A002-SUM-06", closure.get("unsupported_claim_ids") == [], "A002 unsupported claims must stay empty"),
        ("M265-A002-SUM-07", closure.get("optional_send_source_supported") is True, "A002 must keep optional-send source support"),
        ("M265-A002-SUM-08", closure.get("typed_keypath_literal_source_supported") is True, "A002 must keep typed keypath source support"),
        ("M265-A002-SUM-09", closure.get("ready_for_semantic_expansion") is True, "A002 must remain ready for semantic expansion"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {"contract_id": payload.get("contract_id")}


def validate_b003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-B003", path, M265_B003_CONTRACT_ID, failures)
    artifact = display_path(path)
    dynamic = payload.get("dynamic") if isinstance(payload.get("dynamic"), dict) else {}
    packet = dynamic.get("positive_class_root_packet") if isinstance(dynamic.get("positive_class_root_packet"), dict) else {}
    for check_id, condition, detail in (
        ("M265-B003-SUM-05", payload.get("dynamic_probes_executed") is True, "B003 must retain live semantic proof"),
        ("M265-B003-SUM-06", packet.get("typed_keypath_class_root_sites") == 1, "B003 must keep one class-root keypath site"),
        ("M265-B003-SUM-07", packet.get("ready_for_lowering_and_runtime") is True, "B003 must remain lowering/runtime ready"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {"contract_id": payload.get("contract_id")}


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-C003", path, "", failures)
    artifact = display_path(path)
    evidence = payload.get("evidence") if isinstance(payload.get("evidence"), dict) else {}
    lowering_packet = evidence.get("lowering_packet") if isinstance(evidence.get("lowering_packet"), dict) else {}
    generic_packet = evidence.get("generic_packet") if isinstance(evidence.get("generic_packet"), dict) else {}
    for check_id, condition, detail in (
        ("M265-C003-SUM-05", payload.get("dynamic_probes_executed") is True, "C003 must retain live lowering/runtime proof"),
        ("M265-C003-SUM-06", lowering_packet.get("live_typed_keypath_artifact_sites") == 1, "C003 must keep one live typed keypath artifact site"),
        ("M265-C003-SUM-07", lowering_packet.get("ready_for_typed_keypath_artifact_emission") is True, "C003 lowering packet must stay ready"),
        ("M265-C003-SUM-08", generic_packet.get("generic_metadata_abi_sites") == 1, "C003 generic packet must keep one ABI site"),
        ("M265-C003-SUM-09", evidence.get("linked_executable_exit_code") == 7, "C003 executable runtime proof must keep exit code 7"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {
        "contract_id": M265_C003_CONTRACT_ID,
        "generic_metadata_abi_sites": generic_packet.get("generic_metadata_abi_sites"),
        "linked_executable_exit_code": evidence.get("linked_executable_exit_code"),
    }


def validate_d003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-D003", path, M265_D003_CONTRACT_ID, failures)
    artifact = display_path(path)
    evidence = payload.get("evidence") if isinstance(payload.get("evidence"), dict) else {}
    consumer = evidence.get("consumer") if isinstance(evidence.get("consumer"), dict) else {}
    link_plan = consumer.get("link_plan") if isinstance(consumer.get("link_plan"), dict) else {}
    probe = evidence.get("probe") if isinstance(evidence.get("probe"), dict) else {}
    probe_payload = probe.get("probe") if isinstance(probe.get("probe"), dict) else {}
    for check_id, condition, detail in (
        ("M265-D003-SUM-05", payload.get("dynamic_probes_executed") is True, "D003 must retain live cross-module proof"),
        ("M265-D003-SUM-06", link_plan.get("module_image_count") == 2, "D003 must keep two images in the link plan"),
        ("M265-D003-SUM-07", probe_payload.get("generic_metadata_replay_key_present") == 1, "D003 probe must preserve generic metadata replay evidence"),
        ("M265-D003-SUM-08", probe_payload.get("entry_found") == 1, "D003 probe must still find the keypath entry"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {"contract_id": payload.get("contract_id")}


def validate_e001(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-E001", path, M265_E001_CONTRACT_ID, failures)
    artifact = display_path(path)
    dynamic = payload.get("dynamic") if isinstance(payload.get("dynamic"), dict) else {}
    integrated = dynamic.get("integrated_fixture") if isinstance(dynamic.get("integrated_fixture"), dict) else {}
    for check_id, condition, detail in (
        ("M265-E001-SUM-05", integrated.get("backend") == "llvm-direct", "E001 integrated fixture must stay on llvm-direct"),
        ("M265-E001-SUM-06", integrated.get("module_name") == "optionalsFrontend", "E001 integrated fixture module name drifted"),
        ("M265-E001-SUM-07", payload.get("next_closeout_issue") == "M265-E002", "E001 must continue to hand off to M265-E002"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {"contract_id": payload.get("contract_id")}


def ensure_native_build(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    summary_path = SUMMARY_OUT.parent / "ensure_objc3c_native_build_summary.json"
    completed = run_process([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m265-e002-runnable-type-surface-closeout",
        "--summary-out",
        str(summary_path),
    ])
    payload: dict[str, Any] = {"returncode": completed.returncode, "summary_out": display_path(summary_path)}
    if summary_path.exists():
        payload["summary"] = load_json(summary_path)
    total = 1
    passed = require(completed.returncode == 0, display_path(BUILD_HELPER), "M265-E002-BUILD-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
    return total, passed, payload


def load_runtime_library_path(registration_manifest_path: Path) -> Path | None:
    registration_manifest = load_json(registration_manifest_path)
    relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(relative_path, str) or not relative_path.strip():
        return None
    candidate = (ROOT / relative_path).resolve()
    return candidate if candidate.exists() else None


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    runtime_library_path = load_runtime_library_path(registration_manifest_path)
    if runtime_library_path is None:
        return None, None
    result = run_process([
        resolve_clang(),
        str(obj_path),
        str(runtime_library_path),
        f"@{rsp_path.resolve()}",
        "-o",
        str(exe_path),
    ], cwd=out_dir)
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def compile_and_run_row(name: str, fixture: Path, expected_exit_code: int, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    out_dir = PROBE_ROOT / name
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])
    artifact = display_path(fixture)
    evidence: dict[str, Any] = {"fixture": artifact, "out_dir": display_path(out_dir), "compile_returncode": compile_result.returncode}
    total += 1
    passed += require(compile_result.returncode == 0, artifact, f"M265-E002-{name}-compile", f"compile failed: {compile_result.stdout}{compile_result.stderr}", failures)
    manifest_path = out_dir / "module.manifest.json"
    obj_path = out_dir / "module.obj"
    ir_path = out_dir / "module.ll"
    backend_path = out_dir / "module.object-backend.txt"
    registration_path = out_dir / "module.runtime-registration-manifest.json"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    for check_id, path_obj, detail in (
        (f"M265-E002-{name}-manifest", manifest_path, "manifest missing"),
        (f"M265-E002-{name}-obj", obj_path, "object missing"),
        (f"M265-E002-{name}-ir", ir_path, "IR missing"),
        (f"M265-E002-{name}-backend", backend_path, "backend marker missing"),
        (f"M265-E002-{name}-registration", registration_path, "registration manifest missing"),
        (f"M265-E002-{name}-rsp", rsp_path, "linker response missing"),
    ):
        total += 1
        passed += require(path_obj.exists(), display_path(path_obj), check_id, detail, failures)
    if not (manifest_path.exists() and backend_path.exists() and compile_result.returncode == 0):
        return total, passed, evidence

    manifest = load_json(manifest_path)
    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    type_surface = semantic_surface.get("objc_part3_type_semantic_model", {}) if isinstance(semantic_surface, dict) else {}
    lowering_surface = semantic_surface.get("objc_part3_optional_keypath_lowering_contract", {}) if isinstance(semantic_surface, dict) else {}
    runtime_surface = semantic_surface.get("objc_part3_optional_keypath_runtime_helper_contract", {}) if isinstance(semantic_surface, dict) else {}
    backend = backend_path.read_text(encoding="utf-8").strip()
    total += 1
    passed += require(backend == "llvm-direct", display_path(backend_path), f"M265-E002-{name}-backend-llvm", f"expected llvm-direct backend, got {backend!r}", failures)
    total += 1
    passed += require(runtime_surface.get("contract_id") == RUNTIME_HELPER_CONTRACT_ID, display_path(manifest_path), f"M265-E002-{name}-runtime-packet", "runtime helper packet contract drifted", failures)

    if name == "optional-send-short-circuit":
        total += 2
        passed += require(int(lowering_surface.get("optional_send_sites", 0)) >= 1, display_path(manifest_path), "M265-E002-opt-send-sites", "expected at least one optional send site", failures)
        passed += require(runtime_surface.get("optional_send_runtime_ready") is True, display_path(manifest_path), "M265-E002-opt-send-runtime", "optional send runtime readiness drifted", failures)
    elif name == "optional-binding-refinement":
        total += 3
        passed += require(int(type_surface.get("optional_binding_sites", 0)) >= 1, display_path(manifest_path), "M265-E002-opt-bind-sites", "expected optional binding sites", failures)
        passed += require(int(type_surface.get("optional_flow_refinement_sites", 0)) >= 1, display_path(manifest_path), "M265-E002-opt-flow-sites", "expected optional flow refinement sites", failures)
        passed += require(int(type_surface.get("nil_coalescing_sites", 0)) >= 1, display_path(manifest_path), "M265-E002-opt-coalesce-sites", "expected nil coalescing sites", failures)
    elif name == "optional-member-access":
        total += 2
        passed += require(int(type_surface.get("optional_send_sites", 0)) >= 1, display_path(manifest_path), "M265-E002-opt-member-sites", "expected lowered optional-send sites for optional-member access", failures)
        passed += require(int(lowering_surface.get("live_optional_lowering_sites", 0)) >= 1, display_path(manifest_path), "M265-E002-opt-member-lowering", "expected live optional lowering sites", failures)
    elif name == "typed-keypath-runtime":
        total += 3
        passed += require(int(lowering_surface.get("typed_keypath_literal_sites", 0)) == 1, display_path(manifest_path), "M265-E002-keypath-sites", "expected one typed keypath literal site", failures)
        passed += require(runtime_surface.get("typed_keypath_descriptor_handles_ready") is True, display_path(manifest_path), "M265-E002-keypath-handles", "typed keypath descriptor handles readiness drifted", failures)
        passed += require(runtime_surface.get("typed_keypath_runtime_execution_helper_landed") is True, display_path(manifest_path), "M265-E002-keypath-runtime", "typed keypath runtime helper drifted", failures)

    exe_path, link_result = link_executable(out_dir)
    total += 1
    passed += require(exe_path is not None, display_path(out_dir), f"M265-E002-{name}-link", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}", failures)
    if exe_path is not None:
        run_result = run_process([str(exe_path)], cwd=out_dir)
        total += 1
        passed += require(run_result.returncode == expected_exit_code, display_path(exe_path), f"M265-E002-{name}-run", f"expected exit {expected_exit_code}, got {run_result.returncode}", failures)
        evidence["run_exit"] = run_result.returncode
    evidence["backend"] = backend
    evidence["module"] = manifest.get("module")
    evidence["expected_exit"] = expected_exit_code
    return total, passed, evidence


def main(argv: Sequence[str] | None = None) -> int:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic: dict[str, Any] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        total, passed, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += passed
        failures.extend(static_failures)

    total, passed, build_case = ensure_native_build(failures)
    checks_total += total
    checks_passed += passed
    dynamic["build"] = build_case

    upstream_summaries: dict[str, Any] = {}
    for label, validator, path in (
        ("M265-A002", validate_a002, A002_SUMMARY),
        ("M265-B003", validate_b003, B003_SUMMARY),
        ("M265-C003", validate_c003, C003_SUMMARY),
        ("M265-D003", validate_d003, D003_SUMMARY),
        ("M265-E001", validate_e001, E001_SUMMARY),
    ):
        total, passed, distilled = validator(path, failures)
        checks_total += total
        checks_passed += passed
        upstream_summaries[label] = distilled

    runtime_rows: list[dict[str, Any]] = []
    if build_case.get("returncode") == 0:
        for name, fixture, expected_exit in FIXTURE_ROWS:
            total, passed, row = compile_and_run_row(name, fixture, expected_exit, failures)
            checks_total += total
            checks_passed += passed
            runtime_rows.append({"row": name, **row})

    checks_total += 3
    c003 = upstream_summaries.get("M265-C003", {})
    d003 = load_json(D003_SUMMARY)
    d003_probe = d003.get("evidence", {}).get("probe", {}).get("probe", {}) if isinstance(d003.get("evidence"), dict) else {}
    checks_passed += require(c003.get("generic_metadata_abi_sites") == 1, display_path(C003_SUMMARY), "M265-E002-generic-row-01", "C003 must keep one generic metadata ABI site", failures)
    checks_passed += require(d003_probe.get("generic_metadata_replay_key_present") == 1, display_path(D003_SUMMARY), "M265-E002-generic-row-02", "D003 must preserve generic metadata replay key", failures)
    checks_passed += require(d003_probe.get("entry_metadata_provider_count") == 1, display_path(D003_SUMMARY), "M265-E002-generic-row-03", "D003 must keep one metadata provider", failures)

    summary = {
        "ok": not failures,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "matrix_model": MATRIX_MODEL,
        "smoke_model": SMOKE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runtime_rows": runtime_rows,
        "generic_preservation_row": {
            "status": "ok" if not any(f.check_id.startswith("M265-E002-generic-row") for f in failures) else "fail",
            "generic_metadata_abi_sites": c003.get("generic_metadata_abi_sites"),
            "generic_metadata_replay_key_present": d003_probe.get("generic_metadata_replay_key_present"),
            "entry_metadata_provider_count": d003_probe.get("entry_metadata_provider_count"),
        },
        "upstream_summaries": upstream_summaries,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"[fail] M265-E002 runnable type-surface closeout failed ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        return 1

    print(f"[ok] M265-E002 runnable type-surface closeout validated ({checks_passed}/{checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
