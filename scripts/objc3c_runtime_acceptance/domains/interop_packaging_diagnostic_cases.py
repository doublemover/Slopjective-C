"""Interop packaging diagnostic acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from ..case_result import CaseResult
from ..native_build import (
    ROOT,
    compile_fixture_expect_failure,
    compile_fixture_outputs_with_args,
    compile_fixture_with_args,
)
from ..probes import compile_probe, parse_json_output, parse_key_value_output, run_probe
from ..assertions import expect
from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)
_EXPORTED_CASE_NAMES = [
    "check_import_version_feature_claim_diagnostics_case",
]

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_import_version_feature_claim_diagnostics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "import-version-feature-claim-diagnostics"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )

    advanced_feature_gate = json.loads(
        (
            provider_compile_dir / "module.objc3-advanced-feature-gate.json"
        ).read_text(encoding="utf-8")
    )
    release_candidate_matrix = json.loads(
        (
            provider_compile_dir / "module.objc3-release-candidate-matrix.json"
        ).read_text(encoding="utf-8")
    )
    expect(
        advanced_feature_gate.get("contract_id")
        == "objc3c.tooling.integrated.advanced.feature.gate.v1"
        and advanced_feature_gate.get("ready") is True,
        "expected provider compile to publish a ready advanced feature-gate artifact",
    )
    expect(
        release_candidate_matrix.get("contract_id")
        == "objc3c.tooling.release.candidate.execution.matrix.v1"
        and release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json"
        and release_candidate_matrix.get("ready") is True,
        "expected provider compile to publish a ready release-candidate matrix tied to the advanced feature-gate artifact",
    )

    corrupted_import_surface_path = case_dir / "provider-corrupted.runtime-import-surface.json"
    corrupted_import_surface = json.loads(
        (
            provider_compile_dir / "module.runtime-import-surface.json"
        ).read_text(encoding="utf-8")
    )
    corrupted_import_surface[
        "objc_interop_header_module_and_bridge_generation"
    ]["contract_id"] = "objc3c.interop.header.module.and.bridge.generation.v999"
    corrupted_import_surface_path.write_text(
        json.dumps(corrupted_import_surface, indent=2) + "\n", encoding="utf-8"
    )

    import_version_negative = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "negative-import-version-drift",
        expected_snippets=[
            "unexpected Part 11 header/module/bridge generation contract id in import surface"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(corrupted_import_surface_path),
        ],
        allow_missing_structured_diagnostics=True,
    )

    return CaseResult(
        case_id="import-version-feature-claim-diagnostics",
        probe="compile-feature-gate-sidecars-and-fail-closed-import-version-diagnostic",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "advanced_feature_gate_profiles": advanced_feature_gate.get(
                "targeted_profile_ids"
            ),
            "release_candidate_matrix_rows": len(
                release_candidate_matrix.get("matrix_rows", [])
            ),
            "import_version_negative_returncode": import_version_negative[
                "returncode"
            ],
            "import_version_negative_diagnostics_path": import_version_negative[
                "diagnostics_path"
            ],
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
