#!/usr/bin/env python3
"""Fail-closed checker for M265-E001 type-surface executable conformance gate."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m265-e001-type-surface-executable-conformance-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-type-surface-executable-conformance-gate/m265-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d003-summary-chain"
EXECUTION_GATE_MODEL = (
    "runnable-part3-type-surface-gate-consumes-frontend-sema-lowering-runtime-and-cross-module-proofs"
)
FAILURE_MODEL = "fail-closed-on-runnable-part3-type-surface-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M265-E002"

M265_A002_CONTRACT_ID = "objc3c-part3-type-source-closure/m265-a002-v1"
M265_B003_CONTRACT_ID = "objc3c-part3-type-semantic-model/m265-b001-v1"
M265_C003_LINEAGE_CONTRACT_ID = "objc3c-part3-optional-keypath-lowering/m265-c001-v1"
M265_C003_CONTRACT_ID = "objc3c-part3-typed-keypath-artifact-emission/m265-c003-v1"
M265_D003_CONTRACT_ID = "objc3c-part3-cross-module-type-surface-preservation/m265-d003-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_type_surface_executable_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_e001_type_surface_executable_conformance_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_binding_send_coalescing_keypath_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "e001"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-A002" / "frontend_support_optional_binding_send_coalescing_keypath_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-B003" / "generic_erasure_keypath_legality_completion_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-C003" / "typed_keypath_artifact_emission_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-D003" / "cross_module_type_surface_preservation_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m265" / "M265-E001" / "type_surface_executable_conformance_gate_summary.json"


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
        SnippetCheck("M265-E001-EXP-01", "# M265 Type-Surface Executable Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M265-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M265-E001-EXP-03", "tmp/reports/m265/M265-D003/cross_module_type_surface_preservation_summary.json"),
        SnippetCheck("M265-E001-EXP-04", "The gate must explicitly hand off to `M265-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M265-E001-PKT-01", "# M265-E001 Type-Surface Executable Conformance Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M265-E001-PKT-02", "Packet: `M265-E001`"),
        SnippetCheck("M265-E001-PKT-03", "Issue: `#7255`"),
        SnippetCheck("M265-E001-PKT-04", "- `M265-D003`"),
        SnippetCheck("M265-E001-PKT-05", "`M265-E002` is the explicit next issue"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M265-E001-SRC-01", "## M265 type-surface executable conformance gate (M265-E001)"),
        SnippetCheck("M265-E001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E001-SRC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M265-E001-SRC-04", "`M265-E002` is the next issue."),
    ),
    DOC_NATIVE: (
        SnippetCheck("M265-E001-NDOC-01", "## M265 type-surface executable conformance gate (M265-E001)"),
        SnippetCheck("M265-E001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M265-E001-NDOC-04", "`M265-E002` is the next issue."),
    ),
    SPEC_AM: (
        SnippetCheck("M265-E001-AM-01", "## M265 executable type-surface gate (E001)"),
        SnippetCheck("M265-E001-AM-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E001-AM-03", "optional bindings, optional sends, nil coalescing, typed key-path artifacts, and cross-module preservation"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M265-E001-ATTR-01", "## M265 executable type-surface gate (E001)"),
        SnippetCheck("M265-E001-ATTR-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M265-E001-ATTR-03", "`M265-E002` is the first issue allowed to broaden this gate into a runnable matrix."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M265-E001-ARCH-01", "## M265 Type-Surface Executable Conformance Gate (E001)"),
        SnippetCheck("M265-E001-ARCH-02", "`M265-E001` freezes the supported executable Part 3 slice for lane-E."),
        SnippetCheck("M265-E001-ARCH-03", "`M265-E002` is the next issue."),
    ),
    DRIVER_CPP: (
        SnippetCheck("M265-E001-DRV-01", "M265-E001 type-surface executable gate anchor"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M265-E001-MAN-01", "M265-E001 type-surface executable gate anchor"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M265-E001-FA-01", "M265-E001 type-surface executable gate anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M265-E001-PKG-01", '"check:objc3c:m265-e001-type-surface-executable-conformance-gate": "python scripts/check_m265_e001_type_surface_executable_conformance_gate_contract_and_architecture_freeze.py"'),
        SnippetCheck("M265-E001-PKG-02", '"test:tooling:m265-e001-type-surface-executable-conformance-gate": "python -m pytest tests/tooling/test_check_m265_e001_type_surface_executable_conformance_gate_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M265-E001-PKG-03", '"check:objc3c:m265-e001-lane-e-readiness": "python scripts/run_m265_e001_lane_e_readiness.py"'),
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


def ensure_native_build(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    summary_path = SUMMARY_OUT.parent / "ensure_objc3c_native_build_summary.json"
    completed = run_process(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m265-e001-type-surface-executable-conformance-gate",
            "--summary-out",
            str(summary_path),
        ]
    )
    payload: dict[str, Any] = {
        "returncode": completed.returncode,
        "summary_out": display_path(summary_path),
    }
    if summary_path.exists():
        payload["summary"] = load_json(summary_path)
    total = 1
    passed = require(completed.returncode == 0, display_path(BUILD_HELPER), "M265-E001-BUILD-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
    return total, passed, payload


def validate_summary_core(issue: str, path: Path, expected_contract: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    for check_id, condition, detail in (
        (f"{issue}-SUM-01", payload.get("checks_passed") == payload.get("checks_total"), "summary must report checks_passed == checks_total"),
        (f"{issue}-SUM-02", payload.get("checks_total", 0) > 0, "summary must report at least one check"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
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
        ("M265-A002-SUM-06", closure.get("unsupported_claim_ids") == [], "A002 must keep unsupported claim ids empty"),
        ("M265-A002-SUM-07", closure.get("optional_send_sites") == 1, "A002 must keep one optional send site"),
        ("M265-A002-SUM-08", closure.get("nil_coalescing_sites") == 1, "A002 must keep one nil coalescing site"),
        ("M265-A002-SUM-09", closure.get("typed_keypath_literal_sites") == 1, "A002 must keep one typed keypath site"),
        ("M265-A002-SUM-10", closure.get("deterministic_handoff") is True, "A002 must keep deterministic handoff"),
        ("M265-A002-SUM-11", closure.get("ready_for_semantic_expansion") is True, "A002 must remain ready for semantic expansion"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {
        "contract_id": payload.get("contract_id"),
        "optional_send_sites": closure.get("optional_send_sites"),
        "nil_coalescing_sites": closure.get("nil_coalescing_sites"),
        "typed_keypath_literal_sites": closure.get("typed_keypath_literal_sites"),
        "ready_for_semantic_expansion": closure.get("ready_for_semantic_expansion"),
    }


def validate_b003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-B003", path, M265_B003_CONTRACT_ID, failures)
    artifact = display_path(path)
    dynamic = payload.get("dynamic") if isinstance(payload.get("dynamic"), dict) else {}
    packet = dynamic.get("positive_class_root_packet") if isinstance(dynamic.get("positive_class_root_packet"), dict) else {}
    for check_id, condition, detail in (
        ("M265-B003-SUM-05", payload.get("dynamic_probes_executed") is True, "B003 must retain live semantic proof"),
        ("M265-B003-SUM-06", packet.get("typed_keypath_class_root_sites") == 1, "B003 must keep one class-root keypath site"),
        ("M265-B003-SUM-07", packet.get("ready_for_lowering_and_runtime") is True, "B003 must remain lowering/runtime ready"),
        (
            "M265-B003-SUM-08",
            all(int(packet.get(name, -1)) == 0 for name in (
                "typed_keypath_root_legality_violation_sites",
                "typed_keypath_member_path_contract_violation_sites",
                "typed_keypath_contract_violation_sites",
            )),
            "B003 must keep all key-path violation counters at zero",
        ),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {
        "contract_id": payload.get("contract_id"),
        "typed_keypath_class_root_sites": packet.get("typed_keypath_class_root_sites"),
        "ready_for_lowering_and_runtime": packet.get("ready_for_lowering_and_runtime"),
    }


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total, passed, payload = validate_summary_core("M265-C003", path, "", failures)
    artifact = display_path(path)
    evidence = payload.get("evidence") if isinstance(payload.get("evidence"), dict) else {}
    lowering_packet = evidence.get("lowering_packet") if isinstance(evidence.get("lowering_packet"), dict) else {}
    generic_packet = evidence.get("generic_packet") if isinstance(evidence.get("generic_packet"), dict) else {}
    execution = evidence.get("runtime_execution") if isinstance(evidence.get("runtime_execution"), dict) else {}
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
        "lineage_contract_id": M265_C003_LINEAGE_CONTRACT_ID,
        "live_typed_keypath_artifact_sites": lowering_packet.get("live_typed_keypath_artifact_sites"),
        "typed_keypath_artifact_ready": lowering_packet.get("ready_for_typed_keypath_artifact_emission"),
        "runtime_exit_code": evidence.get("linked_executable_exit_code"),
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
        ("M265-D003-SUM-07", link_plan.get("module_names_lexicographic") == ["runtimePackagingConsumer", "typedKeyPathRuntimeModule"], "D003 packaged module set drifted"),
        ("M265-D003-SUM-08", probe_payload.get("entry_found") == 1, "D003 probe must still find the keypath entry"),
        ("M265-D003-SUM-09", probe_payload.get("root_name") == "Person", "D003 probe root_name drifted"),
        ("M265-D003-SUM-10", probe_payload.get("component_path") == "name", "D003 probe component path drifted"),
        ("M265-D003-SUM-11", int(probe_payload.get("image_backed_keypath_count", 0)) >= 1, "D003 probe must keep at least one image-backed keypath"),
    ):
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, {
        "contract_id": payload.get("contract_id"),
        "module_names_lexicographic": link_plan.get("module_names_lexicographic"),
        "entry_found": probe_payload.get("entry_found"),
        "image_backed_keypath_count": probe_payload.get("image_backed_keypath_count"),
    }


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def validate_integrated_fixture(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "integrated"
    completed = compile_fixture(FIXTURE, out_dir)
    total = 0
    passed = 0
    artifact = display_path(FIXTURE)
    result: dict[str, Any] = {
        "fixture": artifact,
        "out_dir": display_path(out_dir),
        "compile_returncode": completed.returncode,
        "compile_stdout": completed.stdout,
        "compile_stderr": completed.stderr,
    }
    total += 1
    passed += require(completed.returncode == 0, artifact, "M265-E001-DYN-01", f"integrated fixture compile failed: {completed.stdout}{completed.stderr}", failures)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    for check_id, path_obj, detail in (
        ("M265-E001-DYN-02", manifest_path, "integrated fixture manifest missing"),
        ("M265-E001-DYN-03", ir_path, "integrated fixture IR missing"),
        ("M265-E001-DYN-04", obj_path, "integrated fixture object missing"),
        ("M265-E001-DYN-05", backend_path, "integrated fixture backend marker missing"),
    ):
        total += 1
        passed += require(path_obj.exists(), display_path(path_obj), check_id, detail, failures)
    if not (completed.returncode == 0 and manifest_path.exists() and backend_path.exists()):
        return total, passed, result
    manifest = load_json(manifest_path)
    result["module_name"] = manifest.get("module")
    backend = backend_path.read_text(encoding="utf-8").strip()
    result["backend"] = backend
    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    type_surface = semantic_surface.get("objc_part3_type_semantic_model", {}) if isinstance(semantic_surface, dict) else {}
    lowering_surface = semantic_surface.get("objc_part3_optional_keypath_lowering_contract", {}) if isinstance(semantic_surface, dict) else {}
    runtime_surface = semantic_surface.get("objc_part3_optional_keypath_runtime_helper_contract", {}) if isinstance(semantic_surface, dict) else {}
    result["type_surface"] = {
        "optional_binding_sites": type_surface.get("optional_binding_sites"),
        "optional_binding_clause_sites": type_surface.get("optional_binding_clause_sites"),
        "optional_send_sites": type_surface.get("optional_send_sites"),
        "nil_coalescing_sites": type_surface.get("nil_coalescing_sites"),
        "typed_keypath_literal_sites": type_surface.get("typed_keypath_literal_sites"),
    }
    result["lowering_surface"] = {
        "optional_send_sites": lowering_surface.get("optional_send_sites"),
        "live_optional_lowering_sites": lowering_surface.get("live_optional_lowering_sites"),
        "typed_keypath_literal_sites": lowering_surface.get("typed_keypath_literal_sites"),
        "ready_for_native_optional_lowering": lowering_surface.get("ready_for_native_optional_lowering"),
        "ready_for_typed_keypath_artifact_emission": lowering_surface.get("ready_for_typed_keypath_artifact_emission"),
    }
    total += 1
    passed += require(manifest.get("module") == "optionalsFrontend", display_path(manifest_path), "M265-E001-DYN-06", "unexpected integrated fixture module name", failures)
    total += 1
    passed += require(backend == "llvm-direct", display_path(backend_path), "M265-E001-DYN-07", f"expected llvm-direct backend, got {backend!r}", failures)
    for check_id, actual, expected, label in (
        ("M265-E001-DYN-08", type_surface.get("optional_binding_sites"), 2, "optional_binding_sites"),
        ("M265-E001-DYN-09", type_surface.get("optional_binding_clause_sites"), 2, "optional_binding_clause_sites"),
        ("M265-E001-DYN-10", type_surface.get("optional_send_sites"), 1, "optional_send_sites"),
        ("M265-E001-DYN-11", type_surface.get("nil_coalescing_sites"), 1, "nil_coalescing_sites"),
        ("M265-E001-DYN-12", type_surface.get("typed_keypath_literal_sites"), 1, "typed_keypath_literal_sites"),
        ("M265-E001-DYN-13", lowering_surface.get("live_optional_lowering_sites"), 4, "live_optional_lowering_sites"),
        ("M265-E001-DYN-14", lowering_surface.get("typed_keypath_literal_sites"), 1, "lowering typed_keypath_literal_sites"),
    ):
        total += 1
        passed += require(actual == expected, display_path(manifest_path), check_id, f"unexpected integrated fixture {label}: expected {expected!r}, got {actual!r}", failures)
    for check_id, actual, label in (
        ("M265-E001-DYN-15", lowering_surface.get("ready_for_native_optional_lowering") is True, "ready_for_native_optional_lowering"),
        ("M265-E001-DYN-16", lowering_surface.get("ready_for_typed_keypath_artifact_emission") is True, "ready_for_typed_keypath_artifact_emission"),
        ("M265-E001-DYN-17", runtime_surface.get("optional_send_runtime_ready") is True, "optional_send_runtime_ready"),
        ("M265-E001-DYN-18", runtime_surface.get("typed_keypath_runtime_execution_helper_landed") is True, "typed_keypath_runtime_execution_helper_landed"),
    ):
        total += 1
        passed += require(actual, display_path(manifest_path), check_id, f"integrated fixture runtime/lowering flag drifted: {label}", failures)
    return total, passed, result


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
    ):
        total, passed, distilled = validator(path, failures)
        checks_total += total
        checks_passed += passed
        upstream_summaries[label] = distilled

    if build_case.get("returncode") == 0:
        total, passed, integrated = validate_integrated_fixture(failures)
        checks_total += total
        checks_passed += passed
        dynamic["integrated_fixture"] = integrated

    summary = {
        "ok": not failures,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "execution_gate_model": EXECUTION_GATE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
        "upstream_summaries": upstream_summaries,
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"[fail] M265-E001 type-surface gate failed ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        return 1

    print(f"[ok] M265-E001 type-surface gate validated ({checks_passed}/{checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
