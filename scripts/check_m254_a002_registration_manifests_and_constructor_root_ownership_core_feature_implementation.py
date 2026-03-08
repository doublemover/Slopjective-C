#!/usr/bin/env python3
"""Fail-closed contract checker for M254-A002 registration manifests and constructor-root ownership."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m254-a002-registration-manifests-and-constructor-root-ownership-"
    "core-feature-implementation-v1"
)
CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
TRANSLATION_UNIT_REGISTRATION_CONTRACT_ID = (
    "objc3c-translation-unit-registration-surface-freeze/m254-a001-v1"
)
RUNTIME_SUPPORT_LINK_WIRING_CONTRACT_ID = (
    "objc3c-runtime-support-library-link-wiring/m251-d003-v1"
)
SURFACE_PATH = (
    "frontend.pipeline.semantic_surface."
    "objc_runtime_translation_unit_registration_manifest"
)
PAYLOAD_MODEL = "translation-unit-registration-manifest-json-v1"
MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
PAYLOAD_ARTIFACT = "module.runtime-metadata.bin"
LINKER_RESPONSE_ARTIFACT = "module.runtime-metadata-linker-options.rsp"
DISCOVERY_ARTIFACT = "module.runtime-metadata-discovery.json"
RUNTIME_SUPPORT_LIBRARY_ARCHIVE = "artifacts/lib/objc3_runtime.lib"
CONSTRUCTOR_ROOT_SYMBOL = "__objc3_runtime_register_image_ctor"
CONSTRUCTOR_ROOT_OWNERSHIP_MODEL = (
    "compiler-emits-constructor-root-runtime-owns-registration-state"
)
MANIFEST_AUTHORITY_MODEL = (
    "registration-manifest-authoritative-for-constructor-root-shape"
)
INIT_STUB_SYMBOL_PREFIX = "__objc3_runtime_register_image_init_stub_"
INIT_STUB_OWNERSHIP_MODEL = (
    "lowering-emits-init-stub-from-registration-manifest"
)
CONSTRUCTOR_PRIORITY_POLICY = "deferred-until-m254-c001"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_registration_manifests_and_constructor_root_ownership_core_feature_implementation_a002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_MANIFEST_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
DEFAULT_MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "a002-registration-manifests-and-constructor-root-ownership"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m254/M254-A002/"
    "registration_manifests_and_constructor_root_ownership_summary.json"
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
        "M254-A002-DOC-EXP-01",
        "# M254 Registration Manifests and Constructor-Root Ownership Core Feature Implementation Expectations (A002)",
    ),
    SnippetCheck("M254-A002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-A002-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-A002-DOC-EXP-04", f"`{MANIFEST_ARTIFACT}`"),
    SnippetCheck("M254-A002-DOC-EXP-05", f"`{MANIFEST_AUTHORITY_MODEL}`"),
    SnippetCheck("M254-A002-DOC-EXP-06", f"`{INIT_STUB_OWNERSHIP_MODEL}`"),
    SnippetCheck("M254-A002-DOC-EXP-07", "`M254-C001`"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M254-A002-DOC-PKT-01",
        "# M254-A002 Registration Manifests and Constructor-Root Ownership Core Feature Implementation Packet",
    ),
    SnippetCheck("M254-A002-DOC-PKT-02", "Packet: `M254-A002`"),
    SnippetCheck("M254-A002-DOC-PKT-03", "Dependencies: `M254-A001`"),
    SnippetCheck("M254-A002-DOC-PKT-04", f"- Contract id `{CONTRACT_ID}`"),
    SnippetCheck("M254-A002-DOC-PKT-05", f"`{MANIFEST_ARTIFACT}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck(
        "M254-A002-ARCH-01",
        "M254 lane-A A002 registration-manifest implementation publishes",
    ),
    SnippetCheck(
        "M254-A002-ARCH-02",
        "`module.runtime-registration-manifest.json`",
    ),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M254-A002-NDOC-01",
        "## Registration manifests and constructor-root ownership (M254-A002)",
    ),
    SnippetCheck("M254-A002-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-A002-NDOC-03", f"`{MANIFEST_ARTIFACT}`"),
    SnippetCheck("M254-A002-NDOC-04", f"`{INIT_STUB_SYMBOL_PREFIX}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-A002-SPC-01",
        "## M254 translation-unit registration manifest implementation (A002)",
    ),
    SnippetCheck("M254-A002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-A002-SPC-03", f"`{MANIFEST_ARTIFACT}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-A002-META-01",
        "## M254 translation-unit registration manifest metadata anchors (A002)",
    ),
    SnippetCheck("M254-A002-META-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-A002-META-03", f"`{RUNTIME_SUPPORT_LIBRARY_ARCHIVE}`"),
)
AST_SNIPPETS = (
    SnippetCheck(
        "M254-A002-AST-01",
        "kObjc3RuntimeTranslationUnitRegistrationManifestContractId",
    ),
    SnippetCheck(
        "M254-A002-AST-02",
        "kObjc3RuntimeTranslationUnitRegistrationManifestSurfacePath",
    ),
    SnippetCheck(
        "M254-A002-AST-03",
        "kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix",
    ),
    SnippetCheck(
        "M254-A002-AST-04",
        "kObjc3RuntimeTranslationUnitRegistrationManifestPriorityPolicy",
    ),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck(
        "M254-A002-TYPES-01",
        "struct Objc3RuntimeTranslationUnitRegistrationManifestSummary {",
    ),
    SnippetCheck(
        "M254-A002-TYPES-02",
        "std::string manifest_artifact_relative_path =",
    ),
    SnippetCheck(
        "M254-A002-TYPES-03",
        "std::string constructor_init_stub_symbol_prefix =",
    ),
    SnippetCheck(
        "M254-A002-TYPES-04",
        "inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(",
    ),
)
FRONTEND_ARTIFACTS_HEADER_SNIPPETS = (
    SnippetCheck(
        "M254-A002-ARTH-01",
        "Objc3RuntimeTranslationUnitRegistrationManifestSummary",
    ),
    SnippetCheck(
        "M254-A002-ARTH-02",
        "runtime_translation_unit_registration_manifest_summary;",
    ),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck(
        "M254-A002-ART-01",
        "BuildRuntimeTranslationUnitRegistrationManifestSummary(",
    ),
    SnippetCheck(
        "M254-A002-ART-02",
        "BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(",
    ),
    SnippetCheck(
        "M254-A002-ART-03",
        "objc_runtime_translation_unit_registration_manifest",
    ),
    SnippetCheck(
        "M254-A002-ART-04",
        "runtime_translation_unit_registration_manifest_contract_id",
    ),
)
MANIFEST_ARTIFACTS_HEADER_SNIPPETS = (
    SnippetCheck(
        "M254-A002-MANH-01",
        "BuildRuntimeRegistrationManifestArtifactPath(",
    ),
    SnippetCheck(
        "M254-A002-MANH-02",
        "WriteRuntimeRegistrationManifestArtifact(",
    ),
)
MANIFEST_ARTIFACTS_CPP_SNIPPETS = (
    SnippetCheck(
        "M254-A002-MANC-01",
        "BuildRuntimeRegistrationManifestArtifactPath(",
    ),
    SnippetCheck(
        "M254-A002-MANC-02",
        "kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix",
    ),
    SnippetCheck(
        "M254-A002-MANC-03",
        "WriteRuntimeRegistrationManifestArtifact(",
    ),
)
DRIVER_SNIPPETS = (
    SnippetCheck(
        "M254-A002-DRV-01",
        "M254-A002 registration-manifest anchor",
    ),
    SnippetCheck(
        "M254-A002-DRV-02",
        "TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(",
    ),
    SnippetCheck(
        "M254-A002-DRV-03",
        "WriteRuntimeRegistrationManifestArtifact(",
    ),
)
PROCESS_HEADER_SNIPPETS = (
    SnippetCheck(
        "M254-A002-PROCH-01",
        "struct Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs {",
    ),
    SnippetCheck(
        "M254-A002-PROCH-02",
        "TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(",
    ),
    SnippetCheck(
        "M254-A002-PROCH-03",
        "struct Objc3RuntimeMetadataLinkerRetentionArtifacts {",
    ),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-A002-PROC-01", "MakeIdentifierSafeSuffix("),
    SnippetCheck(
        "M254-A002-PROC-02",
        "TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(",
    ),
    SnippetCheck(
        "M254-A002-PROC-03",
        'ready_for_lowering_init_stub_emission\\": true',
    ),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck(
        "M254-A002-FRONT-01",
        "runtime_translation_unit_registration_manifest_summary",
    ),
    SnippetCheck(
        "M254-A002-FRONT-02",
        "TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(",
    ),
    SnippetCheck(
        "M254-A002-FRONT-03",
        "BuildRuntimeRegistrationManifestArtifactPath(",
    ),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck(
        "M254-A002-RUN-01",
        "`M254-A002` now emits one real `module.runtime-registration-manifest.json`",
    ),
    SnippetCheck("M254-A002-RUN-02", f"`{MANIFEST_AUTHORITY_MODEL}`"),
    SnippetCheck("M254-A002-RUN-03", f"`{INIT_STUB_OWNERSHIP_MODEL}`"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M254-A002-FIX-01", "module Demo;"),
    SnippetCheck("M254-A002-FIX-02", "fn main() {"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M254-A002-PKG-01",
        '"check:objc3c:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation": "python scripts/check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M254-A002-PKG-02",
        '"test:tooling:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation": "python -m pytest tests/tooling/test_check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M254-A002-PKG-03",
        '"check:objc3c:m254-a002-lane-a-readiness": "npm run check:objc3c:m254-a001-lane-a-readiness && npm run check:objc3c:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation && npm run test:tooling:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation"',
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
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts-header", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_HEADER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--manifest-artifacts-header", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_HEADER)
    parser.add_argument("--manifest-artifacts-cpp", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
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


def run_native(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, check=False)


def make_identifier_safe_suffix(text: str) -> str:
    suffix = []
    for char in text:
        if char.isalnum() or char == "_":
            suffix.append(char)
        else:
            suffix.append("_")
    return "".join(suffix) if suffix else "translation_unit"


def run_manifest_case(*, native_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M254-A002-FIXTURE-EXISTS", "fixture is missing", findings)
    checks_total += require(native_exe.exists(), display_path(native_exe), "M254-A002-NATIVE-EXISTS", "native executable is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(native_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    completed = run_native(command)

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / MANIFEST_ARTIFACT
    payload_path = out_dir / PAYLOAD_ARTIFACT
    linker_response_path = out_dir / LINKER_RESPONSE_ARTIFACT
    discovery_path = out_dir / DISCOVERY_ARTIFACT
    object_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += require(completed.returncode == 0, display_path(out_dir), "M254-A002-NATIVE-EXIT", "native compile must succeed", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M254-A002-MANIFEST", "manifest is missing", findings)
    checks_total += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M254-A002-REG-MANIFEST", "registration manifest is missing", findings)
    checks_total += require(payload_path.exists(), display_path(payload_path), "M254-A002-PAYLOAD", "runtime metadata binary is missing", findings)
    checks_total += require(linker_response_path.exists(), display_path(linker_response_path), "M254-A002-LINKER-RSP", "linker-response artifact is missing", findings)
    checks_total += require(discovery_path.exists(), display_path(discovery_path), "M254-A002-DISCOVERY", "discovery artifact is missing", findings)
    checks_total += require(object_path.exists(), display_path(object_path), "M254-A002-OBJECT", "object artifact is missing", findings)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M254-A002-BACKEND", "backend marker is missing", findings)
    if findings:
        return checks_total, findings, None

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    discovery_payload = load_json(discovery_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    linker_response_text = linker_response_path.read_text(encoding="utf-8").strip()

    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    sema_pass_manager = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    registration = semantic_surface.get("objc_runtime_translation_unit_registration_manifest") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(registration, dict), display_path(manifest_path), "M254-A002-SURFACE-EXISTS", "registration-manifest summary missing from semantic surface", findings)
    checks_total += require(isinstance(sema_pass_manager, dict), display_path(manifest_path), "M254-A002-FLAT-EXISTS", "flattened sema pass manager summary missing", findings)
    if findings:
        return checks_total, findings, None

    expected_payload_artifacts = [PAYLOAD_ARTIFACT, LINKER_RESPONSE_ARTIFACT, DISCOVERY_ARTIFACT]
    checks_total += require(registration.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-A002-CONTRACT", "contract id mismatch", findings)
    checks_total += require(registration.get("translation_unit_registration_contract_id") == TRANSLATION_UNIT_REGISTRATION_CONTRACT_ID, display_path(manifest_path), "M254-A002-UPSTREAM-CONTRACT", "upstream registration contract mismatch", findings)
    checks_total += require(registration.get("runtime_support_library_link_wiring_contract_id") == RUNTIME_SUPPORT_LINK_WIRING_CONTRACT_ID, display_path(manifest_path), "M254-A002-RUNTIME-CONTRACT", "runtime support link-wiring contract mismatch", findings)
    checks_total += require(registration.get("manifest_surface_path") == SURFACE_PATH, display_path(manifest_path), "M254-A002-SURFACE-PATH", "surface path mismatch", findings)
    checks_total += require(registration.get("manifest_payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M254-A002-PAYLOAD-MODEL", "payload model mismatch", findings)
    checks_total += require(registration.get("manifest_artifact_relative_path") == MANIFEST_ARTIFACT, display_path(manifest_path), "M254-A002-ARTIFACT-PATH", "manifest artifact path mismatch", findings)
    checks_total += require(registration.get("runtime_owned_payload_artifact_count") == 3, display_path(manifest_path), "M254-A002-PAYLOAD-COUNT", "payload artifact count mismatch", findings)
    checks_total += require(registration.get("runtime_owned_payload_artifacts") == expected_payload_artifacts, display_path(manifest_path), "M254-A002-PAYLOAD-LIST", "payload artifact list mismatch", findings)
    checks_total += require(registration.get("runtime_support_library_archive_relative_path") == RUNTIME_SUPPORT_LIBRARY_ARCHIVE, display_path(manifest_path), "M254-A002-ARCHIVE", "runtime archive path mismatch", findings)
    checks_total += require(registration.get("constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(manifest_path), "M254-A002-CONSTRUCTOR-SYMBOL", "constructor-root symbol mismatch", findings)
    checks_total += require(registration.get("constructor_root_ownership_model") == CONSTRUCTOR_ROOT_OWNERSHIP_MODEL, display_path(manifest_path), "M254-A002-CONSTRUCTOR-OWNERSHIP", "constructor-root ownership mismatch", findings)
    checks_total += require(registration.get("manifest_authority_model") == MANIFEST_AUTHORITY_MODEL, display_path(manifest_path), "M254-A002-AUTHORITY", "manifest authority model mismatch", findings)
    checks_total += require(registration.get("constructor_init_stub_symbol_prefix") == INIT_STUB_SYMBOL_PREFIX, display_path(manifest_path), "M254-A002-INIT-PREFIX", "init-stub symbol prefix mismatch", findings)
    checks_total += require(registration.get("constructor_init_stub_ownership_model") == INIT_STUB_OWNERSHIP_MODEL, display_path(manifest_path), "M254-A002-INIT-OWNERSHIP", "init-stub ownership model mismatch", findings)
    checks_total += require(registration.get("constructor_priority_policy") == CONSTRUCTOR_PRIORITY_POLICY, display_path(manifest_path), "M254-A002-CONSTRUCTOR-PRIORITY", "constructor priority policy mismatch", findings)
    checks_total += require(registration.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(manifest_path), "M254-A002-ENTRYPOINT", "registration entrypoint mismatch", findings)
    checks_total += require(registration.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(manifest_path), "M254-A002-TU-MODEL", "translation-unit identity model mismatch", findings)
    checks_total += require(registration.get("ready") is True, display_path(manifest_path), "M254-A002-READY", "registration-manifest surface must be ready", findings)
    checks_total += require(registration.get("fail_closed") is True, display_path(manifest_path), "M254-A002-FAIL-CLOSED", "registration-manifest surface must be fail-closed", findings)
    checks_total += require(registration.get("translation_unit_registration_contract_ready") is True, display_path(manifest_path), "M254-A002-UPSTREAM-READY", "upstream registration surface must be ready", findings)
    checks_total += require(registration.get("runtime_support_library_link_wiring_ready") is True, display_path(manifest_path), "M254-A002-RUNTIME-READY", "runtime support link wiring must be ready", findings)
    checks_total += require(registration.get("runtime_manifest_template_published") is True, display_path(manifest_path), "M254-A002-TEMPLATE", "manifest template flag must be true", findings)
    checks_total += require(registration.get("constructor_root_manifest_authoritative") is True, display_path(manifest_path), "M254-A002-CONSTRUCTOR-AUTH", "constructor-root authority flag must be true", findings)
    checks_total += require(registration.get("constructor_root_reserved_for_lowering") is True, display_path(manifest_path), "M254-A002-CONSTRUCTOR-LOWERING", "constructor-root reserved-for-lowering flag must be true", findings)
    checks_total += require(registration.get("init_stub_emission_deferred_to_lowering") is True, display_path(manifest_path), "M254-A002-INIT-DEFERRED", "init-stub emission must remain deferred to lowering", findings)
    checks_total += require(registration.get("runtime_registration_artifact_emitted_by_driver") is True, display_path(manifest_path), "M254-A002-DRIVER-EMITS", "driver-emitted manifest flag must be true", findings)
    checks_total += require(registration.get("ready_for_lowering_init_stub_emission") is True, display_path(manifest_path), "M254-A002-READY-FOR-C001", "ready-for-lowering-init-stub-emission must be true", findings)
    checks_total += require(bool(registration.get("translation_unit_registration_replay_key")), display_path(manifest_path), "M254-A002-UPSTREAM-REPLAY", "upstream replay key must be non-empty", findings)
    checks_total += require(bool(registration.get("replay_key")), display_path(manifest_path), "M254-A002-REPLAY", "manifest replay key must be non-empty", findings)
    checks_total += require(registration.get("failure_reason") == "", display_path(manifest_path), "M254-A002-FAILURE-REASON", "failure reason must be empty", findings)

    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-A002-FLAT-CONTRACT", "flattened manifest contract id mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M254-A002-FLAT-PAYLOAD", "flattened payload model mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_artifact_relative_path") == MANIFEST_ARTIFACT, display_path(manifest_path), "M254-A002-FLAT-ARTIFACT", "flattened manifest artifact path mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(manifest_path), "M254-A002-FLAT-CONSTRUCTOR", "flattened constructor-root symbol mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_authority_model") == MANIFEST_AUTHORITY_MODEL, display_path(manifest_path), "M254-A002-FLAT-AUTHORITY", "flattened authority model mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_init_stub_symbol_prefix") == INIT_STUB_SYMBOL_PREFIX, display_path(manifest_path), "M254-A002-FLAT-PREFIX", "flattened init-stub prefix mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_init_stub_ownership_model") == INIT_STUB_OWNERSHIP_MODEL, display_path(manifest_path), "M254-A002-FLAT-INIT-OWNERSHIP", "flattened init-stub ownership mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_manifest_ready_for_lowering_init_stub_emission") is True, display_path(manifest_path), "M254-A002-FLAT-READY", "flattened ready-for-lowering flag mismatch", findings)

    checks_total += require(registration_manifest_payload.get("contract_id") == CONTRACT_ID, display_path(registration_manifest_path), "M254-A002-REGFILE-CONTRACT", "registration-manifest artifact contract mismatch", findings)
    checks_total += require(registration_manifest_payload.get("translation_unit_registration_contract_id") == TRANSLATION_UNIT_REGISTRATION_CONTRACT_ID, display_path(registration_manifest_path), "M254-A002-REGFILE-UPSTREAM", "registration-manifest artifact upstream contract mismatch", findings)
    checks_total += require(registration_manifest_payload.get("runtime_support_library_link_wiring_contract_id") == RUNTIME_SUPPORT_LINK_WIRING_CONTRACT_ID, display_path(registration_manifest_path), "M254-A002-REGFILE-RUNTIME", "registration-manifest artifact runtime contract mismatch", findings)
    checks_total += require(registration_manifest_payload.get("manifest_payload_model") == PAYLOAD_MODEL, display_path(registration_manifest_path), "M254-A002-REGFILE-PAYLOAD", "registration-manifest artifact payload model mismatch", findings)
    checks_total += require(registration_manifest_payload.get("manifest_artifact") == MANIFEST_ARTIFACT, display_path(registration_manifest_path), "M254-A002-REGFILE-ARTIFACT", "registration-manifest artifact path mismatch", findings)
    checks_total += require(registration_manifest_payload.get("object_artifact") == "module.obj", display_path(registration_manifest_path), "M254-A002-REGFILE-OBJECT", "registration-manifest object artifact mismatch", findings)
    checks_total += require(registration_manifest_payload.get("backend_artifact") == "module.object-backend.txt", display_path(registration_manifest_path), "M254-A002-REGFILE-BACKEND", "registration-manifest backend artifact mismatch", findings)
    checks_total += require(registration_manifest_payload.get("runtime_owned_payload_artifacts") == expected_payload_artifacts, display_path(registration_manifest_path), "M254-A002-REGFILE-PAYLOAD-LIST", "registration-manifest payload artifact list mismatch", findings)
    metadata_binary_size = registration_manifest_payload.get("runtime_metadata_binary_byte_count")
    checks_total += require(isinstance(metadata_binary_size, int) and metadata_binary_size > 0, display_path(registration_manifest_path), "M254-A002-REGFILE-BYTE-COUNT", "runtime metadata binary byte count must be positive", findings)
    checks_total += require(registration_manifest_payload.get("runtime_support_library_archive_relative_path") == RUNTIME_SUPPORT_LIBRARY_ARCHIVE, display_path(registration_manifest_path), "M254-A002-REGFILE-ARCHIVE", "registration-manifest runtime archive mismatch", findings)
    checks_total += require(registration_manifest_payload.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(registration_manifest_path), "M254-A002-REGFILE-ENTRYPOINT", "registration-manifest entrypoint mismatch", findings)
    checks_total += require(registration_manifest_payload.get("constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(registration_manifest_path), "M254-A002-REGFILE-CONSTRUCTOR", "registration-manifest constructor-root mismatch", findings)
    checks_total += require(registration_manifest_payload.get("constructor_root_ownership_model") == CONSTRUCTOR_ROOT_OWNERSHIP_MODEL, display_path(registration_manifest_path), "M254-A002-REGFILE-OWNERSHIP", "registration-manifest constructor ownership mismatch", findings)
    checks_total += require(registration_manifest_payload.get("manifest_authority_model") == MANIFEST_AUTHORITY_MODEL, display_path(registration_manifest_path), "M254-A002-REGFILE-AUTHORITY", "registration-manifest authority model mismatch", findings)
    checks_total += require(registration_manifest_payload.get("constructor_init_stub_ownership_model") == INIT_STUB_OWNERSHIP_MODEL, display_path(registration_manifest_path), "M254-A002-REGFILE-INIT-OWNERSHIP", "registration-manifest init-stub ownership mismatch", findings)
    checks_total += require(registration_manifest_payload.get("constructor_priority_policy") == CONSTRUCTOR_PRIORITY_POLICY, display_path(registration_manifest_path), "M254-A002-REGFILE-PRIORITY", "registration-manifest constructor priority mismatch", findings)
    checks_total += require(registration_manifest_payload.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(registration_manifest_path), "M254-A002-REGFILE-TU-MODEL", "registration-manifest identity model mismatch", findings)
    checks_total += require(registration_manifest_payload.get("translation_unit_identity_key") == discovery_payload.get("translation_unit_identity_key"), display_path(registration_manifest_path), "M254-A002-REGFILE-TU-KEY", "registration-manifest identity key must match discovery artifact", findings)
    checks_total += require(registration_manifest_payload.get("object_format") == discovery_payload.get("object_format"), display_path(registration_manifest_path), "M254-A002-REGFILE-FORMAT", "registration-manifest object format mismatch", findings)
    checks_total += require(registration_manifest_payload.get("linker_anchor_symbol") == discovery_payload.get("linker_anchor_symbol"), display_path(registration_manifest_path), "M254-A002-REGFILE-LINKER-ANCHOR", "registration-manifest linker-anchor symbol mismatch", findings)
    checks_total += require(registration_manifest_payload.get("discovery_root_symbol") == discovery_payload.get("discovery_root_symbol"), display_path(registration_manifest_path), "M254-A002-REGFILE-DISCOVERY-ROOT", "registration-manifest discovery-root symbol mismatch", findings)
    driver_flags = discovery_payload.get("driver_linker_flags")
    reg_driver_flags = registration_manifest_payload.get("driver_linker_flags")
    checks_total += require(isinstance(driver_flags, list) and len(driver_flags) == 1 and isinstance(driver_flags[0], str), display_path(discovery_path), "M254-A002-DISCOVERY-FLAGS", "discovery linker flags must contain one string flag", findings)
    checks_total += require(reg_driver_flags == driver_flags, display_path(registration_manifest_path), "M254-A002-REGFILE-FLAGS", "registration-manifest driver linker flags must match discovery artifact", findings)
    if isinstance(driver_flags, list) and driver_flags and isinstance(driver_flags[0], str):
        checks_total += require(driver_flags[0] in linker_response_text, display_path(linker_response_path), "M254-A002-RSP-FLAG", "linker response must contain discovery linker flag", findings)

    init_stub_symbol = registration_manifest_payload.get("constructor_init_stub_symbol")
    expected_init_stub_symbol = INIT_STUB_SYMBOL_PREFIX + make_identifier_safe_suffix(str(discovery_payload.get("translation_unit_identity_key", "")))
    checks_total += require(isinstance(init_stub_symbol, str) and init_stub_symbol.startswith(INIT_STUB_SYMBOL_PREFIX), display_path(registration_manifest_path), "M254-A002-REGFILE-INIT-SYMBOL-PREFIX", "registration-manifest init-stub symbol must use the canonical prefix", findings)
    checks_total += require(init_stub_symbol == expected_init_stub_symbol, display_path(registration_manifest_path), "M254-A002-REGFILE-INIT-SYMBOL", "registration-manifest init-stub symbol must be derived from the translation-unit identity key", findings)
    checks_total += require(registration_manifest_payload.get("ready_for_lowering_init_stub_emission") is True, display_path(registration_manifest_path), "M254-A002-REGFILE-READY", "registration-manifest ready-for-lowering flag must be true", findings)
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M254-A002-BACKEND-TEXT", "backend marker must remain llvm-direct", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
        "payload_artifact": display_path(payload_path),
        "linker_response_artifact": display_path(linker_response_path),
        "discovery_artifact": display_path(discovery_path),
        "backend": backend_text,
        "translation_unit_identity_key": discovery_payload.get("translation_unit_identity_key"),
        "constructor_init_stub_symbol": init_stub_symbol,
        "runtime_support_library_archive_relative_path": registration_manifest_payload.get("runtime_support_library_archive_relative_path"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    dynamic_cases: list[dict[str, object]] = []

    doc_checks: tuple[tuple[Path, str, tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, "M254-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M254-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M254-A002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M254-A002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M254-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M254-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M254-A002-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M254-A002-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts_header, "M254-A002-ARTH-EXISTS", FRONTEND_ARTIFACTS_HEADER_SNIPPETS),
        (args.frontend_artifacts, "M254-A002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.manifest_artifacts_header, "M254-A002-MANH-EXISTS", MANIFEST_ARTIFACTS_HEADER_SNIPPETS),
        (args.manifest_artifacts_cpp, "M254-A002-MANC-EXISTS", MANIFEST_ARTIFACTS_CPP_SNIPPETS),
        (args.driver_cpp, "M254-A002-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.process_header, "M254-A002-PROCH-EXISTS", PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, "M254-A002-PROC-EXISTS", PROCESS_CPP_SNIPPETS),
        (args.frontend_anchor_cpp, "M254-A002-FRONT-EXISTS", FRONTEND_ANCHOR_SNIPPETS),
        (args.runtime_readme, "M254-A002-RUN-EXISTS", RUNTIME_README_SNIPPETS),
        (args.fixture, "M254-A002-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.package_json, "M254-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        added_checks, added_failures = check_doc_contract(
            path=path, exists_check_id=exists_check_id, snippets=snippets
        )
        checks_total += added_checks
        failures.extend(added_failures)

    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        case_checks, case_failures, case_payload = run_manifest_case(
            native_exe=args.native_exe.resolve(),
            fixture_path=args.fixture.resolve(),
            out_dir=args.probe_root.resolve() / "hello-registration-manifest",
        )
        checks_total += case_checks
        failures.extend(case_failures)
        if case_payload is not None:
            dynamic_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases,
        "next_implementation_issue": "M254-B001",
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(
            f"[FAIL] M254-A002 registration-manifest drift; summary: {display_path(summary_out)}",
            file=sys.stderr,
        )
        return 1
    print(
        f"[PASS] M254-A002 registration-manifest contract preserved; summary: {display_path(summary_out)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
