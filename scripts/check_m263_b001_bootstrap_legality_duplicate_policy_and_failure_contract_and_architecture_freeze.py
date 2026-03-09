#!/usr/bin/env python3
"""Fail-closed checker for M263-B001 bootstrap legality freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract-v1"
CONTRACT_ID = (
    "objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1"
)
FRONTEND_CLOSURE_CONTRACT_ID = (
    "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"
)
BOOTSTRAP_SEMANTICS_CONTRACT_ID = (
    "objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1"
)
SURFACE_PATH = (
    "frontend.pipeline.semantic_surface."
    "objc_runtime_bootstrap_legality_failure_contract"
)
DUPLICATE_REGISTRATION_POLICY = "fail-closed-by-translation-unit-identity-key"
IMAGE_ORDER_INVARIANT = "strictly-monotonic-positive-registration-order-ordinal"
FAILURE_MODE = "abort-before-user-main-no-partial-registration-commit"
RESTART_LIFECYCLE_MODEL = (
    "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells"
)
REPLAY_ORDER_MODEL = (
    "replay-re-registers-retained-images-in-original-registration-order"
)
IMAGE_LOCAL_INIT_RESET_MODEL = (
    "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay"
)
CATALOG_RETENTION_MODEL = (
    "bootstrap-catalog-retained-across-reset-for-deterministic-replay"
)
RUNTIME_STATE_SNAPSHOT_SYMBOL = "objc3_runtime_copy_registration_state_for_testing"
SUMMARY_OUT = (
    ROOT
    / "tmp"
    / "reports"
    / "m263"
    / "M263-B001"
    / "bootstrap_legality_duplicate_policy_and_failure_contract_summary.json"
)
PROBE_ROOT = (
    ROOT
    / "tmp"
    / "artifacts"
    / "compilation"
    / "objc3c-native"
    / "m263"
    / "b001-bootstrap-legality-duplicate-policy-and-failure-contract"
)
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m263_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze_b001_expectations.md"
)
PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m263"
    / "m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze_packet.md"
)
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = (
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
)
FRONTEND_TYPES = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
)
FRONTEND_ARTIFACTS_H = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
)
FRONTEND_ARTIFACTS_CPP = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
)
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m263_b001_lane_b_readiness.py"
TEST_FILE = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py"
)
EXPLICIT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_legality_explicit.objc3"
)
DEFAULT_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_legality_default.objc3"
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


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck(
            "M263-B001-DOC-EXP-01",
            "# M263 Bootstrap Legality, Duplicate Policy, and Failure Contract Expectations (B001)",
        ),
        SnippetCheck("M263-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M263-B001-DOC-EXP-03", "Issue: `#7222`"),
        SnippetCheck("M263-B001-DOC-EXP-04", f"`{SURFACE_PATH}`"),
        SnippetCheck(
            "M263-B001-DOC-EXP-05",
            f"`{DUPLICATE_REGISTRATION_POLICY}`",
        ),
        SnippetCheck("M263-B001-DOC-EXP-06", "`M263-B002`"),
    ),
    PACKET_DOC: (
        SnippetCheck(
            "M263-B001-DOC-PKT-01",
            "# M263-B001 Bootstrap Legality, Duplicate Policy, and Failure Contract Packet",
        ),
        SnippetCheck("M263-B001-DOC-PKT-02", "Packet: `M263-B001`"),
        SnippetCheck(
            "M263-B001-DOC-PKT-03", "Dependencies: `M263-A002`, `M259-E002`"
        ),
        SnippetCheck("M263-B001-DOC-PKT-04", "Next issue: `M263-B002`"),
        SnippetCheck("M263-B001-DOC-PKT-05", f"`{CONTRACT_ID}`"),
    ),
    NATIVE_DOC: (
        SnippetCheck(
            "M263-B001-NDOC-01",
            "## Bootstrap legality, duplicate policy, and failure contract (M263-B001)",
        ),
        SnippetCheck("M263-B001-NDOC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M263-B001-NDOC-03", f"`{RESTART_LIFECYCLE_MODEL}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck(
            "M263-B001-SPC-01",
            "## M263 bootstrap legality, duplicate policy, and failure contract (B001)",
        ),
        SnippetCheck("M263-B001-SPC-02", f"`{FRONTEND_CLOSURE_CONTRACT_ID}`"),
        SnippetCheck("M263-B001-SPC-03", f"`{BOOTSTRAP_SEMANTICS_CONTRACT_ID}`"),
        SnippetCheck("M263-B001-SPC-04", f"`{IMAGE_ORDER_INVARIANT}`"),
    ),
    METADATA_SPEC: (
        SnippetCheck(
            "M263-B001-META-01",
            "## M263 bootstrap legality metadata anchors (B001)",
        ),
        SnippetCheck("M263-B001-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck(
            "M263-B001-META-03", "`translation_unit_registration_order_ordinal`"
        ),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck(
            "M263-B001-ARCH-01",
            "## M263 bootstrap legality, duplicate policy, and failure contract (B001)",
        ),
        SnippetCheck(
            "M263-B001-ARCH-02",
            "later lowering/runtime work must extend this one legality bridge",
        ),
    ),
    SEMA_CONTRACT: (
        SnippetCheck(
            "M263-B001-SEMA-01", "kObjc3BootstrapLegalityFailureContractId"
        ),
        SnippetCheck(
            "M263-B001-SEMA-02",
            "struct Objc3BootstrapLegalityFailureContractSummary {",
        ),
        SnippetCheck(
            "M263-B001-SEMA-03",
            "bootstrap_legality_failure_contract_summary;",
        ),
    ),
    SEMANTIC_PASSES: (
        SnippetCheck(
            "M263-B001-PASS-01",
            "BuildBootstrapLegalityFailureContractSummaryFromIntegrationSurface(",
        ),
        SnippetCheck(
            "M263-B001-PASS-02",
            "surface.bootstrap_legality_failure_contract_summary =",
        ),
    ),
    FRONTEND_TYPES: (
        SnippetCheck(
            "M263-B001-TYPES-01",
            "struct Objc3RuntimeBootstrapLegalityFailureContractSummary {",
        ),
        SnippetCheck(
            "M263-B001-TYPES-02",
            "registration_descriptor_frontend_closure_contract_ready",
        ),
        SnippetCheck(
            "M263-B001-TYPES-03",
            "inline bool IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(",
        ),
    ),
    FRONTEND_ARTIFACTS_H: (
        SnippetCheck(
            "M263-B001-ARTH-01",
            "runtime_bootstrap_legality_failure_contract_summary;",
        ),
    ),
    FRONTEND_ARTIFACTS_CPP: (
        SnippetCheck(
            "M263-B001-ART-01",
            "BuildRuntimeBootstrapLegalityFailureContractSummary(",
        ),
        SnippetCheck(
            "M263-B001-ART-02",
            "BuildRuntimeBootstrapLegalityFailureContractSummaryJson(",
        ),
        SnippetCheck(
            "M263-B001-ART-03",
            "objc_runtime_bootstrap_legality_failure_contract",
        ),
        SnippetCheck(
            "M263-B001-ART-04",
            "runtime_bootstrap_legality_failure_duplicate_registration_policy",
        ),
    ),
    PACKAGE_JSON: (
        SnippetCheck(
            "M263-B001-PKG-01",
            '"check:objc3c:m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract"',
        ),
        SnippetCheck(
            "M263-B001-PKG-02",
            '"test:tooling:m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract"',
        ),
        SnippetCheck("M263-B001-PKG-03", '"check:objc3c:m263-b001-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M263-B001-RUN-01", "M263-A002 + M254-B002"),
        SnippetCheck(
            "M263-B001-RUN-02",
            "check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py",
        ),
    ),
    TEST_FILE: (
        SnippetCheck("M263-B001-TEST-01", "def test_checker_passes_static("),
        SnippetCheck("M263-B001-TEST-02", "def test_checker_passes_dynamic("),
    ),
    EXPLICIT_FIXTURE: (
        SnippetCheck(
            "M263-B001-FIX-EXP-01",
            "#pragma objc_registration_descriptor(BootstrapLegalityDescriptor)",
        ),
        SnippetCheck(
            "M263-B001-FIX-EXP-02",
            "#pragma objc_image_root(BootstrapLegalityImageRoot)",
        ),
        SnippetCheck(
            "M263-B001-FIX-EXP-03", "module BootstrapLegalityExplicit;"
        ),
    ),
    DEFAULT_FIXTURE: (
        SnippetCheck(
            "M263-B001-FIX-DEF-01", "module BootstrapLegalityDefault;"
        ),
        SnippetCheck(
            "M263-B001-FIX-DEF-02", "#pragma objc_language_version(3)"
        ),
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


def check_static_contract(
    path: Path, snippets: tuple[SnippetCheck, ...]
) -> tuple[int, list[Finding]]:
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
        "M263-B001-NATIVE-EXISTS",
        "native binary is missing",
        failures,
    )
    checks_total += require(
        fixture.exists(),
        display_path(fixture),
        "M263-B001-FIXTURE-EXISTS",
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
    checks_total += require(
        completed.returncode == 0,
        display_path(out_dir),
        "M263-B001-COMPILE",
        "fixture must compile successfully",
        failures,
    )
    checks_total += require(
        manifest_path.exists(),
        display_path(manifest_path),
        "M263-B001-MANIFEST",
        "module manifest is missing",
        failures,
    )
    if failures:
        return checks_total, failures, {
            "fixture": display_path(fixture),
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = (
        pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    )
    flattened = (
        pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    )
    surface = (
        semantic_surface.get("objc_runtime_bootstrap_legality_failure_contract")
        if isinstance(semantic_surface, dict)
        else None
    )
    closure = (
        semantic_surface.get("objc_runtime_registration_descriptor_frontend_closure")
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
        "M263-B001-SURFACE",
        "bootstrap legality surface missing",
        failures,
    )
    checks_total += require(
        isinstance(flattened, dict),
        display_path(manifest_path),
        "M263-B001-FLAT",
        "flattened summary missing",
        failures,
    )
    checks_total += require(
        isinstance(closure, dict),
        display_path(manifest_path),
        "M263-B001-CLOSURE",
        "registration descriptor frontend closure missing",
        failures,
    )
    checks_total += require(
        isinstance(bootstrap_semantics, dict),
        display_path(manifest_path),
        "M263-B001-BOOTSTRAP",
        "bootstrap semantics surface missing",
        failures,
    )
    if failures:
        return checks_total, failures, {"fixture": display_path(fixture)}

    def require_surface(container: dict[str, Any], artifact: str) -> None:
        nonlocal checks_total
        checks_total += require(
            container.get("contract_id") == CONTRACT_ID,
            artifact,
            "M263-B001-CONTRACT",
            "contract id mismatch",
            failures,
        )
        checks_total += require(
            container.get("registration_descriptor_frontend_closure_contract_id")
            == FRONTEND_CLOSURE_CONTRACT_ID,
            artifact,
            "M263-B001-FRONTEND-UPSTREAM",
            "frontend closure contract mismatch",
            failures,
        )
        checks_total += require(
            container.get("bootstrap_semantics_contract_id")
            == BOOTSTRAP_SEMANTICS_CONTRACT_ID,
            artifact,
            "M263-B001-BOOTSTRAP-UPSTREAM",
            "bootstrap semantics contract mismatch",
            failures,
        )
        checks_total += require(
            container.get("frontend_surface_path") == SURFACE_PATH,
            artifact,
            "M263-B001-SURFACE-PATH",
            "surface path mismatch",
            failures,
        )
        checks_total += require(
            container.get("duplicate_registration_policy")
            == DUPLICATE_REGISTRATION_POLICY,
            artifact,
            "M263-B001-DUPLICATE",
            "duplicate-registration policy mismatch",
            failures,
        )
        checks_total += require(
            container.get("image_registration_order_invariant")
            == IMAGE_ORDER_INVARIANT,
            artifact,
            "M263-B001-ORDER",
            "image-order invariant mismatch",
            failures,
        )
        checks_total += require(
            container.get("failure_mode") == FAILURE_MODE,
            artifact,
            "M263-B001-FAILURE",
            "failure mode mismatch",
            failures,
        )
        checks_total += require(
            container.get("restart_lifecycle_model") == RESTART_LIFECYCLE_MODEL,
            artifact,
            "M263-B001-RESTART",
            "restart lifecycle model mismatch",
            failures,
        )
        checks_total += require(
            container.get("replay_order_model") == REPLAY_ORDER_MODEL,
            artifact,
            "M263-B001-REPLAY-ORDER",
            "replay order model mismatch",
            failures,
        )
        checks_total += require(
            container.get("image_local_init_reset_model")
            == IMAGE_LOCAL_INIT_RESET_MODEL,
            artifact,
            "M263-B001-IMAGE-LOCAL-RESET",
            "image-local init reset model mismatch",
            failures,
        )
        checks_total += require(
            container.get("catalog_retention_model") == CATALOG_RETENTION_MODEL,
            artifact,
            "M263-B001-CATALOG",
            "catalog retention model mismatch",
            failures,
        )
        checks_total += require(
            container.get("runtime_state_snapshot_symbol")
            == RUNTIME_STATE_SNAPSHOT_SYMBOL,
            artifact,
            "M263-B001-SNAPSHOT",
            "runtime state snapshot symbol mismatch",
            failures,
        )
        checks_total += require(
            container.get("registration_descriptor_identifier")
            == expected_registration_descriptor,
            artifact,
            "M263-B001-REG-ID",
            "registration descriptor identifier mismatch",
            failures,
        )
        checks_total += require(
            container.get("image_root_identifier") == expected_image_root,
            artifact,
            "M263-B001-ROOT-ID",
            "image-root identifier mismatch",
            failures,
        )
        checks_total += require(
            container.get("registration_descriptor_identity_source")
            == expected_registration_source,
            artifact,
            "M263-B001-REG-SOURCE",
            "registration descriptor identity source mismatch",
            failures,
        )
        checks_total += require(
            container.get("image_root_identity_source")
            == expected_image_root_source,
            artifact,
            "M263-B001-ROOT-SOURCE",
            "image-root identity source mismatch",
            failures,
        )
        checks_total += require(
            container.get("translation_unit_registration_order_ordinal") == 1,
            artifact,
            "M263-B001-ORDINAL",
            "translation-unit registration ordinal mismatch",
            failures,
        )
        checks_total += require(
            container.get("ready") is True,
            artifact,
            "M263-B001-READY",
            "surface must be ready",
            failures,
        )
        checks_total += require(
            container.get("fail_closed") is True,
            artifact,
            "M263-B001-FAIL-CLOSED",
            "surface must be fail-closed",
            failures,
        )
        for key, check_id in (
            (
                "registration_descriptor_frontend_closure_contract_ready",
                "M263-B001-CLOSURE-READY",
            ),
            ("bootstrap_semantics_contract_ready", "M263-B001-SEMANTICS-READY"),
            ("semantic_boundary_ready", "M263-B001-SEMA-READY"),
            (
                "duplicate_registration_policy_frozen",
                "M263-B001-DUPLICATE-FROZEN",
            ),
            ("image_order_invariant_frozen", "M263-B001-ORDER-FROZEN"),
            ("bootstrap_rejection_frozen", "M263-B001-REJECTION-FROZEN"),
            ("restart_boundary_frozen", "M263-B001-BOUNDARY-FROZEN"),
            ("semantic_diagnostics_required", "M263-B001-DIAGS"),
            ("ready_for_lowering_and_runtime", "M263-B001-LOWERING-READY"),
        ):
            checks_total += require(
                container.get(key) is True,
                artifact,
                check_id,
                f"{key} must be true",
                failures,
            )
        for key, check_id in (
            (
                "registration_descriptor_frontend_closure_replay_key",
                "M263-B001-CLOSURE-REPLAY",
            ),
            ("bootstrap_semantics_replay_key", "M263-B001-SEMANTICS-REPLAY"),
            ("semantic_boundary_replay_key", "M263-B001-SEMA-REPLAY"),
            ("replay_key", "M263-B001-REPLAY"),
        ):
            checks_total += require(
                bool(container.get(key)),
                artifact,
                check_id,
                f"{key} must be non-empty",
                failures,
            )
        checks_total += require(
            container.get("failure_reason") == "",
            artifact,
            "M263-B001-FAILURE-REASON",
            "failure reason must be empty",
            failures,
        )

    require_surface(surface, display_path(manifest_path))

    checks_total += require(
        surface.get("registration_descriptor_identifier")
        == closure.get("registration_descriptor_identifier"),
        display_path(manifest_path),
        "M263-B001-CLOSURE-CONTINUITY",
        "registration descriptor identifier must flow through from M263-A002",
        failures,
    )
    checks_total += require(
        surface.get("image_root_identifier") == closure.get("image_root_identifier"),
        display_path(manifest_path),
        "M263-B001-ROOT-CONTINUITY",
        "image-root identifier must flow through from M263-A002",
        failures,
    )
    checks_total += require(
        surface.get("translation_unit_registration_order_ordinal")
        == closure.get("translation_unit_registration_order_ordinal"),
        display_path(manifest_path),
        "M263-B001-ORDINAL-CLOSURE",
        "registration ordinal must match M263-A002",
        failures,
    )
    checks_total += require(
        surface.get("duplicate_registration_policy")
        == bootstrap_semantics.get("duplicate_registration_policy"),
        display_path(manifest_path),
        "M263-B001-DUPLICATE-CONTINUITY",
        "duplicate policy must match M254-B002 bootstrap semantics",
        failures,
    )
    checks_total += require(
        surface.get("image_registration_order_invariant")
        == bootstrap_semantics.get("registration_order_ordinal_model"),
        display_path(manifest_path),
        "M263-B001-ORDER-CONTINUITY",
        "image-order invariant must match M254-B002 bootstrap semantics",
        failures,
    )
    checks_total += require(
        surface.get("failure_mode") == bootstrap_semantics.get("failure_mode"),
        display_path(manifest_path),
        "M263-B001-FAILURE-CONTINUITY",
        "failure mode must match M254-B002 bootstrap semantics",
        failures,
    )
    checks_total += require(
        surface.get("runtime_state_snapshot_symbol")
        == bootstrap_semantics.get("runtime_state_snapshot_symbol"),
        display_path(manifest_path),
        "M263-B001-SNAPSHOT-CONTINUITY",
        "state snapshot symbol must match M254-B002 bootstrap semantics",
        failures,
    )

    flat_prefix = "runtime_bootstrap_legality_failure_"
    checks_total += require(
        flattened.get(f"{flat_prefix}contract_id") == CONTRACT_ID,
        display_path(manifest_path),
        "M263-B001-FLAT-CONTRACT",
        "flattened contract id mismatch",
        failures,
    )
    checks_total += require(
        flattened.get(f"{flat_prefix}duplicate_registration_policy")
        == DUPLICATE_REGISTRATION_POLICY,
        display_path(manifest_path),
        "M263-B001-FLAT-DUPLICATE",
        "flattened duplicate-registration policy mismatch",
        failures,
    )
    checks_total += require(
        flattened.get(f"{flat_prefix}image_registration_order_invariant")
        == IMAGE_ORDER_INVARIANT,
        display_path(manifest_path),
        "M263-B001-FLAT-ORDER",
        "flattened image-order invariant mismatch",
        failures,
    )
    checks_total += require(
        flattened.get(f"{flat_prefix}ready_for_lowering_and_runtime") is True,
        display_path(manifest_path),
        "M263-B001-FLAT-READY",
        "flattened lowering/runtime readiness must be true",
        failures,
    )
    checks_total += require(
        bool(flattened.get(f"{flat_prefix}replay_key")),
        display_path(manifest_path),
        "M263-B001-FLAT-REPLAY",
        "flattened replay key must be non-empty",
        failures,
    )

    return checks_total, failures, {
        "fixture": display_path(fixture),
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
        explicit_checks, explicit_failures, explicit_summary = compile_fixture(
            fixture=EXPLICIT_FIXTURE,
            out_dir=PROBE_ROOT / "explicit",
            expected_registration_descriptor="BootstrapLegalityDescriptor",
            expected_image_root="BootstrapLegalityImageRoot",
            expected_registration_source="source-pragma",
            expected_image_root_source="source-pragma",
        )
        default_checks, default_failures, default_summary = compile_fixture(
            fixture=DEFAULT_FIXTURE,
            out_dir=PROBE_ROOT / "default",
            expected_registration_descriptor="BootstrapLegalityDefault_registration_descriptor",
            expected_image_root="BootstrapLegalityDefault_image_root",
            expected_registration_source="module-derived-default",
            expected_image_root_source="module-derived-default",
        )
        checks_total += explicit_checks + default_checks
        failures.extend(explicit_failures)
        failures.extend(default_failures)
        dynamic_summary["explicit"] = explicit_summary
        dynamic_summary["default"] = default_summary

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
        "next_implementation_issue": "M263-B002",
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
