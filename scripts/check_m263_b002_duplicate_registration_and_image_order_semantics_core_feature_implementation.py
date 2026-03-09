#!/usr/bin/env python3
"""Checker for M263-B002 duplicate-registration and image-order semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-b002-duplicate-registration-and-image-order-semantics-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1"
FAILURE_CONTRACT_ID = (
    "objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1"
)
FRONTEND_CLOSURE_CONTRACT_ID = (
    "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"
)
BOOTSTRAP_SEMANTICS_CONTRACT_ID = (
    "objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1"
)
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics"
DUPLICATE_REGISTRATION_POLICY = "fail-closed-by-translation-unit-identity-key"
IMAGE_ORDER_INVARIANT = "strictly-monotonic-positive-registration-order-ordinal"
CROSS_IMAGE_LEGALITY_MODEL = (
    "translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality"
)
SEMANTIC_DIAGNOSTIC_MODEL = "fail-closed-bootstrap-legality-before-runtime-handoff"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"
SUMMARY_OUT = (
    ROOT
    / "tmp"
    / "reports"
    / "m263"
    / "M263-B002"
    / "duplicate_registration_and_image_order_semantics_summary.json"
)
PROBE_ROOT = (
    ROOT
    / "tmp"
    / "artifacts"
    / "compilation"
    / "objc3c-native"
    / "m263"
    / "b002-duplicate-registration-and-image-order-semantics"
)
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m263_duplicate_registration_and_image_order_semantics_core_feature_implementation_b002_expectations.md"
)
PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m263"
    / "m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation_packet.md"
)
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m263_b002_lane_b_readiness.py"
TEST_FILE = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py"
)
EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_legality_explicit.objc3"
EXPLICIT_PEER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_legality_explicit_peer.objc3"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_legality_default.objc3"


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
        SnippetCheck(
            "M263-B002-DOC-EXP-01",
            "# M263 Duplicate-Registration and Image-Order Semantics Expectations (B002)",
        ),
        SnippetCheck("M263-B002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M263-B002-DOC-EXP-03", "Issue: `#7223`"),
        SnippetCheck("M263-B002-DOC-EXP-04", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-B002-DOC-EXP-05", "`translation_unit_identity_key`"),
        SnippetCheck("M263-B002-DOC-EXP-06", "`M263-B003`"),
    ),
    PACKET_DOC: (
        SnippetCheck(
            "M263-B002-DOC-PKT-01",
            "# M263-B002 Duplicate-Registration and Image-Order Semantics Packet",
        ),
        SnippetCheck("M263-B002-DOC-PKT-02", "Packet: `M263-B002`"),
        SnippetCheck(
            "M263-B002-DOC-PKT-03",
            "Dependencies: `M263-B001`, `M263-A002`, `M254-B002`",
        ),
        SnippetCheck("M263-B002-DOC-PKT-04", "Next issue: `M263-B003`"),
        SnippetCheck("M263-B002-DOC-PKT-05", f"`{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck(
            "M263-B002-NDOC-01",
            "## Duplicate-registration and image-order semantics (M263-B002)",
        ),
        SnippetCheck("M263-B002-NDOC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-B002-NDOC-03", "`translation_unit_identity_key`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck(
            "M263-B002-SPC-01",
            "## M263 duplicate-registration and image-order semantics (B002)",
        ),
        SnippetCheck("M263-B002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck(
            "M263-B002-SPC-03", f"`{CROSS_IMAGE_LEGALITY_MODEL}`"
        ),
    ),
    METADATA_SPEC: (
        SnippetCheck(
            "M263-B002-META-01",
            "## M263 duplicate-registration and image-order semantics metadata anchors (B002)",
        ),
        SnippetCheck("M263-B002-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck(
            "M263-B002-META-03", "`duplicate_registration_semantics_landed`"
        ),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck(
            "M263-B002-ARCH-01",
            "## M263 duplicate-registration and image-order semantics (B002)",
        ),
        SnippetCheck(
            "M263-B002-ARCH-02",
            "check:objc3c:m263-b002-duplicate-registration-and-image-order-semantics",
        ),
    ),
    SEMA_CONTRACT: (
        SnippetCheck(
            "M263-B002-SEMA-01", "kObjc3BootstrapLegalitySemanticsContractId"
        ),
        SnippetCheck(
            "M263-B002-SEMA-02",
            "struct Objc3BootstrapLegalitySemanticsSummary {",
        ),
        SnippetCheck(
            "M263-B002-SEMA-03", "bootstrap_legality_semantics_summary;"
        ),
    ),
    SEMANTIC_PASSES: (
        SnippetCheck(
            "M263-B002-PASS-01",
            "BuildBootstrapLegalitySemanticsSummaryFromIntegrationSurface(",
        ),
        SnippetCheck(
            "M263-B002-PASS-02",
            "surface.bootstrap_legality_semantics_summary =",
        ),
    ),
    FRONTEND_TYPES: (
        SnippetCheck(
            "M263-B002-TYPES-01",
            "struct Objc3RuntimeBootstrapLegalitySemanticsSummary {",
        ),
        SnippetCheck(
            "M263-B002-TYPES-02", "translation_unit_identity_key"
        ),
        SnippetCheck(
            "M263-B002-TYPES-03", "duplicate_registration_semantics_landed"
        ),
    ),
    FRONTEND_ARTIFACTS_H: (
        SnippetCheck(
            "M263-B002-ARTH-01", "runtime_bootstrap_legality_semantics_summary;"
        ),
    ),
    FRONTEND_ARTIFACTS_CPP: (
        SnippetCheck(
            "M263-B002-ART-01", "BuildRuntimeBootstrapLegalitySemanticsSummaryJson("
        ),
        SnippetCheck(
            "M263-B002-ART-02", "objc_runtime_bootstrap_legality_semantics"
        ),
        SnippetCheck(
            "M263-B002-ART-03",
            "bundle.runtime_bootstrap_legality_semantics_summary =",
        ),
        SnippetCheck(
            "M263-B002-ART-04",
            "runtime_metadata_archive_static_link_translation_unit_identity_key =",
        ),
        SnippetCheck(
            "M263-B002-ART-05", "BuildObjc3TranslationUnitIdentityKey("
        ),
    ),
    PACKAGE_JSON: (
        SnippetCheck(
            "M263-B002-PKG-01",
            '"check:objc3c:m263-b002-duplicate-registration-and-image-order-semantics"',
        ),
        SnippetCheck(
            "M263-B002-PKG-02",
            '"test:tooling:m263-b002-duplicate-registration-and-image-order-semantics"',
        ),
        SnippetCheck(
            "M263-B002-PKG-03", '"check:objc3c:m263-b002-lane-b-readiness"'
        ),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M263-B002-RUN-01", "M263-B001 + M263-A002 + M254-B002"),
        SnippetCheck(
            "M263-B002-RUN-02",
            "check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py",
        ),
    ),
    TEST_FILE: (
        SnippetCheck("M263-B002-TEST-01", "def test_checker_passes_static("),
        SnippetCheck("M263-B002-TEST-02", "def test_checker_passes_dynamic("),
    ),
    EXPLICIT_FIXTURE: (
        SnippetCheck(
            "M263-B002-FIX-EXP-01", "#pragma objc_registration_descriptor(BootstrapLegalityDescriptor)"
        ),
        SnippetCheck(
            "M263-B002-FIX-EXP-02", "#pragma objc_image_root(BootstrapLegalityImageRoot)"
        ),
    ),
    EXPLICIT_PEER_FIXTURE: (
        SnippetCheck(
            "M263-B002-FIX-PEER-01", "#pragma objc_registration_descriptor(BootstrapLegalityDescriptor)"
        ),
        SnippetCheck(
            "M263-B002-FIX-PEER-02", "#pragma objc_image_root(BootstrapLegalityImageRoot)"
        ),
    ),
    DEFAULT_FIXTURE: (
        SnippetCheck(
            "M263-B002-FIX-DEF-01", "module BootstrapLegalityDefault;"
        ),
        SnippetCheck("M263-B002-FIX-DEF-02", "#pragma objc_language_version(3)"),
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


def require(
    condition: bool,
    artifact: str,
    check_id: str,
    detail: str,
    failures: list[Finding],
) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        failures.append(
            Finding(
                display_path(path),
                "STATIC-EXISTS",
                f"missing required artifact: {display_path(path)}",
            )
        )
        return checks_total, failures
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
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
    expected_registration_descriptor: str,
    expected_image_root: str,
    expected_registration_source: str,
    expected_image_root_source: str,
) -> tuple[int, list[Finding], dict[str, object]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(
        NATIVE_EXE.exists(),
        display_path(NATIVE_EXE),
        "M263-B002-NATIVE-EXISTS",
        "native binary is missing",
        failures,
    )
    checks_total += require(
        fixture.exists(),
        display_path(fixture),
        "M263-B002-FIXTURE-EXISTS",
        "fixture is missing",
        failures,
    )
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
    registration_descriptor_path = out_dir / "module.runtime-registration-descriptor.json"

    checks_total += require(
        completed.returncode == 0,
        display_path(out_dir),
        "M263-B002-COMPILE",
        "fixture must compile successfully",
        failures,
    )
    for path, check_id, detail in (
        (manifest_path, "M263-B002-MANIFEST", "module manifest is missing"),
        (
            registration_manifest_path,
            "M263-B002-REG-MANIFEST",
            "runtime registration manifest is missing",
        ),
        (
            registration_descriptor_path,
            "M263-B002-REG-DESCRIPTOR",
            "runtime registration descriptor is missing",
        ),
    ):
        checks_total += require(path.exists(), display_path(path), check_id, detail, failures)

    if failures:
        return checks_total, failures, {
            "fixture": display_path(fixture),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    manifest = load_json(manifest_path)
    registration_manifest = load_json(registration_manifest_path)
    registration_descriptor = load_json(registration_descriptor_path)

    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    flattened = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    surface = (
        semantic_surface.get("objc_runtime_bootstrap_legality_semantics")
        if isinstance(semantic_surface, dict)
        else None
    )
    closure = (
        semantic_surface.get("objc_runtime_registration_descriptor_frontend_closure")
        if isinstance(semantic_surface, dict)
        else None
    )
    failure_contract = (
        semantic_surface.get("objc_runtime_bootstrap_legality_failure_contract")
        if isinstance(semantic_surface, dict)
        else None
    )
    bootstrap_semantics = (
        semantic_surface.get("objc_runtime_startup_bootstrap_semantics")
        if isinstance(semantic_surface, dict)
        else None
    )

    checks_total += require(
        isinstance(surface, dict),
        display_path(manifest_path),
        "M263-B002-SURFACE",
        "bootstrap legality semantics surface missing",
        failures,
    )
    checks_total += require(
        isinstance(flattened, dict),
        display_path(manifest_path),
        "M263-B002-FLAT",
        "flattened summary missing",
        failures,
    )
    checks_total += require(
        isinstance(closure, dict),
        display_path(manifest_path),
        "M263-B002-CLOSURE",
        "registration descriptor frontend closure missing",
        failures,
    )
    checks_total += require(
        isinstance(failure_contract, dict),
        display_path(manifest_path),
        "M263-B002-FAILURE-CONTRACT",
        "bootstrap legality failure contract missing",
        failures,
    )
    checks_total += require(
        isinstance(bootstrap_semantics, dict),
        display_path(manifest_path),
        "M263-B002-BOOTSTRAP",
        "bootstrap semantics surface missing",
        failures,
    )
    if failures:
        return checks_total, failures, {"fixture": display_path(fixture)}

    def check_expected_container(
        container: dict[str, Any],
        artifact: Path | str,
        prefix: str = "",
        *,
        require_ready_field: bool = True,
    ) -> None:
        nonlocal checks_total
        label = display_path(artifact) if isinstance(artifact, Path) else artifact
        key = lambda name: f"{prefix}{name}" if prefix else name
        checks_total += require(container.get(key("contract_id")) == CONTRACT_ID, label, "M263-B002-CONTRACT", "contract id mismatch", failures)
        checks_total += require(
            container.get(key("bootstrap_legality_failure_contract_id")) == FAILURE_CONTRACT_ID,
            label,
            "M263-B002-UPSTREAM-FAILURE",
            "bootstrap legality failure contract id mismatch",
            failures,
        )
        checks_total += require(
            container.get(key("registration_descriptor_frontend_closure_contract_id")) == FRONTEND_CLOSURE_CONTRACT_ID,
            label,
            "M263-B002-UPSTREAM-CLOSURE",
            "registration descriptor frontend closure contract id mismatch",
            failures,
        )
        checks_total += require(
            container.get(key("bootstrap_semantics_contract_id")) == BOOTSTRAP_SEMANTICS_CONTRACT_ID,
            label,
            "M263-B002-UPSTREAM-BOOTSTRAP",
            "bootstrap semantics contract id mismatch",
            failures,
        )
        checks_total += require(container.get(key("frontend_surface_path")) == SURFACE_PATH, label, "M263-B002-SURFACE-PATH", "surface path mismatch", failures)
        checks_total += require(container.get(key("duplicate_registration_policy")) == DUPLICATE_REGISTRATION_POLICY, label, "M263-B002-DUPLICATE", "duplicate policy mismatch", failures)
        checks_total += require(container.get(key("image_registration_order_invariant")) == IMAGE_ORDER_INVARIANT, label, "M263-B002-ORDER", "image order invariant mismatch", failures)
        checks_total += require(container.get(key("cross_image_legality_model")) == CROSS_IMAGE_LEGALITY_MODEL, label, "M263-B002-CROSS-IMAGE", "cross-image legality model mismatch", failures)
        checks_total += require(container.get(key("semantic_diagnostic_model")) == SEMANTIC_DIAGNOSTIC_MODEL, label, "M263-B002-DIAGNOSTIC-MODEL", "semantic diagnostic model mismatch", failures)
        checks_total += require(container.get(key("translation_unit_identity_model")) == TRANSLATION_UNIT_IDENTITY_MODEL, label, "M263-B002-IDENTITY-MODEL", "identity model mismatch", failures)
        checks_total += require(bool(container.get(key("translation_unit_identity_key"))), label, "M263-B002-IDENTITY-KEY", "identity key must be non-empty", failures)
        checks_total += require(container.get(key("registration_descriptor_identifier")) == expected_registration_descriptor, label, "M263-B002-REG-ID", "registration descriptor identifier mismatch", failures)
        checks_total += require(container.get(key("image_root_identifier")) == expected_image_root, label, "M263-B002-ROOT-ID", "image root identifier mismatch", failures)
        checks_total += require(container.get(key("registration_descriptor_identity_source")) == expected_registration_source, label, "M263-B002-REG-SOURCE", "registration descriptor identity source mismatch", failures)
        checks_total += require(container.get(key("image_root_identity_source")) == expected_image_root_source, label, "M263-B002-ROOT-SOURCE", "image root identity source mismatch", failures)
        checks_total += require(container.get(key("translation_unit_registration_order_ordinal")) == 1, label, "M263-B002-ORDINAL", "registration order ordinal mismatch", failures)
        if require_ready_field:
            checks_total += require(
                container.get(key("ready")) is True,
                label,
                "M263-B002-READY",
                "ready must be true",
                failures,
            )
        for field_name, check_id in (
            ("fail_closed", "M263-B002-FAIL-CLOSED"),
            ("semantic_boundary_ready", "M263-B002-SEMA-READY"),
            (
                "bootstrap_legality_failure_contract_ready",
                "M263-B002-FAILURE-READY",
            ),
            (
                "registration_descriptor_frontend_closure_contract_ready",
                "M263-B002-CLOSURE-READY",
            ),
            (
                "bootstrap_semantics_contract_ready",
                "M263-B002-BOOTSTRAP-READY",
            ),
            (
                "duplicate_registration_semantics_landed",
                "M263-B002-DUPLICATE-LANDED",
            ),
            ("image_order_semantics_landed", "M263-B002-ORDER-LANDED"),
            (
                "cross_image_legality_semantics_landed",
                "M263-B002-CROSS-IMAGE-LANDED",
            ),
            (
                "semantic_diagnostics_landed",
                "M263-B002-DIAGNOSTICS-LANDED",
            ),
            (
                "ready_for_lowering_and_runtime",
                "M263-B002-LOWERING-READY",
            ),
        ):
            checks_total += require(
                container.get(key(field_name)) is True,
                label,
                check_id,
                f"{field_name} must be true",
                failures,
            )
        for field_name, check_id in (
            ("semantic_boundary_replay_key", "M263-B002-SEMA-REPLAY"),
            (
                "bootstrap_legality_failure_contract_replay_key",
                "M263-B002-FAILURE-REPLAY",
            ),
            (
                "registration_descriptor_frontend_closure_replay_key",
                "M263-B002-CLOSURE-REPLAY",
            ),
            ("bootstrap_semantics_replay_key", "M263-B002-BOOTSTRAP-REPLAY"),
            ("replay_key", "M263-B002-REPLAY"),
        ):
            checks_total += require(
                bool(container.get(key(field_name))),
                label,
                check_id,
                f"{field_name} must be non-empty",
                failures,
            )
        checks_total += require(container.get(key("failure_reason")) == "", label, "M263-B002-FAILURE-REASON", "failure reason must be empty", failures)

    check_expected_container(surface, manifest_path)
    check_expected_container(
        flattened,
        manifest_path,
        "runtime_bootstrap_legality_semantics_",
        require_ready_field=False,
    )

    surface_identity_key = surface.get("translation_unit_identity_key")
    manifest_identity_key = registration_manifest.get("translation_unit_identity_key")
    descriptor_identity_key = registration_descriptor.get("translation_unit_identity_key")
    checks_total += require(
        surface_identity_key == manifest_identity_key,
        display_path(manifest_path),
        "M263-B002-MANIFEST-IDENTITY-CONTINUITY",
        "surface identity key must match registration manifest",
        failures,
    )
    checks_total += require(
        surface_identity_key == descriptor_identity_key,
        display_path(manifest_path),
        "M263-B002-DESCRIPTOR-IDENTITY-CONTINUITY",
        "surface identity key must match registration descriptor",
        failures,
    )
    checks_total += require(
        surface.get("translation_unit_identity_model")
        == registration_manifest.get("translation_unit_identity_model"),
        display_path(manifest_path),
        "M263-B002-MANIFEST-IDENTITY-MODEL",
        "surface identity model must match registration manifest",
        failures,
    )
    checks_total += require(
        surface.get("translation_unit_identity_model")
        == registration_descriptor.get("translation_unit_identity_model"),
        display_path(manifest_path),
        "M263-B002-DESCRIPTOR-IDENTITY-MODEL",
        "surface identity model must match registration descriptor",
        failures,
    )
    for field_name, check_id in (
        ("registration_descriptor_identifier", "M263-B002-CLOSURE-REG-ID"),
        ("image_root_identifier", "M263-B002-CLOSURE-ROOT-ID"),
        (
            "registration_descriptor_identity_source",
            "M263-B002-CLOSURE-REG-SOURCE",
        ),
        ("image_root_identity_source", "M263-B002-CLOSURE-ROOT-SOURCE"),
        (
            "translation_unit_registration_order_ordinal",
            "M263-B002-CLOSURE-ORDINAL",
        ),
    ):
        checks_total += require(
            surface.get(field_name) == closure.get(field_name),
            display_path(manifest_path),
            check_id,
            f"{field_name} must flow through from M263-A002",
            failures,
        )
    checks_total += require(
        surface.get("duplicate_registration_policy")
        == failure_contract.get("duplicate_registration_policy"),
        display_path(manifest_path),
        "M263-B002-FAILURE-DUPLICATE-CONTINUITY",
        "duplicate policy must match M263-B001 failure contract",
        failures,
    )
    checks_total += require(
        surface.get("image_registration_order_invariant")
        == failure_contract.get("image_registration_order_invariant"),
        display_path(manifest_path),
        "M263-B002-FAILURE-ORDER-CONTINUITY",
        "image-order invariant must match M263-B001 failure contract",
        failures,
    )
    checks_total += require(
        surface.get("duplicate_registration_policy")
        == bootstrap_semantics.get("duplicate_registration_policy"),
        display_path(manifest_path),
        "M263-B002-BOOTSTRAP-DUPLICATE-CONTINUITY",
        "duplicate policy must match M254-B002 semantics",
        failures,
    )
    checks_total += require(
        surface.get("image_registration_order_invariant")
        == bootstrap_semantics.get("registration_order_ordinal_model"),
        display_path(manifest_path),
        "M263-B002-BOOTSTRAP-ORDER-CONTINUITY",
        "image-order invariant must match M254-B002 semantics",
        failures,
    )
    checks_total += require(
        surface.get("translation_unit_registration_order_ordinal")
        == registration_manifest.get("translation_unit_registration_order_ordinal"),
        display_path(manifest_path),
        "M263-B002-MANIFEST-ORDINAL-CONTINUITY",
        "surface order ordinal must match registration manifest",
        failures,
    )
    checks_total += require(
        surface.get("translation_unit_registration_order_ordinal")
        == registration_descriptor.get("translation_unit_registration_order_ordinal"),
        display_path(manifest_path),
        "M263-B002-DESCRIPTOR-ORDINAL-CONTINUITY",
        "surface order ordinal must match registration descriptor",
        failures,
    )

    return checks_total, failures, {
        "fixture": display_path(fixture),
        "translation_unit_identity_key": surface_identity_key,
        "translation_unit_identity_model": surface.get(
            "translation_unit_identity_model"
        ),
        "registration_descriptor_identifier": surface.get(
            "registration_descriptor_identifier"
        ),
        "image_root_identifier": surface.get("image_root_identifier"),
        "registration_descriptor_identity_source": surface.get(
            "registration_descriptor_identity_source"
        ),
        "image_root_identity_source": surface.get("image_root_identity_source"),
        "translation_unit_registration_order_ordinal": surface.get(
            "translation_unit_registration_order_ordinal"
        ),
        "replay_key": surface.get("replay_key"),
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
        probe_specs = (
            (
                "explicit_primary",
                EXPLICIT_FIXTURE,
                PROBE_ROOT / "explicit_primary",
                "BootstrapLegalityDescriptor",
                "BootstrapLegalityImageRoot",
                "source-pragma",
                "source-pragma",
            ),
            (
                "explicit_rebuild",
                EXPLICIT_FIXTURE,
                PROBE_ROOT / "explicit_rebuild",
                "BootstrapLegalityDescriptor",
                "BootstrapLegalityImageRoot",
                "source-pragma",
                "source-pragma",
            ),
            (
                "explicit_peer",
                EXPLICIT_PEER_FIXTURE,
                PROBE_ROOT / "explicit_peer",
                "BootstrapLegalityDescriptor",
                "BootstrapLegalityImageRoot",
                "source-pragma",
                "source-pragma",
            ),
            (
                "default",
                DEFAULT_FIXTURE,
                PROBE_ROOT / "default",
                "BootstrapLegalityDefault_registration_descriptor",
                "BootstrapLegalityDefault_image_root",
                "module-derived-default",
                "module-derived-default",
            ),
        )
        probe_results: dict[str, dict[str, object]] = {}
        for key, fixture, out_dir, reg_id, root_id, reg_source, root_source in probe_specs:
            probe_checks, probe_failures, probe_summary = compile_fixture(
                fixture=fixture,
                out_dir=out_dir,
                expected_registration_descriptor=reg_id,
                expected_image_root=root_id,
                expected_registration_source=reg_source,
                expected_image_root_source=root_source,
            )
            checks_total += probe_checks
            failures.extend(probe_failures)
            probe_results[key] = probe_summary
        dynamic_summary.update(probe_results)

        primary = probe_results.get("explicit_primary", {})
        rebuild = probe_results.get("explicit_rebuild", {})
        peer = probe_results.get("explicit_peer", {})
        default = probe_results.get("default", {})
        comparisons = (
            (
                primary.get("translation_unit_identity_key")
                == rebuild.get("translation_unit_identity_key"),
                "M263-B002-REBUILD-STABLE",
                "same translation unit must preserve the same identity key",
            ),
            (
                primary.get("translation_unit_identity_key")
                != peer.get("translation_unit_identity_key"),
                "M263-B002-PEER-DIFFERENCE",
                "peer translation unit must produce a distinct identity key",
            ),
            (
                primary.get("registration_descriptor_identifier")
                == peer.get("registration_descriptor_identifier"),
                "M263-B002-PEER-REG-ID",
                "peer translation unit must preserve visible registration descriptor identifier",
            ),
            (
                primary.get("image_root_identifier") == peer.get("image_root_identifier"),
                "M263-B002-PEER-ROOT-ID",
                "peer translation unit must preserve visible image root identifier",
            ),
            (
                primary.get("translation_unit_identity_key")
                != default.get("translation_unit_identity_key"),
                "M263-B002-DEFAULT-DIFFERENCE",
                "default translation unit should not share the explicit identity key",
            ),
        )
        for condition, check_id, detail in comparisons:
            checks_total += require(condition, "dynamic-probes", check_id, detail, failures)

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
        "next_implementation_issue": "M263-B003",
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
            print(
                f"[fail] {failure.check_id} {failure.artifact}: {failure.detail}",
                file=sys.stderr,
            )
        print(f"[error] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
