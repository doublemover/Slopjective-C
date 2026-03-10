#!/usr/bin/env python3
"""Validate M258-E002 runnable import/module execution matrix."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-e002-runnable-import-module-execution-matrix-plus-docs-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-runnable-import-module-execution-matrix/m258-e002-v1"
EVIDENCE_MODEL = "a002-b002-c002-d002-e001-summary-chain-plus-live-cross-module-runtime-execution"
EXECUTION_MATRIX_MODEL = (
    "runnable-import-module-matrix-composes-upstream-summaries-with-live-two-image-startup-dispatch-selector-cache-and-replay-proof"
)
FAILURE_MODEL = "fail-closed-on-runnable-import-module-execution-matrix-drift-or-missing-live-runtime-proof"
NEXT_ISSUE = "M259-A001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-E002" / "runnable_import_module_execution_matrix_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
CLANGXX_CANDIDATES = ("clang++", "clang++-21")
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "e002-runnable-import-module-matrix"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_consumer.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m258_e002_import_module_execution_matrix_probe.cpp"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-A002" / "runtime_aware_import_module_frontend_closure_summary.json"
B002_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-B002" / "imported_runtime_metadata_semantic_rules_summary.json"
C002_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-C002" / "module_metadata_artifact_reuse_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-D002" / "cross_module_runtime_packaging_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-E001" / "cross_module_object_model_gate_summary.json"

A002_CONTRACT_ID = "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"
B002_CONTRACT_ID = "objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1"
C002_CONTRACT_ID = "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1"
D002_CONTRACT_ID = "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"
E001_CONTRACT_ID = "objc3c-cross-module-object-model-gate/m258-e001-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_e002_lane_e_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py"
PROBE_SOURCE = ROOT / "tests" / "tooling" / "runtime" / "m258_e002_import_module_execution_matrix_probe.cpp"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
DISCOVERY_ARTIFACT = "module.runtime-metadata-discovery.json"
RUNTIME_LINKER_RESPONSE_ARTIFACT = "module.runtime-metadata-linker-options.rsp"
OBJECT_ARTIFACT = "module.obj"
BACKEND_ARTIFACT = "module.object-backend.txt"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"
LINKER_RESPONSE_ARTIFACT = "module.cross-module-runtime-linker-options.rsp"
EXPECTED_MODULES = ["runtimePackagingConsumer", "runtimePackagingProvider"]


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
        SnippetCheck("M258-E002-DOC-EXP-01", "# M258 Runnable Import And Module Execution Matrix Plus Docs Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M258-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-E002-DOC-EXP-03", "method-cache snapshots preserve the current truthful fallback-dispatch"),
        SnippetCheck("M258-E002-DOC-EXP-04", "The matrix must explicitly hand off to `M259-A001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-E002-DOC-PKT-01", "# M258-E002 Runnable Import And Module Execution Matrix Plus Docs Cross-Lane Integration Sync Packet"),
        SnippetCheck("M258-E002-DOC-PKT-02", "Packet: `M258-E002`"),
        SnippetCheck("M258-E002-DOC-PKT-03", "Issue: `#7167`"),
        SnippetCheck("M258-E002-DOC-PKT-04", "- `M258-E001`"),
        SnippetCheck("M258-E002-DOC-PKT-05", "Next issue: `M259-A001`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-E002-NDOC-01", "## Runnable import and module execution matrix (M258-E002)"),
        SnippetCheck("M258-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-E002-NDOC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M258-E002-NDOC-04", "tmp/reports/m258/M258-E002/runnable_import_module_execution_matrix_summary.json"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-E002-SPC-01", "## M258 runnable import/module execution matrix (E002)"),
        SnippetCheck("M258-E002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-E002-SPC-03", f"`{EXECUTION_MATRIX_MODEL}`"),
        SnippetCheck("M258-E002-SPC-04", "`M259-A001`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-E002-META-01", "## M258 runnable import/module execution matrix metadata anchors (E002)"),
        SnippetCheck("M258-E002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-E002-META-03", "`tmp/reports/m258/M258-E002/runnable_import_module_execution_matrix_summary.json`"),
        SnippetCheck("M258-E002-META-04", "`tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-E002-ARCH-01", "## M258 runnable import/module execution matrix (E002)"),
        SnippetCheck("M258-E002-ARCH-02", "`M258-E002` broadens the frozen `M258-E001` gate into one live cross-module"),
        SnippetCheck("M258-E002-ARCH-03", "the next implementation issue is `M259-A001`"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-E002-FRONT-01", "M258-E002 runnable import/module execution-matrix anchor:"),
        SnippetCheck("M258-E002-FRONT-02", "same emitted frontend surface remains the canonical replay"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-E002-IR-01", "M258-E002 runnable import/module execution-matrix anchor:"),
        SnippetCheck("M258-E002-IR-02", "emitter still stays object-local while lane-E proves the integrated"),
    ),
    API_H: (
        SnippetCheck("M258-E002-API-01", "M258-E002 runnable import/module execution-matrix anchor:"),
        SnippetCheck("M258-E002-API-02", "filesystem artifacts, but this public C ABI still exposes no in-memory"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-E002-PKG-01", '"check:objc3c:m258-e002-runnable-import-and-module-execution-matrix-plus-docs"'),
        SnippetCheck("M258-E002-PKG-02", '"test:tooling:m258-e002-runnable-import-and-module-execution-matrix-plus-docs"'),
        SnippetCheck("M258-E002-PKG-03", '"check:objc3c:m258-e002-lane-e-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-E002-RUN-01", "check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py"),
        SnippetCheck("M258-E002-RUN-02", "check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-E002-TEST-01", "def test_checker_passes_static(tmp_path: Path) -> None:"),
        SnippetCheck("M258-E002-TEST-02", CONTRACT_ID),
    ),
    PROBE_SOURCE: (
        SnippetCheck("M258-E002-PRB-01", 'objc3_runtime_copy_selector_lookup_table_state_for_testing('),
        SnippetCheck("M258-E002-PRB-02", 'objc3_runtime_copy_method_cache_entry_for_testing('),
        SnippetCheck("M258-E002-PRB-03", '"post_replay_local_consumer_class_value"'),
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


def normalize_path_text(path: Path | str) -> str:
    return str(Path(path).resolve()).replace("\\", "/")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd),
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


def resolve_tool(candidates: Sequence[str]) -> str | None:
    for candidate in candidates:
        direct = shutil.which(candidate)
        if direct:
            return direct
        if sys.platform == "win32":
            llvm_bin = Path("C:/Program Files/LLVM/bin")
            candidate_path = llvm_bin / candidate
            if candidate_path.exists():
                return str(candidate_path)
            if not candidate.endswith(".exe"):
                exe_candidate = llvm_bin / f"{candidate}.exe"
                if exe_candidate.exists():
                    return str(exe_candidate)
    return None


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def ensure_binaries(failures: list[Finding]) -> int:
    checks_total = 0
    if NATIVE_EXE.exists() and RUNTIME_LIBRARY.exists():
        checks_total += require(True, display_path(NATIVE_EXE), "M258-E002-BIN-READY", "native binary present", failures)
        checks_total += require(True, display_path(RUNTIME_LIBRARY), "M258-E002-LIB-READY", "runtime library present", failures)
        return checks_total
    build = run_command([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    output = (build.stdout or "") + (build.stderr or "")
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-E002-BUILD", output.strip() or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-E002-NATIVE-EXISTS", "native binary missing after build", failures)
    checks_total += require(RUNTIME_LIBRARY.exists(), display_path(RUNTIME_LIBRARY), "M258-E002-LIB-EXISTS", "runtime library missing after build", failures)
    return checks_total


def compile_fixture(*, fixture: Path, out_dir: Path, registration_order_ordinal: int, import_surface: Path | None = None) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        str(registration_order_ordinal),
    ]
    if import_surface is not None:
        command.extend(["--objc3-import-runtime-surface", str(import_surface)])
    return run_command(command)


def parse_response_lines(path: Path) -> list[str]:
    return [line.strip() for line in read_text(path).splitlines() if line.strip()]


def validate_a002(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(A002_SUMMARY)
    checks_total = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    native_class = dynamic.get("native_class") if isinstance(dynamic, dict) else {}
    native_category = dynamic.get("native_category") if isinstance(dynamic, dict) else {}
    checks_total += require(payload.get("ok") is True, artifact, "M258-E002-A002-01", "A002 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == A002_CONTRACT_ID, artifact, "M258-E002-A002-02", "A002 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E002-A002-03", "A002 summary must report full coverage", failures)
    checks_total += require(dynamic.get("parity") is True, artifact, "M258-E002-A002-04", "A002 parity proof drifted", failures)
    checks_total += require(native_class.get("module_name") == "runtimeMetadataClassRecords", artifact, "M258-E002-A002-05", "A002 native class module drifted", failures)
    checks_total += require(native_category.get("module_name") == "runtimeMetadataCategoryRecords", artifact, "M258-E002-A002-06", "A002 native category module drifted", failures)
    return checks_total, {
        "contract_id": payload.get("contract_id"),
        "parity": dynamic.get("parity"),
        "native_class_module": native_class.get("module_name"),
        "native_category_module": native_category.get("module_name"),
    }


def validate_b002(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(B002_SUMMARY)
    checks_total = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    consumer = dynamic.get("consumer_surface") if isinstance(dynamic, dict) else {}
    duplicate = dynamic.get("duplicate_path_failure") if isinstance(dynamic, dict) else {}
    checks_total += require(payload.get("ok") is True, artifact, "M258-E002-B002-01", "B002 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == B002_CONTRACT_ID, artifact, "M258-E002-B002-02", "B002 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E002-B002-03", "B002 summary must report full coverage", failures)
    checks_total += require(consumer.get("ready") is True, artifact, "M258-E002-B002-04", "B002 consumer surface must remain ready", failures)
    checks_total += require(consumer.get("ready_for_cross_module_dispatch_equivalence") is True, artifact, "M258-E002-B002-05", "B002 dispatch-equivalence readiness drifted", failures)
    checks_total += require(consumer.get("imported_module_names_lexicographic") == ["runtimeMetadataCategoryRecords", "runtimeMetadataClassRecords"], artifact, "M258-E002-B002-06", "B002 imported module inventory drifted", failures)
    checks_total += require(int(duplicate.get("returncode", 0)) != 0 and duplicate.get("diagnostic_contains_o3s264") is True, artifact, "M258-E002-B002-07", "B002 duplicate-path fail-closed proof drifted", failures)
    return checks_total, {
        "contract_id": payload.get("contract_id"),
        "consumer_ready": consumer.get("ready"),
        "dispatch_equivalence_ready": consumer.get("ready_for_cross_module_dispatch_equivalence"),
        "imported_modules": consumer.get("imported_module_names_lexicographic"),
    }


def validate_c002(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(C002_SUMMARY)
    checks_total = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    downstream = dynamic.get("downstream") if isinstance(dynamic, dict) else {}
    manifest_surface = downstream.get("manifest_surface") if isinstance(downstream, dict) else {}
    reuse_payload = downstream.get("reuse_payload") if isinstance(downstream, dict) else {}
    checks_total += require(payload.get("ok") is True, artifact, "M258-E002-C002-01", "C002 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == C002_CONTRACT_ID, artifact, "M258-E002-C002-02", "C002 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E002-C002-03", "C002 summary must report full coverage", failures)
    checks_total += require(manifest_surface.get("ready") is True, artifact, "M258-E002-C002-04", "C002 manifest surface must remain ready", failures)
    checks_total += require(manifest_surface.get("artifact_reuse_landed") is True, artifact, "M258-E002-C002-05", "C002 artifact reuse drifted", failures)
    checks_total += require(manifest_surface.get("downstream_module_consumption_ready") is True, artifact, "M258-E002-C002-06", "C002 downstream module consumption drifted", failures)
    checks_total += require(isinstance(reuse_payload.get("reused_module_names_lexicographic"), list), artifact, "M258-E002-C002-07", "C002 reused module set missing", failures)
    return checks_total, {
        "contract_id": payload.get("contract_id"),
        "manifest_ready": manifest_surface.get("ready"),
        "artifact_reuse_landed": manifest_surface.get("artifact_reuse_landed"),
        "reused_modules": reuse_payload.get("reused_module_names_lexicographic"),
    }


def validate_d002(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(D002_SUMMARY)
    checks_total = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    consumer = dynamic.get("consumer") if isinstance(dynamic, dict) else {}
    probe_summary = dynamic.get("probe") if isinstance(dynamic, dict) else {}
    plan = consumer.get("cross_module_link_plan") if isinstance(consumer, dict) else {}
    probe = probe_summary.get("probe_payload") if isinstance(probe_summary, dict) else {}
    checks_total += require(payload.get("ok") is True, artifact, "M258-E002-D002-01", "D002 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == D002_CONTRACT_ID, artifact, "M258-E002-D002-02", "D002 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E002-D002-03", "D002 summary must report full coverage", failures)
    checks_total += require(plan.get("ready") is True, artifact, "M258-E002-D002-04", "D002 link plan must remain ready", failures)
    checks_total += require(plan.get("module_names_lexicographic") == EXPECTED_MODULES, artifact, "M258-E002-D002-05", "D002 packaged module set drifted", failures)
    checks_total += require(int(probe_summary.get("link_returncode", -1)) == 0, artifact, "M258-E002-D002-06", "D002 probe link must remain successful", failures)
    checks_total += require(int(probe.get("startup_registered_image_count", 0)) == 2, artifact, "M258-E002-D002-07", "D002 startup image count drifted", failures)
    checks_total += require(int(probe.get("post_replay_registered_image_count", 0)) == 2, artifact, "M258-E002-D002-08", "D002 replay image count drifted", failures)
    return checks_total, {
        "contract_id": payload.get("contract_id"),
        "link_plan_ready": plan.get("ready"),
        "packaged_modules": plan.get("module_names_lexicographic"),
        "startup_registered_image_count": probe.get("startup_registered_image_count"),
        "post_replay_registered_image_count": probe.get("post_replay_registered_image_count"),
    }


def validate_e001(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    artifact = display_path(E001_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M258-E002-E001-01", "E001 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == E001_CONTRACT_ID, artifact, "M258-E002-E001-02", "E001 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E002-E001-03", "E001 summary must report full coverage", failures)
    checks_total += require(payload.get("next_closeout_issue") == "M258-E002", artifact, "M258-E002-E001-04", "E001 handoff must remain M258-E002", failures)
    return checks_total, {
        "contract_id": payload.get("contract_id"),
        "next_closeout_issue": payload.get("next_closeout_issue"),
    }


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    artifact = "dynamic_probe_payload"
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(isinstance(payload, dict), "M258-E002-DYN-PAY-01", "probe payload must be a JSON object")
    if not isinstance(payload, dict):
        return checks_passed, checks_total

    check(payload.get("startup_registration_copy_status") == 0, "M258-E002-DYN-PAY-02", "startup registration snapshot must succeed")
    check(payload.get("startup_registered_image_count") == 2, "M258-E002-DYN-PAY-03", "startup must register two images")
    check(payload.get("startup_next_expected_registration_order_ordinal") == 3, "M258-E002-DYN-PAY-04", "startup next expected ordinal must be 3")
    check(payload.get("startup_image_walk_status") == 0, "M258-E002-DYN-PAY-05", "startup image-walk snapshot must succeed")
    check(payload.get("startup_walked_image_count") == 2, "M258-E002-DYN-PAY-06", "startup image walk must report two walked images")
    check(payload.get("startup_graph_status") == 0, "M258-E002-DYN-PAY-07", "startup realized-class graph snapshot must succeed")
    check(int(payload.get("startup_realized_class_count", 0)) >= 2, "M258-E002-DYN-PAY-08", "startup realized-class graph must contain at least two classes")
    check(payload.get("imported_entry_status") == 0 and payload.get("imported_entry_found") == 1, "M258-E002-DYN-PAY-09", "imported provider entry must resolve")
    check(payload.get("local_entry_status") == 0 and payload.get("local_entry_found") == 1, "M258-E002-DYN-PAY-10", "local consumer entry must resolve")
    check(payload.get("imported_registration_order_ordinal") == 1, "M258-E002-DYN-PAY-11", "imported provider ordinal must remain 1")
    check(payload.get("local_registration_order_ordinal") == 2, "M258-E002-DYN-PAY-12", "local consumer ordinal must remain 2")
    check(payload.get("imported_direct_protocol_count") == 1, "M258-E002-DYN-PAY-13", "imported provider must publish one direct protocol")
    check(payload.get("local_direct_protocol_count") == 0, "M258-E002-DYN-PAY-14", "local consumer must publish zero direct protocols")
    check(payload.get("imported_runtime_property_accessor_count") == 0, "M258-E002-DYN-PAY-15", "imported provider must publish zero runtime property accessors")
    check(payload.get("local_runtime_property_accessor_count") == 0, "M258-E002-DYN-PAY-16", "local consumer must publish zero runtime property accessors")
    check(isinstance(payload.get("imported_class_owner_identity"), str), "M258-E002-DYN-PAY-17", "imported provider owner identity must remain present")
    check(isinstance(payload.get("local_class_owner_identity"), str), "M258-E002-DYN-PAY-18", "local consumer owner identity must remain present")
    check(int(payload.get("imported_provider_class_value", 0)) != 0, "M258-E002-DYN-PAY-19", "providerClassValue must remain non-zero on the current runtime path")
    check(int(payload.get("imported_provider_protocol_value", 0)) != 0, "M258-E002-DYN-PAY-20", "importedProtocolValue must remain non-zero on the current runtime path")
    check(int(payload.get("local_consumer_class_value", 0)) != 0, "M258-E002-DYN-PAY-21", "localClassValue must remain non-zero on the current runtime path")
    check(payload.get("selector_table_status") == 0, "M258-E002-DYN-PAY-22", "selector table snapshot must succeed")
    check(int(payload.get("selector_table_entry_count", 0)) >= 3, "M258-E002-DYN-PAY-23", "selector table must publish at least three entries")
    check(int(payload.get("selector_metadata_backed_selector_count", 0)) >= 3, "M258-E002-DYN-PAY-24", "selector table must publish at least three metadata-backed selectors")
    check(payload.get("selector_dynamic_selector_count") == 0, "M258-E002-DYN-PAY-25", "selector table must not publish dynamic selectors")
    check(payload.get("provider_selector_status") == 0 and payload.get("provider_selector_found") == 1 and payload.get("provider_selector_metadata_backed") == 1, "M258-E002-DYN-PAY-26", "provider selector lookup drifted")
    check(payload.get("provider_selector_provider_count") == 1, "M258-E002-DYN-PAY-27", "provider selector must have one metadata provider")
    check(payload.get("provider_selector_first_ordinal") == 1 and payload.get("provider_selector_last_ordinal") == 1, "M258-E002-DYN-PAY-28", "provider selector ordinal drifted")
    check(payload.get("imported_protocol_selector_status") == 0 and payload.get("imported_protocol_selector_found") == 1 and payload.get("imported_protocol_selector_metadata_backed") == 1, "M258-E002-DYN-PAY-29", "imported protocol selector lookup drifted")
    check(payload.get("imported_protocol_selector_provider_count") == 1, "M258-E002-DYN-PAY-30", "imported protocol selector must have one metadata provider")
    check(payload.get("imported_protocol_selector_first_ordinal") == 1 and payload.get("imported_protocol_selector_last_ordinal") == 1, "M258-E002-DYN-PAY-31", "imported protocol selector ordinal drifted")
    check(payload.get("local_selector_status") == 0 and payload.get("local_selector_found") == 1 and payload.get("local_selector_metadata_backed") == 1, "M258-E002-DYN-PAY-32", "local selector lookup drifted")
    check(payload.get("local_selector_provider_count") == 1, "M258-E002-DYN-PAY-33", "local selector must have one metadata provider")
    check(payload.get("local_selector_first_ordinal") == 2 and payload.get("local_selector_last_ordinal") == 2, "M258-E002-DYN-PAY-34", "local selector ordinal drifted")
    check(payload.get("method_cache_state_status") == 0, "M258-E002-DYN-PAY-35", "method-cache snapshot must succeed")
    check(int(payload.get("method_cache_entry_count", 0)) >= 3, "M258-E002-DYN-PAY-36", "method cache must publish at least three entries")
    check(payload.get("method_cache_live_dispatch_count") == 0, "M258-E002-DYN-PAY-37", "method cache must report zero live dispatches on the fallback path")
    check(payload.get("method_cache_fallback_dispatch_count") == 3, "M258-E002-DYN-PAY-38", "method cache must record three fallback dispatches")
    check(payload.get("method_cache_last_selector") == "localClassValue", "M258-E002-DYN-PAY-39", "method cache last selector drifted")
    check(payload.get("method_cache_last_resolved_class_name") is None, "M258-E002-DYN-PAY-40", "method cache last resolved class must remain null on the fallback path")
    check(payload.get("method_cache_last_resolved_owner_identity") is None, "M258-E002-DYN-PAY-41", "method cache last resolved owner must remain null on the fallback path")
    check(payload.get("provider_method_status") == 0 and payload.get("provider_method_found") == 1 and payload.get("provider_method_resolved") == 0, "M258-E002-DYN-PAY-42", "provider method cache entry must remain unresolved on the fallback path")
    check(payload.get("provider_method_owner_identity") is None, "M258-E002-DYN-PAY-43", "provider method owner must remain null on the fallback path")
    check(payload.get("imported_protocol_method_status") == 0 and payload.get("imported_protocol_method_found") == 1 and payload.get("imported_protocol_method_resolved") == 0, "M258-E002-DYN-PAY-44", "imported protocol method cache entry must remain unresolved on the fallback path")
    check(payload.get("imported_protocol_method_owner_identity") is None, "M258-E002-DYN-PAY-45", "imported protocol method owner must remain null on the fallback path")
    check(payload.get("local_method_status") == 0 and payload.get("local_method_found") == 1 and payload.get("local_method_resolved") == 0, "M258-E002-DYN-PAY-46", "local method cache entry must remain unresolved on the fallback path")
    check(payload.get("local_method_owner_identity") is None, "M258-E002-DYN-PAY-47", "local method owner must remain null on the fallback path")
    check(payload.get("protocol_query_status") == 0, "M258-E002-DYN-PAY-48", "protocol conformance query must succeed")
    check(payload.get("protocol_query_class_found") == 1 and payload.get("protocol_query_protocol_found") == 1 and payload.get("protocol_query_conforms") == 1, "M258-E002-DYN-PAY-49", "protocol conformance proof drifted")
    check(int(payload.get("protocol_query_visited_protocol_count", 0)) >= 1, "M258-E002-DYN-PAY-50", "protocol query must visit at least one protocol")
    check(payload.get("protocol_query_attached_category_count") == 0, "M258-E002-DYN-PAY-51", "protocol query must report zero attached categories for ImportedProvider")
    check(isinstance(payload.get("protocol_query_matched_protocol_owner_identity"), str), "M258-E002-DYN-PAY-52", "matched protocol owner identity must remain observable")
    check(payload.get("post_reset_registration_copy_status") == 0 and payload.get("post_reset_replay_copy_status") == 0, "M258-E002-DYN-PAY-53", "post-reset snapshots must succeed")
    check(payload.get("post_reset_registered_image_count") == 0, "M258-E002-DYN-PAY-54", "reset must clear live registration state")
    check(payload.get("post_reset_retained_bootstrap_image_count") == 2, "M258-E002-DYN-PAY-55", "reset must retain both bootstrap images")
    check(payload.get("post_reset_generation") >= 1, "M258-E002-DYN-PAY-56", "reset generation must advance")
    check(payload.get("replay_status") == 0, "M258-E002-DYN-PAY-57", "replay must succeed")
    check(payload.get("post_replay_registration_copy_status") == 0 and payload.get("post_replay_replay_copy_status") == 0, "M258-E002-DYN-PAY-58", "post-replay snapshots must succeed")
    check(payload.get("post_replay_registered_image_count") == 2, "M258-E002-DYN-PAY-59", "replay must restore two images")
    check(payload.get("post_replay_next_expected_registration_order_ordinal") == 3, "M258-E002-DYN-PAY-60", "post-replay next expected ordinal must be 3")
    check(payload.get("post_replay_image_walk_status") == 0 and payload.get("post_replay_walked_image_count") == 2, "M258-E002-DYN-PAY-61", "post-replay image walk must report two walked images")
    check(payload.get("post_replay_graph_status") == 0 and int(payload.get("post_replay_realized_class_count", 0)) >= 2, "M258-E002-DYN-PAY-62", "post-replay realized-class graph drifted")
    check(payload.get("post_replay_replay_generation") >= 1, "M258-E002-DYN-PAY-63", "replay generation must advance")
    check(payload.get("post_replay_retained_bootstrap_image_count") == 2, "M258-E002-DYN-PAY-64", "post-replay retained bootstrap image count drifted")
    check(payload.get("post_replay_imported_entry_status") == 0 and payload.get("post_replay_imported_entry_found") == 1, "M258-E002-DYN-PAY-65", "post-replay imported entry must resolve")
    check(payload.get("post_replay_local_entry_status") == 0 and payload.get("post_replay_local_entry_found") == 1, "M258-E002-DYN-PAY-66", "post-replay local entry must resolve")
    check(payload.get("post_replay_imported_provider_class_value") == payload.get("imported_provider_class_value"), "M258-E002-DYN-PAY-67", "post-replay providerClassValue must remain replay-stable")
    check(payload.get("post_replay_imported_provider_protocol_value") == payload.get("imported_provider_protocol_value"), "M258-E002-DYN-PAY-68", "post-replay importedProtocolValue must remain replay-stable")
    check(payload.get("post_replay_local_consumer_class_value") == payload.get("local_consumer_class_value"), "M258-E002-DYN-PAY-69", "post-replay localClassValue must remain replay-stable")
    check(payload.get("post_replay_selector_table_status") == 0, "M258-E002-DYN-PAY-70", "post-replay selector table snapshot must succeed")
    check(int(payload.get("post_replay_selector_table_entry_count", 0)) >= 3, "M258-E002-DYN-PAY-71", "post-replay selector table must publish at least three entries")
    check(int(payload.get("post_replay_selector_metadata_backed_selector_count", 0)) >= 3, "M258-E002-DYN-PAY-72", "post-replay selector metadata-backed count drifted")
    check(payload.get("post_replay_provider_selector_status") == 0 and payload.get("post_replay_provider_selector_found") == 1, "M258-E002-DYN-PAY-73", "post-replay provider selector lookup drifted")
    check(payload.get("post_replay_imported_protocol_selector_status") == 0 and payload.get("post_replay_imported_protocol_selector_found") == 1, "M258-E002-DYN-PAY-74", "post-replay imported protocol selector lookup drifted")
    check(payload.get("post_replay_local_selector_status") == 0 and payload.get("post_replay_local_selector_found") == 1, "M258-E002-DYN-PAY-75", "post-replay local selector lookup drifted")
    check(payload.get("post_replay_method_cache_state_status") == 0, "M258-E002-DYN-PAY-76", "post-replay method cache snapshot must succeed")
    check(int(payload.get("post_replay_method_cache_entry_count", 0)) >= 3, "M258-E002-DYN-PAY-77", "post-replay method cache must publish at least three entries")
    check(payload.get("post_replay_method_cache_live_dispatch_count") == 0, "M258-E002-DYN-PAY-78", "post-replay method cache must report zero live dispatches on the fallback path")
    check(payload.get("post_replay_method_cache_fallback_dispatch_count") == 3, "M258-E002-DYN-PAY-79", "post-replay method cache must record three fallback dispatches")
    check(payload.get("post_replay_method_cache_last_selector") == "localClassValue", "M258-E002-DYN-PAY-80", "post-replay method cache last selector drifted")
    check(payload.get("post_replay_method_cache_last_resolved_class_name") is None, "M258-E002-DYN-PAY-81", "post-replay method cache last resolved class must remain null on the fallback path")
    check(payload.get("post_replay_method_cache_last_resolved_owner_identity") is None, "M258-E002-DYN-PAY-82", "post-replay method cache last resolved owner must remain null on the fallback path")
    check(payload.get("post_replay_provider_method_status") == 0 and payload.get("post_replay_provider_method_found") == 1 and payload.get("post_replay_provider_method_resolved") == 0, "M258-E002-DYN-PAY-83", "post-replay provider method cache entry must remain unresolved on the fallback path")
    check(payload.get("post_replay_provider_method_owner_identity") is None, "M258-E002-DYN-PAY-84", "post-replay provider method owner must remain null on the fallback path")
    check(payload.get("post_replay_imported_protocol_method_status") == 0 and payload.get("post_replay_imported_protocol_method_found") == 1 and payload.get("post_replay_imported_protocol_method_resolved") == 0, "M258-E002-DYN-PAY-85", "post-replay imported protocol method cache entry must remain unresolved on the fallback path")
    check(payload.get("post_replay_imported_protocol_method_owner_identity") is None, "M258-E002-DYN-PAY-86", "post-replay imported protocol method owner must remain null on the fallback path")
    check(payload.get("post_replay_local_method_status") == 0 and payload.get("post_replay_local_method_found") == 1 and payload.get("post_replay_local_method_resolved") == 0, "M258-E002-DYN-PAY-87", "post-replay local method cache entry must remain unresolved on the fallback path")
    check(payload.get("post_replay_local_method_owner_identity") is None, "M258-E002-DYN-PAY-88", "post-replay local method owner must remain null on the fallback path")
    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    dynamic_summary: dict[str, Any] = {"skipped": False}

    def check(condition: bool, artifact: str, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), display_path(args.native_exe), "M258-E002-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), display_path(args.runtime_library), "M258-E002-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.provider_fixture.exists(), display_path(args.provider_fixture), "M258-E002-DYN-03", f"missing provider fixture: {display_path(args.provider_fixture)}")
    check(args.consumer_fixture.exists(), display_path(args.consumer_fixture), "M258-E002-DYN-04", f"missing consumer fixture: {display_path(args.consumer_fixture)}")
    check(args.runtime_probe.exists(), display_path(args.runtime_probe), "M258-E002-DYN-05", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "clang++", "M258-E002-DYN-06", f"unable to resolve any clang++ candidate from {args.clangxx}")
    if failures:
        return checks_passed, checks_total, dynamic_summary

    probe_dir = args.probe_root.resolve() / f"probe-{uuid.uuid4().hex}"
    provider_dir = probe_dir / "provider"
    consumer_dir = probe_dir / "consumer"

    provider_compile = compile_fixture(
        fixture=args.provider_fixture,
        out_dir=provider_dir,
        registration_order_ordinal=1,
    )
    provider_import = provider_dir / IMPORT_ARTIFACT
    provider_manifest = provider_dir / REGISTRATION_MANIFEST_ARTIFACT
    provider_discovery = provider_dir / DISCOVERY_ARTIFACT
    provider_rsp = provider_dir / RUNTIME_LINKER_RESPONSE_ARTIFACT
    provider_obj = provider_dir / OBJECT_ARTIFACT
    provider_backend = provider_dir / BACKEND_ARTIFACT
    check(provider_compile.returncode == 0, display_path(args.provider_fixture), "M258-E002-DYN-07", f"provider compile failed: {provider_compile.stdout}{provider_compile.stderr}")
    check(provider_import.exists(), display_path(provider_import), "M258-E002-DYN-08", "provider import surface missing")
    check(provider_manifest.exists(), display_path(provider_manifest), "M258-E002-DYN-09", "provider registration manifest missing")
    check(provider_discovery.exists(), display_path(provider_discovery), "M258-E002-DYN-10", "provider discovery artifact missing")
    check(provider_rsp.exists(), display_path(provider_rsp), "M258-E002-DYN-11", "provider runtime linker response missing")
    check(provider_obj.exists(), display_path(provider_obj), "M258-E002-DYN-12", "provider object missing")
    check(provider_backend.exists() and read_text(provider_backend).strip() == "llvm-direct", display_path(provider_backend), "M258-E002-DYN-13", "provider backend must be llvm-direct")
    if failures:
        return checks_passed, checks_total, dynamic_summary

    consumer_compile = compile_fixture(
        fixture=args.consumer_fixture,
        out_dir=consumer_dir,
        registration_order_ordinal=2,
        import_surface=provider_import,
    )
    consumer_import = consumer_dir / IMPORT_ARTIFACT
    consumer_manifest = consumer_dir / REGISTRATION_MANIFEST_ARTIFACT
    consumer_discovery = consumer_dir / DISCOVERY_ARTIFACT
    consumer_rsp = consumer_dir / RUNTIME_LINKER_RESPONSE_ARTIFACT
    consumer_obj = consumer_dir / OBJECT_ARTIFACT
    consumer_backend = consumer_dir / BACKEND_ARTIFACT
    consumer_plan = consumer_dir / LINK_PLAN_ARTIFACT
    consumer_plan_rsp = consumer_dir / LINKER_RESPONSE_ARTIFACT
    check(consumer_compile.returncode == 0, display_path(args.consumer_fixture), "M258-E002-DYN-14", f"consumer compile failed: {consumer_compile.stdout}{consumer_compile.stderr}")
    check(consumer_import.exists(), display_path(consumer_import), "M258-E002-DYN-15", "consumer import surface missing")
    check(consumer_manifest.exists(), display_path(consumer_manifest), "M258-E002-DYN-16", "consumer registration manifest missing")
    check(consumer_discovery.exists(), display_path(consumer_discovery), "M258-E002-DYN-17", "consumer discovery artifact missing")
    check(consumer_rsp.exists(), display_path(consumer_rsp), "M258-E002-DYN-18", "consumer runtime linker response missing")
    check(consumer_obj.exists(), display_path(consumer_obj), "M258-E002-DYN-19", "consumer object missing")
    check(consumer_backend.exists() and read_text(consumer_backend).strip() == "llvm-direct", display_path(consumer_backend), "M258-E002-DYN-20", "consumer backend must be llvm-direct")
    check(consumer_plan.exists(), display_path(consumer_plan), "M258-E002-DYN-21", "consumer cross-module link plan missing")
    check(consumer_plan_rsp.exists(), display_path(consumer_plan_rsp), "M258-E002-DYN-22", "consumer cross-module linker response missing")
    if failures:
        return checks_passed, checks_total, dynamic_summary

    provider_import_payload = load_json(provider_import)
    provider_manifest_payload = load_json(provider_manifest)
    consumer_import_payload = load_json(consumer_import)
    consumer_manifest_payload = load_json(consumer_manifest)
    plan_payload = load_json(consumer_plan)
    response_lines = parse_response_lines(consumer_plan_rsp)

    check(plan_payload.get("contract_id") == D002_CONTRACT_ID, display_path(consumer_plan), "M258-E002-DYN-23", "consumer cross-module link plan contract drifted")
    check(plan_payload.get("ready") is True, display_path(consumer_plan), "M258-E002-DYN-24", "consumer cross-module link plan must remain ready")
    check(plan_payload.get("module_names_lexicographic") == EXPECTED_MODULES, display_path(consumer_plan), "M258-E002-DYN-25", "consumer cross-module link plan module set drifted")
    check(plan_payload.get("module_image_count") == 2, display_path(consumer_plan), "M258-E002-DYN-26", "consumer cross-module link plan image count drifted")
    check(plan_payload.get("direct_import_input_count") == 1, display_path(consumer_plan), "M258-E002-DYN-27", "consumer cross-module link plan import count drifted")
    check(plan_payload.get("link_object_artifacts") == [normalize_path_text(provider_obj), normalize_path_text(consumer_obj)], display_path(consumer_plan), "M258-E002-DYN-28", "consumer cross-module link object order drifted")
    expected_flags = list(provider_manifest_payload.get("driver_linker_flags", [])) + list(consumer_manifest_payload.get("driver_linker_flags", []))
    check(plan_payload.get("driver_linker_flags") == expected_flags, display_path(consumer_plan), "M258-E002-DYN-29", "consumer merged driver linker flags drifted")
    check(response_lines == expected_flags, display_path(consumer_plan_rsp), "M258-E002-DYN-30", "consumer linker response content drifted")
    check(provider_import_payload.get("module_name") == "runtimePackagingProvider", display_path(provider_import), "M258-E002-DYN-31", "provider import-surface module name drifted")
    check(consumer_import_payload.get("module_name") == "runtimePackagingConsumer", display_path(consumer_import), "M258-E002-DYN-32", "consumer import-surface module name drifted")
    check(provider_manifest_payload.get("translation_unit_registration_order_ordinal") == 1, display_path(provider_manifest), "M258-E002-DYN-33", "provider manifest ordinal drifted")
    check(consumer_manifest_payload.get("translation_unit_registration_order_ordinal") == 2, display_path(consumer_manifest), "M258-E002-DYN-34", "consumer manifest ordinal drifted")
    if failures:
        dynamic_summary.update(
            {
                "provider_compile": {"returncode": provider_compile.returncode},
                "consumer_compile": {"returncode": consumer_compile.returncode},
                "consumer": {"cross_module_link_plan": plan_payload, "cross_module_linker_response": response_lines},
            }
        )
        return checks_passed, checks_total, dynamic_summary

    probe_exe = probe_dir / "m258_e002_import_module_execution_matrix_probe.exe"
    link_command = [
        str(clangxx),
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        f"-I{args.runtime_include_root.resolve()}",
        str(args.runtime_probe.resolve()),
        *[str(Path(value)) for value in plan_payload["link_object_artifacts"]],
        str(args.runtime_library.resolve()),
        f"@{consumer_plan_rsp.resolve()}",
        "-o",
        str(probe_exe.resolve()),
    ]
    link_completed = run_command(link_command)
    link_output = (link_completed.stdout or "") + (link_completed.stderr or "")
    check(link_completed.returncode == 0, display_path(probe_exe), "M258-E002-DYN-35", f"probe link failed: {link_output.strip()}")
    check(probe_exe.exists(), display_path(probe_exe), "M258-E002-DYN-36", "probe executable missing after link")
    check("LNK4078" not in link_output, display_path(probe_exe), "M258-E002-DYN-37", "probe link must not emit section-attribute mismatch warnings")
    if failures:
        dynamic_summary.update(
            {
                "provider_compile": {"returncode": provider_compile.returncode},
                "consumer_compile": {"returncode": consumer_compile.returncode},
                "consumer": {"cross_module_link_plan": plan_payload, "cross_module_linker_response": response_lines},
                "probe": {"link_command": link_command, "link_returncode": link_completed.returncode, "link_output": link_output},
            }
        )
        return checks_passed, checks_total, dynamic_summary

    probe_run = run_command([str(probe_exe.resolve())])
    check(probe_run.returncode == 0, display_path(probe_exe), "M258-E002-DYN-38", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        dynamic_summary.update(
            {
                "probe": {
                    "link_command": link_command,
                    "link_returncode": link_completed.returncode,
                    "link_output": link_output,
                    "run_returncode": probe_run.returncode,
                    "run_stdout": probe_run.stdout,
                    "run_stderr": probe_run.stderr,
                }
            }
        )
        return checks_passed, checks_total, dynamic_summary

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), "M258-E002-DYN-39", f"probe output is not valid JSON: {exc}"))
        dynamic_summary.update(
            {
                "probe": {
                    "link_command": link_command,
                    "link_returncode": link_completed.returncode,
                    "link_output": link_output,
                    "run_returncode": probe_run.returncode,
                    "run_stdout": probe_run.stdout,
                    "run_stderr": probe_run.stderr,
                }
            }
        )
        return checks_passed, checks_total + 1, dynamic_summary

    payload_passed, payload_total = validate_probe_payload(probe_payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total

    dynamic_summary.update(
        {
            "provider_compile": {
                "command": [str(NATIVE_EXE), str(args.provider_fixture), "--out-dir", str(provider_dir), "--emit-prefix", "module", "--objc3-bootstrap-registration-order-ordinal", "1"],
                "returncode": provider_compile.returncode,
            },
            "consumer_compile": {
                "command": [str(NATIVE_EXE), str(args.consumer_fixture), "--out-dir", str(consumer_dir), "--emit-prefix", "module", "--objc3-bootstrap-registration-order-ordinal", "2", "--objc3-import-runtime-surface", str(provider_import)],
                "returncode": consumer_compile.returncode,
            },
            "provider": {
                "import_surface": provider_import_payload,
                "registration_manifest": provider_manifest_payload,
                "runtime_metadata_response": parse_response_lines(provider_rsp),
            },
            "consumer": {
                "import_surface": consumer_import_payload,
                "registration_manifest": consumer_manifest_payload,
                "cross_module_link_plan": plan_payload,
                "cross_module_linker_response": response_lines,
            },
            "probe": {
                "probe_dir": display_path(probe_dir),
                "link_command": link_command,
                "link_returncode": link_completed.returncode,
                "link_output": link_output,
                "run_returncode": probe_run.returncode,
                "probe_payload": probe_payload,
            },
        }
    )
    return checks_passed, checks_total, dynamic_summary


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--probe-root", type=Path, default=PROBE_ROOT)
    parser.add_argument("--provider-fixture", type=Path, default=PROVIDER_FIXTURE)
    parser.add_argument("--consumer-fixture", type=Path, default=CONSUMER_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=RUNTIME_PROBE)
    parser.add_argument("--clangxx", nargs="+", default=list(CLANGXX_CANDIDATES))
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        static_total, static_findings = check_static_contract(path, snippets)
        checks_total += static_total
        checks_passed += static_total - len(static_findings)
        failures.extend(static_findings)

    upstream_summaries: dict[str, Any] = {}
    for label, validator, path in (
        ("M258-A002", validate_a002, A002_SUMMARY),
        ("M258-B002", validate_b002, B002_SUMMARY),
        ("M258-C002", validate_c002, C002_SUMMARY),
        ("M258-D002", validate_d002, D002_SUMMARY),
        ("M258-E001", validate_e001, E001_SUMMARY),
    ):
        total, distilled = validator(load_json(path), failures)
        checks_total += total
        checks_passed += total
        upstream_summaries[label] = distilled
    checks_passed -= len(failures)

    dynamic_summary: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        checks_total += ensure_binaries(failures)
        checks_passed = checks_total - len(failures)
        if not failures:
            dynamic_passed, dynamic_total, dynamic_summary = run_dynamic_case(args, failures)
            checks_total += dynamic_total
            checks_passed += dynamic_passed
        else:
            dynamic_summary = {"skipped": False}

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "execution_matrix_model": EXECUTION_MATRIX_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_total if not failures else checks_total - len(failures),
        "checks_total": checks_total,
        "failures": [failure.__dict__ for failure in failures],
        "upstream_summaries": upstream_summaries,
        "dynamic_probes": dynamic_summary,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.artifact} :: {failure.check_id} :: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} validated")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
