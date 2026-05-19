"""Error bridging diagnostic surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.errors_surface_support import (
    authoritative_case_ids,
)

from ..runtime_contract_errors import RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID


def build_runtime_bridging_filter_unwind_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.bridge.legality.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/sema/objc3_sema_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"bridging-filter-unwind-compatibility-diagnostics"},
        ),
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/bridge_legality_positive.objc3",
            "tests/tooling/fixtures/native/bridge_legality_native_fail_closed.objc3",
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_nserror_bad_return_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_throws_conflict_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_marker_conflict_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_error_type_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_missing_mapping_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_mapping_signature_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_status_return_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


__all__ = ["build_runtime_bridging_filter_unwind_diagnostics_surface"]
