#!/usr/bin/env python3
"""Fail-closed checker for M258-A002 runtime-aware import/module frontend closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-a002-runtime-aware-import-module-frontend-closure-v1"
CONTRACT_ID = "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"
A001_CONTRACT_ID = "objc3c-runtime-aware-import-module-surface/m258-a001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_frontend_closure"
A001_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract"
ARTIFACT_RELATIVE_PATH = "module.runtime-import-surface.json"
PAYLOAD_MODEL = "runtime-aware-import-module-surface-json-v1"
AUTHORITY_MODEL = "runtime-import-surface-artifact-derived-from-frozen-import-surface-and-runtime-metadata-source-records"
PAYLOAD_OWNERSHIP_MODEL = "compiler-emits-runtime-import-surface-artifact-frontend-and-later-module-consumers-own-cross-translation-unit-handoff"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-A002" / "runtime_aware_import_module_frontend_closure_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "a002-runtime-aware-import-module-frontend-closure"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
MANIFEST_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py"
CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"


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
        SnippetCheck("M258-A002-DOC-01", "# M258 Import Surface For Runtime-Owned Declarations And Metadata References Core Feature Implementation Expectations (A002)"),
        SnippetCheck("M258-A002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-A002-DOC-03", "Issue: `#7159`"),
        SnippetCheck("M258-A002-DOC-04", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-A002-DOC-05", f"`{ARTIFACT_RELATIVE_PATH}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-A002-PKT-01", "# M258-A002 Import Surface For Runtime-Owned Declarations And Metadata References Core Feature Implementation Packet"),
        SnippetCheck("M258-A002-PKT-02", "Packet: `M258-A002`"),
        SnippetCheck("M258-A002-PKT-03", "Dependencies: `M258-A001`"),
        SnippetCheck("M258-A002-PKT-04", "Next issue: `M258-B001`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-A002-NDOC-01", "## Runtime-aware import/module frontend closure (M258-A002)"),
        SnippetCheck("M258-A002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-A002-NDOC-03", f"`{ARTIFACT_RELATIVE_PATH}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-A002-SPC-01", "## M258 runtime-aware import/module frontend closure (A002)"),
        SnippetCheck("M258-A002-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-A002-SPC-03", f"`{PAYLOAD_MODEL}`"),
        SnippetCheck("M258-A002-SPC-04", "IR still remains translation-unit local"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-A002-META-01", "## M258 runtime-aware import/module frontend artifact anchors (A002)"),
        SnippetCheck("M258-A002-META-02", f"`{ARTIFACT_RELATIVE_PATH}`"),
        SnippetCheck("M258-A002-META-03", "runtime-owned declaration inventories"),
        SnippetCheck("M258-A002-META-04", "metadata-reference inventories"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-A002-ARCH-01", "## M258 runtime-aware import/module frontend closure (A002)"),
        SnippetCheck("M258-A002-ARCH-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-A002-ARCH-03", f"`{ARTIFACT_RELATIVE_PATH}`"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M258-A002-TYPE-01", "struct Objc3RuntimeAwareImportModuleFrontendClosureSummary {"),
        SnippetCheck("M258-A002-TYPE-02", "runtime_owned_declaration_count"),
        SnippetCheck("M258-A002-TYPE-03", "metadata_reference_count"),
        SnippetCheck("M258-A002-TYPE-04", "ready_for_frontend_module_consumption"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-A002-ART-01", "BuildRuntimeAwareImportModuleFrontendClosureSummary("),
        SnippetCheck("M258-A002-ART-02", "BuildRuntimeAwareImportModuleFrontendClosureSummaryJson("),
        SnippetCheck("M258-A002-ART-03", "BuildRuntimeAwareImportModuleArtifactJson("),
        SnippetCheck("M258-A002-ART-04", "objc_runtime_aware_import_module_frontend_closure"),
    ),
    MANIFEST_ARTIFACTS_H: (
        SnippetCheck("M258-A002-MANH-01", "BuildRuntimeAwareImportModuleArtifactPath("),
        SnippetCheck("M258-A002-MANH-02", "WriteRuntimeAwareImportModuleArtifact("),
    ),
    MANIFEST_ARTIFACTS_CPP: (
        SnippetCheck("M258-A002-MANC-01", "BuildRuntimeAwareImportModuleArtifactPath("),
        SnippetCheck("M258-A002-MANC-02", "WriteRuntimeAwareImportModuleArtifact("),
        SnippetCheck("M258-A002-MANC-03", "kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M258-A002-DRV-01", "runtime-aware import/module frontend closure not ready"),
        SnippetCheck("M258-A002-DRV-02", "WriteRuntimeAwareImportModuleArtifact("),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M258-A002-ANCHOR-01", "runtime-aware import/module frontend closure not ready"),
        SnippetCheck("M258-A002-ANCHOR-02", "BuildRuntimeAwareImportModuleArtifactPath("),
    ),
    API_H: (
        SnippetCheck("M258-A002-API-01", "module.runtime-import-surface.json"),
        SnippetCheck("M258-A002-API-02", "in-memory imported module handles"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-A002-IR-01", "M258-A001/A002 runtime-aware import/module surface anchor"),
        SnippetCheck("M258-A002-IR-02", "frontend now emits a canonical runtime-import surface artifact"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-A002-PKG-01", '"check:objc3c:m258-a002-import-surface-runtime-owned-declarations-and-metadata-references"'),
        SnippetCheck("M258-A002-PKG-02", '"test:tooling:m258-a002-import-surface-runtime-owned-declarations-and-metadata-references"'),
        SnippetCheck("M258-A002-PKG-03", '"check:objc3c:m258-a002-lane-a-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-A002-RUN-01", "M258-A001 + M258-A002"),
        SnippetCheck("M258-A002-RUN-02", "check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-A002-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-A002-TEST-02", "def test_checker_passes_dynamic"),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, failures
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_binaries(failures: list[Finding]) -> int:
    checks_total = 0
    if NATIVE_EXE.exists() and RUNNER_EXE.exists():
        checks_total += require(True, display_path(NATIVE_EXE), "M258-A002-BIN-READY", "native and runner binaries present", failures)
        return checks_total
    build = run_process([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-A002-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-A002-NATIVE-EXISTS", "native binary missing after build", failures)
    checks_total += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M258-A002-RUNNER-EXISTS", "frontend runner missing after build", failures)
    return checks_total


def validate_surface_and_artifact(
    *,
    manifest: dict[str, Any],
    artifact_payload: dict[str, Any],
    artifact_label: str,
    expected_module: str,
    expect_class_records: bool,
    expect_category_records: bool,
    failures: list[Finding],
) -> tuple[int, dict[str, object]]:
    checks_total = 0
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    source_surface = semantic_surface.get("objc_runtime_aware_import_module_surface_contract") if isinstance(semantic_surface, dict) else None
    closure_surface = semantic_surface.get("objc_runtime_aware_import_module_frontend_closure") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(frontend, dict), artifact_label, "M258-A002-FRONTEND", "frontend payload missing", failures)
    checks_total += require(isinstance(source_surface, dict), artifact_label, "M258-A002-A001-SURFACE", "A001 source surface missing", failures)
    checks_total += require(isinstance(closure_surface, dict), artifact_label, "M258-A002-A002-SURFACE", "A002 closure surface missing", failures)
    if failures:
        return checks_total, {}

    declarations = artifact_payload.get("runtime_owned_declarations")
    references = artifact_payload.get("metadata_references")
    checks_total += require(isinstance(declarations, dict), artifact_label, "M258-A002-DECLS", "runtime-owned declaration inventory missing", failures)
    checks_total += require(isinstance(references, list), artifact_label, "M258-A002-REFS", "metadata reference inventory missing", failures)
    if failures:
        return checks_total, {}

    class_records = declarations.get("classes") if isinstance(declarations, dict) else None
    protocol_records = declarations.get("protocols") if isinstance(declarations, dict) else None
    category_records = declarations.get("categories") if isinstance(declarations, dict) else None
    property_records = declarations.get("properties") if isinstance(declarations, dict) else None
    method_records = declarations.get("methods") if isinstance(declarations, dict) else None
    ivar_records = declarations.get("ivars") if isinstance(declarations, dict) else None

    for key, value in {
        "classes": class_records,
        "protocols": protocol_records,
        "categories": category_records,
        "properties": property_records,
        "methods": method_records,
        "ivars": ivar_records,
    }.items():
        checks_total += require(isinstance(value, list), artifact_label, f"M258-A002-LIST-{key.upper()}", f"{key} inventory must be a list", failures)
    if failures:
        return checks_total, {}

    runtime_owned_declaration_count = (
        len(class_records)
        + len(protocol_records)
        + len(category_records)
        + len(property_records)
        + len(method_records)
        + len(ivar_records)
    )

    checks_total += require(closure_surface.get("contract_id") == CONTRACT_ID, artifact_label, "M258-A002-SURFACE-CONTRACT", "closure contract id mismatch", failures)
    checks_total += require(closure_surface.get("source_surface_contract_id") == A001_CONTRACT_ID, artifact_label, "M258-A002-SURFACE-A001", "upstream source contract id mismatch", failures)
    checks_total += require(closure_surface.get("frontend_surface_path") == SURFACE_PATH, artifact_label, "M258-A002-SURFACE-PATH", "closure surface path mismatch", failures)
    checks_total += require(closure_surface.get("artifact_relative_path") == ARTIFACT_RELATIVE_PATH, artifact_label, "M258-A002-SURFACE-ARTIFACT", "artifact relative path mismatch", failures)
    checks_total += require(closure_surface.get("payload_model") == PAYLOAD_MODEL, artifact_label, "M258-A002-SURFACE-PAYLOAD", "payload model mismatch", failures)
    checks_total += require(closure_surface.get("authority_model") == AUTHORITY_MODEL, artifact_label, "M258-A002-SURFACE-AUTHORITY", "authority model mismatch", failures)
    checks_total += require(closure_surface.get("payload_ownership_model") == PAYLOAD_OWNERSHIP_MODEL, artifact_label, "M258-A002-SURFACE-OWNERSHIP", "payload ownership model mismatch", failures)
    checks_total += require(closure_surface.get("module_name") == expected_module, artifact_label, "M258-A002-SURFACE-MODULE", "module name mismatch", failures)
    checks_total += require(closure_surface.get("source_surface_replay_key") == source_surface.get("replay_key"), artifact_label, "M258-A002-SOURCE-REPLAY", "A001 replay key bridge mismatch", failures)
    checks_total += require(closure_surface.get("ready") is True, artifact_label, "M258-A002-SURFACE-READY", "closure surface must be ready", failures)
    checks_total += require(closure_surface.get("fail_closed") is True, artifact_label, "M258-A002-SURFACE-FAIL-CLOSED", "closure surface must remain fail-closed", failures)
    checks_total += require(closure_surface.get("runtime_aware_import_declarations_landed") is True, artifact_label, "M258-A002-SURFACE-DECL-LANDED", "runtime-aware import declaration surface must be landed", failures)
    checks_total += require(closure_surface.get("module_metadata_import_surface_landed") is True, artifact_label, "M258-A002-SURFACE-MODULE-LANDED", "module metadata import surface must be landed", failures)
    checks_total += require(closure_surface.get("runtime_owned_declaration_import_landed") is True, artifact_label, "M258-A002-SURFACE-OWNED-LANDED", "runtime-owned declaration import surface must be landed", failures)
    checks_total += require(closure_surface.get("runtime_metadata_reference_import_landed") is True, artifact_label, "M258-A002-SURFACE-REF-LANDED", "runtime metadata reference surface must be landed", failures)
    checks_total += require(closure_surface.get("public_frontend_api_module_surface_landed") is True, artifact_label, "M258-A002-SURFACE-API-LANDED", "public frontend API surface must be landed", failures)
    checks_total += require(closure_surface.get("ready_for_import_artifact_emission") is True, artifact_label, "M258-A002-SURFACE-ARTIFACT-READY", "closure surface must be ready for artifact emission", failures)
    checks_total += require(closure_surface.get("ready_for_frontend_module_consumption") is True, artifact_label, "M258-A002-SURFACE-CONSUMPTION-READY", "closure surface must be ready for frontend module consumption", failures)
    checks_total += require(bool(closure_surface.get("replay_key")), artifact_label, "M258-A002-SURFACE-REPLAY", "closure replay key must be non-empty", failures)
    checks_total += require(closure_surface.get("failure_reason") == "", artifact_label, "M258-A002-SURFACE-FAILURE", "closure failure reason must be empty", failures)

    checks_total += require(artifact_payload.get("contract_id") == CONTRACT_ID, artifact_label, "M258-A002-ARTIFACT-CONTRACT", "artifact contract id mismatch", failures)
    checks_total += require(artifact_payload.get("source_surface_contract_id") == A001_CONTRACT_ID, artifact_label, "M258-A002-ARTIFACT-A001", "artifact source contract id mismatch", failures)
    checks_total += require(artifact_payload.get("frontend_surface_path") == SURFACE_PATH, artifact_label, "M258-A002-ARTIFACT-PATH", "artifact surface path mismatch", failures)
    checks_total += require(artifact_payload.get("artifact") == ARTIFACT_RELATIVE_PATH, artifact_label, "M258-A002-ARTIFACT-NAME", "artifact name mismatch", failures)
    checks_total += require(artifact_payload.get("payload_model") == PAYLOAD_MODEL, artifact_label, "M258-A002-ARTIFACT-PAYLOAD", "artifact payload model mismatch", failures)
    checks_total += require(artifact_payload.get("authority_model") == AUTHORITY_MODEL, artifact_label, "M258-A002-ARTIFACT-AUTHORITY", "artifact authority model mismatch", failures)
    checks_total += require(artifact_payload.get("payload_ownership_model") == PAYLOAD_OWNERSHIP_MODEL, artifact_label, "M258-A002-ARTIFACT-OWNERSHIP", "artifact payload ownership model mismatch", failures)
    checks_total += require(artifact_payload.get("module_name") == expected_module, artifact_label, "M258-A002-ARTIFACT-MODULE", "artifact module name mismatch", failures)
    checks_total += require(artifact_payload.get("source_surface_replay_key") == source_surface.get("replay_key"), artifact_label, "M258-A002-ARTIFACT-SOURCE-REPLAY", "artifact source replay key mismatch", failures)
    checks_total += require(artifact_payload.get("replay_key") == closure_surface.get("replay_key"), artifact_label, "M258-A002-ARTIFACT-REPLAY", "artifact replay key mismatch", failures)
    checks_total += require(artifact_payload.get("runtime_owned_declaration_count") == runtime_owned_declaration_count, artifact_label, "M258-A002-ARTIFACT-DECL-COUNT", "artifact runtime-owned declaration count mismatch", failures)
    checks_total += require(closure_surface.get("runtime_owned_declaration_count") == runtime_owned_declaration_count, artifact_label, "M258-A002-SURFACE-DECL-COUNT", "surface runtime-owned declaration count mismatch", failures)
    checks_total += require(artifact_payload.get("metadata_reference_count") == len(references), artifact_label, "M258-A002-ARTIFACT-REF-COUNT", "artifact metadata reference count mismatch", failures)
    checks_total += require(closure_surface.get("metadata_reference_count") == len(references), artifact_label, "M258-A002-SURFACE-REF-COUNT", "surface metadata reference count mismatch", failures)
    checks_total += require(closure_surface.get("class_record_count") == len(class_records), artifact_label, "M258-A002-SURFACE-CLASS-COUNT", "surface class-record count mismatch", failures)
    checks_total += require(closure_surface.get("protocol_record_count") == len(protocol_records), artifact_label, "M258-A002-SURFACE-PROTOCOL-COUNT", "surface protocol-record count mismatch", failures)
    checks_total += require(closure_surface.get("category_record_count") == len(category_records), artifact_label, "M258-A002-SURFACE-CATEGORY-COUNT", "surface category-record count mismatch", failures)
    checks_total += require(closure_surface.get("property_record_count") == len(property_records), artifact_label, "M258-A002-SURFACE-PROPERTY-COUNT", "surface property-record count mismatch", failures)
    checks_total += require(closure_surface.get("method_record_count") == len(method_records), artifact_label, "M258-A002-SURFACE-METHOD-COUNT", "surface method-record count mismatch", failures)
    checks_total += require(closure_surface.get("ivar_record_count") == len(ivar_records), artifact_label, "M258-A002-SURFACE-IVAR-COUNT", "surface ivar-record count mismatch", failures)

    checks_total += require(source_surface.get("contract_id") == A001_CONTRACT_ID, artifact_label, "M258-A002-A001-CONTRACT", "A001 contract id mismatch", failures)
    checks_total += require(source_surface.get("surface_path") == A001_SURFACE_PATH, artifact_label, "M258-A002-A001-PATH", "A001 surface path mismatch", failures)
    checks_total += require(source_surface.get("module_name") == expected_module, artifact_label, "M258-A002-A001-MODULE", "A001 module name mismatch", failures)
    checks_total += require(source_surface.get("runtime_aware_import_declarations_landed") is False, artifact_label, "M258-A002-A001-DECL-FALSE", "A001 declaration landed flag must remain false", failures)
    checks_total += require(source_surface.get("module_metadata_import_surface_landed") is False, artifact_label, "M258-A002-A001-MODULE-FALSE", "A001 module surface landed flag must remain false", failures)
    checks_total += require(source_surface.get("runtime_owned_declaration_import_landed") is False, artifact_label, "M258-A002-A001-OWNED-FALSE", "A001 runtime-owned declaration landed flag must remain false", failures)
    checks_total += require(source_surface.get("runtime_metadata_reference_import_landed") is False, artifact_label, "M258-A002-A001-REF-FALSE", "A001 metadata reference landed flag must remain false", failures)
    checks_total += require(source_surface.get("public_frontend_api_module_surface_landed") is False, artifact_label, "M258-A002-A001-API-FALSE", "A001 public API landed flag must remain false", failures)

    checks_total += require((len(class_records) > 0) is expect_class_records, artifact_label, "M258-A002-DECL-CLASS", "class inventory expectation mismatch", failures)
    checks_total += require(len(protocol_records) > 0, artifact_label, "M258-A002-DECL-PROTOCOL", "protocol inventory must be non-empty", failures)
    checks_total += require(len(property_records) > 0, artifact_label, "M258-A002-DECL-PROPERTY", "property inventory must be non-empty", failures)
    checks_total += require(len(method_records) > 0, artifact_label, "M258-A002-DECL-METHOD", "method inventory must be non-empty", failures)
    checks_total += require(len(ivar_records) >= 0, artifact_label, "M258-A002-DECL-IVAR", "ivar inventory presence check failed", failures)
    checks_total += require((len(category_records) > 0) is expect_category_records, artifact_label, "M258-A002-DECL-CATEGORY", "category inventory expectation mismatch", failures)
    checks_total += require(any(ref.get("reference_kind") == "method-selector" for ref in references), artifact_label, "M258-A002-REF-METHOD", "method selector references must be present", failures)
    checks_total += require(any(ref.get("reference_kind") == "property-getter-selector" for ref in references), artifact_label, "M258-A002-REF-GETTER", "property getter references must be present", failures)
    checks_total += require(any(ref.get("reference_kind") == "protocol-inherited-protocol" for ref in references) or any(ref.get("reference_kind") == "class-adopted-protocol" for ref in references) or any(ref.get("reference_kind") == "category-adopted-protocol" for ref in references), artifact_label, "M258-A002-REF-PROTOCOL", "protocol references must be present", failures)

    return checks_total, {
        "module_name": expected_module,
        "runtime_owned_declaration_count": runtime_owned_declaration_count,
        "metadata_reference_count": len(references),
        "class_record_count": len(class_records),
        "protocol_record_count": len(protocol_records),
        "category_record_count": len(category_records),
        "property_record_count": len(property_records),
        "method_record_count": len(method_records),
        "ivar_record_count": len(ivar_records),
        "surface_replay_key": closure_surface.get("replay_key"),
    }


def compile_fixture(
    *,
    label: str,
    executable: Path,
    fixture: Path,
    out_dir: Path,
    expected_module: str,
    expect_class_records: bool,
    expect_category_records: bool,
    extra_args: Sequence[str] = (),
    require_backend: bool = False,
    failures: list[Finding],
) -> tuple[int, dict[str, object], dict[str, Any]]:
    checks_total = 0
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process([str(executable), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", *extra_args])
    manifest_path = out_dir / "module.manifest.json"
    artifact_path = out_dir / ARTIFACT_RELATIVE_PATH
    checks_total += require(completed.returncode == 0, label, "M258-A002-COMPILE", completed.stderr or completed.stdout or "compile failed", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M258-A002-MANIFEST", "manifest missing", failures)
    checks_total += require(artifact_path.exists(), display_path(artifact_path), "M258-A002-ARTIFACT", "runtime import artifact missing", failures)
    if require_backend:
        backend_path = out_dir / "module.object-backend.txt"
        checks_total += require(backend_path.exists(), display_path(backend_path), "M258-A002-BACKEND", "backend marker missing", failures)
        if backend_path.exists():
            backend_text = backend_path.read_text(encoding="utf-8").strip()
            checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M258-A002-BACKEND-TEXT", "backend marker must remain llvm-direct", failures)
    if failures:
        return checks_total, {}, {}
    manifest = load_json(manifest_path)
    artifact_payload = load_json(artifact_path)
    surface_checks, summary = validate_surface_and_artifact(
        manifest=manifest,
        artifact_payload=artifact_payload,
        artifact_label=display_path(artifact_path),
        expected_module=expected_module,
        expect_class_records=expect_class_records,
        expect_category_records=expect_category_records,
        failures=failures,
    )
    checks_total += surface_checks
    return checks_total, summary, artifact_payload


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding], int]:
    failures: list[Finding] = []
    checks_total = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_checks, path_failures = check_static_contract(path, snippets)
        checks_total += path_checks
        failures.extend(path_failures)
        static_summary[display_path(path)] = {"checks": path_checks, "ok": not path_failures}

    dynamic_summary: dict[str, object] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        checks_total += ensure_binaries(failures)
        if not failures:
            native_class_checks, native_class_summary, native_class_artifact = compile_fixture(
                label="native-class-fixture",
                executable=NATIVE_EXE,
                fixture=CLASS_FIXTURE,
                out_dir=PROBE_ROOT / "native_class",
                expected_module="runtimeMetadataClassRecords",
                expect_class_records=True,
                expect_category_records=False,
                failures=failures,
                require_backend=True,
            )
            checks_total += native_class_checks
            runner_class_checks, runner_class_summary, runner_class_artifact = compile_fixture(
                label="runner-class-fixture",
                executable=RUNNER_EXE,
                fixture=CLASS_FIXTURE,
                out_dir=PROBE_ROOT / "runner_class",
                expected_module="runtimeMetadataClassRecords",
                expect_class_records=True,
                expect_category_records=False,
                extra_args=("--no-emit-ir", "--no-emit-object"),
                failures=failures,
            )
            checks_total += runner_class_checks
            native_category_checks, native_category_summary, native_category_artifact = compile_fixture(
                label="native-category-fixture",
                executable=NATIVE_EXE,
                fixture=CATEGORY_FIXTURE,
                out_dir=PROBE_ROOT / "native_category",
                expected_module="runtimeMetadataCategoryRecords",
                expect_class_records=False,
                expect_category_records=True,
                failures=failures,
                require_backend=True,
            )
            checks_total += native_category_checks
            checks_total += require(
                canonical_json(native_class_artifact) == canonical_json(runner_class_artifact),
                "native/runner class artifact parity",
                "M258-A002-PARITY",
                "native driver and frontend runner must emit identical class import artifacts",
                failures,
            )
            dynamic_summary.update(
                {
                    "native_class": native_class_summary,
                    "runner_class": runner_class_summary,
                    "native_category": native_category_summary,
                    "parity": canonical_json(native_class_artifact) == canonical_json(runner_class_artifact),
                }
            )

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total,
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    if failures:
        payload["checks_passed"] = checks_total - len(failures)
    return payload, failures, checks_total


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true", help="only run static contract checks")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT, help="where to write the JSON summary")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    payload, failures, checks_total = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    checks_passed = payload["checks_passed"]
    if failures:
        print(f"[error] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for failure in failures:
            print(f"- {failure.check_id} [{failure.artifact}] {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
