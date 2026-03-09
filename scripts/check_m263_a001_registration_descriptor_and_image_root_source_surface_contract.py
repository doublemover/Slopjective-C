#!/usr/bin/env python3
"""Fail-closed checker for M263-A001 registration descriptor/image-root source surface."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-a001-registration-descriptor-image-root-source-surface-contract-v1"
CONTRACT_ID = "objc3c-bootstrap-registration-descriptor-image-root-source-surface/m263-a001-v1"
REGISTRATION_MANIFEST_CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_image_root_source_surface"
PRAGMA_CONTRACT_PATH = "frontend.bootstrap_registration_source_pragma_contract"
REGISTRATION_DESCRIPTOR_PRAGMA = "objc_registration_descriptor"
IMAGE_ROOT_PRAGMA = "objc_image_root"
MODULE_IDENTITY_SOURCE = "module-declaration-or-default"
PRAGMA_IDENTITY_SOURCE = "source-pragma"
DEFAULT_IDENTITY_SOURCE = "module-derived-default"
OWNERSHIP_MODEL = "image-root-owns-registration-descriptor-runtime-owns-bootstrap-state"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-A001" / "registration_descriptor_image_root_source_surface_contract_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m263" / "a001-registration-descriptor-image-root-source-surface"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_registration_descriptor_and_image_root_source_surface_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_a001_registration_descriptor_and_image_root_source_surface_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m263_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py"
EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_registration_descriptor_image_root_explicit.objc3"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_registration_descriptor_image_root_default.objc3"


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
        SnippetCheck("M263-A001-DOC-EXP-01", "# M263 Registration Descriptor and Image-Root Source Surface Contract and Architecture Freeze Expectations (A001)"),
        SnippetCheck("M263-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M263-A001-DOC-EXP-03", "Issue: `#7220`"),
        SnippetCheck("M263-A001-DOC-EXP-04", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-A001-DOC-EXP-05", f"`{PRAGMA_CONTRACT_PATH}`"),
        SnippetCheck("M263-A001-DOC-EXP-06", f"`{OWNERSHIP_MODEL}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M263-A001-DOC-PKT-01", "# M263-A001 Registration Descriptor and Image-Root Source Surface Contract and Architecture Freeze Packet"),
        SnippetCheck("M263-A001-DOC-PKT-02", "Packet: `M263-A001`"),
        SnippetCheck("M263-A001-DOC-PKT-03", "Dependencies: `M259-E002`"),
        SnippetCheck("M263-A001-DOC-PKT-04", "Next issue: `M263-A002`"),
        SnippetCheck("M263-A001-DOC-PKT-05", f"`{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M263-A001-NDOC-01", "## Registration descriptor and image-root source surface (M263-A001)"),
        SnippetCheck("M263-A001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M263-A001-NDOC-03", f"`{REGISTRATION_DESCRIPTOR_PRAGMA}`"),
        SnippetCheck("M263-A001-NDOC-04", f"`{IMAGE_ROOT_PRAGMA}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M263-A001-SPC-01", "## M263 registration descriptor and image-root source surface (A001)"),
        SnippetCheck("M263-A001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M263-A001-SPC-03", f"`{DEFAULT_IDENTITY_SOURCE}`"),
        SnippetCheck("M263-A001-SPC-04", f"`{OWNERSHIP_MODEL}`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M263-A001-META-01", "## M263 registration descriptor and image-root metadata anchors (A001)"),
        SnippetCheck("M263-A001-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-A001-META-03", f"`{REGISTRATION_DESCRIPTOR_PRAGMA}`"),
        SnippetCheck("M263-A001-META-04", f"`{IMAGE_ROOT_PRAGMA}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M263-A001-ARCH-01", "M263 lane-A A001 freezes the registration-descriptor/image-root source surface"),
        SnippetCheck("M263-A001-ARCH-02", "module.runtime-registration-manifest.json"),
        SnippetCheck("M263-A001-ARCH-03", f"{OWNERSHIP_MODEL}"),
    ),
    TOKEN_HEADER: (
        SnippetCheck("M263-A001-TOKEN-01", "kObjc3BootstrapRegistrationDescriptorPragmaName"),
        SnippetCheck("M263-A001-TOKEN-02", "kObjc3BootstrapImageRootPragmaName"),
    ),
    LEXER_CPP: (
        SnippetCheck("M263-A001-LEX-01", "ConsumeBootstrapRegistrationPragmaDirective("),
        SnippetCheck("M263-A001-LEX-02", "ConsumeNamedIdentifierPragmaDirective("),
        SnippetCheck("M263-A001-LEX-03", "O3L009"),
        SnippetCheck("M263-A001-LEX-04", "O3L010"),
        SnippetCheck("M263-A001-LEX-05", "O3L011"),
    ),
    PARSER_CPP: (
        SnippetCheck("M263-A001-PARSE-01", "M263-A001 registration-descriptor/image-root source-surface anchor"),
        SnippetCheck("M263-A001-PARSE-02", "module identity remains parser-owned here"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M263-A001-TYPES-01", "struct Objc3FrontendBootstrapRegistrationSourcePragmaContract {"),
        SnippetCheck("M263-A001-TYPES-02", "struct Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary {"),
        SnippetCheck("M263-A001-TYPES-03", "registration_descriptor_identity_source"),
        SnippetCheck("M263-A001-TYPES-04", "bootstrap_visible_metadata_ownership_model"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M263-A001-ART-01", "BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummary("),
        SnippetCheck("M263-A001-ART-02", "BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson("),
        SnippetCheck("M263-A001-ART-03", "bootstrap_registration_source_pragma_contract"),
        SnippetCheck("M263-A001-ART-04", "objc_runtime_registration_descriptor_image_root_source_surface"),
    ),
    PROCESS_HEADER: (
        SnippetCheck("M263-A001-PHDR-01", "registration_descriptor_source_contract_id"),
        SnippetCheck("M263-A001-PHDR-02", "image_root_identifier"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M263-A001-PROC-01", "registration_descriptor_source_contract_id"),
        SnippetCheck("M263-A001-PROC-02", "bootstrap_visible_metadata_ownership_model"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M263-A001-DRV-01", "registration descriptor/image-root source surface not ready"),
        SnippetCheck("M263-A001-DRV-02", "runtime_registration_descriptor_image_root_source_surface_summary"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M263-A001-ANCHOR-01", "registration descriptor/image-root source surface not ready"),
        SnippetCheck("M263-A001-ANCHOR-02", "runtime_registration_descriptor_image_root_source_surface_summary"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M263-A001-PKG-01", '"check:objc3c:m263-a001-registration-descriptor-and-image-root-source-surface-contract"'),
        SnippetCheck("M263-A001-PKG-02", '"test:tooling:m263-a001-registration-descriptor-and-image-root-source-surface-contract"'),
        SnippetCheck("M263-A001-PKG-03", '"check:objc3c:m263-a001-lane-a-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M263-A001-RUN-01", "M259-E002 + M254-A002 + M254-D004"),
        SnippetCheck("M263-A001-RUN-02", "check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M263-A001-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M263-A001-TEST-02", "def test_checker_passes_dynamic"),
    ),
    EXPLICIT_FIXTURE: (
        SnippetCheck("M263-A001-FIX-EXP-01", "#pragma objc_registration_descriptor(DemoRegistrationDescriptor)"),
        SnippetCheck("M263-A001-FIX-EXP-02", "#pragma objc_image_root(DemoImageRoot)"),
        SnippetCheck("M263-A001-FIX-EXP-03", "module DemoBootstrap;"),
    ),
    DEFAULT_FIXTURE: (
        SnippetCheck("M263-A001-FIX-DEF-01", "module AutoBootstrap;"),
        SnippetCheck("M263-A001-FIX-DEF-02", "#pragma objc_language_version(3)"),
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
    expect_pragmas_seen: bool,
) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M263-A001-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(fixture.exists(), display_path(fixture), "M263-A001-FIXTURE-EXISTS", "fixture is missing", failures)
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
    backend_path = out_dir / "module.object-backend.txt"
    diagnostics_text_path = out_dir / "module.diagnostics.txt"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M263-A001-COMPILE", "fixture must compile successfully", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M263-A001-MANIFEST", "module manifest is missing", failures)
    checks_total += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M263-A001-REG-MANIFEST", "runtime registration manifest is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M263-A001-BACKEND", "backend marker is missing", failures)
    if failures:
        return checks_total, failures, {
            "fixture": display_path(fixture),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
            "diagnostics_text": diagnostics_text_path.read_text(encoding="utf-8") if diagnostics_text_path.exists() else "",
        }

    manifest = load_json(manifest_path)
    runtime_registration_manifest = load_json(registration_manifest_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()

    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    pragma_contract = frontend.get("bootstrap_registration_source_pragma_contract") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    sema_pass_manager = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    source_surface = semantic_surface.get("objc_runtime_registration_descriptor_image_root_source_surface") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(frontend, dict), display_path(manifest_path), "M263-A001-FRONTEND", "frontend payload missing", failures)
    checks_total += require(isinstance(pragma_contract, dict), display_path(manifest_path), "M263-A001-PRAGMA-CONTRACT", "bootstrap registration pragma contract missing", failures)
    checks_total += require(isinstance(source_surface, dict), display_path(manifest_path), "M263-A001-SOURCE-SURFACE", "registration descriptor/image-root source surface missing", failures)
    checks_total += require(isinstance(sema_pass_manager, dict), display_path(manifest_path), "M263-A001-FLAT-SUMMARY", "flattened sema pass manager summary missing", failures)
    if failures:
        return checks_total, failures, {
            "fixture": display_path(fixture),
            "manifest_path": display_path(manifest_path),
            "registration_manifest_path": display_path(registration_manifest_path),
            "backend": backend_text,
        }

    registration_descriptor_contract = pragma_contract.get("registration_descriptor")
    image_root_contract = pragma_contract.get("image_root")
    checks_total += require(isinstance(registration_descriptor_contract, dict), display_path(manifest_path), "M263-A001-PRAGMA-REG-DICT", "registration_descriptor contract missing", failures)
    checks_total += require(isinstance(image_root_contract, dict), display_path(manifest_path), "M263-A001-PRAGMA-ROOT-DICT", "image_root contract missing", failures)

    checks_total += require(pragma_contract.get("registration_descriptor_pragma_name") == REGISTRATION_DESCRIPTOR_PRAGMA, display_path(manifest_path), "M263-A001-PRAGMA-REG-NAME", "registration descriptor pragma name mismatch", failures)
    checks_total += require(pragma_contract.get("image_root_pragma_name") == IMAGE_ROOT_PRAGMA, display_path(manifest_path), "M263-A001-PRAGMA-ROOT-NAME", "image-root pragma name mismatch", failures)

    checks_total += require(source_surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M263-A001-CONTRACT", "source-surface contract id mismatch", failures)
    checks_total += require(source_surface.get("registration_manifest_contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(manifest_path), "M263-A001-UPSTREAM-CONTRACT", "registration-manifest contract id mismatch", failures)
    checks_total += require(source_surface.get("source_surface_path") == SURFACE_PATH, display_path(manifest_path), "M263-A001-SURFACE-PATH", "source-surface path mismatch", failures)
    checks_total += require(source_surface.get("registration_descriptor_pragma_name") == REGISTRATION_DESCRIPTOR_PRAGMA, display_path(manifest_path), "M263-A001-SURFACE-REG-PRAGMA", "source-surface registration descriptor pragma mismatch", failures)
    checks_total += require(source_surface.get("image_root_pragma_name") == IMAGE_ROOT_PRAGMA, display_path(manifest_path), "M263-A001-SURFACE-ROOT-PRAGMA", "source-surface image-root pragma mismatch", failures)
    checks_total += require(source_surface.get("module_identity_source") == MODULE_IDENTITY_SOURCE, display_path(manifest_path), "M263-A001-MODULE-SOURCE", "module identity source mismatch", failures)
    checks_total += require(source_surface.get("registration_descriptor_identity_source") == expected_registration_source, display_path(manifest_path), "M263-A001-REG-SOURCE", "registration descriptor identity source mismatch", failures)
    checks_total += require(source_surface.get("image_root_identity_source") == expected_image_root_source, display_path(manifest_path), "M263-A001-ROOT-SOURCE", "image-root identity source mismatch", failures)
    checks_total += require(source_surface.get("bootstrap_visible_metadata_ownership_model") == OWNERSHIP_MODEL, display_path(manifest_path), "M263-A001-OWNERSHIP", "ownership model mismatch", failures)
    checks_total += require(source_surface.get("module_name") == expected_module, display_path(manifest_path), "M263-A001-MODULE-NAME", "module name mismatch", failures)
    checks_total += require(source_surface.get("registration_descriptor_identifier") == expected_registration_descriptor, display_path(manifest_path), "M263-A001-REG-ID", "registration descriptor identifier mismatch", failures)
    checks_total += require(source_surface.get("image_root_identifier") == expected_image_root, display_path(manifest_path), "M263-A001-ROOT-ID", "image-root identifier mismatch", failures)
    checks_total += require(source_surface.get("ready") is True, display_path(manifest_path), "M263-A001-READY", "source surface must be ready", failures)
    checks_total += require(source_surface.get("fail_closed") is True, display_path(manifest_path), "M263-A001-FAIL-CLOSED", "source surface must be fail-closed", failures)
    checks_total += require(source_surface.get("registration_manifest_contract_ready") is True, display_path(manifest_path), "M263-A001-MANIFEST-READY", "source surface must report registration-manifest readiness", failures)
    checks_total += require(source_surface.get("source_surface_frozen") is True, display_path(manifest_path), "M263-A001-FROZEN", "source surface must be frozen", failures)
    checks_total += require(source_surface.get("prelude_pragma_contract_published") is True, display_path(manifest_path), "M263-A001-PRELUDE-PUBLISHED", "prelude pragma contract must be published", failures)
    checks_total += require(source_surface.get("registration_descriptor_identifier_resolved") is True, display_path(manifest_path), "M263-A001-REG-RESOLVED", "registration descriptor identifier must be resolved", failures)
    checks_total += require(source_surface.get("image_root_identifier_resolved") is True, display_path(manifest_path), "M263-A001-ROOT-RESOLVED", "image-root identifier must be resolved", failures)
    checks_total += require(source_surface.get("bootstrap_visible_metadata_ownership_published") is True, display_path(manifest_path), "M263-A001-OWNERSHIP-PUBLISHED", "ownership model must be published", failures)
    checks_total += require(source_surface.get("ready_for_descriptor_frontend_closure") is True, display_path(manifest_path), "M263-A001-FRONTEND-CLOSURE", "source surface must be ready for descriptor frontend closure", failures)
    checks_total += require(bool(source_surface.get("registration_manifest_replay_key")), display_path(manifest_path), "M263-A001-UPSTREAM-REPLAY", "registration-manifest replay key must be non-empty", failures)
    checks_total += require(bool(source_surface.get("replay_key")), display_path(manifest_path), "M263-A001-REPLAY", "source-surface replay key must be non-empty", failures)
    checks_total += require(source_surface.get("failure_reason") == "", display_path(manifest_path), "M263-A001-FAILURE-REASON", "failure reason must be empty", failures)

    checks_total += require(registration_descriptor_contract.get("seen") is expect_pragmas_seen, display_path(manifest_path), "M263-A001-PRAGMA-REG-SEEN", "registration descriptor pragma seen mismatch", failures)
    checks_total += require(registration_descriptor_contract.get("directive_count") == (1 if expect_pragmas_seen else 0), display_path(manifest_path), "M263-A001-PRAGMA-REG-COUNT", "registration descriptor directive count mismatch", failures)
    checks_total += require(registration_descriptor_contract.get("duplicate") is False, display_path(manifest_path), "M263-A001-PRAGMA-REG-DUP", "registration descriptor pragma must not be duplicate", failures)
    checks_total += require(registration_descriptor_contract.get("non_leading") is False, display_path(manifest_path), "M263-A001-PRAGMA-REG-NONLEADING", "registration descriptor pragma must remain in prelude", failures)
    checks_total += require(registration_descriptor_contract.get("identifier") == (expected_registration_descriptor if expect_pragmas_seen else ""), display_path(manifest_path), "M263-A001-PRAGMA-REG-IDENT", "registration descriptor pragma identifier mismatch", failures)
    checks_total += require(image_root_contract.get("seen") is expect_pragmas_seen, display_path(manifest_path), "M263-A001-PRAGMA-ROOT-SEEN", "image-root pragma seen mismatch", failures)
    checks_total += require(image_root_contract.get("directive_count") == (1 if expect_pragmas_seen else 0), display_path(manifest_path), "M263-A001-PRAGMA-ROOT-COUNT", "image-root directive count mismatch", failures)
    checks_total += require(image_root_contract.get("duplicate") is False, display_path(manifest_path), "M263-A001-PRAGMA-ROOT-DUP", "image-root pragma must not be duplicate", failures)
    checks_total += require(image_root_contract.get("non_leading") is False, display_path(manifest_path), "M263-A001-PRAGMA-ROOT-NONLEADING", "image-root pragma must remain in prelude", failures)
    checks_total += require(image_root_contract.get("identifier") == (expected_image_root if expect_pragmas_seen else ""), display_path(manifest_path), "M263-A001-PRAGMA-ROOT-IDENT", "image-root pragma identifier mismatch", failures)

    checks_total += require(sema_pass_manager.get("runtime_registration_descriptor_image_root_source_surface_contract_id") == CONTRACT_ID, display_path(manifest_path), "M263-A001-FLAT-CONTRACT", "flattened contract id mismatch", failures)
    checks_total += require(sema_pass_manager.get("runtime_registration_descriptor_image_root_source_surface_path") == SURFACE_PATH, display_path(manifest_path), "M263-A001-FLAT-PATH", "flattened path mismatch", failures)
    checks_total += require(sema_pass_manager.get("runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier") == expected_registration_descriptor, display_path(manifest_path), "M263-A001-FLAT-REG-ID", "flattened registration descriptor identifier mismatch", failures)
    checks_total += require(sema_pass_manager.get("runtime_registration_descriptor_image_root_source_surface_image_root_identifier") == expected_image_root, display_path(manifest_path), "M263-A001-FLAT-ROOT-ID", "flattened image-root identifier mismatch", failures)
    checks_total += require(sema_pass_manager.get("runtime_registration_descriptor_image_root_source_surface_ready_for_descriptor_frontend_closure") is True, display_path(manifest_path), "M263-A001-FLAT-READY", "flattened ready flag mismatch", failures)

    checks_total += require(runtime_registration_manifest.get("registration_descriptor_source_contract_id") == CONTRACT_ID, display_path(registration_manifest_path), "M263-A001-RMAN-CONTRACT", "runtime registration manifest contract id mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("registration_descriptor_source_surface_path") == SURFACE_PATH, display_path(registration_manifest_path), "M263-A001-RMAN-PATH", "runtime registration manifest source path mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("registration_descriptor_pragma_name") == REGISTRATION_DESCRIPTOR_PRAGMA, display_path(registration_manifest_path), "M263-A001-RMAN-REG-PRAGMA", "runtime registration manifest registration descriptor pragma mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("image_root_pragma_name") == IMAGE_ROOT_PRAGMA, display_path(registration_manifest_path), "M263-A001-RMAN-ROOT-PRAGMA", "runtime registration manifest image-root pragma mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("module_identity_source") == MODULE_IDENTITY_SOURCE, display_path(registration_manifest_path), "M263-A001-RMAN-MODULE-SOURCE", "runtime registration manifest module identity source mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("registration_descriptor_identifier") == expected_registration_descriptor, display_path(registration_manifest_path), "M263-A001-RMAN-REG-ID", "runtime registration manifest registration descriptor identifier mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("registration_descriptor_identity_source") == expected_registration_source, display_path(registration_manifest_path), "M263-A001-RMAN-REG-SOURCE", "runtime registration manifest registration descriptor identity source mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("image_root_identifier") == expected_image_root, display_path(registration_manifest_path), "M263-A001-RMAN-ROOT-ID", "runtime registration manifest image-root identifier mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("image_root_identity_source") == expected_image_root_source, display_path(registration_manifest_path), "M263-A001-RMAN-ROOT-SOURCE", "runtime registration manifest image-root identity source mismatch", failures)
    checks_total += require(runtime_registration_manifest.get("bootstrap_visible_metadata_ownership_model") == OWNERSHIP_MODEL, display_path(registration_manifest_path), "M263-A001-RMAN-OWNERSHIP", "runtime registration manifest ownership model mismatch", failures)
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M263-A001-BACKEND-TEXT", "backend marker must remain llvm-direct", failures)

    payload = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "runtime_registration_manifest_path": display_path(registration_manifest_path),
        "backend": backend_text,
        "module": manifest.get("module"),
        "source_surface_replay_key": source_surface.get("replay_key"),
        "runtime_registration_identity_key": runtime_registration_manifest.get("translation_unit_identity_key"),
        "registration_descriptor_identifier": runtime_registration_manifest.get("registration_descriptor_identifier"),
        "image_root_identifier": runtime_registration_manifest.get("image_root_identifier"),
        "registration_descriptor_identity_source": runtime_registration_manifest.get("registration_descriptor_identity_source"),
        "image_root_identity_source": runtime_registration_manifest.get("image_root_identity_source"),
    }
    return checks_total, failures, payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    checks_total = 0
    failures: list[Finding] = []

    for path, snippets in STATIC_SNIPPETS.items():
        count, found = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(found)

    dynamic_payload: dict[str, object] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        explicit_checks, explicit_failures, explicit_payload = compile_fixture(
            fixture=EXPLICIT_FIXTURE,
            out_dir=PROBE_ROOT / "explicit",
            expected_module="DemoBootstrap",
            expected_registration_descriptor="DemoRegistrationDescriptor",
            expected_image_root="DemoImageRoot",
            expected_registration_source=PRAGMA_IDENTITY_SOURCE,
            expected_image_root_source=PRAGMA_IDENTITY_SOURCE,
            expect_pragmas_seen=True,
        )
        checks_total += explicit_checks
        failures.extend(explicit_failures)

        default_checks, default_failures, default_payload = compile_fixture(
            fixture=DEFAULT_FIXTURE,
            out_dir=PROBE_ROOT / "default",
            expected_module="AutoBootstrap",
            expected_registration_descriptor="AutoBootstrap_registration_descriptor",
            expected_image_root="AutoBootstrap_image_root",
            expected_registration_source=DEFAULT_IDENTITY_SOURCE,
            expected_image_root_source=DEFAULT_IDENTITY_SOURCE,
            expect_pragmas_seen=False,
        )
        checks_total += default_checks
        failures.extend(default_failures)

        dynamic_payload = {
            "skipped": False,
            "explicit": explicit_payload,
            "default": default_payload,
        }

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "registration_manifest_contract_id": REGISTRATION_MANIFEST_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "surface_path": SURFACE_PATH,
        "pragma_contract_path": PRAGMA_CONTRACT_PATH,
        "module_identity_source": MODULE_IDENTITY_SOURCE,
        "pragma_identity_source": PRAGMA_IDENTITY_SOURCE,
        "default_identity_source": DEFAULT_IDENTITY_SOURCE,
        "ownership_model": OWNERSHIP_MODEL,
        "dynamic_probes": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        print(canonical_json(payload), file=sys.stderr, end="")
        return 1
    print(canonical_json(payload), end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
