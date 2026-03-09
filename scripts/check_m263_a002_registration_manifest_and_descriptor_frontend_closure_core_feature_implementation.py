#!/usr/bin/env python3
"""Fail-closed checker for M263-A002 registration-descriptor frontend closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-a002-registration-manifest-and-descriptor-frontend-closure-v1"
CONTRACT_ID = "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"
REGISTRATION_MANIFEST_CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
SOURCE_SURFACE_CONTRACT_ID = "objc3c-bootstrap-registration-descriptor-image-root-source-surface/m263-a001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_frontend_closure"
ARTIFACT_RELATIVE_PATH = "module.runtime-registration-descriptor.json"
PAYLOAD_MODEL = "runtime-registration-descriptor-json-v1"
AUTHORITY_MODEL = "registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest"
PAYLOAD_OWNERSHIP_MODEL = "compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity"
BOOTSTRAP_OWNERSHIP_MODEL = "image-root-owns-registration-descriptor-runtime-owns-bootstrap-state"
MODULE_IDENTITY_SOURCE = "module-declaration-or-default"
PRAGMA_IDENTITY_SOURCE = "source-pragma"
DEFAULT_IDENTITY_SOURCE = "module-derived-default"
RUNTIME_SUPPORT_LIBRARY_RELATIVE_PATH = "artifacts/lib/objc3_runtime.lib"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-A002" / "registration_manifest_and_descriptor_frontend_closure_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m263" / "a002-registration-manifest-and-descriptor-frontend-closure"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
MANIFEST_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m263_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py"
EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_registration_descriptor_frontend_closure_explicit.objc3"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_registration_descriptor_frontend_closure_default.objc3"


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
        SnippetCheck("M263-A002-DOC-EXP-01", "# M263 Registration Manifest and Descriptor Frontend Closure Core Feature Implementation Expectations (A002)"),
        SnippetCheck("M263-A002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M263-A002-DOC-EXP-03", "Issue: `#7221`"),
        SnippetCheck("M263-A002-DOC-EXP-04", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-A002-DOC-EXP-05", f"`{ARTIFACT_RELATIVE_PATH}`"),
        SnippetCheck("M263-A002-DOC-EXP-06", f"`{PAYLOAD_OWNERSHIP_MODEL}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M263-A002-DOC-PKT-01", "# M263-A002 Registration Manifest and Descriptor Frontend Closure Core Feature Implementation Packet"),
        SnippetCheck("M263-A002-DOC-PKT-02", "Packet: `M263-A002`"),
        SnippetCheck("M263-A002-DOC-PKT-03", "Dependencies: `M263-A001`"),
        SnippetCheck("M263-A002-DOC-PKT-04", "Next issue: `M263-B001`"),
        SnippetCheck("M263-A002-DOC-PKT-05", f"`{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M263-A002-NDOC-01", "## Registration manifest and descriptor frontend closure (M263-A002)"),
        SnippetCheck("M263-A002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M263-A002-NDOC-03", f"`{ARTIFACT_RELATIVE_PATH}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M263-A002-SPC-01", "## M263 registration manifest and descriptor frontend closure (A002)"),
        SnippetCheck("M263-A002-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-A002-SPC-03", f"`{PAYLOAD_MODEL}`"),
        SnippetCheck("M263-A002-SPC-04", f"`{AUTHORITY_MODEL}`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M263-A002-META-01", "## M263 registration descriptor frontend artifact metadata anchors (A002)"),
        SnippetCheck("M263-A002-META-02", f"`{ARTIFACT_RELATIVE_PATH}`"),
        SnippetCheck("M263-A002-META-03", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-A002-META-04", "`registration_descriptor_identity_source`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M263-A002-ARCH-01", "M263 lane-A A002 extends that frozen source surface into one emitted"),
        SnippetCheck("M263-A002-ARCH-02", "`module.runtime-registration-descriptor.json`"),
        SnippetCheck("M263-A002-ARCH-03", f"{PAYLOAD_OWNERSHIP_MODEL}"),
    ),
    TOKEN_HEADER: (
        SnippetCheck("M263-A002-TOKEN-01", "M263-A001/A002 source-surface/frontend-closure anchor"),
    ),
    LEXER_CPP: (
        SnippetCheck("M263-A002-LEX-01", "ConsumeBootstrapRegistrationPragmaDirective("),
    ),
    PARSER_CPP: (
        SnippetCheck("M263-A002-PARSE-01", "M263-A001/A002 registration-descriptor/image-root source-surface and"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M263-A002-TYPES-01", "struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary {"),
        SnippetCheck("M263-A002-TYPES-02", "artifact_relative_path"),
        SnippetCheck("M263-A002-TYPES-03", "ready_for_descriptor_artifact_emission"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M263-A002-ART-01", "BuildRuntimeRegistrationDescriptorFrontendClosureSummary("),
        SnippetCheck("M263-A002-ART-02", "BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson("),
        SnippetCheck("M263-A002-ART-03", "objc_runtime_registration_descriptor_frontend_closure"),
        SnippetCheck("M263-A002-ART-04", "runtime_registration_descriptor_frontend_closure_artifact_relative_path"),
    ),
    MANIFEST_ARTIFACTS_H: (
        SnippetCheck("M263-A002-MANH-01", "BuildRuntimeRegistrationDescriptorArtifactPath("),
        SnippetCheck("M263-A002-MANH-02", "WriteRuntimeRegistrationDescriptorArtifact("),
    ),
    MANIFEST_ARTIFACTS_CPP: (
        SnippetCheck("M263-A002-MANC-01", "BuildRuntimeRegistrationDescriptorArtifactPath("),
        SnippetCheck("M263-A002-MANC-02", "WriteRuntimeRegistrationDescriptorArtifact("),
        SnippetCheck("M263-A002-MANC-03", "kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactSuffix"),
    ),
    PROCESS_HEADER: (
        SnippetCheck("M263-A002-PHDR-01", "struct Objc3RuntimeRegistrationDescriptorArtifactInputs {"),
        SnippetCheck("M263-A002-PHDR-02", "TryBuildObjc3RuntimeRegistrationDescriptorArtifact("),
    ),
    PROCESS_CPP: (
        SnippetCheck("M263-A002-PROC-01", "TryBuildObjc3RuntimeRegistrationDescriptorArtifact("),
        SnippetCheck("M263-A002-PROC-02", '<< "  \\"artifact\\": \\""'),
        SnippetCheck("M263-A002-PROC-03", '<< "  \\"ready_for_registration_descriptor_lowering\\": true\\n"'),
    ),
    DRIVER_CPP: (
        SnippetCheck("M263-A002-DRV-01", "registration descriptor frontend closure not ready"),
        SnippetCheck("M263-A002-DRV-02", "WriteRuntimeRegistrationDescriptorArtifact("),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M263-A002-ANCHOR-01", "registration descriptor frontend closure not ready"),
        SnippetCheck("M263-A002-ANCHOR-02", "BuildRuntimeRegistrationDescriptorArtifactPath("),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M263-A002-PKG-01", '"check:objc3c:m263-a002-registration-manifest-and-descriptor-frontend-closure"'),
        SnippetCheck("M263-A002-PKG-02", '"test:tooling:m263-a002-registration-manifest-and-descriptor-frontend-closure"'),
        SnippetCheck("M263-A002-PKG-03", '"check:objc3c:m263-a002-lane-a-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M263-A002-RUN-01", "M263-A001 + M254-A002"),
        SnippetCheck("M263-A002-RUN-02", "check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M263-A002-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M263-A002-TEST-02", "def test_checker_passes_dynamic"),
    ),
    EXPLICIT_FIXTURE: (
        SnippetCheck("M263-A002-FIX-EXP-01", "#pragma objc_registration_descriptor(FrontendDescriptorRegistration)"),
        SnippetCheck("M263-A002-FIX-EXP-02", "#pragma objc_image_root(FrontendDescriptorImageRoot)"),
        SnippetCheck("M263-A002-FIX-EXP-03", "module DescriptorFrontendExplicit;"),
    ),
    DEFAULT_FIXTURE: (
        SnippetCheck("M263-A002-FIX-DEF-01", "module DescriptorFrontendAuto;"),
        SnippetCheck("M263-A002-FIX-DEF-02", "#pragma objc_language_version(3)"),
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


def compile_fixture(
    *,
    fixture: Path,
    out_dir: Path,
    expected_module: str,
    expected_registration_descriptor: str,
    expected_image_root: str,
    expected_registration_source: str,
    expected_image_root_source: str,
) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M263-A002-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(fixture.exists(), display_path(fixture), "M263-A002-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures:
        return checks_total, failures, {}

    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
    )
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    registration_descriptor_path = out_dir / ARTIFACT_RELATIVE_PATH
    backend_path = out_dir / "module.object-backend.txt"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M263-A002-COMPILE", "fixture must compile successfully", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M263-A002-MANIFEST", "module manifest is missing", failures)
    checks_total += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M263-A002-REG-MANIFEST", "runtime registration manifest is missing", failures)
    checks_total += require(registration_descriptor_path.exists(), display_path(registration_descriptor_path), "M263-A002-REG-DESCRIPTOR", "runtime registration descriptor is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M263-A002-BACKEND", "backend marker is missing", failures)
    if failures:
        return checks_total, failures, {
            "fixture": display_path(fixture),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    manifest = load_json(manifest_path)
    registration_manifest = load_json(registration_manifest_path)
    registration_descriptor = load_json(registration_descriptor_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()

    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    flattened = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    surface = semantic_surface.get("objc_runtime_registration_descriptor_frontend_closure") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(frontend, dict), display_path(manifest_path), "M263-A002-FRONTEND", "frontend payload missing", failures)
    checks_total += require(isinstance(surface, dict), display_path(manifest_path), "M263-A002-SURFACE", "registration descriptor frontend closure missing", failures)
    checks_total += require(isinstance(flattened, dict), display_path(manifest_path), "M263-A002-FLAT", "flattened summary missing", failures)
    if failures:
        return checks_total, failures, {
            "fixture": display_path(fixture),
            "backend": backend_text,
        }

    def check_expected_container(
        container: dict[str, Any],
        artifact: Path | str,
        prefix: str = "",
        *,
        require_registration_manifest_contract: bool = True,
        require_surface_path: bool = True,
    ) -> None:
        nonlocal checks_total
        label = display_path(artifact) if isinstance(artifact, Path) else artifact
        checks_total += require(container.get(f"{prefix}contract_id") == CONTRACT_ID, label, "M263-A002-CONTRACT", "contract id mismatch", failures)
        if require_registration_manifest_contract:
            checks_total += require(
                container.get(f"{prefix}registration_manifest_contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID,
                label,
                "M263-A002-UPSTREAM-MANIFEST",
                "registration-manifest contract id mismatch",
                failures,
            )
        source_key = f"{prefix}source_surface_contract_id"
        checks_total += require(container.get(source_key) == SOURCE_SURFACE_CONTRACT_ID, label, "M263-A002-UPSTREAM-SOURCE", "source-surface contract id mismatch", failures)
        surface_key = f"{prefix}frontend_surface_path" if prefix else "frontend_surface_path"
        artifact_key = f"{prefix}artifact_relative_path" if prefix else "artifact_relative_path"
        if require_surface_path:
            checks_total += require(container.get(surface_key) == SURFACE_PATH, label, "M263-A002-SURFACE-PATH", "frontend surface path mismatch", failures)
        checks_total += require(container.get(artifact_key) == ARTIFACT_RELATIVE_PATH, label, "M263-A002-ARTIFACT-PATH", "artifact relative path mismatch", failures)
        checks_total += require(container.get(f"{prefix}payload_model") == PAYLOAD_MODEL, label, "M263-A002-PAYLOAD", "payload model mismatch", failures)
        checks_total += require(container.get(f"{prefix}authority_model") == AUTHORITY_MODEL, label, "M263-A002-AUTHORITY", "authority model mismatch", failures)
        checks_total += require(container.get(f"{prefix}payload_ownership_model") == PAYLOAD_OWNERSHIP_MODEL, label, "M263-A002-PAYLOAD-OWNERSHIP", "payload ownership model mismatch", failures)
        checks_total += require(container.get(f"{prefix}registration_descriptor_identifier") == expected_registration_descriptor, label, "M263-A002-REG-ID", "registration descriptor identifier mismatch", failures)
        checks_total += require(container.get(f"{prefix}registration_descriptor_identity_source") == expected_registration_source, label, "M263-A002-REG-SOURCE", "registration descriptor identity source mismatch", failures)
        checks_total += require(container.get(f"{prefix}image_root_identifier") == expected_image_root, label, "M263-A002-ROOT-ID", "image-root identifier mismatch", failures)
        checks_total += require(container.get(f"{prefix}image_root_identity_source") == expected_image_root_source, label, "M263-A002-ROOT-SOURCE", "image-root identity source mismatch", failures)
        checks_total += require(container.get(f"{prefix}bootstrap_visible_metadata_ownership_model") == BOOTSTRAP_OWNERSHIP_MODEL, label, "M263-A002-BOOTSTRAP-OWNERSHIP", "bootstrap ownership model mismatch", failures)
        checks_total += require(container.get(f"{prefix}class_descriptor_count") == 0, label, "M263-A002-CLASS-COUNT", "class descriptor count must be zero for probe fixtures", failures)
        checks_total += require(container.get(f"{prefix}protocol_descriptor_count") == 0, label, "M263-A002-PROTOCOL-COUNT", "protocol descriptor count must be zero for probe fixtures", failures)
        checks_total += require(container.get(f"{prefix}category_descriptor_count") == 0, label, "M263-A002-CATEGORY-COUNT", "category descriptor count must be zero for probe fixtures", failures)
        checks_total += require(container.get(f"{prefix}property_descriptor_count") == 0, label, "M263-A002-PROPERTY-COUNT", "property descriptor count must be zero for probe fixtures", failures)
        checks_total += require(container.get(f"{prefix}ivar_descriptor_count") == 0, label, "M263-A002-IVAR-COUNT", "ivar descriptor count must be zero for probe fixtures", failures)
        checks_total += require(container.get(f"{prefix}total_descriptor_count") == 0, label, "M263-A002-TOTAL-COUNT", "total descriptor count must be zero for probe fixtures", failures)

    check_expected_container(surface, manifest_path)
    checks_total += require(surface.get("ready") is True, display_path(manifest_path), "M263-A002-READY", "surface must be ready", failures)
    checks_total += require(surface.get("fail_closed") is True, display_path(manifest_path), "M263-A002-FAIL-CLOSED", "surface must be fail-closed", failures)
    checks_total += require(surface.get("source_surface_contract_ready") is True, display_path(manifest_path), "M263-A002-SOURCE-READY", "surface must report source-surface readiness", failures)
    checks_total += require(surface.get("registration_manifest_contract_ready") is True, display_path(manifest_path), "M263-A002-MANIFEST-READY", "surface must report registration-manifest readiness", failures)
    checks_total += require(surface.get("descriptor_frontend_surface_published") is True, display_path(manifest_path), "M263-A002-FRONTEND-PUBLISHED", "surface must be published", failures)
    checks_total += require(surface.get("descriptor_artifact_template_published") is True, display_path(manifest_path), "M263-A002-TEMPLATE-PUBLISHED", "artifact template must be published", failures)
    checks_total += require(surface.get("descriptor_fields_resolved") is True, display_path(manifest_path), "M263-A002-FIELDS-RESOLVED", "descriptor fields must be resolved", failures)
    checks_total += require(surface.get("ready_for_descriptor_artifact_emission") is True, display_path(manifest_path), "M263-A002-ARTIFACT-READY", "surface must be ready for artifact emission", failures)
    checks_total += require(surface.get("ready_for_registration_descriptor_lowering") is True, display_path(manifest_path), "M263-A002-LOWERING-READY", "surface must be ready for lowering", failures)
    checks_total += require(bool(surface.get("source_surface_replay_key")), display_path(manifest_path), "M263-A002-SOURCE-REPLAY", "source-surface replay key must be non-empty", failures)
    checks_total += require(bool(surface.get("registration_manifest_replay_key")), display_path(manifest_path), "M263-A002-MANIFEST-REPLAY", "registration-manifest replay key must be non-empty", failures)
    checks_total += require(bool(surface.get("replay_key")), display_path(manifest_path), "M263-A002-REPLAY", "surface replay key must be non-empty", failures)
    checks_total += require(surface.get("failure_reason") == "", display_path(manifest_path), "M263-A002-FAILURE", "surface failure reason must be empty", failures)

    check_expected_container(
        flattened,
        manifest_path,
        "runtime_registration_descriptor_frontend_closure_",
        require_registration_manifest_contract=False,
        require_surface_path=False,
    )

    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M263-A002-BACKEND-TEXT", "backend marker must remain llvm-direct", failures)
    checks_total += require(registration_descriptor.get("artifact") == ARTIFACT_RELATIVE_PATH, display_path(registration_descriptor_path), "M263-A002-ARTIFACT-NAME", "descriptor artifact name mismatch", failures)
    checks_total += require(registration_descriptor.get("payload_model") == PAYLOAD_MODEL, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-PAYLOAD", "descriptor payload model mismatch", failures)
    checks_total += require(registration_descriptor.get("authority_model") == AUTHORITY_MODEL, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-AUTHORITY", "descriptor authority model mismatch", failures)
    checks_total += require(registration_descriptor.get("payload_ownership_model") == PAYLOAD_OWNERSHIP_MODEL, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-OWNERSHIP", "descriptor ownership model mismatch", failures)
    checks_total += require(registration_descriptor.get("module_identity_source") == MODULE_IDENTITY_SOURCE, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-MODULE-SOURCE", "module identity source mismatch", failures)
    checks_total += require(registration_descriptor.get("runtime_support_library_archive_relative_path") == RUNTIME_SUPPORT_LIBRARY_RELATIVE_PATH, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-RUNTIME-LIB", "runtime support library path mismatch", failures)
    checks_total += require(registration_descriptor.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-ENTRYPOINT", "registration entrypoint mismatch", failures)
    checks_total += require(registration_descriptor.get("ready_for_descriptor_artifact_emission") is True, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-READY", "descriptor artifact must report emission readiness", failures)
    checks_total += require(registration_descriptor.get("ready_for_registration_descriptor_lowering") is True, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-LOWERING", "descriptor artifact must report lowering readiness", failures)
    checks_total += require(bool(registration_descriptor.get("translation_unit_identity_key")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-TU-KEY", "translation unit identity key must be non-empty", failures)
    checks_total += require(bool(registration_descriptor.get("constructor_root_symbol")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-CTOR", "constructor root symbol must be non-empty", failures)
    checks_total += require(bool(registration_descriptor.get("constructor_init_stub_symbol")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-INIT-STUB", "constructor init stub symbol must be non-empty", failures)
    checks_total += require(bool(registration_descriptor.get("bootstrap_registration_table_symbol")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-TABLE", "registration table symbol must be non-empty", failures)
    checks_total += require(bool(registration_descriptor.get("bootstrap_image_local_init_state_symbol")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-INIT-STATE", "image-local init state symbol must be non-empty", failures)
    checks_total += require(bool(registration_descriptor.get("linker_anchor_symbol")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-LINKER-ANCHOR", "linker anchor symbol must be non-empty", failures)
    checks_total += require(bool(registration_descriptor.get("discovery_root_symbol")), display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-DISCOVERY", "discovery root symbol must be non-empty", failures)
    checks_total += require(registration_descriptor.get("object_artifact") == "module.obj", display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-OBJECT", "object artifact name mismatch", failures)
    checks_total += require(registration_descriptor.get("backend_artifact") == "module.object-backend.txt", display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-BACKEND", "backend artifact name mismatch", failures)
    checks_total += require(registration_descriptor.get("object_format") in {"coff", "elf", "mach-o"}, display_path(registration_descriptor_path), "M263-A002-DESCRIPTOR-FORMAT", "object format must be one of coff/elf/mach-o", failures)

    checks_total += require(registration_manifest.get("registration_descriptor_source_contract_id") == SOURCE_SURFACE_CONTRACT_ID, display_path(registration_manifest_path), "M263-A002-MANIFEST-SOURCE-ID", "registration manifest source contract id mismatch", failures)
    checks_total += require(registration_manifest.get("registration_descriptor_source_surface_path") == "frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_image_root_source_surface", display_path(registration_manifest_path), "M263-A002-MANIFEST-SOURCE-PATH", "registration manifest source surface path mismatch", failures)
    checks_total += require(registration_manifest.get("registration_descriptor_identifier") == expected_registration_descriptor, display_path(registration_manifest_path), "M263-A002-MANIFEST-REG-ID", "registration manifest registration descriptor mismatch", failures)
    checks_total += require(registration_manifest.get("registration_descriptor_identity_source") == expected_registration_source, display_path(registration_manifest_path), "M263-A002-MANIFEST-REG-SOURCE", "registration manifest registration source mismatch", failures)
    checks_total += require(registration_manifest.get("image_root_identifier") == expected_image_root, display_path(registration_manifest_path), "M263-A002-MANIFEST-ROOT-ID", "registration manifest image-root mismatch", failures)
    checks_total += require(registration_manifest.get("image_root_identity_source") == expected_image_root_source, display_path(registration_manifest_path), "M263-A002-MANIFEST-ROOT-SOURCE", "registration manifest image-root source mismatch", failures)
    checks_total += require(registration_manifest.get("bootstrap_visible_metadata_ownership_model") == BOOTSTRAP_OWNERSHIP_MODEL, display_path(registration_manifest_path), "M263-A002-MANIFEST-OWNERSHIP", "registration manifest bootstrap ownership mismatch", failures)

    return checks_total, failures, {
        "fixture": display_path(fixture),
        "module_name": expected_module,
        "backend": backend_text,
        "registration_descriptor_identifier": registration_descriptor.get("registration_descriptor_identifier"),
        "registration_descriptor_identity_source": registration_descriptor.get("registration_descriptor_identity_source"),
        "image_root_identifier": registration_descriptor.get("image_root_identifier"),
        "image_root_identity_source": registration_descriptor.get("image_root_identity_source"),
        "artifact": registration_descriptor.get("artifact"),
        "surface_path": surface.get("frontend_surface_path"),
    }


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding], int]:
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
        explicit_checks, explicit_failures, explicit_summary = compile_fixture(
            fixture=EXPLICIT_FIXTURE,
            out_dir=PROBE_ROOT / "explicit",
            expected_module="DescriptorFrontendExplicit",
            expected_registration_descriptor="FrontendDescriptorRegistration",
            expected_image_root="FrontendDescriptorImageRoot",
            expected_registration_source=PRAGMA_IDENTITY_SOURCE,
            expected_image_root_source=PRAGMA_IDENTITY_SOURCE,
        )
        default_checks, default_failures, default_summary = compile_fixture(
            fixture=DEFAULT_FIXTURE,
            out_dir=PROBE_ROOT / "default",
            expected_module="DescriptorFrontendAuto",
            expected_registration_descriptor="DescriptorFrontendAuto_registration_descriptor",
            expected_image_root="DescriptorFrontendAuto_image_root",
            expected_registration_source=DEFAULT_IDENTITY_SOURCE,
            expected_image_root_source=DEFAULT_IDENTITY_SOURCE,
        )
        checks_total += explicit_checks + default_checks
        failures.extend(explicit_failures)
        failures.extend(default_failures)
        dynamic_summary.update(
            {
                "skipped": False,
                "explicit": explicit_summary,
                "default": default_summary,
            }
        )

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    return summary, failures, checks_total


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true", help="only validate static contract anchors")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT, help="path to write the JSON summary")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    summary, failures, _checks_total = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.check_id} @ {failure.artifact}: {failure.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M263-A002 contract validated; summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
